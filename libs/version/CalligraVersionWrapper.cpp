/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include <CalligraVersionWrapper.h>

#include <calligragitversion.h>
#include <calligraversion.h>

QString CalligraVersionWrapper::versionYear()
{
    return QLatin1String(CALLIGRA_YEAR);
}

QString CalligraVersionWrapper::versionString(bool checkGit)
{
    QString calligraVersion(CALLIGRA_VERSION_STRING);
    QString version = calligraVersion;

    if (checkGit) {
#ifdef CALLIGRA_GIT_SHA1_STRING
        QString gitVersion(CALLIGRA_GIT_SHA1_STRING);
        version = QString("%1 (git %2)").arg(calligraVersion, gitVersion).toLatin1();
#endif
    }
    return version;
}
