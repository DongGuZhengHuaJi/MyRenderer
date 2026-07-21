#include <QApplication>
#include <QMainWindow>
#include "window/RenderViewport.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QMainWindow window;
    window.setWindowTitle("MyRenderer Sandbox - v1.0");
    window.resize(1024, 768);

    // 将我们的渲染视口作为中央控件嵌入窗口
    RenderViewport *viewport = new RenderViewport(&window);
    window.setCentralWidget(viewport);

    window.show();
    return a.exec();
}