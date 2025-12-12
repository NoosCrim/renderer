ROOT_DIR ?= ../
SUB_DIR_NAME := OpenGL_utils
include $(ROOT_DIR)/shared.mk

$(eval $(call compile_target))

PHONY: OpenGL_utils

OpenGL_utils: $(OpenGL_utils_OUT_DIR)OpenGL_utils
$(OpenGL_utils_OUT_DIR)OpenGL_utils: $(patsubst $(OpenGL_utils_SUB_DIR_PATH)%.cpp, $(OpenGL_utils_OBJ_DIR)%.o, $(wildcard $(OpenGL_utils_SUB_DIR_PATH)*.cpp))
	@echo "Linking OpenGL_utils..."
	$(AR) rcs -o $(OpenGL_utils_OUT_DIR)OpenGL_utils.so $^
	@echo "OpenGL_utils built successfully."