/* This file is part of the KOffice project
 * Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
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

#ifndef SCRIPTING_FRAMESET_H
#define SCRIPTING_FRAMESET_H

#include <QPointer>
#include <QObject>
#include <QTextDocument>
#include <kdebug.h>

#include "TextDocument.h"
#include "Frame.h"

#include <KoShapeRegistry.h>
#include <KoShapeFactory.h>

#include <KoTextShape.h>
#include <KoStyleManager.h>
//#include <KoParagraphStyle.h>
//#include <KoCharacterStyle.h>
//#include <KoListStyle.h>

#include <KWFrame.h>
#include <KWTextFrame.h>
#include <KWFrameSet.h>
#include <KWTextFrameSet.h>

namespace Scripting {

    /**
    * A frameset holds a number of \a Frame (zero or more) objects where
    * each frame holds the content that is displayed on screen.
    */
    class FrameSet : public QObject
    {
            Q_OBJECT
        public:
            FrameSet(QObject* parent, KWFrameSet* frameset)
                : QObject(parent), m_frameset(frameset) {}
            virtual ~FrameSet() {}

        public Q_SLOTS:

            /** Return this framesets name. */
            const QString name() { return m_frameset->name(); }
            /** Set the framesets name. */
            void setName(const QString &name) { m_frameset->setName(name); }

            /** Return the number of frames this frameset has. */
            int frameCount() { return m_frameset->frames().count(); }
            /** Return the \a Frame object with index \p frameNr or NULL if there exists no \a Frame with such a index. */
            QObject* frame(int frameNr) {
                if( frameNr >= 0 && frameNr < m_frameset->frames().count() )
                    return new Frame(this, m_frameset->frames().at(frameNr));
                return 0;
            }

            /** Add and return a new \a Frame to this frameset using the shape
            defined with the \p shapeId identifier. */
            QObject* addFrame(const QString& shapeId) {
                KoShapeFactory *factory = KoShapeRegistry::instance()->get(shapeId);
                if( ! factory ) {
                    kWarning(32010) << "Scripting::Module::addFrame() Invalid shapeId: " << shapeId << endl;
                    return 0;
                }
                KoShape *shape = factory->createDefaultShape();
                Q_ASSERT(shape);
                shape->setZIndex( 100 + m_frameset->frameCount() );
                KWFrame* frame = 0;
                KWTextFrameSet* textframeset = dynamic_cast<KWTextFrameSet*>( (KWFrameSet*)m_frameset );
                if( textframeset )
                    frame = new KWTextFrame(shape, textframeset);
                else
                    frame = new KWFrame(shape, m_frameset);
                return new Frame(this, frame);
            }

            /** Add and return a new \a Frame to this frameset which has the
            shapeId KoTextShape_SHAPEID and is used to display the \a TextDocument . */
            QObject* addTextFrame() { return addFrame(KoTextShape_SHAPEID); }

            //void addFrame(KWFrame *frame);
            //void removeFrame(KWFrame *frame);

            /** Return the \a TextDocument object or NULL if this frameset does not have a \a TextDocument object. */
            QObject* textDocument() {
                KWTextFrameSet* textframeset = dynamic_cast< KWTextFrameSet* >( (KWFrameSet*)m_frameset );
                return textframeset ? new TextDocument(this, textframeset->document()) : 0;
            }

        private:
            QPointer<KWFrameSet> m_frameset;
    };

}

#endif
