#ifndef PRESETMODEL_H
#define PRESETMODEL_H

#include <QAbstractListModel>

class PresetModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum PresetRoles
    {
        ImageRole = Qt::UserRole + 1,
        TextRole
    };

    explicit PresetModel(QObject *parent = 0);
    virtual ~PresetModel();

    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    class Private;
    Private* d;
};

#endif // PRESETMODEL_H
