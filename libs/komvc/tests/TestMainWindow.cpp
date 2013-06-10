#include "TestMainWindow.h"

#include <QMdiArea>
#include <QMenu>
#include <QMenuBar>
#include <KoControllerBase.h>

#include <TestView.h>
#include <TestDocument.h>

class TestMainWindow::Private {
public:
    QMdiArea *mdiArea;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *imageMenu;
    QMenu *layerMenu;
    QMenu *selectMenu;
    QMenu *filterMenu;
    QMenu *toolsMenu;
    QMenu *settingsMenu;
    QMenu *helpMenu;

    KoControllerBase *controller;
};

TestMainWindow::TestMainWindow(KoControllerBase *controller)
    : KoMainWindowBase()
    , d(new Private)
{
    d->controller = controller;

    d->mdiArea = new QMdiArea(this);
    d->mdiArea->setViewMode(QMdiArea::TabbedView);
    setCentralWidget(d->mdiArea);
}

TestMainWindow::~TestMainWindow()
{
    delete d;
}

bool TestMainWindow::addView(KoViewBase *view)
{
    view->setAttribute(Qt::WA_DeleteOnClose);
    d->mdiArea->addSubWindow(view);
    view->show();
    return true;
}

void TestMainWindow::createMenu()
{
    d->fileMenu = menuBar()->addMenu(tr("&File"));
    d->fileMenu->addAction(d->controller->actionByName(this, "File/New"));
    d->fileMenu->addAction(d->controller->actionByName(this, "File/Open"));
    d->fileMenu->addSeparator();
    d->fileMenu->addAction(d->controller->actionByName(this, "File/Save"));
    d->fileMenu->addAction(d->controller->actionByName(this, "File/SaveAs"));
    d->fileMenu->addAction(d->controller->actionByName(this, "File/SaveAll"));
    d->fileMenu->addAction(d->controller->actionByName(this, "File/Export"));
    d->fileMenu->addSeparator();
    d->fileMenu->addAction(d->controller->actionByName(this, "File/Close"));
    d->fileMenu->addAction(d->controller->actionByName(this, "File/CloseAll"));
    d->fileMenu->addSeparator();
    d->fileMenu->addAction(d->controller->actionByName(this, "Application/Exit"));

    d->editMenu = menuBar()->addMenu(tr("&Edit"));
    d->viewMenu = menuBar()->addMenu(tr("&View"));
    d->imageMenu = menuBar()->addMenu(tr("&Image"));
    d->layerMenu = menuBar()->addMenu(tr("&Layer"));
    d->selectMenu = menuBar()->addMenu(tr("&Select"));
    d->filterMenu = menuBar()->addMenu(tr("Filte&r"));
    d->toolsMenu = menuBar()->addMenu(tr("&Tools"));
    d->settingsMenu = menuBar()->addMenu(tr("&Settings"));
    d->helpMenu = menuBar()->addMenu(tr("&Help"));
}


KoMainWindowBase *TestMainWindowFactory::create(KoControllerBase *controller)
{
    TestMainWindow *window = new TestMainWindow(controller);
    controller->addWindow(window);
    window->createMenu();
    return window;
}
