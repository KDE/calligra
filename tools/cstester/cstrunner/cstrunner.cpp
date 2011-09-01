#include <QCoreApplication>

#include "CSTProcessRunner.h"

#include <QApplication>
#include <QTimer>
#include <QDebug>

int main(int argc, char *argv[])
{
    if (argc < 4) {
        qDebug() << "usage" << argv[0] << "docDir resultDir concurrentProcesses";
        exit(-1);
    }

    QCoreApplication app(argc, argv);

    QString docDir(argv[1]);
    QString resultDir(argv[2]);
    QString number(argv[3]);
    CSTProcessRunner runner(docDir, resultDir, number.toInt());
    QTimer::singleShot(0, &runner, SLOT(start()));

    return app.exec();
}
