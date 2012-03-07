/* Part of the Calligra project
 * Copyright (C) 2008 Peter Simonsson <peter.simonsson@gmail.com>
 * Copyright (C) 2010-2012 Yue Liu <yue.liu@mail.com>
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
#include "StencilBoxTreeWidget.h"

#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoCanvasController.h>
#include <KoToolManager.h>
#include <KoCreateShapesTool.h>
#include <KoShape.h>
#include <KoShapeGroup.h>
#include <KoShapeBorderModel.h>
#include <KoZoomHandler.h>
#include <KoProperties.h>

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
    QWidget *mainWidget = new QWidget(this);

    m_menu = new QMenu();
    QAction* ghnsAction = m_menu->addAction(
                KIcon("get-hot-new-stuff"), i18n("Get more stencils"));
    QAction* installAction = m_menu->addAction(
                KIcon("document-open-folder"), i18n("Install local stencil"));
    QAction* createAction = m_menu->addAction(
                KIcon("folder-new"), i18n("Create new stencil"));

    connect(ghnsAction, SIGNAL(triggered()), this, SLOT(getHotNewStuff()));
    connect(installAction, SIGNAL(triggered()), this, SLOT(installStencil()));
    connect(createAction, SIGNAL(triggered()), this, SLOT(createStencil()));

    m_menu->addSeparator();

    updateStencilMap();
    m_stencilActionGroup = new QActionGroup(this);
    QMap<QString, QStringList>::const_iterator i = m_stencilMap.constBegin();
    while (i != m_stencilMap.constEnd()) {
        m_stencilActionGroup->addAction(i.key());
        m_menu->addAction(i.key(), this);
        ++i;
    }
    connect(m_stencilActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(stencilOpened(QAction*)));

    m_button = new QToolButton;
    m_button->setIcon(SmallIcon("view-catalog")); //or "view-list-details" ?
    m_button->setToolTip(i18n("More Stencils"));
    m_button->setMenu(m_menu);
    m_button->setPopupMode(QToolButton::InstantPopup);

    //lineedit for search installed stencils
    m_filterLineEdit = new KLineEdit;
#if QT_VERSION >= 0x040700
    m_filterLineEdit->setPlaceholderText(i18n("Filter"));
#endif
    m_filterLineEdit->setClearButtonShown(true);

    m_treeWidget = new StencilBoxTreeWidget(mainWidget);
    m_treeWidget->setSelectionMode(QListView::SingleSelection);
    m_treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //layout
    m_subLayout = new QHBoxLayout();
    m_subLayout->addWidget(m_filterLineEdit);
    m_subLayout->addWidget(m_button);

    m_layout = new QVBoxLayout(mainWidget);
    m_layout->addLayout(m_subLayout);
    m_layout->addWidget(m_treeWidget);
    setWidget(mainWidget);

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
    //m_treeWidget->setFilter(regExp);
}

/// Generate stencil list
void StencilBoxDocker::updateStencilMap()
{
    QStringList dirs = KGlobal::activeComponent().dirs()->resourceDirs("app_shape_collections");
    foreach(const QString& path, dirs)
    {
        QDir dir(path);
        QStringList stencilDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach(const QString& stencilDir, stencilDirs) {
            QString confPath = QString(stencilDir + QDir::separator () + "stencil.desktop");
            KDesktopFile stencilConf = dir.absoluteFilePath(confPath);
            KConfigGroup dg = stencilConf.desktopGroup();
            QString name = dg.readEntry("Name");
            QString type = dg.readEntry("X-KDE-DirType");
            QStringList list = (QStringList() << stencilDir << type);
            m_stencilMap.insert(name, list);
        }
    }
}

void StencilBoxDocker::openStencil(QAction* action)
{
    if (m_stencilMap.contains(action->text())) {
        m_treeWidget->addStencil(m_stencilMap.value(action->text()));
    }
}
