#!/bin/bash

# Update package list
sudo apt-get update

# Install essential build tools
sudo apt-get install -y \
    build-essential \
    cmake \
    git

# Install OpenGL dependencies
sudo apt-get install -y \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    mesa-common-dev

# Install GLFW dependencies
sudo apt-get install -y \
    libglfw3 \
    libglfw3-dev

# Install GLEW
sudo apt-get install -y \
    libglew-dev

echo "All dependencies have been installed successfully!"