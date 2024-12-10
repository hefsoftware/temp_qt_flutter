#pragma once
#include "flutterapp.h"
// #include "QOpenGLContext"
#include <QOpenGLFunctions_3_0>

class QOffscreenSurface;
class QOpenGLContext;
class QOpenGLFramebufferObject;
class FlutterAppOpenGl: public FlutterApp
{
protected:
  class OpenGlContext {
  public:
    /**
     * @brief Initializes the resources of opengl context.
     *
     * This function must be called from main thread.
     * @return True on success
     */
    virtual bool initialize();
    /**
     * @brief makeCurrent Selects the context for usage
     *
     * Will be called inside the thread that will actually use the context
     * @return True on success
     */
    virtual bool makeCurrent();
    /**
     * @brief Deselect the context for usage
     * @return True on success
     */
    virtual bool clearCurrent();
    // TODO implement
    virtual bool destroy();
    /**
     * @brief Returns the pointer to an openGl function.
     * @param Name of the function to get.
     * @return A pointer to the function or null on error.
     */
    void *procAddress(const char *procName);
    // QOpenGLContext *context() { return m_context; }
  protected:
    /**
     * @brief Initialize the context before its usage in a thread.
     * @return True on success
     */
    virtual bool initializeThread();
    /**
     * @brief Sets the context as active in the current thread
     * @return True on success
     */
    virtual bool setAsActive();
  private:
    QOffscreenSurface *m_surface=nullptr;
    QOpenGLContext *m_context=nullptr;
    QThread *m_thread=nullptr;
  };
  template <typename T> requires (std::derived_from<T, QAbstractOpenGLFunctions>) class OpenGlContextFunctionsBase: public OpenGlContext, public T {
  };
  template <typename T> class OpenGlContextFunctions: public OpenGlContextFunctionsBase<T> {
    protected:
    bool initializeThread() override {
      bool ret;
      ret=FlutterAppOpenGl::OpenGlContext::initializeThread();
      ret=ret && FlutterAppOpenGl::OpenGlContext::setAsActive();
      ret=ret && this->initializeOpenGLFunctions();
      return ret;
    }
  };


public:
  explicit FlutterAppOpenGl();
protected:
  std::optional<FlutterRendererConfig> rendererConfig() override;
  virtual bool initialize();
  virtual uint32_t prepareFrameBuffer(QSize renderSize)=0;
  virtual bool render(const QImage &image)=0;
  virtual bool present()=0;
  virtual OpenGlContext &renderContext()=0;
  virtual OpenGlContext &resourceContext()=0;
private:
  bool makeCurrent();
  bool clearCurrent();
  bool makeResourceCurrent();
  void *procResolver(const char *procName);
  // QOpenGLFramebufferObject *frameBuffer(const FlutterFrameInfo *frameInfo);
};


class FlutterAppOpenGlFrameBuffer: public FlutterAppOpenGl {
protected:
  uint32_t prepareFrameBuffer(QSize renderSize) override;
  bool present() override;
private:
  QSize m_frameBufferSize={};
  QOpenGLFramebufferObject *m_frameBuffer=nullptr;
};

void testOpenGl(std::function<void(QImage)> postResult);
