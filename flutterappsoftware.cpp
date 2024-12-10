#include "flutterappsoftware.h"
#include <QPixmap>
#include <QDebug>

FlutterAppSoftware::FlutterAppSoftware()
{

}

bool FlutterAppSoftware::render(const QImage &image)
{
  qDebug()<<"Render";
  return true;
}

std::optional<FlutterRendererConfig> FlutterAppSoftware::rendererConfig()
{
  FlutterRendererConfig renderer = {};
  renderer.type = kSoftware;
  renderer.software.struct_size = sizeof(FlutterSoftwareRendererConfig);
  renderer.software.surface_present_callback = [](void * instance,const void* allocation,
                                                  size_t row_bytes,
                                                  size_t height) {
    auto image=QImage(static_cast<const unsigned char *>(allocation), row_bytes/sizeof(uint32_t), height, row_bytes, QImage::Format_ARGB32);
    return static_cast<FlutterAppSoftware*>(instance)->render(image.copy());
  };
  return renderer;
}
