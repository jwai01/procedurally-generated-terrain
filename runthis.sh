#!/bin/bash

# Colors for terminal output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Procedural Terrain Generator - Build & Run Script${NC}"
echo "======================================================="

# Make sure we're in the project directory
cd "$(dirname "$0")"

# Step 1: Check if dependencies are installed
# echo -e "\n${YELLOW}Checking dependencies...${NC}"
# if [ -f "./dependencies.sh" ]; then
#     echo "Dependencies script found. Running dependencies.sh..."
#     chmod +x ./dependencies.sh
#     ./dependencies.sh
# else
#     echo -e "${YELLOW}No dependencies.sh found. Make sure all required libraries are installed.${NC}"
# fi

# Step 2: Create build directory if it doesn't exist
echo -e "\n${YELLOW}Setting up build directory...${NC}"
if [ ! -d "./build" ]; then
    echo "Creating build directory..."
    mkdir build
    if [ $? -ne 0 ]; then
        echo -e "${RED}Failed to create build directory!${NC}"
        exit 1
    fi
fi

# Step 3: Run CMake
echo -e "\n${YELLOW}Configuring with CMake...${NC}"
cd build
cmake ..
if [ $? -ne 0 ]; then
    echo -e "${RED}CMake configuration failed!${NC}"
    exit 1
fi

# Step 4: Build the project
echo -e "\n${YELLOW}Building project...${NC}"
cmake --build .
if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

# Step 5: Run the executable
echo -e "\n${YELLOW}Running Terrain Generator...${NC}"
echo "======================================================="
echo -e "${GREEN}Controls:${NC}"
echo "  W/A/S/D - Change look direction"
echo "  O       - Move forward"
echo "  L       - Move backward"
echo "======================================================="
echo ""

# Make sure the executable is runnable
chmod +x ./TerrainGenerator

# Run the application
./TerrainGenerator

# Check if application ran successfully
if [ $? -ne 0 ]; then
    echo -e "\n${RED}Application crashed or exited with errors!${NC}"
    exit 1
else
    echo -e "\n${GREEN}Application closed successfully.${NC}"
fi