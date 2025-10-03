#include <SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "Font.hpp"
#include "FontDefs.hpp"
#include "UTF8Iterator.hpp"

#include "IBMFDriver/IBMFFontData.hpp"
#include "IBMFFonts/SolSans_75.h"

using namespace ibmf_defs;
using namespace font_defs;

static void convert1bppToARGB8888(const uint8_t *src, int srcPitch, int width, int height,
                                  uint32_t *dst, int dstPitchPixels) {
    const uint32_t on = 0xFFFFFFFFu;   // white
    const uint32_t off = 0xFF000000u;  // black
    for (int y = 0; y < height; ++y) {
        const uint8_t *row = src + y * srcPitch;
        uint32_t *out = dst + y * dstPitchPixels;
        for (int x = 0; x < width; ++x) {
            int byteIndex = x >> 3;
            int bitIndex = 7 - (x & 7);
            uint8_t byte = row[byteIndex];
            uint8_t bit = (byte >> bitIndex) & 1;
            out[x] = bit ? on : off;
        }
    }
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    const int winW = 640;
    const int winH = 360;

    SDL_Window *win = SDL_CreateWindow("IBMF SDL Example", SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED, winW, winH, SDL_WINDOW_SHOWN);
    if (!win) {
        std::fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) {
        std::fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                         winW, winH);
    if (!tex) {
        std::fprintf(stderr, "SDL_CreateTexture Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // Create a 1bpp framebuffer for the canvas
    Bitmap canvas;
    canvas.dim = Dim(winW, winH);
    canvas.pitch = (winW + 7) >> 3; // bytes per row in 1bpp
    size_t bufSize = static_cast<size_t>(canvas.pitch) * static_cast<size_t>(winH);
    canvas.pixels = new uint8_t[bufSize];
    std::memset(canvas.pixels, 0xFF, bufSize); // start white background for 1bpp

    // Load IBMF font data and set up faces
    FontData fontData(SOLSANS_75_IBMF, SOLSANS_75_IBMF_LEN);
    std::printf("IBMF faces: %d\n", fontData.getFaceCount());
    std::vector<Font> fonts;
    int x = 20;
    int y = 20;
    for (int i = 0; i < fontData.getFaceCount(); i++) {
        Font font0(fontData, i);
        fonts.push_back(font0);
        font0.drawSingleLineOfText(canvas, Pos(x, y), "Hello IBMF " + std::to_string(i), false);
        y += font0.lineHeight() + 8;
    }

    // Upload to SDL texture
    void *pixels = nullptr;
    int pitch = 0;
    if (SDL_LockTexture(tex, nullptr, &pixels, &pitch) == 0) {
        convert1bppToARGB8888(canvas.pixels, canvas.pitch, winW, winH, static_cast<uint32_t *>(pixels), pitch / 4);
        SDL_UnlockTexture(tex);
    }

    bool quit = false;
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) quit = true;
        }

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, nullptr, nullptr);
        SDL_RenderPresent(ren);

        SDL_Delay(16);
    }

    delete[] canvas.pixels;
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}


