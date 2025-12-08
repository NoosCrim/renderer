ROOT_DIR ?= ../
SUB_DIR_NAME := glsl_preprocess
include $(ROOT_DIR)/shared.mk
PHONY: glsl_preprocess
glsl_preprocess: $($(SUB_DIR_NAME)_OUT_DIR)glsl_preprocess
$($(SUB_DIR_NAME)_OUT_DIR)glsl_preprocess: $($(SUB_DIR_NAME)_OBJ_DIR)glsl_preprocess.o
	$(C++) $($(SUB_DIR_NAME)_OBJ_DIR)glsl_preprocess.o -o $($(SUB_DIR_NAME)_OUT_DIR)glsl_preprocess
clear_glsl_preprocess:
	rm -rf $($(SUB_DIR_NAME)_OUT_DIR)hello_world