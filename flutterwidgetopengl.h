#pragma once
#include <QWidget>

class FlutterWidgetOpenGlApp;

class FlutterWidgetOpenGl : public QWidget
{
  friend class FlutterWidgetOpenGlApp;
  Q_OBJECT
  public:
  explicit FlutterWidgetOpenGl(QWidget *parent = nullptr);
  bool start(const QString &assetPath);
  // QWidget interface
  protected:
  void presentSurface(const QImage &image);
  void resizeEvent(QResizeEvent *event) override;
  private:
  FlutterWidgetOpenGlApp *m_flutter=nullptr;
  QPixmap m_pixmap;

  // QWidget interface
  protected:
  void paintEvent(QPaintEvent *event) override;

  // QObject interface
  public:
  // bool event(QEvent *event) override;

  // QWidget interface
  protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
};
