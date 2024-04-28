/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef RDF_KORDFSEMANTICTREEWIDGETITEM_H
#define RDF_KORDFSEMANTICTREEWIDGETITEM_H

#include "RdfForward.h"
#include "kordf_export.h"
// Qt
#include <QObject>
#include <QTreeWidgetItem>

class QAction;
class KoCanvasBase;

/**
 * Code wishing to display a list of KoRdfSemanticItem objects can create TreeWidgetItems
 * using KoRdfSemanticItem::createQTreeWidgetItem().
 *
 * These tree widget items can in turn generate a context menu by calling the
 * actions() method. These actions are already setup to work on the underlying
 * KoRdfSemanticItem objects.
 *
 * @author Ben Martin <ben.martin@kogmbh.com>
 * @see KoRdfSemanticItem::createQTreeWidgetItem()
 * @see KoDocumentRdf
 */
class KORDF_EXPORT KoRdfSemanticTreeWidgetItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
protected:
    virtual QString uIObjectName() const = 0;
    void addApplyStylesheetActions(QWidget *parent, QList<QAction *> &actions, KoCanvasBase *host);
    QAction *createAction(QWidget *parent, KoCanvasBase *host, const QString &text);

public:
    enum { ColName = 0, ColSize };

    explicit KoRdfSemanticTreeWidgetItem(QTreeWidgetItem *parent);
    virtual ~KoRdfSemanticTreeWidgetItem();

    /**
     * Get the underlying SemanticItem for this widget
     */
    virtual hKoRdfSemanticItem semanticItem() const = 0;

    virtual QList<QAction *> actions(QWidget *parent, KoCanvasBase *host = 0);
    virtual void insert(KoCanvasBase *host);

public Q_SLOTS:
    virtual void edit();
};
#endif
