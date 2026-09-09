file(GLOB_RECURSE sources CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/emb/*.cpp)

add_library(embdev OBJECT ${sources})

# PUBLIC propagates `#include <emb/dev/...>` to consumers.
target_include_directories(embdev PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})
