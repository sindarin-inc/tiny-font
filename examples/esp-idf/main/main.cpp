// Minimal smoke test for ESP-IDF demonstrating both IBMF and TTF builds.
// Select at menuconfig: Tiny Font Options -> Font System.

#include <Font.hpp>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if CONFIG_TINYFONT_TTF
#include "TTFDriver/TTFNotoSansLight.hpp"
using namespace ttf_defs;
#else
#include "IBMFFonts/SolSans_75.h"
using namespace ibmf_defs;
#endif

using namespace font_defs;

extern "C" void app_main(void) {
    const char *line = "Tiny Font on ESP-IDF";

#if CONFIG_TINYFONT_TTF
    // TTF path: 8bpp canvas, grayscale drawing
    TTFNotoSansLight fontData;
    Font font(fontData, 22);

    Dim textDim = font.getTextSize(line);
    int width = textDim.width + 8;
    int height = font.lineHeight() + 8;

    Bitmap canvas{};
    canvas.dim = Dim(width, height);
    canvas.pitch = width; // 8bpp
    canvas.pixels =
        static_cast<uint8_t *>(heap_caps_malloc((size_t)canvas.pitch * height, MALLOC_CAP_SPIRAM));
    if (!canvas.pixels) {
        canvas.pixels = static_cast<uint8_t *>(
            heap_caps_malloc((size_t)canvas.pitch * height, MALLOC_CAP_DEFAULT));
    }
    if (!canvas.pixels) {
        printf("Failed to allocate canvas\n");
        return;
    }
    memset(canvas.pixels, 255, (size_t)canvas.pitch * height); // white background

    font.drawSingleLineOfText(canvas, Pos(4, 4), line, false);

    printf("[TTF] Rendered line: %s (w=%d h=%d lineHeight=%d)\n", line, textDim.width,
           textDim.height, font.lineHeight());

    // Dump first row sample
    for (int i = 0; i < (width > 8 ? 8 : width); ++i) {
        printf(i == 0 ? "row0: %02X" : " %02X", canvas.pixels[i]);
    }
    printf("\n");

    heap_caps_free(canvas.pixels);
#else
    // IBMF path: 1bpp canvas, bitmap drawing
    FontData fontData(SOLSANS_75_IBMF, SOLSANS_75_IBMF_LEN);
    Font font(fontData, 0);

    Dim textDim = font.getTextSize(line);
    int width = textDim.width + 4;
    int height = font.lineHeight() + 4;

    Bitmap canvas{};
    canvas.dim = Dim(width, height);
    canvas.pitch = (width + 7) >> 3; // 1bpp packed
    canvas.pixels =
        static_cast<uint8_t *>(heap_caps_malloc((size_t)canvas.pitch * height, MALLOC_CAP_SPIRAM));
    if (!canvas.pixels) {
        canvas.pixels = static_cast<uint8_t *>(
            heap_caps_malloc((size_t)canvas.pitch * height, MALLOC_CAP_DEFAULT));
    }
    if (!canvas.pixels) {
        printf("Failed to allocate canvas\n");
        return;
    }
    memset(canvas.pixels, 0x00, (size_t)canvas.pitch * height);

    font.drawSingleLineOfText(canvas, Pos(2, 2), line, false);

    printf("[IBMF] Rendered line: %s (w=%d h=%d lineHeight=%d)\n", line, textDim.width,
           textDim.height, font.lineHeight());

    // Dump first few bytes as a sanity check
    for (int i = 0; i < (height > 4 ? 4 : height); ++i) {
        printf("row %d: %02X %02X %02X %02X\n", i, canvas.pixels[i * canvas.pitch + 0],
               (canvas.pitch > 1 ? canvas.pixels[i * canvas.pitch + 1] : 0),
               (canvas.pitch > 2 ? canvas.pixels[i * canvas.pitch + 2] : 0),
               (canvas.pitch > 3 ? canvas.pixels[i * canvas.pitch + 3] : 0));
    }

    heap_caps_free(canvas.pixels);
#endif

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
