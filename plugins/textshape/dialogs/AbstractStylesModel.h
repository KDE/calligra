/* This file is part of the KDE project
 * Copyright (C) 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
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

#ifndef ABSTRACTSTYLESMODEL_H
#define ABSTRACTSTYLESMODEL_H

#include <QAbstractListModel>
#include <QSize>

class KoStyleThumbnailer;

class KoParagraphStyle;
class KoCharacterStyle;

class QImage;

/** This class is used to provide widgets (like the @class StylesCombo) the styles available to the document being worked on.
 *
 * This is an abstract class supposed to be inherited only. DO NOT instentiate this class directly.
 *
 * On top of the standard QAbstractItemModel methods to re-implement, there are 4 specific methods which need to be re-implemented in order to be used with the styles widgets:
 * - setStyleThumbnailer: a @class KoStyleThumbnailer is used to layout/draw a preview of the style
 * - indexForParagraphStyle: for a given paragraph style, returns corresponding the QModelIndex
 * - indexForCharacterStyle: as above for character styles
 * - stylePreview: returns a QImage, preview of the given style (given as row number in the list model)
 *
 * Following assumptions are made:
 * - the AbstractStylesModel derived model is a flat list of items (this also means that "parent" QModelIndexes are always invalid)
 * - the AbstractStylesModel derived model has only one column
 * - there is no header in the AbstractStylesModel derived model
*/

class AbstractStylesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AbstractStylesModel(QObject *parent = 0);

    /** Re-implement from QAbstractItemModel. */

    virtual QModelIndex index(int row, int column=0, const QModelIndex &parent = QModelIndex()) const = 0;

    virtual int rowCount(const QModelIndex &parent) const = 0;

    virtual QVariant data(const QModelIndex &index, int role) const = 0;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const = 0;

    /** Specific methods of the AbstractStylesModel */

    /** Sets the @class KoStyleThumbnailer of the model. It is required that a @param thumbnailer is set before using the model. */
    virtual void setStyleThumbnailer(KoStyleThumbnailer *thumbnailer) = 0;

    /** Return a @class QModelIndex for the specified @param style. */
    virtual QModelIndex indexForParagraphStyle(const KoParagraphStyle &style) const = 0;

    /** Return a @class QModelIndex for the specified @param style. */
    virtual QModelIndex indexForCharacterStyle(const KoCharacterStyle &style) const = 0;

    /** Returns a QImage which is a preview of the style specified by @param row of the given @param size.
      * If size isn't specified, the default size of the given @class KoStyleThumbnailer is used.
    */
    virtual QImage stylePreview(int row, QSize size = QSize()) = 0;

private:
    KoStyleThumbnailer *m_styleThumbnailer;
};

#endif // ABSTRACTSTYLESMODEL_H
