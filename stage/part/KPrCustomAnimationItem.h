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
/// time line view of slide animations.
/// Tree structure has 2 levels (beside the main root)
/// "Show slide" Event
///   |_ After/With previous animations
/// "On click Animation"
///   |_ After/With previous animations
class STAGE_EXPORT KPrCustomAnimationItem : public QObject
{
    Q_OBJECT
public:
    explicit KPrCustomAnimationItem(KPrShapeAnimation *animation = 0, KPrCustomAnimationItem *root=0);
    ~KPrCustomAnimationItem();

    /// Initialize item to hold a shapeAnimation
    void setShapeAnimation(KPrShapeAnimation *animation);

    /// Quick access to KPrShapeAnimation properties:
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
    KPrCustomAnimationItem *parent() const;
    KPrCustomAnimationItem *childAt(int row);
    QList<KPrCustomAnimationItem *> childrenAt(const int beginRow, const int endRow);
    int rowOfChild(KPrCustomAnimationItem *child);
    int childCount();
    QList<KPrCustomAnimationItem *> children();
    //void insertChild(int row, KPrCustomAnimationItem *item);
    static void addChild(KPrShapeAnimation *animation, KPrCustomAnimationItem *item);
    static void removeChild(KPrCustomAnimationItem *item);
    static KPrCustomAnimationItem *itemByAnimation(KPrShapeAnimation *animation);
    static KPrCustomAnimationItem *defaultAnimation();
    static void setDefaultAnimation(KPrCustomAnimationItem *animation);
    KPrPage *activePage();

    //void swapChildren(int oldRow, int newRow) {m_children.swap(oldRow, newRow); }

    /** Set item as "show slide" default event
        It must be the firs element of the animations list
    */
    void initAsDefaultAnimation(KPrPage *activePage);
    bool isDefaulAnimation() const;

    /** Set Item as tree root (Root item that holds all other animations)
     Used to start the data structure and create list for quick
     search of childs and parents
    */
    void initAsRootAnimation(KPrPage *activePage);
    bool isRootAnimation();

    void notifyRootModified();

signals:
    void timeChanged(int begin, int end);
    void triggerEventChanged(KPrShapeAnimation::Node_Type event);
    void rootModified();

private:

    /// Generates a thumbnail of the animation target (shape)
    QImage createThumbnail(KoShape* shape, const QSize &thumbSize) const;

    KPrShapeAnimation* m_shapeAnimation;          //pointer to target element (shape)
    KPrCustomAnimationItem *m_root;
    static KPrCustomAnimationItem *m_defaultAnimation;
    QList <KPrCustomAnimationItem*> m_children;
    static QHash<KPrShapeAnimation*, KPrCustomAnimationItem*> m_itemList;

    bool isDefaultInitAnimation;
    bool m_isRootAnimation;
    KPrPage *m_activePage;
};

#endif // KPRCUSTOMANIMATIONITEM_H
