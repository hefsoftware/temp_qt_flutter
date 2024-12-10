#include "flutterwidgetopengl.h"
#include "flutterappopengl2.h"
#include <QPainter>
#include <QResizeEvent>
#include <QThread>

class FlutterWidgetOpenGlApp: public FlutterAppOpenGlRenderBuffer {
  public:
  FlutterWidgetOpenGlApp(FlutterWidgetOpenGl *widget): m_widget(widget) { }

  private:
  FlutterWidgetOpenGl *m_widget;
  // FlutterAppOpenGl interface
  protected:
  bool render(const QImage &pixmap) override;
};

bool FlutterWidgetOpenGlApp::render(const QImage &image)
{
  qDebug()<<"My render";
  QMetaObject::invokeMethod(m_widget, &FlutterWidgetOpenGl::presentSurface, image);
  return true;
}

FlutterWidgetOpenGl::FlutterWidgetOpenGl(QWidget *parent)
    : QWidget{parent}
{
  m_flutter=new FlutterWidgetOpenGlApp(this);
  setAttribute(Qt::WA_AcceptTouchEvents);
}

bool FlutterWidgetOpenGl::start(const QString &assetPath)
{
  return m_flutter->start(assetPath);
}

void FlutterWidgetOpenGl::presentSurface(const QImage &image)
{
  qDebug()<<"PresentArm";
  m_pixmap=QPixmap::fromImage(image);
  update();
}

void FlutterWidgetOpenGl::resizeEvent(QResizeEvent *event)
{
  m_flutter->setSize(event->size());
}

void FlutterWidgetOpenGl::paintEvent(QPaintEvent *event)
{
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.drawPixmap(0, 0, m_pixmap);
}

void FlutterWidgetOpenGl::mousePressEvent(QMouseEvent *event)
{
  m_flutter->cursorPressEvent(event->pos());
}

void FlutterWidgetOpenGl::mouseReleaseEvent(QMouseEvent *event)
{
  m_flutter->cursorReleaseEvent(event->pos());
}

void FlutterWidgetOpenGl::mouseMoveEvent(QMouseEvent *event)
{
}
