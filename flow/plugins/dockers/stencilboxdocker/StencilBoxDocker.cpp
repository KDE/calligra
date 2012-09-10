/* Part of the Calligra project
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

#include "StencilShapeFactory.h"
#include "StencilBoxView.h"

#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoCanvasController.h>
#include <KoToolManager.h>
#include <KoCreateShapesTool.h>
#include <KoShape.h>
#include <KoShapeGroup.h>
#include <KoZoomHandler.h>
#include <KoProperties.h>
#include <KoIcon.h>

#include <klocale.h>
#include <kcombobox.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kcomponentdata.h>
#include <kdesktopfile.h>
#include <kconfiggroup.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <knewstuff3/downloaddialog.h>
#include <KFileDialog>
#include <KIO/NetAccess>
#include <KStandardDirs>
#include <KTar>

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

#define StencilShapeId "StencilShape"

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

    m_menu = new QMenu();
    QAction *ghnsAction = m_menu->addAction(koIcon("get-hot-new-stuff"), i18n("Get more stencils"));
    QAction *installAction = m_menu->addAction(koIcon("document-open-folder"), i18n("Install stencil"));

    connect(ghnsAction, SIGNAL(triggered()), this, SLOT(getHotNewStuff()));
    connect(installAction, SIGNAL(triggered()), this, SLOT(installStencil()));

    m_button = new QToolButton;
    m_button->setIcon(koIcon("list-add"));
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
    
    loadDefaultShapes();
    if(! KGlobal::activeComponent().dirs()->resourceDirs("app_shape_collections").isEmpty())
    {
        loadShapeCollections();
    }

    m_treeWidget->setFamilyMap(m_modelMap);
    m_treeWidget->regenerateFilteredMap();
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(locationChanged(Qt::DockWidgetArea)));
    connect(m_filterLineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(reapplyFilter()));
}

void StencilBoxDocker::getHotNewStuff()
{
    KNS3::DownloadDialog dialog("flow_stencils.knsrc", this);
    dialog.exec();
    if(!dialog.installedEntries().isEmpty()) {
        KMessageBox::information(0, i18n("Stencils successfully installed."));
    }
    else if(!dialog.changedEntries().isEmpty()) {
        KMessageBox::information(0, i18n("Stencils successfully uninstalled."));
    }
}

void StencilBoxDocker::installStencil()
{
    KUrl dir;
    QString path = KFileDialog::getOpenFileName(dir,
           "*.cstencil.tar *.cstencil.tar.bz2 *.cstencil.tar.gz|"
           + i18n("Calligra Stencil Packages (*.cstencil.tar, *.cstencil.tar.bz2, *.cstencil.tar.gz)")
           , this);
    if(path.isNull()) return;
    
    KTar archive(path);
    if(!archive.open(QIODevice::ReadOnly)) {
        KMessageBox::sorry(0, i18n("Could not read this package."));
        return;
    }
    
    QString destination = KStandardDirs::locateLocal("data", "flow/stencils", true);
    const KArchiveDirectory* const archiveDir = archive.directory();

    // Prevent installing a stencil collection that's already installed
    const QString collectionFolder = destination + QDir::separator() + archiveDir->entries().first();
    //kDebug() << destination << archiveDir << collectionFolder;
    if(QFile::exists(collectionFolder)) {
        KMessageBox::error(0, i18n("A collection of stencils in family '%1' is already installed. "
                                   "Please uninstall it first.", archiveDir->entries().first()));
        return;
    }
    
    archiveDir->copyTo(destination);
    KMessageBox::information(0, i18n("Stencils successfully installed."));
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
    m_treeWidget->setFilter(regExp);
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
            temp.icon = KIcon(shapeTemplate.iconName);
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
            temp.icon = KIcon(factory->iconName());
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

    if(type != "odg-collection")
        return false;
    if(!m_modelMap.contains(family)) {
        CollectionItemModel* model = new CollectionItemModel(this);
        m_modelMap.insert(family, model);
    }

    CollectionItemModel* model = m_modelMap[family];
    QList<KoCollectionItem> templateList = model->shapeTemplateList();
    QStringList stencils = dir.entryList(QStringList("*.desktop"));
    foreach(const QString & stencil, stencils) {
        if(stencil == "collection.desktop")
            continue;
        KDesktopFile entry(dir.absoluteFilePath(stencil));
        KConfigGroup content = entry.desktopGroup();
        QString name = content.readEntry("Name");
        QString noExt = dir.absoluteFilePath(stencil);
        noExt.chop(7);
        QString source = noExt + "odg";
        QString icon = noExt + "png";
        //kDebug() << name << source << icon;
        QString keepAspectRatio = content.readEntry("CS-KeepAspectRatio", "0");
        KoProperties* props = new KoProperties();
        props->setProperty("keepAspectRatio", keepAspectRatio.toInt());
        KoCollectionItem temp;
        temp.id = source;
        temp.name = name;
        temp.toolTip = name;
        temp.icon = QIcon(icon);
        temp.properties = props;
        templateList.append(temp);
        StencilShapeFactory* factory = new StencilShapeFactory(source, name, source, props);
        KoShapeRegistry::instance()->add(source, factory);
    }
    model->setShapeTemplateList(templateList);
    return true;
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
        m_treeWidget->regenerateFilteredMap();
    }
}

void StencilBoxDocker::setViewMode(QListView::ViewMode iconMode)
{
    QMapIterator<QString, CollectionItemModel*> i(m_modelMap);
    while(i.hasNext())
    {
        i.next();
        i.value()->setViewMode(iconMode);
    }
}
