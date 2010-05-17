/* This file is part of the KDE project
 * Copyright (C) 2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KWClipFrameCommand.h"
#include "../KWDocument.h"
#include "../KWCanvas.h"
#include "../KWView.h"
#include "../frames/KWFrame.h"

#include <KoShapeContainer.h>
#include <KoShapeManager.h>

#include <KLocale>

class KWFrameClipper : public KoShapeContainer
{
public:
    void paintComponent(QPainter &, const KoViewConverter &) {}
    bool loadOdf(const KoXmlElement &, KoShapeLoadingContext &) { return true;}
    void saveOdf(KoShapeSavingContext &) const {}
};

KWClipFrameCommand::KWClipFrameCommand(const QList<KWFrame*> &frames, KWDocument *document, QUndoCommand *parent)
    : QUndoCommand(i18n("Clip Frame"), parent),
    m_document(document),
    m_frames(frames),
    m_ownClipShapes(false)
{
}

KWClipFrameCommand::~KWClipFrameCommand()
{
    if (m_ownClipShapes)
        qDeleteAll(m_clipShapes);
}

void KWClipFrameCommand::redo()
{
    QUndoCommand::redo();
    bool createClipShapes = m_clipShapes.count() != m_frames.count();
    for (int i = 0; i < m_frames.count(); ++i) {
        KWFrame *frame = m_frames.at(i);
        KoShapeContainer *container;
        if (createClipShapes) {
            container = new KWFrameClipper();
            m_clipShapes.append(container);
            container->setSize(frame->shape()->size());
            container->setTransformation(frame->shape()->transformation());
            container->setZIndex(frame->shape()->zIndex());
        } else {
            container = m_clipShapes.at(i);
        }
        container->addShape(frame->shape());
        container->setClipped(frame->shape(), true);
        frame->shape()->setTransformation(QMatrix());
        foreach (KoView *view, m_document->views()) {
            KWCanvas *canvas = static_cast<KWView*>(view)->kwcanvas();
            canvas->shapeManager()->addShape(container);
        }
    }
    m_ownClipShapes = false;
}

void KWClipFrameCommand::undo()
{
    QUndoCommand::undo();
    Q_ASSERT(m_clipShapes.count() == m_frames.count());

    for (int i = 0; i < m_frames.count(); ++i) {
        KWFrame *frame = m_frames.at(i);
        KoShapeContainer *container = m_clipShapes.at(i);
        Q_ASSERT(frame->shape()->parent() == container);
        QPointF pos = frame->shape()->absolutePosition();
        frame->shape()->setParent(0);
        frame->shape()->setAbsolutePosition(pos);
        frame->shape()->setTransformation(container->transformation());
        foreach (KoView *view, m_document->views()) {
            KWCanvas *canvas = static_cast<KWView*>(view)->kwcanvas();
            canvas->shapeManager()->remove(container);
        }
    }
    m_ownClipShapes = true; // we have to delete them
}
