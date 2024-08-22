/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "InsertInlineObjectActionBase_p.h"
#include "KoInlineObject.h"
#include "KoTextEditor.h"

#include <KoCanvasBase.h>
#include <QWidget>

#include "TextDebug.h"

InsertInlineObjectActionBase::InsertInlineObjectActionBase(KoCanvasBase *canvas, const QString &name)
    : QAction(name, canvas->canvasWidget())
    , m_canvas(canvas)
{
    connect(this, &QAction::triggered, this, &InsertInlineObjectActionBase::activated);
}

InsertInlineObjectActionBase::~InsertInlineObjectActionBase() = default;

void InsertInlineObjectActionBase::activated()
{
    Q_ASSERT(m_canvas);
    KoTextEditor *editor = KoTextEditor::getTextEditorFromCanvas(m_canvas);
    if (editor) {
        KoInlineObject *obj = createInlineObject();
        if (obj) {
            editor->insertInlineObject(obj);
        }
    }
}
