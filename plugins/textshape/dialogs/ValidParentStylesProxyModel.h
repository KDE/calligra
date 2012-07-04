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

#ifndef VALIDPARENTSTYLESPROXYMODEL_H
#define VALIDPARENTSTYLESPROXYMODEL_H

#include "AbstractStylesModel.h"

#include <QVector>

class KoStyleManager;

class QModelIndex;

/** This class is acting as a proxy between a "real" StylesModel and views. It is used to filter out styles which would not make a valid parent of the given childStyle (see void setCurrentChildStyleId(int styleId) ).
 *
 * In that matter, QSortFilterProxyModel implementation was a great source of inspiration.
 *
 * It is to be noted that this is in no way a full proxyModel. It is built with several assumptions:
 * - it is used to filter a StylesModel which in turn is a flat list of items. There is only one level of items. (this also means that "parent" QModelIndexes are always invalid)
 * - there is no header in the model.
 * - the model has only one column
 * - only the following methods are used when updating the underlying model's data: resetModel, insertRows, moveRows, removeRows (cf QAbstractItemModel)
*/

class ValidParentStylesProxyModel : public AbstractStylesModel
{
    Q_OBJECT
public:
    ValidParentStylesProxyModel(QObject *parent = 0);

    /** Re-implement from QAbstractItemModel. */

    virtual QModelIndex index(int row, int column=0, const QModelIndex &parent = QModelIndex()) const;

    virtual int rowCount(const QModelIndex &parent) const;

    virtual QVariant data(const QModelIndex &index, int role) const;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    /** Re-implement from AbstractStylesModel */

    /** Sets the @class KoStyleThumbnailer of the model. It is required that a @param thumbnailer is set before using the model. */
    virtual void setStyleThumbnailer(KoStyleThumbnailer *thumbnailer);

    /** Return a @class QModelIndex for the specified @param style. */
    virtual QModelIndex indexForParagraphStyle(const KoParagraphStyle &style) const;

    /** Return a @class QModelIndex for the specified @param style. */
    virtual QModelIndex indexForCharacterStyle(const KoCharacterStyle &style) const;

    /** Returns a QImage which is a preview of the style specified by @param row of the given @param size.
      * If size isn't specified, the default size of the given @class KoStyleThumbnailer is used.
    */
    virtual QImage stylePreview(int row, QSize size = QSize());

    /** Sets the style model to be proxied. */
    void setStylesModel(AbstractStylesModel *model);

    void setStyleManager(KoStyleManager *manager); //it is needed to travel the style's parenting chain in order to assess if the currentChildStyle is suitable for being a parent
    void setCurrentChildStyleId(int styleId);

private slots:
    void modelAboutToBeReset();
    void modelReset();
    void rowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
    void rowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
    void rowsRemoved(const QModelIndex &parent, int start, int end);

private:
    void createMapping();

    KoStyleManager *m_styleManager;
    AbstractStylesModel *m_sourceModel;

    int m_currentChildStyleId;
    QVector<int> m_sourceToProxy;
    QVector<int> m_proxyToSource;
};

#endif // VALIDPARENTSTYLESPROXYMODEL_H
