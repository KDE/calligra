/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef STYLESFILTEREDMODELBASE_H
#define STYLESFILTEREDMODELBASE_H

#include "AbstractStylesModel.h"

#include <QVector>

/** This class serves as a base for filtering an @class AbstractStylesmodel. The base class implements a one to one mapping of the model.
 * Reimplementing the method createMapping is sufficient for basic sorting/filtering.
 *
 * QSortFilterProxyModel implementation was a great source of inspiration.
 *
 * It is to be noted that this is in no way a full proxyModel. It is built with several assumptions:
 * - it is used to filter a StylesModel which in turn is a flat list of items. There is only one level of items. (this also means that "parent" QModelIndexes
 * are always invalid)
 * - there is no header in the model.
 * - the model has only one column
 * - only the following methods are used when updating the underlying model's data: resetModel, insertRows, moveRows, removeRows (cf QAbstractItemModel)
 */

class StylesFilteredModelBase : public AbstractStylesModel
{
    Q_OBJECT
public:
    explicit StylesFilteredModelBase(QObject *parent = nullptr);

    /** Re-implement from QAbstractItemModel. */

    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int columnCount(const QModelIndex &parent) const override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /** Specific methods of the AbstractStylesModel */

    /** Sets the @class KoStyleThumbnailer of the model. It is required that a @param thumbnailer is set before using the model. */
    void setStyleThumbnailer(KoStyleThumbnailer *thumbnailer) override;

    /** Return a @class QModelIndex for the specified @param style.
     * @param style may be either a character or paragraph style.
     */
    QModelIndex indexOf(const KoCharacterStyle *style) const override;

    /** Returns a QImage which is a preview of the style specified by @param row of the given @param size.
     * If size isn't specified, the default size of the given @class KoStyleThumbnailer is used.
     */
    QImage stylePreview(int row, const QSize &size = QSize()) override;
    //    virtual QImage stylePreview(QModelIndex &index, const QSize &size = QSize());

    AbstractStylesModel::Type stylesType() const override;

    /** Specific methods of the StylesFiltermodelBase */

    /** Sets the sourceModel. Setting the model will trigger the mapping.
     */
    void setStylesModel(AbstractStylesModel *sourceModel);

protected Q_SLOTS:
    void slotModelAboutToBeReset();
    void slotModelReset();
    void slotRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
    void slotRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
    void slotRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void slotRowsInserted(const QModelIndex &parent, int start, int end);
    void slotRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
    void slotRowsRemoved(const QModelIndex &parent, int start, int end);

protected:
    virtual void createMapping();

    AbstractStylesModel *m_sourceModel;

    QVector<int> m_sourceToProxy;
    QVector<int> m_proxyToSource;
};

#endif // STYLESFILTEREDMODELBASE_H
