list(APPEND python_bindings_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/ONVPath_bindings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SpinResolvedONVBasis_bindings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SpinResolvedONV_bindings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SeniorityZeroONVBasis_bindings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SpinUnresolvedONVBasis_bindings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SpinUnresolvedONV_bindings.cpp
)

set(python_bindings_sources ${python_bindings_sources} PARENT_SCOPE)
