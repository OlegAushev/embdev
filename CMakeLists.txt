# Header-only for now: INTERFACE rather than OBJECT, which cannot be given an
# empty source list. The first .cpp makes this an OBJECT target with
# `PRIVATE build_config` attached by the consuming project, as emblib is.
add_library(embdev INTERFACE)

# PUBLIC would be an error on an INTERFACE target; this propagates
# `#include <emb/dev/...>` to consumers all the same.
target_include_directories(embdev INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR})
