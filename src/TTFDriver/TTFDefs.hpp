#pragma once

#if CONFIG_FONT_TTF

#include "../FontDefs.hpp"

const constexpr bool TTF_TRACING = false;

namespace ttf_defs {

const constexpr int SCREEN_RES_PER_INCH = CONFIG_TTF_SCREEN_RES_PER_INCH;
const constexpr int SUP_SUB_FONT_DOWNSIZING = 2;

using namespace font_defs;

} // namespace ttf_defs

#endif
