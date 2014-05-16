#ifndef STORAGELISTMODEL_H
#define STORAGELISTMODEL_H

#include <QtCore/QAbstractListModel>

class StorageListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* kritaSteamClient READ kritaSteamClient WRITE setKritaSteamClient NOTIFY kritaSteamClientChanged)
    Q_PROPERTY(qint64 maxCapacity READ maxCapacity NOTIFY maxCapacityChanged)
    Q_PROPERTY(qint64 usedCapacity READ usedCapacity NOTIFY usedCapacityChanged)
public:
    enum PresetRoles {
        ImageRole = Qt::UserRole + 1,
        FilePathRole,
        FileNameRole,
        FileSizeRole,
        CheckedRole
    };

    explicit StorageListModel(QObject *parent = 0);
    virtual ~StorageListModel();
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QObject* kritaSteamClient();
    void setKritaSteamClient(QObject* kritaSteamClient);

    qint64 maxCapacity();
    qint64 usedCapacity();

    Q_INVOKABLE void requestIndexForFilename(const QString& filename);

signals:
    void kritaSteamClientChanged();
    void filenameFound(int index);
    void maxCapacityChanged();
    void usedCapacityChanged();

public slots:

private:
    class Private;
    Private* d;
};

#endif // STORAGELISTMODEL_H
