#pragma once 

#include "vivid/types.h" 
#include <vector> 

#if __cplusplus <= 201402L
namespace vivid {
namespace data {
#else
namespace vivid::data {
#endif


static const std::vector<srgb_t> vivid = 
{ 
    { 0.45f, 0.36f, 0.69f },
    { 0.f, 0.64f, 0.94f },
    { 0.42f, 0.71f, 0.24f },
    { 0.91f, 0.62f, 0.25f },
    { 0.99f, 0.25f, 0.52f }
};


#if __cplusplus <= 201402L
} // namespace data
} // namespace vivid
#else
}   //  ::vivid::data
#endif
