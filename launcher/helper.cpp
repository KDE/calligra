// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "helper.h"

#include <KIO/CommandLauncherJob>
#include <KIO/JobUiDelegateFactory>
#include <KJobUiDelegate>
#include <KService>
#include <QApplication>

using namespace Qt::StringLiterals;

Helper::Helper(QObject *parent)
    : QObject(parent)
{
}

void Helper::execute(const QString &appId)
{
    if (appId.isEmpty()) {
        qApp->quit();
    }

    auto job = new KIO::CommandLauncherJob(appId, QStringList{});
    connect(job, &KJob::result, this, [job](KJob *) {
        if (job->error() != KJob::NoError) {
            qWarning() << job->errorString();
        }
        qApp->quit();
    });
    job->start();
}
