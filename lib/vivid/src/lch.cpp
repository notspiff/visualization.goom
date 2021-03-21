#include "vivid/conversion.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

#if __cplusplus <= 201402L
namespace vivid {
namespace lch {
#else
namespace vivid::lch {
#endif


////////////////////////////////////////////////////////////////////////////////
lch_t fromLab( const lab_t& lab )
{
    const float h = glm::degrees( std::atan2( lab.z, lab.y ) );

    lch_t lch;
    lch.x = lab.x;
    lch.y = std::sqrt( lab.y * lab.y + lab.z * lab.z );
    lch.z = ( h >= 0 ) ? h : ( h + 360.f );

    return lch;
}


////////////////////////////////////////////////////////////////////////////////
lch_t fromSrgb( const srgb_t& srgb ) {
    return lch::fromLab( lab::fromXyz( xyz::fromSrgb( srgb ) ) );
}


////////////////////////////////////////////////////////////////////////////////
lch_t fromXyz( const xyz_t& xyz ) {
    return lch::fromLab( lab::fromXyz( xyz ) );
}


#if __cplusplus <= 201402L
} // namespace lch
} // namespace vivid
#else
}   //  ::vivid::lch
#endif
