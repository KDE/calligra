#ifndef KPREDITANIMATIONSWIDGET_H
#define KPREDITANIMATIONSWIDGET_H

#include <QWidget>

class KPrAnimationsTimeLineView;
class KPrAnimationsDataModel;
class KPrView;
class KoPAViewBase;
class QModelIndex;
class KoShape;
class QModelIndex;
class KPrCustomAnimationItem;

class KPrEditAnimationsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KPrEditAnimationsWidget(QWidget *parent = 0);
    void setView(KoPAViewBase *view);
    void setParentItem(KPrCustomAnimationItem *item, KPrCustomAnimationItem *rootItem);
    void setCurrentIndex(const QModelIndex &index);
    void setActiveShape(KoShape *shape);
    
signals:
    void itemClicked(const QModelIndex &index);

public slots:

private:
    KPrView* m_view;
    KPrAnimationsTimeLineView *m_timeLineView;
    KPrAnimationsDataModel *m_timeLineModel;
    
};

#endif // KPREDITANIMATIONSWIDGET_H
