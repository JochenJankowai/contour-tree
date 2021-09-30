cmake_minimum_required(VERSION 3.1)

project(harishcontourtree)

find_package(Qt5Core REQUIRED)
find_package(OpenMP REQUIRED)

set (CMAKE_CXX_STANDARD 17)

# Add header files
set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/constants.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ContourTree.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ContourTreeData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/DisjointSets.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Grid3D.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/HyperVolume.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/MergeTree.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Persistence.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ScalarFunction.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SimFunction.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SimplifyCT.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/TopologicalFeatures.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/TriMesh.hpp
)

# Add source files
set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ContourTree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ContourTreeData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperVolume.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MergeTree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Persistence.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SimplifyCT.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TopologicalFeatures.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TriMesh.cpp
)

# Create library
add_library(harishcontourtree STATIC ${SOURCE_FILES} ${HEADER_FILES})

ivw_define_standard_definitions(harishcontourtree harishcontourtree)
ivw_suppress_compiler_warnings(harishcontourtree)

target_include_directories(harishcontourtree PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

ivw_folder(harishcontourtree ext)

ivw_make_package(HarishContourTree harishcontourtree)
ivw_default_install_targets(harishcontourtree)