// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "helper.h"

#include <KIO/ApplicationLauncherJob>
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

    KService::Ptr service = KService::serviceByDesktopName(appId);
    auto job = new KIO::ApplicationLauncherJob(service);
    connect(job, &KJob::result, this, [job](KJob *) {
        qApp->quit();
    });
    job->start();
}
