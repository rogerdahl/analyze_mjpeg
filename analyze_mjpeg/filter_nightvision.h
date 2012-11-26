#pragma once

#include "stdafx.h"


// Given a range from a to b, find the position of t as a value between 0 and 1.
template<typename T>
T normalized_range(T a, T b, T t) {
  return (t - a) / (b - a);
}


class FilterNightVision
{
public:
  FilterNightVision();
  ~FilterNightVision();
  bool is_initialized();
  void init(u32 x_, u32 y_, u32 bytes_per_pixel_);
  void add(const Image& image);
  Image get();

private:
  bool is_initialized_;

  u32 x_;
  u32 y_;
  u32 bytes_per_pixel_;

  float* sum_;
  float high_;
  float low_;
  u64 n_sums_;
};

