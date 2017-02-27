#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "rik_client.h"

#include "pthread.h"

#include "cvSix/cvRun.h"

void *thread_cv(void *arg)
{
    return NULL;
}

int main(int argc, char *argv[])
{
    int thread_cv_err = -1;
    pthread_t ntid;
    thread_cv_err = pthread_create(&ntid, NULL, thread_cv, NULL);

    QGuiApplication app(argc, argv);

    qmlRegisterSingletonType<RIK_Client>("RIK_Client", 1, 0, "Client", &RIK_Client::GetInstance);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
