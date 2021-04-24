/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_KoRdfCalendarEventTreeWidgetItem_h__
#define __rdf_KoRdfCalendarEventTreeWidgetItem_h__

#include "KoRdfCalendarEvent.h"
#include "KoRdfSemanticTreeWidgetItem.h"

class KoCanvasBase;

/**
 * @short Display ical/vevent semantic data with a context menu tailored
 *        to such infomartion.
 * @author Ben Martin <ben.martin@kogmbh.com>
 */
class KoRdfCalendarEventTreeWidgetItem : public KoRdfSemanticTreeWidgetItem
{
    Q_OBJECT

public:
    KoRdfCalendarEventTreeWidgetItem(QTreeWidgetItem *parent, hKoRdfCalendarEvent ev);

    // inherited and reimplemented...

    hKoRdfCalendarEvent semanticObject() const;
    virtual QList<QAction *> actions(QWidget *parent, KoCanvasBase *host = 0);
    virtual void insert(KoCanvasBase *host);

public Q_SLOTS:
    void saveToKCal();
    void exportToFile();

private:
    hKoRdfCalendarEvent m_semanticObject;

protected:
    virtual hKoRdfSemanticItem semanticItem() const;
    virtual QString uIObjectName() const;
};

#endif
