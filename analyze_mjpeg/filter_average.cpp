#include "StdAfx.h"

#include "filter_average.h"

using namespace std;

// Get the average value for each pixel in the image.

FilterAverage::FilterAverage() : is_initialized_(false)
{
}


void FilterAverage::init(u32 x, u32 y, u32 bytes_per_pixel)
{
  is_initialized_ = true;
  n_sums_ = 0;
  x_ = x;
  y_ = y;
  bytes_per_pixel_ = bytes_per_pixel;
  sums_ = new u64[x_ * y_ * bytes_per_pixel_]();
}


FilterAverage::~FilterAverage()
{
  delete[] sums_;
}


bool FilterAverage::is_initialized() {
  return is_initialized_;
}


void FilterAverage::add(const Image& image)
{
  u64* sums_ptr(sums_);
  const u8* image_ptr(&image.image_data[0]);

  for (u32 y(0); y < image.h; ++y) {
    for (u32 x(0); x < image.w; ++x) {

      u8 r(*image_ptr++);
      u8 g(*image_ptr++);
      u8 b(*image_ptr++);

      *sums_ptr++ += r;
      *sums_ptr++ += g;
      *sums_ptr++ += b;
    }
  }
  ++n_sums_;

  cout << "Averages: " << n_sums_ << endl;
}


Image FilterAverage::get()
{
  Image m;
  m.w = x_;
  m.h = y_;

  u64* sums_ptr(sums_);

  ImageData img(x_ * y_ * bytes_per_pixel_);
  for (u32 i(0); i < x_ * y_ * bytes_per_pixel_; ++i) {
    u64 t(*sums_ptr++);
    img[i] = static_cast<u8>(t / n_sums_);
  }
  m.image_data = img;
  m.bytes_per_pixel = bytes_per_pixel_;

  return m;
}
