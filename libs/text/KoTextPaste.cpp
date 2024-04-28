/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextPaste.h"

#include <KoCanvasBase.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoSectionModel.h>
#include <KoShape.h>
#include <KoShapeController.h>
#include <KoShapeLoadingContext.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoTextSharedLoadingData.h>
#include <opendocument/KoTextLoader.h>

#include "TextDebug.h"
#ifdef SHOULD_BUILD_RDF
#include "KoTextRdfCore.h"
#include <Soprano/Soprano>
#endif

class Q_DECL_HIDDEN KoTextPaste::Private
{
public:
    Private(KoTextEditor *editor, KoShapeController *shapeCont, QSharedPointer<Soprano::Model> _rdfModel, KoCanvasBase *c, KUndo2Command *cmd)
        : editor(editor)
        , resourceManager(shapeCont->resourceManager())
        , rdfModel(_rdfModel)
        , shapeController(shapeCont)
        , command(cmd)
        , canvas(c)
    {
    }

    KoTextEditor *editor;
    KoDocumentResourceManager *resourceManager;
    QSharedPointer<Soprano::Model> rdfModel;
    KoShapeController *shapeController;
    KUndo2Command *command;
    KoCanvasBase *canvas;
};

KoTextPaste::KoTextPaste(KoTextEditor *editor, KoShapeController *shapeController, QSharedPointer<Soprano::Model> rdfModel, KoCanvasBase *c, KUndo2Command *cmd)
    : d(new Private(editor, shapeController, rdfModel, c, cmd))
{
}

KoTextPaste::~KoTextPaste()
{
    delete d;
}

bool KoTextPaste::process(const KoXmlElement &body, KoOdfReadStore &odfStore)
{
    bool ok = true;
    KoOdfLoadingContext loadingContext(odfStore.styles(), odfStore.store());
    KoShapeLoadingContext context(loadingContext, d->resourceManager);
    context.setSectionModel(KoTextDocument(d->editor->document()).sectionModel());

    KoTextLoader loader(context);

    debugText << "text paste";
    // load the paste directly into the editor's cursor -- which breaks encapsulation
    loader.loadBody(body, *d->editor->cursor(), KoTextLoader::PasteMode); // now let's load the body from the ODF KoXmlElement.

    //     context.sectionModel()->invalidate(); FIXME!!

#ifdef SHOULD_BUILD_RDF
    debugText << "text paste, rdf handling" << d->rdfModel;
    // RDF: Grab RDF metadata from ODF file if present & load it into rdfModel
    if (d->rdfModel) {
        QSharedPointer<Soprano::Model> tmpmodel(Soprano::createModel());
        ok = KoTextRdfCore::loadManifest(odfStore.store(), tmpmodel);
        debugText << "ok:" << ok << " tmpmodel.sz:" << tmpmodel->statementCount();
        debugText << "existing rdf model.sz:" << d->rdfModel->statementCount();
#ifndef NDEBUG
        KoTextRdfCore::dumpModel("RDF from C+P", tmpmodel);
#endif
        d->rdfModel->addStatements(tmpmodel->listStatements().allElements());
        debugText << "done... existing rdf model.sz:" << d->rdfModel->statementCount();
#ifndef NDEBUG
        KoTextRdfCore::dumpModel("Imported RDF after C+P", d->rdfModel);
#endif
    }
#endif

    KoTextSharedLoadingData *sharedData = static_cast<KoTextSharedLoadingData *>(context.sharedData(KOTEXT_SHARED_LOADING_ID));

    // add shapes to the document
    foreach (KoShape *shape, sharedData->insertedShapes()) {
        QPointF move;
        d->canvas->clipToDocument(shape, move);
        if (move.x() != 0 || move.y() != 0) {
            shape->setPosition(shape->position() + move);
        }

        // During load we make page anchored shapes invisible, because otherwise
        // they leave empty rects in the text if there is run-around
        // now is the time to make them visible again
        shape->setVisible(true);

        d->editor->addCommand(d->shapeController->addShapeDirect(shape, d->command));
    }

    return ok;
}
