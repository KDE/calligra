/* This file is part of the KDE project
 * Copyright (C) 2011 Smit Patel <smitpatel24@gmail.com>
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
#ifndef KOINLINECITE_H
#define KOINLINECITE_H

#include "KoInlineObject.h"
#include "kotext_export.h"

#include "KoXmlReaderForward.h"

#include <QTextFrame>

class KoShapeLoadingContext;
class KoChangeTracker;
class KoStyleManager;

class QTextCursor;

/**
 * This object is an inline object, which means it is anchored in the text-flow and it can hold
 * bibliography-mark(citation).
 */
class KOTEXT_EXPORT KoInlineCite : public KoInlineObject
{
    Q_OBJECT
public:
    enum Type {
        Citation,
        ClonedCitation                  //cloned from other citation in document
    };
    /**
     * Construct a new cite to be inserted in the text using KoTextSelectionHandler::insertInlineObject() for example.
     */
    explicit KoInlineCite(Type type);

    virtual ~KoInlineCite();

    bool operator!= (const KoInlineCite &cite) const;

    KoInlineCite &operator= (const KoInlineCite &cite);

    Type type() const;        //return type of cite

    void setType(Type t);

    bool hasSameData(KoInlineCite *cite) const;

    void copyFrom(KoInlineCite *cite);

    void setField(QString fieldName, QString fieldValue);

    void setFields(const QMap<QString, QString> &fields);

    QString value(const QString &fieldName) const;

    QMap<QString, QString> fieldMap() const;

    int posInDocument() const;

    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);

    ///reimplemented
    void saveOdf(KoShapeSavingContext &context);

protected:
    /// reimplemented
    virtual void updatePosition(const QTextDocument *document, int posInDocument, const QTextCharFormat &format);
    /// reimplemented
    virtual void resize(const QTextDocument *document, QTextInlineObject object,
                        int posInDocument, const QTextCharFormat &format, QPaintDevice *pd);
    /// reimplemented
    virtual void paint(QPainter &painter, QPaintDevice *pd, const QTextDocument *document,
                       const QRectF &rect, QTextInlineObject object, int posInDocument, const QTextCharFormat &format);

private:
    class Private;
    Private * const d;

};

#endif // KOINLINECITE_H
