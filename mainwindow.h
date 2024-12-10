#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class FlutterApp;
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
  MainWindow(const QString &script, QWidget *parent = nullptr);
  ~MainWindow();
  void render(QPixmap px);
  private:
  void testOpenGl();
  Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
