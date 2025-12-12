ROOT_DIR ?= ../
SUB_DIR_NAME := glsl_preprocess
include $(ROOT_DIR)/shared.mk

$(eval $(call compile_target))

PHONY: glsl_preprocess
glsl_preprocess: $(glsl_preprocess_OUT_DIR)glsl_preprocess

$(glsl_preprocess_OUT_DIR)glsl_preprocess.so: $(glsl_preprocess_OBJ_DIR)glsl_preprocess.o
	@echo "Linking glsl_preprocess..."
	$(C++) -std=c++20 $(glsl_preprocess_OBJ_DIR)glsl_preprocess.o -o $(glsl_preprocess_OUT_DIR)glsl_preprocess
	@echo "glsl_preprocess built successfully."

clear_glsl_preprocess:
	rm -rf $(glsl_preprocess_OUT_DIR)