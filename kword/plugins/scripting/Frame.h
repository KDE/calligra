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

#ifndef SCRIPTING_FRAME_H
#define SCRIPTING_FRAME_H

#include <QObject>
#include <QSizeF>
#include <QPointF>

#include <KWFrame.h>
#include <KoShape.h>

namespace Scripting {

    /**
    * A frame holds a number of \a Frame (zero or more) objects where
    * each frame holds the content that is displayed on screen.
    */
    class Frame : public QObject
    {
            Q_OBJECT
            Q_ENUMS(TextRunAround)
            Q_ENUMS(FrameBehavior)
        public:
            Frame(QObject* parentFrameSet, KWFrame* frame) : QObject(parentFrameSet), m_frame(frame) {}
            virtual ~Frame() {}

            enum TextRunAround {
                NoRunAround = KWord::NoRunAround, ///< The text will be completely avoiding the frame by keeping the horizontal space that this frame occupies blank.
                RunAround = KWord::RunAround, ///< The text will run around the outline of the frame
                RunThrough = KWord::RunThrough ///< The text will completely ignore the frame and layout as if it was not there
            };

            enum FrameBehavior {
                AutoExtendFrameBehavior = KWord::AutoExtendFrameBehavior, ///< Make the frame bigger to fit the contents
                AutoCreateNewFrameBehavior = KWord::AutoCreateNewFrameBehavior, ///< Create a new frame on the next page
                IgnoreContentFrameBehavior = KWord::IgnoreContentFrameBehavior ///< Ignore the content and clip it
            };

        public Q_SLOTS:

            /** Return the Id of this shape, identifying the type of shape by the id of the factory. */
            QString shapeId() const { return m_frame->shape()->shapeId(); }
            /** Return the parent \a FrameSet object this \a Frame object is child of. */
            QObject* frameSet() { return parent(); }

            /** This property what should happen when the frame is full. */
            int frameBehavior() const { return m_frame->frameBehavior(); }
            /** Set what should happen when the frame is full. */
            void setFrameBehavior(int framebehavior) { m_frame->setFrameBehavior( (KWord::FrameBehavior) framebehavior ); }

            /** Return the text runaround property for this frame. This property specifies
            how text from another textframe will behave when this frame intersects with it. */
            int textRunAround() const { return m_frame->textRunAround(); }
            /** Set the text runaround property for this frame. */
            void setTextRunAround(int textrunaround) { return m_frame->setTextRunAround( (KWord::TextRunAround) textrunaround ); }
            /** Return the space between this frames edge and the text when that text runs around this frame. */
            double runAroundDistance() const { return m_frame->runAroundDistance(); }
            /** Set the space between this frames edge and the text when that text runs around this frame. */
            void setRunAroundDistance(double runarounddistance) { m_frame->setRunAroundDistance(runarounddistance); }

            /** Request a repaint to be queued. */
            void repaint() const { m_frame->shape()->repaint(); }

            /** Returns current visibility state of this shape. */
            bool isVisible() const { return m_frame->shape()->isVisible(); }
            /** Changes the Shape to be visible or invisible. */
            void setVisible(bool on) { m_frame->shape()->setVisible(on); }

            /** Return the current scaling adjustment over the X axis. */
            double scaleX() const { return m_frame->shape()->scaleX(); }
            /** Return the current scaling adjustment over the Y axis. */
            double scaleY() const { return m_frame->shape()->scaleY(); }
            /** Scale the shape using the zero-point which is the top-left corner. */
            void scale(double sx, double sy) { m_frame->shape()->scale(sx, sy); }

            /** Return the current rotation in degrees. */
            double rotation() const { return m_frame->shape()->rotation(); }
            /** The shape will be rotated using the center of the shape using the size(). */
            void rotate(double angle) { m_frame->shape()->rotate(angle); }

            /** Return the current horizontal shearing angle for this shape. */
            double shearX() const { return m_frame->shape()->shearX(); }
            /** Return the current vertical shearing angle for this shape. */
            double shearY() const { return m_frame->shape()->shearY(); }
            /** The shape will be sheared using the zero-point which is the top-left corner. */
            void shear(double sx, double sy) { m_frame->shape()->shear(sx, sy); }

            /** Get the width of the shape in pt. */
            double width() const { return m_frame->shape()->size().width(); }
            /** Get the width of the shape in pt. */
            double height() const { return m_frame->shape()->size().height(); }
            /** Resize the shape. */
            void resize(double width, double height) { m_frame->shape()->resize(QSizeF(width,height)); }

            /** Get the X-position of the shape in pt. */
            double positionX() const { return m_frame->shape()->position().x(); }
            /** Get the Y-position of the shape in pt. */
            double positionY() const { return m_frame->shape()->position().y(); }
            /** Set the position of the shape in pt. */
            void setPosition(double x, double y) { m_frame->shape()->setPosition(QPointF(x,y)); }

            /** Retrieve the z-coordinate of this shape. */
            int zIndex() const { return m_frame->shape()->zIndex(); }
            /** Set the z-coordinate of this shape. */
            void setZIndex(int zIndex) { m_frame->shape()->setZIndex(zIndex); }

            /** Return the background color of the shape. */
            QString backgroundColor() const {
                return m_frame->shape()->background().color().name();
            }
            /** Set the background color of the shape. */
            void setBackgroundColor(const QString& color) {
                QBrush brush = m_frame->shape()->background();
                QColor c(color);
                if( c.isValid() ) {
                    brush.setColor(c);
                    m_frame->shape()->setBackground(brush);
                }
            }

        private:
            KWFrame* m_frame;
    };
}

#endif
