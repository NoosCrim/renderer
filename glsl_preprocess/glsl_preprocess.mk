.PHONY: glsl_preprocess

GLSL_PREPROCESS_OBJ:=$(patsubst %.cpp,$(OBJ)%.o,$(wildcard ./glsl_preprocess/*.cpp))
GLSL_PREPROCESS_EXEC:=$(OUT)glsl_preprocess/glsl_preprocess

glsl_preprocess: $(GLSL_PREPROCESS_EXEC)

$(GLSL_PREPROCESS_EXEC): $(GLSL_PREPROCESS_OBJ) ./glsl_preprocess/glsl_preprocess.mk
	@mkdir -p $(OUT)glsl_preprocess/
	@echo "Linking $(GLSL_PREPROCESS_EXEC)..."
	g++ -o $@ $(GLSL_PREPROCESS_OBJ)

-include $(wildcard $(DEP)glsl_preprocess/*.d)