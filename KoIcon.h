/*  This file is part of the Calligra project, made within the KDE community.

    SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOICON_H
#define KOICON_H

// Qt
#include <QIcon>

/**
 * Macros to support collecting the icons in use.
 *
 * After any change to this list of macros the file /CheckIcons.sh needs to be
 * updated accordingly, to ensure that the icon names of the affected macros are
 * still considered in the extraction.
 *
 * The naming pattern of the macros is like this:
 * * koIcon* returns a QIcon object
 * * koIconFallback* returns a QIcon object for given name or fallback name
 * * koIconName* returns a QLatin1String (aligned with usual API where "iconName" property is of type QString)
 * * koIconNameCStr* returns a const char*
 */

/// Use these macros for icons without any issues
#define koIcon(name) (QIcon::fromTheme(QStringLiteral(name)))
#define koIconFallback(name, fallbackName) (QIcon::fromTheme(QStringLiteral(name), QIcon::fromTheme(QStringLiteral(fallbackName))))
#define koIconName(name) (QStringLiteral(name))
#define koIconNameCStr(name) (name)
/// Use these definitions in files where needed:
// #define koSmallIcon(name) (SmallIcon(QStringLiteral(name)))
// #define koDesktopIcon(name) (DesktopIcon(QStringLiteral(name)))
// Also #include <KIconLoader>

/// Use these macros if there is a proper icon missing
#define koIconNeeded(comment, neededName) (QIcon::fromTheme(QStringLiteral(neededName)))
#define koIconNeededWithSubs(comment, neededName, substituteName) (QIcon::fromTheme(QStringLiteral(substituteName)))
#define koIconNameNeeded(comment, neededName) (QStringLiteral(neededName))
#define koIconNameNeededWithSubs(comment, neededName, substituteName) (QStringLiteral(substituteName))
#define koIconNameCStrNeeded(comment, neededName) (neededName)
#define koIconNameCStrNeededWithSubs(comment, neededName, substituteName) (substituteName)

/// Use these macros if the UI is okay without any icon, but would be better with one.
#define koIconWanted(comment, wantedName) (QIcon())
#define koIconNameWanted(comment, wantedName) (QString())

#endif
