#include "KPrAnimationsDataModel.h"

#include <QDebug>
#include <KIconLoader>
#include <KLocale>

KPrAnimationsDataModel::KPrAnimationsDataModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    //Populate temporal model
    AnimationsData data1;
    data1.duration = 2;
    data1.name="shape 1";
    data1.startTime = 0;
    data1.triggerEvent = KPrAnimationsDataModel::onClick;
    data1.type = KPrAnimationsDataModel::entrance;
    data1.thumbnail = KIconLoader::global()->loadIcon(QString("stage"),
                                                      KIconLoader::NoGroup,
                                                      32);
    data1.animationName = i18n("unrecognized animation");
    data1.animationIcon = KIconLoader::global()->loadIcon(QString("unrecognized_animation"),
                                                          KIconLoader::NoGroup,
                                                          32);

    AnimationsData data2;
    data2.duration = 3;
    data2.name="shape 2";
    data2.startTime = 1;
    data2.triggerEvent = KPrAnimationsDataModel::afterPrevious;
    data2.type = KPrAnimationsDataModel::custom;
    data2.thumbnail = KIconLoader::global()->loadIcon(QString("stage"),
                                                      KIconLoader::NoGroup,
                                                      32);
    data2.animationName = i18n("unrecognized animation");
    data2.animationIcon = KIconLoader::global()->loadIcon(QString("unrecognized_animation"),
                                                          KIconLoader::NoGroup,
                                                          32);

    AnimationsData data3;
    data3.duration = 3;
    data3.name="shape 3";
    data3.startTime = 0;
    data3.triggerEvent = KPrAnimationsDataModel::withPrevious;
    data3.type = KPrAnimationsDataModel::exit;
    data3.thumbnail = KIconLoader::global()->loadIcon(QString("stage"),
                                                      KIconLoader::NoGroup,
                                                      32);
    data3.animationName = i18n("unrecognized animation");
    data3.animationIcon = KIconLoader::global()->loadIcon(QString("unrecognized_animation"),
                                                          KIconLoader::NoGroup,
                                                          32);

    m_data.append(data1);
    m_data.append(data2);
    m_data.append(data3);


}

int KPrAnimationsDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

int KPrAnimationsDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 8;
}

QVariant KPrAnimationsDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role == Qt::TextAlignmentRole) {
        return int(Qt::AlignRight | Qt::AlignVCenter);
    } else if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return index.row();
        case 1:
            return m_data.at(index.row()).name;
        case 2:
            return QVariant();
        case 3:
            return QVariant();
        case 4:
            return QVariant();
        case 5:
            return m_data.at(index.row()).startTime;
        case 6:
            return m_data.at(index.row()).duration;
        case 7:
            return m_data.at(index.row()).type;
        default:
            return QVariant();

        }
    } else if (role == Qt::DecorationRole) {
        switch (index.column()) {
        case 2:
            return m_data.at(index.row()).thumbnail;
        case 3:
            return m_data.at(index.row()).animationIcon;
        case 4:
            if (m_data.at(index.row()).triggerEvent == KPrAnimationsDataModel::onClick)
                return KIconLoader::global()->loadIcon(QString("onclick"),
                                                       KIconLoader::NoGroup,
                                                       32);
            if (m_data.at(index.row()).triggerEvent == KPrAnimationsDataModel::afterPrevious)
                return KIconLoader::global()->loadIcon(QString("after_previous"),
                                                       KIconLoader::NoGroup,
                                                       32);
            if (m_data.at(index.row()).triggerEvent == KPrAnimationsDataModel::withPrevious)
                return KIconLoader::global()->loadIcon(QString("with_previous"),
                                                       KIconLoader::NoGroup,
                                                       32);
        default:
            return QVariant();
        }
    } else if (role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;
    } else if (role == Qt::ToolTipRole) {
        switch (index.column()) {
        case 0:
        case 1:
        case 2:
            return QVariant();
        case 3:
            return m_data.at(index.row()).animationName;
        case 4:
            if (m_data.at(index.row()).triggerEvent == KPrAnimationsDataModel::onClick)
                return i18n("start on mouse click");
            if (m_data.at(index.row()).triggerEvent == KPrAnimationsDataModel::afterPrevious)
                return i18n("start after previous animation");
            if (m_data.at(index.row()).triggerEvent == KPrAnimationsDataModel::withPrevious)
                return i18n("start with previous animation");
        case 5:
            return i18n("Start after %1 seconds. Duration of %2 seconds").
                    arg(m_data.at(index.row()).startTime).arg(m_data.at(index.row()).duration);
        case 6:
        case 7:
        default:
            return QVariant();

        }
    }
    return QVariant();
}

QVariant KPrAnimationsDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QVariant();
        case 1:
            return i18n("Seconds");
        case 2:
            return QVariant();
        case 3:
            return QVariant();
        case 4:
            return QVariant();
        case 5:
            return QVariant();
        default:
            return QVariant();
        }

    }
    return QVariant();
}

bool KPrAnimationsDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        switch (index.column()) {
        case 0:
            return false;
        case 1:
            return false;
        case 2:
            return false;
        case 3:
            return false;
        case 4:
            return false;
        case 5:
            m_data[index.row()].startTime = value.toDouble();
            emit dataChanged(index, index);
            return true;
        case 6:
            m_data[index.row()].duration = value.toDouble();
            emit dataChanged(index, index);
            return true;
        case 7:
            return false;
        default:
            return false;

        }
    }
    return false;
}

Qt::ItemFlags KPrAnimationsDataModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    return flags;
}

#include "KPrAnimationsDataModel.moc"
