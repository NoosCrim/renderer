C++ := g++
AR := ar
COMPILE_FLAGS := -Wall -Wextra -Werror -std=c++20 -g -DRENDER_DEBUG_LEVEL -I. -DGLM_ENABLE_EXPERIMENTAL
OUT := ./out/
OBJ := $(OUT)obj/
DEP := $(OUT)dep/
INTERMEDIATE := $(OUT)intermediate/

all: renderer renderer_demo

include glsl_preprocess/glsl_preprocess.mk
include OpenGL_utils/OpenGL_utils.mk
include renderer/renderer.mk

$(OBJ)%.o $(DEP)%.d: %.cpp
	@mkdir -p $(dir $(DEP)$*.d) $(dir $(OBJ)$*.o)
	@echo "Compiling $<..."
	$(C++) $(COMPILE_FLAGS) -MMD -MP -MF $(DEP)$*.d -c $< -o $(OBJ)$*.o

.PHONY: clean

clean:
	rm -r $(OUT)/*
