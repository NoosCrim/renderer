ROOT_DIR ?= ./
GLOBAL_OUT_DIR := $(ROOT_DIR)out/
C++ := g++
AR := ar
COMPILE_FLAGS := -Wall -Wextra -Werror -std=c++20 -g -DRENDER_DEBUG_LEVEL

$(SUB_DIR_NAME)_SUB_DIR_PATH := $(ROOT_DIR)$(SUB_DIR_NAME)/
$(SUB_DIR_NAME)_OUT_DIR := $(GLOBAL_OUT_DIR)$(SUB_DIR_NAME)/
$(SUB_DIR_NAME)_DEP_DIR := $($(SUB_DIR_NAME)_OUT_DIR)dep/
$(SUB_DIR_NAME)_OBJ_DIR := $($(SUB_DIR_NAME)_OUT_DIR)obj/

%/:
	mkdir -p $@



define compile_target

-include $(DEP_DIR)*.d
$($(SUB_DIR_NAME)_OBJ_DIR)%.o: $($(SUB_DIR_NAME)_SUB_DIR_PATH)%.cpp
	@echo "Compiling $$<..."
	mkdir -p $($(SUB_DIR_NAME)_OUT_DIR) $($(SUB_DIR_NAME)_OBJ_DIR) $($(SUB_DIR_NAME)_DEP_DIR)
	$(C++) $(COMPILE_FLAGS) -MMD -MP -MF $($(SUB_DIR_NAME)_DEP_DIR)$$*.d -c $$< -o $($(SUB_DIR_NAME)_OBJ_DIR)$$*.o
	@echo "Compiled $$< successfully."

endef

