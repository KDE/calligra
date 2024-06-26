/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __koDocumentRdfEditWidget_h__
#define __koDocumentRdfEditWidget_h__

#include "RdfForward.h"
#include "kordf_export.h"
// Calligra
#include <KoPageWidgetItem.h>
// Qt
#include <QWidget>

class KoDocumentRdf;
class KoSemanticStylesheet;
class QComboBox;

/**
 * @short A widget to let the user view and edit the Rdf for with the document
 * @author Ben Martin <ben.martin@kogmbh.com>
 * @see KoDocumentRdf
 *
 * This is initially used by the KoDocumentInfoDlg class to add a new page
 * that shows you and editable interface for your Rdf. The class was kept
 * as its own QWidget so that it can be moved to other dialogs and or shown
 * in many places.
 *
 * The widget lets the user edit Rdf that is stored both inline in content.xml
 * as well as from manifest.rdf and other external Rdf files from the OASIS
 * document.
 *
 */
class KORDF_EXPORT KoDocumentRdfEditWidget : public QWidget, public KoPageWidgetItem
{
    Q_OBJECT
public:
    /**
     * The constructor
     * @param docRdf a pointer to the KoDocumentRdf to show/edit
     */
    explicit KoDocumentRdfEditWidget(KoDocumentRdf *docRdf);

    /** The destructor */
    virtual ~KoDocumentRdfEditWidget();

    /** Add this widget to a user interface where you want Rdf editing */
    QWidget *widget();

    bool shouldDialogCloseBeVetoed();

    /** OK button in dialog, if this returns false then do not close the dialog */
    void apply();

    QString name() const;
    QString iconName() const;

public Q_SLOTS:

    /**
     * Create a new triple in the model and UI in the Triples page.
     */
    void addTriple();

    /**
     * copy the triples which are currently selected in the Triples
     * page. new triples will have a unique identifier appended to
     * their object to avoid attempting to insert the same
     * subj,pred,obj twice.
     */
    void copyTriples();

    /**
     * Delete the selected triples in the Triples page
     */
    void deleteTriples();

    /**
     * Create a new namespace
     */
    void addNamespace();

    /**
     * Delete the selected namespaces
     */
    void deleteNamespace();

    /**
     * Execute the SPARQL query the user has provided.
     *
     * query is taken from m_ui->m_sparqlQuery
     * results are added to m_ui->m_sparqlResultView
     */
    void sparqlExecute();

private Q_SLOTS:

    /**
     * This methods set the default stylesheet to the
     * user selection for each type of KoRdfSemanticItem.
     */
    void onDefaultSheetButtonClicked();
    void onDefaultAllSheetButtonClicked();

    /**
     * Show a context menu for the semantic treeview
     */
    void showSemanticViewContextMenu(const QPoint &at);

    /**
     * The user edited a semantic item, update the view.
     */
    void semanticObjectUpdated(hKoRdfBasicSemanticItem item);

private:
    hKoSemanticStylesheet stylesheetFromComboBox(QComboBox *w) const;
    void applyStylesheetFromComboBox(QComboBox *comboBox) const;

private:
    class KoDocumentRdfEditWidgetPrivate;
    KoDocumentRdfEditWidgetPrivate *const d;
};

#endif
