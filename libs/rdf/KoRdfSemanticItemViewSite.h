/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_KoRdfSemanticItemViewSite_h__
#define __rdf_KoRdfSemanticItemViewSite_h__

#include "RdfForward.h"
#include "kordf_export.h"
// Soprano
#include <Soprano/Soprano>

class KoCanvasBase;
class KoCanvasResourceManager;

class KoRdfSemanticItemViewSitePrivate;

/**
 * @short Handling a specific reference to a semantic item in the document text.
 * @author Ben Martin <ben.martin@kogmbh.com>
 *
 * There can be many references to a single RdfSemanticItem in a
 * document. RdfSemanticItem is the model, this is the view/controller.
 *
 * For example:
 * foaf pkg:idref frodo1
 * foaf pkg:idref frodo2
 *
 * the foaf/contact data is the KoRdfSemanticItem (model) and each
 * xml:id frodo1 and frodo2 are KoRdfSemanticItemViewSite instances.
 * This class allows different stylesheets to present different
 * formatting for each presentation of the same KoRdfSemanticItem
 * (model).
 */
class KORDF_EXPORT KoRdfSemanticItemViewSite
{
    KoRdfSemanticItemViewSitePrivate *const d;

public:
    /**
     * Performing actions on a specific reference to a semantic item in the document.
     */
    KoRdfSemanticItemViewSite(hKoRdfSemanticItem si, const QString &xmlid);
    ~KoRdfSemanticItemViewSite();

    /**
     * The stylesheet that has been set for this view site
     */
    hKoSemanticStylesheet stylesheet() const;

    /**
     * If there is a stylesheet set for this view site it is forgotten
     * and the reference can be freely edited by the user
     */
    void disassociateStylesheet();

    /**
     * Apply a stylesheet for the semantic item.
     * Note that the setting is remembered for this site too.
     *
     * The application can be done separately using the setStylesheetWithoutReflow()
     * and reflowUsingCurrentStylesheet() methods. Performing the stylesheet
     * application in two parts is convenient if you are applying a stylesheet to many
     * semantic items at once, or to all the locations in the document which reference
     * a single semantic item.
     *
     * @see setStylesheetWithoutReflow()
     * @see reflowUsingCurrentStylesheet()
     */
    void applyStylesheet(KoTextEditor *editor, hKoSemanticStylesheet ss);
    /**
     * Remember that a specific stylesheet should be applied for this
     * semantic item. No reflow of the document is performed and thus
     * no layout or user visible changes occur.
     *
     * @see applyStylesheet()
     */
    void setStylesheetWithoutReflow(hKoSemanticStylesheet ss);

    /**
     * Reflow the text that shows the user this semantic item in the
     * document.
     *
     * @see applyStylesheet()
     */
    void reflowUsingCurrentStylesheet(KoTextEditor *editor);

    /**
     * Select this view of the semantic item in the document.
     */
    void select(KoCanvasBase *canvas);

private:
    /**
     * This is similar to the linkingSubject() used by KoRdfSemanticItem
     * in that you have:
     * linkingSubject() common#idref xml:id
     * but metadata about the site at xml:id is stored as properties
     * off the KoRdfSemanticItemViewSite::linkingSubject() subject.
     *
     * The difference between this linkingSubject() and KoRdfSemanticItem
     * is that this is for Rdf describing a single xml:id site in the document,
     * the KoRdfSemanticItem::linkingSubject() is the model that can be referenced
     * by many xml:id sites.
     */
    Soprano::Node linkingSubject() const;

    /**
     * Convenience method to get a specific property from the Rdf
     * model. There should be either zero or only one value for X in
     * the triple:
     *
     * linkingSubject(), prop, X
     *
     * if the property does not exist defval is returned.
     */
    QString getProperty(const QString &prop, const QString &defval) const;
    /**
     * Set the single value for the Rdf predicate prop.
     * @see getProperty()
     */
    void setProperty(const QString &prop, const QString &v);

    /**
     * Convenience method to select from startpos to endpos in the
     * document
     */
    void selectRange(KoCanvasResourceManager *provider, int startpos, int endpos);
};

#endif
