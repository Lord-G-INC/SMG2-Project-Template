CC := deps/CodeWarrior/mwcceppc.exe
LD := deps/Kamek/Kamek.exe
CC_FLAGS := -c -Cpp_exceptions off -nodefaults -proc gekko -fp hard -lang=c++ -O4,s -inline on -rtti off -sdata 0 -sdata2 0 -align powerpc -func_align 4 -str pool -enum int -DGEKKO -DMTX_USE_PS -i . -I- -i include -I- -i deps\\Syati\\include
SYMBOLS := deps/Syati/symbols

INPUT := $(wildcard source/*.cpp) $(wildcard source/pt/**/*.cpp) $(wildcard source/pt/**/**/*.cpp)
OUTPUT := $(patsubst source/%.cpp, build/%.o,$(INPUT))
RESULT := CustomCode_$(REG).bin

$(if $(EXTRA), $(info Extra Build Flag $(EXTRA) enabled.))

main: $(RESULT)
	@cmd /c rmdir /s /q build 

$(RESULT): $(OUTPUT)
	@$(LD) $^ -externals=$(SYMBOLS)/$(REG).txt -output-kamek=$@

build/%.o: source/%.cpp
	@mkdir -p $(patsubst %/,%,$(dir $@))
	@$(if $(EXTRA), $(CC) $(CC_FLAGS) -D$(REG) -D$(EXTRA) $< -o $@, $(CC) $(CC_FLAGS) -D$(REG) $< -o $@)
	@echo $@

GLE:
	$(MAKE) REG=USA EXTRA=GLE
	$(MAKE) REG=PAL EXTRA=GLE
	$(MAKE) REG=JAP EXTRA=GLE
	@mv *.bin SMG2PTD/CustomCode
	@cd SMG2PTD; ..\\7z.exe a "../PTD Debug with GLE.zip" *.* -r -x!LoaderK.bin -x!LoaderW.bin; cd CustomCode; rm CustomCode*.bin

NO_GLE:
	$(MAKE) REG=USA EXTRA=NOGLE
	$(MAKE) REG=PAL EXTRA=NOGLE
	$(MAKE) REG=JAP EXTRA=NOGLE
	$(MAKE) REG=KOR EXTRA=NOGLE
	$(MAKE) REG=TWN EXTRA=NOGLE
	@mv *.bin SMG2PTD/CustomCode
	@cd SMG2PTD; ..\\7z.exe a "../PTD Debug.zip" *.* -r; cd CustomCode; rm CustomCode*.bin

all:
	$(MAKE) NO_GLE
	$(MAKE) GLE