.PHONY: OpenGL_utils

OPENGL_UTILS_OBJ:=$(patsubst %.cpp,$(OBJ)%.o,$(wildcard ./OpenGL_utils/*.cpp))
OPENGL_UTILS_LIB:=$(OUT)OpenGL_utils/libOpenGL_utils.a

OpenGL_utils: $(OPENGL_UTILS_LIB)

$(OPENGL_UTILS_LIB): $(OPENGL_UTILS_OBJ) ./OpenGL_utils/OpenGL_utils.mk
	@mkdir -p $(dir $@)
	@echo "Linking $(OPENGL_UTILS_LIB)..."
	$(AR) rcs -o $@ $(OPENGL_UTILS_OBJ)

-include $(wildcard $(DEP)OpenGL_utils/*.d)