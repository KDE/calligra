/* Part of the KOffice project
 * Copyright (C) 2008 Peter Simonsson <peter.simonsson@gmail.com>
 * Copyright (C) 2010 Yue Liu <opuspace@gmail.com>
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

#include "StencilBoxDocker.h"

#include "OdfCollectionLoader.h"
#include "CollectionShapeFactory.h"
#include "StencilBoxView.h"

#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoCanvasController.h>
#include <KoToolManager.h>
#include <KoCreateShapesTool.h>
#include <KoShape.h>
#include <KoShapeGroup.h>
#include <KoShapeBorderModel.h>
#include <KoZoomHandler.h>

#include <klocale.h>
#include <kcombobox.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kcomponentdata.h>
#include <kdesktopfile.h>
#include <kconfiggroup.h>
#include <kicon.h>
#include <kmessagebox.h>
#include <klineedit.h>

#include <QVBoxLayout>
#include <QListView>
#include <QStandardItemModel>
#include <QRegExp>
#include <QSortFilterProxyModel>
#include <QList>
#include <QSize>
#include <QToolButton>
#include <QDir>
#include <QMenu>
#include <QPainter>

//
// StencilBoxDockerFactory
//

StencilBoxDockerFactory::StencilBoxDockerFactory()
    : KoDockFactoryBase()
{
}

QString StencilBoxDockerFactory::id() const
{
    return QString("FlowStencilBoxDocker");
}

QDockWidget* StencilBoxDockerFactory::createDockWidget()
{
    StencilBoxDocker* docker = new StencilBoxDocker();
    return docker;
}

//
// StencilBoxDocker
//

StencilBoxDocker::StencilBoxDocker(QWidget* parent)
    : QDockWidget(parent)
{
    setWindowTitle(i18n("Stencil Box"));
    QWidget* mainWidget = new QWidget(this);
    setWidget(mainWidget);

    //menu is currently unfinished
    m_menu = new QMenu();
    QAction* getOnlineAction = m_menu->addAction(i18n("Download shape collections online"));
    QAction* installLocalAction = m_menu->addAction(i18n("Install local collection file"));
    QAction* createNewAction = m_menu->addAction(i18n("Create a new collection"));
    connect(getOnlineAction, SIGNAL(triggered()), this, SLOT(getCollectionOnline()));
    connect(installLocalAction, SIGNAL(triggered()), this, SLOT(installLocalCollection()));
    connect(createNewAction, SIGNAL(triggered()), this, SLOT(createNewCollection()));

    m_button = new QToolButton;
    m_button->setIcon(SmallIcon("list-add"));
    m_button->setToolTip(i18n("More shapes"));
    m_button->setMenu(m_menu);
    m_button->setPopupMode(QToolButton::InstantPopup);

    m_filterLineEdit = new KLineEdit;
#if QT_VERSION >= 0x040700
    m_filterLineEdit->setPlaceholderText(i18n("Filter"));
#endif
    m_filterLineEdit->setClearButtonShown(true);

    m_treeWidget = new CollectionTreeWidget(mainWidget);
    m_treeWidget->setSelectionMode(QListView::SingleSelection);
    m_treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_panelLayout = new QHBoxLayout();
    m_panelLayout->addWidget(m_button);
    m_panelLayout->addWidget(m_filterLineEdit);

    m_layout = new QVBoxLayout(mainWidget);
    m_layout->addLayout(m_panelLayout);
    m_layout->addWidget(m_treeWidget);
    
    if(! KGlobal::activeComponent().dirs()->resourceDirs("app_shape_collections").isEmpty())
    {
        loadShapeCollections();
    }
    
    loadDefaultShapes();
    regenerateProxyMap();
    m_treeWidget->setFamilyMap(m_proxyMap);

    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(locationChanged(Qt::DockWidgetArea)));
    connect(m_filterLineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(reapplyFilter()));
}

void StencilBoxDocker::getCollectionOnline()
{
    ;//FIXME
}

void StencilBoxDocker::installLocalCollection()
{
    ;//FIXME
}

void StencilBoxDocker::createNewCollection()
{
    ;//FIXME
}

void StencilBoxDocker::locationChanged(Qt::DockWidgetArea area)
{
    switch(area) {
        case Qt::TopDockWidgetArea:
        case Qt::BottomDockWidgetArea:
            break;
        case Qt::LeftDockWidgetArea:
        case Qt::RightDockWidgetArea:
            break;
        default:
            break;
    }
    m_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    m_layout->invalidate();
}

void StencilBoxDocker::reapplyFilter()
{
    QRegExp regExp(m_filterLineEdit->originalText(), Qt::CaseInsensitive, QRegExp::RegExp2);
    foreach(QSortFilterProxyModel* model, m_proxyMap)
    {
        model->setFilterRegExp(regExp);
    }
    m_treeWidget->filter();
}

void StencilBoxDocker::regenerateProxyMap()
{
    QMapIterator<QString, CollectionItemModel*> i(m_modelMap);
    while(i.hasNext())
    {
        i.next();
        QSortFilterProxyModel* proxy = new QSortFilterProxyModel();
        proxy->setSourceModel(i.value());
        m_proxyMap.insert(i.key(), proxy);
    }
}

/// Generate lists of shapes registered
void StencilBoxDocker::loadDefaultShapes()
{
    QMap<QString, QList<KoCollectionItem> > familyMap;
    foreach( const QString & id, KoShapeRegistry::instance()->keys() )
    {
        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(id);
        // don't show hidden factories
        if ( factory->hidden() )
	{
            continue;
        }
        bool oneAdded = false;

        foreach( const KoShapeTemplate & shapeTemplate, factory->templates() )
        {
            oneAdded = true;
            KoCollectionItem temp;
            temp.id = shapeTemplate.id;
            temp.name = shapeTemplate.name;
            temp.toolTip = shapeTemplate.toolTip;
            temp.icon = KIcon(shapeTemplate.icon);
            temp.properties = shapeTemplate.properties;

            if(familyMap.contains(shapeTemplate.family))
            {
                familyMap[shapeTemplate.family].append(temp);
            }
            else
            {
                QList<KoCollectionItem> list;
                list.append(temp);
                familyMap.insert(shapeTemplate.family, list);
            }
        }

        if(!oneAdded)
        {
            KoCollectionItem temp;
            temp.id = factory->id();
            temp.name = factory->name();
            temp.toolTip = factory->toolTip();
            temp.icon = KIcon(factory->icon());
            temp.properties = 0;

            if(familyMap.contains(factory->family()))
            {
                familyMap[factory->family()].append(temp);
            }
            else
            {
                QList<KoCollectionItem> list;
                list.append(temp);
                familyMap.insert(factory->family(), list);
            }
        }
    }

    QMapIterator<QString, QList<KoCollectionItem> > i(familyMap);
    while(i.hasNext())
    {
        i.next();
        CollectionItemModel* model = new CollectionItemModel(this);
        model->setShapeTemplateList(i.value());
        m_modelMap.insert(i.key(), model);
    }
}

/// Load shape collections to m_modelMap and register in the KoShapeRegistry
void StencilBoxDocker::loadShapeCollections()
{
    QStringList dirs = KGlobal::activeComponent().dirs()->resourceDirs("app_shape_collections");
    foreach(const QString& path, dirs)
    {
        QDir dir(path);
        QStringList collectionDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach(const QString & collectionDirName, collectionDirs) {
            addCollection(path + collectionDirName);
        }
    }
}

bool StencilBoxDocker::addCollection(const QString& path)
{
    QDir dir(path);

    if(!dir.exists("collection.desktop"))
        return false;

    KDesktopFile collection(dir.absoluteFilePath("collection.desktop"));
    KConfigGroup dg = collection.desktopGroup();
    QString family = dg.readEntry("Name");
    QString type = dg.readEntry("X-KDE-DirType");
    //kDebug() << family << type;

    if(type == "odg-collection") {
        if(m_modelMap.contains(family))
            return true;

        CollectionItemModel* model = new CollectionItemModel(this);
        m_modelMap.insert(family, model);

        OdfCollectionLoader* loader = new OdfCollectionLoader(path + QDir::separator(), family, this);
        connect(loader, SIGNAL(loadingFailed(const QString&)),
                this, SLOT(onLoadingFailed(const QString&)));
        connect(loader, SIGNAL(loadingFinished()),
                this, SLOT(onLoadingFinished()));

        loader->load();
        return true;
    }
    else {
        return false;
    }
}

void StencilBoxDocker::onLoadingFailed(const QString& reason)
{
    OdfCollectionLoader* loader = qobject_cast<OdfCollectionLoader*>(sender());

    if(loader)
    {
        removeCollection(loader->collectionPath());
        QList<KoShape*> shapeList = loader->shapeList();
        qDeleteAll(shapeList);
        loader->deleteLater();
    }

    KMessageBox::error (this, reason, i18n("Collection Error"));
}

void StencilBoxDocker::onLoadingFinished()
{
    OdfCollectionLoader* loader = qobject_cast<OdfCollectionLoader*>(sender());

    if(!loader)
    {
        kWarning(31000) << "Not called by a OdfCollectionLoader!";
        return;
    }

    QList<KoCollectionItem> templateList;
    QList<KoShape*> shapeList = loader->shapeList();

    foreach(KoShape* shape, shapeList)
    {
        KoCollectionItem temp;
        temp.id = loader->collectionPath() + shape->name();
        temp.name = shape->name();
        temp.toolTip = shape->name();
        temp.icon = generateShapeIcon(shape);
        templateList.append(temp);
        CollectionShapeFactory* factory =
                new CollectionShapeFactory(loader->collectionPath() + shape->name(), shape);
        KoShapeRegistry::instance()->add(loader->collectionPath() + shape->name(), factory);
    }

    CollectionItemModel* model = m_modelMap[loader->collectionFamily()];
    model->setShapeTemplateList(templateList);

    loader->deleteLater();
}

void StencilBoxDocker::removeCollection(const QString& family)
{
    if(m_modelMap.contains(family))
    {
        CollectionItemModel* model = m_modelMap[family];
        QList<KoCollectionItem> list = model->shapeTemplateList();
        foreach(const KoCollectionItem & temp, list)
        {
            KoShapeFactoryBase* factory = KoShapeRegistry::instance()->get(temp.id);
            KoShapeRegistry::instance()->remove(temp.id);
            delete factory;
        }

        m_modelMap.remove(family);
        delete model;
    }
}

QIcon StencilBoxDocker::generateShapeIcon(KoShape* shape)
{
    KoZoomHandler converter;
    bool isGroup = (shape->size()==QSizeF(0,0)) ? true : false;

    QRectF bound;
    KoShapeGroup* group;
    QList<KoShape*> sortedObjects;

    if (isGroup == true) {
        bool boundInitialized = false;
        group = static_cast<KoShapeGroup*>(shape);
        sortedObjects = group->shapes();
        qSort(sortedObjects.begin(), sortedObjects.end(), KoShape::compareShapeZIndex);
        foreach(KoShape * subShape, sortedObjects) {
            if (! boundInitialized) {
                bound = subShape->boundingRect();
                boundInitialized = true;
            } else
                bound = bound.united(subShape->boundingRect());
        }
    } else {
        bound = shape->boundingRect();
    }

    qreal diffx = 30 / converter.documentToViewX(bound.width());
    qreal diffy = 30 / converter.documentToViewY(bound.height());
    converter.setZoom(qMin(diffx, diffy));

    QPixmap pixmap(qRound(converter.documentToViewX(bound.width())) + 2, qRound(converter.documentToViewY(bound.height())) + 2);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QTransform baseMatrix = shape->absoluteTransformation(&converter).inverted() * painter.transform();
    painter.translate(1, 1);

    if (isGroup == true) {
        foreach(KoShape * subShape, sortedObjects) {
            //kDebug(30006) <<"KoShapeContainer::painting shape:" << subShape->shapeId() <<"," << subShape->boundingRect();
            if (!subShape->isVisible())
                continue;
            painter.save();
            painter.setTransform(subShape->absoluteTransformation(&converter) * baseMatrix);
            subShape->paint(painter, converter);
            painter.restore();
            if (subShape->border()) {
                painter.save();
                painter.setTransform(subShape->absoluteTransformation(&converter) * baseMatrix);
                subShape->border()->paint(subShape, painter, converter);
                painter.restore();
            }
        }
    } else {
        shape->paint(painter, converter);
    }

    painter.end();

    return QIcon(pixmap);
}
