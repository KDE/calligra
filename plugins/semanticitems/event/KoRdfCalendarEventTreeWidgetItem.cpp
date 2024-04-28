/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoRdfCalendarEventTreeWidgetItem.h"

#include "KoDocumentRdf.h"
#include "RdfSemanticTreeWidgetSelectAction.h"

#include <KLocalizedString>
#include <kdebug.h>

KoRdfCalendarEventTreeWidgetItem::KoRdfCalendarEventTreeWidgetItem(QTreeWidgetItem *parent, hKoRdfCalendarEvent ev)
    : KoRdfSemanticTreeWidgetItem(parent)
    , m_semanticObject(ev)
{
    setText(ColName, m_semanticObject->name());
}

hKoRdfSemanticItem KoRdfCalendarEventTreeWidgetItem::semanticItem() const
{
    return m_semanticObject;
}

QString KoRdfCalendarEventTreeWidgetItem::uIObjectName() const
{
    return i18n("Calendar Event");
}

QList<QAction *> KoRdfCalendarEventTreeWidgetItem::actions(QWidget *parent, KoCanvasBase *host)
{
    QList<QAction *> m_actions;
    QAction *action = 0;
    action = createAction(parent, host, i18n("Edit..."));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(edit()));
    m_actions.append(action);
    action = createAction(parent, host, i18n("Import event to Calendar"));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(saveToKCal()));
    m_actions.append(action);
    action = createAction(parent, host, i18n("Export event to iCal file..."));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(exportToFile()));
    m_actions.append(action);
    addApplyStylesheetActions(parent, m_actions, host);
    if (host) {
        action = new RdfSemanticTreeWidgetSelectAction(parent, host, semanticItem());
        m_actions.append(action);
    }
    return m_actions;
}

hKoRdfCalendarEvent KoRdfCalendarEventTreeWidgetItem::semanticObject() const
{
    return m_semanticObject;
}

void KoRdfCalendarEventTreeWidgetItem::insert(KoCanvasBase *host)
{
    semanticObject()->insert(host);
}

void KoRdfCalendarEventTreeWidgetItem::saveToKCal()
{
    kDebug(30015) << "import a calendar event from the document... "
                  << " name:" << m_semanticObject->name();
    semanticObject()->saveToKCal();
}

void KoRdfCalendarEventTreeWidgetItem::exportToFile()
{
    kDebug(30015) << "exporting to an iCal file..."
                  << " name:" << m_semanticObject->name();
    semanticObject()->exportToFile();
}
