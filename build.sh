# Create a 'build' directory if it doesn't exist
mkdir -p build

# Change into the 'build' directory
cd build

# Run CMake with Ninja generator
cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

# Build using Ninja
ninja

# Change back to the original directory
cd ..

# Copy the compile_commands.json file to the root directory
cp build/compile_commands.json compile_commands.json
