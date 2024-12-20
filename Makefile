include .env

CFLAGS = -std=c++17 -I. -I"$(VULKAN_SDK_PATH)/include" -I"$(GLFW_PATH)/include" -I"$(ENTT_PATH)"
LDFLAGS = -L"$(VULKAN_SDK_PATH)/lib" -L"$(GLFW_PATH)/lib-mingw-w64" -lglfw3 -lvulkan-1 -lgdi32

# create list of all spv files and set as dependency
vertSources = $(shell find ./shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find ./shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))

TARGET = a.out
$(TARGET): $(vertObjFiles) $(fragObjFiles)
$(TARGET): *.cpp *.hpp
	g++ $(CFLAGS) -o $(TARGET) *.cpp $(LDFLAGS)

# make shader targets
%.spv: %
	${GLSLC} $< -o $@

.PHONY: test clean

test: a.out
	./a.out

clean:
	rm -f a.out