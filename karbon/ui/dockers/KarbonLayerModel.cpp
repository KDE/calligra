/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KarbonLayerModel.h"

#include <KarbonDocument.h>
#include <KoShapePainter.h>

#include <KoShapeManager.h>
#include <KoShapeBorderModel.h>
#include <KoShapeContainer.h>
#include <KoToolManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoShapeControllerBase.h>
#include <KoSelection.h>
#include <KoZoomHandler.h>
#include <KoShapeLayer.h>
#include <KoShapeGroup.h>
#include <KoShapeGroupCommand.h>
#include <KoShapeUngroupCommand.h>

#include <klocale.h>
#include <kicon.h>
#include <kiconloader.h>
#include <kdebug.h>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QMimeData>

KarbonLayerModel::KarbonLayerModel(QObject * parent)
        : KoDocumentSectionModel(parent), m_document(0)
{
    setSupportedDragActions(Qt::MoveAction);
}

void KarbonLayerModel::update()
{
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

void KarbonLayerModel::setDocument(KarbonDocument * newDocument)
{
    m_document = newDocument;
    reset();
}

int KarbonLayerModel::rowCount(const QModelIndex &parent) const
{
    if (! m_document)
        return 0;

    // check if parent is root node
    if (! parent.isValid())
        return m_document->layers().count();

    Q_ASSERT(parent.model() == this);
    Q_ASSERT(parent.internalPointer());

    KoShapeContainer *parentShape = dynamic_cast<KoShapeContainer*>((KoShape*)parent.internalPointer());
    if (! parentShape)
        return 0;

    return parentShape->shapeCount();
}

int KarbonLayerModel::columnCount(const QModelIndex &) const
{
    if (! m_document)
        return 0;
    else
        return 1;
}

QModelIndex KarbonLayerModel::index(int row, int column, const QModelIndex &parent) const
{
    if (! m_document)
        return QModelIndex();

    // check if parent is root node
    if (! parent.isValid()) {
        if (row >= 0 && row < m_document->layers().count())
            return createIndex(row, column, m_document->layers().at(row));
        else
            return QModelIndex();
    }

    Q_ASSERT(parent.model() == this);
    Q_ASSERT(parent.internalPointer());

    KoShapeContainer *parentShape = dynamic_cast<KoShapeContainer*>((KoShape*)parent.internalPointer());
    if (! parentShape)
        return QModelIndex();

    if (row < parentShape->shapeCount())
        return createIndex(row, column, childFromIndex(parentShape, row));
    else
        return QModelIndex();
}

QModelIndex KarbonLayerModel::parent(const QModelIndex &child) const
{
    // check if child is root node
    if (! m_document || ! child.isValid())
        return QModelIndex();

    Q_ASSERT(child.model() == this);
    Q_ASSERT(child.internalPointer());

    KoShape *childShape = static_cast<KoShape*>(child.internalPointer());
    if (! childShape)
        return QModelIndex();

    return parentIndexFromShape(childShape);

    // check if child shape is a layer, and return invalid model index if it is
    KoShapeLayer *childlayer = dynamic_cast<KoShapeLayer*>(childShape);
    if (childlayer)
        return QModelIndex();

    // get the children's parent shape
    KoShapeContainer *parentShape = childShape->parent();
    if (! parentShape)
        return QModelIndex();

    // check if the parent is a layer
    KoShapeLayer *parentLayer = dynamic_cast<KoShapeLayer*>(parentShape);
    if (parentLayer)
        return createIndex(m_document->layers().indexOf(parentLayer), 0, parentShape);

    // get the grandparent to determine the row of the parent shape
    KoShapeContainer *grandParentShape = parentShape->parent();
    if (! grandParentShape)
        return QModelIndex();

    return createIndex(indexFromChild(grandParentShape, parentShape), 0, parentShape);
}

QVariant KarbonLayerModel::data(const QModelIndex &index, int role) const
{
    if (! index.isValid())
        return QVariant();

    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalPointer());

    KoShape *shape = static_cast<KoShape*>(index.internalPointer());

    switch (role) {
    case Qt::DisplayRole: {
        QString name = shape->name();
        if (name.isEmpty()) {
            if (dynamic_cast<KoShapeLayer*>(shape))
                name = i18n("Layer");
            else if (dynamic_cast<KoShapeGroup*>(shape))
                name = i18nc("A group of shapes", "Group");
            else
                name = i18n("Shape");
        }
        return name;// + QString(" (%1)").arg( shape->zIndex() );
    }
    case Qt::DecorationRole: return QVariant();//return shape->icon();
    case Qt::EditRole: return shape->name();
    case Qt::SizeHintRole: return shape->size();
    case ActiveRole: {
        KoCanvasController * canvasController = KoToolManager::instance()->activeCanvasController();
        KoSelection * selection = canvasController->canvas()->shapeManager()->selection();
        if (! selection)
            return false;

        KoShapeLayer *layer = dynamic_cast<KoShapeLayer*>(shape);
        if (layer)
            return (layer == selection->activeLayer());
        else
            return selection->isSelected(shape);
    }
    case PropertiesRole: return QVariant::fromValue(properties(shape));
    case AspectRatioRole: {
        QMatrix matrix = shape->absoluteTransformation(0);
        QRectF bbox = matrix.mapRect(shape->outline().boundingRect());
        KoShapeContainer *container = dynamic_cast<KoShapeContainer*>(shape);
        if (container) {
            bbox = QRectF();
            foreach(KoShape* shape, container->shapes())
            bbox = bbox.united(shape->outline().boundingRect());
        }
        return qreal(bbox.width()) / bbox.height();
    }
    default:
        if (role >= int(BeginThumbnailRole))
            return createThumbnail(shape, QSize(role - int(BeginThumbnailRole), role - int(BeginThumbnailRole)));
        else
            return QVariant();
    }
}

Qt::ItemFlags KarbonLayerModel::flags(const QModelIndex &index) const
{
    if (! index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalPointer());

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEditable;
    //if( dynamic_cast<KoShapeContainer*>( (KoShape*)index.internalPointer() ) )
    flags |= Qt::ItemIsDropEnabled;
    return flags;
}

bool KarbonLayerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (! index.isValid())
        return false;

    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalPointer());

    KoShape *shape = static_cast<KoShape*>(index.internalPointer());
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        shape->setName(value.toString());
        break;
    case PropertiesRole:
        setProperties(shape, value.value<PropertyList>());
        // fall through
    case ActiveRole: {
        KoCanvasController * canvasController = KoToolManager::instance()->activeCanvasController();
        KoSelection * selection = canvasController->canvas()->shapeManager()->selection();

        KoShapeLayer *layer = dynamic_cast<KoShapeLayer*>(shape);
        if (layer && selection)
            selection->setActiveLayer(layer);
    }
    break;
    default:
        return false;
    }

    emit dataChanged(index, index);
    return true;
}

KoDocumentSectionModel::PropertyList KarbonLayerModel::properties(KoShape* shape) const
{
    PropertyList l;
    l << Property(i18nc("Visibility state of the shape", "Visible"), SmallIcon("14_layer_visible"), SmallIcon("14_layer_novisible"), shape->isVisible());
    l << Property(i18nc("Lock state of the shape", "Locked"), SmallIcon("object-locked"), SmallIcon("object-unlocked"), shape->isGeometryProtected());
    l << Property(i18nc("The z-index of the shape", "zIndex"), QString("%1").arg(shape->zIndex()));
    l << Property(i18nc("The opacity of the shape", "Opacity"), QString("%1").arg(1.0 - shape->transparency()));
    return l;
}

void KarbonLayerModel::setProperties(KoShape* shape, const PropertyList &properties)
{
    bool oldVisibleState = shape->isVisible();
    bool oldLockedState = shape->isGeometryProtected();
    bool newVisibleState = properties.at(0).state.toBool();
    bool newLockedState = properties.at(1).state.toBool();

    shape->setVisible(newVisibleState);
    shape->setGeometryProtected(newLockedState);

    KoShapeContainer * container = dynamic_cast<KoShapeContainer*>(shape);
    if (container)
        lockRecursively(container, newLockedState);
    else
        shape->setSelectable(!newLockedState);

    if ((oldVisibleState != shape->isVisible()) || (oldLockedState != shape->isGeometryProtected()))
        shape->update();
}

void KarbonLayerModel::lockRecursively(KoShapeContainer *container, bool lock)
{
    if (!container)
       return;

    if (!lock) {
        container->setSelectable(!container->isGeometryProtected());
    } else {
        container->setSelectable(!lock);
    }

    foreach(KoShape *shape, container->childShapes()) {
        KoShapeContainer * shapeContainer = dynamic_cast<KoShapeContainer*>(shape);
        if (shapeContainer) {
            lockRecursively(shapeContainer, lock);
        } else {
            if (!lock) {
                shape->setSelectable(!shape->isGeometryProtected());
            } else {
                shape->setSelectable(!lock);
            }
        }
    }
}

QImage KarbonLayerModel::createThumbnail(KoShape* shape, const QSize &thumbSize) const
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

KoShape * KarbonLayerModel::childFromIndex(KoShapeContainer *parent, int row) const
{
    return parent->shapes().at(row);
}

int KarbonLayerModel::indexFromChild(KoShapeContainer *parent, KoShape *child) const
{
    return parent->shapes().indexOf(child);
}

Qt::DropActions KarbonLayerModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

QStringList KarbonLayerModel::mimeTypes() const
{
    QStringList types;
    types << QLatin1String("application/x-karbonlayermodeldatalist");
    return types;
}

QMimeData * KarbonLayerModel::mimeData(const QModelIndexList & indexes) const
{
    // check if there is data to encode
    if (! indexes.count())
        return 0;

    // check if we support a format
    QStringList types = mimeTypes();
    if (types.isEmpty())
        return 0;

    QMimeData *data = new QMimeData();
    QString format = types[0];
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);

    // encode the data
    QModelIndexList::ConstIterator it = indexes.begin();
    for (; it != indexes.end(); ++it)
        stream << qVariantFromValue(qulonglong(it->internalPointer()));

    data->setData(format, encoded);
    return data;
}

bool KarbonLayerModel::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    // check if the action is supported
    if (! data || action != Qt::MoveAction)
        return false;
    // check if the format is supported
    QStringList types = mimeTypes();
    if (types.isEmpty())
        return false;
    QString format = types[0];
    if (! data->hasFormat(format))
        return false;

    QByteArray encoded = data->data(format);
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    QList<KoShape*> shapes;

    // decode the data
    while (! stream.atEnd()) {
        QVariant v;
        stream >> v;
        shapes.append(static_cast<KoShape*>((void*)v.value<qulonglong>()));
    }

    // no shapes to drop, exit gracefully
    if (shapes.count() == 0)
        return false;

    QList<KoShape*> toplevelShapes;
    QList<KoShapeLayer*> layers;
    // remove shapes having its parent in the list
    // and separate the layers
    foreach(KoShape * shape, shapes) {
        KoShapeContainer * parent = shape->parent();
        bool hasParentInList = false;
        while (parent) {
            if (shapes.contains(parent)) {
                hasParentInList = true;
                break;
            }
            parent = parent->parent();
        }
        if (hasParentInList)
            continue;

        KoShapeLayer * layer = dynamic_cast<KoShapeLayer*>(shape);
        if (layer)
            layers.append(layer);
        else
            toplevelShapes.append(shape);
    }

    if (! parent.isValid()) {
        kDebug(38000) << "KarbonLayerModel::dropMimeData parent = root";
        return false;
    }
    KoShape *shape = static_cast<KoShape*>(parent.internalPointer());
    KoShapeContainer * container = dynamic_cast<KoShapeContainer*>(shape);
    if (container) {
        KoShapeGroup * group = dynamic_cast<KoShapeGroup*>(container);
        if (group) {
            kDebug(38000) << "KarbonLayerModel::dropMimeData parent = group";
            if (! toplevelShapes.count())
                return false;

            emit layoutAboutToBeChanged();

            beginInsertRows(parent, group->shapeCount(), group->shapeCount() + toplevelShapes.count());

            QUndoCommand * cmd = new QUndoCommand();
            cmd->setText(i18n("Reparent shapes"));

            foreach(KoShape * shape, toplevelShapes)
            new KoShapeUngroupCommand(shape->parent(), QList<KoShape*>() << shape, QList<KoShape*>(), cmd);

            new KoShapeGroupCommand(group, toplevelShapes, cmd);
            KoCanvasController * canvasController = KoToolManager::instance()->activeCanvasController();
            canvasController->canvas()->addCommand(cmd);

            endInsertRows();

            emit layoutChanged();
        } else {
            kDebug(38000) << "KarbonLayerModel::dropMimeData parent = container";
            if (toplevelShapes.count()) {
                emit layoutAboutToBeChanged();

                beginInsertRows(parent, container->shapeCount(), container->shapeCount() + toplevelShapes.count());

                QUndoCommand * cmd = new QUndoCommand();
                cmd->setText(i18n("Reparent shapes"));

                QList<bool> clipped;
                foreach(KoShape * shape, toplevelShapes) {
                    if (! shape->parent()) {
                        clipped.append(false);
                        continue;
                    }

                    clipped.append(shape->parent()->isClipped(shape));
                    new KoShapeUngroupCommand(shape->parent(), QList<KoShape*>() << shape, QList<KoShape*>(), cmd);
                }

                // shapes are dropped on a container, so add them to the container
                new KoShapeGroupCommand(container, toplevelShapes, clipped, cmd);
                KoCanvasController * canvasController = KoToolManager::instance()->activeCanvasController();
                canvasController->canvas()->addCommand(cmd);

                endInsertRows();

                emit layoutChanged();
            } else if (layers.count()) {
                KoShapeLayer * layer = dynamic_cast<KoShapeLayer*>(container);
                if (! layer)
                    return false;

                // TODO layers are dropped on a layer, so change layer ordering
                return false;
            }
        }
    } else {
        kDebug(38000) << "KarbonLayerModel::dropMimeData parent = shape";
        if (! toplevelShapes.count())
            return false;

        // TODO shapes are dropped on a shape, reorder them
        return false;
    }

    return true;
}

QModelIndex KarbonLayerModel::parentIndexFromShape(const KoShape * child) const
{
    if (! m_document)
        return QModelIndex();

    // check if child shape is a layer, and return invalid model index if it is
    const KoShapeLayer *childlayer = dynamic_cast<const KoShapeLayer*>(child);
    if (childlayer)
        return QModelIndex();

    // get the children's parent shape
    KoShapeContainer *parentShape = child->parent();
    if (! parentShape)
        return QModelIndex();

    // check if the parent is a layer
    KoShapeLayer *parentLayer = dynamic_cast<KoShapeLayer*>(parentShape);
    if (parentLayer)
        return createIndex(m_document->layers().indexOf(parentLayer), 0, parentShape);

    // get the grandparent to determine the row of the parent shape
    KoShapeContainer *grandParentShape = parentShape->parent();
    if (! grandParentShape)
        return QModelIndex();

    return createIndex(indexFromChild(grandParentShape, parentShape), 0, parentShape);
}

#include "KarbonLayerModel.moc"

// kate: replace-tabs on; space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
