.PHONY: build
build:
	@mkdir -p build
	@cd build && cmake .. && cmake --build . --target ttf_sdl -j

.PHONY: run
run: build
	@cd build && ./ttf_sdl

.phony: clean
clean:
	@rm -rf build
