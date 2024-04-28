/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoRdfLocationTreeWidgetItem.h"

#include "KoDocumentRdf.h"
#include "RdfSemanticTreeWidgetSelectAction.h"
// KF5
#include <KLocalizedString>
#include <kdebug.h>

KoRdfLocationTreeWidgetItem::KoRdfLocationTreeWidgetItem(QTreeWidgetItem *parent, hKoRdfLocation semObj)
    : KoRdfSemanticTreeWidgetItem(parent)
    , m_semanticObject(semObj)
{
    setText(ColName, m_semanticObject->name());
}

KoRdfLocationTreeWidgetItem::~KoRdfLocationTreeWidgetItem()
{
    kDebug(30015) << "DTOR()";
    if (m_semanticObject)
        kDebug(30015) << "semobj:" << m_semanticObject->name();
    else
        kDebug(30015) << "NO SEMOBJ";
    kDebug(30015) << "DTOR(END)";
}

hKoRdfSemanticItem KoRdfLocationTreeWidgetItem::semanticItem() const
{
    return m_semanticObject;
}

QString KoRdfLocationTreeWidgetItem::uIObjectName() const
{
    return i18n("Location Information");
}

QList<QAction *> KoRdfLocationTreeWidgetItem::actions(QWidget *parent, KoCanvasBase *host)
{
    QList<QAction *> m_actions;
    QAction *action = 0;

#ifdef CAN_USE_MARBLE
    // These were coded to need marble
    action = createAction(parent, host, "Edit...");
    connect(action, SIGNAL(triggered(bool)), this, SLOT(edit()));
    m_actions.append(action);
    action = createAction(parent, host, "Show location on a map");
    connect(action, SIGNAL(triggered(bool)), this, SLOT(showInViewer()));
    m_actions.append(action);
#endif

    action = createAction(parent, host, "Export location to KML file...");
    connect(action, SIGNAL(triggered(bool)), this, SLOT(exportToFile()));
    m_actions.append(action);
    addApplyStylesheetActions(parent, m_actions, host);
    if (host) {
        action = new RdfSemanticTreeWidgetSelectAction(parent, host, semanticItem());
        m_actions.append(action);
    }
    return m_actions;
}

hKoRdfLocation KoRdfLocationTreeWidgetItem::semanticObject() const
{
    return m_semanticObject;
}

void KoRdfLocationTreeWidgetItem::insert(KoCanvasBase *host)
{
    semanticObject()->insert(host);
}

void KoRdfLocationTreeWidgetItem::showInViewer()
{
    semanticObject()->showInViewer();
}

void KoRdfLocationTreeWidgetItem::exportToFile()
{
    semanticObject()->exportToFile();
}
