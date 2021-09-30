cmake_minimum_required(VERSION 3.1)

project(harishcontourtree)

find_package(Qt5Core REQUIRED)
find_package(OpenMP REQUIRED)

set (CMAKE_CXX_STANDARD 17)

# Add header files
set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/constants.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ContourTree.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ContourTreeData.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/DisjointSets.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Grid3D.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/HyperVolume.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/MergeTree.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Persistence.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ScalarFunction.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SimFunction.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SimplifyCT.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/TopologicalFeatures.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/TriMesh.h
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