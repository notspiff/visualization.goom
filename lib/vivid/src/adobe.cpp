#include "vivid/conversion.h"
#include "vivid/utility.h"
#include "vivid/profiles.h"

#if __cplusplus <= 201402L
namespace vivid {
namespace adobe {
#else
namespace vivid::adobe {
#endif


////////////////////////////////////////////////////////////////////////////////
adobe_t fromSrgb( const srgb_t& srgb ) {
    return adobe::fromXyz( xyz::fromSrgb( srgb ) );
}


////////////////////////////////////////////////////////////////////////////////
adobe_t fromXyz( const xyz_t& xyz )
{
    const auto lrgb = static_cast<lrgb_t>( matrices::xyz_to_adobe * xyz );

    return static_cast<adobe_t>(
        rgb::gamma( lrgb, profiles::gamma_adobe )
    );
}


#if __cplusplus <= 201402L
} // namespace adobe
} // namespace vivid
#else
}   //  ::vivid::adobe
#endif
