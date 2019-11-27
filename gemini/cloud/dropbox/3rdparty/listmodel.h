/*

    Copyright 2011 Cuong Le <metacuong@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
    ~ListItem() override {}
    virtual QString id() const = 0;
    virtual QVariant data(int role) const = 0;

Q_SIGNALS:
    void dataChanged();
};

class ListModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY( int count READ count)

public:
  explicit ListModel(ListItem* prototype, QObject* parent = 0);
  ~ListModel() override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  void appendRow(ListItem* item);
  void appendRows(const QList<ListItem*> &items);
  void insertRow(int row, ListItem* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
  ListItem* takeRow(int row);
  ListItem* getRow(int row);
  ListItem* find(const QString &id) const;
  QModelIndex indexFromItem( const ListItem* item) const;
  void clear();

  int count() const;
  int getCount() { return this->rowCount();}
  Q_INVOKABLE QVariantMap get(int row) const;

private Q_SLOTS:
  void handleItemChange();

Q_SIGNALS:
  void countChanged();

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  ListItem* m_prototype;
  QList<ListItem*> m_list;
};

#endif // LISTMODEL_H
