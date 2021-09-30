#pragma once

namespace contourtree {

static constexpr char REGULAR = 0;
static constexpr char MINIMUM = 1;
static constexpr char MAXIMUM = 2;
static constexpr char SADDLE = 4;

// Following the nomenclature of original Carr paper.
// JoinTree -> maxima and SplitTree -> minima
enum class TreeType {JoinTree, SplitTree, ContourTree};

typedef float scalar_t;

}
