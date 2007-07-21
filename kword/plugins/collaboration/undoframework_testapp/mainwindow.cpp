/****************************************************************************
**
** Copyright (C) 2007-2007 Trolltech ASA. All rights reserved.
** Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
**
** This file was part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include <libcollaboration/dialogs/ShareDialog.h>
#include <libcollaboration/dialogs/ConnectDialog.h>
#include <libcollaboration/CollaborationManager.h>
#include <libcollaboration/UndoStack.h>

#include "mainwindow.h"
#include "diagramscene.h"
#include "diagramitem.h"
#include "commands.h"

MainWindow::MainWindow()
{
    collaborationManager = new kcollaborate::CollaborationManager(this);
    undoStack = new kcollaborate::UndoStack();
    connectDialog = new kcollaborate::ConnectDialog(this);
    shareDialog = new kcollaborate::ShareDialog(this);

    connect(connectDialog, SIGNAL(connect(const Url&)),
            collaborationManager, SLOT(connect(const Url&)));
    connect(shareDialog, SIGNAL(share(const Url&)),
            collaborationManager, SLOT(share(const Url&)));

    createActions();
    createMenus();

    connect(undoStack, SIGNAL(canRedoChanged(bool)),
            redoAction, SLOT(setEnabled(bool)));
    connect(undoStack, SIGNAL(canUndoChanged(bool)),
            undoAction, SLOT(setEnabled(bool)));
    createUndoView();

    diagramScene = new DiagramScene();
    QBrush pixmapBrush(QPixmap(":/images/cross.png").scaled(30, 30));
    diagramScene->setBackgroundBrush(pixmapBrush);
    diagramScene->setSceneRect(QRect(0, 0, 500, 500));

    connect(diagramScene, SIGNAL(itemMoved(DiagramItem *, const QPointF &)),
            this, SLOT(itemMoved(DiagramItem *, const QPointF &)));

    setWindowTitle("Undo Framework");
    QGraphicsView *view = new QGraphicsView(diagramScene);
    setCentralWidget(view);
    resize(700, 500);
}

void MainWindow::createUndoView()
{
    undoView = new QUndoView(undoStack);
    undoView->setWindowTitle(tr("Command List"));
    undoView->show();
    undoView->setAttribute(Qt::WA_QuitOnClose, false);
}

void MainWindow::createActions()
{
    deleteAction = new QAction(tr("&Delete Item"), this);
    deleteAction->setShortcut(tr("Del"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));

    addBoxAction = new QAction(tr("Add &Box"), this);
    addBoxAction->setShortcut(tr("Ctrl+O"));
    connect(addBoxAction, SIGNAL(triggered()), this, SLOT(addBox()));

    addTriangleAction = new QAction(tr("Add &Triangle"), this);
    addTriangleAction->setShortcut(tr("Ctrl+T"));
    connect(addTriangleAction, SIGNAL(triggered()), this, SLOT(addTriangle()));

    undoAction = new QAction(tr("&Undo"), this);
    undoAction->setShortcut(tr("Ctrl+Z"));
    undoAction->setEnabled(false);
    connect(undoAction, SIGNAL(triggered()), undoStack, SLOT(undo()));

    redoAction = new QAction(tr("&Redo"), this);
    QList<QKeySequence> redoShortcuts;
    redoShortcuts << tr("Ctrl+Y") << tr("Shift+Ctrl+Z");
    redoAction->setShortcuts(redoShortcuts);
    redoAction->setEnabled(false);
    connect(redoAction, SIGNAL(triggered()), undoStack, SLOT(redo()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    aboutAction = new QAction(tr("&About"), this);
    QList<QKeySequence> aboutShortcuts;
    aboutShortcuts << tr("Ctrl+A") << tr("Ctrl+B");
    aboutAction->setShortcuts(aboutShortcuts);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));


    connectAction = new QAction(tr("&Connect to..."), this);
    connect(connectAction, SIGNAL(triggered()), this, SLOT(collaborateConnect()));

    shareAction = new QAction(tr("&Share document..."), this);
    connect(shareAction, SIGNAL(triggered()), this, SLOT(collaborateShare()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(deleteAction);
    connect(editMenu, SIGNAL(aboutToShow()),
            this, SLOT(itemMenuAboutToShow()));
    connect(editMenu, SIGNAL(aboutToHide()),
            this, SLOT(itemMenuAboutToHide()));
    
    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu->addAction(addBoxAction);
    itemMenu->addAction(addTriangleAction);

    collaborateMenu = menuBar()->addMenu(tr("&Collaborate"));
    collaborateMenu->addAction(shareAction);
    collaborateMenu->addAction(connectAction);

    helpMenu = menuBar()->addMenu(tr("&About"));
    helpMenu->addAction(aboutAction);
}

void MainWindow::itemMoved(DiagramItem *movedItem,
                           const QPointF &oldPosition)
{
    undoStack->push(new MoveCommand(movedItem, oldPosition));
}

void MainWindow::deleteItem()
{
    if (diagramScene->selectedItems().isEmpty())
        return;

    QUndoCommand *deleteCommand = new DeleteCommand(diagramScene);
    undoStack->push(deleteCommand);
}

void MainWindow::itemMenuAboutToHide()
{
    deleteAction->setEnabled(true);
}

void MainWindow::itemMenuAboutToShow()
{
    undoAction->setText(tr("Undo ") + undoStack->undoText());
    redoAction->setText(tr("Redo ") + undoStack->redoText());
    deleteAction->setEnabled(!diagramScene->selectedItems().isEmpty());
}

void MainWindow::addBox()
{
    QUndoCommand *addCommand = new AddCommand(DiagramItem::Box, diagramScene);
    undoStack->push(addCommand);
}

void MainWindow::addTriangle()
{
    QUndoCommand *addCommand = new AddCommand(DiagramItem::Triangle,
                                              diagramScene);
    undoStack->push(addCommand);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Undo"),
                       tr("The <b>Undo</b> example demonstrates how to "
                          "use libkcollaborate."));
}

void MainWindow::collaborateConnect()
{
    connectDialog->exec();
}

void MainWindow::collaborateShare()
{
    shareDialog->exec();
}
