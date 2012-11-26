#pragma once

#include "stdafx.h"

class FilterAverage
{
public:
  FilterAverage();
  ~FilterAverage();
  bool is_initialized();
  void init(u32 x_, u32 y_, u32 bytes_per_pixel_);
  void add(const Image& image);
  Image get();
private:
  bool is_initialized_;

  u32 x_;
  u32 y_;
  u32 bytes_per_pixel_;

  u64* sums_;
  u64 n_sums_;
};

