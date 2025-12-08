ROOT_DIR ?= ./
include shared.mk

clear:
	rm -rf $(GLOBAL_OUT_DIR)*

include glsl_preprocess/Makefile
include renderer/Makefile