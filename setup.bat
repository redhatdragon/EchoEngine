mkdir deps
cd deps
git clone https://github.com/libsdl-org/SDL SDL2
git clone https://github.com/libsdl-org/SDL_image SDL2_image
git clone https://github.com/libsdl-org/SDL_ttf SDL2_ttf
cd SDL2_ttf/external
git clone https://github.com/freetype/freetype freetype
cd ../../
cmake ./
cd ../

git clone https://github.com/slembcke/Chipmunk2D deps/Chipmunk2D