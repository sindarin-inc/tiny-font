#pragma once

#include "Font.hpp"
#include "FontEntry.hpp"

#if CONFIG_FONT_IBMF
#include "IBMFDriver/IBMFFont.hpp"
#include "IBMFDriver/IBMFFontData.hpp"

// For the IBMF Fonts, a single file contains multiple "face" sizes. A single
// object is created using the IBMFFontData class that manage the whole .h file
// and "faces" are hooked through the IBMFFont class.

// All the fonts are instanciated in Fonts.cpp

extern FontData ibmf75;

#endif

#if CONFIG_FONT_TTF
#include "TTFDriver/TTFFont.hpp"
#include "TTFDriver/TTFNotoSansLight.hpp"
#include "TTFFonts/NotoSans-Light.h"

// For the IBMF Fonts, a single file contains multiple "face" sizes. A single
// object is created using the IBMFFontData class that manage the whole .h file
// and "faces" are hooked through the IBMFFont class.

// All the fonts are instanciated in Fonts.cpp

extern TTFNotoSansLight mainFont;

#endif
