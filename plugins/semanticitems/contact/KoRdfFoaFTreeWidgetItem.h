/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_KoRdfFoaFTreeWidgetItem_h__
#define __rdf_KoRdfFoaFTreeWidgetItem_h__

#include "KoRdfFoaF.h"
#include "KoRdfSemanticTreeWidgetItem.h"

class KoCanvasBase;

/**
 * @short Display Contact/FOAF semantic data with a context menu tailored
 *        to such infomartion.
 * @author Ben Martin <ben.martin@kogmbh.com>
 */
class KoRdfFoaFTreeWidgetItem : public KoRdfSemanticTreeWidgetItem
{
    Q_OBJECT
public:
    KoRdfFoaFTreeWidgetItem(QTreeWidgetItem *parent, hKoRdfFoaF foaf);

    // inherited and reimplemented...

    hKoRdfFoaF foaf() const;
    virtual QList<QAction *> actions(QWidget *parent, KoCanvasBase *host = 0);
    virtual void insert(KoCanvasBase *host);

public Q_SLOTS:
    void importSelectedSemanticViewContact();
    void exportToFile();

protected:
    virtual hKoRdfSemanticItem semanticItem() const;
    virtual QString uIObjectName() const;

private:
    hKoRdfFoaF m_foaf;
};

#endif
