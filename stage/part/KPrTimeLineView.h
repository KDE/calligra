#ifndef KPRTIMELINEVIEW_H
#define KPRTIMELINEVIEW_H

#include <QWidget>

class KPrAnimationsTimeLineView;
class QGraphicsView;
class QGraphicsScene;
class KPrTimeLineHeader;
class TimeLineItem;
class QAbstractItemModel;
class QModelIndex;


class KPrTimeLineView: public QWidget
{
     Q_OBJECT
public:
    KPrTimeLineView(QWidget* parent = 0);
signals:
    void clicked(const QModelIndex&);
    void scaleLimitReached();

private:
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    bool eventFilter(QObject *target, QEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool event(QEvent *event);

    int rowAt(int ypos);
    int columnAt(int xpos);

    void paintEvent(QPaintEvent * event);
    void paintRow(QPainter *painter, int  row, int y, const int RowHeight);
    void paintItemBackground(QPainter *painter, const QRect &rect, bool selected);
    void paintLine(QPainter *painter, int row, const QRect &rect, bool selected);
    void paintTextRow(QPainter *painter, int x, int y, int row, int column, const int RowHeight);
    void paintIconRow(QPainter *painter, int x, int y, int row, int column, int iconSize, const int RowHeight);
    double modD(double x, double y);

    QGraphicsView *m_view;
    QGraphicsScene *m_scene;
    KPrTimeLineHeader *m_header;
    qreal m_start;
    qreal m_end;
    qreal m_realEnd;
    int m_height, m_steps;
    int m_scroll;
    QAbstractItemModel *m_model;
    QList<TimeLineItem *> m_lines;
    int m_current;
    int m_rowHeight;
    int m_visibleHeight;
    KPrAnimationsTimeLineView *m_mainView;
    bool m_resize;
    bool m_move;
    int m_resizedRow;
    int startDragPos;
    bool m_adjust;

    friend class KPrAnimationsTimeLineView;
};

#endif // KPRTIMELINEVIEW_H
