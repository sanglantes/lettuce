
#!/bin/bash
set -e  # Exit on error

if pkg-config --exists sdl3; then
    echo "SDL3 is already installed. Skipping installation."
else
    echo "SDL3 not found. Installing..."
    git clone https://github.com/libsdl-org/SDL SDL3
    cd SDL3
    cmake -S . -B build
    cd build
    make -j$(nproc)
    sudo make install
    cd ../..
    sudo ldconfig
fi

git clone https://github.com/sabdul-khabir/SDL3_gfx
cd SDL3_gfx
cmake -S . -B build
cd build
make -j$(nproc)
sudo make install
cd ../..
sudo ldconfig

echo "Compiling the project..."
gcc -ISDL3_gfx/ -Iinclude/ src/babai.c src/lagrange.c src/utils.c src/vector.c src/main.c \
    -lm -lSDL3 -lSDL3_gfx -Ofast -o lettuce

echo "Build complete! Run ./lettuce to start."
