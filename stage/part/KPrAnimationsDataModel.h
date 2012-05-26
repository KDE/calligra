#ifndef KPRANIMATIONSDATAMODEL_H
#define KPRANIMATIONSDATAMODEL_H

#include <QAbstractTableModel>
#include <QPixmap>
#include <QIcon>

#include "stage_export.h"

class STAGE_TEST_EXPORT KPrAnimationsDataModel : public QAbstractTableModel
{
public:
    explicit KPrAnimationsDataModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    enum TriggerEvents {
        onClick,
        afterPrevious,
        withPrevious
    };

    enum Types {
        entrance,
        exit,
        emphasis,
        custom
    };


    struct AnimationsData {   // Declare struct type
        QString name;
        QPixmap thumbnail;
        QPixmap animationIcon;
        QString animationName;
        KPrAnimationsDataModel::Types type;
        KPrAnimationsDataModel::TriggerEvents triggerEvent;
        qreal startTime;
        qreal duration;
    };   // Define object

private:
    QList<AnimationsData> m_data;
};

#endif // KPRANIMATIONSDATAMODEL_H
