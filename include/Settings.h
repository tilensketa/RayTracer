#pragma once

enum ViewportMode { Flat = 0, Shaded, Wireframe };

struct Settings {
  int mMaxDepth = 10;
  ViewportMode mViewportMode = ViewportMode::Shaded;
  int mDownsampleFactor = 1;
};
