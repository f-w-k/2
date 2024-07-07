if "%1"=="tidy" (
	del *.obj
	del *.exe
	del *.pdb
	del *.ilk
	del *.dll
	exit /b
)

copy /y ..\..\..\code\3rd\sdl3\x64\SDL3.dll
cl *.cpp -I 3rd /std:c++20 /EHsc -I ..\..\..\code\3rd\ /link /LIBPATH:..\..\..\code\3rd\sdl3\x64 %*
