/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_KoSemanticStylesheetsEditor_h__
#define __rdf_KoSemanticStylesheetsEditor_h__

#include "RdfForward.h"
#include "kordf_export.h"
// Calligra
#include <KoDialog.h>
// Qt
#include <QSharedPointer>

class QTreeWidgetItem;
class QTableWidgetItem;

/**
 * @short A dialog to allow the user to see the system stylesheet definitions
 *        and create and edit user stylesheets.
 *
 * @author Ben Martin <ben.martin@kogmbh.com>
 * @see KoDocument
 */
class KORDF_EXPORT KoSemanticStylesheetsEditor : public KoDialog
{
    Q_OBJECT
public:
    KoSemanticStylesheetsEditor(QWidget *parent, KoDocumentRdf *rdf);
    ~KoSemanticStylesheetsEditor();

protected Q_SLOTS:
    void slotOk();
    void currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void newStylesheet();
    void deleteStylesheet();
    void onVariableActivated(QTableWidgetItem *item);
    void definitionChanged();

private:
    void setupStylesheetsItems(const QString &semanticClass,
                               hKoRdfSemanticItem si,
                               const QList<hKoSemanticStylesheet> &ssl,
                               const QMap<QString, QTreeWidgetItem *> &m,
                               bool editable = false);
    void maskButtonsDependingOnCurrentItem(QTreeWidgetItem *current);

    class Private;
    QSharedPointer<Private> d;
};

#endif
