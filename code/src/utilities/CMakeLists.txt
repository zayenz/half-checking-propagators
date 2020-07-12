add_library(IPUtilitiesLib tsp.cpp graph.cpp)

target_sources(IPUtilitiesLib INTERFACE ${UTILITIES_HEADER_FILES})
