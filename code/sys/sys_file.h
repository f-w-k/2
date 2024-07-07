// io utilities,
// - rlyeh, public domain

#if !CODE

#define DIR_SEP '/'
#define DIR_MAX MAX_PATH

API char*  file_read(const char *pathfile, int *size); // do not free() after use
API int    file_write(const char *pathfile, const void *blob, int len);
API bool   file_append( const char *file, const void *ptr, int len );
//API char** file_list(const char *pathfile, const char *glob, int *count); // must free() after use

API int    is_file(const char *pathfile);
API int    is_folder(const char *pathfile);

API const char *file_path( const char *pathfile ); // c:/prj/file.txt > c:/prj/
API const char *file_name( const char *pathfile ); // c:/prj/file.txt > file.txt
API const char *file_ext ( const char *pathfile ); // c:/prj/file.txt > .txt

API const char *file_norm( const char *pathfile ); // converts to native slashes and quotes

API FILE * file_handle( const char *pathfile );

#else

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <sys/stat.h>

static
const char *file_locate_recursively(const char *dir, const char *name) { // file.txt > ./art/dir/file.txt
    // @fixme: optimize me
    int count;
    char **list = SDL_GlobDirectory(dir, "*", SDL_GLOB_CASEINSENSITIVE, &count);
    if( list ) {
        for( int i = 0; i < count; ++i ) {
            char built[DIR_MAX];
            snprintf(built, DIR_MAX, "%s/%s", dir, list[i]);

            if( is_file(built) ) {
                const char *match = strendi(name, list[i]);
                if( match ? match == name || (match > name && match[-1] == '/') : false ) {
                    return SDL_free(list), va("%s", built);
                }
            } else if( list[i][0] != '.' ) {
                const char *result = file_locate_recursively(built, name);
                if( result ) {
                    return SDL_free(list), va("%s", result);
                }
            }
        }
        SDL_free(list);
    }

    return NULL;
}
static
const char *file_locate(const char *pathfile) { // file.txt > ./art/dir/file.txt
    if( is_file(pathfile) ) {
        return pathfile;
    }
    char norm[DIR_MAX]; snprintf(norm, DIR_MAX, "%s", pathfile); 
    while(strchr(norm, '\\')) *strchr(norm, '\\') = '/'; // replace(norm, '\\', '/');
    return file_locate_recursively(".", norm);
}

const char *file_name( const char *pathfile ) { // c:/prj/file.txt > file.txt
    const char *a = strrchr(pathfile, '/');  a += !!a;
    const char *b = strrchr(pathfile, '\\'); b += !!b;
    return a > b ? a : b > a ? b : pathfile;
}
const char *file_path( const char *pathfile ) { // c:/prj/file.txt > c:/prj/
    const char *name = file_name(pathfile);
    return va("%.*s", (int)(strlen(pathfile) - strlen(name)), pathfile );
}
const char *file_ext(const char *pathfile) { // c:/prj/file.txt > .txt
    const char *name = file_name(pathfile), *dot = strchr(name, '.');
    return dot ? dot : (const char*)va("");
}

FILE *file_handle(const char *pathfile) {
    const char *solved = file_locate(pathfile);
    if( !solved ) return NULL;

    // process/cook it if needed
    pathfile = cook( solved );
    return fopen( pathfile, "rb" );
}

char *file_read(const char *pathfile, int *size) { // do not free() after use
    FILE *fp = file_handle(pathfile);
    if( !fp ) {
#if HAS_EMBED
        // try embedded resources as fallback
        for( int i = 0; i < countof(resources); ++i ) {
            if( resources[i].data && strendi(pathfile, resources[i].name) ) {
                printf("found embedded resource: %s (%d bytes)\n", resources[i].name, resources[i].size);
                if( size ) *size = resources[i].size;
                return (char*)resources[i].data; // @fixme: remove cast. should return a (char*)COPY of such data, and must re-use the slot[] logics below
            }
        }
#endif
        printf(ANSI_RED("cannot find file (%s)\n"), pathfile);
        return NULL;
    }

    fseek(fp, 0L, SEEK_END);
    size_t len = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    if(size) *size = (int)len;

    enum { NUM_SLOTS = 1024 }; // @fixme: use block of mem similar to how va() works
    static __thread char* slot[NUM_SLOTS] = {0};
    static __thread int index = 0; index = (index+1) % NUM_SLOTS;

    char *bin = (slot[index] = realloc(slot[index], len+1));

    if( fread(bin, 1, len, fp) != len ) return fclose(fp), NULL;

    return fclose(fp), bin[len] = '\0', bin;
}
int file_write(const char *pathfile, const void *blob, int len) {
    int ok = 0;
    FILE *fp = fopen(pathfile, "wb");
    if( fp ) {
        ok = fwrite(blob, len, 1, fp) == 1;
        fclose(fp);
    }
    return ok;
}
bool file_append(const char *name, const void *ptr, int len) {
    bool ok = 0;
    for( FILE *fp = name && ptr && len >= 0 ? fopen(name, "a+b") : NULL; fp; fclose(fp), fp = 0) {
        ok = fwrite(ptr, len,1, fp) == 1;
    }
    return ok;
}

int is_file( const char *pathfile ) {
    struct stat st;
    return stat(pathfile, &st) >= 0 ? S_IFDIR != ( st.st_mode & S_IFMT ) : 0;
}
int is_folder( const char *pathfile ) {
    // @fixme: win32+tcc wont like ending slashes in stat()
    struct stat st;
    return stat(pathfile, &st) >= 0 ? S_IFDIR == ( st.st_mode & S_IFMT ) : 0;
}

const char *file_norm(const char *pathfile) {
    return ifdef(win32,
            strswap(strswap(va("%s", pathfile), "/", "\\"), "'", "\""),
            strswap(strswap(va("%s", pathfile), "\\", "/"), "\"", "'"));
}

#endif
