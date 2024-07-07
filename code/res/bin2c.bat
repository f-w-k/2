if not exist bin2c.exe cl bin2c.c /MT
if not exist bin2c.exe exit /b

del res.c >nul 2>nul

for %%i in (*.ttf;*.otf) do bin2c.exe %%i %%~ni %%~ni && (
	echo #if __has_include^("%%~ni"^)>> res.c
	echo #include "%%~ni">> res.c
	echo #endif>> res.c
)
echo struct resource_t { const char *name, *data; unsigned size; } resources[] = {>> res.c
for %%i in (*.ttf;*.otf) do (
	echo #if __has_include^("%%~ni"^)>> res.c
	echo { "%%i", %%~ni, ^(unsigned^)sizeof^(%%~ni^) },>> res.c
	echo #endif>> res.c
)
	echo { NULL, NULL, 0u },>> res.c
echo };>> res.c
echo.>> res.c

type res.c

del *.exe >nul 2>nul
del *.obj >nul 2>nul
