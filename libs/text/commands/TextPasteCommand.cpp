// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
// SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "TextPasteCommand.h"

#include <KoParagraphStyle.h>
#include <KoShapeController.h>
#include <KoText.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoTextPaste.h>

#include "TextDebug.h"
#include <KLocalizedString>

#include <QMimeData>
#include <QTextDocument>

#include "DeleteCommand.h"
#include "KoDocumentRdfBase.h"

#ifdef SHOULD_BUILD_RDF
#include <Soprano/Soprano>
#else
namespace Soprano
{
class Model
{
};
}
#endif

TextPasteCommand::TextPasteCommand(const QMimeData *mimeData,
                                   QTextDocument *document,
                                   KoShapeController *shapeController,
                                   KoCanvasBase *canvas,
                                   KUndo2Command *parent,
                                   bool pasteAsText)
    : KUndo2Command(parent)
    , m_mimeData(mimeData)
    , m_document(document)
    , m_rdf(nullptr)
    , m_shapeController(shapeController)
    , m_canvas(canvas)
    , m_pasteAsText(pasteAsText)
    , m_first(true)
{
    m_rdf = qobject_cast<KoDocumentRdfBase *>(shapeController->resourceManager()->resource(KoText::DocumentRdf).value<QObject *>());

    if (m_pasteAsText)
        setText(kundo2_i18n("Paste As Text"));
    else
        setText(kundo2_i18n("Paste"));
}

void TextPasteCommand::undo()
{
    KUndo2Command::undo();
}

void TextPasteCommand::redo()
{
    if (m_document.isNull())
        return;

    KoTextDocument textDocument(m_document);
    KoTextEditor *editor = textDocument.textEditor();

    if (!m_first) {
        KUndo2Command::redo();
    } else {
        editor->beginEditBlock(); // this is needed so Qt does not merge successive paste actions together
        m_first = false;
        if (editor->hasSelection()) { // TODO
            editor->addCommand(new DeleteCommand(DeleteCommand::NextChar, m_document.data(), m_shapeController, this));
        }

        // check for mime type
        if (m_mimeData->hasFormat(KoOdf::mimeType(KoOdf::Text)) || m_mimeData->hasFormat(KoOdf::mimeType(KoOdf::OpenOfficeClipboard))) {
            KoOdf::DocumentType odfType = KoOdf::Text;
            if (!m_mimeData->hasFormat(KoOdf::mimeType(odfType))) {
                odfType = KoOdf::OpenOfficeClipboard;
            }

            if (editor->blockFormat().hasProperty(KoParagraphStyle::HiddenByTable)) {
                editor->insertText("");
            }

            if (m_pasteAsText) {
                editor->insertText(m_mimeData->text());
            } else {
                QSharedPointer<Soprano::Model> rdfModel;
#ifdef SHOULD_BUILD_RDF
                if (!m_rdf) {
                    rdfModel = QSharedPointer<Soprano::Model>(Soprano::createModel());
                } else {
                    rdfModel = m_rdf->model();
                }
#endif

                KoTextPaste paste(editor, m_shapeController, rdfModel, m_canvas, this);
                paste.paste(odfType, m_mimeData);

#ifdef SHOULD_BUILD_RDF
                if (m_rdf) {
                    m_rdf->updateInlineRdfStatements(editor->document());
                }
#endif
            }
        } else if (m_pasteAsText || m_mimeData->hasText()) {
            editor->insertText(m_mimeData->text());
        }
        editor->endEditBlock(); // see above beginEditBlock
    }
}
