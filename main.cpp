#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QTime>
#include <QTimer>

#include "rik_client.h"

//QTimer *timer = new QTimer(this);
//connect(timer, SIGNAL(timeout()), this, SLOT(dosth()));
//timer->start(1000);

//void dosth(){


//}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterSingletonType<RIK_Client>("RIK_Client", 1, 0, "Client", &RIK_Client::GetInstance);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

//    QEventLoop loop;
//    QTimer::singleShot(5000, &app, SLOT(quit()));
//    loop.exec();

    return app.exec();
}
