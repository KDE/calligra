#ifndef KPRSLIDESMANAGERVIEW_H
#define KPRSLIDESMANAGERVIEW_H

#include <QListView>
class KoToolProxy;

class KPrSlidesManagerView : public QListView
{
    Q_OBJECT
public:
    explicit KPrSlidesManagerView(KoToolProxy *toolProxy, QWidget *parent = 0);

    ~KPrSlidesManagerView();

    virtual void paintEvent ( QPaintEvent * event);

    virtual void contextMenuEvent(QContextMenuEvent *event);

    virtual void mouseDoubleClickEvent(QMouseEvent *event);

    virtual void startDrag ( Qt::DropActions supportedActions );

    virtual void dropEvent(QDropEvent* ev);

    virtual void dragMoveEvent(QDragMoveEvent* ev);

    virtual void dragEnterEvent(QDragEnterEvent *event);

    int pageBefore(QPoint point);

    /**
     * Setter of the size with a rect
     *
     * @param size which is a QRect
     */
    void setItemSize(QRect size);


signals:
    void requestContextMenu(QContextMenuEvent *event);
    void slideDblClick();

private:

    void setLastItemNumber(int number);

    int lastItemNumber();

    /**
     * The rect of an items, essentialy used to have the size of the full icon
     *
     * @return the rect of the item
     */
    QRect itemSize() const;

    /**
     * Setter for the draging flag
     *
     * @param flag boolean
     */
    void setDragingFlag(bool flag = true);

    /**
     * Permit to know if a slide is draging
     *
     * @return boolean
     */
    bool isDraging();

    int m_lastItemNumber;
    QRect m_itemSize;
    bool m_dragingFlag;
    KoToolProxy * m_toolProxy;

};

#endif // KPRSLIDESMANAGERVIEW_H
