list(APPEND test_target_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/CubicGrid_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Field_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WeightedGrid_test.cpp
)

set(test_target_sources ${test_target_sources} PARENT_SCOPE)
