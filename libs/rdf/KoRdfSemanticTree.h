/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>
   SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_KoRdfSemanticTree_h__
#define __rdf_KoRdfSemanticTree_h__

#include "kordf_export.h"
// Qt
#include <QSharedPointer>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class KoDocumentRdf;
class KoRdfSemanticTreePrivate;

namespace Soprano
{
class Model;
}
/**
 * @short Manage a QTreeWidget showing a collection of Semantic Items
 * @author Ben Martin <ben.martin@kogmbh.com>
 *
 * Because a tree viewing people, places, events etc "Semantic Objects"
 * is created and updated in multiple places, it makes sense to collect
 * the code handling it into a central location for easy global updates.
 */
class KORDF_EXPORT KoRdfSemanticTree
{
public:
    KoRdfSemanticTree();
    explicit KoRdfSemanticTree(QTreeWidget *tree);
    KoRdfSemanticTree(const KoRdfSemanticTree &orig);
    KoRdfSemanticTree &operator=(const KoRdfSemanticTree &other);
    ~KoRdfSemanticTree();

    /**
     * Setup the TreeWidget with a default sorting order etc and
     * initialize the people, events and locations base tree Items.
     */
    static KoRdfSemanticTree createTree(QTreeWidget *v);

    /**
     * Update the Items shown in the tree to reflect the Rdf in the
     * given model. if the model is not passed in then any Rdf in the
     * KoDocumentRdf is used to populate the tree.
     */
    void update(KoDocumentRdf *rdf, QSharedPointer<Soprano::Model> model = QSharedPointer<Soprano::Model>(0));

private:
    QExplicitlySharedDataPointer<KoRdfSemanticTreePrivate> d;
};

#endif
