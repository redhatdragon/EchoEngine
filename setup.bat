mkdir deps
cd deps
echo disabled... git clone https://github.com/libsdl-org/SDL SDL2
echo disabled... git clone https://github.com/libsdl-org/SDL_image SDL2_image
echo disabled... git clone https://github.com/libsdl-org/SDL_ttf SDL2_ttf
echo disabled... cd SDL2_ttf/external
echo disabled... git clone https://github.com/freetype/freetype freetype
echo disabled... cd ../../
echo disabled... cmake ./
cd ../

git clone https://github.com/slembcke/Chipmunk2D deps/Chipmunk2D

cd depsVisible/magnum
git clone https://github.com/mosra/corrade.git
git clone https://github.com/mosra/magnum.git
cd ../../

git clone https://github.com/glfw/glfw deps/glfw
git clone https://github.com/g-truc/glm deps/glm
echo git clone https://github.com/nigels-com/glew deps/glew
git clone https://github.com/nothings/stb deps/stb

git clone https://github.com/Bly7/OBJ-Loader deps/OBJ-Loader