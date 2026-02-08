@echo off

mkdir ..\..\build
pushd ..\..\build
cl -Fc -Zi "..\C game\code\win32_handmade.cpp" user32.lib Gdi32.lib onecore.lib Xinput.lib Xinput9_1_0.lib Xaudio2.lib 
popd