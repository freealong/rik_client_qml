#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "rik_client.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterSingletonType<RIK_Client>("RIK_Client", 1, 0, "Client", &RIK_Client::GetInstance);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
