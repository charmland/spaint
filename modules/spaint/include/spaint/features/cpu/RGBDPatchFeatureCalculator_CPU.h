/**
 * spaint: RGBDPatchFeatureCalculator_CPU.h
 * Copyright (c) Torr Vision Group, University of Oxford, 2016. All rights reserved.
 */

#ifndef H_SPAINT_RGBDPATCHFEATURECALCULATORCPU
#define H_SPAINT_RGBDPATCHFEATURECALCULATORCPU

#include "../interface/RGBDPatchFeatureCalculator.h"

namespace spaint
{
class RGBDPatchFeatureCalculator_CPU: public RGBDPatchFeatureCalculator
{
public:
  RGBDPatchFeatureCalculator_CPU();

  virtual void ComputeFeature(const ITMUChar4Image *rgb_image,
      const ITMFloatImage *depth_image, const Vector4f &intrinsics,
      RGBDPatchFeatureImage *features_image, const Matrix4f &cameraPose) const;
};
}

#endif