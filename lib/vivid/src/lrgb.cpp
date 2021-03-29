#include "vivid/conversion.h"
#include "vivid/utility.h"

#if __cplusplus <= 201402L
namespace vivid {
namespace lrgb {
#else
namespace vivid::lrgb {
#endif


lrgb_t fromSrgb( const srgb_t& srgb )
{
    return {
        srgb::inverseCompound( srgb.x ),
        srgb::inverseCompound( srgb.y ),
        srgb::inverseCompound( srgb.z )
    };
}


#if __cplusplus <= 201402L
} // namespace lrgb
} // namespace vivid
#else
}   //  ::vivid::lrgb
#endif
