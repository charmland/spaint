/**
 * spaint: GPUForest_Shared.h
 * Copyright (c) Torr Vision Group, University of Oxford, 2016. All rights reserved.
 */

#ifndef H_SPAINT_GPUFORESTSHARED
#define H_SPAINT_GPUFORESTSHARED

namespace spaint
{
_CPU_AND_GPU_CODE_
inline void evaluate_forest_shared(const GPUForestNode* forestTexture,
    const RGBDPatchFeature* featureData, const Vector2i &imgSize,
    GPUForest::LeafIndices* leafData, int x, int y)
{
  const int linear_feature_idx = y * imgSize.width + x;
  const RGBDPatchFeature &currentFeature = featureData[linear_feature_idx];

  for (int treeIdx = 0; treeIdx < GPUForest::NTREES; ++treeIdx)
  {
    int currentNodeIdx = 0;
    GPUForestNode node = forestTexture[currentNodeIdx * GPUForest::NTREES
        + treeIdx];
    bool isLeaf = node.leafIdx >= 0;

    while (!isLeaf)
    {
      // Evaluate feature
      currentNodeIdx = node.leftChildIdx
          + (currentFeature.data[node.featureIdx] > node.featureThreshold);

      // Update node
      node = forestTexture[currentNodeIdx * GPUForest::NTREES + treeIdx];
      isLeaf = node.leafIdx >= 0; // a bit redundant but clearer
    }

    leafData[linear_feature_idx][treeIdx] = node.leafIdx;
  }
}

_CPU_AND_GPU_CODE_
inline void get_prediction_for_leaf_shared(
    const GPUForestPrediction* leafPredictions,
    const GPUForest::LeafIndices* leafIndices,
    GPUForestPrediction* outPredictions, Vector2i imgSize, int x, int y)
{
  const int linearIdx = y * imgSize.width + x;
  const GPUForest::LeafIndices selectedLeaves = leafIndices[linearIdx];

  // Setup the indices of the selected mode for each prediction
  int treeModeIdx[GPUForest::NTREES];
  for (int treeIdx = 0; treeIdx < GPUForest::NTREES; ++treeIdx)
  {
    treeModeIdx[treeIdx] = 0;
  }

  // TODO: maybe copying them is faster...
  const GPUForestPrediction *selectedPredictions[GPUForest::NTREES];
  for (int treeIdx = 0; treeIdx < GPUForest::NTREES; ++treeIdx)
  {
    selectedPredictions[treeIdx] = &leafPredictions[selectedLeaves[treeIdx]];
  }

  GPUForestPrediction &outPrediction = outPredictions[linearIdx];
  outPrediction.nbModes = 0;

  // Merge first MAX_MODES from the sorted mode arrays
  while (outPrediction.nbModes < GPUForestPrediction::MAX_MODES)
  {
    int bestTreeIdx = 0;
    int bestTreeNbInliers = 0;

    // Find the tree with most inliers
    for (int treeIdx = 0; treeIdx < GPUForest::NTREES; ++treeIdx)
    {
      if (selectedPredictions[treeIdx]->nbModes > treeModeIdx[treeIdx]
          && selectedPredictions[treeIdx]->modes[treeModeIdx[treeIdx]].nbInliers
              > bestTreeNbInliers)
      {
        bestTreeIdx = treeIdx;
        bestTreeNbInliers =
            selectedPredictions[treeIdx]->modes[treeModeIdx[treeIdx]].nbInliers;
      }
    }

    if (bestTreeNbInliers == 0)
    {
      // No more modes
      break;
    }

    // Copy its mode into the output array, increment its index
    outPrediction.modes[outPrediction.nbModes] =
        selectedPredictions[bestTreeIdx]->modes[treeModeIdx[bestTreeIdx]];
    outPrediction.nbModes++;
    treeModeIdx[bestTreeIdx]++;
  }
}
}

#endif