
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
        "${PROJECT_SOURCE_DIR}/src/config.h.in"
        "${PROJECT_BINARY_DIR}/src/config.h"
)


set(EXTERN_HEADER_FILES result.h catch2.h)
set(UTILITIES_HEADER_FILES geometry.h tsp.h spatial_index.h runner.h value_selection.h disjoint-set.h graph.h)

add_subdirectory (extern)
add_subdirectory (utilities)
add_subdirectory (propagators)
add_subdirectory (models)
add_subdirectory (programs)
