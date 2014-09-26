/*

    Copyright 2011 Cuong Le <metacuong@gmail.com>

    License is under GPLv2 <http://www.gnu.org/licenses/gpl-2.0.txt>

*/

#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>

class ListItem: public QObject {
  Q_OBJECT

public:
    enum Roles {
        RevisionRole = Qt::UserRole+1,
        Thumb_existsRole,
        BytesRole,
        ModifiedRole,
        PathRole,
        Is_dirRole,
        IconRole,
        Mime_typeRole,
        SizeRole,
        CheckedRole,
        NameRole,
        SectionRole
    };
    ListItem(QObject* parent = 0) : QObject(parent) {}
    virtual ~ListItem() {}
    virtual QString id() const = 0;
    virtual QVariant data(int role) const = 0;
//    virtual QHash<int, QByteArray> roleNames() const = 0;

signals:
    void dataChanged();
};

class ListModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY( int count READ count)

public:
  explicit ListModel(ListItem* prototype, QObject* parent = 0);
  ~ListModel();
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(ListItem* item);
  void appendRows(const QList<ListItem*> &items);
  void insertRow(int row, ListItem* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  ListItem* takeRow(int row);
  ListItem* getRow(int row);
  ListItem* find(const QString &id) const;
  QModelIndex indexFromItem( const ListItem* item) const;
  void clear();

  //QHash<int, QByteArray> roleNames() const;

  int count() const;
  int getCount() { return this->rowCount();}
  Q_INVOKABLE QVariantMap get(int row) const;

private slots:
  void handleItemChange();

signals:
  void countChanged();

private:
  ListItem* m_prototype;
  QList<ListItem*> m_list;
};

#endif // LISTMODEL_H
