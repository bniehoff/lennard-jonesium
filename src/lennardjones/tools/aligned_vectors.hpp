/**
 * Here we just define some typedefs.
 */

#ifndef LJ_ALIGNED_VECTORS_HPP
#define LJ_ALIGNED_VECTORS_HPP

#include <unsupported/Eigen/AlignedVector3>

typedef Eigen::AlignedVector3<double> AlignedVector3d;

typedef Eigen::AlignedVector3<float> AlignedVector3f;

typedef Eigen::AlignedVector3<int> AlignedVector3i;

#endif
