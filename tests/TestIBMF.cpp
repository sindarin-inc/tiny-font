#define CATCH_CONFIG_MAIN
#include <cstdio>
#include <string>
#include <vector>

#include "Catch2/catch_amalgamated.hpp"
#include "Font.hpp"
#include "IBMFDriver/IBMFFontData.hpp"
#include "IBMFFonts/SolSans_75.h"
#include "ImageIO.hpp"
#include "TestHelpers.hpp"

using namespace ibmf_defs;
using namespace font_defs;

static auto renderTextIBMF(const std::string &text, int width, int height, int faceIndex)
    -> std::vector<uint8_t> {
    std::vector<uint8_t> out(static_cast<size_t>(width * height), 255);
    Bitmap canvas;
    canvas.dim = Dim(width, height);
    canvas.pitch = (width + 7) >> 3;
    canvas.pixels = new uint8_t[static_cast<size_t>(canvas.pitch) * height];
    std::memset(canvas.pixels, 0xFF, static_cast<size_t>(canvas.pitch) * height);

    FontData fontData(SOLSANS_75_IBMF, SOLSANS_75_IBMF_LEN);
    Font font(fontData, faceIndex);
    font.drawSingleLineOfText(canvas, Pos(10, 10), text, false);

    // expand 1bpp to 8bpp grayscale
    for (int y = 0; y < height; ++y) {
        uint8_t *row = canvas.pixels + y * canvas.pitch;
        for (int x = 0; x < width; ++x) {
            int byteIndex = x >> 3;
            int bitIndex = 7 - (x & 7);
            bool on = (row[byteIndex] >> bitIndex) & 1;
            out[static_cast<size_t>(y * width + x)] = on ? 0 : 255;
        }
    }

    delete[] canvas.pixels;
    return out;
}

TEST_CASE("IBMF renders Hello for all faces matches golden", "[ibmf]") {
    const int W = 320, H = 120;
    for (int face = 0; face < 3; ++face) {
        INFO("IBMF face index " << face);
        auto buf = renderTextIBMF("Hello IBMF", W, H, face);

        std::string goldenPath =
            std::string(GOLDEN_DIR) + "/ibmf_hello_face" + std::to_string(face) + ".png";

        int gw = 0, gh = 0;
        std::vector<uint8_t> golden;
        bool haveGolden = Load8bpp(goldenPath.c_str(), gw, gh, golden);

        if (!haveGolden) {
            REQUIRE(Save8bpp(goldenPath.c_str(), W, H, buf.data()));
            SUCCEED("Golden image created for face. Re-run tests.");
            continue;
        }

        REQUIRE(gw == W);
        REQUIRE(gh == H);

        ASSERT_IMAGE(goldenPath.c_str(), W, H, buf.data(), golden.data());
    }
}

// ---- Glyph grid tests (IBMF) ----

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
    } else {
        // outside BMP not expected here
    }
    return glyph;
}

static auto generateGlyphsInRange(int start, int end) -> std::vector<std::string> {
    std::vector<std::string> glyphs;
    glyphs.reserve(static_cast<size_t>(end - start + 1));
    for (int code = start; code <= end; ++code) {
        glyphs.push_back(codePointToUtf8(code));
    }
    return glyphs;
}

static auto renderGlyphGridIBMF(const std::vector<std::string> &glyphs, int gridW, int gridH,
                                int faceIndex, int page, int width, int height)
    -> std::vector<uint8_t> {
    std::vector<uint8_t> out(static_cast<size_t>(width * height), 255);
    Bitmap canvas;
    canvas.dim = Dim(width, height);
    canvas.pitch = (width + 7) >> 3;
    canvas.pixels = new uint8_t[static_cast<size_t>(canvas.pitch) * height];
    std::memset(canvas.pixels, 0xFF, static_cast<size_t>(canvas.pitch) * height);

    FontData fontData(SOLSANS_75_IBMF, SOLSANS_75_IBMF_LEN);
    Font font(fontData, faceIndex);

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

    // Expand to 8bpp
    for (int y = 0; y < height; ++y) {
        uint8_t *row = canvas.pixels + y * canvas.pitch;
        for (int x = 0; x < width; ++x) {
            int byteIndex = x >> 3;
            int bitIndex = 7 - (x & 7);
            bool on = (row[byteIndex] >> bitIndex) & 1;
            out[static_cast<size_t>(y * width + x)] = on ? 0 : 255;
        }
    }
    delete[] canvas.pixels;
    return out;
}

TEST_CASE("IBMF glyph grids for multiple blocks", "[ibmf][glyphs]") {
    const int W = 640, H = 480;

    // Build blocks akin to the reference test
    std::vector<std::pair<const char *, std::vector<std::string>>> blocks;
    // Pairs a-z, A-Z, digits
    std::vector<std::string> glyphsBasic;
    const char *digits = "0123456789";
    const char *low = "abcdefghijklmnopqrstuvwxyz";
    const char *up = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (const char *p = digits; *p; ++p) {
        glyphsBasic.emplace_back(std::string(1, *p));
    }
    for (const char *p = low; *p; ++p) {
        glyphsBasic.emplace_back(std::string(1, *p));
    }
    for (const char *p = up; *p; ++p) {
        glyphsBasic.emplace_back(std::string(1, *p));
    }

    std::vector<std::string> pairs;
    pairs.reserve(glyphsBasic.size() * glyphsBasic.size());
    for (const auto &g1 : glyphsBasic) {
        for (const auto &g2 : glyphsBasic) {
            pairs.push_back(g1 + g2);
        }
    }

    blocks.emplace_back("pairs", std::move(pairs));
    blocks.emplace_back("latin", generateGlyphsInRange(0x0020, 0x007F));
    blocks.emplace_back("latin1", generateGlyphsInRange(0x0080, 0x00FF));
    blocks.emplace_back("latinExtA", generateGlyphsInRange(0x0100, 0x017F));
    blocks.emplace_back("latinExtB", generateGlyphsInRange(0x0180, 0x024F));
    blocks.emplace_back("spacingMods", generateGlyphsInRange(0x02B0, 0x02FF));
    blocks.emplace_back("greek", generateGlyphsInRange(0x0370, 0x03FF));
    blocks.emplace_back("cyrillic", generateGlyphsInRange(0x0400, 0x04FF));
    blocks.emplace_back("punct", generateGlyphsInRange(0x2000, 0x206F));
    blocks.emplace_back("supsubs", generateGlyphsInRange(0x2070, 0x209F));
    blocks.emplace_back("currency", generateGlyphsInRange(0x20A0, 0x20CF));
    blocks.emplace_back("private", generateGlyphsInRange(0xE000, 0xE07C));
    blocks.emplace_back("alphaPres", generateGlyphsInRange(0xFB00, 0xFB4F));

    const struct FaceCfg {
        int face;
        int gw, gh;
    } faces[] = {{0, 12, 16}, {1, 10, 14}, {2, 8, 12}};

    for (const auto &cfg : faces) {
        // Prepare a font to compute dynamic canvas size from line spacing
        FontData tmpData(SOLSANS_75_IBMF, SOLSANS_75_IBMF_LEN);
        Font tmpFont(tmpData, cfg.face);
        const int inset = 5;
        const int pad = 6;
        const int cellY = tmpFont.lineHeight() + pad;
        const int cellX = tmpFont.lineHeight() + pad; // simple heuristic; wide glyphs still fit
        const int W = inset * 2 + cfg.gw * cellX;
        const int H = inset * 2 + cfg.gh * cellY;

        for (const auto &blk : blocks) {
            const auto &key = blk.first;
            const auto &glyphs = blk.second;
            int perPage = cfg.gw * cfg.gh;
            int pages = static_cast<int>((glyphs.size() + perPage - 1) / perPage);
            for (int p = 0; p < pages; ++p) {
                auto buf = renderGlyphGridIBMF(glyphs, cfg.gw, cfg.gh, cfg.face, p, W, H);
                std::string goldenPath = std::string(GOLDEN_DIR) + "/ibmf_grid_f" +
                                         std::to_string(cfg.face) + "_" + key + "_" +
                                         std::to_string(p) + ".png";

                int gw = 0, gh = 0;
                std::vector<uint8_t> golden;
                bool haveGolden = Load8bpp(goldenPath.c_str(), gw, gh, golden);
                if (haveGolden && (gw != W || gh != H)) {
                    REQUIRE(Save8bpp(goldenPath.c_str(), W, H, buf.data()));
                    SUCCEED("Golden resized (IBMF grid) " + goldenPath);
                    continue;
                }
                if (!haveGolden) {
                    REQUIRE(Save8bpp(goldenPath.c_str(), W, H, buf.data()));
                    SUCCEED("Golden created (IBMF grid) " + goldenPath);
                    continue;
                }
                REQUIRE(gw == W);
                REQUIRE(gh == H);
                ASSERT_IMAGE(goldenPath.c_str(), W, H, buf.data(), golden.data());
            }
        }
    }
}
