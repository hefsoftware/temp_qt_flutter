#include "FlutterAppOpenGl2.h"
#include <QImage>
uint32_t FlutterAppOpenGlRenderBuffer::prepareFrameBuffer(QSize renderSize)
{
  uint32_t ret=0;
  qDebug()<<"Prepare frame buffer"<<renderSize;
  auto getInt=[this](GLenum value) { GLint ret; m_renderContext.glGetIntegerv(value, &ret); return ret; };

  if(m_renderBuffer==0) {
    qDebug()<<"Create render buffer";
    qDebug()<<"FrameBuffer"<<getInt(GL_FRAMEBUFFER_BINDING);
    m_renderContext.glGenRenderbuffers(1, &m_renderBuffer);
    if(m_renderBuffer!=0) {
      m_renderContext.glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
      m_renderContext.glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, renderSize.width(), renderSize.height());
      m_renderContext.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_renderBuffer);
      m_renderBufferSize=renderSize;
    }
  }
  return m_renderContext.frameBuffer();
}

bool FlutterAppOpenGlRenderBuffer::render(const QImage &image)
{
  qDebug()<<"Render";
  return true;
}

bool FlutterAppOpenGlRenderBuffer::present()
{
  if(!m_renderBuffer)
    qDebug()<<"**** Present with an invalid render buffer";
  else {
    qDebug()<<"Present"<<m_renderBufferSize;
    QImage image(m_renderBufferSize.width(), m_renderBufferSize.height(), QImage::Format_RGBA8888);
    // uint8_t data[10000];
    // memset(data, 'x', sizeof(data));
    // // printEnv();
    // qDebug()<<image.data_ptr();
    // opengl.glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);
    m_renderContext.glReadBuffer(GL_COLOR_ATTACHMENT0);
    m_renderContext.glReadPixels(0, 0, m_renderBufferSize.width(), m_renderBufferSize.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    qDebug()<<"Present result"<<m_renderContext.glGetError();
    render(image);
  }
  return true;
}

FlutterAppOpenGl::OpenGlContext &FlutterAppOpenGlRenderBuffer::renderContext()
{
  return m_renderContext;
}

FlutterAppOpenGl::OpenGlContext &FlutterAppOpenGlRenderBuffer::resourceContext()
{
  return m_resourceContext;
}


bool FlutterAppOpenGlRenderBuffer::OpenGlContextRender::initializeThread()
{
  bool ret=OpenGlContextFunctions<QOpenGLFunctions_3_0>::initializeThread();
  qDebug()<<"Init functions"<<ret;
  if(ret && m_frameBuffer==0) {
    glGenFramebuffers(1, &m_frameBuffer);
    if(m_frameBuffer==0)
      ret=false;
    qDebug()<<"Gen functions"<<ret;
  }
  if(ret) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    ret=(glGetError()==GL_NO_ERROR);
    qDebug()<<"Bind functions"<<ret;
  }
  qDebug()<<"Initialize thread"<<ret;
  return ret;
}


std::optional<FlutterRendererConfig> FlutterAppOpenGlRenderBuffer::rendererConfig()
{
  auto ret=FlutterAppOpenGl::rendererConfig();
  if(ret) {
    // ret->open_gl.surface_transformation=[](void *) {
    //   qDebug()<<"Transform";
    //   FlutterTransformation ret={};
    //   ret.scaleX=1;
    //   ret.scaleY=1;
    //   ret.pers0=0.01;
    //   ret.pers1=0.01;
    //   // ret.transY=50;
    //   return ret;
    // };
  }
  return ret;
}
