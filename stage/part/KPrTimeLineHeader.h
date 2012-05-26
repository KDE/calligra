#ifndef KPRTIMELINEHEADER_H
#define KPRTIMELINEHEADER_H

#include <QWidget>
class KPrAnimationsTimeLineView;

class KPrTimeLineHeader : public QWidget
{
    Q_OBJECT
public:
    KPrTimeLineHeader(QWidget *parent);
    QSize minimumSizeHint() const;
    QSize sizeHint() const {return minimumSizeHint(); }
protected:
    void paintEvent(QPaintEvent *event);
    void paintHeader(QPainter *painter, const int RowHeight);
    void paintHeaderItem(QPainter *painter, const QRect &rect, const QString &text);
    void paintTimeScale(QPainter *painter, const QRect &rect);
    bool eventFilter(QObject *target, QEvent *event);
    KPrAnimationsTimeLineView *m_mainView;
};

#endif // KPRTIMELINEHEADER_H
