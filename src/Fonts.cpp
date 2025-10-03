#include <array>
#include <cinttypes>

#include "FontEntry.hpp"

#if CONFIG_FONT_IBMF

#include "IBMFDriver/IBMFFont.hpp"
#include "IBMFDriver/IBMFFontData.hpp"
#include "IBMFFonts/SolSans_75.h"

std::array<FontEntry, 1> fontsList{{
    {"SolSans", SOLSANS_75_IBMF, SOLSANS_75_IBMF_LEN}
}};

FontData mainFont = FontData(fontsList[0].content, fontsList[0].length);

#endif

#if CONFIG_FONT_TTF

#include "TTFDriver/TTFFont.hpp"
#include "TTFDriver/TTFNotoSansLight.hpp"

TTFNotoSansLight mainFont;

#endif
