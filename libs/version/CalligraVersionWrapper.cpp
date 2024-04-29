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
    QLatin1StringView calligraVersion(CALLIGRA_VERSION_STRING);
    QString version = calligraVersion;

    if (checkGit) {
#ifdef CALLIGRA_GIT_SHA1_STRING
        QLatin1StringView gitVersion(CALLIGRA_GIT_SHA1_STRING);
        version = QStringLiteral("%1 (git %2)").arg(calligraVersion, gitVersion);
#endif
    }
    return version;
}
