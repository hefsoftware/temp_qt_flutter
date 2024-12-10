#include "flutterappopengl.h"
#include <QPixmap>
#include <QDebug>
#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>
#include <QThread>
#include "qopenglfunctions_3_0.h"

FlutterAppOpenGl::FlutterAppOpenGl()
{
}

std::optional<FlutterRendererConfig> FlutterAppOpenGl::rendererConfig()
{
  if(!initialize())
    return {};
  else {
    FlutterRendererConfig renderer = {};
    renderer.type=kOpenGL;
    auto &opengl=renderer.open_gl;
    opengl.struct_size=sizeof(FlutterOpenGLRendererConfig);
    opengl.make_current=[](void *instance) {
      return static_cast<FlutterAppOpenGl*>(instance)->makeCurrent();
    };
    opengl.clear_current=[](void *instance) {
      return static_cast<FlutterAppOpenGl*>(instance)->clearCurrent();
    };
    opengl.present=[](void *instance) {
      return static_cast<FlutterAppOpenGl*>(instance)->present();
     };
    // opengl.fbo_callback=[](void *instance)->uint32_t {
    //   // return static_cast<FlutterAppOpenGl*>(instance)->frameBuffer()->handle();
    // };
    opengl.fbo_with_frame_info_callback=[](void *instance, const FlutterFrameInfo *frameInfo) {
      return static_cast<FlutterAppOpenGl*>(instance)->prepareFrameBuffer(QSize(frameInfo->size.width, frameInfo->size.height));
    };
    // opengl.make_resource_current=[](void *instance)->bool {
    //   return static_cast<FlutterAppOpenGl*>(instance)->makeResourceCurrent();
    // };
    opengl.gl_proc_resolver=[](void *instance, const char *data)->void *{
      return static_cast<FlutterAppOpenGl*>(instance)->procResolver(data);
    };
    return renderer;
  }
}

bool FlutterAppOpenGl::makeCurrent()
{
  qDebug()<<"Make current 2";
  return renderContext().makeCurrent();
}

bool FlutterAppOpenGl::clearCurrent()
{
  qDebug()<<"Clear current";
  return renderContext().clearCurrent();
}

bool FlutterAppOpenGl::makeResourceCurrent()
{
  qDebug()<<"Make resource current";
  return resourceContext().makeCurrent();
}

bool FlutterAppOpenGl::initialize()
{
  return renderContext().initialize() && resourceContext().initialize();
}

void *FlutterAppOpenGl::procResolver(const char *procName)
{
  return renderContext().procAddress(procName);
}

// QOpenGLFramebufferObject *FlutterAppOpenGl::frameBuffer(const FlutterFrameInfo *frameInfo)
// {
//   qDebug()<<"frame buffer"<<m_frameBuffer<<frameInfo->size.width<<frameInfo->size.height;
//   if(!m_frameBuffer) m_frameBuffer=new QOpenGLFramebufferObject(1024, 1024);
//   return m_frameBuffer;
// }

// QOpenGLFramebufferObject *FlutterAppOpenGl::frameBuffer()
// {
//   if(!m_frameBuffer) m_frameBuffer=new QOpenGLFramebufferObject(1024, 1024);
//   return m_frameBuffer;
// }

// #include "qopenglfunctions_2_1.h"
// #include <QThread>
// void testOpenGl(std::function<void (QImage)> postResult)
// {
//   //https://forum.qt.io/topic/84779/how-to-create-a-qoffscreensurface-correctly/3
//   auto offscreenSurface=new QOffscreenSurface{};
//   offscreenSurface->create();
//   auto thread=QThread::create([postResult, offscreenSurface]() {
//     qDebug()<<"Starting thread"<<QThread::currentThread();
//     auto context=new QOpenGLContext{offscreenSurface};
//     context->setFormat(offscreenSurface->requestedFormat());
//     qDebug()<<context->create();
//     qDebug()<<"Make current"<<context->makeCurrent(offscreenSurface);
//     QOpenGLFunctions_2_1 opengl;
//     qDebug()<<"init function"<<opengl.initializeOpenGLFunctions();
//     auto fbo=new QOpenGLFramebufferObject(320, 200);
//     fbo->bind();
//     opengl.glClear(GL_COLOR_BUFFER_BIT
//             | GL_DEPTH_BUFFER_BIT
//             | GL_STENCIL_BUFFER_BIT);

//     opengl.glViewport(0,0,100,100);
//     opengl.glOrtho(0,100,0,100,0,1);

//     opengl.glColor3f(1.0,0.0,1.0);

//     opengl.glBegin(GL_POLYGON);
//     opengl.glVertex2f(10.0f,10.0f);
//     opengl.glVertex2f(90.0f,10.0f);
//     opengl.glVertex2f(90.0f,90.0f);
//     opengl.glVertex2f(90.0f,10.0f);
//     opengl.glEnd();
//     auto image=fbo->toImage();
//     qDebug()<<"Done";
//     postResult(image);
//   });
//   qDebug()<<"Thread"<<thread;
//   offscreenSurface->moveToThread(thread);
//   thread->start();
// }

bool FlutterAppOpenGl::OpenGlContext::initialize()
{
  bool ret=true;
  if(!m_surface) {
    QSurfaceFormat format;
    format.setSwapBehavior(QSurfaceFormat::SingleBuffer);
    m_surface=new QOffscreenSurface{};
    m_surface->setFormat(format);
    m_surface->create();
    ret=m_surface->isValid();
    if(!ret) {
      delete m_surface;
      m_surface=nullptr;
    }
    else
      // Removes the affinity of the surface so we can grab it from flutter thread.
      m_surface->moveToThread(nullptr);
  }
  if(ret && !m_context) {
    auto s=m_surface;
    m_context=new QOpenGLContext{s};
    if(!m_context->create()) {
      delete m_context;
      m_context=nullptr;
      ret=false;
    }
    else {
      // Removes the affinity of the context so we can grab it from flutter thread.
      m_context->moveToThread(nullptr);
    }
  }
  return ret;
  // if(!m_resourceContext) {
  // }
  // return m_resourceContext;
}

bool FlutterAppOpenGl::OpenGlContext::makeCurrent()
{
  bool ret=false;
  if(m_surface && m_context) {
    qDebug()<<"Make current";
    if(!m_thread) {
      m_thread=QThread::currentThread();
      ret=initializeThread();
    }
    else {
      Q_ASSERT(m_thread==QThread::currentThread());
      ret=true;
    }
    if(ret)
      setAsActive();
  }
  qDebug()<<"Make current"<<ret;
  return ret;

}

bool FlutterAppOpenGl::OpenGlContext::clearCurrent()
{
  bool ret=false;
  if(m_context) {
    m_context->doneCurrent();
    ret=true;
  }
  return ret;
}

bool FlutterAppOpenGl::OpenGlContext::destroy() {
  qDebug()<<"Destroy context";
  return true;
}

void *FlutterAppOpenGl::OpenGlContext::procAddress(const char *procName)
{
  void *ret=nullptr;
  if(m_context)
    ret=(void *)m_context->getProcAddress(procName);
  return ret;
}

bool FlutterAppOpenGl::OpenGlContext::initializeThread()
{
  m_surface->moveToThread(m_thread);
  m_context->moveToThread(m_thread);
  return true;
}

bool FlutterAppOpenGl::OpenGlContext::setAsActive()
{
  return m_context->makeCurrent(m_surface);
}

uint32_t FlutterAppOpenGlFrameBuffer::prepareFrameBuffer(QSize renderSize)
{
  renderSize=QSize(qNextPowerOfTwo(renderSize.width()), qNextPowerOfTwo(renderSize.height()));
  if(renderSize!=m_frameBufferSize) {
    delete m_frameBuffer;
    m_frameBuffer=nullptr;
  }
  if(!m_frameBuffer) {
    m_frameBuffer=new QOpenGLFramebufferObject(renderSize);
    m_frameBufferSize=renderSize;
  }
  qDebug()<<"Prepare framebuffer"<<m_frameBuffer->handle();
  return m_frameBuffer->handle();
  // return static_cast<FlutterAppOpenGl*>(instance)->frameBuffer(frameInfo)->handle(); //frameBuffer()->handle();

}

bool FlutterAppOpenGlFrameBuffer::present()
{
  bool ret=false;
  if(m_frameBuffer) {
    auto img=m_frameBuffer->toImage();
    qDebug()<<"Present" <<img;
    render(img);
    ret=true;
  }
  return ret;
}

// uint32_t FlutterAppOpenGlRenderBuffer::prepareFrameBuffer(QSize renderSize)
// {
//   uint32_t ret=0;
//   if(auto context=m_context.context()) {
//     if(!m_initializedContextFunctions) {
//       m_initializedContextFunctions=m_contextFunctions.initializeOpenGLFunctions();
//     }
//     if(m_initializedContextFunctions) {
//       // m_contextFunctions.glC
//       // qDebug()<<"ok";
//       m_context.makeCurrent();
//       // ret=context->defaultFramebufferObject();
//       m_contextFunctions.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_context.context()->defaultFramebufferObject());
//       if(m_renderBuffer==0) {
//         m_contextFunctions.glGenRenderbuffers(1, &m_renderBuffer);
//         if(m_renderBuffer!=0) {
//           // m_contextFunctions.gl_re
//           m_contextFunctions.glBindRenderbuffer( GL_RENDERBUFFER, m_renderBuffer);
//           m_renderSize=renderSize;
//           m_contextFunctions.glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, renderSize.width(), renderSize.height());
//         }
//       }
//       // qDebug()<<renderBuffer;
//       // if(renderBuffer!=GL_INVALID_VALUE )
//     }

//   }
//   return ret;
// }

// bool FlutterAppOpenGlRenderBuffer::present()
// {
//   // m_contextFunctions.glFramebufferRenderbuffer()
//   // GLint drawFboId = 0, readFboId = 0;
//   m_contextFunctions.glBindFramebuffer(GL_READ_FRAMEBUFFER, m_context.context()->defaultFramebufferObject());
//   m_contextFunctions.glBindRenderbuffer( GL_RENDERBUFFER, m_renderBuffer);

//   // m_contextFunctions.glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);
//   // m_contextFunctions.glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId);
//   QImage image(m_renderSize.width(), m_renderSize.height(), QImage::Format_RGBA8888);
//   m_contextFunctions.glReadPixels(0, 0, m_renderSize.width(), m_renderSize.height(), GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, image.data_ptr());
//   render(image);
//   qDebug()<<"Present"<<m_renderBuffer;
//   return true;
// }

// bool FlutterAppOpenGlRenderBuffer::initialize()
// {
//   bool ret=FlutterAppOpenGl::initialize();
//   if(ret) {
//     m_contextFunctions=QOpenGLFunctions_3_0{}
//   }
//   return ret;
// }

