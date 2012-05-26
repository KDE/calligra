#ifndef KPRANIMATIONSTIMELINEVIEW_H
#define KPRANIMATIONSTIMELINEVIEW_H

#include <QWidget>
class QTableView;
class KPrAnimationsDataModel;
class KPrTimeLineView;
class QScrollArea;
class QModelIndex;
class TimeLineWidget;
class KPrTimeLineHeader;
class QColor;

class KPrAnimationsTimeLineView : public QWidget
{
    Q_OBJECT
public:
    explicit KPrAnimationsTimeLineView(QWidget *parent = 0);
    void setModel(KPrAnimationsDataModel *model);
    void resizeEvent(QResizeEvent *event);
    KPrAnimationsDataModel *model();
    int widthOfColumn(int column) const;
    void setSelectedRow(int row);
    void setSelectedColumn(int column);
    void setCurrentIndex(const QModelIndex &index);
    int rowsHeigth() const;
    int totalWidth() const;
    int selectedRow() const {return m_selectedRow;}
    int selectedColumn() const {return m_selectedColumn;}
    void paintItemBorder(QPainter *painter, const QPalette &palette, const QRect &rect);
    QScrollArea *scrollArea() const;
    int numberOfSteps() const;
    void setNumberOfSteps(int steps);
    void incrementScale();
    void adjustScale();
    int stepsScale();
    qreal maxLineLength() const;
    void setMaxLineLength(qreal length);
    QColor colorforRow(int row);
    int rowCount() const;



signals:
    void clicked(const QModelIndex&);
public slots:
;

private:
    KPrTimeLineView *m_view;
    KPrTimeLineHeader *m_header;
    KPrAnimationsDataModel *m_model;
    int m_selectedRow;
    int m_selectedColumn;
    QScrollArea *m_scrollArea;
    int m_rowsHeigth;
    int m_stepsNumber;
    int m_scaleOversize;
    int m_nameWidth;
    int m_orderColumnWidth;
    int m_shapeNameColumnWidth;
    qreal m_maxLength;
};

#endif // KPRANIMATIONSTIMELINEVIEW_H
