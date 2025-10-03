.PHONY: build
build:
	@mkdir -p build
	@cd build && cmake .. && cmake --build . --target ibmf_sdl -j

.PHONY: run
run: build
	@cd build && ./ibmf_sdl

.phony: clean
clean:
	@rm -rf build
