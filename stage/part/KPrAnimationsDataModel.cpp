#include "KPrAnimationsDataModel.h"

#include <QDebug>
#include <KIconLoader>
#include <KLocale>
#include <KPrPage.h>
#include "KoShape.h"
#include "KoShapeContainer.h"
#include "KoShapePainter.h"
#include <QImage>
#include <QPainter>
#include <animations/KPrAnimationStep.h>
#include "animations/KPrAnimationSubStep.h"
#include <QAbstractAnimation>
#include <KoPADocument.h>
#include <KPrView.h>
#include <KoShapeManager.h>

KPrAnimationsDataModel::KPrAnimationsDataModel(QObject *parent) :
    QAbstractTableModel(parent)
  , m_activePage(0)
{
    m_data.clear();
}

QModelIndex KPrAnimationsDataModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!m_activePage) {
        return QModelIndex();
    }

    // check if parent is root node
    if(!parent.isValid()) {
        if(row >= 0 && row < rowCount(QModelIndex())) {
            return createIndex(row, column, m_data.at(row).shape);
        }
    }
    return QModelIndex();
}

QModelIndex KPrAnimationsDataModel::indexByShape(KoShape *shape)
{
        int row = -1;
        foreach (AnimationsData data, m_data) {
            row++;
            if (shape == data.shape)
                return index(row, 1);
        }
        return index(row,1);
}

int KPrAnimationsDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);   
    return m_data.count();
}

int KPrAnimationsDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 8;
}

QVariant KPrAnimationsDataModel::data(const QModelIndex &index, int role) const
{
    if (!m_activePage)
        return QVariant();
    if (!index.isValid())
        return QVariant();
    if (role == Qt::TextAlignmentRole) {
        return int(Qt::AlignRight | Qt::AlignVCenter);
    } else if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            //TODO: Only return a number when animation starts on click
            return (m_data.at(index.row()).order == 0) ? QVariant() : m_data.at(index.row()).order;
        case 1:
            return m_data.at(index.row()).name;
        case 2:
            return QVariant();
        case 3:
            return QVariant();
        case 4:
            return QVariant();
        case 5:
            return m_data.at(index.row()).startTime;
        case 6:
            return m_data.at(index.row()).duration;
        case 7:
            return m_data.at(index.row()).type;
        default:
            return QVariant();

        }
    } else if (role == Qt::DecorationRole) {
        switch (index.column()) {
        case 2:
            return m_data.at(index.row()).thumbnail;
        case 3:
            return m_data.at(index.row()).animationIcon;
        case 4:
            if (m_data.at(index.row()).triggerEvent == KPrShapeAnimation::On_Click)
                return KIconLoader::global()->loadIcon(QString("onclick"),
                                                       KIconLoader::NoGroup,
                                                       32);
            if (m_data.at(index.row()).triggerEvent == KPrShapeAnimation::After_Previous)
                return KIconLoader::global()->loadIcon(QString("after_previous"),
                                                       KIconLoader::NoGroup,
                                                       32);
            if (m_data.at(index.row()).triggerEvent == KPrShapeAnimation::With_Previous)
                return KIconLoader::global()->loadIcon(QString("with_previous"),
                                                       KIconLoader::NoGroup,
                                                       32);
        default:
            return QVariant();
        }
    } else if (role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;
    } else if (role == Qt::ToolTipRole) {
        switch (index.column()) {
        case 0:
        case 1:
        case 2:
            return QVariant();
        case 3:
            return m_data.at(index.row()).animationName;
        case 4:
            if (m_data.at(index.row()).triggerEvent == KPrShapeAnimation::On_Click)
                return i18n("start on mouse click");
            if (m_data.at(index.row()).triggerEvent == KPrShapeAnimation::After_Previous)
                return i18n("start after previous animation");
            if (m_data.at(index.row()).triggerEvent == KPrShapeAnimation::With_Previous)
                return i18n("start with previous animation");
        case 5:
            return i18n("Start after %1 seconds. Duration of %2 seconds").
                    arg(m_data.at(index.row()).startTime).arg(m_data.at(index.row()).duration);
        case 6:
        case 7:
        default:
            return QVariant();

        }
    }
    return QVariant();
}

QVariant KPrAnimationsDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!m_activePage)
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QVariant();
        case 1:
            return i18n("Seconds");
        case 2:
            return QVariant();
        case 3:
            return QVariant();
        case 4:
            return QVariant();
        case 5:
            return QVariant();
        default:
            return QVariant();
        }

    }
    return QVariant();
}

bool KPrAnimationsDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        switch (index.column()) {
        case 0:
            return false;
        case 1:
            return false;
        case 2:
            return false;
        case 3:
            return false;
        case 4:
            return false;
        case 5:
            //TODO: save new value in animation step.
            /*
            m_data[index.row()].startTime = value.toDouble();
            emit dataChanged(index, index);
            return true;
        case 6:
            m_data[index.row()].duration = value.toDouble();
            emit dataChanged(index, index);
            return true;
            */
        case 7:
            return false;
        default:
            return false;

        }
    }
    return false;
}

Qt::ItemFlags KPrAnimationsDataModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    return flags;
}

void KPrAnimationsDataModel::setActivePage(KPrPage *activePage)
{
    Q_ASSERT(activePage);
    m_activePage = activePage;
    m_data.clear();
    m_steps = activePage->animationSteps();
    int k = 0;
    int l = 1;
    foreach (KPrAnimationStep *step, m_steps) {
        qDebug() << "Step count" << step->animationCount();
        for (int i=0; i < step->animationCount(); i++) {
            QAbstractAnimation *animation = step->animationAt(i);
            if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
                qDebug() << "substep count" << a->animationCount();

                for (int j=0; j < a->animationCount(); j++) {
                    QAbstractAnimation *shapeAnimation = a->animationAt(j);

                    if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                        k++;
                        qDebug() << "is shape in view: " << m_view->shapeManager()->shapes().contains(b->shape());
                        if ((b->presetClass() != KPrShapeAnimation::None) && (m_view->shapeManager()->shapes().contains(b->shape()))) {
                            AnimationsData data1;
                            //Load start and end time, convert them to seconds
                            QPair <int, int> timeRange = b->timeRange();
                            qDebug() << "Start at: " << timeRange.first << "end at: " << timeRange.second;
                            data1.startTime = timeRange.first/1000.0;
                            data1.duration = timeRange.second/1000.0;
                            data1.triggerEvent = b->NodeType();

                            if (data1.triggerEvent == KPrShapeAnimation::On_Click) {
                                data1.order = l;
                                l++;
                            }
                            else {
                                data1.order = 0;
                            }

                            qDebug() << "node type: " << b->NodeType();
                            data1.type = b->presetClass();
                            setNameAndAnimationIcon(data1, b->id());

                            //TODO: Draw image file to load when shape thumbnail is not loaded
                            data1.thumbnail = KIconLoader::global()->loadIcon(QString("stage"),
                                                                              KIconLoader::NoGroup,
                                                                              KIconLoader::SizeMedium);
                            data1.shape = b->shape();
                            if (!b->shape()->name().isEmpty()) {
                                data1.name = b->shape()->name();
                            }
                            else {
                                data1.name=i18n("Shape %1").arg(k);
                            }

                            QPixmap thumbnail;
                            if (thumbnail.convertFromImage(createThumbnail(b->shape(),
                                                                           QSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium)))) {
                                thumbnail.scaled(QSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium), Qt::KeepAspectRatio);
                            }
                            data1.thumbnail = thumbnail;

                            m_data.append(data1);
                            qDebug() << "Preset in model: " << b->presetClassText();
                            qDebug() << b->id();
                        }

                    }
                }
            }
        }
    }
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

QImage KPrAnimationsDataModel::createThumbnail(KoShape* shape, const QSize &thumbSize) const
{
    KoShapePainter painter;

    QList<KoShape*> shapes;

    shapes.append(shape);
    KoShapeContainer * container = dynamic_cast<KoShapeContainer*>(shape);
    if (container)
        shapes.append(container->shapes());

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

void KPrAnimationsDataModel::setNameAndAnimationIcon(KPrAnimationsDataModel::AnimationsData &data, QString id)
{
    //TODO: Identify animations supported by stage
    QStringList descriptionList = id.split("-");
    if (descriptionList.count() > 2) {
        descriptionList.removeFirst();
        descriptionList.removeFirst();
    }
    qDebug() << descriptionList;
    data.animationName = descriptionList.join(QString(" "));
    //TODO: Parse animation preset Class and read icon name
    data.animationIcon = KIconLoader::global()->loadIcon(QString("unrecognized_animation"),
                                                          KIconLoader::NoGroup,
                                                          KIconLoader::SizeMedium);
}

void KPrAnimationsDataModel::setDocumentView(KPrView *view)
{
    m_view = view;
    if (m_view->kopaDocument())
    {
        connect(m_view->kopaDocument(), SIGNAL(shapeRemoved(KoShape*)), this, SLOT(update()));
        connect(m_view->kopaDocument(), SIGNAL(shapeAdded(KoShape*)), this, SLOT(update()));
    }
    reset();
}

void KPrAnimationsDataModel::update()
{
    qDebug() << "update animations model called";
    setActivePage(m_activePage);
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}


