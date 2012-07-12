#include "KPrCustomAnimationItem.h"

//Qt Headers
#include <QAbstractAnimation>
#include <QPainter>

//Stage Headers
#include <animations/KPrAnimationStep.h>
#include <animations/KPrAnimationSubStep.h>
#include <KPrPage.h>

//Calligra Headers
#include <KoShapePainter.h>
#include <KoShapeContainer.h>

//Kde Headers
#include <KIcon>
#include <KIconLoader>
#include <KLocale>

// Initialize static data
KPrCustomAnimationItem *KPrCustomAnimationItem::m_defaultAnimation = 0;
QHash<KPrShapeAnimation*, KPrCustomAnimationItem*> KPrCustomAnimationItem::m_itemList;

KPrCustomAnimationItem::KPrCustomAnimationItem(KPrShapeAnimation *animation, KPrCustomAnimationItem *root)
    : m_shapeAnimation(animation)
    , m_root(root)
    , isDefaultInitAnimation(false)
    , m_isRootAnimation(false)
    , m_activePage(0)
{
    m_children = QList <KPrCustomAnimationItem*>();
    if (m_shapeAnimation) {
        KPrCustomAnimationItem::addChild(animation, this);
        connect(m_shapeAnimation, SIGNAL(timeChanged(int, int)), this, SIGNAL(timeChanged(int, int)));
        connect(m_shapeAnimation, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this,
                SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)));
        connect(m_shapeAnimation, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    }
}

KPrCustomAnimationItem::~KPrCustomAnimationItem()
{
    m_children.clear();
    if (!m_isRootAnimation && m_root->isRootAnimation() && m_root) {
        KPrCustomAnimationItem::removeChild(this);
    }
    if (m_isRootAnimation) {
        KPrCustomAnimationItem::m_itemList.clear();
    }
}

void KPrCustomAnimationItem::setShapeAnimation(KPrShapeAnimation *animation)
{
    m_shapeAnimation = animation;
    if (isDefaultInitAnimation) {
        isDefaultInitAnimation = false;
    }
}

KPrShapeAnimation *KPrCustomAnimationItem::animation() const
{
    return m_shapeAnimation;
}

QString KPrCustomAnimationItem::name() const
{
    if (m_shapeAnimation) {
        return m_shapeAnimation->shape()->name();
    }
    else if (isDefaultInitAnimation) {
        return m_activePage->name();
    }
    return QString();
}

QPixmap KPrCustomAnimationItem::thumbnail() const
{
    if (m_shapeAnimation) {
        //TODO: Draw image file to load when shape thumbnail can't be created
       QPixmap thumbnail = KIcon("stage").pixmap(KIconLoader::SizeMedium, KIconLoader::SizeMedium);

        if (thumbnail.convertFromImage(createThumbnail(m_shapeAnimation->shape(),
                                                       QSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium)))) {
            thumbnail.scaled(QSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium), Qt::KeepAspectRatio);
        }
        return thumbnail;
    }
    else if (isDefaultInitAnimation) {
        QPixmap thumbnail;
        if (thumbnail.convertFromImage(m_activePage->thumbImage(QSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium)))) {
            return thumbnail;
        }
    }
    return QPixmap();
}

QPixmap KPrCustomAnimationItem::animationIcon() const
{
    //TODO: Parse animation preset Class and read icon name
    //At future these data will be loaded from a XML file
    if (m_shapeAnimation) {
        return KIcon("unrecognized_animation").pixmap(KIconLoader::SizeMedium, KIconLoader::SizeMedium);
    }
    return QPixmap();
}

QString KPrCustomAnimationItem::animationName() const
{
    if (m_shapeAnimation) {
        //TODO: Identify animations supported by stage
        QStringList descriptionList = m_shapeAnimation->id().split("-");
        if (descriptionList.count() > 2) {
            descriptionList.removeFirst();
            descriptionList.removeFirst();
        }
        return descriptionList.join(QString(" "));
    }
    else if (isDefaultInitAnimation) {
        return i18n("Show Slide");
    }
    return QString();
}

KPrShapeAnimation::Preset_Class KPrCustomAnimationItem::type() const
{
    if (m_shapeAnimation) {
        return m_shapeAnimation->presetClass();
    }
    return KPrShapeAnimation::None;
}

QString KPrCustomAnimationItem::typeText() const
{
    if (m_shapeAnimation) {
        return m_shapeAnimation->presetClassText();
    }
    return QString();
}

KPrShapeAnimation::Node_Type KPrCustomAnimationItem::triggerEvent() const
{
    if (m_shapeAnimation) {
        return m_shapeAnimation->NodeType();
    }
    return KPrShapeAnimation::On_Click;
}

qreal KPrCustomAnimationItem::startTimeSeconds() const
{
    if (m_shapeAnimation) {
        return m_shapeAnimation->timeRange().first / 1000.0;
    }
    return 0;
}

int KPrCustomAnimationItem::beginTime() const
{
    if (m_shapeAnimation) {
        return m_shapeAnimation->timeRange().first;
    }
    return 0;
}

qreal KPrCustomAnimationItem::durationSeconds() const
{
    if (m_shapeAnimation) {
        return m_shapeAnimation->globalDuration() / 1000.0;
    }
    return 0;
}

int KPrCustomAnimationItem::duration() const
{
    if (m_shapeAnimation) {
        return m_shapeAnimation->globalDuration();
    }
    return 0;
}

KoShape *KPrCustomAnimationItem::shape() const
{
    if (m_shapeAnimation) {
        return m_shapeAnimation->shape();
    }
    return 0;
}

void KPrCustomAnimationItem::setStartTime(int timeMS)
{
    //Add a command
    if (m_shapeAnimation) {
        m_shapeAnimation->setBeginTime(timeMS);
    }
}

void KPrCustomAnimationItem::setDuration(int timeMS)
{
    if (m_shapeAnimation) {
        m_shapeAnimation->setGlobalDuration(timeMS);
    }
}

KPrCustomAnimationItem *KPrCustomAnimationItem::parent() const
{
    if (isDefaulAnimation()) {
        // "Show slide" event parent
        return m_root;
    }
    else if (m_isRootAnimation || !(m_shapeAnimation)) {
        // Root item has no valid parent
        return 0;
    }
    else {
        if (triggerEvent() == KPrShapeAnimation::On_Click) {
            // All on click events has root as parent
            return m_root;
        }
        else {
            //look for parent in the step substep animation tree
            for (int stepNumber =
                 m_root->activePage()->animationSteps().indexOf(m_shapeAnimation->step());
                 stepNumber > 0; stepNumber--) {
                KPrAnimationStep *step = m_root->activePage()->animationSteps().at(stepNumber);
                for (int i = 0; i < step->animationCount(); i++) {
                    QAbstractAnimation *animation = step->animationAt(i);
                    if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
                        for (int j=0; j < a->animationCount(); j++) {
                            QAbstractAnimation *shapeAnimation = a->animationAt(j);
                            if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                                if ((b->presetClass() != KPrShapeAnimation::None) && (b->shape())) {
                                    if (b->NodeType() == KPrShapeAnimation::On_Click) {
                                        return KPrCustomAnimationItem::itemByAnimation(b);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            // return appropiate parent for childs of "show slide" event
            return KPrCustomAnimationItem::defaultAnimation();
        }
    }
    return 0;
}

KPrCustomAnimationItem *KPrCustomAnimationItem::childAt(int row)
{
    return children().value(row);
}

QList<KPrCustomAnimationItem *> KPrCustomAnimationItem::childrenAt(const int beginRow, const int endRow)
{
    QList<KPrCustomAnimationItem *> allChildren = children();
    Q_ASSERT(beginRow >= 0);
    QList<KPrCustomAnimationItem *> newList = QList<KPrCustomAnimationItem *>();
    if (allChildren.isEmpty())
        return newList;
    Q_ASSERT(endRow < allChildren.count());
    for (int i = beginRow; i <= endRow; i++) {
        newList.append(allChildren.at(i));
    }
    return newList;
}


int KPrCustomAnimationItem::rowOfChild(KPrCustomAnimationItem *child)
{
    return children().indexOf(child);
}

int KPrCustomAnimationItem::childCount()
{
    return children().count();
}

QList<KPrCustomAnimationItem *> KPrCustomAnimationItem::children()
{
    // update children list to always keep in sync with step-substep-animation structure
    m_children.clear();
    // Populate children list for root and "show slide" items
    if ((isRootAnimation() && m_activePage) || isDefaulAnimation()) {
        if (KPrCustomAnimationItem::defaultAnimation() && isRootAnimation()) {
            m_children.append(KPrCustomAnimationItem::defaultAnimation());
        }
        foreach (KPrAnimationStep *step, m_activePage->animationSteps()) {
            for (int i=0; i < step->animationCount(); i++) {
                QAbstractAnimation *animation = step->animationAt(i);
                if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
                    for (int j=0; j < a->animationCount(); j++) {
                        QAbstractAnimation *shapeAnimation = a->animationAt(j);
                        if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                            if ((b->presetClass() != KPrShapeAnimation::None) && (b->shape())) {
                                if (b->NodeType() == KPrShapeAnimation::On_Click) {
                                    if (isRootAnimation()) {
                                        m_children.append(KPrCustomAnimationItem::itemByAnimation(b));
                                    }
                                    else if (isDefaulAnimation()) {
                                        return m_children;
                                    }
                                }
                                if (isDefaulAnimation()) {
                                    m_children.append(KPrCustomAnimationItem::itemByAnimation(b));
                                }
                            }
                        }
                    }
                }
            }
        }
        return m_children;
    }
    // Leafs don't have children
    if (!m_shapeAnimation || (triggerEvent() != KPrShapeAnimation::On_Click)) {
        return m_children;
    }

    // Children list for "On click" animations
    if (m_root->activePage()->animationSteps().indexOf(m_shapeAnimation->step()) == -1) {
        return m_children;
    }
    for (int stepNumber =
         m_root->activePage()->animationSteps().indexOf(m_shapeAnimation->step());
         stepNumber < m_root->activePage()->animationSteps().count(); stepNumber++) {
        KPrAnimationStep *step = m_root->activePage()->animationSteps().at(stepNumber);
        for (int i = 0; i < step->animationCount(); i++) {
            QAbstractAnimation *animation = step->animationAt(i);
            if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
                for (int j=0; j < a->animationCount(); j++) {
                    QAbstractAnimation *shapeAnimation = a->animationAt(j);
                    if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                        if ((b->presetClass() != KPrShapeAnimation::None) && (b->shape())
                                && (b != m_shapeAnimation)) {
                            if (b->NodeType() == KPrShapeAnimation::On_Click) {
                                return m_children;
                            }
                            m_children.append(KPrCustomAnimationItem::itemByAnimation(b));
                        }
                    }
                }
            }
        }
    }
    return m_children;
}

//Methods only used in root animation
void KPrCustomAnimationItem::addChild(KPrShapeAnimation *animation, KPrCustomAnimationItem *item)
{
    m_itemList.insert(animation, item);
}

void KPrCustomAnimationItem::removeChild(KPrCustomAnimationItem *item)
{
    if (m_itemList.isEmpty()) {
        return;
    }
    m_itemList.remove(item->animation());
}

KPrCustomAnimationItem *KPrCustomAnimationItem::itemByAnimation(KPrShapeAnimation *animation)
{
    return m_itemList.value(animation);
}

KPrCustomAnimationItem *KPrCustomAnimationItem::defaultAnimation()
{
    return m_defaultAnimation;
}

void KPrCustomAnimationItem::setDefaultAnimation(KPrCustomAnimationItem *animation)
{
    m_defaultAnimation = animation;
}

KPrPage *KPrCustomAnimationItem::activePage()
{
    Q_ASSERT(m_isRootAnimation);
    return m_activePage;
}

/*
void KPrCustomAnimationItem::insertChild(int row, KPrCustomAnimationItem *item)
{
    item->m_parent = this;
    m_children.insert(row, item);
}

void KPrCustomAnimationItem::addChild(KPrCustomAnimationItem *item)
{
    item->m_parent = this;
    m_children << item;
}

KPrCustomAnimationItem *KPrCustomAnimationItem::takeChild(int row)
{
    KPrCustomAnimationItem *item = m_children.takeAt(row);
    Q_ASSERT(item);
    item->m_parent = 0;
    return item;
}*/

void KPrCustomAnimationItem::initAsDefaultAnimation(KPrPage *activePage)
{
    if (activePage) {
        m_shapeAnimation = 0;
        m_activePage = activePage;
        isDefaultInitAnimation = true;
        KPrCustomAnimationItem::setDefaultAnimation(this);
    }
}

bool KPrCustomAnimationItem::isDefaulAnimation() const
{
    return isDefaultInitAnimation;
}

void KPrCustomAnimationItem::initAsRootAnimation(KPrPage *activePage)
{
    Q_ASSERT(activePage);
    m_shapeAnimation = 0;
    m_activePage = activePage;
    qDeleteAll(children());
    m_isRootAnimation = true;
    m_root = this;
    KPrCustomAnimationItem *newItem;
    //Start defaul event
    newItem = new KPrCustomAnimationItem(0, this);
    newItem->initAsDefaultAnimation(activePage);
    //Load animations
    foreach (KPrAnimationStep *step, activePage->animationSteps()) {
        for (int i=0; i < step->animationCount(); i++) {
            QAbstractAnimation *animation = step->animationAt(i);
            if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
                for (int j=0; j < a->animationCount(); j++) {
                    QAbstractAnimation *shapeAnimation = a->animationAt(j);
                    if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                        if ((b->presetClass() != KPrShapeAnimation::None) && (b->shape())) {
                                newItem = new KPrCustomAnimationItem(b, this);
                        }
                    }
                }
            }
        }
    }
    Q_UNUSED(newItem);
}

bool KPrCustomAnimationItem::isRootAnimation()
{
    return m_isRootAnimation;
}

void KPrCustomAnimationItem::notifyRootModified()
{
    emit rootModified();
}

QImage KPrCustomAnimationItem::createThumbnail(KoShape *shape, const QSize &thumbSize) const
{
    KoShapePainter painter;
    QList<KoShape*> shapes;
    shapes.append(shape);
    KoShapeContainer * container = dynamic_cast<KoShapeContainer*>(shape);
    if (container) {
        shapes.append(container->shapes());
    }

    painter.setShapes(shapes);

    QImage thumb(thumbSize, QImage::Format_RGB32);
    // draw the background of the thumbnail
    thumb.fill(QColor(Qt::white).rgb());

    QRect imageRect = thumb.rect();
    // use 2 pixel border around the content
    imageRect.adjust(2, 2, -2, -2);

    QPainter p(&thumb);
    painter.paint(p, imageRect, painter.contentRect());

    return thumb;
}
