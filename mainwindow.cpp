#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "flutterapp.h"
#include "flutterappopengl.h"
#include <QThread>

MainWindow::MainWindow(const QString &script, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  // testOpenGl();
  ui->flutter->start(script);
  ui->flutterSw->start(script);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::render(QPixmap px)
{
  qDebug()<<"Render window"<<QThread::currentThread();
  // ui->img->setPixmap(px);
}
#include <QOffscreenSurface>
void MainWindow::testOpenGl()
{
  qDebug()<<"Start";

  auto offscreenSurface=new QOffscreenSurface{};
  offscreenSurface->create();
  Q_ASSERT(offscreenSurface->isValid());
  qDebug()<<"Offscreen";
  auto context=new QOpenGLContext{offscreenSurface};
  context->setFormat(offscreenSurface->format());
  Q_ASSERT(context->create());
  context->makeCurrent(offscreenSurface);
  qDebug()<<"Made current";
  QOpenGLFunctions_3_0 opengl;
  qDebug()<<opengl.initializeOpenGLFunctions();
  qDebug()<<"OpenGl";
// #define GL_FRAMEBUFFER_BINDING            0x8CA6
// #define GL_DRAW_FRAMEBUFFER_BINDING       0x8CA6
// #define GL_RENDERBUFFER_BINDING           0x8CA7
// #define GL_READ_FRAMEBUFFER               0x8CA8
// #define GL_DRAW_FRAMEBUFFER               0x8CA9
// #define GL_READ_FRAMEBUFFER_BINDING
  auto getInt=[&opengl](GLenum value) { GLint ret; opengl.glGetIntegerv(value, &ret); return ret; };
  // auto drawFbo=[&opengl]() { GLint ret; opengl.glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &ret); return ret: };
  auto printEnv=[&]() {
    qDebug()<<"FrameBuffer"<<getInt(GL_FRAMEBUFFER_BINDING);
    qDebug()<<"ReadFrameBuffer"<<getInt(GL_READ_FRAMEBUFFER_BINDING);
    qDebug()<<"RenderBuffer"<<getInt(GL_RENDERBUFFER_BINDING);
  };
  qDebug()<<"Default fb"<<context->defaultFramebufferObject();
  GLuint fbTemp[2], fb,fb2;
  opengl.glGenFramebuffers(2, fbTemp);
  fb=fbTemp[0];
  fb2=fbTemp[1];
  Q_ASSERT(fb);
  Q_ASSERT(fb2);
  qDebug()<<"Created framebuffer"<<fb<<fb2;
  opengl.glBindFramebuffer(GL_FRAMEBUFFER, fb);
  GLuint renderBuffer;
  opengl.glGenRenderbuffers(1, &renderBuffer);
  Q_ASSERT(renderBuffer);
  qDebug()<<"A"<<opengl.glGetError();
  opengl.glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
  qDebug()<<"B"<<opengl.glGetError();
  opengl.glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, 640, 480);
  qDebug()<<"C"<<opengl.glGetError();
  opengl.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);
  qDebug()<<"D"<<opengl.glGetError();
  opengl.glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
  qDebug()<<"E"<<opengl.glGetError();
  opengl.glClear(GL_COLOR_BUFFER_BIT);
  qDebug()<<"F"<<opengl.glGetError();
  QImage image(640, 480, QImage::Format_RGBA8888);
  // uint8_t data[10000];
  // memset(data, 'x', sizeof(data));
  // // printEnv();
  // qDebug()<<image.data_ptr();
  // opengl.glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);
  opengl.glReadBuffer(GL_COLOR_ATTACHMENT0);
  qDebug()<<"G"<<opengl.glGetError();
  opengl.glReadPixels(0, 0, 640, 480, GL_RGBA, GL_UNSIGNED_BYTE /*GL_UNSIGNED_INT_8_8_8_8*/, image.bits());
  qDebug()<<"H"<<opengl.glGetError();
  qDebug()<<image.size()<<image.sizeInBytes();
  // ui->label->setPixmap(QPixmap::fromImage(image));
  // // GL_NO_ERROR
  for(unsigned i=0;i<5000;i++) {
    printf("%d %02x\n",i, image.bits()[i]);
  }
/*  qDebug()<<"------";
  printEnv();
  opengl.glBindFramebuffer(GL_FRAMEBUFFER, fb2);
  qDebug()<<"------";
  printEnv();
  opengl.glBindFramebuffer(GL_FRAMEBUFFER, fb);
  qDebug()<<"------";
  printEnv()*/;


    // if(m_renderBuffer!=0) {
    //   // m_contextFunctions.gl_re
    //   m_contextFunctions.glBindRenderbuffer( GL_RENDERBUFFER, m_renderBuffer);
    //   m_renderSize=renderSize;
    //   m_contextFunctions.glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, renderSize.width(), renderSize.height());
    // }

  // printEnv();
  context->doneCurrent();
  //   auto thread=QThread::create([postResult, offscreenSurface]() {
  //     qDebug()<<"Starting thread"<<QThread::currentThread();
  //
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
}
