#include "StylesSortFilterProxyModel.h"

#include <QDebug>

StylesSortFilterProxyModel::StylesSortFilterProxyModel(QObject *parent)
: QSortFilterProxyModel(parent)
{
}

bool StylesSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData= sourceModel()->data(left, Qt::DisplayRole);
    QVariant rightData = sourceModel()->data(right, Qt::DisplayRole);

    QString leftName = leftData.toString();
    QString rightName = rightData.toString();
    return QString::localeAwareCompare(leftName, rightName) < 0;
}
