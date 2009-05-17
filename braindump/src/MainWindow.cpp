/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "MainWindow.h"

#include <QApplication>
#include <QDockWidget>
#include <QLayout>

#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kconfiggroup.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kstandardaction.h>
#include <kundostack.h>

#include <KoDockFactory.h>

#include "RootSection.h"
#include "View.h"
#include "Canvas.h"
#include "RootSection.h"

MainWindow::MainWindow(RootSection* document, const KComponentData &componentData) : m_doc(document)
{
  Q_ASSERT(componentData.isValid());
  KGlobal::setActiveComponent(componentData);
  
  view = new View( m_doc, this);
  setCentralWidget(view);

  // then, setup our actions
  setupActions();

  // a call to KXmlGuiWindow::setupGUI() populates the GUI
  // with actions, using KXMLGUI.
  // It also applies the saved mainwindow settings, if any, and ask the
  // mainwindow to automatically save settings if changed: window size,
  // toolbar position, icon size, etc.
  setupGUI();
  
  // Position and show toolbars according to user's preference
  setAutoSaveSettings(componentData.componentName(), false);

  foreach (QDockWidget *wdg, m_dockWidgets) {
      if ((wdg->features() & QDockWidget::DockWidgetClosable) == 0) {
          wdg->setVisible(true);
      }
  }
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupActions()
{
  KStandardAction::quit(qApp, SLOT(closeAllWindows()), actionCollection());
  m_doc->undoStack()->createUndoAction(actionCollection());
  m_doc->undoStack()->createRedoAction(actionCollection());
  m_dockWidgetMenu  = new KActionMenu(i18n("Dockers"), this);
  actionCollection()->addAction("settings_dockers_menu", m_dockWidgetMenu);
  m_dockWidgetMenu->setVisible(false);
}

QDockWidget* MainWindow::createDockWidget(KoDockFactory* factory)
{
    QDockWidget* dockWidget = 0;

    if (!m_dockWidgetMap.contains(factory->id())) {
        dockWidget = factory->createDockWidget();

        // It is quite possible that a dock factory cannot create the dock; don't
        // do anything in that case.
        if (!dockWidget) return 0;
        m_dockWidgets.push_back(dockWidget);

        dockWidget->setObjectName(factory->id());
        dockWidget->setParent(this);

        if (dockWidget->widget() && dockWidget->widget()->layout())
            dockWidget->widget()->layout()->setContentsMargins(1, 1, 1, 1);

        Qt::DockWidgetArea side = Qt::RightDockWidgetArea;
        bool visible = true;

        switch (factory->defaultDockPosition()) {
        case KoDockFactory::DockTornOff:
            dockWidget->setFloating(true); // position nicely?
            break;
        case KoDockFactory::DockTop:
            side = Qt::TopDockWidgetArea; break;
        case KoDockFactory::DockLeft:
            side = Qt::LeftDockWidgetArea; break;
        case KoDockFactory::DockBottom:
            side = Qt::BottomDockWidgetArea; break;
        case KoDockFactory::DockRight:
            side = Qt::RightDockWidgetArea; break;
        case KoDockFactory::DockMinimized:
            visible = false; break;
        default:;
        }

        addDockWidget(side, dockWidget);
        if (dockWidget->features() & QDockWidget::DockWidgetClosable) {
            m_dockWidgetMenu->addAction(dockWidget->toggleViewAction());
            if (!visible)
                dockWidget->hide();
        }

        m_dockWidgetMap.insert(factory->id(), dockWidget);
    } else {
        dockWidget = m_dockWidgetMap[ factory->id()];
    }

    KConfigGroup group(KGlobal::config(), "GUI");
    QFont dockWidgetFont  = KGlobalSettings::generalFont();
    qreal pointSize = group.readEntry("palettefontsize", dockWidgetFont.pointSize() * 0.75);
    pointSize = qMax(pointSize, KGlobalSettings::smallestReadableFont().pointSizeF());
    dockWidgetFont.setPointSizeF(pointSize);
#ifdef Q_WS_MAC
    dockWidget->setAttribute(Qt::WA_MacSmallSize, true);
#endif
    dockWidget->setFont(dockWidgetFont);

    connect(dockWidget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(forceDockTabFonts()));

    return dockWidget;
}
