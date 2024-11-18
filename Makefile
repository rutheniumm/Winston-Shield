# Define paths
SRC_DIR := src
SHADERS_DIR := shaders
ASSETS_DIR := assets
BUILD_DIR := build
INCLUDE_DIR := include

# Emscripten toolchain
EMCC := emcc
CXXFLAGS := -std=c++17 -O3 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sUSE_SDL=2 -FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1
LDFLAGS := -s TOTAL_MEMORY=512MB

# Add source files
SRC_FILES := $(wildcard $(SRC_DIR)/**/*.cpp)  # All .cpp files
SHADER_FILES := $(wildcard $(SHADERS_DIR)/*.vs $(SHADERS_DIR)/*.fs)  # Shader files
ASSET_FILES := $(wildcard $(ASSETS_DIR)/*)  # Asset files (e.g., .obj, textures)

# Output
OUTPUT := index.html

# Include directories (Assimp, GLM, GLFW, GLAD)
INCLUDE_FLAGS := -I$(INCLUDE_DIR) -I$(SRC_DIR) -I$(INCLUDE_DIR)/glm
LIBS := 

# Default target
all: $(OUTPUT)

# Build the project
$(OUTPUT): $(SRC_FILES) $(SHADER_FILES) $(ASSET_FILES)
	$(EMCC) $(CXXFLAGS) $(SRC_DIR)/Main.cpp $(SRC_DIR)/Core/tiny_obj_loader.cc $(SRC_DIR)/Game.cpp $(SRC_DIR)/Scene.cpp $(SRC_FILES) $(INCLUDE_FLAGS) -o $(OUTPUT) $(LIBS) \
		--preload-file $(ASSETS_DIR)@/assets \
		--preload-file $(SHADERS_DIR)@/shaders

# Clean build directory
clean:
	rm -rf $(BUILD_DIR) $(OUTPUT) *.js *.wasm *.data
