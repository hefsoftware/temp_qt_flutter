#pragma once
#include <QObject>
#include <QSize>
#include "flutter_engine/flutter_embedder.h"
#include <mutex>

class FlutterApp
{
public:
  explicit FlutterApp();
  void setSize(QSize size);
  bool start(QString assetPath);
  void cursorPressEvent(QPointF pos);
  void cursorReleaseEvent(QPointF pos);
protected:
  virtual void log(const QString &message);
  virtual std::optional<FlutterRendererConfig> rendererConfig()=0;
  void setWindowSizeInternal();
  std::mutex m_mutex;
  QSize m_size;
  FlutterEngine m_engine=nullptr;
};
