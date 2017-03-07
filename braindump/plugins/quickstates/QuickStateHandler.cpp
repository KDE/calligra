/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "QuickStateHandler.h"

#include <QTextCursor>

#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoCanvasBase.h>
#include <KoToolProxy.h>
#include <KoShapeRegistry.h>
#include <KoTextAnchor.h>
#include <KoTextDocumentLayout.h>
#include <KoShapeController.h>
#include <KoProperties.h>
#include <KoInlineTextObjectManager.h>
#include <KoShapeManager.h>

QuickStateHandler::QuickStateHandler(const QString& _catId, const QString& _textId, QObject* _obj) :
    QObject(_obj),
    m_catId(_catId),
    m_textId(_textId)
{

}

QuickStateHandler::~QuickStateHandler()
{
}

void QuickStateHandler::activate()
{
    KoCanvasBase* canvas = KoToolManager::instance()->activeCanvasController()->canvas();
    KoToolSelection* selection = canvas->toolProxy()->selection();
    if(not selection) return;

    KoTextEditor *handler = qobject_cast<KoTextEditor*> (selection);
    if(not handler) return;
    QTextCursor cursor = handler->caret();
    cursor.movePosition(QTextCursor::StartOfBlock);

    KoProperties properties;
    properties.setProperty("category", m_catId);
    properties.setProperty("state", m_textId);
    KoShape* shape = KoShapeRegistry::instance()->get("StateShape")->createShapeAndInit(&properties,  canvas->shapeController()->dataCenterMap());
    Q_ASSERT(shape);
    KoTextAnchor *anchor = new KoTextAnchor(shape);

    KoTextDocumentLayout *layout = dynamic_cast<KoTextDocumentLayout*>(handler->textShapeData()->document()->documentLayout());
    Q_ASSERT(layout);
    Q_ASSERT(layout->inlineTextObjectManager());
    layout->inlineTextObjectManager()->insertInlineObject(cursor, anchor);
    canvas->shapeManager()->add(shape);
}
