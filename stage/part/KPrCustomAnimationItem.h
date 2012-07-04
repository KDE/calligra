#ifndef KPRCUSTOMANIMATIONITEM_H
#define KPRCUSTOMANIMATIONITEM_H

#include "animations/KPrShapeAnimation.h"
#include "stage_export.h"

#include <QPixmap>

/*Data Model for Animations Time Line View
It requires an activePage to be set*/

#include <QObject>

class KPrPage;

/// Basic data structure for KPrAnimationsTreeModel.
/// Keeps pointers of KPrShapeAnimation objects and
/// provides a interface more suitable for the tree and
/// time line view.
class STAGE_EXPORT KPrCustomAnimationItem : public QObject
{
    Q_OBJECT
public:
    explicit KPrCustomAnimationItem(KPrShapeAnimation *animation = 0, KPrCustomAnimationItem *root=0);
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
    qreal startTimeSeconds() const;                            //Animation start time
    int beginTime() const;
    qreal durationSeconds() const;
    int duration() const;
    KoShape *shape() const;

    /// Set start time / animation delay (Takes timeMS in miliseconds)
    void setStartTime(int timeMS);
    /// Set duration (Takes timeMS in miliseconds)
    void setDuration(int timeMS);

    /// Helper methods to manage tre view
    KPrShapeAnimation *parent() const;
    KPrShapeAnimation *childAt(int row) const;
    int rowOfChild(KPrCustomAnimationItem *child) const;
    int childCount() const;
    QList<KPrShapeAnimation *> children() const;
    //void insertChild(int row, KPrCustomAnimationItem *item);
    //void addChild(KPrCustomAnimationItem *item);
    //void swapChildren(int oldRow, int newRow) {m_children.swap(oldRow, newRow); }
    //KPrCustomAnimationItem *takeChild(int row);

    /// Set Item as default (Root item that holds show slide event)
    void initAsDefaultAnimation(KPrPage *page);
    bool isDefaulAnimation();

    void initAsRootAnimation(KPrPage *page);
    bool isRootAnimation();

signals:
    void timeChanged(int begin, int end);
    void triggerEventChanged(KPrShapeAnimation::Node_Type event);

private:

    /// Generates a thumbnail of the animation target (shape)
    QImage createThumbnail(KoShape* shape, const QSize &thumbSize) const;

    KPrShapeAnimation* m_shapeAnimation;          //pointer to target element (shape)
    KPrCustomAnimationItem *m_root;
    QList <KPrShapeAnimation*> m_children;
    bool isDefaultInitAnimation;
    bool m_isRootAnimation;
    KPrPage *m_activePage;
};

#endif // KPRCUSTOMANIMATIONITEM_H
