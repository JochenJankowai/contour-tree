#pragma once

namespace contourtree {

const char REGULAR = 0;
const char MINIMUM = 1;
const char MAXIMUM = 2;
const char SADDLE = 4;

// Following the nomenclature of original Carr paper.
// JoinTree -> maxima and SplitTree -> minima
enum class TreeType {JoinTree, SplitTree, ContourTree};

typedef float scalar_t;

}
