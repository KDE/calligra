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

#include <kactioncollection.h>
#include <kglobal.h>
#include <kstandardaction.h>
#include <kundostack.h>

#include "RootSection.h"
#include "View.h"
#include "Canvas.h"
#include "RootSection.h"

MainWindow::MainWindow(RootSection* document, const KComponentData &componentData) : m_doc(document)
{
  Q_ASSERT(componentData.isValid());
  KGlobal::setActiveComponent(componentData);
  
  canvas = new Canvas(0, m_doc);
  setCentralWidget(canvas);

  // then, setup our actions
  setupActions();

  // a call to KXmlGuiWindow::setupGUI() populates the GUI
  // with actions, using KXMLGUI.
  // It also applies the saved mainwindow settings, if any, and ask the
  // mainwindow to automatically save settings if changed: window size,
  // toolbar position, icon size, etc.
  setupGUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupActions()
{
  KStandardAction::quit(qApp, SLOT(closeAllWindows()), actionCollection());
  m_doc->undoStack()->createUndoAction(actionCollection());
  m_doc->undoStack()->createRedoAction(actionCollection());
}

QDockWidget* MainWindow::createDockWidget(KoDockFactory* factory)
{
  return 0;
}
