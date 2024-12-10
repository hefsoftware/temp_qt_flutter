#pragma once
#include "flutterapp.h"

class FlutterAppSoftware: public FlutterApp
{
public:
  explicit FlutterAppSoftware();
protected:
  virtual bool render(const QImage &image);
  std::optional<FlutterRendererConfig> rendererConfig() override;
};
