#include "goom_graphic.h"

namespace GOOM
{

const Pixel Pixel::BLACK{/*.channels*/ {/*.r = */ 0, /*.g = */ 0, /*.b = */ 0,
                                        /*.a = */ MAX_COLOR_VAL}};

const Pixel Pixel::WHITE{/*.channels*/ {/*.r = */ MAX_COLOR_VAL, /*.g = */ MAX_COLOR_VAL,
                                        /*.b = */ MAX_COLOR_VAL, /*.a = */ MAX_COLOR_VAL}};

} // namespace GOOM
