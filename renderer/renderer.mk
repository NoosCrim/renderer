.PHONY: renderer shaders renderer_demo run_renderer_demo all

RENDERER_SHADERS:=./renderer/shader/processed/general.vert.glsl ./renderer/shader/processed/brdf.frag.glsl

shaders: $(RENDERER_SHADERS)

./renderer/shader/processed/%.glsl: $(GLSL_PREPROCESS_EXEC) ./renderer/shader/%.glsl ./renderer/renderer.mk
	@mkdir -p $(dir $@)
	@echo "Preprocessing $@..."
	./out/glsl_preprocess/glsl_preprocess ./renderer/shader/$*.glsl ./renderer/shader/processed/$*.glsl

RENDERER_OBJ:=$(patsubst %.cpp,$(OBJ)%.o,$(wildcard renderer/*.cpp))
RENDERER_LIBRARIAN:=$(INTERMEDIATE)renderer/librarian.mri
RENDERER_NO_UTILS_LIB:=$(INTERMEDIATE)renderer/librenderer_no_utils.a
RENDERER_LIBS_TO_MERGE:=$(OPENGL_UTILS_LIB) $(RENDERER_NO_UTILS_LIB)
RENDERER_LIB:=$(OUT)renderer/librenderer.a

renderer: $(RENDERER_LIB)

$(RENDERER_NO_UTILS_LIB) : $(RENDERER_OBJ) ./renderer/renderer.mk
	@mkdir -p $(dir $@)
	@echo Linking $(RENDERER_NO_UTILS_LIB)...
	$(AR) rcs -o $@ $(RENDERER_OBJ)

$(RENDERER_LIBRARIAN) : ./renderer/renderer.mk
	@mkdir -p $(dir $@)
	@echo Creating $(RENDERER_LIBRARIAN)...
	@echo "create $(RENDERER_LIB)" > $(RENDERER_LIBRARIAN)
	@$(foreach lib,$(RENDERER_LIBS_TO_MERGE),echo "addlib $(lib)" >> $(RENDERER_LIBRARIAN);)
	@echo "save" >> $(RENDERER_LIBRARIAN)
	@echo "end" >> $(RENDERER_LIBRARIAN)

$(RENDERER_LIB) : $(RENDERER_LIBRARIAN) $(RENDERER_LIBS_TO_MERGE) $(RENDERER_SHADERS)
	@mkdir -p $(dir $@)
	rm -f @$
	ar -M < $(RENDERER_LIBRARIAN)


RENDERER_DEMO_OBJ:=$(patsubst %.cpp,$(OBJ)%.o,$(wildcard renderer/demo/*.cpp))
RENDERER_DEMO_EXEC:=$(OUT)renderer/demo/renderer_demo

renderer_demo: $(RENDERER_DEMO_EXEC)

run_renderer_demo: $(RENDERER_DEMO_EXEC)
	$(RENDERER_DEMO_EXEC)

$(RENDERER_DEMO_EXEC): $(RENDERER_LIB) $(RENDERER_DEMO_OBJ) ./renderer/renderer.mk
	mkdir -p $(dir $@)
	@echo "Linking renderer_demo..."
	g++ -o $@ $(RENDERER_DEMO_OBJ) -L./out/renderer -lGL -lGLEW -lglfw -lrenderer

-include $(wildcard $(DEP)renderer/demo/*.d)
-include $(wildcard $(DEP)renderer/*.d)