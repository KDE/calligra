#include "KPrAnimationGroupProxyModel.h"

KPrAnimationGroupProxyModel::KPrAnimationGroupProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_currentGrup(-1)
{
}

KPrAnimationGroupProxyModel::~KPrAnimationGroupProxyModel()
{
}

bool KPrAnimationGroupProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex currentIndex = sourceModel()->index(source_row, 0, source_parent);
    int currentGroup = sourceModel()->data(currentIndex).toInt();
    return (currentGroup == m_currentGrup) ? true : false;
}

void KPrAnimationGroupProxyModel::setCurrentIndex(const QModelIndex &index)
{
    QModelIndex currentIndex = sourceModel()->index(index.row(), 0);
    int currentGroup = sourceModel()->data(currentIndex).toInt();
    if (currentGroup != m_currentGrup) {
        m_currentGrup = currentGroup;
        invalidateFilter();
    }
}
