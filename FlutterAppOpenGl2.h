#pragma once
#include "flutterappopengl.h"

class FlutterAppOpenGlRenderBuffer: public FlutterAppOpenGl {
  struct OpenGlContextRender: public OpenGlContextFunctions<QOpenGLFunctions_3_0> {
    GLuint frameBuffer() { return m_frameBuffer; }

    // OpenGlContext interface
    protected:
    bool initializeThread() override;
    private:
    GLuint m_frameBuffer=0;
  };
public:

private:
  OpenGlContextRender m_renderContext;
  OpenGlContext m_resourceContext;

  // FlutterAppOpenGl interface
  protected:
  uint32_t prepareFrameBuffer(QSize renderSize) override;
  bool render(const QImage &image) override;
  bool present() override;
  OpenGlContext &renderContext() override;
  OpenGlContext &resourceContext() override;
  GLuint m_renderBuffer=0;
  QSize m_renderBufferSize={};
  std::optional<FlutterRendererConfig> rendererConfig() override;
};
