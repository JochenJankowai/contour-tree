QT += core

CONFIG += c++11

TARGET = ContourTree
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += ./src/main.cpp \
    ./src/MergeTree.cpp \
    ./src/SimplifyCT.cpp \
    ./src/ContourTreeData.cpp \
    ./src/Persistence.cpp \
    ./src/TriMesh.cpp \
    ./src/TopologicalFeatures.cpp \
    ./src/HyperVolume.cpp \
    ./src/ContourTree.cpp

HEADERS += \
    ./include/DisjointSets.hpp \
    ./include/MergeTree.hpp \
    ./include/ScalarFunction.hpp \
    ./include/Grid3D.hpp \
    ./include/SimplifyCT.hpp \
    ./include/ContourTreeData.hpp \
    ./include/constants.h \
    ./include/SimFunction.hpp \
    ./include/Persistence.hpp \
    ./include/TriMesh.hpp \
    ./include/TopologicalFeatures.hpp \
    ./include/HyperVolume.hpp \
    ./include/ContourTree.hpp

# Unix configuration
unix:!macx{
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS   += -fopenmp
}

win32{
    CONFIG += console
    QMAKE_CXXFLAGS += -openmp
    QMAKE_LFLAGS   += -openmp
}
