#!/bin/sh
# Type a script or drag a script file from your workspace to insert its path.
echo "Compiling shaders!"
SHADERS="./vulkan-fun/shaders/*"
COMPILED="./vulkan-fun/compiled/"
GLSLC="/Users/tylerrajotte/VulkanSDK/1.2.176.1/macOS/bin/glslc"
for SHADER in $SHADERS
do
 SHADERPATH=$(echo $SHADER | cut -d '.' -f 2)
 SHADERNAME=$(echo $SHADERPATH | cut -d '/' -f 4)
 $GLSLC $SHADER -o $COMPILED$SHADERNAME.spv
done

