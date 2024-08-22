/* This file is part of the Calligra project, made within the KDE community.
 *
 * SPDX-FileCopyrightText: 2013 Friedrich W. H. Kossebau <friedrich@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TextDocumentInspectionDocker.h"

#include "KoTextShapeData.h"
#include "TextDocumentStructureModel.h"
#include "TextShape.h"
//
#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
// Qt
#include <QDebug>
#include <QTreeView>

TextDocumentInspectionDocker::TextDocumentInspectionDocker(QWidget *parent)
    : QDockWidget(parent)
    , m_canvas(nullptr)
    , m_mainWidget(new QTreeView(this))
    , m_textDocumentStructureModel(new TextDocumentStructureModel(this))
{
    setWindowTitle(QLatin1String("TextDocument Inspector"));
    setWidget(m_mainWidget);

    //     m_mainWidget->setRootIsDecorated(false);
    m_mainWidget->setAllColumnsShowFocus(true);
    m_mainWidget->setUniformRowHeights(true);
    m_mainWidget->setAlternatingRowColors(true);
    m_mainWidget->setModel(m_textDocumentStructureModel);
}

TextDocumentInspectionDocker::~TextDocumentInspectionDocker() = default;

void TextDocumentInspectionDocker::setCanvas(KoCanvasBase *canvas)
{
    setEnabled(canvas != nullptr);

    if (m_canvas) {
        m_canvas->disconnectCanvasObserver(this);
    }

    m_canvas = canvas;
    if (!m_canvas) {
        m_textDocumentStructureModel->setTextDocument(nullptr);
        return;
    }

    connect(m_canvas->shapeManager(), &KoShapeManager::selectionChanged, this, &TextDocumentInspectionDocker::onShapeSelectionChanged);

    onShapeSelectionChanged();
}

void TextDocumentInspectionDocker::unsetCanvas()
{
    setEnabled(false);
    m_textDocumentStructureModel->setTextDocument(nullptr);
    m_canvas = nullptr;
}

void TextDocumentInspectionDocker::onShapeSelectionChanged()
{
    QTextDocument *textDocument = nullptr;

    // TODO: big fail: shapeManager of a canvas still emits signals after unsetCanvas()
    // was called on us. And at least by the current API dox there is no way in unsetCanvas()
    // to get the shapeManager anymore, as "it's dead".
    // So we need to guard us here, this can be called when we officially are not
    // connected to any canvas.
    // Cmp. StrokeDocker::selectionChanged()
    if (m_canvas) {
        KoShape *shape = m_canvas->shapeManager()->selection()->firstSelectedShape();
        if (shape) {
            TextShape *textShape = dynamic_cast<TextShape *>(shape);
            if (textShape) {
                textDocument = textShape->textShapeData()->document();
            }
        }
    }

    m_textDocumentStructureModel->setTextDocument(textDocument);
    m_mainWidget->expandToDepth(1);
}
