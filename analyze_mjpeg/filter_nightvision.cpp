#include "StdAfx.h"

#include "filter_nightvision.h"

using namespace std;

FilterNightVision::FilterNightVision() : is_initialized_(false)
{
}


void FilterNightVision::init(u32 x, u32 y, u32 bytes_per_pixel)
{
  is_initialized_ = true;
  n_sums_ = 0;
  x_ = x;
  y_ = y;
  bytes_per_pixel_ = bytes_per_pixel;
  sum_ = new float[x_ * y_ * bytes_per_pixel_]();
}


FilterNightVision::~FilterNightVision()
{
  delete[] sum_;
}


bool FilterNightVision::is_initialized() {
  return is_initialized_;
}


void FilterNightVision::add(const Image& image)
{
  float* sum_ptr(sum_);
  const u8* image_ptr(&image.image_data[0]); // + x * bytes_per_pixel_ + y * image.w * bytes_per_pixel_]);

  //float high(255.0f);
  //float low(0.0f);

  float avg(0.0f);

  for (u32 y(0); y < image.h; ++y) {
    for (u32 x(0); x < image.w; ++x) {

      u8 r(*image_ptr++);
      u8 g(*image_ptr++);
      u8 b(*image_ptr++);

      //if (g < low) {
      //  low = g;
      //}

      //if (g > high) {
      //  high = g;
      //}

      *sum_ptr++ += static_cast<float>(r);
      *sum_ptr++ += static_cast<float>(g);
      *sum_ptr++ += static_cast<float>(b);

      avg += g;
    }
  }
  ++n_sums_;

  avg /= image.h * image.w;

  cout << "avg: " << avg << endl;

  low_ = avg - 10.0f;
  high_ = avg + 10.0f;

  //high_ = 60.0f;

  cout << "Averages: " << n_sums_ << endl;
  cout << "low: " << low_ << endl;
  cout << "high: " << high_ << endl;
}


Image FilterNightVision::get()
{
  Image m;
  m.w = x_;
  m.h = y_;

  float* sum_ptr(sum_);

  ImageData img(x_ * y_ * bytes_per_pixel_);
  for (u32 i(0); i < x_ * y_ * bytes_per_pixel_; ++i) {
    float t(*sum_ptr++);
    float v(t / n_sums_);
    float normalized_v(normalized_range(low_, high_, v));
    float screen_v(normalized_v * 255.0f);
    if (screen_v < 0) {
      screen_v = 0;
    }
    if (screen_v > 255.0f) {
      screen_v = 255.0f;
    }
    img[i] = static_cast<u8>(screen_v);
  }
  m.image_data = img;
  m.bytes_per_pixel = bytes_per_pixel_;

  return m;
}
