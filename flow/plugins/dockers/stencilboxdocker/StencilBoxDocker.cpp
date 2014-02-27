/* Part of the Calligra project
 * Copyright (C) 2010-2014 Yue Liu <yue.liu@mail.com>
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
#include "CollectionItemModel.h"
#include "CollectionTreeWidget.h"

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
#include <kcomponentdata.h>
#include <kdesktopfile.h>
#include <kconfiggroup.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <knewstuff3/downloaddialog.h>
#include <kfiledialog.h>
#include <KIO/NetAccess>
#include <kstandarddirs.h>
#include <ktar.h>

#include <QVBoxLayout>
#include <QListView>
#include <QStandardItemModel>
#include <QRegExp>
#include <QSortFilterProxyModel>
#include <QList>
#include <QSize>
#include <QToolButton>
#include <QDir>
#include <QFile>
#include <QMenu>
#include <QPainter>
#include <QDesktopServices>

#define StencilShapeId "StencilShape"

StencilBoxDocker::StencilBoxDocker(QWidget* parent)
    : QDockWidget(parent)
{
    setWindowTitle(i18n("Stencil Box"));
    QWidget* mainWidget = new QWidget(this);
    mainWidget->setAcceptDrops(true);
    setWidget(mainWidget);

    m_menu = new QMenu();
    QAction *ghnsAction = m_menu->addAction(koIcon("get-hot-new-stuff"), i18n("Stencils Online"));
    QAction *installAction = m_menu->addAction(koIcon("document-open-folder"), i18n("Add/Remove Stencil"));

    connect(ghnsAction, SIGNAL(triggered()), this, SLOT(getHotNewStuff()));
    connect(installAction, SIGNAL(triggered()), this, SLOT(manageStencilsFolder()));

    m_button = new QToolButton;
    /*
    m_button->setFixedHeight(qApp->fontMetrics().height()+3);
    m_button->setAutoFillBackground(true);
    m_button->setStyleSheet("\
        QToolButton {\
            border: 1px solid #a0a0a0;\
            border-top: 0px;\
            border-left: 0px;\
            border-right: 0px;\
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\
            stop:0 #ffffff, stop:0.5 #e0e0e0, stop:1 #ffffff);\
        }");*/
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
    
    if(! KGlobal::activeComponent().dirs()->resourceDirs("app_shape_collections").empty()) {
        loadShapeCollections();
    }

    m_treeWidget->setFamilyMap(m_modelMap);
    m_treeWidget->regenerateFilteredMap();
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(locationChanged(Qt::DockWidgetArea)));
    connect(m_filterLineEdit, SIGNAL(textEdited(QString)), this, SLOT(reapplyFilter()));
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

void StencilBoxDocker::manageStencilsFolder()
{
    QString destination = KStandardDirs::locateLocal("data", "flow/stencils/", true);
    QFile file(destination + "/readme.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "could not open" << destination + "/readme.txt" << "for writing";
    } else {
        QTextStream out(&file);
        out << i18n("\
This is the user stencils directory.\n\
From here you can add / remove stencils for use in the Stencil Box docker.\n\
\n\
Stencils are organized in collections, a collection is a folder containing a text file 'collection.desktop':\n\
\n\
ollection.desktop\n\
\n\
[Desktop Entry]\n\
Name=Your Stencil Collection Name\n\
\n\
A stencil is an ODG file, a desktop file, an optional PNG icon file, all with with name under its cpollection folder:\n\
\n\
foo.odg\n\
\n\
ODF elements for stencil should be a <draw:g> element or <draw:custom-shape> element\n\
\n\
foo.desktop\n\
\n\
[Desktop Entry]\n\
Name=Foo\n\
CS-KeepAspectRatio=1\n\
\n\
If CS-KeepAspectRatio=1, the stencil added to canvas will have geomatry aspect ratio locked, by default it's 0.\n\
\n\
foo.png\n\
\n\
Should have size 32x32 pixel, if the png file is not included, Flow will render the ODG file as the icon,\n\
but it won't look good under small pixels when the stencil stroke is complicated.\n");
        file.close();
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(destination));
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

/// Load shape collections to m_modelMap and register in the KoShapeRegistry
void StencilBoxDocker::loadShapeCollections()
{
    QStringList dirs = KGlobal::activeComponent().dirs()->resourceDirs("app_shape_collections");
    foreach(const QString& path, dirs)
    {
        qDebug() << path;
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
