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
#include "CSTProcessRunner.h"

#include <QCoreApplication>
#include <QDir>
#include <QString>
#include <QDebug>

static const char PROGRAM[] = "cstwrapper.sh";

CSTProcessRunner::CSTProcessRunner(const QString &documentDir, const QString &resultDir, int concurrentProcesses, bool pickup)
: m_resultDir(resultDir)
, m_concurrentProcesses(concurrentProcesses)
{
    if (!QDir::current().exists(resultDir)) {
        qWarning() << "Creating result directory " << resultDir;
        if (!QDir::current().mkpath(resultDir)) {
            qCritical() << "Could not create result directory " << resultDir;
            ::exit(-1);
        }
        // if the dir was not there we can't pickup
        pickup = false;
    }
    QDir docDir(documentDir);
    QFileInfoList list = docDir.entryInfoList(QDir::Files, QDir::Name);
    foreach(const QFileInfo &entry, list) {
        m_documents.append(entry.filePath());
    }
    if (pickup) {
        QDir resDir(resultDir);
        QFileInfoList resList = resDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        foreach(const QFileInfo &entry, resList) {
            QString fileName = entry.fileName();
            if (fileName.endsWith(".check")) {
                fileName.resize(fileName.length() - 6);
                m_documents.removeOne(docDir.path() + docDir.separator() + fileName);
            }
        }
    }
    qDebug() << "Documents to process:" << m_documents.size();
}

CSTProcessRunner::~CSTProcessRunner()
{
    qDeleteAll(m_processes.keys());
}

void CSTProcessRunner::start()
{
    for (int i = 0; i < m_concurrentProcesses; ++i) {
        QProcess *process = new QProcess();
        connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
        startCstester(process);
    }
}

void CSTProcessRunner::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QProcess *process = qobject_cast<QProcess *>(sender());
    if (process) {
        QString &document = m_processes[process];

        if (process->exitStatus() != QProcess::NormalExit) {
            qWarning() << "Process exited with errors";
            if (process->error() == QProcess::FailedToStart) {
                qWarning() << "It did not even start !";
                if (!document.isEmpty()) {
                    qWarning() << "Check for your path : does it contain " << PROGRAM;
                } else {
                    qWarning() << "Check for your path : does it contain cstmd5gen.sh";
                }
            }
        }

        if (!document.isEmpty()) {
            qDebug() << "finished:" << process << document << exitCode << exitStatus;
            if (exitCode != 0) {
                if (exitCode & 127) {
                    int signal = exitCode & 127;
                    m_killed[signal].append(document);
//                     qDebug() << "exit with signal:" << signal;
                }
                startCstester(process);
            }
            else {
                QString tmp(document);
                document.clear();
                startMd5(process, tmp);
            }
        }
        else {
            if (exitCode != 0) {
                qWarning() << "cstmd5gen.sh failed";
            }
//             qDebug() << "md5 done";
            startCstester(process);
        }
    }
    else {
        qWarning("processFinished but progress not there");
    }
}

void CSTProcessRunner::startCstester(QProcess *process)
{
    if (m_documents.isEmpty()) {
        QMap<QProcess *, QString>::iterator it = m_processes.begin();
        bool finished = true;
        for (; it != m_processes.end(); ++it) {
            if (it.key()->state() != QProcess::NotRunning) {
                finished = false;
            }
        }

        if (finished) {
            logResult();

            QCoreApplication::exit(0);
        }
    }
    else {
        //TODO: check if result is already there and then do nothing
        QString document = m_documents.takeFirst();
        //qDebug() << "start:" << process << document << m_resultDir;
        QStringList arguments;
        arguments << "--outdir" << m_resultDir << "--create" << document;
        m_processes[process] = document;
        process->start(PROGRAM, arguments, QIODevice::NotOpen);
    }
}

void CSTProcessRunner::startMd5(QProcess *process, const QString &document)
{
    QFileInfo file(document);
    QString dir = m_resultDir + '/' + file.fileName() + ".check";
    QStringList arguments;
    arguments << dir;
    process->start("cstmd5gen.sh", arguments, QIODevice::NotOpen);
}

void CSTProcessRunner::logResult()
{
    QMap<int, QList<QString> >::const_iterator it = m_killed.constBegin();

    QTextStream out(stdout);
    out << "Documents resulted in a signal\n";
    for (; it != m_killed.constEnd(); ++it) {
        out << "Signal " << it.key() << ", " << it.value().size() << " documents\n";
        QList<QString>::const_iterator lIt = it.value().constBegin();
        for (; lIt != it.value().constEnd(); ++lIt)
        {
            out << *lIt << "\n";
        }
    }
}
