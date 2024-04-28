/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef OOTHREAD_H
#define OOTHREAD_H

#include <QAtomicInt>
#include <QMutex>
#include <QPair>
#include <QThread>
#include <QWaitCondition>

class OoThread : public QThread
{
    Q_OBJECT
private:
    QAtomicInt running;
    QMutex mutex;
    QWaitCondition moreWork;
    class Conversion
    {
    public:
        QString from;
        QString to;
        int width;
        operator bool()
        {
            return !from.isEmpty();
        }
    };
    Conversion currentToOdp;
    Conversion currentToPng;
    Conversion nextToOdp;
    Conversion nextToPng;
    class OOConnection;
    OOConnection *oo;

    void convertToOdp(const Conversion &c);
    void convertToPng(const Conversion &c);

protected:
    void run();

public:
    explicit OoThread(QObject *o);
    ~OoThread();

    void stop();

    /**
     * Read ppt file at path and convert it to an odp file.
     * The path where the new file will occur is returned.
     **/
    QString toOdp(const QString &path);
    /**
     * Read file at path and convert it to png files in a new temporary
     * directory. The path of the directory where the files will occur is
     * returned.
     **/
    QString toPng(const QString &path, int pngwidth);
    /**
     * Return true of the job for which the given output is given, is still
     * waiting or busy.
     **/
    bool waitingOrBusy(const QString &path);
Q_SIGNALS:
    void toOdpDone(const QString &odppath);
    void toPngDone(const QString &odppath);
};

#endif
