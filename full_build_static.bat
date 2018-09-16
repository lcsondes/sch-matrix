call prepare

rem External libek
rem ===================================

rem Lua
rem ---------------
pushd ext_libs\lua
call make mingw
copy src\liblua.a ..\..\build\ /y
popd

rem Internal libek
rem ===================================

rem QtIOCompressor
rem ---------------
pushd int_libs\qtiocompressor
call qmake
call make release
copy release\libqtiocompressor.a ..\..\build\ /y
popd

rem libmnp4
rem ---------------
pushd int_libs\mnp4
call qmake
call make release
copy release\libmnp4.a ..\..\build\ /y
popd

rem Programok
rem ===================================

rem Animeditor
rem ---------------
pushd apps\animeditor
call lrelease animeditor.pro
call qmake
call make release
copy release\animeditor.exe ..\..\build\ /y
copy release\animeditor.exe ..\..\dist\ /y
popd

rem Kliens
rem ---------------
pushd apps\mxclient
call qmake
call make release
copy release\mxclient.exe ..\..\build\ /y
popd

rem Szerver
rem ---------------
pushd apps\mxserver
call qmake
call make release
copy release\mxserver.exe ..\..\build\ /y
popd

rem Q4Xtract
rem ---------------
pushd apps\q4xtract
call qmake
call make release
copy release\q4xtract.exe ..\..\build\ /y
popd
