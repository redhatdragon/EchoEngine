mkdir deps
cd deps
git clone https://github.com/libsdl-org/SDL SDL2
https://github.com/libsdl-org/SDL_image SDL2_image
git clone https://github.com/libsdl-org/SDL_ttf SDL2_ttf
cd SDL2_ttf/external
git clone https://github.com/freetype/freetype freetype
cd ../../
cd ../