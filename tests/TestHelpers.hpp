#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "Catch2/catch_amalgamated.hpp"
#include "ImageIO.hpp"

#define ASSERT_IMAGE(NAME, WIDTH, HEIGHT, ACT_PTR, GOLD_PTR)                                         \
    do {                                                                                            \
        bool __same =                                                                               \
            Compare8bpp((const uint8_t *)(ACT_PTR), (const uint8_t *)(GOLD_PTR),                    \
                        static_cast<size_t>(WIDTH) * static_cast<size_t>(HEIGHT));                  \
        INFO("image: " << (NAME) << " size:" << (WIDTH) << "x" << (HEIGHT));                    \
        CHECK(__same);                                                                              \
    } while (0)
