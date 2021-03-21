#include "vivid/conversion.h"
#include "vivid/data/xterm.h"

#if __cplusplus <= 201402L
namespace vivid {
namespace name {
#else
namespace vivid::name {
#endif


////////////////////////////////////////////////////////////////////////////////
const std::string& fromRgb( const rgb_t& rgb ) {
    return name::fromIndex( index::fromRgb( rgb ) );
}


////////////////////////////////////////////////////////////////////////////////
const std::string& fromIndex( const uint8_t index ) {
    return data::xterm.at( index ).name;
}


#if __cplusplus <= 201402L
} // namespace name
} // namespace vivid
#else
}   //  ::vivid::name
#endif
