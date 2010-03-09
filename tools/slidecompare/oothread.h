#ifndef OOTHREAD_H
#define OOTHREAD_H

#include <QtCore/QThread>
#include <QtCore/QAtomicInt>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QPair>

class OoThread : public QThread {
Q_OBJECT
private:
    QAtomicInt running;
    QMutex mutex;
    QWaitCondition moreWork;
    class Conversion {
    public:
        QString from;
        QString to;
        int width;
        operator bool() {
            return !from.isEmpty();
        }
    };
    Conversion currentToOdp;
    Conversion currentToPng;
    Conversion nextToOdp;
    Conversion nextToPng;
    class OOConnection;
    OOConnection* oo;

    void convertToOdp(const Conversion& c);
    void convertToPng(const Conversion& c);
protected:
    void run();
public:
    OoThread(QObject* o);
    ~OoThread();

    void stop();

    /**
     * Read ppt file at path and convert it to an odp file.
     * The path where the new file will occur is returned.
     **/
    QString toOdp(const QString& path);
    /**
     * Read file at path and convert it to png files in a new temporary
     * directory. The path of the direcotry where the files will occur is
     * returned.
     **/
    QString toPng(const QString& path, int pngwidth);
    /**
     * Return true of the job for which the given output is given, is still
     * waiting or busy.
     **/
    bool waitingOrBusy(const QString& path);
signals:
    void toOdpDone(const QString& odppath);
    void toPngDone(const QString& odppath);
};

#endif
