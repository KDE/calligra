/* This file is part of the KDE project
   Copyright (C) 2012 Friedrich W. H. Kossebau <kossebau@kde.org>
   Copyright (C) 2015 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIICON_H
#define KEXIICON_H

#ifdef KOICON_H
# error "Don't use with KoIcon.h!"
#endif

#include <QApplication>
#include <QColor>
#include <QIcon>
#include <QPalette>

#include <KIconLoader>

/**
 * Macros to support collecting the icons in use.
 *
 * After any change to this list of macros the file /CheckIcons.sh needs to be
 * updated accordingly, to ensure that the icon names of the affected macros are
 * still considered in the extraction.
 *
 * The naming pattern of the macros is like this:
 * * koIcon* returns a QIcon object
 * * koIconName* returns a QLatin1String (aligned with usual API where "iconName" property is of type QString)
 * * koIconNameCStr* returns a const char*
 */

//! Use these macros for icons without any issues
#define koIcon(name) (QIcon::fromTheme(QLatin1String(name)))
#define koIconName(name) (QLatin1String(name))
#define koIconNameCStr(name) (name)
#define koSmallIcon(name) (SmallIcon(QLatin1String(name)))
#define koDesktopIcon(name) (DesktopIcon(QLatin1String(name)))

//! Use these macros if there is a proper icon missing
#define koIconNeeded(comment, neededName) (QIcon::fromTheme(QLatin1String(neededName)))
#define koIconNeededWithSubs(comment, neededName, substituteName) (QIcon::fromTheme(QLatin1String(substituteName)))
#define koIconNameNeeded(comment, neededName) (QLatin1String(neededName))
#define koIconNameNeededWithSubs(comment, neededName, substituteName) (QLatin1String(substituteName))
#define koIconNameCStrNeeded(comment, neededName) (neededName)
#define koIconNameCStrNeededWithSubs(comment, neededName, substituteName) (substituteName)

//! Use these macros if the UI is okay without any icon, but would be better with one.
#define koIconWanted(comment, wantedName) (QIcon())
#define koIconNameWanted(comment, wantedName) (QString())

//! Use this function to load an icon that fits the current color theme
inline QIcon themedIcon(const QString &name, bool fast = false) {
    Q_UNUSED(fast);

    static bool firstUse = true;
    if (firstUse) {
        // workaround for some kde-related crash
        bool _unused = KIconLoader::global()->iconPath(name, KIconLoader::NoGroup, true).isEmpty();
        Q_UNUSED(_unused);
        firstUse = false;
    }

    // try load themed icon
    QColor background = qApp->palette().background().color();
    bool useDarkIcons = background.value() > 100;
    const char * const prefix = useDarkIcons ? "dark_" : "light_";

    QString realName = QLatin1String(prefix) + name;
    QIcon icon = QIcon::fromTheme(realName);

    // fallback
    if (icon.isNull()) {
        return QIcon::fromTheme(name);
    }

    return icon;
}

#endif
