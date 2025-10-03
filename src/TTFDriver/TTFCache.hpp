#pragma once

#if CONFIG_TINYFONT_TTF

#include <memory>
#include <optional>

#include "../FontDefs.hpp"
#include "../Misc/SpiramAllocator.hpp"
#include "TTFDefs.hpp"

using namespace font_defs;

class Font;

class TTFCache {
private:
#if CONFIG_TINYFONT_USE_SPIRAM
    template <typename K, typename V>
    using SpiramMap = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>,
                                         FontSpiramAllocator<std::pair<const K, V>>>;

    SpiramMap<uint32_t, std::shared_ptr<Glyph>> glyphCache_;
#else
    std::unordered_map<uint32_t, std::shared_ptr<Glyph>> glyphCache_;
#endif
    uint32_t hitCount_ = 0;
    uint32_t missCount_ = 0;

    auto doGetGlyph(Font &font, GlyphCode glyphCode, uint32_t key) -> std::optional<const Glyph *>;

public:
    TTFCache() = default;

    ~TTFCache() {
        showStats();
        clear();
    }

    inline auto getGlyph(Font &font, GlyphCode glyphCode, int16_t charSize)
        -> std::optional<const Glyph *> {

        auto key = static_cast<uint32_t>(static_cast<uint32_t>(charSize << 16) | glyphCode);
        auto it = glyphCache_.find(key);
        if (it != glyphCache_.end()) {
            // log_d("hit(%" PRIu16 ") -> %p!", glyphCode, (void *)it->second);
            hitCount_++;
            return it->second.get();
        }

        return doGetGlyph(font, glyphCode, key);
    }

    void clear();
    void showStats() const;

    void showBitmap(const Bitmap &bitmap, bool inverted, PixelResolution pixelResolution) const;

    void showGlyph(const Glyph &glyph, bool displayBitmap, PixelResolution pixelResolution) const;
};

#endif