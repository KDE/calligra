#include "KoControllerBase.h"
#include "KoMainWindowBase.h"

#include <QApplication>

class KoControllerBase::Private {
public:
    QMap<KoMainWindowBase*, QMap<QString, QAction*> > actionCollections;
};

KoControllerBase::KoControllerBase(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

void KoControllerBase::addWindow(KoMainWindowBase *window)
{
    Q_ASSERT(window);
    Q_ASSERT(!d->actionCollections.contains(window));

    connect(window, SIGNAL(aboutToClose()), this, SLOT(windowClosed()));

    QMap<QString, QAction*> actionCollection;

    // FILE

    // XXX: make these strings global constants
    actionCollection["File/New"] = new QAction(tr("&New"), window);
    actionCollection["File/New"]->setShortcut(QKeySequence::New);
    connect(actionCollection["File/New"], SIGNAL(activated()), window, SLOT(fileNew()));

    actionCollection["File/Open"] = new QAction(tr("&Open"), window);
    actionCollection["File/Open"]->setShortcut(QKeySequence::Open);
    connect(actionCollection["File/Open"], SIGNAL(activated()), window, SLOT(fileOpen()));

    actionCollection["File/Save"] = new QAction(tr("&Save"), window);
    actionCollection["File/Save"]->setShortcut(QKeySequence::Save);
    connect(actionCollection["File/Save"], SIGNAL(activated()), window, SLOT(fileSave()));

    actionCollection["File/SaveAs"] = new QAction(tr("Save &As"), window);
    actionCollection["File/SaveAs"]->setShortcut(QKeySequence::SaveAs);
    connect(actionCollection["File/SaveAs"], SIGNAL(activated()), window, SLOT(fileSaveAs()));

    actionCollection["File/SaveAll"] = new QAction(tr("Save A&ll"), window);
    connect(actionCollection["File/SaveAll"], SIGNAL(activated()), window, SLOT(fileSaveAll()));

    actionCollection["File/Export"] = new QAction(tr("&Export"), window);
    connect(actionCollection["File/Export"], SIGNAL(activated()), window, SLOT(fileExport()));

    actionCollection["File/Print"] = new QAction(tr("&Print"), window);
    actionCollection["File/Print"]->setShortcut(QKeySequence::Print);

    actionCollection["File/Refresh"] = new QAction(tr("&Reload"), window);
    actionCollection["File/Refresh"]->setShortcut(QKeySequence::Refresh);

    actionCollection["File/Close"] = new QAction(tr("Close &Window"), window);
    actionCollection["File/Close"]->setShortcut(QKeySequence::Close);
    connect(actionCollection["File/Close"], SIGNAL(activated()), window, SLOT(fileClose()));

    actionCollection["File/CloseAll"] = new QAction(tr("Close All Windows"), window);
    connect(actionCollection["File/CloseAll"], SIGNAL(activated()), window, SLOT(fileCloseAll()));

    actionCollection["Application/Exit"] = new QAction(tr("E&xit"), window);
    actionCollection["Application/Exit"]->setShortcut(QKeySequence::Quit);
    actionCollection["Application/Exit"]->setMenuRole(QAction::QuitRole);
    connect(actionCollection["Application/Exit"], SIGNAL(activated()), qApp, SLOT(quit()));

    // EDIT
    actionCollection["Edit/Undo"] = new QAction(tr("&Undo"), window);
    actionCollection["Edit/Undo"]->setShortcut(QKeySequence::Undo);

    actionCollection["Edit/Redo"] = new QAction(tr("&Redo"), window);
    actionCollection["Edit/Redo"]->setShortcut(QKeySequence::Redo);

    actionCollection["Edit/Copy"] = new QAction(tr("&Copy"), window);
    actionCollection["Edit/Copy"]->setShortcut(QKeySequence::Copy);

    actionCollection["Edit/Cut"] = new QAction(tr("&Cu&t"), window);
    actionCollection["Edit/Cut"]->setShortcut(QKeySequence::Cut);

    actionCollection["Edit/Paste"] = new QAction(tr("&Paste"), window);
    actionCollection["Edit/Paste"]->setShortcut(QKeySequence::Paste);

    actionCollection["Edit/Delete"] = new QAction(tr("&Delete"), window);
    actionCollection["Edit/Delete"]->setShortcut(QKeySequence::Delete);

    actionCollection["Edit/Replace"] = new QAction(tr("&Replace"), window);
    actionCollection["Edit/Replace"]->setShortcut(QKeySequence::Replace);

    actionCollection["Edit/Find"] = new QAction(tr("&Find"), window);
    actionCollection["Edit/Find"]->setShortcut(QKeySequence::Find);

    actionCollection["Edit/FindNext"] = new QAction(tr("Find &Next"), window);
    actionCollection["Edit/FindNext"]->setShortcut(QKeySequence::FindNext);

    actionCollection["Edit/FindPrevious"] = new QAction(tr("Find Pre&vious"), window);
    actionCollection["Edit/FindPrevious"]->setShortcut(QKeySequence::FindPrevious);

    actionCollection["Edit/FindPrevious"] = new QAction(tr("Find Pre&vious"), window);
    actionCollection["Edit/FindPrevious"]->setShortcut(QKeySequence::FindPrevious);

    actionCollection["Edit/SelectAll"] = new QAction(tr("Select &All"), window);
    actionCollection["Edit/SelectAll"]->setShortcut(QKeySequence::SelectAll);


    // VIEW
    actionCollection["View/ZoomIn"] = new QAction(tr("Zoom &In"), window);
    actionCollection["View/ZoomIn"]->setShortcut(QKeySequence::ZoomIn);

    actionCollection["View/ZoomOut"] = new QAction(tr("Zoom &Out"), window);
    actionCollection["View/ZoomOut"]->setShortcut(QKeySequence::ZoomOut);

    // WINDOW
    actionCollection["Window/NextChild"] = new QAction(tr("&Next Tab"), window);
    actionCollection["Window/NextChild"]->setShortcut(QKeySequence::NextChild);

    actionCollection["Window/PreviousChild"] = new QAction(tr("&Previous Tab"), window);
    actionCollection["Window/PreviousChild"]->setShortcut(QKeySequence::PreviousChild);


    // SETTINGS
    actionCollection["Settings/Preferences"] = new QAction(tr("&Preferences"), window);
    actionCollection["Settings/Preferences"]->setShortcut(QKeySequence::Preferences);

    // HELP
    actionCollection["Help/Contents"] = new QAction(tr("&Contents"), window);
    actionCollection["Help/Contents"]->setShortcut(QKeySequence::HelpContents);

    d->actionCollections[window] = actionCollection;
}

QAction *KoControllerBase::actionByName(KoMainWindowBase *window, const QString &name) const
{
    Q_ASSERT(d->actionCollections.contains(window));
    Q_ASSERT(d->actionCollections[window].contains(name));

    if (!d->actionCollections.contains(window)) return 0;
    if (!d->actionCollections[window].contains(name)) return 0;

    return d->actionCollections[window][name];
}

void KoControllerBase::windowClosed()
{
    KoMainWindowBase *window = qobject_cast<KoMainWindowBase*>(sender());
    if (window) {
        d->actionCollections.remove(window);
    }
}
