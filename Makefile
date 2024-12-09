include .env

CFLAGS = -std=c++17 -I. -I"$(VULKAN_SDK_PATH)/include" -I"$(GLFW_PATH)/include" -I"$(ENTT_PATH)"
LDFLAGS = -L"$(VULKAN_SDK_PATH)/lib" -L"$(GLFW_PATH)/lib-mingw-w64" -lglfw3 -lvulkan-1 -lgdi32

# Get all cpp files and create corresponding object files
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Shader files
vertSources = $(shell find ./shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find ./shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))

TARGET = a.out

$(TARGET): $(OBJECTS) $(vertObjFiles) $(fragObjFiles)
	g++ $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Rule for compiling cpp files to object files
%.o: %.cpp
	g++ $(CFLAGS) -MMD -MP -c $< -o $@

-include $(OBJECTS:.o=.d)

# Shader compilation rule
%.spv: %
	${GLSLC} $< -o $@

.PHONY: test clean

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)