/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "InsertTextLocator_p.h"
#include "KoTextEditor.h"

#include <KoCanvasBase.h>

InsertTextLocator::InsertTextLocator(KoCanvasBase *canvas)
    : InsertInlineObjectActionBase(canvas, i18n("Index Reference"))
{
}

KoInlineObject *InsertTextLocator::createInlineObject()
{
    Q_ASSERT(m_canvas);
    KoTextEditor *editor = KoTextEditor::getTextEditorFromCanvas(m_canvas);
    if (editor) {
        KoInlineObject *obj = editor->insertIndexMarker();
        Q_UNUSED(obj); // intentionally unused: if we return it, it gets inserted again
    }
    return nullptr;
}
