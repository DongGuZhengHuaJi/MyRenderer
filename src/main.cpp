#include <QApplication>
#include <QMainWindow>
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow window;
    window.setWindowTitle("MyRenderer");
    window.resize(1920, 1200); // 设置窗口大小

    window.show();
    return a.exec();
}