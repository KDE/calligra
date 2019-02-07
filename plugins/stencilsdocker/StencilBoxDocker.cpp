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

#include "StencilBoxDocker_p.h"
#include "StencilShapeFactory.h"
#include "CollectionItemModel.h"
#include "CollectionTreeWidget.h"
#include "StencilBoxDebug.h"

#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoCanvasController.h>
#include <KoCreateShapesTool.h>
#include <KoShape.h>
#include <KoShapeGroup.h>
#include <KoZoomHandler.h>
#include <KoShapePaintingContext.h>
#include <KoProperties.h>
#include <KoIcon.h>

#include <kiconeffect.h>
#include <kcolorscheme.h>
#include <klocalizedstring.h>
#include <kdesktopfile.h>
#include <kconfiggroup.h>
#include <kmessagebox.h>
#include <klineedit.h>

#ifdef GHNS
#include <knewstuff3/downloaddialog.h>
#endif

#include <QStandardPaths>
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
#include <QPixmapCache>

#define StencilShapeId "StencilShape"

StencilBoxDocker::StencilBoxDocker(QWidget* parent)
    : QDockWidget(parent)
{
    setWindowTitle(i18n("Stencil Box"));
    QWidget* mainWidget = new QWidget(this);
    mainWidget->setAcceptDrops(true);
    setWidget(mainWidget);

    m_menu = new QMenu();
#ifdef GHNS
    QAction *ghnsAction = m_menu->addAction(koIcon("get-hot-new-stuff"), i18n("Stencils Online"));
    connect(ghnsAction, SIGNAL(triggered()), this, SLOT(getHotNewStuff()));
#endif
    QAction *installAction = m_menu->addAction(koIcon("document-open-folder"), i18n("Add/Remove Stencil"));
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
    m_filterLineEdit->setPlaceholderText(i18n("Filter"));
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

    // Load the stencils
    m_loader = new StencilBoxDockerLoader(this);
    m_loader->moveToThread(&loaderThread);
    connect(&loaderThread, SIGNAL(started()), this, SLOT(threadStarted()));
    connect(this , SIGNAL(startLoading()), m_loader, SLOT(loadShapeCollections()));
    connect(&loaderThread, SIGNAL(finished()), m_loader, SLOT(deleteLater()));
    connect(m_loader, SIGNAL(resultReady()), this, SLOT(collectionsLoaded()));
    loaderThread.start();
}

StencilBoxDocker::~StencilBoxDocker()
{
    loaderThread.quit();
    loaderThread.wait();
    qDeleteAll(m_modelMap);
}

void StencilBoxDocker::threadStarted()
{
    Q_EMIT startLoading();
}

void StencilBoxDocker::collectionsLoaded()
{
    debugStencilBox;
    m_modelMap = m_loader->m_modelMap;
    m_treeWidget->setFamilyMap(m_modelMap);
    m_treeWidget->regenerateFilteredMap();
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(locationChanged(Qt::DockWidgetArea)));
    connect(m_filterLineEdit, SIGNAL(textEdited(QString)), this, SLOT(reapplyFilter()));

    loaderThread.quit();
}

#ifdef GHNS
void StencilBoxDocker::getHotNewStuff()
{
    KNS3::DownloadDialog dialog("calligra_stencils.knsrc", this);
    dialog.exec();
    if(!dialog.installedEntries().isEmpty()) {
        KMessageBox::information(0, i18n("Stencils successfully installed."));
    }
    else if(!dialog.changedEntries().isEmpty()) {
        KMessageBox::information(0, i18n("Stencils successfully uninstalled."));
    }
}
#endif

void StencilBoxDocker::manageStencilsFolder()
{
    const QString destination = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/calligra/stencils");
    QDir().mkpath(destination);
    QFile file(destination + "/readme.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        debugStencilBox << "could not open" << destination + "/readme.txt" << "for writing";
    } else {
        QTextStream out(&file);
        out << i18n("\
This is the user stencils directory.\n\
From here you can add / remove stencils for use in the Stencil Box docker.\n\
\n\
Stencils are organized in collections, a collection is a folder containing a text file 'collection.desktop':\n\
\n\
collection.desktop\n\
\n\
[Desktop Entry]\n\
Name=Your Stencil Collection Name\n\
\n\
A stencil is an ODG/SVG file, a desktop file, an optional PNG icon file, all with the same name under its collection folder:\n\
\n\
foo.odg or foo.svgz or foo.svg\n\
\n\
ODF elements for stencil should be a <draw:g> element or <draw:custom-shape> element\n\
No special requirements to SVG file\n\
\n\
foo.desktop\n\
\n\
[Desktop Entry]\n\
Name=Foo\n\
CS-KeepAspectRatio=1\n\
\n\
If CS-KeepAspectRatio=1, the stencil added to canvas will have geometry aspect ratio locked, by default it's 0.\n\
\n\
foo.png\n\
\n\
Should have size 32x32 pixel, if the png file is not included, the ODG/SVG file will be rendered as the icon,\n\
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
void StencilBoxDockerLoader::loadShapeCollections()
{
    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("calligra/stencils"), QStandardPaths::LocateDirectory);
    foreach(const QString& path, dirs)
    {
        debugStencilBox << path;
        QDir dir(path);
        QStringList collectionDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach(const QString & collectionDirName, collectionDirs) {
            addCollection(path + QLatin1Char('/') + collectionDirName);
            debugStencilBox << path + collectionDirName;
        }
    }
    emit resultReady();
}

bool StencilBoxDockerLoader::addCollection(const QString& path)
{
    QDir dir(path);

    if(!dir.exists("collection.desktop"))
        return false;

    KDesktopFile collection(dir.absoluteFilePath("collection.desktop"));
    KConfigGroup dg = collection.desktopGroup();
    QString family = dg.readEntry("Name");

    if(!m_modelMap.contains(family)) {
        CollectionItemModel* model = new CollectionItemModel();
        m_modelMap.insert(family, model);
    }

    CollectionItemModel* model = m_modelMap[family];
    QList<KoCollectionItem> templateList = model->shapeTemplateList();
    QStringList stencils = dir.entryList(QStringList("*.desktop"));

    KStatefulBrush brushForeground(KColorScheme::Window, KColorScheme::NormalText);
    KStatefulBrush brushBackground(KColorScheme::Window, KColorScheme::NormalBackground);
    const QColor blackColor = brushForeground.brush(q).color();
    const QColor whiteColor = brushBackground.brush(q).color();

    foreach(const QString & stencil, stencils) {
        if(stencil == "collection.desktop")
            continue;

        KDesktopFile entry(dir.absoluteFilePath(stencil));
        KConfigGroup content = entry.desktopGroup();
        QString name = content.readEntry("Name");
        bool keepAspectRatio = content.readEntry("CS-KeepAspectRatio", false);
        KoProperties* props = new KoProperties();
        props->setProperty("keepAspectRatio", keepAspectRatio);

        // find data file path
        QString filename = dir.absoluteFilePath(stencil);
        filename.chop(7); // remove 'desktop'
        static const char * const suffix[3] = { "odg", "svgz", "svg"};
        static const int suffixCount = sizeof(suffix)/sizeof(suffix[0]);

        QString source;
        for (int i = 0; i < suffixCount; ++i) {
            source = filename + QLatin1String(suffix[i]);
            if (QFile::exists(source)) {
                break;
            }
            source.clear();
        }
        if (source.isEmpty()) {
            debugStencilBox << filename << "not found";
            continue;
        }

        // register shape factory
        StencilShapeFactory* factory = new StencilShapeFactory(source, name, props);
        KoShapeRegistry::instance()->add(source, factory);

        KoCollectionItem temp;
        temp.id = source;
        temp.name = name;
        temp.toolTip = name;

        QImage img;
        const QString thumbnailFile = filename + QStringLiteral("png");
        if (QFile::exists(thumbnailFile)) {
            img = QIcon(thumbnailFile).pixmap(QSize(22, 22)).toImage();
        } else {
            // generate icon using factory
            QPixmap pix(22, 22);
            pix.fill(Qt::white);
            if (!QPixmapCache::find(source, &pix)) {
                KoShape* shape = factory->createDefaultShape();
                if (shape) {
                    KoZoomHandler converter;
                    qreal diffx = 20 / converter.documentToViewX(shape->size().width());
                    qreal diffy = 20 / converter.documentToViewY(shape->size().height());
                    converter.setZoom(qMin(diffx, diffy));
                    QPainter painter(&pix);
                    painter.setRenderHint(QPainter::Antialiasing, true);
                    painter.translate(1, 1);
                    KoShapePaintingContext paintContext;
                    shape->paint(painter, converter, paintContext);
                    painter.end();
                    QPixmapCache::insert(source, pix);
                    delete shape;
                }
            }
            img = pix.toImage();
        }
        KIconEffect::toMonochrome(img, blackColor, whiteColor, 1.0f);
        temp.icon = QIcon(QPixmap::fromImage(img));
        templateList.append(temp);
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
