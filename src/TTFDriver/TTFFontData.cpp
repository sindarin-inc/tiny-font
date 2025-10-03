#if CONFIG_FONT_TTF

#include "TTFFontData.hpp"

#include <freetype/ftmodapi.h>

FT_Library FontData::library = nullptr;

static auto myFtAlloc(FT_Memory memory, long size) -> void * {
#if CONFIG_USE_SPIRAM
    return heap_caps_malloc(static_cast<size_t>(size), MALLOC_CAP_SPIRAM);
#else
    return malloc(static_cast<size_t>(size));
#endif
}

static auto myFtRealloc(FT_Memory memory, long currSize, long newSize, void *block) -> void * {
#if CONFIG_USE_SPIRAM
    return heap_caps_realloc(block, static_cast<size_t>(newSize), MALLOC_CAP_SPIRAM);
#else
    return realloc(block, static_cast<size_t>(newSize));
#endif
}

static void myFtFree(FT_Memory memory, void *block) {
#if CONFIG_USE_SPIRAM
    heap_caps_free(block);
#else
    free(block);
#endif
}

static FT_MemoryRec_ ftMemory = {
    nullptr,
    myFtAlloc,
    myFtFree,
    myFtRealloc,
};

auto FontData::load() -> bool {

    initialized_ = false;

    if (library == nullptr) {
        FT_Error error = FT_New_Library(&ftMemory, &library);
        if (error) {
            LOGE("An error occurred during FreeType library initialization.");
        }
        FT_Add_Default_Modules(library);
    }

    initialized_ = true;

    return true;
}

#endif