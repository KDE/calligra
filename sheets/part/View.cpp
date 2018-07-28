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
   Copyright 2010 Boudewijn Rempt <boud@kogmbh.com>

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
#include <QToolButton>
#ifndef QT_NO_SQL
#include <QSqlDatabase>
#endif
#include <QSizePolicy>
#include <QScrollBar>
#include <QStatusBar>
#include <QInputDialog>
#include <QTimer>

// KF5 includes
#include <kactioncollection.h>
#include <kconfig.h>

#include <kmessagebox.h>
#include <kstandardaction.h>
#include <ktoggleaction.h>
#include <kxmlguifactory.h>
#include <kstandardguiitem.h>

// Calligra includes
#include <KoComponentData.h>
#include <KoPluginLoader.h>
#include <KoGlobal.h>
#include <KoColor.h>
#include <KoCanvasControllerWidget.h>
#include <KoMainWindow.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoDockerManager.h>
#include <KoToolManager.h>
#include <KoTemplateCreateDia.h>
#include <KoXmlNS.h>
#include <KoZoomAction.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>
#include <KoToolProxy.h>
#include <KoModeBoxFactory.h>
#include <KoIcon.h>
#include <KoCanvasResourceManager.h>
#include <KoCanvasResourceIdentities.h>

// Sheets includes
#include "SheetsDebug.h"
#include "ApplicationSettings.h"
#include "BindingManager.h"
#include "CalculationSettings.h"
#include "CellStorage.h"
#include "Damages.h"
#include "DependencyManager.h"
#include "Doc.h"
#include "Factory.h"
#include "HeaderFooter.h"
#include "LoadingInfo.h"
#include "Canvas.h"
#include "Global.h"
#include "Headers.h"
#include "HeaderWidgets.h"
#include "Localization.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "PrintSettings.h"
#include "RecalcManager.h"
#include "RowColumnFormat.h"
#include "ShapeApplicationData.h"
#include "Sheet.h"
#include "SheetPrint.h"
#include "Style.h"
#include "StyleManager.h"
#include "StyleStorage.h"
#include "ToolRegistry.h"
#include "Util.h"
#include "ValueCalc.h"
#include "ValueConverter.h"
#include "PrintJob.h"
#include "ElapsedTime_p.h"

// commands
#include "commands/CopyCommand.h"
#include "commands/DefinePrintRangeCommand.h"
#include "commands/SheetCommands.h"

// dialogs
#include "dialogs/PageLayoutDialog.h"
#include "dialogs/PreferenceDialog.h"
#include "dialogs/ShowDialog.h"

#include "dialogs/SheetPropertiesDialog.h"

// ui
#include "ui/CellView.h"
#include "ui/MapViewModel.h"
#include "ui/RightToLeftPaintingStrategy.h"
#include "ui/Selection.h"
#include "ui/SheetView.h"
#include "ui/PixmapCachingSheetView.h"

// D-Bus
#ifndef QT_NO_DBUS
#include "interfaces/ViewAdaptor.h"
#include <knotifyconfigwidget.h>
#endif

using namespace Calligra::Sheets;

class ViewActions;

class Q_DECL_HIDDEN View::Private
{
public:
    View* view;
    Doc* doc;

    // the active sheet, may be 0
    // this is the sheet which has the input focus
    Sheet* activeSheet;
    MapViewModel* mapViewModel;
    QHash<const Sheet*, QPointer<SheetView> > sheetViews;

    // GUI elements
    QWidget *frame;
    Canvas *canvas;
    KoCanvasController* canvasController;
    KoZoomController* zoomController;
    KoZoomHandler* zoomHandler;
    RowHeaderWidget *rowHeader;
    ColumnHeaderWidget *columnHeader;
    SelectAllButtonWidget* selectAllButton;
    QScrollBar *horzScrollBar;
    QScrollBar *vertScrollBar;
    TabBar *tabBar;
    QLabel* calcLabel;
    QGridLayout* viewLayout;
    QGridLayout* tabScrollBarLayout;

    // all UI actions
    ViewActions* actions;

    // if true, Calligra Sheets is still loading the document
    // don't try to refresh the view
    bool loading;

    // selection/marker
    Selection* selection;
    QMap<Sheet*, QPoint> savedAnchors;
    QMap<Sheet*, QPoint> savedMarkers;
    QMap<Sheet*, QPointF> savedOffsets;

    void initActions();
    void adjustActions(bool mode);

    // On timeout this will execute the status bar operation (e.g. SUM).
    // This is delayed to speed up the selection.
    QTimer statusBarOpTimer;

    QTimer *scrollTimer;
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
    QAction * deleteShape;

    // page layout
    QAction * paperLayout;
    QAction * resetPrintRange;
    KToggleAction* showPageOutline;

    // recalculation
    QAction * recalcWorksheet;
    QAction * recalcWorkbook;

    // protection
    KToggleAction* protectSheet;
    KToggleAction* protectDoc;

    // navigation
    QAction * nextSheet;
    QAction * prevSheet;
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

    // -- sheet/workbook actions --
    actions->sheetProperties  = new QAction(i18n("Sheet Properties..."), view);
    ac->addAction("sheetProperties", actions->sheetProperties);
    connect(actions->sheetProperties, SIGNAL(triggered(bool)), view, SLOT(sheetProperties()));
    actions->sheetProperties->setToolTip(i18n("Modify current sheet's properties"));

    actions->insertSheet = new QAction(koIcon("insert-table"), i18n("Sheet"), view);
    actions->insertSheet->setIconText(i18n("Insert Sheet"));
    actions->insertSheet->setToolTip(i18n("Insert a new sheet"));
    ac->addAction("insertSheet", actions->insertSheet);
    connect(actions->insertSheet, SIGNAL(triggered(bool)), view, SLOT(insertSheet()));

    actions->duplicateSheet = new QAction(/*koIcon("inserttable"),*/ i18n("Duplicate Sheet"), view);
    actions->duplicateSheet->setToolTip(i18n("Duplicate the selected sheet"));
    ac->addAction("duplicateSheet", actions->duplicateSheet);
    connect(actions->duplicateSheet, SIGNAL(triggered(bool)), view, SLOT(duplicateSheet()));

    actions->deleteSheet = new QAction(koIcon("edit-delete"), i18n("Sheet"), view);
    actions->deleteSheet->setIconText(i18n("Remove Sheet"));
    actions->deleteSheet->setToolTip(i18n("Remove the active sheet"));
    ac->addAction("deleteSheet", actions->deleteSheet);
    connect(actions->deleteSheet, SIGNAL(triggered(bool)), view, SLOT(deleteSheet()));

    actions->renameSheet  = new QAction(i18n("Rename Sheet..."), view);
    ac->addAction("renameSheet", actions->renameSheet);
    connect(actions->renameSheet, SIGNAL(triggered(bool)), view, SLOT(slotRename()));
    actions->renameSheet->setToolTip(i18n("Rename the active sheet"));

    actions->showSheet  = new QAction(i18n("Show Sheet..."), view);
    ac->addAction("showSheet", actions->showSheet);
    connect(actions->showSheet, SIGNAL(triggered(bool)), view, SLOT(showSheet()));
    actions->showSheet->setToolTip(i18n("Show a hidden sheet"));

    actions->hideSheet  = new QAction(i18n("Hide Sheet"), view);
    ac->addAction("hideSheet", actions->hideSheet);
    connect(actions->hideSheet, SIGNAL(triggered(bool)), view, SLOT(hideSheet()));
    actions->hideSheet->setToolTip(i18n("Hide the active sheet"));

    actions->paperLayout  = new QAction(i18n("Page Layout..."), view);
    ac->addAction("paperLayout", actions->paperLayout);
    connect(actions->paperLayout, SIGNAL(triggered(bool)), view, SLOT(paperLayoutDlg()));
    actions->paperLayout->setToolTip(i18n("Specify the layout of the spreadsheet for a printout"));

    actions->resetPrintRange  = new QAction(i18n("Reset Print Range"), view);
    ac->addAction("resetPrintRange", actions->resetPrintRange);
    connect(actions->resetPrintRange, SIGNAL(triggered(bool)), view, SLOT(resetPrintRange()));
    actions->resetPrintRange->setToolTip(i18n("Reset the print range in the current sheet"));

    actions->showPageOutline = new KToggleAction(i18n("Page Outline"), view);
    actions->showPageOutline->setToolTip(i18n("Show on the spreadsheet where the page boundary will be"));
    ac->addAction("showPageOutline", actions->showPageOutline);
    connect(actions->showPageOutline, SIGNAL(toggled(bool)), view, SLOT(togglePageOutline(bool)));

    actions->recalcWorksheet  = new QAction(i18n("Recalculate Sheet"), view);
    actions->recalcWorksheet->setIcon(koIcon("view-refresh"));
    actions->recalcWorksheet->setIconText(i18n("Recalculate"));
    ac->addAction("RecalcWorkSheet", actions->recalcWorksheet);
    actions->recalcWorksheet->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F9));
    connect(actions->recalcWorksheet, SIGNAL(triggered(bool)), view, SLOT(recalcWorkSheet()));
    actions->recalcWorksheet->setToolTip(i18n("Recalculate the value of every cell in the current worksheet"));

    actions->recalcWorkbook  = new QAction(i18n("Recalculate Document"), view);
    actions->recalcWorkbook->setIcon(koIcon("view-refresh"));
    actions->recalcWorkbook->setIconText(i18n("Recalculate"));
    ac->addAction("RecalcWorkBook", actions->recalcWorkbook);
    actions->recalcWorkbook->setShortcut(QKeySequence(Qt::Key_F9));
    connect(actions->recalcWorkbook, SIGNAL(triggered(bool)), view, SLOT(recalcWorkBook()));
    actions->recalcWorkbook->setToolTip(i18n("Recalculate the value of every cell in all worksheets"));

    actions->protectSheet  = new KToggleAction(i18n("Protect &Sheet..."), view);
    ac->addAction("protectSheet", actions->protectSheet);
    actions->protectSheet->setToolTip(i18n("Protect the sheet from being modified"));
    connect(actions->protectSheet, SIGNAL(triggered(bool)),
            view, SLOT(toggleProtectSheet(bool)));

    actions->protectDoc  = new KToggleAction(i18n("Protect &Document..."), view);
    ac->addAction("protectDoc", actions->protectDoc);
    actions->protectDoc->setToolTip(i18n("Protect the document from being modified"));
    connect(actions->protectDoc, SIGNAL(triggered(bool)), view, SLOT(toggleProtectDoc(bool)));

    // -- misc actions --

    actions->createTemplate  = new QAction(i18n("&Create Template From Document..."), view);
    ac->addAction("createTemplate", actions->createTemplate);
    connect(actions->createTemplate, SIGNAL(triggered(bool)), view, SLOT(createTemplate()));

    actions->shapeAnchor = new KSelectAction(i18n("Anchor"), view);
    actions->shapeAnchor->addAction(i18n("Cell"));
    actions->shapeAnchor->addAction(i18n("Page"));
    actions->shapeAnchor->setEnabled(false);
    actions->shapeAnchor->setToolTip(i18n("Switch shape anchoring"));
    ac->addAction("shapeAnchor", actions->shapeAnchor);
    connect(actions->shapeAnchor, SIGNAL(triggered(QString)),
            view, SLOT(setShapeAnchoring(QString)));

    // -- navigation actions --

    actions->nextSheet  = new QAction(koIcon("go-next"), i18n("Next Sheet"), view);
    actions->nextSheet->setIconText(i18n("Next"));
    actions->nextSheet->setToolTip(i18n("Move to the next sheet"));
    ac->addAction("go_next", actions->nextSheet);
    actions->nextSheet->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageDown));
    connect(actions->nextSheet, SIGNAL(triggered(bool)), view, SLOT(nextSheet()));

    actions->prevSheet  = new QAction(koIcon("go-previous"), i18n("Previous Sheet"), view);
    actions->prevSheet->setIconText(i18n("Previous"));
    actions->prevSheet->setToolTip(i18n("Move to the previous sheet"));
    ac->addAction("go_previous", actions->prevSheet);
    actions->prevSheet->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageUp));
    connect(actions->prevSheet, SIGNAL(triggered(bool)), view, SLOT(previousSheet()));

    actions->firstSheet  = new QAction(koIcon("go-first"), i18n("First Sheet"), view);
    actions->firstSheet->setIconText(i18n("First"));
    actions->firstSheet->setToolTip(i18n("Move to the first sheet"));
    ac->addAction("go_first", actions->firstSheet);
    connect(actions->firstSheet, SIGNAL(triggered(bool)), view, SLOT(firstSheet()));

    actions->lastSheet  = new QAction(koIcon("go-last"), i18n("Last Sheet"), view);
    actions->lastSheet->setIconText(i18nc("Move to the last sheet", "Last"));
    actions->lastSheet->setToolTip(i18n("Move to the last sheet"));
    ac->addAction("go_last", actions->lastSheet);
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
    actions->preference->setToolTip(i18n("Set various Calligra Sheets options"));
    ac->addAction("preference", actions->preference);

    QAction *notifyAction = KStandardAction::configureNotifications(view, SLOT(optionsNotifications()), view);
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
    actions->deleteShape = new QAction(koIcon("edit-delete"), i18n("Delete"), view);
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


/*****************************************************************************
 *
 * View
 *
 *****************************************************************************/

View::View(KoPart *part, QWidget *_parent, Doc *_doc)
        : KoView(part, _doc, _parent)
        , d(new Private)
{
    ElapsedTime et("View constructor");

    d->view = this;
    d->doc = _doc;

    d->activeSheet = 0;

    d->loading = true;

    setComponentName(Factory::global().componentName(), Factory::global().componentDisplayName());
    setXMLFile("calligrasheets.rc");

    // GUI Initializations
    initView();

    d->initActions();

    const QList<KPluginFactory *> pluginFactories =
        KoPluginLoader::instantiatePluginFactories(QStringLiteral("calligrasheets/extensions"));

    foreach (KPluginFactory* factory, pluginFactories) {
        QObject *object = factory->create<QObject>(this, QVariantList());
        KXMLGUIClient *clientPlugin = dynamic_cast<KXMLGUIClient*>(object);
        if (clientPlugin) {
            insertChildClient(clientPlugin);
        } else {
            // not our/valid plugin, so delete the created object
            object->deleteLater();
        }
    }

    // Connect updateView() signal to View::update() in order to repaint its
    // child widgets: the column/row headers and the select all button.
    // Connect to Canvas::update() explicitly as it lives in the viewport
    // of the KoCanvasController.
    connect(doc(), SIGNAL(updateView()),
            this, SLOT(update()));
    connect(doc(), SIGNAL(updateView()),
            d->canvas, SLOT(update()));
    connect(doc()->map(), SIGNAL(sheetAdded(Sheet*)),
            this, SLOT(addSheet(Sheet*)));
    connect(doc()->map(), SIGNAL(sheetRemoved(Sheet*)),
            this, SLOT(removeSheet(Sheet*)));
    connect(doc()->map(), SIGNAL(sheetRevived(Sheet*)),
            this, SLOT(addSheet(Sheet*)));
    connect(doc()->map(), SIGNAL(damagesFlushed(QList<Damage*>)),
            this, SLOT(handleDamages(QList<Damage*>)));
    if (statusBar()) {
        connect(doc()->map(), SIGNAL(statusMessage(QString,int)),
                statusBar(), SLOT(showMessage(QString,int)));
    }

    connect(&d->statusBarOpTimer, SIGNAL(timeout()), this, SLOT(calcStatusBarOp()));

    // Delay the setting of the initial position, because we need to have
    // a sensible widget size, which is not always the case from the beginning
    // of the View's lifetime.
    // Therefore, initialPosition(), the last operation in the "View loading"
    // process, is called from resizeEvent(). The loading flag will be unset
    // at the end of initialPosition().

#ifndef QT_NO_DBUS
    new ViewAdaptor(this);
#endif

    d->scrollTimer = new QTimer(this);
    connect(d->scrollTimer, SIGNAL(timeout()), this, SLOT(slotAutoScroll()));

    initialPosition();

    d->canvas->setFocus();
}

View::~View()
{
    selection()->emitCloseEditor(true); // save changes

    // if (d->calcLabel) disconnect(d->calcLabel,SIGNAL(pressed(int)),this,SLOT(statusBarClicked(int)));

    d->selection->emitCloseEditor(false);
    d->selection->endReferenceSelection(false);
    d->activeSheet = 0; // set the active sheet to 0 so that when during destruction
    // of embedded child documents possible repaints in Sheet are not
    // performed.

    // delete the sheetView's after calling d->selection->emitCloseEditor cause the
    // emitCloseEditor may trigger over the Selection::emitChanged a Canvas::scrollToCell
    // which in turn needs the sheetview's to access the sheet itself.
    qDeleteAll(d->sheetViews.values());

    delete d->scrollTimer;

    delete d->selection;
    d->selection = 0;
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
    KoCanvasControllerWidget *canvasController = new KoCanvasControllerWidget(actionCollection(), this);
    d->canvasController = canvasController;
    d->canvasController->setCanvas(d->canvas);
    d->canvasController->setCanvasMode(KoCanvasController::Spreadsheet);
    canvasController->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    canvasController->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Setup the map model.
    d->mapViewModel = new MapViewModel(d->doc->map(), d->canvas, this);
    connect(d->mapViewModel, SIGNAL(addCommandRequested(KUndo2Command*)),
            doc(), SLOT(addCommand(KUndo2Command*)));
    connect(d->mapViewModel, SIGNAL(activeSheetChanged(Sheet*)),
            this, SLOT(setActiveSheet(Sheet*)));

    // Setup the selection.
    d->selection = new Selection(d->canvas);
    connect(d->selection, SIGNAL(changed(Region)), this, SLOT(slotChangeSelection(Region)));
    connect(d->selection, SIGNAL(changed(Region)), this, SLOT(slotScrollChoice(Region)));
    connect(d->selection, SIGNAL(aboutToModify(Region)), this, SLOT(aboutToModify(Region)));
    connect(d->selection, SIGNAL(modified(Region)), this, SLOT(refreshSelection(Region)));
    connect(d->selection, SIGNAL(visibleSheetRequested(Sheet*)), this, SLOT(setActiveSheet(Sheet*)));
    connect(d->selection, SIGNAL(refreshSheetViews()), this, SLOT(refreshSheetViews()));
    connect(d->selection, SIGNAL(updateAccessedCellRange(Sheet*,QPoint)), this, SLOT(updateAccessedCellRange(Sheet*,QPoint)));
    connect(this, SIGNAL(documentReadWriteToggled(bool)),
            d->selection, SIGNAL(documentReadWriteToggled(bool)));
    connect(this, SIGNAL(sheetProtectionToggled(bool)),
            d->selection, SIGNAL(sheetProtectionToggled(bool)));

    // Let the selection pointer become a canvas resource.
    QVariant variant;
    variant.setValue<void*>(d->selection);
    d->canvas->resourceManager()->setResource(::Sheets::CanvasResource::Selection, variant);
    variant.setValue<QObject*>(doc()->map()->bindingManager());

    // Load the Calligra Sheets Tools
    ToolRegistry::instance()->loadTools();

    if (mainWindow())
    {
        KoToolManager::instance()->addController(d->canvasController);
        KoToolManager::instance()->registerTools(actionCollection(), d->canvasController);
        KoModeBoxFactory modeBoxFactory(canvasController, qApp->applicationName(), i18n("Tools"));
        QDockWidget* modeBox = mainWindow()->createDockWidget(&modeBoxFactory);
        mainWindow()->dockerManager()->removeToolOptionsDocker();
        dynamic_cast<KoCanvasObserverBase*>(modeBox)->setObservedCanvas(d->canvas);

        // Setup the tool options dock widget manager.
        //connect(canvasController, SIGNAL(toolOptionWidgetsChanged(QList<QPointer<QWidget> >)),
        //        mainWindow()->dockerManager(), SLOT(newOptionWidgets(QList<QPointer<QWidget> >)));
    }
    // Setup the zoom controller.
    d->zoomHandler = new KoZoomHandler();
    d->zoomController = new KoZoomController(d->canvasController, d->zoomHandler, actionCollection(), 0, this);
    d->zoomController->zoomAction()->setZoomModes(KoZoomMode::ZOOM_CONSTANT);
    QWidget *zoomWidget = d->zoomController->zoomAction()->createWidget(statusBar());
    zoomWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    addStatusBarItem(zoomWidget, 0, true);
    connect(d->zoomController, SIGNAL(zoomChanged(KoZoomMode::Mode,qreal)),
            this, SLOT(viewZoom(KoZoomMode::Mode,qreal)));

    d->columnHeader = new ColumnHeaderWidget(this, d->canvas, this);
    d->rowHeader = new RowHeaderWidget(this, d->canvas , this);
    d->columnHeader->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    d->rowHeader->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    d->selectAllButton = new SelectAllButtonWidget(d->canvas);
    d->selectAllButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    d->canvas->setFocusPolicy(Qt::StrongFocus);
    QWidget::setFocusPolicy(Qt::StrongFocus);
    setFocusProxy(d->canvas);

    // Vert. Scroll Bar
    d->calcLabel  = 0;
    d->vertScrollBar = new QScrollBar(this);
    canvasController->setVerticalScrollBar(d->vertScrollBar);
    connect(d->vertScrollBar, SIGNAL(valueChanged(int)), canvasController, SLOT(updateCanvasOffsetY()));
    d->vertScrollBar->setOrientation(Qt::Vertical);
    d->vertScrollBar->setSingleStep(60);  //just random guess based on what feels okay
    d->vertScrollBar->setPageStep(60);  //This should be controlled dynamically, depending on how many rows are shown

    QWidget* bottomPart = new QWidget(this);
    d->tabScrollBarLayout = new QGridLayout(bottomPart);
    d->tabScrollBarLayout->setMargin(0);
    d->tabScrollBarLayout->setSpacing(0);
    d->tabScrollBarLayout->setColumnStretch(1, 1);
    d->tabBar = new TabBar(0);
    d->tabScrollBarLayout->addWidget(d->tabBar, 0, 0);
    d->horzScrollBar = new QScrollBar(0);
    canvasController->setHorizontalScrollBar(d->horzScrollBar);
    connect(d->horzScrollBar, SIGNAL(valueChanged(int)), canvasController, SLOT(updateCanvasOffsetX()));
    d->tabScrollBarLayout->addWidget(d->horzScrollBar, 0, 1, 2, 1, Qt::AlignVCenter);

    d->horzScrollBar->setOrientation(Qt::Horizontal);
    d->horzScrollBar->setSingleStep(60); //just random guess based on what feels okay
    d->horzScrollBar->setPageStep(60);

    connect(d->tabBar, SIGNAL(tabChanged(QString)), this, SLOT(changeSheet(QString)));
    connect(d->tabBar, SIGNAL(tabMoved(unsigned,unsigned)),
            this, SLOT(moveSheet(unsigned,unsigned)));
    connect(d->tabBar, SIGNAL(contextMenu(QPoint)),
            this, SLOT(popupTabBarMenu(QPoint)));
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
    d->viewLayout->addWidget(canvasController, 2, 1);
    d->viewLayout->addWidget(d->vertScrollBar, 1, 2, 2, 1, Qt::AlignHCenter);
    d->viewLayout->addWidget(bottomPart, 3, 0, 1, 2);
    d->viewLayout->setColumnMinimumWidth(2, extent);
    d->viewLayout->setRowMinimumHeight(3, extent);

    QStatusBar * sb = statusBar();
    d->calcLabel = sb ? new QLabel(sb) : 0;
    if (d->calcLabel) {
        d->calcLabel->setContextMenuPolicy(Qt::CustomContextMenu);
        addStatusBarItem(d->calcLabel, 0);
        connect(d->calcLabel , SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(statusBarClicked(QPoint)));
    }

    // signal slot
    connect(d->canvas, SIGNAL(documentSizeChanged(QSize)),
            d->canvasController->proxyObject, SLOT(updateDocumentSize(QSize)));
    connect(d->canvasController->proxyObject, SIGNAL(moveDocumentOffset(QPoint)),
            d->canvas, SLOT(setDocumentOffset(QPoint)));
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

ColumnHeaderWidget* View::columnHeader()const
{
    return d->columnHeader;
}

RowHeaderWidget* View::rowHeader()const
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

Sheet* View::activeSheet() const
{
    return d->activeSheet;
}

void View::sheetDestroyed(QObject* obj)
{
    if (Sheet *sheet = dynamic_cast<Sheet*>(obj)) {
        Q_ASSERT(d->sheetViews.contains(sheet));
        d->sheetViews.remove(sheet);
        // The SheetView will be proper destroyed already cause it's a QObject-child of the sheet.
    }
}

SheetView* View::sheetView(const Sheet* sheet) const
{
    SheetView *sheetView = d->sheetViews.value(sheet);
    if (!sheetView) {
        debugSheetsRender << "View: Creating SheetView for" << sheet->sheetName();
        sheetView = new SheetView(sheet);
        d->sheetViews.insert(sheet, sheetView);
        sheetView->setViewConverter(zoomHandler());
        connect(sheetView, SIGNAL(visibleSizeChanged(QSizeF)),
                d->canvas, SLOT(setDocumentSize(QSizeF)));
        connect(sheetView, SIGNAL(visibleSizeChanged(QSizeF)),
                d->zoomController, SLOT(setDocumentSize(QSizeF)));
        connect(sheet, SIGNAL(visibleSizeChanged()),
                sheetView, SLOT(updateAccessedCellRange()));
        connect(sheet, SIGNAL(destroyed(QObject*)),
                this, SLOT(sheetDestroyed(QObject*)));
    }
    return sheetView;
}

void View::refreshSheetViews()
{
    QList<const Sheet*> sheets = d->sheetViews.keys();
    QList< QPointer<SheetView> > sheetViews = d->sheetViews.values();

    foreach(const Sheet *sheet, d->sheetViews.keys()) {
        disconnect(sheet, SIGNAL(destroyed(QObject*)), this, SLOT(sheetDestroyed(QObject*)));
    }

    foreach (SheetView *sheetView, sheetViews) {
        disconnect(sheetView, SIGNAL(visibleSizeChanged(QSizeF)),
                   d->canvas, SLOT(setDocumentSize(QSizeF)));
        disconnect(sheetView, SIGNAL(visibleSizeChanged(QSizeF)),
                   d->zoomController, SLOT(setDocumentSize(QSizeF)));
        disconnect(sheetView->sheet(), SIGNAL(visibleSizeChanged()),
                   sheetView, SLOT(updateAccessedCellRange()));
    }

    qDeleteAll(sheetViews);
    d->sheetViews.clear();

    foreach(const Sheet *sheet, d->doc->map()->sheetList()) {
        sheet->cellStorage()->invalidateStyleCache();
    }
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
        doc()->map()->settings()->setCompletionMode((KCompletion::CompletionMode)parameterGroup.readEntry("Completion Mode", (int)(KCompletion::CompletionAuto)));
    doc()->map()->settings()->setMoveToValue((Calligra::Sheets::MoveTo)parameterGroup.readEntry("Move", (int)(Bottom)));
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
    doc()->map()->settings()->changePageOutlineColor(colorGroup.readEntry("PageOutlineColor", QColor(Qt::red)));

    initCalcMenu();
    calcStatusBarOp();
}

void View::changeNbOfRecentFiles(int _nb)
{
    if (mainWindow())
        mainWindow()->setMaxRecentItems(_nb);
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
    d->mapViewModel->setActiveSheet(sheet);

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

    updateShowSheetMenu();

    // Initialize shape anchoring action.
    shapeSelectionChanged();

    initConfig();

    d->canvas->setFocus();

    QTimer::singleShot(50, this, SLOT(finishLoading()));
}

void View::finishLoading()
{
    // finish the "View Loading" process
    d->loading = false;
    doc()->map()->deleteLoadingInfo();

    setHeaderMinima();

    // Activate the cell tool.
    if (mainWindow())
        KoToolManager::instance()->switchToolRequested("KSpreadCellToolId");
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
    d->actions->showPageOutline->setEnabled(true);
    d->tabBar->setReadOnly(doc()->map()->isProtected());
}

void View::createTemplate()
{
    KoTemplateCreateDia::createTemplate(doc()->documentPart()->templatesResourcePath(), ".ots",
                                        doc(), this);
}

void View::setActiveSheet(Sheet* sheet, bool updateSheet)
{
    // It can happen that our tabBar->activeTab() is not in sync with our activeSheet() if
    // setActiveSheet() was previously called before the delayed View::initialPosition()
    // was called and had a change to proper TabBar::setTabs().
    if (sheet == d->activeSheet && (!sheet || d->tabBar->activeTab() == sheet->sheetName()))
        return;

    if (d->activeSheet != 0 && !d->selection->referenceSelectionMode()) {
        selection()->emitCloseEditor(true); // save changes
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
    // and then update the toolmanager separately
    KoToolManager::instance()->updateShapeControllerBase(d->activeSheet, d->canvas->canvasController());

    d->canvas->shapeManager()->setShapes(d->activeSheet->shapes());
    // Tell the Canvas about the new visible sheet size.
    sheetView(d->activeSheet)->updateAccessedCellRange();

    // If there was no sheet before or the layout directions differ.
    if (!oldSheet || oldSheet->layoutDirection() != d->activeSheet->layoutDirection()) {
        // Propagate the layout direction to the canvas and horz. scrollbar.
        const Qt::LayoutDirection direction = d->activeSheet->layoutDirection();
        d->canvas->setLayoutDirection(direction);
        d->horzScrollBar->setLayoutDirection(direction);
        // Replace the painting strategy for painting shapes.
        KoShapeManager *const shapeManager = d->canvas->shapeManager();
        KoShapeManagerPaintingStrategy *paintingStrategy = 0;
        if (direction == Qt::LeftToRight) {
            paintingStrategy = new KoShapeManagerPaintingStrategy(shapeManager);
        } else {
            paintingStrategy = new RightToLeftPaintingStrategy(shapeManager, d->canvas);
        }
        shapeManager->setPaintingStrategy(paintingStrategy);
    }
    // If there was no sheet before or the formula visibilities differ.
    if (!oldSheet || oldSheet->getShowFormula() != d->activeSheet->getShowFormula()) {
        const bool showFormulas = d->activeSheet->getShowFormula();
        stateChanged("show_formulas", showFormulas ? StateNoReverse : StateReverse);
    }

    // Restore the old scrolling offset.
    QMap<Sheet*, QPointF>::ConstIterator it3 = d->savedOffsets.constFind(d->activeSheet);
    if (it3 != d->savedOffsets.constEnd()) {
        const QPoint offset = zoomHandler()->documentToView(*it3).toPoint();
        d->canvas->setDocumentOffset(offset);
        d->horzScrollBar->setValue(offset.x());
        d->vertScrollBar->setValue(offset.y());
    }

    // tell the resource manager of the newly active page
    d->canvas->resourceManager()->setResource(KoCanvasResourceManager::CurrentPage, QVariant(sheet->map()->indexOf(sheet) + 1));

    // Always repaint the visible cells.
    d->canvas->update();
    d->rowHeader->update();
    d->columnHeader->update();
    d->selectAllButton->update();

    // Prevents an endless loop, if called by the TabBar.
    if (updateSheet) {
        d->tabBar->setActiveTab(d->activeSheet->sheetName());
    }

    if (d->selection->referenceSelectionMode()) {
        d->selection->setActiveSheet(d->activeSheet);
        return;
    }

    /* see if there was a previous selection on this other sheet */
    QMap<Sheet*, QPoint>::ConstIterator it = d->savedAnchors.constFind(d->activeSheet);
    QMap<Sheet*, QPoint>::ConstIterator it2 = d->savedMarkers.constFind(d->activeSheet);

    // restore the old anchor and marker
    const QPoint newAnchor = (it == d->savedAnchors.constEnd()) ? QPoint(1, 1) : *it;
    const QPoint newMarker = (it2 == d->savedMarkers.constEnd()) ? QPoint(1, 1) : *it2;

    d->selection->clear();
    d->selection->setActiveSheet(d->activeSheet);
    d->selection->setOriginSheet(d->activeSheet);
    d->selection->initialize(QRect(newMarker, newAnchor));

    d->actions->showPageOutline->blockSignals(true);
    d->actions->showPageOutline->setChecked(d->activeSheet->isShowPageOutline());
    d->actions->showPageOutline->blockSignals(false);

    d->actions->protectSheet->blockSignals(true);
    d->actions->protectSheet->setChecked(d->activeSheet->isProtected());
    d->actions->protectSheet->blockSignals(false);

    d->actions->protectDoc->blockSignals(true);
    d->actions->protectDoc->setChecked(doc()->map()->isProtected());
    d->actions->protectDoc->blockSignals(false);

    d->adjustActions(!d->activeSheet->isProtected());
    const bool protect = d->activeSheet->isProtected();
    stateChanged("sheet_is_protected", protect ? StateNoReverse : StateReverse);

    // Auto calculation state for the INFO function.
    const bool autoCalc = d->activeSheet->isAutoCalculationEnabled();
    d->doc->map()->calculationSettings()->setAutoCalculationEnabled(autoCalc);

    calcStatusBarOp();
}

void View::changeSheet(const QString& _name)
{
    if (activeSheet()->sheetName() == _name)
        return;

    Sheet *t = doc()->map()->findSheet(_name);
    if (!t) {
        debugSheets << "Unknown sheet" << _name;
        return;
    }
    setActiveSheet(t, false /* False: Endless loop because of setActiveTab() => do the visual area update manually*/);
    d->mapViewModel->setActiveSheet(t);
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
    bool formulaVisibilityChanged = false;

    QPointer<SheetPropertiesDialog> dlg = new SheetPropertiesDialog(this);
    dlg->setLayoutDirection(d->activeSheet->layoutDirection());
    dlg->setAutoCalculationEnabled(d->activeSheet->isAutoCalculationEnabled());
    dlg->setShowGrid(d->activeSheet->getShowGrid());
    dlg->setShowPageOutline(d->activeSheet->isShowPageOutline());
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
        if (d->activeSheet->getShowFormula() != dlg->showFormula()) {
            formulaVisibilityChanged = true;
        }

        command->setLayoutDirection(dlg->layoutDirection());
        command->setAutoCalculationEnabled(dlg->autoCalc());
        command->setShowGrid(dlg->showGrid());
        command->setShowPageOutline(dlg->showPageOutline());
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
        d->canvas->setLayoutDirection(d->activeSheet->layoutDirection()); // for scrolling
        d->horzScrollBar->setLayoutDirection(d->activeSheet->layoutDirection());
        d->columnHeader->update();
        // Replace the painting strategy for painting shapes.
        KoShapeManager *const shapeManager = d->canvas->shapeManager();
        KoShapeManagerPaintingStrategy *paintingStrategy = 0;
        if (d->activeSheet->layoutDirection() == Qt::LeftToRight) {
            paintingStrategy = new KoShapeManagerPaintingStrategy(shapeManager);
        } else {
            paintingStrategy = new RightToLeftPaintingStrategy(shapeManager, d->canvas);
        }
        shapeManager->setPaintingStrategy(paintingStrategy);
    }
    if (formulaVisibilityChanged) {
        const bool showFormulas = d->activeSheet->getShowFormula();
        stateChanged("show_formulas", showFormulas ? StateNoReverse : StateReverse);
        sheetView(d->activeSheet)->invalidate();
        d->canvas->update();
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
    KUndo2Command* command = new AddSheetCommand(t);
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

    KUndo2Command* command = new HideSheetCommand(activeSheet());
    doc()->addCommand(command);

    d->tabBar->removeTab(d->activeSheet->sheetName());
    d->tabBar->setActiveTab(sn);
}

void View::showSheet()
{
    if (!d->activeSheet)
        return;

    ShowDialog dialog(this, d->selection);
    dialog.exec();
}

void View::copyAsText()
{
    if (!d->activeSheet)
        return;
    QMimeData* mimeData = new QMimeData();
    mimeData->setText(CopyCommand::saveAsPlainText(*selection()));

    QApplication::clipboard()->setMimeData(mimeData);
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

    bool success;
    if (mode) {
        success = doc()->map()->showPasswordDialog(this, ProtectableObject::Lock,
                  i18n("Protect Document"));
    } else {
        success = doc()->map()->showPasswordDialog(this, ProtectableObject::Unlock,
                  i18n("Unprotect Document"));
    }
    if (!success) {
        d->actions->protectDoc->setChecked(!mode);
        return;
    }

    doc()->setModified(true);
    stateChanged("map_is_protected", mode ? StateNoReverse : StateReverse);
    d->tabBar->setReadOnly(doc()->map()->isProtected());
}

void View::toggleProtectSheet(bool mode)
{
    if (!d->activeSheet)
        return;

    bool success;
    if (mode) {
        success = activeSheet()->showPasswordDialog(this, ProtectableObject::Lock,
                  i18n("Protect Sheet"));
    } else {
        success = activeSheet()->showPasswordDialog(this, ProtectableObject::Unlock,
                  i18n("Unprotect Sheet"));
    }
    if (!success) {
        d->actions->protectSheet->setChecked(!mode);
        return;
    }

    doc()->setModified(true);
    d->adjustActions(!mode);

    // The sheet protection change may hide/unhide some values or formulas,
    // so the cached visual data has become invalid.
    refreshSheetViews();
    d->canvas->update();

    // inform the cell tool
    emit sheetProtectionToggled(mode);
}

void View::togglePageOutline(bool mode)
{
    if (!d->activeSheet)
        return;

    d->activeSheet->setShowPageOutline(mode);
}

void View::viewZoom(KoZoomMode::Mode mode, qreal zoom)
{
    Q_UNUSED(zoom)
#ifdef NDEBUG
    Q_UNUSED(mode);
#endif
    Q_ASSERT(mode == KoZoomMode::ZOOM_CONSTANT);
    selection()->emitCloseEditor(true); // save changes
    setHeaderMinima();
    d->canvas->update();
    d->columnHeader->update();
    d->rowHeader->update();
    d->selectAllButton->update();
}

void View::showColumnHeader(bool enable)
{
    doc()->map()->settings()->setShowColumnHeader(enable);
    d->columnHeader->setVisible(enable);
    d->selectAllButton->setVisible(enable && d->rowHeader->isVisible());
}

void View::showRowHeader(bool enable)
{
    doc()->map()->settings()->setShowRowHeader(enable);
    d->rowHeader->setVisible(enable);
    d->selectAllButton->setVisible(enable && d->columnHeader->isVisible());
}

void View::showHorizontalScrollBar(bool enable)
{
    doc()->map()->settings()->setShowHorizontalScrollBar(enable);
    d->horzScrollBar->setVisible(enable);
}

void View::showVerticalScrollBar(bool enable)
{
    doc()->map()->settings()->setShowVerticalScrollBar(enable);
    d->vertScrollBar->setVisible(enable);
}

void View::showStatusBar(bool enable)
{
    doc()->map()->settings()->setShowStatusBar(enable);
    if (statusBar()) {
        statusBar()->setVisible(enable);
    }
}

void View::showTabBar(bool enable)
{
    doc()->map()->settings()->setShowTabBar(enable);
    d->tabBar->setVisible(enable);
}

void View::optionsNotifications()
{  
#ifndef QT_NO_DBUS
    KNotifyConfigWidget::configure(this);
#endif
}

void View::preference()
{
    PreferenceDialog dialog(this);
    dialog.exec();
}

void View::nextSheet()
{
    Sheet * t = doc()->map()->nextSheet(activeSheet());
    if (!t) {
        debugSheets << "Unknown sheet";
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
        debugSheets << "Unknown sheet";
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
        debugSheets << "Unknown sheet";
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
        debugSheets << "Unknown sheet";
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
        if (event->key() == Qt::Key_V) { // Ctrl+Shift+V to show debug (similar to Words)
            d->activeSheet->printDebug();
        }
    }
#endif
    QWidget::keyPressEvent(event);
}

int View::leftBorder() const
{
    return (int)(((RowHeader*)d->rowHeader)->width());
}

int View::rightBorder() const
{
    return d->vertScrollBar->width();
}

int View::topBorder() const
{
    return (int)(((ColumnHeader*)d->columnHeader)->height());
}

int View::bottomBorder() const
{
    return d->horzScrollBar->height();
}

void View::setHeaderMinima()
{
    if (d->loading)   // "View Loading" not finished yet
        return;
    QFont font(KoGlobal::defaultFont());
    QFontMetricsF fm(font, 0);
    qreal h = fm.height() + 3;
    qreal w = fm.width(QString::fromLatin1("99999")) + 3;
    d->columnHeader->setMinimumHeight(qRound(h));
    d->rowHeader->setMinimumWidth(qRound(w));
    d->selectAllButton->setMinimumHeight(qRound(h));
    d->selectAllButton->setMinimumWidth(qRound(w));
}

void View::paperLayoutDlg()
{
    selection()->emitCloseEditor(true); // save changes
    SheetPrint* print = d->activeSheet->print();

    KoPageLayout pl = print->settings()->pageLayout();


/*
    const HeaderFooter *const headerFooter = print->headerFooter();
    HeadFoot hf;
    hf.headLeft  = headerFooter->localizeHeadFootLine(headerFooter->headLeft());
    hf.headRight = headerFooter->localizeHeadFootLine(headerFooter->headRight());
    hf.headMid   = headerFooter->localizeHeadFootLine(headerFooter->headMid());
    hf.footLeft  = headerFooter->localizeHeadFootLine(headerFooter->footLeft());
    hf.footRight = headerFooter->localizeHeadFootLine(headerFooter->footRight());
    hf.footMid   = headerFooter->localizeHeadFootLine(headerFooter->footMid());
*/
    PageLayoutDialog dialog(this, d->activeSheet);
    dialog.exec();
}

void View::resetPrintRange()
{
    DefinePrintRangeCommand* command = new DefinePrintRangeCommand();
    command->setText(kundo2_i18n("Reset Print Range"));
    command->setSheet(activeSheet());
    command->add(Region(QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax)), activeSheet()));
    doc()->addCommand(command);
}

void View::deleteSheet()
{
    if (doc()->map()->count() <= 1 || (doc()->map()->visibleSheets().count() <= 1)) {
        KMessageBox::sorry(this, i18n("You cannot delete the only sheet."), i18n("Remove Sheet"));
        return;
    }
    int ret = KMessageBox::warningContinueCancel(this, i18n("You are about to remove the active sheet.\nDo you want to continue?"),
              i18n("Remove Sheet"), KStandardGuiItem::del());

    if (ret == KMessageBox::Continue) {
        selection()->emitCloseEditor(false); // discard changes
        doc()->setModified(true);
        Sheet * tbl = activeSheet();
        KUndo2Command* command = new RemoveSheetCommand(tbl);
        doc()->addCommand(command);
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
    QString newName = QInputDialog::getText(this, i18n("Rename Sheet"), i18n("Enter name:"), QLineEdit::Normal, activeName, &ok);

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

        KUndo2Command* command = new RenameSheetCommand(sheet, newName);
        doc()->addCommand(command);

        doc()->setModified(true);
    }
}

//------------------------------------------------
//
// Document signals
//
//------------------------------------------------

void View::slotChangeSelection(const Calligra::Sheets::Region& changedRegion)
{
    if (!changedRegion.isValid())
        return;

    if (d->selection->referenceSelectionMode()) {
        doc()->map()->addDamage(new SelectionDamage(changedRegion));
        debugSheetsFormula << "Choice:" << *selection();
        return;
    }

    // delayed recalculation of the operation shown in the status bar
    d->statusBarOpTimer.setSingleShot(true);
    d->statusBarOpTimer.start(250);

    if (!d->loading && !doc()->map()->isLoading()) {
        doc()->map()->addDamage(new SelectionDamage(changedRegion));
    }
    d->rowHeader->update();
    d->columnHeader->update();
    d->selectAllButton->update();

    if (d->selection->isColumnSelected() || d->selection->isRowSelected()) {
        return;
    }

    d->canvas->validateSelection();
}

void View::slotScrollChoice(const Calligra::Sheets::Region& changedRegion)
{
    if (!selection()->referenceSelectionMode() || !changedRegion.isValid()) {
        return;
    }
}

void View::calcStatusBarOp()
{
    Sheet * sheet = activeSheet();
    ValueCalc* calc = doc()->map()->calc();
    Value val;
    QString prefix = "";

    MethodOfCalc tmpMethod = doc()->map()->settings()->getTypeOfCalc();
    if (sheet && tmpMethod != NoneCalc) {
        Value range = sheet->cellStorage()->valueRegion(*d->selection);
        switch (tmpMethod) {
        case SumOfNumber:
            val = calc->sum(range);
            prefix = i18n("Sum: ");
            break;
        case Average:
            val = calc->avg(range);
            prefix = i18n("Average: ");
            break;
        case Min:
            val = calc->min(range);
            prefix = i18n("Min: ");
            break;
        case Max:
            val = calc->max(range);
            prefix = i18n("Max: ");
            break;
        case CountA:
            val = Value(calc->count(range));
            prefix = i18n("Count: ");
            break;
        case Count:
            val = Value(calc->count(range, false));
            prefix = i18n("CountA: ");
        case NoneCalc:
            break;
        default:
            break;
        }
        if ((range.columns() > 1) || (range.rows() > 1)) {
            QString size = i18n("%1x%2", range.columns(), range.rows());
            prefix = prefix.size() ? size + ", " + prefix : size;
        }
    }

    QString res = doc()->map()->converter()->asString(val).asString();
    QString tmp;
    if (res.length()) tmp = prefix + res;

    if (d->calcLabel)
        d->calcLabel->setText(QString(' ') + tmp + ' ');
}

void View::statusBarClicked(const QPoint&)
{
    QPoint mousepos = QCursor::pos();
    if (factory())
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

QWidget* View::canvas() const
{
    return d->canvas;
}

void View::popupTabBarMenu(const QPoint & _point)
{
    if (!factory())
        return;
    if (d->tabBar) {
        QMenu* const menu = static_cast<QMenu*>(factory()->container("menupage_popup", this));
        if (!menu)
            return;

        QAction* insertSheet = new QAction(koIcon("insert-table"), i18n("Insert Sheet"), this);
        insertSheet->setToolTip(i18n("Remove the active sheet"));
        connect(insertSheet, SIGNAL(triggered(bool)), this, SLOT(insertSheet()));
        menu->insertAction(d->actions->duplicateSheet, insertSheet);

        QAction* deleteSheet = new QAction(koIcon("delete_table"), i18n("Remove Sheet"), this);
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
        delete insertSheet;
        delete deleteSheet;
    }
}

void View::updateBorderButton()
{
    if (d->activeSheet)
        d->actions->showPageOutline->setChecked(d->activeSheet->isShowPageOutline());
}

void View::addSheet(Sheet *sheet)
{
    if (!sheet->isHidden()) {
        d->tabBar->addTab(sheet->sheetName());
    }
    const bool state = (doc()->map()->visibleSheets().count() > 1);
    d->actions->deleteSheet->setEnabled(state);
    d->actions->hideSheet->setEnabled(state);

    // Connect some signals
    connect(sheet, SIGNAL(shapeAdded(Sheet*,KoShape*)),
            d->mapViewModel, SLOT(addShape(Sheet*,KoShape*)));
    connect(sheet, SIGNAL(shapeRemoved(Sheet*,KoShape*)),
            d->mapViewModel, SLOT(removeShape(Sheet*,KoShape*)));
}

void View::removeSheet(Sheet *sheet)
{
    d->tabBar->removeTab(sheet->sheetName());
    setActiveSheet(doc()->map()->sheet(0));

    const bool state = (doc()->map()->visibleSheets().count() > 1);
    d->actions->deleteSheet->setEnabled(state);
    d->actions->hideSheet->setEnabled(state);

    // Disconnect signals.
    disconnect(sheet, 0, d->mapViewModel, 0);
}

QColor View::borderColor() const
{
    return d->canvas->resourceManager()->foregroundColor().toQColor();
}

void View::updateShowSheetMenu()
{
    if (d->activeSheet) {
        if (d->activeSheet->map()->isProtected())
            d->actions->showSheet->setEnabled(false);
        else
            d->actions->showSheet->setEnabled(doc()->map()->hiddenSheets().count() > 0);
    }
}

QPoint View::markerFromSheet(Sheet* sheet) const
{
    QMap<Sheet*, QPoint>::ConstIterator it = d->savedMarkers.constFind(sheet);
    QPoint newMarker = (it == d->savedMarkers.constEnd()) ? QPoint(1, 1) : *it;
    return newMarker;
}

QPointF View::offsetFromSheet(Sheet* sheet) const
{
    QMap<Sheet*, QPointF>::ConstIterator it = d->savedOffsets.constFind(sheet);
    QPointF offset = (it == d->savedOffsets.constEnd()) ? QPointF() : *it;
    return offset;
}

void View::saveCurrentSheetSelection()
{
    /* save the current selection on this sheet */
    if (d->activeSheet != 0) {
        d->savedAnchors.remove(d->activeSheet);
        d->savedAnchors.insert(d->activeSheet, d->selection->anchor());
        debugSheetsUI << " Current scrollbar vert value:" << d->vertScrollBar->value();
        debugSheetsUI << "Saving marker pos:" << d->selection->marker();
        d->savedMarkers.remove(d->activeSheet);
        d->savedMarkers.insert(d->activeSheet, d->selection->marker());
        d->savedOffsets.remove(d->activeSheet);
        d->savedOffsets.insert(d->activeSheet, QPointF(d->canvas->xOffset(),
                               d->canvas->yOffset()));
    }
}

void View::handleDamages(const QList<Damage*>& damages)
{
    QRegion paintRegion;
    enum { Nothing, Everything, Clipped } paintMode = Nothing;

    QList<Damage*>::ConstIterator end(damages.end());
    for (QList<Damage*>::ConstIterator it = damages.begin(); it != end; ++it) {
        Damage* damage = *it;
        if (!damage) continue;

        if (damage->type() == Damage::Cell) {
            CellDamage* cellDamage = static_cast<CellDamage*>(damage);
            debugSheetsDamage << "Processing\t" << *cellDamage;
            Sheet* const damagedSheet = cellDamage->sheet();

            if (cellDamage->changes() & CellDamage::Appearance) {
                const Region& region = cellDamage->region();
                sheetView(damagedSheet)->invalidateRegion(region);
                paintMode = Everything;
            }
            continue;
        }

        if (damage->type() == Damage::Sheet) {
            SheetDamage* sheetDamage = static_cast<SheetDamage*>(damage);
            debugSheetsDamage << *sheetDamage;
            const SheetDamage::Changes changes = sheetDamage->changes();
            if (changes & (SheetDamage::Name | SheetDamage::Shown)) {
                d->tabBar->setTabs(doc()->map()->visibleSheets());
                paintMode = Everything;
            }
            if (changes & (SheetDamage::Shown | SheetDamage::Hidden)) {
                updateShowSheetMenu();
                paintMode = Everything;
            }
            // The following changes only affect the active sheet.
            if (sheetDamage->sheet() != d->activeSheet) {
                continue;
            }
            if (changes.testFlag(SheetDamage::ContentChanged)) {
                update();
                paintMode = Everything;
            }
            if (changes.testFlag(SheetDamage::PropertiesChanged)) {
                sheetView(d->activeSheet)->invalidate();
                paintMode = Everything;
            }
            if (sheetDamage->changes() & SheetDamage::ColumnsChanged)
                columnHeader()->update();
            if (sheetDamage->changes() & SheetDamage::RowsChanged)
                rowHeader()->update();
            continue;
        }

        if (damage->type() == Damage::Selection) {
            SelectionDamage* selectionDamage = static_cast<SelectionDamage*>(damage);
            debugSheetsDamage << "Processing\t" << *selectionDamage;
            const Region region = selectionDamage->region();

            if (paintMode == Clipped) {
                const QRectF rect = canvasWidget()->cellCoordinatesToView(region.boundingRect());
                paintRegion += rect.toRect().adjusted(-3, -3, 4, 4);
            } else {
                paintMode = Everything;
            }
            continue;
        }

        debugSheetsDamage << "Unhandled\t" << *damage;
    }

    // At last repaint the dirty cells.
    if (paintMode == Clipped) {
        canvas()->update(paintRegion);
    } else if (paintMode == Everything) {
        canvas()->update();
    }
}

KoPrintJob * View::createPrintJob()
{
    if (!activeSheet())
        return 0;
    // About to print; close the editor.
    selection()->emitCloseEditor(true); // save changes
    return new PrintJob(this);
}

void View::updateAccessedCellRange(Sheet* sheet, const QPoint &location)
{
    sheetView(sheet)->updateAccessedCellRange(location);
}

void View::enableAutoScroll()
{
    d->scrollTimer->start(50);
}

void View::disableAutoScroll()
{
    d->scrollTimer->stop();
}

int View::autoScrollAcceleration(int offset) const
{
    if (offset < 40)
        return offset;
    else
        return offset*offset / 40;
}

void View::slotAutoScroll()
{
    QPoint scrollDistance;
    bool actuallyDoScroll = false;
    QPoint pos(mapFromGlobal(QCursor::pos()));

    //Provide progressive scrolling depending on the mouse position
    if (pos.y() < topBorder()) {
        scrollDistance.setY((int) - autoScrollAcceleration(- pos.y() + topBorder()));
        actuallyDoScroll = true;
    } else if (pos.y() > height() - bottomBorder()) {
        scrollDistance.setY((int) autoScrollAcceleration(pos.y() - height() + bottomBorder()));
        actuallyDoScroll = true;
    }

    if (pos.x() < leftBorder()) {
        scrollDistance.setX((int) - autoScrollAcceleration(- pos.x() + leftBorder()));
        actuallyDoScroll = true;
    } else if (pos.x() > width() - rightBorder()) {
        scrollDistance.setX((int) autoScrollAcceleration(pos.x() - width() + rightBorder()));
        actuallyDoScroll = true;
    }

    if (actuallyDoScroll) {
        pos = canvas()->mapFrom(this, pos);
        QMouseEvent* event = new QMouseEvent(QEvent::MouseMove, pos, Qt::NoButton, Qt::NoButton,
                                             QApplication::keyboardModifiers());

        QApplication::postEvent(canvas(), event);
        emit autoScroll(scrollDistance);
    }
}
