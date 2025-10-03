#include "ImageIO.hpp"

#include <cstdio>
#include <cstring>
#include <png.h>
#include <vector>

static auto writePNG(const char *path, int width, int height, const uint8_t *data) -> bool {
    FILE *fp = std::fopen(path, "wb");
    if (!fp) return false;
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        std::fclose(fp);
        return false;
    }
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        std::fclose(fp);
        return false;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        std::fclose(fp);
        return false;
    }
    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);
    std::vector<png_bytep> rows(static_cast<size_t>(height));
    for (int y = 0; y < height; ++y) {
        rows[static_cast<size_t>(y)] = (png_bytep)(data + y * width);
    }
    png_write_image(png_ptr, rows.data());
    png_write_end(png_ptr, nullptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    std::fclose(fp);
    return true;
}

static auto readPNG(const char *path, int &width, int &height, std::vector<uint8_t> &out) -> bool {
    FILE *fp = std::fopen(path, "rb");
    if (!fp) return false;
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        std::fclose(fp);
        return false;
    }
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        std::fclose(fp);
        return false;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        std::fclose(fp);
        return false;
    }
    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);
    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    auto color = png_get_color_type(png_ptr, info_ptr);
    auto depth = png_get_bit_depth(png_ptr, info_ptr);
    if (color != PNG_COLOR_TYPE_GRAY || depth != 8) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        std::fclose(fp);
        return false;
    }
    out.resize(static_cast<size_t>(width * height));
    std::vector<png_bytep> rows(static_cast<size_t>(height));
    for (int y = 0; y < height; ++y) {
        rows[static_cast<size_t>(y)] = (png_bytep)(out.data() + y * width);
    }
    png_read_image(png_ptr, rows.data());
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    std::fclose(fp);
    return true;
}

auto Save8bpp(const char *path, int width, int height, const uint8_t *buffer) -> bool {
    return writePNG(path, width, height, buffer);
}

auto Load8bpp(const char *path, int &width, int &height, std::vector<uint8_t> &out) -> bool {
    return readPNG(path, width, height, out);
}

auto Compare8bpp(const uint8_t *a, const uint8_t *b, size_t count) -> bool {
    for (size_t i = 0; i < count; ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}
