/* This file is part of the KDE project
 * Copyright (C) 2006-2009 Thomas Zander <zander@kde.org>
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
#ifndef KOTEXTRANGE_H
#define KOTEXTRANGE_H

#include "kotext_export.h"

#include <QVariant>
#include <QObject>

class QTextDocument;
class QTextCharFormat;
class QTextCursor;
class QPaintDevice;
class QPainter;
class QRectF;

class KoTextRangeManager;
class KoTextRangePrivate;
class KoShapeSavingContext;
class KoTextInlineRdf;
class KoXmlElement;
class KoShapeLoadingContext;

/**
 * Base class for all text ranges.
 *
 * They are essentially anchored to a specific position or range in the text
 *
 * @see KoTextRangeManager
 */
class KOTEXT_EXPORT KoTextRange : public QObject
{
    Q_OBJECT
public:

    /**
     * constructor
     */
    explicit KoTextRange(const QTextCursor &cursor);
    virtual ~KoTextRange();

    /**
     * Will be called by the manager when this variable is added.
     * Remember that inheriting classes should not use the manager() in the constructor, since it will be 0
     * @param manager the object manager for this object.
     */
    void setManager(KoTextRangeManager *manager);

    /**
     * Return the object manager set on this inline object.
     */
    KoTextRangeManager *manager();

    /**
     * Save the part of this text range corresponding to position as ODF
     * @param context the context for saving.
     * @param position a position in the qtextdocument we are currently saving for.
     */
    virtual void saveOdf(KoShapeSavingContext &context, int position) const = 0;

    /**
     * Update position of the inline object.
     * This is called each time the paragraph this inline object is in is re-layouted giving you the opportunity
     * to reposition your object based on the new information.
     * @param document the text document this inline object is operating on.
     * @param posInDocument the character position in the document (param document) this inline object is at.
     * @param format the character format for the inline object.
     */
    virtual void updatePosition(const QTextDocument *document, int posInDocument, const QTextCharFormat &format) = 0;

    /**
     * Paint the inline-object-base using the provided painter within the rectangle specified by rect.
     * @param painter the painting object to paint on.  Note that unline many places in calligra painting
     * @param pd the postscript-paintdevice that all text is rendered on. Use this for QFont and related
     * @param document the text document this inline object is operating on.
     * @param rect the rectangle inside which the variable can paint itself.  Painting outside the rect
     *    will give varous problems with regards to repainting issues.
     */
    virtual void paint(QPainter &painter, QPaintDevice *pd, const QTextDocument *document,
                       const QRectF &rect) = 0;


    QTextCursor &cursor();

    bool positionOnlyMode() const;
    void setPositionOnlyMode(bool m);

    bool hasSelection() const;
    int selectionStart() const;
    int selectionEnd() const;

    /// return the inline-object Id that is assigned for this object.
    int id() const;

    /// Set the inline-object Id that is assigned for this object by the KoTextRangeManager.
    void setId(int id);

    /**
     * An text range might have some Rdf metadata associated with it
     * in content.xml
     * Ownership of the rdf object is taken by the text range, and you should not
     * delete it.
     */
    void setInlineRdf(KoTextInlineRdf *rdf);

    /**
     * Get any Rdf which was stored in content.xml for this text range
     */
    KoTextInlineRdf *inlineRdf() const;

    /**
     * Load a variable from odf.
     *
     * @param element element which represents the shape in odf
     * @param context the KoShapeLoadingContext used for loading
     *
     * @return false if loading failed
     */
    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) =  0;

protected:
    KoTextRangePrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(KoTextRange)
};

KOTEXT_EXPORT QDebug operator<<(QDebug dbg, const KoTextRange *o);

#endif
