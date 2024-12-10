#pragma once
#include <QWidget>

class FlutterWidgetSoftwareApp;

class FlutterWidgetSoftware : public QWidget
{
  friend class FlutterWidgetSoftwareApp;
  Q_OBJECT
public:
  explicit FlutterWidgetSoftware(QWidget *parent = nullptr);
  bool start(const QString &assetPath);
// QWidget interface
protected:
  void presentSurface(const QImage &image);
  void resizeEvent(QResizeEvent *event) override;
private:
  FlutterWidgetSoftwareApp *m_flutter=nullptr;
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






