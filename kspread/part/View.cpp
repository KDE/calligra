/* This file is part of the KDE project
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2005-2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 1999-2003 Laurent Montel <montel@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 John Dailey <dailey@vt.edu>
   Copyright 1999-2003 David Faure <faure@kde.org>
   Copyright 1999-2001 Simon Hausmann <hausmann@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "View.h"
#include "TabBar.h"

// standard C/C++ includes
#include <assert.h>
#include <stdlib.h>
#include <time.h>

// Qt includes
#include <QBuffer>
#include <QByteArray>
#include <QClipboard>
#include <QCursor>
#include <QEvent>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QList>
#include <QMenu>
#include <QPixmap>
#include <QResizeEvent>
#include <QTimer>
#include <QToolButton>
#include <QSqlDatabase>
#include <QSizePolicy>
#include <QList>
#include <QScrollBar>

// KDE includes
#include <kactioncollection.h>
#include <kconfig.h>
#include <kcomponentdata.h>
#include <kdatatool.h>
#include <kdebug.h>

#include <KFontChooser>
#include <kinputdialog.h>
#include <kmessagebox.h>
#include <knewpassworddialog.h>
#include <kpassivepopup.h>
#include <kpassworddialog.h>
#include <kreplace.h>
#include <kreplacedialog.h>
#include <kstatusbar.h>
#include <kstandardaction.h>
#include <kstandarddirs.h>
#include <ktemporaryfile.h>
#include <KToggleAction>
#include <ktoolinvocation.h>
#include <kparts/event.h>
#include <kpushbutton.h>
#include <kxmlguifactory.h>
#include <kicon.h>
#include <knotifyconfigwidget.h>

// KOffice includes
#include <KoGlobal.h>
#include <KoDpi.h>
#include <KoCanvasController.h>
#include <KoMainWindow.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoOdfStylesReader.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoStore.h>
#include <KoToolBoxFactory.h>
#include <KoDockerManager.h>
#include <KoToolManager.h>
#include <KoToolRegistry.h>
#include <KoTemplateCreateDia.h>
#include <KoXmlNS.h>
#include <KoZoomAction.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>
#include <KoToolProxy.h>

// KSpread includes
#include "ApplicationSettings.h"
#include "BindingManager.h"
#include "CanvasResources.h"
#include "CellStorage.h"
#include "CellView.h"
#include "Damages.h"
#include "DependencyManager.h"
#include "Digest.h"
#include "Doc.h"
#include "Factory.h"
#include "LoadingInfo.h"
#include "Canvas.h"
#include "Editors.h"
#include "Global.h"
#include "Headers.h"
#include "Localization.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "RecalcManager.h"
#include "RowColumnFormat.h"
#include "Selection.h"
#include "ShapeApplicationData.h"
#include "Sheet.h"
#include "SheetPrint.h"
#include "SheetView.h"
#include "Style.h"
#include "StyleManager.h"
#include "StyleStorage.h"
#include "ToolRegistry.h"
#include "Util.h"
#include "ValueCalc.h"
#include "ValueConverter.h"
#include "PrintJob.h"

#include "MapViewModel.h"

// commands
#include "commands/BorderColorCommand.h"
#include "commands/CommentCommand.h"
#include "commands/DataManipulators.h"
#include "commands/DefinePrintRangeCommand.h"
#include "commands/RowColumnManipulators.h"
#include "commands/SheetCommands.h"
#include "commands/StyleCommand.h"
#include "commands/Undo.h"

// dialogs
#include "dialogs/PageLayoutDialog.h"
#include "dialogs/PreferenceDialog.h"
#include "dialogs/ShowDialog.h"

#include "dialogs/SheetPropertiesDialog.h"

// D-Bus
#include "interfaces/ViewAdaptor.h"
#include <QtDBus/QtDBus>

namespace KSpread
{
class ViewActions;

class View::Private
{
public:
    View* view;
    Doc* doc;

    // the active sheet, may be 0
    // this is the sheet which has the input focus
    Sheet* activeSheet;
    MapViewModel* mapViewModel;
    QHash<const Sheet*, SheetView*> sheetViews;

    // GUI elements
    QWidget *frame;
    Canvas *canvas;
    KoCanvasController* canvasController;
    KoZoomController* zoomController;
    KoZoomHandler* zoomHandler;
    RowHeader *rowHeader;
    ColumnHeader *columnHeader;
    SelectAllButton* selectAllButton;
    QScrollBar *horzScrollBar;
    QScrollBar *vertScrollBar;
    TabBar *tabBar;
    QLabel* calcLabel;
    QGridLayout* viewLayout;
    QGridLayout* tabScrollBarLayout;

    // all UI actions
    ViewActions* actions;

    // if true, kspread is still loading the document
    // don't try to refresh the view
    bool loading;

    // selection/marker
    Selection* selection;
    QMap<Sheet*, QPoint> savedAnchors;
    QMap<Sheet*, QPoint> savedMarkers;
    QMap<Sheet*, QPointF> savedOffsets;

    // the tools
    struct ToolEntry {
        QString command;
        KDataToolInfo info;
    };
    QList<ToolEntry*> toolList;

    void initActions();
    void adjustActions(bool mode);
    void adjustWorkbookActions(bool mode);
    QAbstractButton* newIconButton(const char *_file, bool _kbutton = false, QWidget *_parent = 0);

    // On timeout this will execute the status bar operation (e.g. SUM).
    // This is delayed to speed up the selection.
    QTimer statusBarOpTimer;
};

class ViewActions
{
public:
    // sheet/workbook operations
    QAction * sheetProperties;
    QAction * insertSheet;
    QAction * duplicateSheet;
    QAction * deleteSheet;
    QAction * renameSheet;
    QAction * hideSheet;
    QAction * showSheet;

    //Shape manipulation
    KAction * deleteShape;

    // page layout
    QAction * paperLayout;
    QAction * resetPrintRange;
    KToggleAction* showPageBorders;

    // recalculation
    KAction * recalcWorksheet;
    KAction * recalcWorkbook;

    // protection
    KToggleAction* protectSheet;
    KToggleAction* protectDoc;

    // navigation
    KAction * nextSheet;
    KAction * prevSheet;
    QAction * firstSheet;
    QAction * lastSheet;

    // misc
    QAction * createTemplate;
    KSelectAction* shapeAnchor;

    // settings
    KToggleAction* showColumnHeader;
    KToggleAction* showRowHeader;
    KToggleAction* showHorizontalScrollBar;
    KToggleAction* showVerticalScrollBar;
    KToggleAction* showStatusBar;
    KToggleAction* showTabBar;
    QAction * preference;

    // running calculation
    KToggleAction* calcNone;
    KToggleAction* calcMin;
    KToggleAction* calcMax;
    KToggleAction* calcAverage;
    KToggleAction* calcCount;
    KToggleAction* calcSum;
    KToggleAction* calcCountA;
};


void View::Private::initActions()
{
    actions = new ViewActions;

    KActionCollection* ac = view->actionCollection();

    // -- cell operation actions --

    // -- sheet/workbook actions --
    actions->sheetProperties  = new KAction(i18n("Sheet Properties"), view);
    ac->addAction("sheetProperties", actions->sheetProperties);
    connect(actions->sheetProperties, SIGNAL(triggered(bool)), view, SLOT(sheetProperties()));
    actions->sheetProperties->setToolTip(i18n("Modify current sheet's properties"));

    actions->insertSheet = new KAction(KIcon("insert-table"), i18n("Sheet"), view);
    actions->insertSheet->setIconText(i18n("Insert Sheet"));
    actions->insertSheet->setToolTip(i18n("Insert a new sheet"));
    ac->addAction("insertSheet", actions->insertSheet);
    connect(actions->insertSheet, SIGNAL(triggered(bool)), view, SLOT(insertSheet()));

    actions->duplicateSheet = new KAction(/*KIcon("inserttable"),*/ i18n("Duplicate Sheet"), view);
    actions->duplicateSheet->setToolTip(i18n("Duplicate the selected sheet"));
    ac->addAction("duplicateSheet", actions->duplicateSheet);
    connect(actions->duplicateSheet, SIGNAL(triggered(bool)), view, SLOT(duplicateSheet()));

    actions->deleteSheet = new KAction(KIcon("edit-delete"), i18n("Sheet"), view);
    actions->deleteSheet->setIconText(i18n("Remove Sheet"));
    actions->deleteSheet->setToolTip(i18n("Remove the active sheet"));
    ac->addAction("deleteSheet", actions->deleteSheet);
    connect(actions->deleteSheet, SIGNAL(triggered(bool)), view, SLOT(deleteSheet()));

    actions->renameSheet  = new KAction(i18n("Rename Sheet..."), view);
    ac->addAction("renameSheet", actions->renameSheet);
    connect(actions->renameSheet, SIGNAL(triggered(bool)), view, SLOT(slotRename()));
    actions->renameSheet->setToolTip(i18n("Rename the active sheet"));

    actions->showSheet  = new KAction(i18n("Show Sheet..."), view);
    ac->addAction("showSheet", actions->showSheet);
    connect(actions->showSheet, SIGNAL(triggered(bool)), view, SLOT(showSheet()));
    actions->showSheet->setToolTip(i18n("Show a hidden sheet"));

    actions->hideSheet  = new KAction(i18n("Hide Sheet"), view);
    ac->addAction("hideSheet", actions->hideSheet);
    connect(actions->hideSheet, SIGNAL(triggered(bool)), view, SLOT(hideSheet()));
    actions->hideSheet->setToolTip(i18n("Hide the active sheet"));

    actions->paperLayout  = new KAction(i18n("Page Layout..."), view);
    ac->addAction("paperLayout", actions->paperLayout);
    connect(actions->paperLayout, SIGNAL(triggered(bool)), view, SLOT(paperLayoutDlg()));
    actions->paperLayout->setToolTip(i18n("Specify the layout of the spreadsheet for a printout"));

    actions->resetPrintRange  = new KAction(i18n("Reset Print Range"), view);
    ac->addAction("resetPrintRange", actions->resetPrintRange);
    connect(actions->resetPrintRange, SIGNAL(triggered(bool)), view, SLOT(resetPrintRange()));
    actions->resetPrintRange->setToolTip(i18n("Reset the print range in the current sheet"));

    actions->showPageBorders = new KToggleAction(i18n("Page Borders"), view);
    actions->showPageBorders->setToolTip(i18n("Show on the spreadsheet where the page borders will be"));
    ac->addAction("showPageBorders", actions->showPageBorders);
    connect(actions->showPageBorders, SIGNAL(toggled(bool)), view, SLOT(togglePageBorders(bool)));

    actions->recalcWorksheet  = new KAction(i18n("Recalculate Sheet"), view);
    actions->recalcWorksheet->setIcon(KIcon("view-refresh"));
    actions->recalcWorksheet->setIconText(i18n("Recalculate"));
    ac->addAction("RecalcWorkSheet", actions->recalcWorksheet);
    actions->recalcWorksheet->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F9));
    connect(actions->recalcWorksheet, SIGNAL(triggered(bool)), view, SLOT(recalcWorkSheet()));
    actions->recalcWorksheet->setToolTip(i18n("Recalculate the value of every cell in the current worksheet"));

    actions->recalcWorkbook  = new KAction(i18n("Recalculate Document"), view);
    actions->recalcWorkbook->setIcon(KIcon("view-refresh"));
    actions->recalcWorkbook->setIconText(i18n("Recalculate"));
    ac->addAction("RecalcWorkBook", actions->recalcWorkbook);
    actions->recalcWorkbook->setShortcut(QKeySequence(Qt::Key_F9));
    connect(actions->recalcWorkbook, SIGNAL(triggered(bool)), view, SLOT(recalcWorkBook()));
    actions->recalcWorkbook->setToolTip(i18n("Recalculate the value of every cell in all worksheets"));

    actions->protectSheet  = new KToggleAction(i18n("Protect &Sheet..."), view);
    ac->addAction("protectSheet", actions->protectSheet);
    actions->protectSheet->setToolTip(i18n("Protect the sheet from being modified"));
    connect(actions->protectSheet, SIGNAL(toggled(bool)),
            view, SLOT(toggleProtectSheet(bool)));

    actions->protectDoc  = new KToggleAction(i18n("Protect &Document..."), view);
    ac->addAction("protectDoc", actions->protectDoc);
    actions->protectDoc->setToolTip(i18n("Protect the document from being modified"));
    connect(actions->protectDoc, SIGNAL(toggled(bool)), view, SLOT(toggleProtectDoc(bool)));

    // -- misc actions --

    actions->createTemplate  = new KAction(i18n("&Create Template From Document..."), view);
    ac->addAction("createTemplate", actions->createTemplate);
    connect(actions->createTemplate, SIGNAL(triggered(bool)), view, SLOT(createTemplate()));

    actions->shapeAnchor = new KSelectAction(i18n("Anchor"), view);
    actions->shapeAnchor->addAction(i18n("Cell"));
    actions->shapeAnchor->addAction(i18n("Page"));
    actions->shapeAnchor->setEnabled(false);
    actions->shapeAnchor->setToolTip(i18n("Switch shape anchoring"));
    ac->addAction("shapeAnchor", actions->shapeAnchor);
    connect(actions->shapeAnchor, SIGNAL(triggered(const QString&)),
            view, SLOT(setShapeAnchoring(const QString&)));

    // -- navigation actions --

    actions->nextSheet  = new KAction(KIcon("go-next"), i18n("Next Sheet"), view);
    actions->nextSheet->setIconText(i18n("Next"));
    actions->nextSheet->setToolTip(i18n("Move to the next sheet"));
    ac->addAction("nextSheet", actions->nextSheet);
    actions->nextSheet->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageDown));
    connect(actions->nextSheet, SIGNAL(triggered(bool)), view, SLOT(nextSheet()));

    actions->prevSheet  = new KAction(KIcon("go-previous"), i18n("Previous Sheet"), view);
    actions->prevSheet->setIconText(i18n("Previous"));
    actions->prevSheet->setToolTip(i18n("Move to the previous sheet"));
    ac->addAction("previousSheet", actions->prevSheet);
    actions->prevSheet->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageUp));
    connect(actions->prevSheet, SIGNAL(triggered(bool)), view, SLOT(previousSheet()));

    actions->firstSheet  = new KAction(KIcon("go-first"), i18n("First Sheet"), view);
    actions->firstSheet->setIconText(i18n("First"));
    actions->firstSheet->setToolTip(i18n("Move to the first sheet"));
    ac->addAction("firstSheet", actions->firstSheet);
    connect(actions->firstSheet, SIGNAL(triggered(bool)), view, SLOT(firstSheet()));

    actions->lastSheet  = new KAction(KIcon("go-last"), i18n("Last Sheet"), view);
    actions->lastSheet->setIconText(i18n("Last"));
    actions->lastSheet->setToolTip(i18n("Move to the last sheet"));
    ac->addAction("lastSheet", actions->lastSheet);
    connect(actions->lastSheet, SIGNAL(triggered(bool)), view, SLOT(lastSheet()));

    // -- settings actions --

    actions->showColumnHeader = new KToggleAction(i18n("Column Header"), view);
    actions->showColumnHeader->setToolTip(i18n("Show the column header"));
    ac->addAction("showColumnHeader", actions->showColumnHeader);
    connect(actions->showColumnHeader, SIGNAL(toggled(bool)),
            view, SLOT(showColumnHeader(bool)));

    actions->showRowHeader = new KToggleAction(i18n("Row Header"), view);
    actions->showRowHeader->setToolTip(i18n("Show the row header"));
    ac->addAction("showRowHeader", actions->showRowHeader);
    connect(actions->showRowHeader, SIGNAL(toggled(bool)),
            view, SLOT(showRowHeader(bool)));

    actions->showHorizontalScrollBar = new KToggleAction(i18n("Horizontal Scrollbar"), view);
    actions->showHorizontalScrollBar->setToolTip(i18n("Show the horizontal scrollbar"));
    ac->addAction("showHorizontalScrollBar", actions->showHorizontalScrollBar);
    connect(actions->showHorizontalScrollBar, SIGNAL(toggled(bool)),
            view, SLOT(showHorizontalScrollBar(bool)));

    actions->showVerticalScrollBar = new KToggleAction(i18n("Vertical Scrollbar"), view);
    actions->showVerticalScrollBar->setToolTip(i18n("Show the vertical scrollbar"));
    ac->addAction("showVerticalScrollBar", actions->showVerticalScrollBar);
    connect(actions->showVerticalScrollBar, SIGNAL(toggled(bool)),
            view, SLOT(showVerticalScrollBar(bool)));

    actions->showStatusBar = new KToggleAction(i18n("Status Bar"), view);
    actions->showStatusBar->setToolTip(i18n("Show the status bar"));
    ac->addAction("showStatusBar", actions->showStatusBar);
    connect(actions->showStatusBar, SIGNAL(toggled(bool)),
            view, SLOT(showStatusBar(bool)));

    actions->showTabBar = new KToggleAction(i18n("Tab Bar"), view);
    actions->showTabBar->setToolTip(i18n("Show the tab bar"));
    ac->addAction("showTabBar", actions->showTabBar);
    connect(actions->showTabBar, SIGNAL(toggled(bool)),
            view, SLOT(showTabBar(bool)));

    actions->preference = KStandardAction::preferences(view, SLOT(preference()), view);
    actions->preference->setToolTip(i18n("Set various KSpread options"));
    ac->addAction("preference", actions->preference);

    KAction *notifyAction = KStandardAction::configureNotifications(view, SLOT(optionsNotifications()), view);
    ac->addAction("configureNotifications", notifyAction);

    // -- calculation actions --
    //
    QActionGroup* groupCalc = new QActionGroup(view);
    actions->calcNone  = new KToggleAction(i18n("None"), view);
    ac->addAction("menu_none", actions->calcNone);
    connect(actions->calcNone, SIGNAL(toggled(bool)),
            view, SLOT(menuCalc(bool)));
    actions->calcNone->setToolTip(i18n("No calculation"));
    actions->calcNone->setActionGroup(groupCalc);

    actions->calcSum  = new KToggleAction(i18n("Sum"), view);
    ac->addAction("menu_sum", actions->calcSum);
    connect(actions->calcSum, SIGNAL(toggled(bool)),
            view, SLOT(menuCalc(bool)));
    actions->calcSum->setToolTip(i18n("Calculate using sum"));
    actions->calcSum->setActionGroup(groupCalc);

    actions->calcMin  = new KToggleAction(i18n("Min"), view);
    ac->addAction("menu_min", actions->calcMin);
    connect(actions->calcMin, SIGNAL(toggled(bool)),
            view, SLOT(menuCalc(bool)));
    actions->calcMin->setToolTip(i18n("Calculate using minimum"));
    actions->calcMin->setActionGroup(groupCalc);

    actions->calcMax  = new KToggleAction(i18n("Max"), view);
    ac->addAction("menu_max", actions->calcMax);
    connect(actions->calcMax, SIGNAL(toggled(bool)),
            view, SLOT(menuCalc(bool)));
    actions->calcMax->setToolTip(i18n("Calculate using maximum"));
    actions->calcMax->setActionGroup(groupCalc);

    actions->calcAverage  = new KToggleAction(i18n("Average"), view);
    ac->addAction("menu_average", actions->calcAverage);
    connect(actions->calcAverage, SIGNAL(toggled(bool)),
            view, SLOT(menuCalc(bool)));
    actions->calcAverage->setToolTip(i18n("Calculate using average"));
    actions->calcAverage->setActionGroup(groupCalc);

    actions->calcCount  = new KToggleAction(i18n("Count"), view);
    ac->addAction("menu_count", actions->calcCount);
    connect(actions->calcCount, SIGNAL(toggled(bool)),
            view, SLOT(menuCalc(bool)));
    actions->calcCount->setToolTip(i18n("Calculate using the count"));
    actions->calcCount->setActionGroup(groupCalc);

    actions->calcCountA  = new KToggleAction(i18n("CountA"), view);
    ac->addAction("menu_counta", actions->calcCountA);
    connect(actions->calcCountA, SIGNAL(toggled(bool)),
            view, SLOT(menuCalc(bool)));
    actions->calcCountA->setToolTip(i18n("Calculate using the countA"));
    actions->calcCountA->setActionGroup(groupCalc);

    //Shape actions
    actions->deleteShape = new KAction(KIcon("edit-delete"), i18n("Delete"), view);
    actions->deleteShape->setShortcut(QKeySequence("Del"));
    connect(actions->deleteShape, SIGNAL(triggered()), view, SLOT(editDeleteSelection()));
    connect(canvas->toolProxy(), SIGNAL(selectionChanged(bool)), actions->deleteShape, SLOT(setEnabled(bool)));
    ac->addAction("edit_delete", actions->deleteShape);

    // -- special action, only for developers --
    //

    ac->addAssociatedWidget(view->canvasWidget());
    foreach(QAction* action, ac->actions()) {
        action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    }
}

void View::Private::adjustActions(bool mode)
{
    actions->recalcWorkbook->setEnabled(mode);
    actions->recalcWorksheet->setEnabled(mode);
    actions->paperLayout->setEnabled(mode);
    actions->resetPrintRange->setEnabled(mode);
    actions->deleteSheet->setEnabled(mode);
    actions->calcMin->setEnabled(mode);
    actions->calcMax->setEnabled(mode);
    actions->calcAverage->setEnabled(mode);
    actions->calcCount->setEnabled(mode);
    actions->calcCountA->setEnabled(mode);
    actions->calcSum->setEnabled(mode);
    actions->calcNone->setEnabled(mode);

    if (mode && !view->doc()->map()->isProtected())
        actions->renameSheet->setEnabled(true);
    else
        actions->renameSheet->setEnabled(false);

    actions->showColumnHeader->setChecked(view->doc()->map()->settings()->showColumnHeader());
    actions->showRowHeader->setChecked(view->doc()->map()->settings()->showRowHeader());
    actions->showHorizontalScrollBar->setChecked(view->doc()->map()->settings()->showHorizontalScrollBar());
    actions->showVerticalScrollBar->setChecked(view->doc()->map()->settings()->showVerticalScrollBar());
    actions->showStatusBar->setChecked(view->doc()->map()->settings()->showStatusBar());
    actions->showTabBar->setChecked(view->doc()->map()->settings()->showTabBar());

    if (activeSheet)
        selection->update();
}

void View::Private::adjustWorkbookActions(bool mode)
{
    tabBar->setReadOnly(!view->doc()->isReadWrite() || view->doc()->map()->isProtected());

    actions->hideSheet->setEnabled(mode);
    actions->showSheet->setEnabled(mode);
    actions->insertSheet->setEnabled(mode);
    actions->duplicateSheet->setEnabled(mode);
    actions->deleteSheet->setEnabled(mode);

    if (mode) {
        if (activeSheet && !activeSheet->isProtected()) {
            bool state = (view->doc()->map()->visibleSheets().count() > 1);
            actions->deleteSheet->setEnabled(state);
            actions->hideSheet->setEnabled(state);
        }
        actions->showSheet->setEnabled(view->doc()->map()->hiddenSheets().count() > 0);
        actions->renameSheet->setEnabled(activeSheet && !activeSheet->isProtected());
    }
}

QAbstractButton* View::Private::newIconButton(const char *_file, bool _kbutton, QWidget *_parent)
{
    if (_parent == 0)
        _parent = view;

    if (!_kbutton) {
        QPushButton* pb = new QPushButton(_parent);
        pb->setIcon(KIcon(_file));
        return pb;
    } else {
        QToolButton* pb = new QToolButton(_parent);
        pb->setIcon(KIcon(_file));
        return pb;
    }
}


/*****************************************************************************
 *
 * View
 *
 *****************************************************************************/

View::View(QWidget *_parent, Doc *_doc)
        : KoView(_doc, _parent)
        , d(new Private)
{
    ElapsedTime et("View constructor");
//     kDebug() <<"sizeof(Cell)=" << sizeof(Cell);

    d->view = this;
    d->doc = _doc;

    d->activeSheet = 0;

    d->loading = true;

    setComponentData(Factory::global());
    if (doc()->isReadWrite())
        setXMLFile("kspread.rc");
    else
        setXMLFile("kspread_readonly.rc");

    // GUI Initializations
    initView();

    d->initActions();

    connect(doc(), SIGNAL(sig_refreshView()), this, SLOT(slotRefreshView()));

    connect(doc()->map(), SIGNAL(sheetAdded(Sheet*)),
            this, SLOT(slotAddSheet(Sheet*)));
    connect(doc()->map(), SIGNAL(sheetRemoved(Sheet*)),
            this, SLOT(removeSheet(Sheet*)));
    connect(doc()->map(), SIGNAL(sheetRevived(Sheet*)),
            this, SLOT(reviveSheet(Sheet*)));
    connect(doc()->map(), SIGNAL(damagesFlushed(const QList<Damage*>&)),
            this, SLOT(handleDamages(const QList<Damage*>&)));

    if (!doc()->isReadWrite()) {
        setZoom(100, true);
    }

    connect(&d->statusBarOpTimer, SIGNAL(timeout()), this, SLOT(calcStatusBarOp()));

    // Delay the setting of the initial position, because
    // we have to wait for the widget to be shown. Otherwise,
    // we get a wrong widget size.
    // This is the last operation for the "View loading" process.
    // The loading flag will be unset at its end.
    // Don't try to delay this init cause following operations will assume
    // the sheets are already fully setup.
    initialPosition();

    new ViewAdaptor(this);
    d->canvas->setFocus();
}

View::~View()
{
    //  ElapsedTime el( "~View" );
    if (doc()->isReadWrite())   // make sure we're not embedded in Konq
        selection()->emitCloseEditor(true); // save changes

    // if (d->calcLabel) disconnect(d->calcLabel,SIGNAL(pressed( int )),this,SLOT(statusBarClicked(int)));

    d->selection->emitCloseEditor(false);
    d->selection->endReferenceSelection(false);
    d->activeSheet = 0; // set the active sheet to 0 so that when during destruction
    // of embedded child documents possible repaints in Sheet are not
    // performed. The repains can happen if you delete an embedded document,
    // which leads to an regionInvalidated() signal emission in KoView, which calls
    // repaint, etc.etc. :-) (Simon)

    // delete the sheetView's after calling d->selection->emitCloseEditor cause the
    // emitCloseEditor may trigger over the Selection::emitChanged a Canvas::scrollToCell
    // which in turn needs the sheetview's to access the sheet itself.
    qDeleteAll(d->sheetViews);

    delete d->selection;
    delete d->calcLabel;
    delete d->actions;
    delete d->zoomHandler;
    
    // NOTE sebsauer: first unregister the event-handler, then delete the canvas and then we are save to
    // call removeCanvasController without crashing.
    //d->canvasController->canvas()->canvasWidget()->removeEventFilter(d->canvasController);
    //delete d->canvasController->canvas();
    // NOTE sebsauer: We need to remove the canvasController right before deleting it and
    // nothing needs to be done in between cause flake does first delete the TableTool
    // on removeCanvasController and the proxytool which points to that TableTool later
    // while the canvasController is destroyed. That means, that we will have a dangling
    // pointer in the KoToolProxy that points to the KoToolBase the time in between.
    KoToolManager::instance()->removeCanvasController(d->canvasController);
    // NOTE Stefan: Delete the Canvas explicitly, even if it has this view as
    //              parent. Otherwise, it leads to crashes, because it tries to
    //              access this View in some events (Bug #126492).
    //              The KoCanvasController takes ownership of the Canvas and does the deletion.
    delete d->canvasController;
    delete d;
}

Doc* View::doc() const
{
    return d->doc;
}

// should be called only once, from the constructor
/*
 * Top part is the formula bar.
 * Central part is the canvas, row header and vertical scrollbar.
 * Bottom part is the tab bar and horizontal scrollbar.
 *
 * Note that canvas must the one to be created, since other
 * widgets might depend on it.
 */

void View::initView()
{
    d->viewLayout = new QGridLayout(this);
    d->viewLayout->setMargin(0);
    d->viewLayout->setSpacing(0);

    // Setup the Canvas and its controller.
    d->canvas = new Canvas(this);
    d->canvasController = new KoCanvasController(this);
    d->canvasController->setCanvas(d->canvas);
    d->canvasController->setCanvasMode(KoCanvasController::Infinite);
    d->canvasController->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->canvasController->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Setup the map model.
    d->mapViewModel = new MapViewModel(d->doc->map());
    connect(d->mapViewModel, SIGNAL(addCommandRequested(QUndoCommand*)),
            doc(), SLOT(addCommand(QUndoCommand*)));
    connect(d->mapViewModel, SIGNAL(activeSheetChanged(Sheet*)),
            this, SLOT(setActiveSheet(Sheet*)));

    // Setup the selection.
    d->selection = new Selection(d->canvas);
    connect(d->selection, SIGNAL(changed(const Region&)), this, SLOT(slotChangeSelection(const Region&)));
    connect(d->selection, SIGNAL(changed(const Region&)), this, SLOT(slotScrollChoice(const Region&)));
    connect(d->selection, SIGNAL(aboutToModify(const Region&)), this, SLOT(aboutToModify(const Region&)));
    connect(d->selection, SIGNAL(modified(const Region&)), this, SLOT(refreshSelection(const Region&)));
    connect(d->selection, SIGNAL(visibleSheetRequested(Sheet*)), this, SLOT(setActiveSheet(Sheet*)));
    connect(d->selection, SIGNAL(refreshSheetViews()), this, SLOT(refreshSheetViews()));
    connect(this, SIGNAL(documentReadWriteToggled(bool)),
            d->selection, SIGNAL(documentReadWriteToggled(bool)));
    connect(this, SIGNAL(sheetProtectionToggled(bool)),
            d->selection, SIGNAL(sheetProtectionToggled(bool)));

    // Let the selection pointer become a canvas resource.
    QVariant variant;
    variant.setValue<void*>(d->selection);
    d->canvas->resourceManager()->setResource(CanvasResource::Selection, variant);
    variant.setValue<QObject*>(doc()->map()->bindingManager());

    // Load the KSpread Tools
    ToolRegistry::instance();

    if (shell())
    {
        // Setup the tool dock widget.
        KoToolManager::instance()->addController(d->canvasController);
        KoToolManager::instance()->registerTools(actionCollection(), d->canvasController);
        KoToolBoxFactory toolBoxFactory(d->canvasController, i18n("Tools"));
        shell()->createDockWidget(&toolBoxFactory);

        // Setup the tool options dock widget manager.
        connect(d->canvasController, SIGNAL(toolOptionWidgetsChanged(const QMap<QString, QWidget *> &, QWidget*)),
                shell()->dockerManager(), SLOT(newOptionWidgets(const  QMap<QString, QWidget *> &, QWidget*)));
    }
    // Setup the zoom controller.
    d->zoomHandler = new KoZoomHandler();
    d->zoomController = new KoZoomController(d->canvasController, d->zoomHandler, actionCollection(), 0, this);
    d->zoomController->zoomAction()->setZoomModes(KoZoomMode::ZOOM_CONSTANT);
    addStatusBarItem(d->zoomController->zoomAction()->createWidget(statusBar()), 0, true);
    connect(d->zoomController, SIGNAL(zoomChanged(KoZoomMode::Mode, qreal)),
            this, SLOT(viewZoom(KoZoomMode::Mode, qreal)));

    d->columnHeader = new ColumnHeader(this, d->canvas, this);
    d->rowHeader = new RowHeader(this, d->canvas , this);
    d->columnHeader->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    d->rowHeader->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    d->selectAllButton = new SelectAllButton(d->canvas, d->selection);
    d->selectAllButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    d->canvas->setFocusPolicy(Qt::StrongFocus);
    QWidget::setFocusPolicy(Qt::StrongFocus);
    setFocusProxy(d->canvas);

    connect(this, SIGNAL(invalidated()), d->canvas, SLOT(update()));

    // Vert. Scroll Bar
    d->calcLabel  = 0;
    d->vertScrollBar = new QScrollBar(this);
    d->canvasController->setVerticalScrollBar(d->vertScrollBar);
//     d->vertScrollBar->setRange( 0, 4096 );
    d->vertScrollBar->setOrientation(Qt::Vertical);
    d->vertScrollBar->setSingleStep(60);  //just random guess based on what feels okay
    d->vertScrollBar->setPageStep(60);  //This should be controlled dynamically, depending on how many rows are shown

    QWidget* bottomPart = new QWidget(this);
    d->tabScrollBarLayout = new QGridLayout(bottomPart);
    d->tabScrollBarLayout->setMargin(0);
    d->tabScrollBarLayout->setSpacing(0);
    d->tabBar = new TabBar(0);
    d->tabScrollBarLayout->addWidget(d->tabBar, 0, 0);
    d->horzScrollBar = new QScrollBar(0);
    d->canvasController->setHorizontalScrollBar(d->horzScrollBar);
    d->tabScrollBarLayout->addWidget(d->horzScrollBar, 0, 1, 2, 1, Qt::AlignVCenter);

//     d->horzScrollBar->setRange( 0, 4096 );
    d->horzScrollBar->setOrientation(Qt::Horizontal);
    d->horzScrollBar->setSingleStep(60); //just random guess based on what feels okay
    d->horzScrollBar->setPageStep(60);

    connect(d->tabBar, SIGNAL(tabChanged(const QString&)), this, SLOT(changeSheet(const QString&)));
    connect(d->tabBar, SIGNAL(tabMoved(unsigned, unsigned)),
            this, SLOT(moveSheet(unsigned, unsigned)));
    connect(d->tabBar, SIGNAL(contextMenu(const QPoint&)),
            this, SLOT(popupTabBarMenu(const QPoint&)));
    connect(d->tabBar, SIGNAL(doubleClicked()),
            this, SLOT(slotRename()));

    int extent = this->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    if (style()->styleHint(QStyle::SH_ScrollView_FrameOnlyAroundContents)) {
        extent += style()->pixelMetric(QStyle::PM_DefaultFrameWidth) * 2;
    }

    d->viewLayout->setColumnStretch(1, 10);
    d->viewLayout->setRowStretch(2, 10);
    d->viewLayout->addWidget(d->selectAllButton, 1, 0);
    d->viewLayout->addWidget(d->columnHeader, 1, 1, 1, 1);
    d->viewLayout->addWidget(d->rowHeader, 2, 0);
    d->viewLayout->addWidget(d->canvasController, 2, 1);
    d->viewLayout->addWidget(d->vertScrollBar, 1, 2, 2, 1, Qt::AlignHCenter);
    d->viewLayout->addWidget(bottomPart, 3, 0, 1, 2);
    d->viewLayout->setColumnMinimumWidth(2, extent);
    d->viewLayout->setRowMinimumHeight(3, extent);

    KStatusBar * sb = statusBar();
    d->calcLabel = sb ? new QLabel(sb) : 0;
    if (d->calcLabel) {
        d->calcLabel->setContextMenuPolicy(Qt::CustomContextMenu);
        addStatusBarItem(d->calcLabel, 0);
        connect(d->calcLabel , SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(statusBarClicked(QPoint)));
    }

    // signal slot
//     connect( d->vertScrollBar, SIGNAL( valueChanged(int) ), d->canvas, SLOT( slotScrollVert(int) ) );
//     connect( d->horzScrollBar, SIGNAL( valueChanged(int) ), d->canvas, SLOT( slotScrollHorz(int) ) );
    connect(d->canvas, SIGNAL(documentSizeChanged(const QSize&)),
            d->canvasController, SLOT(setDocumentSize(const QSize&)));
    connect(d->canvasController, SIGNAL(moveDocumentOffset(const QPoint&)),
            d->canvas, SLOT(setDocumentOffset(const QPoint&)));
    connect(d->canvas->shapeManager(), SIGNAL(selectionChanged()),
            this, SLOT(shapeSelectionChanged()));
}

Canvas* View::canvasWidget() const
{
    return d->canvas;
}

KoZoomController *View::zoomController() const
{
    return d->zoomController;
}

KoCanvasController* View::canvasController() const
{
    return d->canvasController;
}

ColumnHeader* View::columnHeader()const
{
    return d->columnHeader;
}

RowHeader* View::rowHeader()const
{
    return d->rowHeader;
}

QScrollBar* View::horzScrollBar()const
{
    return d->horzScrollBar;
}

QScrollBar* View::vertScrollBar()const
{
    return d->vertScrollBar;
}

TabBar* View::tabBar() const
{
    return d->tabBar;
}

KoZoomHandler* View::zoomHandler() const
{
    return d->zoomHandler;
}

bool View::isLoading() const
{
    return d->loading;
}

Selection* View::selection() const
{
    return d->selection;
}

const Sheet* View::activeSheet() const
{
    return d->activeSheet;
}

Sheet* View::activeSheet()
{
    return d->activeSheet;
}

SheetView* View::sheetView(const Sheet* sheet) const
{
    if (!d->sheetViews.contains(sheet)) {
        kDebug(36004) << "View: Creating SheetView for" << sheet->sheetName();
        d->sheetViews.insert(sheet, new SheetView(sheet));
        d->sheetViews[ sheet ]->setPaintDevice(d->canvas);
        d->sheetViews[ sheet ]->setViewConverter(zoomHandler());
        connect(d->sheetViews[ sheet ], SIGNAL(visibleSizeChanged(const QSizeF&)),
                d->canvas, SLOT(setDocumentSize(const QSizeF&)));
        connect(d->sheetViews[ sheet ], SIGNAL(visibleSizeChanged(const QSizeF&)),
                d->zoomController, SLOT(setDocumentSize(const QSizeF&)));
        connect(sheet, SIGNAL(visibleSizeChanged()),
                d->sheetViews[ sheet ], SLOT(updateAccessedCellRange()));
    }
    return d->sheetViews[ sheet ];
}

void View::refreshSheetViews()
{
    const QList<SheetView*> sheetViews = d->sheetViews.values();
    for (int i = 0; i < sheetViews.count(); ++i) {
        disconnect(sheetViews[i], SIGNAL(visibleSizeChanged(const QSizeF&)),
                   d->canvas, SLOT(setDocumentSize(const QSizeF&)));
        disconnect(sheetViews[i], SIGNAL(visibleSizeChanged(const QSizeF&)),
                   d->zoomController, SLOT(setDocumentSize(const QSizeF&)));
        disconnect(sheetViews[i]->sheet(), SIGNAL(visibleSizeChanged()),
                   sheetViews[i], SLOT(updateAccessedCellRange()));
    }
    qDeleteAll(d->sheetViews);
    d->sheetViews.clear();
    const QList<Sheet*> sheets = d->doc->map()->sheetList();
    for (int i = 0; i < sheets.count(); ++i)
        sheets[i]->cellStorage()->styleStorage()->invalidateCache();
}

void View::refreshSelection(const Region& region)
{
    doc()->map()->addDamage(new CellDamage(activeSheet(), region, CellDamage::Appearance));
}

void View::aboutToModify(const Region& region)
{
    Q_UNUSED(region);
    selection()->emitCloseEditor(true); // save changes
}

void View::initConfig()
{
    KSharedConfigPtr config = Factory::global().config();
    const KConfigGroup parameterGroup = config->group("Parameters");
    const bool configFromDoc = doc()->configLoadFromFile();
    if (!configFromDoc) {
        doc()->map()->settings()->setShowHorizontalScrollBar(parameterGroup.readEntry("Horiz ScrollBar", true));
        doc()->map()->settings()->setShowVerticalScrollBar(parameterGroup.readEntry("Vert ScrollBar", true));
    }
    doc()->map()->settings()->setShowColumnHeader(parameterGroup.readEntry("Column Header", true));
    doc()->map()->settings()->setShowRowHeader(parameterGroup.readEntry("Row Header", true));
    if (!configFromDoc)
        doc()->map()->settings()->setCompletionMode((KGlobalSettings::Completion)parameterGroup.readEntry("Completion Mode", (int)(KGlobalSettings::CompletionAuto)));
    doc()->map()->settings()->setMoveToValue((KSpread::MoveTo)parameterGroup.readEntry("Move", (int)(Bottom)));
    doc()->map()->settings()->setIndentValue(parameterGroup.readEntry("Indent", 10.0));
    doc()->map()->settings()->setTypeOfCalc((MethodOfCalc)parameterGroup.readEntry("Method of Calc", (int)(SumOfNumber)));
    if (!configFromDoc)
        doc()->map()->settings()->setShowTabBar(parameterGroup.readEntry("Tabbar", true));

    doc()->map()->settings()->setShowStatusBar(parameterGroup.readEntry("Status bar", true));

    changeNbOfRecentFiles(parameterGroup.readEntry("NbRecentFile", 10));
    //autosave value is stored as a minute.
    //but default value is stored as seconde.
    doc()->setAutoSave(parameterGroup.readEntry("AutoSave", KoDocument::defaultAutoSave() / 60)*60);
    doc()->setBackupFile(parameterGroup.readEntry("BackupFile", true));

    const KConfigGroup colorGroup = config->group("KSpread Color");
    doc()->map()->settings()->setGridColor(colorGroup.readEntry("GridColor", QColor(Qt::lightGray)));
    doc()->map()->settings()->changePageBorderColor(colorGroup.readEntry("PageBorderColor", QColor(Qt::red)));

// Do we need a Page Layout in the congiguration file? Isn't this already in the template? Philipp
    /*
        const KConfigGroup pageLayoutGroup = config->group( "KSpread Page Layout" );
        if ( d->activeSheet->isEmpty())
        {
        d->activeSheet->setPaperFormat((KoPageFormat::Format)pageLayoutGroup.readEntry("Default size page",1));

        d->activeSheet->setPaperOrientation((KoPageFormat::Orientation)pageLayoutGroup.readEntry("Default orientation page",0));
        d->activeSheet->setPaperUnit((KoUnit)pageLayoutGroup.readEntry("Default unit page",0));
    }
    */
    doc()->map()->settings()->setCaptureAllArrowKeys(config->group("Editor").readEntry("CaptureAllArrowKeys", true));

    initCalcMenu();
    calcStatusBarOp();
}

void View::changeNbOfRecentFiles(int _nb)
{
    if (shell())
        shell()->setMaxRecentItems(_nb);
}

void View::initCalcMenu()
{
    switch (doc()->map()->settings()->getTypeOfCalc()) {
    case  SumOfNumber:
        d->actions->calcSum->setChecked(true);
        break;
    case  Min:
        d->actions->calcMin->setChecked(true);
        break;
    case  Max:
        d->actions->calcMax->setChecked(true);
        break;
    case  Average:
        d->actions->calcAverage->setChecked(true);
        break;
    case  Count:
        d->actions->calcCount->setChecked(true);
        break;
    case  CountA:
        d->actions->calcCountA->setChecked(true);
        break;
    case  NoneCalc:
        d->actions->calcNone->setChecked(true);
        break;
    default :
        d->actions->calcSum->setChecked(true);
        break;
    }

}


void View::recalcWorkBook()
{
    doc()->map()->recalcManager()->recalcMap();
}

void View::recalcWorkSheet()
{
    if (!activeSheet())
        return;
    doc()->map()->recalcManager()->recalcSheet(activeSheet());
}

void View::shapeSelectionChanged()
{
    const KoSelection* selection = d->canvas->shapeManager()->selection();
    const QList<KoShape*> shapes = selection->selectedShapes(KoFlake::StrippedSelection);

    if (shapes.isEmpty()) {
        d->actions->shapeAnchor->setEnabled(false);
        return;
    }
    d->actions->shapeAnchor->setEnabled(true);

    // start with the first shape
    const KoShape* shape = shapes[0];
    const ShapeApplicationData* data = dynamic_cast<ShapeApplicationData*>(shape->applicationData());
    if (!data) {
        // Container children do not have the application data set, deselect the anchoring action.
        d->actions->shapeAnchor->setCurrentAction(0);
        return;
    }
    bool anchoredToCell = data->isAnchoredToCell();
    d->actions->shapeAnchor->setCurrentAction(anchoredToCell ? i18n("Cell") : i18n("Page"));

    // go on with the remaining shapes
    for (int i = 1; i < shapes.count(); ++i) {
        shape = shapes[i];
        data = dynamic_cast<ShapeApplicationData*>(shape->applicationData());
        Q_ASSERT(data);
        if (anchoredToCell != data->isAnchoredToCell()) {
            // If the anchoring differs between shapes, deselect the anchoring action and stop here.
            d->actions->shapeAnchor->setCurrentAction(0);
            break;
        }
    }
}


void View::editDeleteSelection()
{
    d->canvas->toolProxy()->deleteSelection();
}

void View::initialPosition()
{
    // Loading completed, pick initial worksheet
    foreach(Sheet* sheet, doc()->map()->sheetList()) {
        addSheet(sheet);
    }

    // Set the initial X and Y offsets for the view (OpenDocument loading)
    const LoadingInfo* loadingInfo = doc()->map()->loadingInfo();
    if (loadingInfo->fileFormat() == LoadingInfo::OpenDocument) {
        d->savedAnchors = loadingInfo->cursorPositions();
        d->savedMarkers = loadingInfo->cursorPositions();
        d->savedOffsets = loadingInfo->scrollingOffsets();
    }

    Sheet* sheet = loadingInfo->initialActiveSheet();
    if (!sheet) {
        //activate first table which is not hiding
        sheet = doc()->map()->visibleSheets().isEmpty() ? 0 : doc()->map()->findSheet(doc()->map()->visibleSheets().first());
        if (!sheet) {
            sheet = doc()->map()->sheet(0);
            if (sheet) {
                sheet->setHidden(false);
                QString tabName = sheet->sheetName();
                d->tabBar->addTab(tabName);
            }
        }
    }
    setActiveSheet(sheet);

    // Set the initial X and Y offsets for the view (Native format loading)
    if (loadingInfo->fileFormat() == LoadingInfo::NativeFormat) {
        const QPoint offset = zoomHandler()->documentToView(loadingInfo->scrollingOffsets()[sheet]).toPoint();
        d->canvas->setDocumentOffset(offset);
        d->horzScrollBar->setValue(offset.x());
        d->vertScrollBar->setValue(offset.y());
        // Set the initial position for the marker as stored in the XML file,
        // (1,1) otherwise
        const QPoint marker = loadingInfo->cursorPositions()[sheet];
        d->selection->initialize((marker.x() <= 0 || marker.y() <= 0) ? QPoint(1, 1) : marker);
    }

    updateBorderButton();
    updateShowSheetMenu();

    if (koDocument()->isReadWrite())
        initConfig();

    d->canvas->setFocus();

    QTimer::singleShot(50, this, SLOT(finishLoading()));
}

void View::finishLoading()
{
    // finish the "View Loading" process
    d->loading = false;
    doc()->map()->deleteLoadingInfo();

    refreshView();

    // Activate the cell tool.
    if (shell())
        KoToolManager::instance()->switchToolRequested("KSpreadCellToolId");
}

void View::activateFormulaEditor()
{
}

void View::updateReadWrite(bool readwrite)
{
    // inform the cell tool
    emit documentReadWriteToggled(readwrite);

    const QList<QAction*> actions = actionCollection()->actions();
    for (int i = 0; i < actions.count(); ++i) {
        // The action collection contains also the flake tool actions. Skip them.
        if (actions[i]->parent() == this)
            actions[i]->setEnabled(readwrite);
    }

    if (!doc() || !doc()->map() || doc()->map()->isProtected()) {
        d->actions->showSheet->setEnabled(false);
        d->actions->hideSheet->setEnabled(false);
    } else {
        d->actions->showSheet->setEnabled(true);
        d->actions->hideSheet->setEnabled(true);
    }
    d->actions->showPageBorders->setEnabled(true);
}

void View::createTemplate()
{
    int width = 60;
    int height = 60;
    QPixmap pix = doc()->generatePreview(QSize(width, height));

    KTemporaryFile tempFile;
    tempFile.setSuffix(".kst");
    //Check that creation of temp file was successful
    if (!tempFile.open()) {
        qWarning("Creation of temporary file to store template failed.");
        return;
    }

    doc()->saveNativeFormat(tempFile.fileName());

    KoTemplateCreateDia::createTemplate("kspread_template", Factory::global(),
                                        tempFile.fileName(), pix, this);

    Factory::global().dirs()->addResourceType("kspread_template",
            "data", "kspread/templates/");
}

/*
void View::oszilloscope()
{
    QDialog* dlg = new OsziDlg( this );
    dlg->show();
}
*/

void View::setSelectionTextColor(const QColor &txtColor)
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet(d->activeSheet);
    manipulator->setText(i18n("Change Text Color"));
    manipulator->setFontColor(txtColor);
    manipulator->add(*selection());
    manipulator->execute();
}

void View::setSelectionBackgroundColor(const QColor &bgColor)
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet(d->activeSheet);
    manipulator->setText(i18n("Change Background Color"));
    manipulator->setBackgroundColor(bgColor);
    manipulator->add(*selection());
    manipulator->execute();
}

void View::setSelectionBorderColor(const QColor &bdColor)
{
    BorderColorCommand* command = new BorderColorCommand();
    command->setSheet(activeSheet());
    command->setColor(bdColor);
    command->add(*selection());
    command->execute();
}

void View::helpUsing()
{
    KToolInvocation::invokeHelp();
}

void View::setSelectionBottomBorderColor(const QColor & color)
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet(d->activeSheet);
    manipulator->setText(i18n("Change Border"));
    manipulator->setBottomBorderPen(QPen(color, 1, Qt::SolidLine));
    manipulator->add(*selection());
    manipulator->execute();
}

void View::setSelectionRightBorderColor(const QColor & color)
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet(d->activeSheet);
    manipulator->setText(i18n("Change Border"));
    if (d->activeSheet->layoutDirection() == Qt::RightToLeft)
        manipulator->setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    else
        manipulator->setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    manipulator->add(*selection());
    manipulator->execute();
}

void View::setSelectionLeftBorderColor(const QColor & color)
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet(d->activeSheet);
    manipulator->setText(i18n("Change Border"));
#if 0
    if (d->activeSheet->layoutDirection() == Qt::RightToLeft)
        manipulator->setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    else
        manipulator->setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
#else
    Q_UNUSED(color);
#endif
    manipulator->add(*selection());
    manipulator->execute();
}

void View::setSelectionTopBorderColor(const QColor & color)
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet(d->activeSheet);
    manipulator->setText(i18n("Change Border"));
    manipulator->setTopBorderPen(QPen(color, 1, Qt::SolidLine));
    manipulator->add(*selection());
    manipulator->execute();
}

void View::setSelectionOutlineBorderColor(const QColor & color)
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet(d->activeSheet);
    manipulator->setText(i18n("Change Border"));
    manipulator->setTopBorderPen(QPen(color, 1, Qt::SolidLine));
    manipulator->setBottomBorderPen(QPen(color, 1, Qt::SolidLine));
    manipulator->setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    manipulator->setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    manipulator->add(*selection());
    manipulator->execute();
}

void View::setSelectionAllBorderColor(const QColor & color)
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet(d->activeSheet);
    manipulator->setText(i18n("Change Border"));
    manipulator->setTopBorderPen(QPen(color, 1, Qt::SolidLine));
    manipulator->setBottomBorderPen(QPen(color, 1, Qt::SolidLine));
    manipulator->setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    manipulator->setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    manipulator->setHorizontalPen(QPen(color, 1, Qt::SolidLine));
    manipulator->setVerticalPen(QPen(color, 1, Qt::SolidLine));
    manipulator->add(*selection());
    manipulator->execute();
}

void View::addSheet(Sheet * _t)
{
    reviveSheet(_t);

    // Connect some signals
    connect(_t, SIGNAL(sig_refreshView()), SLOT(slotRefreshView()));
    connect(_t, SIGNAL(sig_updateView(Sheet*)), SLOT(slotUpdateView(Sheet*)));
    connect(_t->print(), SIGNAL(sig_updateView(Sheet*)), SLOT(slotUpdateView(Sheet*)));
    connect(_t, SIGNAL(sig_updateView(Sheet *, const Region&)),
            SLOT(slotUpdateView(Sheet*, const Region&)));
    connect(_t, SIGNAL(sig_updateColumnHeader(Sheet *)),
            SLOT(slotUpdateColumnHeader(Sheet *)));
    connect(_t, SIGNAL(sig_updateRowHeader(Sheet *)),
            SLOT(slotUpdateRowHeader(Sheet *)));
    connect(_t, SIGNAL(sig_nameChanged(Sheet*, const QString&)),
            this, SLOT(slotSheetRenamed(Sheet*, const QString&)));
    connect(_t, SIGNAL(sig_SheetHidden(Sheet*)),
            this, SLOT(slotSheetHidden(Sheet*)));
    connect(_t, SIGNAL(sig_SheetShown(Sheet*)),
            this, SLOT(slotSheetShown(Sheet*)));
    connect(_t, SIGNAL(sig_SheetRemoved(Sheet*)),
            this, SLOT(slotSheetRemoved(Sheet*)));

    if (!d->loading)
        updateBorderButton();
}

void View::slotSheetRemoved(Sheet* sheet)
{
    d->tabBar->removeTab(sheet->sheetName());
    if (doc()->map()->findSheet(doc()->map()->visibleSheets().first()))
        setActiveSheet(doc()->map()->findSheet(doc()->map()->visibleSheets().first()));
    else
        d->activeSheet = 0;
}

void View::removeAllSheets()
{
    d->tabBar->clear();
    setActiveSheet(0);
}

void View::setActiveSheet(Sheet* sheet, bool updateSheet)
{
    if (sheet == d->activeSheet)
        return;

    if (!d->selection->referenceSelectionMode()) {
        saveCurrentSheetSelection();
    }

    const Sheet* oldSheet = d->activeSheet;
    d->activeSheet = sheet;

    if (d->activeSheet == 0) {
        return;
    }

    // flake
    // Change the active shape controller and its shapes.
    d->canvas->shapeController()->setShapeControllerBase(d->activeSheet);
    d->canvas->shapeManager()->setShapes(d->activeSheet->shapes());
    // Tell the Canvas about the new visible sheet size.
    sheetView(d->activeSheet)->updateAccessedCellRange();

    if (oldSheet && oldSheet->layoutDirection() != d->activeSheet->layoutDirection())
        refreshView();

    if (updateSheet) {
        d->tabBar->setActiveTab(d->activeSheet->sheetName());
        d->rowHeader->repaint();
        d->columnHeader->repaint();
        d->selectAllButton->repaint();
    }

    if (d->selection->referenceSelectionMode()) {
        d->selection->setActiveSheet(d->activeSheet);
        return;
    }

    /* see if there was a previous selection on this other sheet */
    QMap<Sheet*, QPoint>::Iterator it = d->savedAnchors.find(d->activeSheet);
    QMap<Sheet*, QPoint>::Iterator it2 = d->savedMarkers.find(d->activeSheet);
    QMap<Sheet*, QPointF>::Iterator it3 = d->savedOffsets.find(d->activeSheet);

    // restore the old anchor and marker
    const QPoint newAnchor = (it == d->savedAnchors.end()) ? QPoint(1, 1) : *it;
    const QPoint newMarker = (it2 == d->savedMarkers.end()) ? QPoint(1, 1) : *it2;

    d->selection->clear();
    d->selection->setActiveSheet(d->activeSheet);
    d->selection->setOriginSheet(d->activeSheet);
    d->selection->initialize(QRect(newMarker, newAnchor));
    d->canvas->scrollToCell(newMarker);
    if (it3 != d->savedOffsets.end()) {
        const QPoint offset = zoomHandler()->documentToView(*it3).toPoint();
        d->canvas->setDocumentOffset(offset);
        d->horzScrollBar->setValue(offset.x());
        d->vertScrollBar->setValue(offset.y());
    }

    d->canvas->update();

    d->actions->showPageBorders->blockSignals(true);
    d->actions->showPageBorders->setChecked(d->activeSheet->isShowPageBorders());
    d->actions->showPageBorders->blockSignals(false);
    
    d->actions->protectSheet->blockSignals(true);
    d->actions->protectSheet->setChecked(d->activeSheet->isProtected());
    d->actions->protectSheet->blockSignals(false);

    d->actions->protectDoc->blockSignals(true);
    d->actions->protectDoc->setChecked(doc()->map()->isProtected());
    d->actions->protectDoc->blockSignals(false);

    d->adjustActions(!d->activeSheet->isProtected());
    d->adjustWorkbookActions(!doc()->map()->isProtected());

    calcStatusBarOp();
}

void View::slotSheetRenamed(Sheet* sheet, const QString& old_name)
{
    d->tabBar->renameTab(old_name, sheet->sheetName());
}

void View::slotSheetHidden(Sheet*)
{
    updateShowSheetMenu();
}

void View::slotSheetShown(Sheet*)
{
    d->tabBar->setTabs(doc()->map()->visibleSheets());
    updateShowSheetMenu();
}

void View::changeSheet(const QString& _name)
{
    if (activeSheet()->sheetName() == _name)
        return;

    Sheet *t = doc()->map()->findSheet(_name);
    if (!t) {
        kDebug() << "Unknown sheet" << _name;
        return;
    }
    if (!selection()->referenceSelectionMode())
        selection()->emitCloseEditor(true); // save changes
    setActiveSheet(t, false /* False: Endless loop because of setActiveTab() => do the visual area update manually*/);
    d->mapViewModel->setActiveSheet(t);

    //refresh toggle button
    updateBorderButton();

    //update visible area
    d->rowHeader->repaint();
    d->columnHeader->repaint();
    d->selectAllButton->repaint();
}

void View::moveSheet(unsigned sheet, unsigned target)
{
    if (doc()->map()->isProtected()) return;

    QStringList vs = doc()->map()->visibleSheets();

    if (target >= (uint) vs.count())
        doc()->map()->moveSheet(vs[ sheet ], vs[ vs.count()-1 ], false);
    else
        doc()->map()->moveSheet(vs[ sheet ], vs[ target ], true);

    d->tabBar->moveTab(sheet, target);
}

void View::sheetProperties()
{
    // sanity check, shouldn't happen
    if (doc()->map()->isProtected()) return;
    if (d->activeSheet->isProtected()) return;

    bool directionChanged = false;

    SheetPropertiesDialog* dlg = new SheetPropertiesDialog(this);
    dlg->setLayoutDirection(d->activeSheet->layoutDirection());
    dlg->setAutoCalculationEnabled(d->activeSheet->isAutoCalculationEnabled());
    dlg->setShowGrid(d->activeSheet->getShowGrid());
    dlg->setShowPageBorders(d->activeSheet->isShowPageBorders());
    dlg->setShowFormula(d->activeSheet->getShowFormula());
    dlg->setHideZero(d->activeSheet->getHideZero());
    dlg->setShowFormulaIndicator(d->activeSheet->getShowFormulaIndicator());
    dlg->setShowCommentIndicator(d->activeSheet->getShowCommentIndicator());
    dlg->setColumnAsNumber(d->activeSheet->getShowColumnNumber());
    dlg->setLcMode(d->activeSheet->getLcMode());
    dlg->setCapitalizeFirstLetter(d->activeSheet->getFirstLetterUpper());

    if (dlg->exec()) {
        SheetPropertiesCommand* command = new SheetPropertiesCommand(d->activeSheet);

        if (d->activeSheet->layoutDirection() != dlg->layoutDirection())
            directionChanged = true;

        command->setLayoutDirection(dlg->layoutDirection());
        command->setAutoCalculationEnabled(dlg->autoCalc());
        command->setShowGrid(dlg->showGrid());
        command->setShowPageBorders(dlg->showPageBorders());
        command->setShowFormula(dlg->showFormula());
        command->setHideZero(dlg->hideZero());
        command->setShowFormulaIndicator(dlg->showFormulaIndicator());
        command->setShowCommentIndicator(dlg->showCommentIndicator());
        command->setColumnAsNumber(dlg->columnAsNumber());
        command->setLcMode(dlg->lcMode());
        command->setCapitalizeFirstLetter(dlg->capitalizeFirstLetter());
        doc()->addCommand(command);
    }

    delete dlg;

    if (directionChanged) {
        // the scrollbar and hborder remain reversed otherwise
        d->horzScrollBar->setValue(d->horzScrollBar->maximum() -
                                   d->horzScrollBar->value());
        d->columnHeader->update();
    }
}

void View::insertSheet()
{
    if (doc()->map()->isProtected()) {
        KMessageBox::error(0, i18n("You cannot change a protected sheet."));
        return;
    }

    selection()->emitCloseEditor(true); // save changes
    Sheet * t = doc()->map()->createSheet();
    QUndoCommand* command = new AddSheetCommand(t);
    doc()->addCommand(command);
    setActiveSheet(t);

    if (doc()->map()->visibleSheets().count() > 1) {
        d->actions->deleteSheet->setEnabled(true);
        d->actions->hideSheet->setEnabled(true);
    }
}

void View::duplicateSheet()
{
    if (doc()->map()->isProtected()) {
        KMessageBox::error(this, i18n("You cannot change a protected sheet."));
        return;
    }

    DuplicateSheetCommand* command = new DuplicateSheetCommand();
    command->setSheet(activeSheet());
    doc()->addCommand(command);

    if (doc()->map()->visibleSheets().count() > 1) {
        d->actions->deleteSheet->setEnabled(true);
        d->actions->hideSheet->setEnabled(true);
    }
}

void View::hideSheet()
{
    if (!d->activeSheet)
        return;

    if (doc()->map()->visibleSheets().count() ==  1) {
        KMessageBox::error(this, i18n("You cannot hide the last visible sheet."));
        return;
    }

    QStringList vs = doc()->map()->visibleSheets();
    int i = vs.indexOf(d->activeSheet->sheetName()) - 1;
    if (i < 0) i = 1;
    QString sn = vs[i];

    QUndoCommand* command = new HideSheetCommand(activeSheet());
    doc()->addCommand(command);

    d->tabBar->removeTab(d->activeSheet->sheetName());
    d->tabBar->setActiveTab(sn);
}

void View::showSheet()
{
    if (!d->activeSheet)
        return;

    ShowDialog dlg(this, "Sheet show");
    dlg.exec();
}

void View::copyAsText()
{
    if (!d->activeSheet)
        return;
    d->activeSheet->copyAsText(selection());
}


void View::setShapeAnchoring(const QString& mode)
{
    const KoSelection* selection = d->canvas->shapeManager()->selection();
    const QList<KoShape*> shapes = selection->selectedShapes(KoFlake::StrippedSelection);
    for (int i = 0; i < shapes.count(); ++i) {
        const KoShape* shape = shapes[i];
        ShapeApplicationData* data = dynamic_cast<ShapeApplicationData*>(shape->applicationData());
        Q_ASSERT(data);
        data->setAnchoredToCell(mode == i18n("Cell"));
    }
}

void View::toggleProtectDoc(bool mode)
{
    if (!doc() || !doc()->map())
        return;

    if (mode) {
        KNewPasswordDialog dlg(this);
        dlg.setPrompt(i18n("Enter a password."));
        dlg.setWindowTitle(i18n("Protect Document"));
        if (dlg.exec() != KPasswordDialog::Accepted) {
            d->actions->protectDoc->blockSignals(true);
            d->actions->protectDoc->setChecked(false);
            d->actions->protectDoc->blockSignals(false);
            return;
        }

        QByteArray hash("");
        QString password = dlg.password();
        if (password.length() > 0)
            SHA1::getHash(password, hash);
        doc()->map()->setProtected(hash);
    } else {
        KPasswordDialog dlg(this);
        dlg.setPrompt(i18n("Enter the password."));
        dlg.setWindowTitle(i18n("Unprotect Document"));
        if (dlg.exec() != KPasswordDialog::Accepted) {
            d->actions->protectDoc->blockSignals(true);
            d->actions->protectDoc->setChecked(true);
            d->actions->protectDoc->blockSignals(false);
            return;
        }

        QByteArray hash("");
        QString password(dlg.password());
        if (password.length() > 0)
            SHA1::getHash(password, hash);
        if (!doc()->map()->checkPassword(hash)) {
            KMessageBox::error(0, i18n("Password is incorrect."));
            d->actions->protectDoc->blockSignals(true);
            d->actions->protectDoc->setChecked(true);
            d->actions->protectDoc->blockSignals(false);
            return;
        }

        doc()->map()->setProtected(QByteArray());
    }

    doc()->setModified(true);
    d->adjustWorkbookActions(!mode);
}

void View::toggleProtectSheet(bool mode)
{
    if (!d->activeSheet)
        return;

    if (mode) {
        KNewPasswordDialog dlg(this);
        dlg.setPrompt(i18n("Enter a password."));
        dlg.setWindowTitle(i18n("Protect Sheet"));
        if (dlg.exec() != KPasswordDialog::Accepted) {
            d->actions->protectSheet->blockSignals(true);
            d->actions->protectSheet->setChecked(false);
            d->actions->protectSheet->blockSignals(false);
            return;
        }

        QByteArray hash("");
        QString password = dlg.password();
        if (password.length() > 0)
            SHA1::getHash(password, hash);
        d->activeSheet->setProtected(hash);
    } else {
        KPasswordDialog dlg(this);
        dlg.setPrompt(i18n("Enter the password."));
        dlg.setWindowTitle(i18n("Unprotect Sheet"));
        if (dlg.exec() != KPasswordDialog::Accepted) {
            d->actions->protectSheet->blockSignals(true);
            d->actions->protectSheet->setChecked(true);
            d->actions->protectSheet->blockSignals(false);
            return;
        }

        QByteArray hash("");
        QString password(dlg.password());
        if (password.length() > 0)
            SHA1::getHash(password, hash);


        if (!d->activeSheet->checkPassword(hash)) {
            KMessageBox::error(0, i18n("Password is incorrect."));
            d->actions->protectSheet->blockSignals(true);
            d->actions->protectSheet->setChecked(true);
            d->actions->protectSheet->blockSignals(false);
            return;
        }

        d->activeSheet->setProtected(QByteArray());

    }

    doc()->setModified(true);
    d->adjustActions(!mode);
    // d->activeSheet->setRegionPaintDirty( QRect(QPoint( 0, 0 ), QPoint( KS_colMax, KS_rowMax ) ) );
    refreshView();
    // inform the cell tool
    emit sheetProtectionToggled(mode);
}

void View::togglePageBorders(bool mode)
{
    if (!d->activeSheet)
        return;

    d->activeSheet->setShowPageBorders(mode);
}

void View::viewZoom(KoZoomMode::Mode mode, qreal zoom)
{
    Q_UNUSED(zoom);
#ifdef NDEBUG
    Q_UNUSED(mode);
#endif
    Q_ASSERT(mode == KoZoomMode::ZOOM_CONSTANT);
    selection()->emitCloseEditor(true); // save changes
    doc()->refreshInterface();
}

void View::setZoom(int zoom, bool /*updateViews*/)
{
    kDebug(36005) << "---------SetZoom:" << zoom;

    // Set the zoom in KoView (for embedded views)
    zoomHandler()->setZoomAndResolution(zoom, KoDpi::dpiX(), KoDpi::dpiY());
    //KoView::setZoom( zoomHandler()->zoomedResolutionY() /* KoView only supports one zoom */ );

    doc()->refreshInterface();
}

void View::showColumnHeader(bool enable)
{
    doc()->map()->settings()->setShowColumnHeader(enable);
    refreshView();
}

void View::showRowHeader(bool enable)
{
    doc()->map()->settings()->setShowRowHeader(enable);
    refreshView();
}

void View::showHorizontalScrollBar(bool enable)
{
    doc()->map()->settings()->setShowHorizontalScrollBar(enable);
    refreshView();
}

void View::showVerticalScrollBar(bool enable)
{
    doc()->map()->settings()->setShowVerticalScrollBar(enable);
    refreshView();
}

void View::showStatusBar(bool b)
{
    doc()->map()->settings()->setShowStatusBar(b);
    refreshView();
}

void View::showTabBar(bool b)
{
    doc()->map()->settings()->setShowTabBar(b);
    refreshView();
}

void View::optionsNotifications()
{
    KNotifyConfigWidget::configure(this);
}

void View::preference()
{
    PreferenceDialog dialog(this);
    if (dialog.exec()) {
        d->canvas->update();
        d->columnHeader->update();
        d->rowHeader->update();
    }
}

void View::setSelectionComment(const QString& comment)
{
    CommentCommand* manipulator = new CommentCommand();
    manipulator->setSheet(d->activeSheet);
    manipulator->setText(i18n("Add Comment"));
    manipulator->setComment(comment.trimmed());
    manipulator->add(*selection());
    manipulator->execute();
}

bool View::showSheet(const QString& sheetName)
{
    Sheet *t = doc()->map()->findSheet(sheetName);
    if (!t) {
        kDebug(36001) << "Unknown sheet" << sheetName;
        return false;
    }
    selection()->emitCloseEditor(true); // save changes
    setActiveSheet(t);

    return true;
}

void View::nextSheet()
{
    Sheet * t = doc()->map()->nextSheet(activeSheet());
    if (!t) {
        kDebug(36001) << "Unknown sheet";
        return;
    }
    selection()->emitCloseEditor(true); // save changes
    setActiveSheet(t);
    d->tabBar->setActiveTab(t->sheetName());
    d->tabBar->ensureVisible(t->sheetName());
}

void View::previousSheet()
{
    Sheet * t = doc()->map()->previousSheet(activeSheet());
    if (!t) {
        kDebug(36001) << "Unknown sheet";
        return;
    }
    selection()->emitCloseEditor(true); // save changes
    setActiveSheet(t);
    d->tabBar->setActiveTab(t->sheetName());
    d->tabBar->ensureVisible(t->sheetName());
}

void View::firstSheet()
{
    Sheet *t = doc()->map()->sheet(0);
    if (!t) {
        kDebug(36001) << "Unknown sheet";
        return;
    }
    selection()->emitCloseEditor(true); // save changes
    setActiveSheet(t);
    d->tabBar->setActiveTab(t->sheetName());
    d->tabBar->ensureVisible(t->sheetName());
}

void View::lastSheet()
{
    Sheet *t = doc()->map()->sheet(doc()->map()->count() - 1);
    if (!t) {
        kDebug(36001) << "Unknown sheet";
        return;
    }
    selection()->emitCloseEditor(true); // save changes
    setActiveSheet(t);
    d->tabBar->setActiveTab(t->sheetName());
    d->tabBar->ensureVisible(t->sheetName());
}

void View::keyPressEvent(QKeyEvent *event)
{
#ifndef NDEBUG
    if ((event->modifiers() & Qt::ControlModifier) && (event->modifiers() & Qt::ShiftModifier)) {
        if (event->key() == Qt::Key_V) { // Ctrl+Shift+V to show debug (similar to KWord)
            d->activeSheet->printDebug();
        }
    }
#endif
    QWidget::keyPressEvent(event);
}

int View::leftBorder() const
{
    return d->rowHeader->width();
}

int View::rightBorder() const
{
    return d->vertScrollBar->width();
}

int View::topBorder() const
{
    return d->columnHeader->height();
}

int View::bottomBorder() const
{
    return d->horzScrollBar->height();
}

void View::refreshView()
{
    if (d->loading)   // "View Loading" not finished yet
        return;
    kDebug(36004) << "refreshing view";

    Sheet * sheet = activeSheet();
    if (!sheet)
        return;

    d->adjustActions(!sheet->isProtected());

    bool active = sheet->getShowFormula();
    Q_UNUSED(active);
    if (sheet && !sheet->isProtected()) {
        // FIXME Stefan: action move
//     d->actions->alignLeft->setEnabled( !active );
//     d->actions->alignCenter->setEnabled( !active );
//     d->actions->alignRight->setEnabled( !active );
    }

    d->tabBar->setReadOnly(!doc()->isReadWrite() || doc()->map()->isProtected());

    d->columnHeader->setVisible(doc()->map()->settings()->showColumnHeader());
    d->rowHeader->setVisible(doc()->map()->settings()->showRowHeader());
    d->selectAllButton->setVisible(doc()->map()->settings()->showColumnHeader() && doc()->map()->settings()->showRowHeader());
    d->vertScrollBar->setVisible(doc()->map()->settings()->showVerticalScrollBar());
    d->horzScrollBar->setVisible(doc()->map()->settings()->showHorizontalScrollBar());
    d->tabBar->setVisible(doc()->map()->settings()->showTabBar());
    if (statusBar()) statusBar()->setVisible(doc()->map()->settings()->showStatusBar());

    QFont font(KoGlobal::defaultFont());
    d->columnHeader->setMinimumHeight(qRound(zoomHandler()->zoomItY(font.pointSizeF() + 3)));
    d->rowHeader->setMinimumWidth(qRound(zoomHandler()->zoomItX(YBORDER_WIDTH)));
    d->selectAllButton->setMinimumHeight(qRound(zoomHandler()->zoomItY(font.pointSizeF() + 3)));
    d->selectAllButton->setMinimumWidth(qRound(zoomHandler()->zoomItX(YBORDER_WIDTH)));

    Qt::LayoutDirection sheetDir = sheet->layoutDirection();
    bool interfaceIsRTL = QApplication::isRightToLeft();

//   kDebug(36004)<<" sheetDir == Qt::LeftToRight :"<<( sheetDir == Qt::LeftToRight );
    if ((sheetDir == Qt::LeftToRight && !interfaceIsRTL) ||
            (sheetDir == Qt::RightToLeft && interfaceIsRTL)) {
        d->viewLayout->setOriginCorner(Qt::TopLeftCorner);
        d->tabBar->setReverseLayout(interfaceIsRTL);
    } else {
        d->viewLayout->setOriginCorner(Qt::TopRightCorner);
        d->tabBar->setReverseLayout(!interfaceIsRTL);
    }
}

void View::resizeEvent(QResizeEvent *)
{
}

void View::paperLayoutDlg()
{
    selection()->emitCloseEditor(true); // save changes
    SheetPrint* print = d->activeSheet->print();

    KoPageLayout pl = print->settings()->pageLayout();

    HeadFoot hf;
    hf.headLeft  = print->localizeHeadFootLine(print->headLeft());
    hf.headRight = print->localizeHeadFootLine(print->headRight());
    hf.headMid   = print->localizeHeadFootLine(print->headMid());
    hf.footLeft  = print->localizeHeadFootLine(print->footLeft());
    hf.footRight = print->localizeHeadFootLine(print->footRight());
    hf.footMid   = print->localizeHeadFootLine(print->footMid());

    KoUnit unit = doc()->unit();

    PageLayoutDialog dialog(this, d->activeSheet);
    dialog.exec();
//     dialog.show(); // Use show() to make selecting columns/rows possible.
}

void View::resetPrintRange()
{
    DefinePrintRangeCommand* command = new DefinePrintRangeCommand();
    command->setText(i18n("Reset Print Range"));
    command->setSheet(activeSheet());
    command->add(Region(QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax)), activeSheet()));
    doc()->addCommand(command);
}

/*
  // TODO Use KoView setScaling/xScaling/yScaling instead
void View::zoomMinus()
{
  if ( m_fZoom <= 0.25 )
    return;

  m_fZoom -= 0.25;

  if ( d->activeSheet != 0 )
    d->activeSheet->setLayoutDirtyFlag();

  d->canvas->repaint();
  d->rowHeader->repaint();
  d->columnHeader->repaint();
}

void View::zoomPlus()
{
  if ( m_fZoom >= 3 )
    return;

  m_fZoom += 0.25;

  if ( d->activeSheet != 0 )
    d->activeSheet->setLayoutDirtyFlag();

  d->canvas->repaint();
  d->rowHeader->repaint();
  d->columnHeader->repaint();
}
*/

void View::deleteSheet()
{
    if (doc()->map()->count() <= 1 || (doc()->map()->visibleSheets().count() <= 1)) {
        KMessageBox::sorry(this, i18n("You cannot delete the only sheet."), i18n("Remove Sheet"));
        return;
    }
    int ret = KMessageBox::warningContinueCancel(this, i18n("You are about to remove the active sheet.\nDo you want to continue?"),
              i18n("Remove Sheet"), KGuiItem(i18n("&Delete"), "edit-delete"));

    if (ret == KMessageBox::Continue) {
        selection()->emitCloseEditor(false); // discard changes
        doc()->setModified(true);
        Sheet * tbl = activeSheet();
        QUndoCommand* command = new RemoveSheetCommand(tbl);
        doc()->addCommand(command);


#if 0
        UndoRemoveSheet * undo = new UndoRemoveSheet(doc(), tbl);
        doc()->addCommand(undo);
        tbl->map()->removeSheet(tbl);
#endif
    }
}


void View::slotRename()
{

    Sheet * sheet = activeSheet();

    if (sheet->isProtected()) {
        KMessageBox::error(0, i18n("You cannot change a protected sheet."));
        return;
    }

    bool ok;
    QString activeName = sheet->sheetName();
    QString newName = KInputDialog::getText(i18n("Rename Sheet"), i18n("Enter name:"), activeName, &ok, this);

    if (!ok) return;

    if ((newName.trimmed()).isEmpty()) { // Sheet name is empty.
        KMessageBox::information(this, i18n("Sheet name cannot be empty."), i18n("Change Sheet Name"));
        // Recursion
        slotRename();
    } else if (newName != activeName) { // Sheet name changed.
        // Is the name already used
        if (doc()->map()->findSheet(newName)) {
            KMessageBox::information(this, i18n("This name is already used."), i18n("Change Sheet Name"));
            // Recursion
            slotRename();
            return;
        }

        QUndoCommand* command = new RenameSheetCommand(sheet, newName);
        doc()->addCommand(command);

        //sheet->setSheetName( newName );

        doc()->setModified(true);
    }
}

//------------------------------------------------
//
// Document signals
//
//------------------------------------------------

void View::slotAddSheet(Sheet *_sheet)
{
    addSheet(_sheet);
}

void View::slotRefreshView()
{
    refreshView();
    d->canvas->repaint();
    d->rowHeader->repaint();
    d->columnHeader->repaint();
    d->selectAllButton->repaint();
}

void View::slotUpdateView(Sheet *_sheet)
{
    // Do we display this sheet ?
    if ((!activeSheet()) || (_sheet != d->activeSheet))
        return;

    d->activeSheet->setRegionPaintDirty(d->canvas->visibleCells());
}

void View::slotUpdateView(Sheet * _sheet, const Region& region)
{
    // qDebug("void View::slotUpdateView( Sheet *_sheet, const QRect& %i %i|%i %i )\n",_rect.left(),_rect.top(),_rect.right(),_rect.bottom());

    // Do we display this sheet ?
    if (_sheet != d->activeSheet)
        return;

    d->activeSheet->setRegionPaintDirty(region);
}

void View::slotUpdateColumnHeader(Sheet * _sheet)
{
    // kDebug(36001)<<"void View::slotUpdateColumnHeader( Sheet *_sheet )";

    // Do we display this sheet ?
    if (_sheet != d->activeSheet)
        return;

    d->columnHeader->update();
}

void View::slotUpdateRowHeader(Sheet *_sheet)
{
    // kDebug("void View::slotUpdateRowHeader( Sheet *_sheet )";

    // Do we display this sheet ?
    if (_sheet != d->activeSheet)
        return;

    d->rowHeader->update();
}

void View::slotChangeSelection(const KSpread::Region& changedRegion)
{
    if (!changedRegion.isValid())
        return;

    if (d->selection->referenceSelectionMode()) {
        doc()->map()->addDamage(new SelectionDamage(changedRegion));
        kDebug(36002) << "Choice:" << *selection();
        return;
    }

    // delayed recalculation of the operation shown in the status bar
    d->statusBarOpTimer.setSingleShot(true);
    d->statusBarOpTimer.start(250);

    if (!d->loading)
        doc()->map()->addDamage(new SelectionDamage(changedRegion));
    d->rowHeader->update();
    d->columnHeader->update();
    d->selectAllButton->update();

    if (d->selection->isColumnSelected() || d->selection->isRowSelected()) {
        return;
    }

    d->canvas->validateSelection();

    d->canvas->scrollToCell(selection()->marker());
}

void View::slotScrollChoice(const KSpread::Region& changedRegion)
{
    if (!selection()->referenceSelectionMode() || !changedRegion.isValid()) {
        return;
    }
    d->canvas->scrollToCell(selection()->marker());
}

void View::calcStatusBarOp()
{
    Sheet * sheet = activeSheet();
    ValueCalc* calc = doc()->map()->calc();
    Value val;
    MethodOfCalc tmpMethod = doc()->map()->settings()->getTypeOfCalc();
    if (sheet && tmpMethod != NoneCalc) {
        Value range = sheet->cellStorage()->valueRegion(*d->selection);
        switch (tmpMethod) {
        case SumOfNumber:
            val = calc->sum(range);
            break;
        case Average:
            val = calc->avg(range);
            break;
        case Min:
            val = calc->min(range);
            break;
        case Max:
            val = calc->max(range);
            break;
        case CountA:
            val = Value(calc->count(range));
            break;
        case Count:
            val = Value(calc->count(range, false));
        case NoneCalc:
            break;
        default:
            break;
        }

    }

    QString res = doc()->map()->converter()->asString(val).asString();
    QString tmp;
    switch (tmpMethod) {
    case SumOfNumber:
        tmp = i18n("Sum: ") + res;
        break;
    case Average:
        tmp = i18n("Average: ") + res;
        break;
    case Min:
        tmp = i18n("Min: ") + res;
        break;
    case Max:
        tmp = i18n("Max: ") + res;
        break;
    case Count:
        tmp = i18n("Count: ") + res;
        break;
    case CountA:
        tmp = i18n("CountA: ") + res;
        break;
    case NoneCalc:
        tmp = "";
        break;
    }

    if (d->calcLabel)
        d->calcLabel->setText(QString(' ') + tmp + ' ');
}

void View::statusBarClicked(const QPoint&)
{
    QPoint mousepos = QCursor::pos();
    if (koDocument()->isReadWrite() && factory())
        if (QMenu* menu = dynamic_cast<QMenu*>(factory()->container("calc_popup" , this)))
            menu->popup(mousepos);
}

void View::menuCalc(bool)
{
    if (d->actions->calcMin->isChecked()) {
        doc()->map()->settings()->setTypeOfCalc(Min);
    } else if (d->actions->calcMax->isChecked()) {
        doc()->map()->settings()->setTypeOfCalc(Max);
    } else if (d->actions->calcCount->isChecked()) {
        doc()->map()->settings()->setTypeOfCalc(Count);
    } else if (d->actions->calcAverage->isChecked()) {
        doc()->map()->settings()->setTypeOfCalc(Average);
    } else if (d->actions->calcSum->isChecked()) {
        doc()->map()->settings()->setTypeOfCalc(SumOfNumber);
    } else if (d->actions->calcCountA->isChecked()) {
        doc()->map()->settings()->setTypeOfCalc(CountA);
    } else if (d->actions->calcNone->isChecked())
        doc()->map()->settings()->setTypeOfCalc(NoneCalc);

    calcStatusBarOp();
}


QMatrix View::matrix() const
{
    QMatrix m;
    m.scale(zoomHandler()->zoomedResolutionX(),
            zoomHandler()->zoomedResolutionY());
    m.translate(- d->canvas->xOffset(), - d->canvas->yOffset());
    return m;
}

QWidget* View::canvas() const
{
    return d->canvas;
}

void View::guiActivateEvent(KParts::GUIActivateEvent *ev)
{
    if (d->activeSheet) {
        if (ev->activated()) {
            if (d->calcLabel)
                calcStatusBarOp();
        } else {
            /*if (d->calcLabel)
              {
              disconnect(d->calcLabel,SIGNAL(pressed( int )),this,SLOT(statusBarClicked(int)));
              }*/
        }
    }

    KoView::guiActivateEvent(ev);
}

void View::popupTabBarMenu(const QPoint & _point)
{
    if (!koDocument()->isReadWrite() || !factory())
        return;
    if (d->tabBar) {
        QMenu* const menu = static_cast<QMenu*>(factory()->container("menupage_popup", this));

        QAction* insertSheet = new KAction(KIcon("insert-table"), i18n("Insert Sheet"), this);
        insertSheet->setToolTip(i18n("Remove the active sheet"));
        connect(insertSheet, SIGNAL(triggered(bool)), this, SLOT(insertSheet()));
        menu->insertAction(d->actions->duplicateSheet, insertSheet);

        QAction* deleteSheet = new KAction(KIcon("delete_table"), i18n("Remove Sheet"), this);
        deleteSheet->setToolTip(i18n("Remove the active sheet"));
        connect(deleteSheet, SIGNAL(triggered(bool)), this, SLOT(deleteSheet()));
        menu->insertAction(d->actions->hideSheet, deleteSheet);

        bool state = (doc()->map()->visibleSheets().count() > 1);
        if (d->activeSheet && d->activeSheet->isProtected()) {
            deleteSheet->setEnabled(false);
            d->actions->hideSheet->setEnabled(false);
            d->actions->showSheet->setEnabled(false);
        } else {
            deleteSheet->setEnabled(state);
            d->actions->hideSheet->setEnabled(state);
            d->actions->showSheet->setEnabled(doc()->map()->hiddenSheets().count() > 0);
        }
        if (!doc() || !doc()->map() || doc()->map()->isProtected()) {
            insertSheet->setEnabled(false);
            deleteSheet->setEnabled(false);
            d->actions->renameSheet->setEnabled(false);
            d->actions->showSheet->setEnabled(false);
            d->actions->hideSheet->setEnabled(false);
        }
        menu->exec(_point);
        menu->removeAction(insertSheet);
        menu->removeAction(deleteSheet);
    }
}

void View::updateBorderButton()
{
    if (d->activeSheet)
        d->actions->showPageBorders->setChecked(d->activeSheet->isShowPageBorders());
}

void View::removeSheet(Sheet *_t)
{
    QString m_tablName = _t->sheetName();
    d->tabBar->removeTab(m_tablName);
    setActiveSheet(doc()->map()->findSheet(doc()->map()->visibleSheets().first()));

    bool state = doc()->map()->visibleSheets().count() > 1;
    d->actions->deleteSheet->setEnabled(state);
    d->actions->hideSheet->setEnabled(state);
}

void View::reviveSheet(Sheet* sheet)
{
    QString tabName = sheet->sheetName();
    if (!sheet->isHidden()) {
        d->tabBar->addTab(tabName);
    }

    bool state = (doc()->map()->visibleSheets().count() > 1);
    d->actions->deleteSheet->setEnabled(state);
    d->actions->hideSheet->setEnabled(state);
}

QColor View::borderColor() const
{
    return d->canvas->resourceManager()->foregroundColor().toQColor();
}

void View::updateShowSheetMenu()
{
    if (d->activeSheet) {
        if (d->activeSheet->isProtected())
            d->actions->showSheet->setEnabled(false);
        else
            d->actions->showSheet->setEnabled(doc()->map()->hiddenSheets().count() > 0);
    }
}

void View::markSelectionAsDirty()
{
    doc()->map()->addDamage(new SelectionDamage(*selection()));
}

void View::paintUpdates()
{
    /* don't do any begin/end operation here -- this is what is called at an
       endOperation
    */
    d->canvas->update();
}

void View::initialiseMarkerFromSheet(Sheet* sheet, const QPoint& point)
{
    d->savedMarkers.remove(sheet);
    d->savedMarkers.insert(sheet, point);
}

QPoint View::markerFromSheet(Sheet* sheet) const
{
    QMap<Sheet*, QPoint>::Iterator it = d->savedMarkers.find(sheet);
    QPoint newMarker = (it == d->savedMarkers.end()) ? QPoint(1, 1) : *it;
    return newMarker;
}

QPointF View::offsetFromSheet(Sheet* sheet) const
{
    QMap<Sheet*, QPointF>::Iterator it = d->savedOffsets.find(sheet);
    QPointF offset = (it == d->savedOffsets.end()) ? QPointF() : *it;
    return offset;
}

void View::saveCurrentSheetSelection()
{
    /* save the current selection on this sheet */
    if (d->activeSheet != 0) {
        d->savedAnchors.remove(d->activeSheet);
        d->savedAnchors.insert(d->activeSheet, d->selection->anchor());
        kDebug(36005) << " Current scrollbar vert value:" << d->vertScrollBar->value();
        kDebug(36005) << "Saving marker pos:" << d->selection->marker();
        d->savedMarkers.remove(d->activeSheet);
        d->savedMarkers.insert(d->activeSheet, d->selection->marker());
        d->savedOffsets.remove(d->activeSheet);
        d->savedOffsets.insert(d->activeSheet, QPointF(d->canvas->xOffset(),
                               d->canvas->yOffset()));
    }
}

void View::handleDamages(const QList<Damage*>& damages)
{
    bool refreshView = false;
    QRegion paintRegion;
    bool paintClipped = true;

    QList<Damage*>::ConstIterator end(damages.end());
    for (QList<Damage*>::ConstIterator it = damages.begin(); it != end; ++it) {
        Damage* damage = *it;
        if (!damage) continue;

        if (damage->type() == Damage::Cell) {
            CellDamage* cellDamage = static_cast<CellDamage*>(damage);
            kDebug(36007) << "Processing\t" << *cellDamage;
            Sheet* const damagedSheet = cellDamage->sheet();
            const Region region = cellDamage->region();

            if (cellDamage->changes() & CellDamage::Appearance) {
                sheetView(damagedSheet)->invalidateRegion(region);
                paintClipped = false;
            }
            continue;
        }

        if (damage->type() == Damage::Sheet) {
            SheetDamage* sheetDamage = static_cast<SheetDamage*>(damage);
            kDebug(36007) << "Processing\t" << *sheetDamage;

            if (sheetDamage->changes() & SheetDamage::PropertiesChanged) {
                d->activeSheet->setRegionPaintDirty(d->canvas->visibleCells());
                refreshView = true;
            }
            if (sheetDamage->changes() & SheetDamage::ColumnsChanged)
                columnHeader()->update();
            if (sheetDamage->changes() & SheetDamage::RowsChanged)
                rowHeader()->update();
            continue;
        }

        if (damage->type() == Damage::Selection) {
            SelectionDamage* selectionDamage = static_cast<SelectionDamage*>(damage);
            kDebug(36007) << "Processing\t" << *selectionDamage;
            const Region region = selectionDamage->region();

            if (paintClipped) {
                const QRectF rect = canvasWidget()->cellCoordinatesToView(region.boundingRect());
                paintRegion += rect.toRect().adjusted(-3, -3, 4, 4);
            }
            continue;
        }

        kDebug(36007) << "Unhandled\t" << *damage;
    }

    // At last repaint the dirty cells.
    if (paintClipped)
        canvas()->update(paintRegion);
    else
        canvas()->update();
    if (refreshView)
        this->refreshView();
}

KoPrintJob * View::createPrintJob()
{
    if (!activeSheet())
        return 0;
    // About to print; close the editor.
    selection()->emitCloseEditor(true); // save changes
    return new PrintJob(this);
}

} // namespace KSpread

#include "View.moc"
