.DEFAULT_GOAL := all
.PHONY: all tidy tidy-tests tidy-sdl-ibmf tidy-sdl-ttf format format-check test examples-sdl-ibmf examples-sdl-ttf

all: test examples-sdl-ibmf examples-sdl-ttf

# Convenience delegations to sub-makefiles
test:
	@$(MAKE) -C tests test

examples-sdl-ibmf:
	@$(MAKE) -C examples/SDL/IBMF build

examples-sdl-ttf:
	@$(MAKE) -C examples/SDL/TTF build

# Override to your local installation if needed
CLANG_TIDY_RUN ?= run-clang-tidy
CLANG_FORMAT ?= clang-format
CMAKE_FLAGS ?= -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
# Exclude vendored freetype and Catch2 amalgamation from analysis
TIDY_FILES ?= '^(?!.*(/freetype/|/Catch2/catch_amalgamated\\.cpp|/Catch2/catch_amalgamated\\.hpp)).*$$'

tidy: tidy-tests tidy-sdl-ibmf tidy-sdl-ttf

tidy-tests:
	@mkdir -p tests/build
	@cmake -S tests -B tests/build $(CMAKE_FLAGS)
	@$(CLANG_TIDY_RUN) -p tests/build $(TIDY_FILES)

tidy-sdl-ibmf:
	@mkdir -p examples/SDL/IBMF/build
	@cmake -S examples/SDL/IBMF -B examples/SDL/IBMF/build $(CMAKE_FLAGS)
	@$(CLANG_TIDY_RUN) -p examples/SDL/IBMF/build $(TIDY_FILES)

tidy-sdl-ttf:
	@mkdir -p examples/SDL/TTF/build
	@cmake -S examples/SDL/TTF -B examples/SDL/TTF/build $(CMAKE_FLAGS)
	@$(CLANG_TIDY_RUN) -p examples/SDL/TTF/build $(TIDY_FILES)

# Format all tracked C/C++ sources via git, excluding freetype and Catch2 amalgamation
FORMAT_FILES := $(shell git ls-files '*.c' '*.cc' '*.cxx' '*.cpp' '*.h' '*.hpp' | grep -Ev '^(freetype/|tests/Catch2/)')

format:
	@echo Formatting $$((`echo "$(FORMAT_FILES)" | wc -w`)) files...
	@echo "$(FORMAT_FILES)" | xargs -n 50 $(CLANG_FORMAT) -i

format-check:
	@echo Checking format on $$((`echo "$(FORMAT_FILES)" | wc -w`)) files...
	@echo "$(FORMAT_FILES)" | xargs -n 50 $(CLANG_FORMAT) -n --Werror
