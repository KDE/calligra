/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_KoRdfLocationTreeWidgetItem_h__
#define __rdf_KoRdfLocationTreeWidgetItem_h__

#include "KoRdfLocation.h"
#include "KoRdfSemanticTreeWidgetItem.h"

class KoCanvasBase;

/**
 * @short Display location (lat/long) semantic data with a context menu tailored
 *        to such infomartion.
 * @author Ben Martin <ben.martin@kogmbh.com>
 */
class KoRdfLocationTreeWidgetItem : public KoRdfSemanticTreeWidgetItem
{
    Q_OBJECT
public:
    KoRdfLocationTreeWidgetItem(QTreeWidgetItem *parent, hKoRdfLocation semObj);
    virtual ~KoRdfLocationTreeWidgetItem();

    /****************************************/
    /****************************************/
    /**** inherited and reimplemented... **/

    hKoRdfLocation semanticObject() const;
    virtual QList<QAction *> actions(QWidget *parent, KoCanvasBase *host = 0);
    virtual void insert(KoCanvasBase *host);

public Q_SLOTS:
    void showInViewer();
    void exportToFile();

protected:
    virtual hKoRdfSemanticItem semanticItem() const;
    virtual QString uIObjectName() const;

private:
    hKoRdfLocation m_semanticObject;
};
#endif
