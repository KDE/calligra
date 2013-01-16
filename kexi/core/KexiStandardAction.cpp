/* This file is part of the KDE project
   Copyright (C) 2007 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KexiStandardAction.h"

#include <KoIcon.h>

#include <QHash>
#include <KGlobal>
#include <KShortcut>
#include <KStandardAction>
#include <KStandardShortcut>
#include <KAction>
#include <KActionCollection>
#include <KLocale>

namespace KexiStandardAction
{

//! @internal
struct Info {
    StandardAction id;
    KShortcut shortcut;
    const char* psName;
    const char* psText;
    const char* psToolTip;
    const char* psWhatsThis;
    const char* psIconName;
};

//! @internal
static const Info g_rgActionInfo[] = {
    { SortAscending, KShortcut(), "data_sort_az", I18N_NOOP("&Ascending"),
        I18N_NOOP("Sort data in ascending order"),
        I18N_NOOP("Sorts data in ascending order (from A to Z and from 0 to 9). Data from selected column is used for sorting."),
        koIconNameCStr("view-sort-ascending") },
    { SortDescending, KShortcut(), "data_sort_za", I18N_NOOP("&Descending"),
      I18N_NOOP("Sort data in descending order"),
      I18N_NOOP("Sorts data in descending (from Z to A and from 9 to 0). Data from selected column is used for sorting."),
      koIconNameCStr("view-sort-descending") },

    { ActionNone, KShortcut(), 0, 0, 0, 0, 0 }
};

//! @internal
class ActionsInfoHash : public QHash<StandardAction, const Info*>
{
public:
    ActionsInfoHash() {
        const Info* actionInfo = g_rgActionInfo;

        for (; actionInfo->id != ActionNone; actionInfo++)
            insert(actionInfo->id, actionInfo);
    }
};

K_GLOBAL_STATIC(ActionsInfoHash, g_rgActionInfoHash)

inline const Info* infoPtr(StandardAction id)
{
    return g_rgActionInfoHash->value(id);
}

//---------------------------------------------

KAction *create(StandardAction id, const QObject *recvr, const char *slot, QObject *parent)
{
    KAction *pAction = 0;
    const Info* pInfo = infoPtr(id);

    if (pInfo) {
        pAction = new KAction(parent);
        pAction->setObjectName(pInfo->psName);
        KShortcut cut(pInfo->shortcut);
        if (!cut.isEmpty())
            pAction->setShortcut(cut);
        pAction->setText(i18n(pInfo->psText));
        pAction->setToolTip(i18n(pInfo->psToolTip));
        pAction->setWhatsThis(i18n(pInfo->psWhatsThis));
        if (pInfo->psIconName)
            pAction->setIcon(KIcon(QLatin1String(pInfo->psIconName)));
    }

    if (recvr && slot)
        QObject::connect(pAction, SIGNAL(triggered(bool)), recvr, slot);

    if (pAction) {
        KActionCollection *collection = qobject_cast<KActionCollection *>(parent);
        if (collection)
            collection->addAction(pAction->objectName(), pAction);
    }
    return pAction;
}

const char* name(StandardAction id)
{
    const Info* pInfo = infoPtr(id);
    return (pInfo) ? pInfo->psName : 0;
}

#define CREATE_METHOD(methodName, enumName) \
    KAction *methodName(const QObject *recvr, const char *slot, QObject *parent) \
    { \
        return KexiStandardAction::create(enumName, recvr, slot, parent); \
    }

CREATE_METHOD(sortAscending, SortAscending)
CREATE_METHOD(sortDescending, SortDescending)

} //KexiStandardAction
