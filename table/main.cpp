#include "main-window.h"

#include <QTimer>
#include <QApplication>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow* w = new MainWindow();

    QSplashScreen splash;
    splash.setPixmap(QPixmap(":/resources/images/splashscreen.png"));
    splash.show();

    a.processEvents();

    QEventLoop loop;
    QTimer::singleShot(3000, &loop, SLOT(quit()));
    loop.exec();

    w->show();
    splash.finish(w);

    return a.exec();
}
