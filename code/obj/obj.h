// built-ins

#if !CODE
typedef enum OBJTYPE_builtins {
    OBJTYPE_obj    =  0,
    OBJTYPE_entity =  1,
    OBJTYPE_vec2   =  2,
    OBJTYPE_vec3   =  3,
    OBJTYPE_vec4   =  4,
    OBJTYPE_quat   =  5,
    OBJTYPE_mat33  =  6,
    OBJTYPE_mat34  =  7,
    OBJTYPE_mat44  =  8,
    OBJTYPE_vec2i  =  9,
    OBJTYPE_vec3i  = 10,
} OBJTYPE_builtins;
#endif

#include "obj_pack.h"
#include "obj_reflect.h"
#include "obj_id.h"
#include "obj_obj.h" // after reflect and objid
#include "obj_pack_cobs.h"
#include "obj_pack_compress.h"
