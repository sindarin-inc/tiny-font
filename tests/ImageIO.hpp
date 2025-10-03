#pragma once

#include <cstdint>
#include <vector>

auto Save8bpp(const char *path, int width, int height, const uint8_t *buffer) -> bool;
auto Load8bpp(const char *path, int &width, int &height, std::vector<uint8_t> &out) -> bool;
auto Compare8bpp(const uint8_t *a, const uint8_t *b, size_t count) -> bool;


