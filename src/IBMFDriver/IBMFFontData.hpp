#pragma once

#if CONFIG_TINYFONT_IBMF

#include <cstring>
#include <fstream>
#include <iostream>

#include "IBMFDefs.hpp"
#include "IBMFFace.hpp"

using namespace ibmf_defs;

/**
 * @brief Access to an IBMF font.
 *
 * This is a low-level class to allow acces to an IBMF font
 *
 */
class FontData {

private:
    typedef IBMFFace *IBMFFacePtr;

    bool initialized_{false};
    PreamblePtr preamble_{nullptr};
    IBMFFace faces_[MAX_FACE_COUNT];

    PlanesPtr planes_{nullptr};
    CodePointBundlesPtr codePointBundles_{nullptr};

    MemoryPtr currentFontData_{nullptr};

    GlyphCode unknownGlyphCode_{0};

public:
    FontData(const uint8_t *fontData, uint32_t length) noexcept {

        load(const_cast<MemoryPtr>(fontData), length);
        if (!initialized_) {
            LOGE("Font data not recognized!");
        }
    }

    FontData() = default;
    ~FontData() = default;

    [[nodiscard]] inline auto getFontFormat() const -> FontFormat {
        return (isInitialized()) ? preamble_->bits.fontFormat : FontFormat::UNKNOWN;
    }
    [[nodiscard]] inline auto isInitialized() const -> bool { return initialized_; }

    [[nodiscard]] inline auto getFace(int idx) -> IBMFFacePtr {
        if (isInitialized()) {
            return (idx < preamble_->faceCount) ? &faces_[idx] : &faces_[preamble_->faceCount - 1];
        } else {
            return nullptr;
        }
    }

    [[nodiscard]] inline auto getFaceCount() const -> int {
        return (isInitialized()) ? preamble_->faceCount : 0;
    }

    auto load(MemoryPtr fontData, uint32_t length) -> bool;

    [[nodiscard]] auto translate(char32_t codePoint) const -> GlyphCode;

    void showCodePointBundles(int firstIdx, int count) const;
    void showPlanes() const;
    void showFont() const;
};

#endif