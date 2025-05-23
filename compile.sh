#!/bin/bash

# Define the source files
SRC_FILES=$(find src/ -name "*.cpp")

# Define the output executable name
OUTPUT="main"

# Define compiler flags
CXX_FLAGS="-g -O2 -Wall -Wextra -pedantic -w -Iheaders "

# Compile the files
g++ $CXX_FLAGS $SRC_FILES -o $OUTPUT

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "✅ Compilation successful. You can run the game with ./$OUTPUT"
    
    # Optional: Uncomment the next line to automatically run the program
    # ./$OUTPUT

else
    echo "❌ Compilation failed."
fi
