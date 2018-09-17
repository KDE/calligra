/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef OOTHREAD_H
#define OOTHREAD_H

#include <QThread>
#include <QAtomicInt>
#include <QMutex>
#include <QWaitCondition>
#include <QPair>

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
    explicit OoThread(QObject* o);
    ~OoThread();

    void stop();

    /**
     * Read ppt file at path and convert it to an odp file.
     * The path where the new file will occur is returned.
     **/
    QString toOdp(const QString& path);
    /**
     * Read file at path and convert it to png files in a new temporary
     * directory. The path of the directory where the files will occur is
     * returned.
     **/
    QString toPng(const QString& path, int pngwidth);
    /**
     * Return true of the job for which the given output is given, is still
     * waiting or busy.
     **/
    bool waitingOrBusy(const QString& path);
Q_SIGNALS:
    void toOdpDone(const QString& odppath);
    void toPngDone(const QString& odppath);
};

#endif
