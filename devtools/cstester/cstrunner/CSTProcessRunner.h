/*
 * This file is part of Calligra
 *
 * Copyright (C) 2011-2012 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef CSTPROCESSRUNNER_H
#define CSTPROCESSRUNNER_H

#include <QList>
#include <QMap>
#include <QObject>
#include <QProcess>

class QString;

class CSTProcessRunner : public QObject
{
    Q_OBJECT
public:
    CSTProcessRunner(const QString &documentDir, const QString &resultDir, int concurrentProcesses, bool pickup);
    ~CSTProcessRunner() override;

public Q_SLOTS:
    void start();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void startCstester(QProcess *process);
    void startMd5(QProcess *process, const QString &document);
    void logResult();

    QString m_resultDir;
    int m_concurrentProcesses;
    QMap<QProcess *, QString> m_processes;
    QList<QString> m_documents;
    QMap<int, QList<QString> > m_killed;
};

#endif /* CSTPROCESSRUNNER_H */
