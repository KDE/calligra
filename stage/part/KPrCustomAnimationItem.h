#ifndef KPRCUSTOMANIMATIONITEM_H
#define KPRCUSTOMANIMATIONITEM_H

#include "animations/KPrShapeAnimation.h"
#include "stage_export.h"

#include <QPixmap>

/*Data Model for Animations Time Line View
It requires an activePage to be set*/

class KPrPage;

class STAGE_EXPORT KPrCustomAnimationItem
{
public:
    explicit KPrCustomAnimationItem(KPrShapeAnimation *animation = 0, KPrCustomAnimationItem *parent=0);
    ~KPrCustomAnimationItem();
    void setShapeAnimation(KPrShapeAnimation *animation);
    KPrShapeAnimation *animation() const;
    QString name() const;
    QPixmap thumbnail() const;                           //Shape Thumbnail
    QPixmap animationIcon() const;                      //Animation Icon
    QString animationName() const;                      //Animation name
    KPrShapeAnimation::Preset_Class type() const;       //Type: Entrance, exit, custom, etc
    QString typeText() const;
    KPrShapeAnimation::Node_Type triggerEvent() const;  //On click, after previous, etc
    qreal startTime() const;                            //Animation start time
    qreal duration() const;
    KoShape *shape() const;

    KPrCustomAnimationItem *parent() const;
    KPrCustomAnimationItem *childAt(int row) const {return m_children.value(row);}
    int rowOfChild(KPrCustomAnimationItem *child) const {return m_children.indexOf(child); }
    int childCount() const {return m_children.count(); }
    bool hasChildren() const {return !m_children.isEmpty(); }
    QList <KPrCustomAnimationItem *> children() const {return m_children; }
    void insertChild(int row, KPrCustomAnimationItem *item);
    void addChild(KPrCustomAnimationItem *item);
    void swapChildren(int oldRow, int newRow) {m_children.swap(oldRow, newRow); }
    KPrCustomAnimationItem *takeChild(int row);
    void initAsDefaultAnimation(KPrPage *page);
    bool isDefaulAnimation();



private:

    /// Generates a thumbnail of the animation target (shape)
    QImage createThumbnail(KoShape* shape, const QSize &thumbSize) const;

    KPrShapeAnimation* m_shapeAnimation;          //pointer to target element (shape)
    KPrCustomAnimationItem *m_parent;
    QList <KPrCustomAnimationItem*> m_children;
    bool isDefaultInitAnimation;
    KPrPage *m_activePage;
};

#endif // KPRCUSTOMANIMATIONITEM_H
