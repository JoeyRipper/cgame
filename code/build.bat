@echo off

mkdir ..\..\build
pushd ..\..\build
cl -Fc -Zi "..\C game\code\win32_handmade.cpp" user32.lib Gdi32.lib onecore.lib
popd