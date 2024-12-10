#include "mainwindow.h"

#include <QApplication>

#include "flutterappopengl.h"

int main(int argc, char *argv[])
{
  if(argc<2) {
    fprintf(stderr,"You must provide a path to flutter_assets\n");
    return 1;
  }
  QApplication a(argc, argv);
  // FlutterAppOpenGl openGl;
  // // FlutterAppOpenGl openGl;
   // // FlutterApp flutter;
  // FlutterAppSoftware app;
  // app.setSize(QSize(640, 480));
  MainWindow w(argv[1]);
  w.show();
  return a.exec();
}
