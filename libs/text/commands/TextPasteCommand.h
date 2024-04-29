// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef TEXTPASTECOMMAND_H
#define TEXTPASTECOMMAND_H

#include <QClipboard>
#include <QPointer>
#include <kundo2command.h>

class QTextDocument;
class KoDocumentRdfBase;
class KoShapeController;
class QMimeData;
class KoCanvasBase;

class TextPasteCommand : public KUndo2Command
{
public:
    TextPasteCommand(const QMimeData *mimeData,
                     QTextDocument *document,
                     KoShapeController *shapeController,
                     KoCanvasBase *canvas,
                     KUndo2Command *parent = nullptr,
                     bool pasteAsText = false);

    void undo() override;

    void redo() override;

private:
    const QMimeData *m_mimeData;
    QPointer<QTextDocument> m_document;
    KoDocumentRdfBase *m_rdf;
    KoShapeController *m_shapeController;
    KoCanvasBase *m_canvas;
    bool m_pasteAsText;
    bool m_first;
};

#endif // TEXTPASTECOMMAND_H
