#define CATCH_CONFIG_MAIN
#include "Catch2/catch_amalgamated.hpp"

#include <string>
#include <vector>

#include "ImageIO.hpp"
#include "TestHelpers.hpp"

#include "TTFDriver/TTFNotoSansLight.hpp"
#include "Font.hpp"

using namespace ttf_defs;
using namespace font_defs;

static auto renderTextTTF(const std::string &text, int width, int height, int ptSize)
    -> std::vector<uint8_t> {
    std::vector<uint8_t> out(static_cast<size_t>(width * height), 255);
    Bitmap canvas;
    canvas.dim = Dim(width, height);
    canvas.pitch = width;
    canvas.pixels = new uint8_t[static_cast<size_t>(canvas.pitch) * height];
    std::memset(canvas.pixels, 255, static_cast<size_t>(canvas.pitch) * height);

    TTFNotoSansLight fontData;
    Font font(fontData, ptSize);
    font.drawSingleLineOfText(canvas, Pos(10, 10), text, false);

    std::memcpy(out.data(), canvas.pixels, out.size());
    delete[] canvas.pixels;
    return out;
}

TEST_CASE("TTF renders Hello for selected sizes matches golden", "[ttf]") {
    const int W = 320, H = 120;
    const int sizes[] = {16, 20, 22, 24, 28};
    for (int sz : sizes) {
        INFO("TTF size " << sz);
        auto buf = renderTextTTF("Hello TTF", W, H, sz);

        std::string goldenPath = std::string(GOLDEN_DIR) + "/ttf_hello_" + std::to_string(sz) + ".png";

        int gw = 0, gh = 0;
        std::vector<uint8_t> golden;
        bool haveGolden = Load8bpp(goldenPath.c_str(), gw, gh, golden);

        if (!haveGolden) {
            REQUIRE(Save8bpp(goldenPath.c_str(), W, H, buf.data()));
            SUCCEED("Golden image created for size. Re-run tests.");
            continue;
        }

        REQUIRE(gw == W);
        REQUIRE(gh == H);
        ASSERT_IMAGE(goldenPath.c_str(), W, H, buf.data(), golden.data());
    }
}

// ---- Glyph grid tests (TTF) ----

static auto codePointToUtf8(int code) -> std::string {
    std::string glyph;
    if (code < 0x80) {
        glyph += static_cast<char>(code);
    } else if (code < 0x800) {
        glyph += static_cast<char>(0xC0 | (code >> 6));
        glyph += static_cast<char>(0x80 | (code & 0x3F));
    } else if (code < 0x10000) {
        glyph += static_cast<char>(0xE0 | (code >> 12));
        glyph += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
        glyph += static_cast<char>(0x80 | (code & 0x3F));
    }
    return glyph;
}

static auto generateGlyphsInRange(int start, int end) -> std::vector<std::string> {
    std::vector<std::string> glyphs;
    glyphs.reserve(static_cast<size_t>(end - start + 1));
    for (int code = start; code <= end; ++code) glyphs.push_back(codePointToUtf8(code));
    return glyphs;
}

static auto renderGlyphGridTTF(const std::vector<std::string> &glyphs, int gridW, int gridH,
                               int ptSize, int page, int width, int height)
    -> std::vector<uint8_t> {
    std::vector<uint8_t> out(static_cast<size_t>(width * height), 255);
    Bitmap canvas;
    canvas.dim = Dim(width, height);
    canvas.pitch = width;
    canvas.pixels = new uint8_t[static_cast<size_t>(canvas.pitch) * height];
    std::memset(canvas.pixels, 255, static_cast<size_t>(canvas.pitch) * height);

    TTFNotoSansLight fontData;
    Font font(fontData, ptSize);

    const int inset = 5;
    int xSpan = (width - inset * 2) / gridW;
    int ySpan = font.lineHeight() + 6; // use font line spacing with small padding
    int perPage = gridW * gridH;
    int startIdx = page * perPage;

    for (int i = 0; i < perPage; ++i) {
        int idx = startIdx + i;
        if (idx >= static_cast<int>(glyphs.size())) break;
        int gx = i % gridW;
        int gy = i / gridW;
        int x = inset + gx * xSpan;
        int y = inset + gy * ySpan;
        font.drawSingleLineOfText(canvas, Pos(x, y), glyphs[idx], false);
    }

    std::memcpy(out.data(), canvas.pixels, out.size());
    delete[] canvas.pixels;
    return out;
}

TEST_CASE("TTF glyph grids for blocks and sizes", "[ttf][glyphs]") {
    const int sizes[] = {16, 20, 22, 24};

    std::vector<std::pair<const char *, std::vector<std::string>>> blocks;
    blocks.emplace_back("latin", generateGlyphsInRange(0x0020, 0x007F));
    blocks.emplace_back("latin1", generateGlyphsInRange(0x0080, 0x00FF));
    blocks.emplace_back("latinExtA", generateGlyphsInRange(0x0100, 0x017F));
    blocks.emplace_back("punct", generateGlyphsInRange(0x2000, 0x206F));

    const struct SizeCfg {
        int pt;
        int gw, gh;
    } cfgs[] = {{16, 16, 18}, {20, 14, 16}, {22, 12, 14}, {24, 12, 12}};

    for (const auto &cfg : cfgs) {
        // Compute dynamic canvas size based on this size's line height
        TTFNotoSansLight tmpData;
        Font tmpFont(tmpData, cfg.pt);
        const int inset = 5;
        const int pad = 6;
        const int cellY = tmpFont.lineHeight() + pad;
        const int cellX = tmpFont.lineHeight() + pad;
        const int W = inset * 2 + cfg.gw * cellX;
        const int H = inset * 2 + cfg.gh * cellY;
        for (const auto &blk : blocks) {
            int perPage = cfg.gw * cfg.gh;
            int pages = static_cast<int>((blk.second.size() + perPage - 1) / perPage);
            for (int p = 0; p < pages; ++p) {
                auto buf = renderGlyphGridTTF(blk.second, cfg.gw, cfg.gh, cfg.pt, p, W, H);
                std::string goldenPath = std::string(GOLDEN_DIR) + "/ttf_grid_" +
                                         std::to_string(cfg.pt) + "_" + blk.first + "_" +
                                         std::to_string(p) + ".png";

                int gw = 0, gh = 0;
                std::vector<uint8_t> golden;
                bool haveGolden = Load8bpp(goldenPath.c_str(), gw, gh, golden);
                if (haveGolden && (gw != W || gh != H)) {
                    REQUIRE(Save8bpp(goldenPath.c_str(), W, H, buf.data()));
                    SUCCEED("Golden resized (TTF grid) " + goldenPath);
                    continue;
                }
                if (!haveGolden) {
                    REQUIRE(Save8bpp(goldenPath.c_str(), W, H, buf.data()));
                    SUCCEED("Golden created (TTF grid) " + goldenPath);
                    continue;
                }
                REQUIRE(gw == W);
                REQUIRE(gh == H);
                ASSERT_IMAGE(goldenPath.c_str(), W, H, buf.data(), golden.data());
            }
        }
    }
}
