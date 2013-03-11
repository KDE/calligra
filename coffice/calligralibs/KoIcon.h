#ifndef CALLIGRADOCUMENTS_KOICON_H
#define CALLIGRADOCUMENTS_KOICON_H

#include <QByteArray>
#include <QIcon>

#include <kicon.h>
#include <kiconloader.h>

#define koIcon(name) (KIcon(QLatin1String(name)))
#define koIconName(name) (QLatin1String(name))
#define koIconNameCStr(name) (name)
#define koSmallIcon(name) (SmallIcon(QLatin1String(name)))
#define koDesktopIcon(name) (DesktopIcon(QLatin1String(name)))
#define koIconNeeded(comment, neededName) (KIcon(QLatin1String(neededName)))
#define koIconNeededWithSubs(comment, neededName, substituteName) (KIcon(QLatin1String(substituteName)))
#define koIconNameNeeded(comment, neededName) (QLatin1String(neededName))
#define koIconNameNeededWithSubs(comment, neededName, substituteName) (QLatin1String(substituteName))
#define koIconNameCStrNeeded(comment, neededName) (neededName)
#define koIconNameCStrNeededWithSubs(comment, neededName, substituteName) (substituteName)
//#define koIconWanted(comment, wantedName) (KIcon())
//#define koIconNameWanted(comment, wantedName) (QString())

class KoIcon : public QIcon
{
};

#endif
