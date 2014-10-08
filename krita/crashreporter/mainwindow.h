/*
 * Copyright (c) 2008-2009 Hyves (Startphone Ltd.)
 * Copyright (c) 2014 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QNetworkReply>
#include <QString>
#include <QWidget>

#include "ui_wdg_crash_reporter.h"


class MainWindow : public QWidget, public Ui::WdgCrashReporter
{
    Q_OBJECT
public:
    MainWindow(const QString &dumpPath, const QString &id, const QString &applicationId, QWidget *parent = 0);
    virtual ~MainWindow();

    static QString getApplicationExeFromId(const QString &applicationId);

public slots:
    void restart();

private slots:
    void close();
    void onToggleAllowUpload(int state);
    void startUpload();
    void uploadDone(QNetworkReply *reply);
    void uploadProgress(qint64 received, qint64 total);
    void uploadError(QNetworkReply::NetworkError);
private:
    void addFileAsField(const QString &filename, const QString &fieldName, QList<QPair<QByteArray, QByteArray>> *fields);
    bool checkForOpenglProblems();
    bool checkAmdDriver(const QString& openglString);
    bool checkNvidiaDriver(const QString& openglString);
    bool checkIntelDriver(const QString& openglString);

    struct Private;
    Private *const m_d;
};

#endif // MAINWINDOW_H
