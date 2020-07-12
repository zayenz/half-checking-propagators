if (APPLE)
    set (HC_LINK_LIBRARIES ${GECODE} IPUtilitiesLib IPModelsLib IPPropagatorsLib)
else()
    set (HC_LINK_LIBRARIES ${GECODE_LIBRARIES} IPUtilitiesLib IPModelsLib IPPropagatorsLib)
endif()

add_executable(tsp-main tsp_main.cpp)
target_link_libraries (tsp-main ${HC_LINK_LIBRARIES} IPUtilitiesLib IPModelsLib IPPropagatorsLib)

add_executable(tsp-prop-amount tsp_prop_amount.cpp)
target_link_libraries (tsp-prop-amount ${HC_LINK_LIBRARIES} IPUtilitiesLib IPModelsLib IPPropagatorsLib)
