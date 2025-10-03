.PHONY: build test
build:
	@mkdir -p build
	@cd build && cmake .. && cmake --build . -j

test: build
	@cd build && ctest --output-on-failure


