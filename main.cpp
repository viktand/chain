#include "game.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QGLFormat fmt;
    fmt.setDoubleBuffer(true);
    QGLFormat::setDefaultFormat(fmt);

    Game window;
    window.setWindowTitle("Chain");
    window.setWindowIcon(QPixmap(QString(":/new/prefix1/ico")));
    window.resize(700,700);
    window.move(200,100);
    window.show();

    return a.exec();
}


   // window.showFullScreen();
   // window.showMaximized();
