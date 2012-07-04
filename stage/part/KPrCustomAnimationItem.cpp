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

KPrCustomAnimationItem::KPrCustomAnimationItem(KPrShapeAnimation *animation, KPrCustomAnimationItem *root)
    : m_shapeAnimation(animation)
    ,  m_root(root)
    , isDefaultInitAnimation(false)
    , m_isRootAnimation(false)
    , m_activePage(0)
{
    if (m_shapeAnimation) {
        connect(m_shapeAnimation, SIGNAL(timeChanged(int, int)), this, SIGNAL(timeChanged(int, int)));
        connect(m_shapeAnimation, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this,
                SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)));
    }
}

KPrCustomAnimationItem::~KPrCustomAnimationItem()
{
    m_children.clear();
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

KPrShapeAnimation *KPrCustomAnimationItem::parent() const
{
    if (m_isRootAnimation || !(m_shapeAnimation)) {
        return 0;
    }
    else if (isDefaulAnimation()) {
        return m_root->animation();
    }
    else {
        return m_shapeAnimation->step()->animationAt(0)->children().first();
    }
}

KPrShapeAnimation *KPrCustomAnimationItem::childAt(int row) const
{
    return children().at(row);
}

int KPrCustomAnimationItem::rowOfChild(KPrCustomAnimationItem *child) const
{
    return children().indexOf(child);
}

int KPrCustomAnimationItem::childCount() const
{
    return children().count();
}

QList<KPrShapeAnimation *> KPrCustomAnimationItem::children() const
{
    m_children.clear();
    if (isRootAnimation() && m_activePage) {
        foreach (KPrAnimationStep *step, m_activePage->animationSteps()) {
            for (int i=0; i < step->animationCount(); i++) {
                QAbstractAnimation *animation = step->animationAt(i);
                if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
                    for (int j=0; j < a->animationCount(); j++) {
                        QAbstractAnimation *shapeAnimation = a->animationAt(j);
                        if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                            if ((b->presetClass() != KPrShapeAnimation::None) && (b->shape())) {
                                m_children.append(b);
                            }
                        }
                    }
                }
            }
        }
        return m_children;
    }
    if (m_shapeAnimation->subStep()->indexOfAnimation(m_shapeAnimation) == 0) {
        if (m_shapeAnimation->step()->indexOfAnimation(m_shapeAnimation->subStep()) == 0) {
            KPrAnimationStep *step = m_shapeAnimation->step();
            for (int i=0; i < step->animationCount(); i++) {
                QAbstractAnimation *animation = step->animationAt(i);
                if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
                    for (int j=0; j < a->animationCount(); j++) {
                        QAbstractAnimation *shapeAnimation = a->animationAt(j);
                        if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                            if ((b->presetClass() != KPrShapeAnimation::None) && (b->shape())) {
                                if (b != m_shapeAnimation) {
                                    m_children.append(b);
                                }
                            }

                        }
                    }
                }
            }
            return m_children;
        }
    }
    return m_children;
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

void KPrCustomAnimationItem::initAsDefaultAnimation(KPrPage *page)
{
    if (page) {
        qDeleteAll(m_children);
        m_shapeAnimation = 0;
        m_activePage = page;
        isDefaultInitAnimation = true;
    }
}

bool KPrCustomAnimationItem::isDefaulAnimation()
{
    return isDefaultInitAnimation;
}

void KPrCustomAnimationItem::initAsRootAnimation(KPrPage *page)
{
    m_shapeAnimation = new KPrShapeAnimation(0,0,true);
    m_activePage = page;
    qDeleteAll(m_children);
    m_isRootAnimation = true;
}

bool KPrCustomAnimationItem::isRootAnimation()
{
    return m_isRootAnimation;
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
