#ifndef KPRANIMATIONGROUPPROXYMODEL_H
#define KPRANIMATIONGROUPPROXYMODEL_H

#include <QSortFilterProxyModel>

#include "stage_export.h"

class STAGE_EXPORT KPrAnimationGroupProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit KPrAnimationGroupProxyModel(QObject *parent = 0);
    ~KPrAnimationGroupProxyModel();

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

    void setCurrentIndex(const QModelIndex &index);
    
signals:
    
public slots:
private:
    int m_currentGrup;
};

#endif // KPRANIMATIONGROUPPROXYMODEL_H
