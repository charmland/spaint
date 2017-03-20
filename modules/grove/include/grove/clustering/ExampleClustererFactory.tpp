/**
 * grove: ExampleClustererFactory.tpp
 * Copyright (c) Torr Vision Group, University of Oxford, 2017. All rights reserved.
 */

#include "ExampleClustererFactory.h"

#include "cpu/ExampleClusterer_CPU.h"

#ifdef WITH_CUDA
#include "cuda/ExampleClusterer_CUDA.h"
#endif

namespace grove {

template <typename ExampleType, typename ClusterType>
typename ExampleClustererFactory<ExampleType, ClusterType>::Clusterer_Ptr ExampleClustererFactory<ExampleType, ClusterType>::make_clusterer(
    ITMLib::ITMLibSettings::DeviceType deviceType, float sigma, float tau, uint32_t maxClusterCount, uint32_t minClusterSize)
{
  Clusterer_Ptr clusterer;

  if (deviceType == ITMLib::ITMLibSettings::DEVICE_CUDA)
  {
#ifdef WITH_CUDA
    clusterer.reset(
        new ExampleClusterer_CUDA<ExampleType, ClusterType>(sigma, tau, maxClusterCount, minClusterSize));
#else
    throw std::runtime_error("Error: CUDA support not currently available. Reconfigure in CMake with the WITH_CUDA option set to on.");
#endif
  }
  else
  {
    clusterer.reset(
        new ExampleClusterer_CPU<ExampleType, ClusterType>(sigma, tau, maxClusterCount, minClusterSize));
  }

  return clusterer;
}

}
