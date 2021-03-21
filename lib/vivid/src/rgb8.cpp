#include "vivid/conversion.h"
#include "vivid/data/xterm.h"
#include <glm/common.hpp>

#if __cplusplus <= 201402L
namespace vivid {
namespace rgb8 {
#else
namespace vivid::rgb8 {
#endif


////////////////////////////////////////////////////////////////////////////////
col8_t fromRgb( const rgb_t& rgb ) {
    return glm::clamp( 255.f * rgb, 0.f, 255.f );
}


////////////////////////////////////////////////////////////////////////////////
col8_t fromIndex( const uint8_t index ) {
    return rgb8::fromRgb32( data::xterm.at( index ).rgb32 );
}


////////////////////////////////////////////////////////////////////////////////
col8_t fromRgb32( const uint32_t rgb32 )
{
    const uint8_t r = ( rgb32 >> 16 ) & 0xff;
    const uint8_t g = ( rgb32 >> 8 ) & 0xff;
    const uint8_t b = rgb32 & 0xff;

    return { r, g, b };
}


////////////////////////////////////////////////////////////////////////////////
col8_t fromHex( const std::string& hexStr ) {
    return rgb8::fromRgb32( rgb32::fromHex( hexStr ) );
}

#if __cplusplus > 201402L
////////////////////////////////////////////////////////////////////////////////
col8_t fromName( const std::string& name ) {
    return rgb8::fromIndex( index::fromName( name ).value_or( 0 ) );
}
#endif


#if __cplusplus <= 201402L
} // namespace rgb8
} // namespace vivid
#else
}   //  ::vivid::rgb8
#endif
