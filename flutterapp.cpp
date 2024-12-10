#include "flutterapp.h"
#include "flutter_engine/embedder.h"
#include <QPixmap>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QGuiApplication>

FlutterApp::FlutterApp()
{

}

void FlutterApp::setSize(QSize size)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_size=size;
  if(m_engine && m_size.isValid())
    setWindowSizeInternal();
}

bool FlutterApp::start(QString assetPath)
{
  bool ret=false;
  std::lock_guard<std::mutex> lock(m_mutex);
  QString icuDataPath=QDir(QCoreApplication::applicationDirPath()).relativeFilePath("icudtl.dat");
  if(m_engine!=nullptr) {
    qDebug()<<"Engine already started";
  }
  else if(!QFileInfo::exists(icuDataPath)) {
    qDebug()<<"Unable to find icu data at "<<icuDataPath;
  }
  else if(auto renderer = rendererConfig()){
    std::vector<const char*> engine_command_line_args = {
        "--disable-observatory",    //
        "--dart-non-checked-mode",  //
    };
    auto icuDataPathString=icuDataPath.toStdString();
    auto assetPathString=assetPath.toStdString(); //
    FlutterProjectArgs args = {
        .struct_size = sizeof(FlutterProjectArgs),
        .assets_path = assetPathString.data(), //MY_PROJECT "\\build\\flutter_assets",
        .icu_data_path = icuDataPathString.data(),
        .command_line_argc = static_cast<int>(engine_command_line_args.size()),
        .command_line_argv = engine_command_line_args.data(),
        // .vsync_callback = [](void* instance, intptr_t baton) {
        //   static_cast<FlutterApp*>(instance)->onVSync(baton);
        //   // qDebug()<<"VSYNC";
        //   // FlutterEngineOnVsync()
        // },
        .log_message_callback=[](const char *tag, const char *message, void *instance) {
          static_cast<FlutterApp*>(instance)->log(QString::fromUtf8(message));
        }
    };

    auto result = FlutterEngineRun(FLUTTER_ENGINE_VERSION, &(*renderer),  // renderer
                                   &args, this, &m_engine);

    if (result != kSuccess) {
      qDebug()<<"FlutterEngineRun failed"<<result;
      m_engine=nullptr;
    }
    else {
      ret=true;
      if(m_size.isValid())
        setWindowSizeInternal();
    }
  }
  else {
    qDebug()<<"Failed to generate configuration";
  }
  return ret;
}

void FlutterApp::cursorPressEvent(QPointF pos)
{
  FlutterPointerEvent event = {};
  event.struct_size = sizeof(event);
  event.phase = FlutterPointerPhase::kDown;
  event.x = pos.x(); //x * g_pixelRatio;
  event.y = pos.y(); //y * g_pixelRatio;
  event.timestamp = FlutterEngineGetCurrentTime();
      // std::chrono::duration_cast<std::chrono::microseconds>(
      //     std::chrono::high_resolution_clock::now().time_since_epoch())
      //     .count();
  FlutterEngineSendPointerEvent(m_engine, &event, 1);
}

void FlutterApp::cursorReleaseEvent(QPointF pos)
{
  FlutterPointerEvent event = {};
  event.struct_size = sizeof(event);
  event.phase = FlutterPointerPhase::kUp;
  event.x = pos.x(); //x * g_pixelRatio;
  event.y = pos.y(); //y * g_pixelRatio;
  event.timestamp = FlutterEngineGetCurrentTime();
      // std::chrono::duration_cast<std::chrono::microseconds>(
      //     std::chrono::high_resolution_clock::now().time_since_epoch())
      //     .count();
  FlutterEngineSendPointerEvent(m_engine, &event, 1);
}


void FlutterApp::log(const QString &message)
{
  qDebug()<<"Log"<<message;
}

void FlutterApp::setWindowSizeInternal()
{
  // qDebug()<<QGuiApplication::primaryScreen()->logicalDotsPerInch();
  static constexpr FlutterViewId implicitViewId = 0;
  FlutterWindowMetricsEvent event = {
      .struct_size=sizeof(FlutterWindowMetricsEvent),
      .width=static_cast<size_t>(m_size.width()),
      .height=static_cast<size_t>(m_size.height()),
      .pixel_ratio=1.0,
      .view_id=implicitViewId
  };
  FlutterEngineSendWindowMetricsEvent(m_engine, &event);
}
