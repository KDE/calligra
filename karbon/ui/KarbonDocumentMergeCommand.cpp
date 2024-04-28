/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonDocumentMergeCommand.h"
#include "KarbonDocument.h"
#include "KarbonPart.h"

#include "KoShapeLayer.h"
#include <KoPAMasterPage.h>
#include <KoPAPage.h>
#include <KoPAPageBase.h>
#include <KoShape.h>

#include <KLocalizedString>

class MergePageCommand : public KUndo2Command
{
public:
    MergePageCommand(KoPADocument *doc, KoPAPageBase *targetPage, KoPAPageBase *sourcePage, KUndo2Command *parent)
        : KUndo2Command(parent)
        , mine(true)
        , doc(doc)
        , targetPage(targetPage)
    {
        layers = sourcePage->shapes();
        sourcePage->removeAllShapes();
    }
    ~MergePageCommand() override
    {
        if (mine) {
            qDeleteAll(layers);
        }
    }
    void redo() override
    {
        for (int i = 0; i < layers.count(); ++i) {
            targetPage->addShape(layers.at(i));
        }
        mine = false;
        doc->emitUpdate(targetPage);
    }
    void undo() override
    {
        for (int i = 0; i < layers.count(); ++i) {
            targetPage->removeShape(layers.at(i));
        }
        mine = true;
        doc->emitUpdate(targetPage);
    }

private:
    bool mine;
    KoPADocument *doc;
    KoPAPageBase *targetPage;
    QList<KoShape *> layers;
};

class AddPageCommand : public KUndo2Command
{
public:
    AddPageCommand(KarbonDocument *doc, KoPAPageBase *sourcePage, KUndo2Command *parent)
        : KUndo2Command(parent)
        , mine(true)
        , doc(doc)
    {
        newPage = doc->newPage(dynamic_cast<KoPAMasterPage *>(doc->pages(true).value(0)));
        QList<KoShape *> layers = sourcePage->shapes();
        sourcePage->removeAllShapes();
        for (int i = 0; i < layers.count(); ++i) {
            newPage->addShape(layers.at(i));
        }
    }
    ~AddPageCommand() override
    {
        if (mine) {
            delete newPage;
        }
    }
    void redo() override
    {
        doc->insertPage(newPage, doc->pages().count());
        mine = false;
    }
    void undo() override
    {
        doc->takePage(newPage);
        mine = true;
    }

private:
    bool mine;
    KarbonDocument *doc;
    KoPAPageBase *newPage;
};

KarbonDocumentMergeCommand::KarbonDocumentMergeCommand(KarbonDocument *targetPart, KarbonDocument &sourcePart, KUndo2Command *parent)
    : KUndo2Command(parent)
{
    QList<KoPAPageBase *> pages;
    for (int i = 0; i < sourcePart.pages().count(); ++i) {
        KoPAPageBase *sourcePage = sourcePart.pages().at(i);
        pages << sourcePage;
        if (i < targetPart->pages().count()) {
            KoPAPageBase *targetPage = targetPart->pages().at(i);
            new MergePageCommand(targetPart, targetPage, sourcePage, this);
        } else {
            new AddPageCommand(targetPart, sourcePage, this);
        }
    }
    setText(kundo2_i18n("Insert graphics"));
}

void KarbonDocumentMergeCommand::redo()
{
    KUndo2Command::redo();
}

void KarbonDocumentMergeCommand::undo()
{
    KUndo2Command::undo();
}
