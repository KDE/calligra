/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef BRAINDUMPABOUTDATA_H
#define BRAINDUMPABOUTDATA_H

#include <KAboutData>
#include <KLocalizedString>
#include <kcoreaddons_version.h>

#define BRAINDUMP_VERSION "0.10.9"

inline KAboutData newBrainDumpAboutData()
{
    KAboutData aboutData(QStringLiteral("braindump"), i18n("Braindump"), QStringLiteral(BRAINDUMP_VERSION),
                         i18n("Braindump: directly from your brain to the computer.")
                         + QLatin1String("\n\n")
                         + i18n("Unmaintained at the time of release of this version. The Calligra community welcomes interested persons to take over maintainership."),
                         KAboutLicense::LGPL, i18n("(c) 2009, 2010, 2011, 2012, 2013 Cyrille Berger"));
#if KCOREADDONS_VERSION >= 0x051600
    aboutData.setDesktopFileName(QStringLiteral("org.kde.braindump"));
#endif
    aboutData.addAuthor(i18n("Somsubhra Bairi"), i18n("Former maintainer"), QStringLiteral("somsubhra.bairi@gmail.com"), QStringLiteral("https://www.somsubhra.com"));
    aboutData.addAuthor(i18n("Cyrille Berger"), i18n("Former maintainer"), QStringLiteral("cberger@cberger.net"));
    // standard ki18n translator strings
    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"),
                            i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    return aboutData;
}

#endif
