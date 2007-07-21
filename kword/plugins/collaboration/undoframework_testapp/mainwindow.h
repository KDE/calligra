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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QToolBar;
class QMenu;
//class QUndoStack;
class QUndoView;
class DiagramScene;
class DiagramItem;

//new
namespace kcollaborate
{
    class CollaborationManager;
    class ConnectDialog;
    class ShareDialog;
    class UndoStack;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

public slots:
    void itemMoved(DiagramItem *movedDiagram, const QPointF &moveStartPosition);

private slots:
    void deleteItem();
    void addBox();
    void addTriangle();
    void about();
    void itemMenuAboutToShow();
    void itemMenuAboutToHide();

    void collaborateConnect();//new
    void collaborateShare();//new

private:
    void createActions();
    void createMenus();
    void createUndoView();

    QAction *deleteAction;
    QAction *addBoxAction;
    QAction *addTriangleAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *exitAction;
    QAction *aboutAction;
    QAction *connectAction;//new
    QAction *shareAction;//new

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *collaborateMenu;
    QMenu *itemMenu;
    QMenu *helpMenu;

    DiagramScene *diagramScene;
    kcollaborate::UndoStack *undoStack;
    QUndoView *undoView;

    kcollaborate::CollaborationManager *collaborationManager;//new
    kcollaborate::ConnectDialog *connectDialog;//new
    kcollaborate::ShareDialog *shareDialog;//new
//    kcollaborate::UserList *userList;//new
};

#endif
