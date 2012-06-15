#ifndef KPRANIMATIONSTREEMODEL_H
#define KPRANIMATIONSTREEMODEL_H

#include <QAbstractItemModel>
#include "stage_export.h"

class KPrPage;
class KPrView;
class KPrCustomAnimationItem;
class KoShape;

class STAGE_EXPORT KPrAnimationsTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit KPrAnimationsTreeModel(QObject *parent=0);
    ~KPrAnimationsTreeModel();
    void clear();

    Qt::ItemFlags flags(const QModelIndex &index) const;
     QVariant data(const QModelIndex &index,
                   int role=Qt::DisplayRole) const;
     QVariant headerData(int section, Qt::Orientation orientation,
                         int role=Qt::DisplayRole) const;
     int rowCount(const QModelIndex &parent=QModelIndex()) const;
     int columnCount(const QModelIndex &parent=QModelIndex()) const;
     QModelIndex index(int row, int column,
                       const QModelIndex &parent=QModelIndex()) const;
     QModelIndex parent(const QModelIndex &index) const;

     bool setHeaderData(int, Qt::Orientation, const QVariant&,
                        int=Qt::EditRole) { return false; }
     bool setData(const QModelIndex &index, const QVariant &value,
                  int role=Qt::EditRole);
     bool insertRows(int row, int count,
                     const QModelIndex &parent=QModelIndex());
     bool removeRows(int row, int count,
                     const QModelIndex &parent=QModelIndex());
 /*
     Qt::DropActions supportedDragActions() const
         { return Qt::MoveAction; }
     Qt::DropActions supportedDropActions() const
         { return Qt::MoveAction; }
     QStringList mimeTypes() const;
     QMimeData *mimeData(const QModelIndexList &indexes) const;
     bool dropMimeData(const QMimeData *mimeData,
             Qt::DropAction action, int row, int column,
             const QModelIndex &parent);

     bool hasCutItem() const { return cutItem; }
 */
     QModelIndex moveUp(const QModelIndex &index);
     QModelIndex moveDown(const QModelIndex &index);
     QModelIndex cut(const QModelIndex &index);
     QModelIndex paste(const QModelIndex &index);
     QModelIndex promote(const QModelIndex &index);
     QModelIndex demote(const QModelIndex &index);


    //An active page is required before use the model
    void setActivePage(KPrPage *activePage);

    //requiere to update model if a shape is removed (or added with undo)
    void setDocumentView(KPrView *view);

    /// Return the first animation index for the given shape
    QModelIndex indexByShape(KoShape* shape);
    QModelIndex indexByItem(KPrCustomAnimationItem* item);

    KPrCustomAnimationItem* rootItem() const;


public slots:
    /// Triggers an update of the complete model
    void updateData();
private:
    KPrCustomAnimationItem* itemForIndex(const QModelIndex &index) const;
    void announceItemChanged(KPrCustomAnimationItem *item);
    QModelIndex moveItem(KPrCustomAnimationItem *parent, int oldRow, int newRow);

    KPrPage *m_activePage;
    KPrView *m_view;
    KPrCustomAnimationItem *m_rootItem;
    KPrCustomAnimationItem *cutItem;
};

#endif // KPRANIMATIONSTREEMODEL_H
