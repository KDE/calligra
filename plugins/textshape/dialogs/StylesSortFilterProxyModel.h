#ifndef STYLESSORTFILTERPROXYMODEL_H
#define STYLESSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class StylesSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    explicit StylesSortFilterProxyModel(QObject *parent = 0);

protected:
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif /* STYLESSORTFILTERPROXYMODEL_H */
