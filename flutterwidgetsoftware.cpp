#include "flutterwidgetsoftware.h"
#include "flutterappsoftware.h"
#include <QPainter>
#include <QResizeEvent>
#include <QThread>
class FlutterWidgetSoftwareApp: public FlutterAppSoftware {
public:
  FlutterWidgetSoftwareApp(FlutterWidgetSoftware *widget): m_widget(widget) { }

private:
  FlutterWidgetSoftware *m_widget;
// FlutterAppSoftware interface
protected:
  bool render(const QImage &pixmap) override;
};

bool FlutterWidgetSoftwareApp::render(const QImage &image)
{
  QMetaObject::invokeMethod(m_widget, &FlutterWidgetSoftware::presentSurface, image);
  return true;
}

FlutterWidgetSoftware::FlutterWidgetSoftware(QWidget *parent)
    : QWidget{parent}
{
  m_flutter=new FlutterWidgetSoftwareApp(this);
  setAttribute(Qt::WA_AcceptTouchEvents);
}

bool FlutterWidgetSoftware::start(const QString &assetPath)
{
  return m_flutter->start(assetPath);
}

void FlutterWidgetSoftware::presentSurface(const QImage &image)
{
  m_pixmap=QPixmap::fromImage(image);
  update();
}

void FlutterWidgetSoftware::resizeEvent(QResizeEvent *event)
{
  m_flutter->setSize(event->size());
}

void FlutterWidgetSoftware::paintEvent(QPaintEvent *event)
{
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.drawPixmap(0, 0, m_pixmap);
}

void FlutterWidgetSoftware::mousePressEvent(QMouseEvent *event)
{
  m_flutter->cursorPressEvent(event->pos());
}

void FlutterWidgetSoftware::mouseReleaseEvent(QMouseEvent *event)
{
  m_flutter->cursorReleaseEvent(event->pos());
}

void FlutterWidgetSoftware::mouseMoveEvent(QMouseEvent *event)
{
}
