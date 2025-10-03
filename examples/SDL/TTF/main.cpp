#include <SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "Font.hpp"
#include "FontDefs.hpp"
#include "TTFDriver/TTFNotoSansLight.hpp"

using namespace ttf_defs;
using namespace font_defs;

static void convert8bppToARGB8888(const uint8_t *src, int width, int height, uint32_t *dst,
                                  int dstPitchPixels) {
    for (int y = 0; y < height; ++y) {
        const uint8_t *row = src + y * width;
        uint32_t *out = dst + y * dstPitchPixels;
        for (int x = 0; x < width; ++x) {
            uint8_t v = row[x];
            out[x] = (0xFFu << 24) | (v << 16) | (v << 8) | v;
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

    SDL_Window *win = SDL_CreateWindow("TTF SDL Example", SDL_WINDOWPOS_CENTERED,
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

    Bitmap canvas;
    canvas.dim = Dim(winW, winH);
    canvas.pitch = winW; // 8bpp, 1 byte per pixel
    size_t bufSize = static_cast<size_t>(canvas.pitch) * static_cast<size_t>(winH);
    canvas.pixels = new uint8_t[bufSize];
    std::memset(canvas.pixels, 255, bufSize); // white background

    TTFNotoSansLight fontData;
    Font font0(fontData, 18);
    Font font1(fontData, 22);
    Font font2(fontData, 28);

    bool success = true;
    success = font0.setDisplayPixelResolution(PixelResolution::EIGHT_BITS);
    if (!success) {
        std::printf("Failed to set display pixel resolution for font0\n");
    }
    success = font1.setDisplayPixelResolution(PixelResolution::EIGHT_BITS);
    if (!success) {
        std::printf("Failed to set display pixel resolution for font1\n");
    }
    success = font2.setDisplayPixelResolution(PixelResolution::EIGHT_BITS);
    if (!success) {
        std::printf("Failed to set display pixel resolution for font2\n");
    }
    font0.setFontPixelResolution(PixelResolution::EIGHT_BITS);
    font1.setFontPixelResolution(PixelResolution::EIGHT_BITS);
    font2.setFontPixelResolution(PixelResolution::EIGHT_BITS);

    int x = 20;
    int y = 20;
    std::string text0 = "Hello TTF 18";
    std::string text1 = "Hello TTF 22";
    std::string text2 = "Hello TTF 28";

    font0.drawSingleLineOfText(canvas, Pos(x, y), text0, false);
    y += font0.lineHeight() + 8;
    font1.drawSingleLineOfText(canvas, Pos(x, y), text1, false);
    y += font1.lineHeight() + 8;
    font2.drawSingleLineOfText(canvas, Pos(x, y), text2, false);

    void *pixels = nullptr;
    int pitch = 0;
    if (SDL_LockTexture(tex, nullptr, &pixels, &pitch) == 0) {
        convert8bppToARGB8888(canvas.pixels, winW, winH, static_cast<uint32_t *>(pixels), pitch / 4);
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


