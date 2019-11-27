/* This file is part of the KDE project
 * Copyright (C) 2000-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2011 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2005-2006, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
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

#ifndef KWFRAMESET_H
#define KWFRAMESET_H

#include "Words.h"
#include "KWFrame.h"
#include "words_export.h"

#include <QObject>

class KWCopyShape;

/**
 * A frameSet holds a number of shapes (1 or more)
 */
class WORDS_EXPORT KWFrameSet : public QObject
{
    Q_OBJECT
public:
    /// Constructor.
    explicit KWFrameSet(Words::FrameSetType type = Words::OtherFrameSet);
    ~KWFrameSet() override;

    /** fetch the frameset of a shape
     * @param shape the shape to fetch the frameset from
     */
    static KWFrameSet *from(KoShape *shape) {KWFrame *f = dynamic_cast<KWFrame*>(shape->applicationData()); return f ? f->frameSetxx() : 0;}

    /**
     * Add a new shape
     * @param shape the new shape to add.
     * @see shapeAdded()
     */
    void addShape(KoShape *shape);

    /**
     * Remove a previously added shape
     * @param shape the shape to be removed
     */
    void removeShape(KoShape *shape);

    /**
     * Give this frameSet a name.
     * Since a frameSet holds content it is logical that the frameset can be given a name for
     * users to look for and use.
     * @param name the new name
     */
    void setName(const QString &name) {
        m_name = name;
    }
    /**
     * Return this framesets name.
     */
    const QString &name() const {
        return m_name;
    }

    /**
     * List all frames this frameset has.  In the order that any content will flow through them.
     */
    const QList<KWFrame*> frames() const {
        QList<KWFrame *> frms;
        foreach (KoShape *shape, m_shapes) {
            frms.append(dynamic_cast<KWFrame *>(shape->applicationData()));
        }

        return frms;
    }

    /**
     * List all shapes this frameset has.  In the order that any content will flow through them.
     */
    const QList<KoShape *> shapes() const {
        return m_shapes;
    }
    /**
     * Return the amount of frames this frameset has.
     */
    int shapeCount() const {
        return m_shapes.count();
    }

    Words::FrameSetType type() const {
        return m_type;
    }

    /**
     * Returns the list of copy-shapes, see @a KWCopyShape
     */
    QList<KWCopyShape *> copyShapes() const;

    void addCopy(KWCopyShape *copyShape);
    void removeCopy(KWCopyShape *copyShape);

Q_SIGNALS:
    /**
     * emitted whenever a shape is added
     * @param shape the shape that has just been added
     */
    void shapeAdded(KoShape *shape);
    /**
     * emitted whenever a shape that was formerly registered is removed
     * @param shape the shape that has just been removed
     */
    void shapeRemoved(KoShape *shape);

protected:
    friend class KWFrame;

    /**
     * Called from addShape
     * Overwrite in inheriting classes to do something with the shape on add.
     * @param shape the shape that has just been added
     */
    virtual void setupShape(KoShape *shape) {
        Q_UNUSED(shape);
    }

    /**
     * @param shape the shape that has just been added
     */
    virtual void cleanupShape(KoShape *shape) {
        Q_UNUSED(shape);
    }

private:
    /// The list of frames that this frameset owns.
    QList<KoShape *> m_shapes;
    /// The type of the frameset.
    Words::FrameSetType m_type;
    /// The name of the frameset.
    QString m_name;
    QList<KWCopyShape *> m_copyShapes;
};

#endif
