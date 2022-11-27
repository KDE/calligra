// This file is part of the KDE project
// SPDX-FileCopyrightText: 2006-2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-FileCopyrightText: 2005-2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
// SPDX-FileCopyrightText: 2002-2005 Ariya Hidayat <ariya@kde.org>
// SPDX-FileCopyrightText: 1999-2003 Laurent Montel <montel@kde.org>
// SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
// SPDX-FileCopyrightText: 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
// SPDX-FileCopyrightText: 2002-2003 John Dailey <dailey@vt.edu>
// SPDX-FileCopyrightText: 1999-2003 David Faure <faure@kde.org>
// SPDX-FileCopyrightText: 1999-2001 Simon Hausmann <hausmann@kde.org>
// SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "CellToolBase.h"
#include "CellToolBase_p.h"

// Sheets
#include "engine/CalculationSettings.h"
#include "engine/Damages.h"
#include "engine/Localization.h"
#include "engine/ValueConverter.h"

#include "core/CellStorage.h"
#include "core/ColFormatStorage.h"
#include "core/Database.h"
#include "core/DocBase.h"
#include "core/Map.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"
#include "core/StyleManager.h"
#include "core/odf/SheetsOdf.h"

#include "ActionOptionWidget.h"
#include "CellEditor.h"
#include "CellView.h"
#include "ExternalEditor.h"
#include "FilterPopup.h"
#include "inspector.h"
#include "SheetView.h"

// actions
#include "actions/Actions.h"
#include "actions/CellAction.h"
#include "actions/Comment.h"   // for CommentCommand, which is used by the Find action

// commands
#include "commands/AutoFilterCommand.h"
#include "commands/BorderColorCommand.h"
#include "commands/ConditionCommand.h"
#include "commands/CopyCommand.h"
#include "commands/DataManipulators.h"
#include "commands/DeleteCommand.h"
#include "commands/MergeCommand.h"
#include "commands/PageBreakCommand.h"
#include "commands/PasteCommand.h"
#include "commands/PrecisionCommand.h"
#include "commands/RowColumnManipulators.h"
#include "commands/SpellCheckCommand.h"
#include "commands/StyleCommand.h"

// dialogs
#include "dialogs/AddNamedAreaDialog.h"
#include "dialogs/AngleDialog.h"
#include "dialogs/AutoFormatDialog.h"
#include "dialogs/ConditionalDialog.h"
#include "dialogs/ConsolidateDialog.h"
#include "dialogs/DatabaseDialog.h"
#include "dialogs/DocumentSettingsDialog.h"
#include "dialogs/GoalSeekDialog.h"
#include "dialogs/GotoDialog.h"
#include "dialogs/InsertDialog.h"
#include "dialogs/LayoutDialog.h"
#include "dialogs/ListDialog.h"
#include "dialogs/NamedAreaDialog.h"
#include "dialogs/PasteInsertDialog.h"
#include "dialogs/SpecialPasteDialog.h"
#include "dialogs/StyleManagerDialog.h"
#include "dialogs/SubtotalDialog.h"
#include "dialogs/pivot.h"

// strategies
#include "strategy/AutoFillStrategy.h"
#include "strategy/DragAndDropStrategy.h"
#include "strategy/HyperlinkStrategy.h"
#include "strategy/MergeStrategy.h"
#include "strategy/PasteStrategy.h"
#include "strategy/SelectionStrategy.h"

// Calligra
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoColorPopupAction.h>
#include <KoPointerEvent.h>
#include <KoShape.h>
#include <KoViewConverter.h>
#include <KoColor.h>
#include <KoIcon.h>

// KF5
#include <kfind.h>
#include <kfontaction.h>
#include <kfontsizeaction.h>
#include <kmessagebox.h>
#include <kreplace.h>
#include <kstandardaction.h>
#include <ktoggleaction.h>

// Qt
#include <QStandardPaths>
#include <QClipboard>
#include <QInputDialog>
#include <QBuffer>
#include <QMimeData>
#include <QMenu>
#include <QPainter>
#include <QDomDocument>
#ifndef QT_NO_SQL
// #include <QSqlDatabase>
#endif

#ifndef NDEBUG
#include <QTableView>
#include "core/SheetModel.h"
#endif

using namespace Calligra::Sheets;

CellToolBase::CellToolBase(KoCanvasBase* canvas)
        : KoInteractionTool(canvas)
        , d(new Private(this))
{
    d->cellEditor = 0;
    d->externalEditor = 0;
    d->formulaDialog = 0;
    d->initialized = false;
    d->popupListChoose = 0;
    d->lastEditorWithFocus = EmbeddedEditor;
    d->borderColor = Qt::black;

    d->findOptions = 0;
    d->findLeftColumn = 0;
    d->findRightColumn = 0;
    d->findTopRow = 0;
    d->findBottomRow = 0;
    d->typeValue = FindOption::Value;
    d->directionValue = FindOption::Row;
    d->find = 0;
    d->replace = 0;
    d->replaceCommand = 0;

    d->searchInSheets.currentSheet = 0;
    d->searchInSheets.firstSheet = 0;

    // Create the extra and ones with extended names for the context menu.
    d->createPopupMenuActions();
    
    // Create the actions.
    QAction* action = nullptr;

    // -- cell style actions --
    d->actions = new Actions(this);

    action = new QAction(koIcon("cell_layout"), i18n("Cell Format..."), this);
    action->setIconText(i18n("Format"));
    addAction("cellStyle", action);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_F));
    connect(action, &QAction::triggered, this, &CellToolBase::cellStyle);
    action->setToolTip(i18n("Set the cell formatting"));

    action = new QAction(i18n("Style Manager..."), this);
    addAction("styleDialog", action);
    connect(action, &QAction::triggered, this, &CellToolBase::styleDialog);
    action->setToolTip(i18n("Edit and organize cell styles"));

    auto selectAction = new KSelectAction(i18n("Style"), this);
    addAction("setStyle", selectAction);
    selectAction->setToolTip(i18n("Apply a predefined style to the selected cells"));
    connect(selectAction, QOverload<const QString &>::of(&KSelectAction::triggered), this, &CellToolBase::setStyle);

    action = new QAction(i18n("Create Style From Cell..."), this);
    action->setIconText(i18n("Style From Cell"));
    addAction("createStyleFromCell", action);
    connect(action, &QAction::triggered, this, &CellToolBase::createStyleFromCell);
    action->setToolTip(i18n("Create a new style based on the currently selected cell"));

    // -- font actions --

    auto fontAction = new KFontAction(i18n("Select Font..."), this);
    fontAction->setIconText(i18n("Font"));
    addAction("font", fontAction);
    connect(fontAction, QOverload<const QString &>::of(&KFontAction::triggered), this, &CellToolBase::font);

    auto fontSizeAction = new KFontSizeAction(i18n("Select Font Size"), this);
    fontSizeAction->setIconText(i18n("Font Size"));
    addAction("fontSize", fontSizeAction);
    connect(fontSizeAction, &KFontSizeAction::fontSizeChanged, this, &CellToolBase::fontSize);

    auto colorAction = new KoColorPopupAction(this);
    colorAction->setIcon(koIcon("format-text-color"));
    colorAction->setText(i18n("Text Color"));
    colorAction->setToolTip(i18n("Set the text color"));
    addAction("textColor", colorAction);
    connect(colorAction, &KoColorPopupAction::colorChanged, this, &CellToolBase::changeTextColor);

    // -- horizontal alignment actions --

    QActionGroup* groupAlign = new QActionGroup(this);
    action = new KToggleAction(koIcon("format-justify-left"), i18n("Align Left"), this);
    action->setIconText(i18n("Left"));
    addAction("alignLeft", action);
    connect(action, &QAction::triggered, this, &CellToolBase::alignLeft);
    action->setToolTip(i18n("Left justify the cell contents"));
    action->setActionGroup(groupAlign);

    action = new KToggleAction(koIcon("format-justify-center"), i18n("Align Center"), this);
    action->setIconText(i18n("Center"));
    addAction("alignCenter", action);
    connect(action, &QAction::triggered, this, &CellToolBase::alignCenter);
    action->setToolTip(i18n("Center the cell contents"));
    action->setActionGroup(groupAlign);

    action = new KToggleAction(koIcon("format-justify-right"), i18n("Align Right"), this);
    action->setIconText(i18n("Right"));
    addAction("alignRight", action);
    connect(action, &QAction::triggered, this, &CellToolBase::alignRight);
    action->setToolTip(i18n("Right justify the cell contents"));
    action->setActionGroup(groupAlign);

    // -- vertical alignment actions --

    QActionGroup* groupPos = new QActionGroup(this);
    action = new KToggleAction(koIcon("format-align-vertical-top"), i18n("Align Top"), this);
    action->setIconText(i18n("Top"));
    addAction("alignTop", action);
    connect(action, &QAction::triggered, this, &CellToolBase::alignTop);
    action->setToolTip(i18n("Align cell contents along the top of the cell"));
    action->setActionGroup(groupPos);

    action = new KToggleAction(koIcon("format-align-vertical-center"), i18n("Align Middle"), this);
    action->setIconText(i18n("Middle"));
    addAction("alignMiddle", action);
    connect(action, &QAction::triggered, this, &CellToolBase::alignMiddle);
    action->setToolTip(i18n("Align cell contents centered in the cell"));
    action->setActionGroup(groupPos);

    action = new KToggleAction(koIcon("format-align-vertical-bottom"), i18n("Align Bottom"), this);
    action->setIconText(i18n("Bottom"));
    addAction("alignBottom", action);
    connect(action, &QAction::triggered, this, &CellToolBase::alignBottom);
    action->setToolTip(i18n("Align cell contents along the bottom of the cell"));
    action->setActionGroup(groupPos);

    // -- border actions --

    colorAction = new KoColorPopupAction(this);
    colorAction->setIcon(koIcon("format-stroke-color"));
    colorAction->setToolTip(i18n("Select a new border color"));
    colorAction->setText(i18n("Border Color"));
    colorAction->setCurrentColor(selectedBorderColor());
    addAction("borderColor", colorAction);
    connect(colorAction, &KoColorPopupAction::colorChanged, this, &CellToolBase::borderColor);

    // -- text layout actions --

    action = new QAction(i18n("Change Angle..."), this);
    action->setIconText(i18n("Angle"));
    addAction("changeAngle", action);
    connect(action, &QAction::triggered, this, &CellToolBase::changeAngle);
    action->setToolTip(i18n("Change the angle that cell contents are printed"));

    // -- value format actions --

    action = new QAction(koIcon("format-precision-more"), i18n("Increase Precision"), this);
    addAction("increasePrecision", action);
    connect(action, &QAction::triggered, this, &CellToolBase::increasePrecision);
    action->setToolTip(i18n("Increase the decimal precision shown onscreen"));

    action = new QAction(koIcon("format-precision-less"), i18n("Decrease Precision"), this);
    addAction("decreasePrecision", action);
    connect(action, &QAction::triggered, this, &CellToolBase::decreasePrecision);
    action->setToolTip(i18n("Decrease the decimal precision shown onscreen"));

    // -- misc style attribute actions --

    colorAction = new KoColorPopupAction(this);
    colorAction->setIcon(koIcon("format-fill-color"));
    colorAction->setToolTip(i18n("Set the background color"));
    colorAction->setText(i18n("Background Color"));
    addAction("backgroundColor", colorAction);
    connect(colorAction, &KoColorPopupAction::colorChanged, this, &CellToolBase::changeBackgroundColor);

    // -- cell insert/remove actions --

    action = new QAction(koIcon("insertcell"), i18n("Cells..."), this);
    action->setIconText(i18n("Insert Cells..."));
    action->setToolTip(i18n("Insert a blank cell into the spreadsheet"));
    addAction("insertCell", action);
    connect(action, &QAction::triggered, this, &CellToolBase::insertCells);

    action = new QAction(koIcon("removecell"), i18n("Cells..."), this);
    action->setIconText(i18n("Remove Cells..."));
    action->setToolTip(i18n("Removes the cells from the spreadsheet"));
    addAction("deleteCell", action);
    connect(action, &QAction::triggered, this, &CellToolBase::deleteCells);

    // -- cell content actions --

    action = new QAction(koIcon("deletecell"), i18n("Clear All"), this);
    action->setIconText(i18n("Clear All"));
    action->setToolTip(i18n("Clear all contents and formatting of the current cell"));
    addAction("clearAll", action);
    connect(action, &QAction::triggered, this, &CellToolBase::clearAll);

    action = new QAction(koIcon("edit-clear"), i18n("Contents"), this);
    action->setIconText(i18n("Clear Contents"));
    action->setToolTip(i18n("Remove the contents of the current cell"));
    addAction("clearContents", action);
    connect(action, &QAction::triggered, this, &CellToolBase::clearContents);

    action = new QAction(i18n("Conditional Styles..."), this);
    action->setToolTip(i18n("Set cell style based on certain conditions"));
    addAction("conditional", action);
    connect(action, &QAction::triggered, this, &CellToolBase::conditional);

    action = new QAction(i18n("Conditional Styles"), this);
    action->setIconText(i18n("Remove Conditional Styles"));
    action->setToolTip(i18n("Remove the conditional cell styles"));
    addAction("clearConditional", action);
    connect(action, &QAction::triggered, this, &CellToolBase::clearConditionalStyles);

    // -- sorting/filtering action --

    action = new QAction(koIcon("view-filter"), i18n("&Auto-Filter"), this);
    addAction("autoFilter", action);
    connect(action, &QAction::triggered, this, &CellToolBase::autoFilter);
    action->setToolTip(i18n("Add an automatic filter to a cell range"));

    // -- data insert actions --

    action = new QAction(koIcon("insert-math-expression"), i18n("&Function..."), this);
    addAction("insertFormula", action);
    connect(action, &QAction::triggered, this, &CellToolBase::insertFormula);
    action->setToolTip(i18n("Insert math expression"));

#ifndef QT_NO_SQL
    action = new QAction(koIcon("network-server-database"), i18n("From &Database..."), this);
    action->setIconText(i18n("Database"));
    addAction("insertFromDatabase", action);
    connect(action, &QAction::triggered, this, &CellToolBase::insertFromDatabase);
    action->setToolTip(i18n("Insert data from a SQL database"));
#endif

    action = new QAction(i18n("Custom Lists..."), this);
    addAction("sortList", action);
    connect(action, &QAction::triggered, this, &CellToolBase::sortList);
    action->setToolTip(i18n("Create custom lists for sorting or autofill"));

    action = new QAction(i18n("&Consolidate..."), this);
    addAction("consolidate", action);
    connect(action, &QAction::triggered, this, &CellToolBase::consolidate);
    action->setToolTip(i18n("Create a region of summary data from a group of similar regions"));

    action = new QAction(i18n("&Goal Seek..."), this);
    addAction("goalSeek", action);
    connect(action, &QAction::triggered, this, &CellToolBase::goalSeek);
    action->setToolTip(i18n("Repeating calculation to find a specific value"));

    action = new QAction(i18n("&Subtotals..."), this);
    addAction("subtotals", action);
    connect(action, &QAction::triggered, this, &CellToolBase::subtotals);
    action->setToolTip(i18n("Create different kind of subtotals to a list or database"));
    
    action = new QAction(i18n("&Pivot Tables..."), this);
    addAction("Pivot", action);
    connect(action, &QAction::triggered, this, &CellToolBase::pivot);
    action->setToolTip(i18n("Create Pivot Tables"));
    
    action = new QAction(i18n("Area Name..."), this);
    addAction("setAreaName", action);
    connect(action, &QAction::triggered, this, &CellToolBase::setAreaName);
    action->setToolTip(i18n("Set a name for a region of the spreadsheet"));

    action = new QAction(i18n("Named Areas..."), this);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_G));
    action->setIconText(i18n("Named Areas"));
    action->setIcon(koIcon("bookmarks"));
    action->setToolTip(i18n("Edit or select named areas"));
    addAction("namedAreaDialog", action);
    connect(action, &QAction::triggered, this, &CellToolBase::namedAreaDialog);

    selectAction = new KSelectAction(i18n("Formula Selection"), this);
    addAction("formulaSelection", selectAction);
    selectAction->setToolTip(i18n("Insert a function"));
    QStringList functionList = {"SUM", "AVERAGE", "IF", "COUNT", "MIN", "MAX", i18n("Others...")};
    selectAction->setItems(functionList);
    selectAction->setComboWidth(80);
    selectAction->setCurrentItem(0);
    connect(selectAction, QOverload<const QString &>::of(&KSelectAction::triggered), this, &CellToolBase::formulaSelection);

    // -- general editing actions --

    action = new QAction(koIcon("cell_edit"), i18n("Modify Cell"), this);
    addAction("editCell", action);
    action->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::Key_M));
    connect(action, &QAction::triggered, this, &CellToolBase::edit);
    action->setToolTip(i18n("Edit the highlighted cell"));

    action = KStandardAction::cut(this, SLOT(cut()), this);
    action->setToolTip(i18n("Move the cell object to the clipboard"));
    addAction("cut", action);

    action = KStandardAction::copy(this, SLOT(copy()), this);
    action->setToolTip(i18n("Copy the cell object to the clipboard"));
    addAction("copy", action);

    action = KStandardAction::paste(this, SLOT(paste()), this);
    action->setToolTip(i18n("Paste the contents of the clipboard at the cursor"));
    addAction("paste", action);

    action = new QAction(koIcon("special_paste"), i18n("Special Paste..."), this);
    addAction("specialPaste", action);
    connect(action, &QAction::triggered, this, &CellToolBase::specialPaste);
    action->setToolTip(i18n("Paste the contents of the clipboard with special options"));

    action = new QAction(koIcon("insertcellcopy"), i18n("Paste with Insertion"), this);
    addAction("pasteWithInsertion", action);
    connect(action, &QAction::triggered, this, &CellToolBase::pasteWithInsertion);
    action->setToolTip(i18n("Inserts a cell from the clipboard into the spreadsheet"));

    action = KStandardAction::selectAll(this, SLOT(selectAll()), this);
    action->setToolTip(i18n("Selects all cells in the current sheet"));
    addAction("selectAll", action);

    action = KStandardAction::find(this, SLOT(find()), this);
    addAction("edit_find", action);

    action = KStandardAction::findNext(this, SLOT(findNext()), this);
    addAction("edit_find_next", action);

    action = KStandardAction::findPrev(this, SLOT(findPrevious()), this);
    addAction("edit_find_last", action);

    action = KStandardAction::replace(this, SLOT(replace()), this);
    addAction("edit_replace", action);

    // -- misc actions --

    action = new QAction(koIcon("go-jump"), i18n("Goto Cell..."), this);
    action->setIconText(i18n("Goto"));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    addAction("gotoCell", action);
    connect(action, &QAction::triggered, this, &CellToolBase::gotoCell);
    action->setToolTip(i18n("Move to a particular cell"));

    action = KStandardAction::spelling(this, SLOT(spellCheck()), this);
    action->setToolTip(i18n("Check the spelling"));
    addAction("tools_spelling", action);

    action = new QAction(koIconWanted("not used in UI, but devs might do, so nice to have", "inspector"), i18n("Run Inspector..."), this);
    addAction("inspector", action);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I));
    connect(action, &QAction::triggered, this, &CellToolBase::inspector);

#ifndef NDEBUG
    action = new QAction(koIcon("table"), i18n("Show QTableView..."), this);
    addAction("qTableView", action);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
    connect(action, &QAction::triggered, this, &CellToolBase::qTableView);
#endif

    action = new QAction(i18n("Auto-Format..."), this);
    addAction("sheetFormat", action);
    connect(action, &QAction::triggered, this, &CellToolBase::sheetFormat);
    action->setToolTip(i18n("Set the worksheet formatting"));

    action = new QAction(koIcon("application-vnd.oasis.opendocument.spreadsheet"), i18n("Document Settings..."), this);
    addAction("documentSettingsDialog", action);
    connect(action, &QAction::triggered, this, &CellToolBase::documentSettingsDialog);
    action->setToolTip(i18n("Show document settings dialog"));

    action = new KToggleAction(i18n("Break Before Column"), this);
    addAction("format_break_before_column", action);
    connect(action, &QAction::triggered, this, &CellToolBase::breakBeforeColumn);
    action->setIconText(i18n("Column Break"));
    action->setToolTip(i18n("Set a manual page break before the column"));

    action = new KToggleAction(i18n("Break Before Row"), this);
    addAction("format_break_before_row", action);
    connect(action, &QAction::triggered, this, &CellToolBase::breakBeforeRow);
    action->setIconText(i18n("Row Break"));
    action->setToolTip(i18n("Set a manual page break before the row"));

    // Editor actions:
    // Set up the permutation of the reference fixations action.
    action = new QAction(i18n("Permute reference fixation"), this);
    addAction("permuteFixation", action);
    action->setShortcut(Qt::Key_F4);
    // connect on creation of the embedded editor
    action->setIconText(i18n("Permute fixation"));
    action->setToolTip(i18n("Permute the fixation of the reference at the text cursor"));

    setTextMode(true);

}

CellToolBase::~CellToolBase()
{
    delete d->formulaDialog;
    delete d->popupListChoose;
    delete d->cellEditor;
    delete d->actions;
    qDeleteAll(d->popupMenuActions);
    qDeleteAll(actions());
    delete d;
}

QList<QAction*> CellToolBase::popupMenuActionList() const
{
    return d->popupActionList();
}

void CellToolBase::paint(QPainter &painter, const KoViewConverter &viewConverter)
{
    KoShape::applyConversion(painter, viewConverter);
    painter.translate(offset()); // the table shape offset
    const QRectF paintRect = QRectF(QPointF(), size());

    /* paint the selection */
    d->paintReferenceSelection(painter, paintRect);
    d->paintSelection(painter, paintRect);
}

void CellToolBase::paintReferenceSelection(QPainter &painter, const QRectF &paintRect)
{
    d->paintReferenceSelection(painter, paintRect);
}

void CellToolBase::paintSelection(QPainter &painter, const QRectF &paintRect)
{
    d->paintSelection(painter, paintRect);
}

void CellToolBase::mousePressEvent(KoPointerEvent* event)
{
    KoInteractionTool::mousePressEvent(event);
}

void CellToolBase::mouseMoveEvent(KoPointerEvent* event)
{
    // Special handling for drag'n'drop.
    if (DragAndDropStrategy *const strategy = dynamic_cast<DragAndDropStrategy*>(currentStrategy())) {
        // FIXME Stefan: QDrag*Event and QDropEvent are not handled by KoInteractionTool YET:
        // Cancel the strategy after the drag has started.
        if (strategy->dragStarted()) {
            cancelCurrentStrategy();
        }
        KoInteractionTool::mouseMoveEvent(event);
        return;
    }
    // Indicators are not necessary if there's a strategy.
    if (currentStrategy()) {
        return KoInteractionTool::mouseMoveEvent(event);
    }

    Sheet *const sheet = selection()->activeSheet();

    // Get info about where the event occurred.
    QPointF position = event->point - offset(); // the shape offset, not the scrolling one.

    // Diagonal cursor, if the selection handle was hit.
    if (SelectionStrategy::hitTestReferenceSizeGrip(canvas(), selection(), position) ||
        SelectionStrategy::hitTestSelectionSizeGrip(canvas(), selection(), position)) {
        if (selection()->activeSheet()->layoutDirection() == Qt::RightToLeft) {
            useCursor(Qt::SizeBDiagCursor);
        } else {
            useCursor(Qt::SizeFDiagCursor);
        }
        return KoInteractionTool::mouseMoveEvent(event);
    }

    // Hand cursor, if the selected area was hit.
    if (!selection()->referenceSelectionMode()) {
        const Region::ConstIterator end(selection()->constEnd());
        for (Region::ConstIterator it(selection()->constBegin()); it != end; ++it) {
            const QRect range = (*it)->rect();
            if (sheet->cellCoordinatesToDocument(range).contains(position)) {
                useCursor(Qt::PointingHandCursor);
                return KoInteractionTool::mouseMoveEvent(event);
            }
        }
    }

    // In which cell did the user click?
    qreal xpos;
    qreal ypos;
    const int col = this->selection()->activeSheet()->leftColumn(position.x(), xpos);
    const int row = this->selection()->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col < 1 || row < 1 || col > maxCol() || row > maxRow()) {
        debugSheetsUI << "col or row is out of range:" << "col:" << col << " row:" << row;
    } else {
        const Cell cell = Cell(selection()->activeSheet(), col, row).masterCell();
        SheetView* const sheetView = this->sheetView(selection()->activeSheet());

        QString url;
        const CellView& cellView = sheetView->cellView(col, row);
        if (selection()->activeSheet()->layoutDirection() == Qt::RightToLeft) {
            url = cellView.testAnchor(sheetView, cell, cell.width() - position.x() + xpos, position.y() - ypos);
        } else {
            url = cellView.testAnchor(sheetView, cell, position.x() - xpos, position.y() - ypos);
        }
        if (!url.isEmpty()) {
            useCursor(Qt::PointingHandCursor);
            return KoInteractionTool::mouseMoveEvent(event);
        }
    }

    // Reset to normal cursor.
    useCursor(Qt::ArrowCursor);
    KoInteractionTool::mouseMoveEvent(event);
}

void CellToolBase::mouseReleaseEvent(KoPointerEvent* event)
{
    KoInteractionTool::mouseReleaseEvent(event);
    scrollToCell(selection()->cursor());
}

void CellToolBase::mouseDoubleClickEvent(KoPointerEvent* event)
{
    Q_UNUSED(event)
    cancelCurrentStrategy();
    scrollToCell(selection()->cursor());
    createEditor(false /* keep content */, true, true /*full editing*/);
}

void CellToolBase::keyPressEvent(QKeyEvent* event)
{
     Sheet * const sheet = selection()->activeSheet();
    if (!sheet) {
        return;
    }

    // Don't handle the remaining special keys.
    if (event->modifiers() & (Qt::AltModifier | Qt::ControlModifier) &&
            (event->key() != Qt::Key_Down) &&
            (event->key() != Qt::Key_Up) &&
            (event->key() != Qt::Key_Right) &&
            (event->key() != Qt::Key_Left) &&
            (event->key() != Qt::Key_Home) &&
            (event->key() != Qt::Key_Enter) &&
            (event->key() != Qt::Key_Return)) {
        event->ignore(); // QKeyEvent
        return;
    }

    // Check for formatting key combination CTRL + ...
    // Qt::Key_Exclam, Qt::Key_At, Qt::Key_Ampersand, Qt::Key_Dollar
    // Qt::Key_Percent, Qt::Key_AsciiCircum, Qt::Key_NumberSign
    if (d->formatKeyPress(event)) {
        return;
    }

#if 0
    // TODO move this to the contextMenuEvent of the view.
    // keyPressEvent() is not called with the contextMenuKey,
    // it's handled separately by Qt.
    if (event->key() == KGlobalSettings::contextMenuKey()) {
        int row = d->canvas->selection()->marker().y();
        int col = d->canvas->selection()->marker().x();
        QPointF p(sheet->columnPosition(col), sheet->rowPosition(row));
        d->canvas->view()->openPopupMenu(d->canvas->mapToGlobal(p.toPoint()));
    }
#endif
    switch (event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        d->processEnterKey(event);
        return;
        break;
    case Qt::Key_Down:
    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Tab: /* a tab behaves just like a right/left arrow */
    case Qt::Key_Backtab:  /* and so does Shift+Tab */
        if (event->modifiers() & Qt::ControlModifier) {
            if (!d->processControlArrowKey(event))
                return;
        } else {
            d->processArrowKey(event);
            return;
        }
        break;

    case Qt::Key_Escape:
        d->processEscapeKey(event);
        return;
        break;

    case Qt::Key_Home:
        if (!d->processHomeKey(event))
            return;
        break;

    case Qt::Key_End:
        if (!d->processEndKey(event))
            return;
        break;

    case Qt::Key_PageUp:  /* Page Up */
        if (!d->processPriorKey(event))
            return;
        break;

    case Qt::Key_PageDown:   /* Page Down */
        if (!d->processNextKey(event))
            return;
        break;

    case Qt::Key_Backspace:
    case Qt::Key_Delete:
	clearContents();
	break;

    case Qt::Key_F2:
	edit();
	break;
	
    default:
        d->processOtherKey(event);
        return;
        break;
    }
}

void CellToolBase::inputMethodEvent(QInputMethodEvent * event)
{
    // Send it to the embedded editor.
    if (editor()) {
        QApplication::sendEvent(editor()->widget(), event);
    }
}

void CellToolBase::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    Q_UNUSED(toolActivation);
    Q_UNUSED(shapes);

    if (!d->initialized) {
        init();
        d->initialized = true;
    }

    useCursor(Qt::ArrowCursor);
    

    // paint the selection rectangle
    selection()->update();
    populateWordCollection();
    // Initialize cell style selection action.
    const StyleManager* styleManager = selection()->activeSheet()->fullMap()->styleManager();
    static_cast<KSelectAction*>(this->action("setStyle"))->setItems(styleManager->styleNames());

    // Establish connections.
    connect(selection(), &Selection::changed,
            this, QOverload<const Region &>::of(&CellToolBase::selectionChanged));
    connect(selection(), &Selection::closeEditor,
            this, &CellToolBase::deleteEditor);
    connect(selection(), &Selection::modified,
            this, &CellToolBase::updateEditor);
    connect(selection(), &Selection::activeSheetChanged,
            this, &CellToolBase::activeSheetChanged);
    connect(selection(), &Selection::requestFocusEditor,
            this, &CellToolBase::focusEditorRequested);
    connect(selection(), &Selection::documentReadWriteToggled,
            this, &CellToolBase::documentReadWriteToggled);
    connect(selection(), &Selection::sheetProtectionToggled,
            this, &CellToolBase::sheetProtectionToggled);

    Map *map = selection()->activeSheet()->fullMap();
    connect(map, &MapBase::damagesFlushed, this, &CellToolBase::handleDamages);
}

void CellToolBase::deactivate()
{
    Selection *sel = selection();
    // Disconnect.
    if (sel) disconnect(sel, 0, this, 0);
    // close the cell editor
    deleteEditor(true); // save changes
    // clear the selection rectangle
    if (sel) sel->update();
}

void CellToolBase::addCellAction(CellAction *a)
{
    addAction(a->name(), a->action());
}

void CellToolBase::init()
{
}

QList<QPointer<QWidget> >  CellToolBase::createOptionWidgets()
{
    QList<QPointer<QWidget> > widgets;

    QString xmlName = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "calligrasheets/CellToolOptionWidgets.xml");
    debugSheets << xmlName;
    if (xmlName.isEmpty()) {
        warnSheets << "couldn't find CellToolOptionWidgets.xml file";
        return widgets;
    }

    QFile f(xmlName);
    if (!f.open(QIODevice::ReadOnly)) {
        warnSheets << "couldn't open CellToolOptionWidgets.xml file";
        return widgets;
    }

    QDomDocument doc(QString::fromLatin1("optionWidgets"));
    QString errorMsg;
    int errorLine, errorCol;
    if (!doc.setContent(&f, &errorMsg, &errorLine, &errorCol)) {
        f.close();
        warnSheets << "couldn't parse CellToolOptionWidgets.xml file:" << errorMsg << "on line" << errorLine << "column" << errorCol;
        return widgets;
    }
    f.close();

    QDomNodeList widgetNodes = doc.elementsByTagName("optionWidget");
    for (int i = 0; i < widgetNodes.size(); ++i) {
        QDomElement e = widgetNodes.at(i).toElement();
        widgets.append(new ActionOptionWidget(this, e));
    }

    return widgets;
}

// TODO: while this event sends the offset-adjusted position, the subsequent handleMouseMove calls do not. This means that they all contain the offset adjustment themselves, which is a needless duplication. Should be improved.
KoInteractionStrategy* CellToolBase::createStrategy(KoPointerEvent* event)
{
    Sheet *const sheet = selection()->activeSheet();
    // Get info about where the event occurred.
    QPointF position = event->point - offset(); // the shape offset, not the scrolling one.

    // Autofilling or merging, if the selection handle was hit.
    if (SelectionStrategy::hitTestSelectionSizeGrip(canvas(), selection(), position)) {
        if (event->button() == Qt::LeftButton)
            return new AutoFillStrategy(this, position, event->modifiers());
        else if (event->button() == Qt::MiddleButton)
            return new MergeStrategy(this, position, event->modifiers());
    }

    // Pasting with the middle mouse button.
    if (event->button() == Qt::MiddleButton) {
        return new PasteStrategy(this, position, event->modifiers());
    }

    // Check, if the selected area was hit.
    bool hitSelection = false;
    Region::ConstIterator end = selection()->constEnd();
    for (Region::ConstIterator it = selection()->constBegin(); it != end; ++it) {
        const QRect range = (*it)->rect();
        if (sheet->cellCoordinatesToDocument(range).contains(position)) {
            // Context menu with the right mouse button.
            if (event->button() == Qt::RightButton) {
                // Setup the context menu.
                setPopupActionList(d->popupActionList());
                event->ignore();
                return nullptr; // Act directly; no further strategy needed.
            }
            hitSelection = true;
            break;
        }
    }

    // In which cell did the user click?
    qreal xpos;
    qreal ypos;
    const int col = sheet->leftColumn(position.x(), xpos);
    const int row = sheet->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > maxCol() || row > maxRow()) {
        debugSheetsUI << "col or row is out of range:" << "col:" << col << " row:" << row;
    } else {
        // Context menu with the right mouse button.
        if (event->button() == Qt::RightButton) {
            selection()->initialize(QPoint(col, row), sheet);
            // Setup the context menu.
            setPopupActionList(d->popupActionList());
            event->ignore();
            return nullptr; // Act directly; no further strategy needed.
        } else {
            const Cell cell = Cell(sheet, col, row).masterCell();
            SheetView* const sheetView = this->sheetView(sheet);

            // Filter button hit.
            const double offsetX = canvas()->canvasController()->canvasOffsetX();
            const double offsetY = canvas()->canvasController()->canvasOffsetY();
            const QPointF p1 = QPointF(xpos, ypos) - offset(); // the shape offset, not the scrolling one.
            const QSizeF s1(cell.width(), cell.height());
            const QRectF cellRect = canvas()->viewConverter()->documentToView(QRectF(p1, s1));
            const QRect cellViewRect = cellRect.translated(offsetX, offsetY).toRect();
            if (sheetView->cellView(col, row).hitTestFilterButton(cell, cellViewRect, event->pos())) {
                Database database = cell.database();
                FilterPopup::showPopup(canvas()->canvasWidget(), cell, cellViewRect, &database);
                return nullptr; // Act directly; no further strategy needed.
            }

            // Hyperlink hit.
            QString url;
            const CellView& cellView = sheetView->cellView(col, row);
            if (selection()->activeSheet()->layoutDirection() == Qt::RightToLeft) {
                url = cellView.testAnchor(sheetView, cell, cell.width() - position.x() + xpos, position.y() - ypos);
            } else {
                url = cellView.testAnchor(sheetView, cell, position.x() - xpos, position.y() - ypos);
            }
            if (!url.isEmpty()) {
                return new HyperlinkStrategy(this, position,
                                             event->modifiers(), url, cellView.textRect());
            }
        }
    }

    // Do we want to drag and drop the selection?
    bool wantDragDrop = hitSelection;
    if (event->modifiers() & Qt::ControlModifier) wantDragDrop = false;   // no drag&drop if Ctrl is pressed
    QRect selRect = selection()->boundingRect();
    // no drag&drop if it's a single cell, assume they want a selection
    if ((selRect.width() <= 1) && (selRect.height() <= 1)) wantDragDrop = false;
    if (selection()->referenceSelectionMode()) wantDragDrop = false;
    if (wantDragDrop)
        return new DragAndDropStrategy(this, position, event->modifiers());

    return new SelectionStrategy(this, position, event->modifiers());
}

// This makes sure that the action buttons stay updated whenever we change anything.
void CellToolBase::handleDamages()
{
    updateActions();
}

void CellToolBase::selectionChanged(const Region& region)
{
    Q_UNUSED(region);
    if (!d->externalEditor) {
        return;
    }
    // Update the editor, if the reference selection is enabled.
    if (editor() && selection()->referenceSelectionMode()) {
        // First, update the formula expression. This will send a signal with
        // the new expression to the external editor, which does not have focus
        // yet (the canvas has). If it would have, it would also send a signal
        // to inform the embedded editor about a changed text.
        editor()->selectionChanged();
        // Focus the embedded or external editor after updating the expression.
        focusEditorRequested();
        return;
    }

    // State of manual page breaks before columns/rows.
    bool columnBreakChecked = false;
    bool columnBreakEnabled = false;
    bool rowBreakChecked = false;
    bool rowBreakEnabled = false;
    const Region::ConstIterator end(selection()->constEnd());
    for (Region::ConstIterator it = selection()->constBegin(); it != end; ++it) {
        const Sheet *const sheet = dynamic_cast<Sheet *>((*it)->sheet());
        if (!sheet) continue;
        const QRect range = (*it)->rect();
        const int column = range.left();
        const int row = range.top();
        columnBreakChecked |= sheet->columnFormats()->hasPageBreak(column);
        columnBreakEnabled |= (column != 1);
        rowBreakChecked |= sheet->rowFormats()->hasPageBreak(row);
        rowBreakEnabled |= (row != 1);
    }
    action("format_break_before_column")->setChecked(columnBreakChecked);
    action("format_break_before_column")->setEnabled(columnBreakEnabled);
    action("format_break_before_row")->setChecked(rowBreakChecked);
    action("format_break_before_row")->setEnabled(rowBreakEnabled);

    const Cell cell = Cell(selection()->activeSheet(), selection()->cursor());
    if (!cell) {
        return;
    }
    d->updateEditor(cell);

    if (selection()->activeSheet()->isProtected()) {
        const Style style = cell.style();
        if (style.notProtected()) {
            if (selection()->isSingular()) {
                d->setProtectedActionsEnabled(true);
            } else { // more than one cell
                d->setProtectedActionsEnabled(false);
            }
        } else {
            d->setProtectedActionsEnabled(false);
        }
    }

    updateActions();
}

void CellToolBase::scrollToCell(const QPoint &location)
{
    Sheet *const sheet = selection()->activeSheet();

    // Adjust the maximum accessed column and row for the scrollbars.
    sheetView(sheet)->updateAccessedCellRange(location);

    // The cell geometry expanded by some pixels in each direction.
    const Cell cell = Cell(sheet, location).masterCell();
    const double xpos = sheet->columnPosition(cell.cellPosition().x());
    const double ypos = sheet->rowPosition(cell.cellPosition().y());
    const double pixelWidth = canvas()->viewConverter()->viewToDocumentX(1);
    const double pixelHeight = canvas()->viewConverter()->viewToDocumentY(1);
    QRectF rect(xpos, ypos, cell.width(), cell.height());
    rect.adjust(-2*pixelWidth, -2*pixelHeight, +2*pixelWidth, +2*pixelHeight);
    rect = rect & QRectF(QPointF(0.0, 0.0), sheet->documentSize());

    // Scroll to cell.
    canvas()->canvasController()->ensureVisible(canvas()->viewConverter()->documentToView(rect), true);
}

CellEditorBase* CellToolBase::editor() const
{
    return d->cellEditor;
}

void CellToolBase::setLastEditorWithFocus(Editor editor)
{
    d->lastEditorWithFocus = editor;
}

double CellToolBase::canvasOffsetX() const
{
    return canvas()->viewConverter()->viewToDocumentX(canvas()->canvasController()->canvasOffsetX());
}

double CellToolBase::canvasOffsetY() const
{
    return canvas()->viewConverter()->viewToDocumentX(canvas()->canvasController()->canvasOffsetY());
}

double CellToolBase::canvasWidth() const
{
    return canvas()->viewConverter()->viewToDocumentX(canvas()->canvasWidget()->width());
}

bool CellToolBase::createEditor(bool clear, bool focus, bool captureArrows)
{
    const Cell cell(selection()->activeSheet(), selection()->marker());
    if (selection()->activeSheet()->isProtected() && !cell.style().notProtected())
        return false;

    if (!editor()) {
        d->cellEditor = new CellEditor(this, d->wordCollection,canvas()->canvasWidget());
        d->cellEditor->setEditorFont(cell.style().font(), true, canvas()->viewConverter());
        connect(action("permuteFixation"), &QAction::triggered,
                d->cellEditor, &CellEditor::permuteFixation);

        if(d->externalEditor) {
            connect(d->cellEditor, &CellEditor::textModified,
                    d->externalEditor, &ExternalEditor::setText);
            connect(d->externalEditor, &ExternalEditor::textModified,
                    d->cellEditor, [this] (const QString &text) {
                        d->cellEditor->setText(text);
                    });
            d->externalEditor->applyAction()->setEnabled(true);
            d->externalEditor->cancelAction()->setEnabled(true);
        }
        
        double w = cell.width();
        double h = cell.height();
        double min_w = cell.width();
        double min_h = cell.height();

        double xpos = selection()->activeSheet()->columnPosition(selection()->marker().x());
        xpos += canvasOffsetX();
        Qt::LayoutDirection sheetDir = selection()->activeSheet()->layoutDirection();
        bool rtlText = cell.displayText().isRightToLeft();

        // if sheet and cell direction don't match, then the editor's location
        // needs to be shifted backwards so that it's right above the cell's text
        if (w > 0 && ((sheetDir == Qt::RightToLeft && !rtlText) ||
                      (sheetDir == Qt::LeftToRight && rtlText)))
            xpos -= w - min_w;

        // paint editor above correct cell if sheet direction is RTL
        if (sheetDir == Qt::RightToLeft) {
            double dwidth = canvasWidth();
            double w2 = qMax(w, min_w);
            xpos = dwidth - w2 - xpos;
        }
        double ypos = selection()->activeSheet()->rowPosition(selection()->marker().y());
        ypos += canvasOffsetY();

        // Setup the editor's palette.
        const Style style = cell.effectiveStyle();
        QPalette editorPalette(d->cellEditor->palette());
        QColor color = style.fontColor();
        if (!color.isValid())
            color = canvas()->canvasWidget()->palette().text().color();
        editorPalette.setColor(QPalette::Text, color);
        color = style.backgroundColor();
        if (!color.isValid())
            color = editorPalette.base().color();
        editorPalette.setColor(QPalette::Background, color);
        d->cellEditor->setPalette(editorPalette);

        // apply (table shape) offset
        xpos += offset().x();
        ypos += offset().y();

        const QRectF rect(xpos + 0.5, ypos + 0.5, w - 0.5, h - 0.5); //needed to circumvent rounding issue with height/width
        const QRectF zoomedRect = canvas()->viewConverter()->documentToView(rect);
        d->cellEditor->setGeometry(zoomedRect.toRect().adjusted(1, 1, -1, -1));
        d->cellEditor->setMinimumSize(QSize((int)canvas()->viewConverter()->documentToViewX(min_w) - 1,
                                       (int)canvas()->viewConverter()->documentToViewY(min_h) - 1));
        d->cellEditor->show();

        // Laurent 2001-12-05
        // Don't add focus when we create a new editor and
        // we select text in edit widget otherwise we don't delete
        // selected text.
        if (focus)
            d->cellEditor->setFocus();

        // clear the selection rectangle
        selection()->update();
    }

    d->cellEditor->setCaptureArrowKeys(captureArrows);

    if (!clear && !cell.isNull()) {
        d->cellEditor->setText(cell.userInput());
        // place cursor at the end
        int pos = d->cellEditor->toPlainText().length();
        d->cellEditor->setCursorPosition(pos);
        if (d->externalEditor) d->externalEditor->setCursorPosition(pos);
    }
    return true;
}

void CellToolBase::populateWordCollection()
{
  const CellStorage* cellstore=selection()->activeSheet()->fullCellStorage();
  int lastrow=cellstore->rows();
  int lastcolumn=cellstore->columns();
  if( lastrow < 2000 && lastcolumn < 20) {
  for (int j=1 ; j <= lastcolumn ; j++) {
    for (int i=1; i<=lastrow ; i++) {
      Value val=Cell( selection()->activeSheet(), j, i).value();
      if(val.isString()) {
          QString value = val.asString();
	if(!d->wordCollection.values(j).contains(value)){
	    d->wordCollection.insertMulti(j, value);
	  }
      }	 
    }
  }
  }
}

void CellToolBase::deleteEditor(bool saveChanges, bool expandMatrix)
{
    if (!d->cellEditor) {
        return;
    }
    const QString userInput = d->cellEditor->toPlainText();
    d->cellEditor->hide();
    // Delete the cell editor first and after that update the document.
    // That means we get a synchronous repaint after the cell editor
    // widget is gone. Otherwise we may get painting errors.
    delete d->cellEditor;
    d->cellEditor = 0;

    delete d->formulaDialog;
    d->formulaDialog = 0;

    if (saveChanges) {
        applyUserInput(userInput, expandMatrix);
    } else {
        selection()->update();
    }
    if (d->externalEditor) {
        d->externalEditor->applyAction()->setEnabled(false);
        d->externalEditor->cancelAction()->setEnabled(false);
    }
    canvas()->canvasWidget()->setFocus();
}

void CellToolBase::activeSheetChanged(Sheet* sheet)
{
#ifdef NDEBUG
    Q_UNUSED(sheet);
#else
    Q_ASSERT(selection()->activeSheet() == sheet);
#endif
    populateWordCollection();
    if (!selection()->referenceSelectionMode()) {
        return;
    }
    if (editor()) {
        if (selection()->originSheet() != selection()->activeSheet()) {
            editor()->widget()->hide();
        } else {
            editor()->widget()->show();
        }
    }
    focusEditorRequested();
}

void CellToolBase::updateEditor()
{
    if (!d->externalEditor) {
        return;
    }
    const Cell cell = Cell(selection()->activeSheet(), selection()->cursor());
    if (!cell) {
        return;
    }
    d->updateEditor(cell);
}

void CellToolBase::focusEditorRequested()
{
    // Nothing to do, if not in editing mode.
    if (!editor()) {
        return;
    }
    // If we are in editing mode, we redirect the focus to the CellEditor or ExternalEditor.
    // This screws up <Tab> though (David)
    if (selection()->originSheet() != selection()->activeSheet()) {
        // Always focus the external editor, if not on the origin sheet.
        if (d->externalEditor) d->externalEditor->setFocus();
    } else {
        // Focus the last active editor, if on the origin sheet.
        if (d->lastEditorWithFocus == EmbeddedEditor) {
            editor()->widget()->setFocus();
        } else {
            if (d->externalEditor) d->externalEditor->setFocus();
        }
    }
}

void CellToolBase::applyUserInput(const QString &userInput, bool expandMatrix)
{
    QString text = userInput;
    if (!text.isEmpty() && text.at(0) == '=') {
        //a formula
        int openParenthese = text.count('(');
        int closeParenthese = text.count(')');
        int diff = qAbs(openParenthese - closeParenthese);
        if (openParenthese > closeParenthese) {
            for (int i = 0; i < diff; ++i) {
                text += ')';
            }
        }
    }
    DataManipulator* command = new DataManipulator();
    command->setSheet(selection()->activeSheet());
    command->setValue(Value(text));
    command->setParsing(true);
    command->setExpandMatrix(expandMatrix);
    command->add(expandMatrix ? *selection() : Region(selection()->cursor(), selection()->activeSheet()));
    command->execute(canvas());

    if (expandMatrix && selection()->isSingular())
        selection()->initialize(*command);
}

void CellToolBase::documentReadWriteToggled(bool readWrite)
{
    d->setProtectedActionsEnabled(readWrite);

    updateActions();
}

void CellToolBase::sheetProtectionToggled(bool protect)
{
    d->setProtectedActionsEnabled(!protect);

    updateActions();
}

void CellToolBase::updateActions()
{
    Sheet *sheet = selection()->activeSheet();
    bool readWrite = sheet->fullMap()->doc()->isReadWrite();
    const Cell cell = Cell(sheet, selection()->cursor());
    d->updateActions(cell);  // TODO - eventuaully get rid of this one
    d->actions->updateOnChange(readWrite, selection(), cell);
}

void CellToolBase::triggerAction(const QString &name)
{
    CellAction *a = d->actions->cellAction(name);
    if (a)
        a->trigger();
    else
        KMessageBox::sorry(canvas()->canvasWidget(), i18n("Unable to locate action %1", name));
}

QColor CellToolBase::selectedBorderColor() const
{
    return d->borderColor;
}

void CellToolBase::cellStyle()
{
    LayoutDialog *dialog = new LayoutDialog(canvas()->canvasWidget(), selection()->activeSheet(), nullptr, false);
    QRect range = selection()->firstRange();
    CellStorage *cs = selection()->activeSheet()->fullCellStorage();
    Style style = cs->style(range);
    bool multicell = ((range.width() > 1) || (range.height() > 1));
    dialog->setStyle(style, multicell);
    if (dialog->exec()) {
        // TODO - this needs a macro command as there are non-style options in that dialog, see LayoutDialog::slotApply
        Style style = dialog->style(multicell);
        StyleCommand* command = new StyleCommand();
        command->setSheet(selection()->activeSheet());
        command->add(*selection());
        command->setStyle(style);
        command->execute(canvas());
    }
    delete dialog;
}

void CellToolBase::styleDialog()
{
    Map* const map = selection()->activeSheet()->fullMap();
    StyleManager* const styleManager = map->styleManager();
    QPointer<StyleManagerDialog> dialog = new StyleManagerDialog(canvas()->canvasWidget(), selection(), styleManager);
    dialog->exec();
    delete dialog;

    static_cast<KSelectAction*>(action("setStyle"))->setItems(styleManager->styleNames());
    if (selection()->activeSheet())
        map->addDamage(new CellDamage(selection()->activeSheet(), Region(1, 1, maxCol(), maxRow()), CellDamage::Appearance));
    canvas()->canvasWidget()->update();
}

void CellToolBase::setStyle(const QString& stylename)
{
    debugSheets << "CellToolBase::setStyle(" << stylename << ")";
    if (selection()->activeSheet()->fullMap()->styleManager()->style(stylename)) {
        StyleCommand* command = new StyleCommand();
        command->setSheet(selection()->activeSheet());
        Style s;
        s.setParentName(stylename);
        command->setStyle(s);
        command->add(*selection());
        command->execute(canvas());
    }
}

void CellToolBase::createStyleFromCell()
{
    QPoint p(selection()->marker());
    Cell cell = Cell(selection()->activeSheet(), p.x(), p.y());

    bool ok = false;
    QString styleName("");

    while (true) {
        styleName = QInputDialog::getText(canvas()->canvasWidget(),
                                          i18n("Create Style From Cell"),
                                          i18n("Enter name:"), QLineEdit::Normal, styleName, &ok);

        if (!ok) // User pushed an OK button.
            return;

        styleName = styleName.trimmed();

        if (styleName.length() < 1) {
            KMessageBox::sorry(canvas()->canvasWidget(), i18n("The style name cannot be empty."));
            continue;
        }

        if (selection()->activeSheet()->fullMap()->styleManager()->style(styleName) != 0) {
            KMessageBox::sorry(canvas()->canvasWidget(), i18n("A style with this name already exists."));
            continue;
        }
        break;
    }

    const Style cellStyle = cell.style();
    CustomStyle*  style = new CustomStyle(styleName);
    style->merge(cellStyle);

    selection()->activeSheet()->fullMap()->styleManager()->insertStyle(style);
    cell.setStyle(*style);
    QStringList functionList(static_cast<KSelectAction*>(action("setStyle"))->items());
    functionList.push_back(styleName);
    static_cast<KSelectAction*>(action("setStyle"))->setItems(functionList);
}

void CellToolBase::font(const QString& font)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(kundo2_i18n("Change Font"));
    Style s;
    s.setFontFamily(font.toLatin1());
    command->setStyle(s);
    command->add(*selection());
    command->execute(canvas());
    // Don't leave the focus in the toolbars combo box ...
    if (editor()) {
        const Style style = Cell(selection()->activeSheet(), selection()->marker()).style();
        editor()->setEditorFont(style.font(), true, canvas()->viewConverter());
        focusEditorRequested();
    } else {
        canvas()->canvasWidget()->setFocus();
    }
}

void CellToolBase::fontSize(int size)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(kundo2_i18n("Change Font"));
    Style s;
    s.setFontSize(size);
    command->setStyle(s);
    command->add(*selection());
    command->execute(canvas());
    // Don't leave the focus in the toolbars combo box ...
    if (editor()) {
        const Cell cell(selection()->activeSheet(), selection()->marker());
        editor()->setEditorFont(cell.style().font(), true, canvas()->viewConverter());
        focusEditorRequested();
    } else {
        canvas()->canvasWidget()->setFocus();
    }
}

void CellToolBase::changeTextColor(const KoColor &color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(kundo2_i18n("Change Text Color"));
    Style s;
    s.setFontColor(color.toQColor());
    command->setStyle(s);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::alignLeft(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(kundo2_i18n("Change Horizontal Alignment"));
    Style s;
    s.setHAlign(enable ? Style::Left : Style::HAlignUndefined);
    command->setStyle(s);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::alignRight(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(kundo2_i18n("Change Horizontal Alignment"));
    Style s;
    s.setHAlign(enable ? Style::Right : Style::HAlignUndefined);
    command->setStyle(s);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::alignCenter(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(kundo2_i18n("Change Horizontal Alignment"));
    Style s;
    s.setHAlign(enable ? Style::Center : Style::HAlignUndefined);
    command->setStyle(s);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::alignTop(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(kundo2_i18n("Change Vertical Alignment"));
    Style s;
    s.setVAlign(enable ? Style::Top : Style::VAlignUndefined);
    command->setStyle(s);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::alignBottom(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(kundo2_i18n("Change Vertical Alignment"));
    Style s;
    s.setVAlign(enable ? Style::Bottom : Style::VAlignUndefined);
    command->setStyle(s);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::alignMiddle(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(kundo2_i18n("Change Vertical Alignment"));
    Style s;
    s.setVAlign(enable ? Style::Middle : Style::VAlignUndefined);
    command->setStyle(s);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::borderColor(const KoColor &color)
{
    BorderColorCommand* command = new BorderColorCommand();
    command->setSheet(selection()->activeSheet());
    QColor c = color.toQColor();
    d->borderColor = c;
    command->setColor(c);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::changeAngle()
{
    QPointer<AngleDialog> dialog = new AngleDialog(canvas()->canvasWidget(), selection());
    dialog->exec();
    delete dialog;
}

void CellToolBase::increasePrecision()
{
    PrecisionCommand* command = new PrecisionCommand();
    command->setSheet(selection()->activeSheet());
    command->add(*selection());
    if (!command->execute())
        delete command;
}

void CellToolBase::decreasePrecision()
{
    PrecisionCommand* command = new PrecisionCommand();
    command->setSheet(selection()->activeSheet());
    command->setDecrease(true);
    command->add(*selection());
    if (!command->execute())
        delete command;
}

void CellToolBase::changeBackgroundColor(const KoColor &color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(kundo2_i18n("Change Background Color"));
    Style s;
    s.setBackgroundColor(color.toQColor());
    command->setStyle(s);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::insertCells()
{
    QPointer<InsertDialog> dialog = new InsertDialog(canvas()->canvasWidget(), selection(), InsertDialog::Insert);
    dialog->exec();
    delete dialog;
}

void CellToolBase::deleteCells()
{
    QPointer<InsertDialog> dialog = new InsertDialog(canvas()->canvasWidget(), selection(), InsertDialog::Remove);
    dialog->exec();
    delete dialog;
}

void CellToolBase::clearAll()
{
    DeleteCommand* command = new DeleteCommand();
    command->setSheet(selection()->activeSheet());
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::clearContents()
{
    // TODO Stefan: Actually this check belongs into the command!
    if (selection()->activeSheet()->areaIsEmpty(*selection()))
        return;

    DataManipulator* command = new DataManipulator();
    command->setSheet(selection()->activeSheet());
    command->setText(kundo2_i18n("Clear Text"));
    // parsing gets set only so that parseUserInput is called as it should be,
    // no actual parsing shall be done
    command->setParsing(true);
    command->setValue(Value(""));
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::conditional()
{
    QPointer<ConditionalDialog> dialog = new ConditionalDialog(canvas()->canvasWidget(), selection());
    dialog->exec();
    delete dialog;
}

void CellToolBase::clearConditionalStyles()
{
    // TODO Stefan: Actually this check belongs into the command!
    if (selection()->activeSheet()->areaIsEmpty(*selection(), Sheet::ConditionalCellAttribute))
        return;

    ConditionCommand* command = new ConditionCommand();
    command->setSheet(selection()->activeSheet());
    command->setConditionList(QLinkedList<Conditional>());
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::autoFilter()
{
    AutoFilterCommand* command = new AutoFilterCommand();
    command->setSheet(selection()->activeSheet());
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::insertFormula()
{
    if (! d->formulaDialog) {
        if (! createEditor())
            return;
        d->formulaDialog = new FormulaDialog(canvas()->canvasWidget(), selection(), editor());
    }
    d->formulaDialog->show(); // dialog deletes itself later
}

void CellToolBase::insertFromDatabase()
{
#ifndef QT_NO_SQL
    selection()->emitCloseEditor(true);

    QStringList str = QSqlDatabase::drivers();
    if (str.isEmpty()) {
        KMessageBox::error(canvas()->canvasWidget(), i18n("No database drivers available. To use this feature you need "
                           "to install the necessary Qt database drivers."));
        return;
    }

    QPointer<DatabaseDialog> dialog = new DatabaseDialog(canvas()->canvasWidget(), selection());
    dialog->exec();
    delete dialog;
#endif
}

void CellToolBase::sortList()
{
    QPointer<ListDialog> dialog = new ListDialog(canvas()->canvasWidget());
    dialog->exec();
    delete dialog;
}

void CellToolBase::consolidate()
{
    selection()->emitCloseEditor(true);
    ConsolidateDialog * dialog = new ConsolidateDialog(canvas()->canvasWidget(), selection());
    dialog->show(); // dialog deletes itself later
}

void CellToolBase::goalSeek()
{
    selection()->emitCloseEditor(true);

    GoalSeekDialog* dialog = new GoalSeekDialog(canvas()->canvasWidget(), selection());
    dialog->show(); // dialog deletes itself later
}

void CellToolBase::subtotals()
{
    if ((selection()->lastRange().width() < 2) || (selection()->lastRange().height() < 2)) {
        KMessageBox::error(canvas()->canvasWidget(), i18n("You must select multiple cells."));
        return;
    }

    QPointer<SubtotalDialog> dialog = new SubtotalDialog(canvas()->canvasWidget(), selection());
    dialog->exec();
    delete dialog;
}

void CellToolBase::pivot()
{
    if ((selection()->lastRange().width() < 2) || (selection()->lastRange().height() < 2)) {
        KMessageBox::error(canvas()->canvasWidget(), i18n("You must select multiple cells."));
        return;
    }

    QPointer<Pivot> dialog = new Pivot(canvas()->canvasWidget(), selection());
    dialog->exec();
    delete dialog;
}

void CellToolBase::setAreaName()
{
    QPointer<AddNamedAreaDialog> dialog = new AddNamedAreaDialog(canvas()->canvasWidget(), selection());
    dialog->exec();
    delete dialog;
}

void CellToolBase::namedAreaDialog()
{
    QPointer<NamedAreaDialog> dialog = new NamedAreaDialog(canvas()->canvasWidget(), selection());
    dialog->exec();
    delete dialog;
}

void CellToolBase::formulaSelection(const QString& expression)
{
    if (expression == i18n("Others...")) {
        insertFormula();
        return;
    }

    createEditor();
    FormulaDialog* dialog = new FormulaDialog(canvas()->canvasWidget(), selection(), editor(), expression);
    dialog->show(); // dialog deletes itself later
}

void CellToolBase::edit()
{
    // Not yet in edit mode?
    if (!editor()) {
        createEditor(false /* keep content */, true, true /*capture arrow keys*/);
        return;
    }

    // If the editor doesn't allow cursor movement, enable it now (enters real editing mode)
    if (!editor()->captureArrowKeys()) {
        editor()->setCaptureArrowKeys(true);
        return;
    }

    // Switch focus.
    if (editor()->widget()->hasFocus()) {
        if (d->externalEditor) d->externalEditor->setFocus();
    } else {
        editor()->widget()->setFocus();
    }
}

void CellToolBase::cut()
{
    if (editor()) {
        editor()->cut();
        selection()->emitModified();
        return;
    }

    QString snippet = CopyCommand::saveAsSnippet(*selection());
    snippet = "CUT\n" + snippet;

    QMimeData* mimeData = new QMimeData();
    mimeData->setText(CopyCommand::saveAsPlainText(*selection()));
    mimeData->setData("application/x-calligra-sheets-snippet", snippet.toUtf8());

    QApplication::clipboard()->setMimeData(mimeData);
}

void CellToolBase::copy() const
{
    Selection* selection = const_cast<CellToolBase*>(this)->selection();
    if (editor()) {
        editor()->copy();
        return;
    }

    QString snippet = CopyCommand::saveAsSnippet(*selection);

    QMimeData* mimeData = new QMimeData();
    mimeData->setText(CopyCommand::saveAsPlainText(*selection));
    mimeData->setData("application/x-calligra-sheets-snippet", snippet.toUtf8());

    QApplication::clipboard()->setMimeData(mimeData);
}

bool CellToolBase::paste()
{
    if (!selection()->activeSheet()->fullMap()->isReadWrite()) // don't paste into a read only document
        return false;

    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData(QClipboard::Clipboard);

    if (mimeData->hasFormat("application/vnd.oasis.opendocument.spreadsheet")) {
        QByteArray returnedTypeMime = "application/vnd.oasis.opendocument.spreadsheet";
        QByteArray arr = mimeData->data(returnedTypeMime);
        if (arr.isEmpty())
            return false;
        QBuffer buffer(&arr);
        Map *map = selection()->activeSheet()->fullMap();
        if (!Odf::paste(buffer, map)) return false;
    }

    if (!editor()) {
        const QMimeData* mimedata = QApplication::clipboard()->mimeData();
        if (!mimedata->hasFormat("application/x-calligra-sheets-snippet") &&
            !mimedata->hasHtml() && mimedata->hasText() &&
            mimeData->text().split('\n').count() >= 2 )
        {
            triggerAction("insertFromClipboard");
        } else {
            //debugSheetsUI <<"Pasting. Rect=" << selection()->lastRange() <<" bytes";
            PasteCommand *const command = new PasteCommand();
            command->setSheet(selection()->activeSheet());
            command->add(*selection());
            command->setMimeData(mimedata, clipboard->ownsClipboard());
            command->setPasteFC(true);
            command->execute(canvas());
        }
        d->updateEditor(Cell(selection()->activeSheet(), selection()->cursor()));
    } else {
        editor()->paste();
    }
    selection()->emitModified();
    return true;
}

void CellToolBase::specialPaste()
{
    QPointer<SpecialPasteDialog> dialog = new SpecialPasteDialog(canvas()->canvasWidget(), selection());
    if (dialog->exec()) {
        selection()->emitModified();
    }
    delete dialog;
}

void CellToolBase::pasteWithInsertion()
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *const mimeData = clipboard->mimeData();

    PasteCommand *const command = new PasteCommand();
    command->setSheet(selection()->activeSheet());
    command->add(*selection());
    command->setMimeData(mimeData, clipboard->ownsClipboard());   // this sets the source, if applicable


    // First the insertion, then the actual pasting
    int shiftMode = -1;   // 0 = right, 1 = down

    if (command->unknownShiftDirection()) {
        QPointer<PasteInsertDialog> dialog= new PasteInsertDialog(canvas()->canvasWidget(), selection());
        int res = dialog->exec();
        if (dialog->checkedRight())
            shiftMode = 0;   // right
        else
            shiftMode = 1;   // down
        delete dialog;
        if (res != QDialog::Accepted) {
            delete command;
            return;
        }
    } else {
        // Determine the shift direction, if needed.
        if (command->isColumnSelected())
            shiftMode = 0;   // right
        else
            shiftMode = 1;   // down
    }

    // The paste command has now generated the correct areas for us, so let's perform the insertion.
    for (Region::ConstIterator it = command->constBegin(); it != command->constEnd(); ++it) {
        QRect moverect = (*it)->rect();
        SheetBase *movesheet = (*it)->sheet();
        AbstractRegionCommand *shiftCommand = nullptr;

        // Shift cells down.
        if (shiftMode == 1) {
            if (moverect.width() >= KS_colMax) {
                // Rows present.
                shiftCommand = new InsertDeleteRowManipulator();
            } else {
                ShiftManipulator *const command = new ShiftManipulator();
                command->setDirection(ShiftManipulator::ShiftBottom);
                shiftCommand = command;
            }
        }
        // Shift cells right.
        if (shiftMode == 0) {
            if (moverect.height() >= KS_rowMax) {
                // Columns present.
                shiftCommand = new InsertDeleteColumnManipulator();
            } else {
                // Neither columns, nor rows present.
                ShiftManipulator *const command = new ShiftManipulator();
                command->setDirection(ShiftManipulator::ShiftRight);
                shiftCommand = command;
            }
        }

        // shift the data
        if (shiftCommand) {
            shiftCommand->setSheet(dynamic_cast<Sheet *>(movesheet));
            shiftCommand->add(Region(moverect, movesheet));

            shiftCommand->execute(canvas());
        }
    }

    // And now we can actually execute the paste command.
    // TODO - this works, but undo is separate for cell shift and actual paste ... maybe group these?
    command->execute(canvas());

    d->updateEditor(Cell(selection()->activeSheet(), selection()->cursor()));
}

void CellToolBase::deleteSelection()
{
    clearContents();
}

void CellToolBase::selectAll()
{
    selection()->selectAll();
}

void CellToolBase::find()
{
    QPointer<FindDlg> dialog = new FindDlg(canvas()->canvasWidget(), "Find", d->findOptions, d->findStrings);
    dialog->setHasSelection(!selection()->isSingular());
    dialog->setHasCursor(true);
    if (KFindDialog::Accepted != dialog->exec())
        return;

    // Save for next time
    d->findOptions = dialog->options();
    d->findStrings = dialog->findHistory();
    d->typeValue = dialog->searchType();
    d->directionValue = dialog->searchDirection();

    // Create the KFind object
    delete d->find;
    delete d->replace;
    d->find = new KFind(dialog->pattern(), dialog->options(), canvas()->canvasWidget());
    d->replace = 0;
    d->replaceCommand = 0;

    d->searchInSheets.currentSheet = selection()->activeSheet();
    d->searchInSheets.firstSheet = d->searchInSheets.currentSheet;

    initFindReplace();
    findNext();
    delete dialog;
}

// Initialize a find or replace operation, using d->find or d->replace,
// and d->findOptions.
void CellToolBase::initFindReplace()
{
    KFind* findObj = d->find ? d->find : d->replace;
    Q_ASSERT(findObj);
    connect(findObj, QOverload<const QString &, int, int>::of(&KFind::highlight),
            this, &CellToolBase::slotHighlight);
    connect(findObj, &KFind::findNext,
            this, &CellToolBase::findNext);

    bool bck = d->findOptions & KFind::FindBackwards;
    Sheet* currentSheet = d->searchInSheets.currentSheet;

    QRect region = (d->findOptions & KFind::SelectedText)
                   ? selection()->lastRange()
                   : QRect(1, 1, currentSheet->fullCellStorage()->columns(), currentSheet->fullCellStorage()->rows()); // All cells

    int colStart = !bck ? region.left() : region.right();
    int colEnd = !bck ? region.right() : region.left();
    int rowStart = !bck ? region.top() : region.bottom();
    int rowEnd = !bck ? region.bottom() : region.top();

    d->findLeftColumn = region.left();
    d->findRightColumn = region.right();
    d->findTopRow = region.top();
    d->findBottomRow = region.bottom();

    d->findStart = QPoint(colStart, rowStart);
    d->findPos = (d->findOptions & KFind::FromCursor) ? selection()->marker() : d->findStart;
    d->findEnd = QPoint(colEnd, rowEnd);
    //debugSheets << d->findPos <<" to" << d->findEnd;
    //debugSheets <<"leftcol=" << d->findLeftColumn <<" rightcol=" << d->findRightColumn;
}

void CellToolBase::findNext()
{
    KFind* findObj = d->find ? d->find : d->replace;
    if (!findObj)  {
        find();
        return;
    }
    KFind::Result res = KFind::NoMatch;
    Cell cell = findNextCell();
    bool forw = !(d->findOptions & KFind::FindBackwards);
    while (res == KFind::NoMatch && !cell.isNull()) {
        if (findObj->needData()) {
            if (d->typeValue == FindOption::Note)
                findObj->setData(cell.comment());
            else
                findObj->setData(cell.userInput());
            d->findPos = QPoint(cell.column(), cell.row());
            //debugSheets <<"setData(cell" << d->findPos << ')';
        }

        // Let KFind inspect the text fragment, and display a dialog if a match is found
        if (d->find)
            res = d->find->find();
        else
            res = d->replace->replace();

        if (res == KFind::NoMatch)  {
            // Go to next cell, skipping unwanted cells
            if (d->directionValue == FindOption::Row) {
                if (forw)
                    ++d->findPos.rx();
                else
                    --d->findPos.rx();
            } else {
                if (forw)
                    ++d->findPos.ry();
                else
                    --d->findPos.ry();
            }
            cell = findNextCell();
        }
    }

    if (res == KFind::NoMatch) {
        //emitUndoRedo();
        //removeHighlight();
        if (findObj->shouldRestart()) {
            d->findOptions &= ~KFind::FromCursor;
            d->findPos = d->findStart;
            findObj->resetCounts();
            findNext();
        } else { // done, close the 'find next' dialog
            if (d->find)
                d->find->closeFindNextDialog();
            else {
                canvas()->addCommand(d->replaceCommand);
                d->replaceCommand = 0;
                d->replace->closeReplaceNextDialog();
            }
        }
    }
    else if (!cell.isNull()) {
        // move to the cell
        Sheet *sheet = dynamic_cast<Sheet *>(cell.sheet());
        if (sheet != selection()->activeSheet())
            selection()->emitVisibleSheetRequested(sheet);
        selection()->initialize (Region (cell.column(), cell.row(), cell.sheet()), sheet);
        scrollToCell (selection()->cursor());
    }
}

Cell CellToolBase::nextFindValidCell(int col, int row)
{
    Cell cell = Cell(d->searchInSheets.currentSheet, col, row);
    if (cell.isDefault() || cell.isPartOfMerged() || cell.isFormula())
        cell = Cell();
    if (d->typeValue == FindOption::Note && !cell.isNull() && cell.comment().isEmpty())
        cell = Cell();
    return cell;
}

Cell CellToolBase::findNextCell()
{
    // cellStorage()->firstInRow / cellStorage()->nextInRow would be faster at doing that,
    // but it doesn't seem to be easy to combine it with 'start a column d->find.x()'...

    Sheet* sheet = d->searchInSheets.currentSheet;
    Cell cell;
    bool forw = !(d->findOptions & KFind::FindBackwards);
    int col = d->findPos.x();
    int row = d->findPos.y();
    int maxRow = sheet->fullCellStorage()->rows();
//     warnSheets <<"findNextCell starting at" << col << ',' << row <<"   forw=" << forw;

    if (d->directionValue == FindOption::Row) {
        while (!cell && (row >= d->findTopRow) && (row <= d->findBottomRow) && (forw ? row <= maxRow : row >= 0)) {
            while (!cell && (forw ? col <= d->findRightColumn : col >= d->findLeftColumn)) {
                cell = nextFindValidCell(col, row);
                if (forw) ++col;
                else --col;
            }
            if (!cell.isNull())
                break;
            // Prepare looking in the next row
            if (forw)  {
                col = d->findLeftColumn;
                ++row;
            } else {
                col = d->findRightColumn;
                --row;
            }
            //warnSheets <<"next row:" << col << ',' << row;
        }
    } else {
        while (!cell && (forw ? col <= d->findRightColumn : col >= d->findLeftColumn)) {
            while (!cell && (row >= d->findTopRow) && (row <= d->findBottomRow) && (forw ? row <= maxRow : row >= 0)) {
                cell = nextFindValidCell(col, row);
                if (forw) ++row;
                else --row;
            }
            if (!cell.isNull())
                break;
            // Prepare looking in the next col
            if (forw)  {
                row = d->findTopRow;
                ++col;
            } else {
                row = d->findBottomRow;
                --col;
            }
            //debugSheets <<"next row:" << col << ',' << row;
        }
    }
    // if (!cell)
    // No more next cell - TODO go to next sheet (if not looking in a selection)
    // (and make d->findEnd(max, max) in that case...)
//    if (cell.isNull()) warnSheets<<"returning null"<<endl;
//    else warnSheets <<" returning" << cell;

    return cell;
}

void CellToolBase::findPrevious()
{
    KFind* findObj = d->find ? d->find : d->replace;
    if (!findObj)  {
        find();
        return;
    }
    //debugSheets <<"findPrevious";
    int opt = d->findOptions;
    bool forw = !(opt & KFind::FindBackwards);
    if (forw)
        d->findOptions = (opt | KFind::FindBackwards);
    else
        d->findOptions = (opt & ~KFind::FindBackwards);

    findNext();

    d->findOptions = opt; // restore initial options
}

void CellToolBase::replace()
{
    QPointer<SearchDlg> dialog = new SearchDlg(canvas()->canvasWidget(), "Replace", d->findOptions, d->findStrings, d->replaceStrings);
    dialog->setHasSelection(!selection()->isSingular());
    dialog->setHasCursor(true);
    if (KReplaceDialog::Accepted != dialog->exec())
        return;

    d->findOptions = dialog->options();
    d->findStrings = dialog->findHistory();
    d->replaceStrings = dialog->replacementHistory();
    d->typeValue = dialog->searchType();

    delete d->find;
    delete d->replace;
    d->find = 0;
    // NOTE Stefan: Avoid beginning of line replacements with nothing which
    //              will lead to an infinite loop (Bug #125535). The reason
    //              for this is unclear to me, but who cares and who would
    //              want to do something like this, häh?!
    if (dialog->pattern() == "^" && dialog->replacement().isEmpty())
        return;
    d->replace = new KReplace(dialog->pattern(), dialog->replacement(), dialog->options());

    d->searchInSheets.currentSheet = selection()->activeSheet();
    d->searchInSheets.firstSheet = d->searchInSheets.currentSheet;
    initFindReplace();
    connect(d->replace, QOverload<const QString &, int, int, int>::of(&KReplace::replace),
            this, &CellToolBase::slotReplace);

    d->replaceCommand = new KUndo2Command(kundo2_i18n("Replace"));

    findNext();
    delete dialog;

#if 0
    // Refresh the editWidget
    // TODO - after a replacement only?
    Cell cell = Cell(activeSheet(), selection()->marker());
    if (cell.userInput() != 0)
        d->editWidget->setText(cell.userInput());
    else
        d->editWidget->setText("");
#endif
}

void CellToolBase::slotHighlight(const QString &/*text*/, int /*matchingIndex*/, int /*matchedLength*/)
{
    selection()->initialize(d->findPos);
    QDialog *dialog = 0;
    if (d->find)
        dialog = d->find->findNextDialog();
    else
        dialog = d->replace->replaceNextDialog();
    debugSheets << " baseDialog :" << dialog;
    QRect globalRect(d->findPos, d->findEnd);
    globalRect.moveTopLeft(canvas()->canvasWidget()->mapToGlobal(globalRect.topLeft()));
    KoDialog::avoidArea(dialog, QRect(d->findPos, d->findEnd));
}

void CellToolBase::slotReplace(const QString &newText, int, int, int)
{
    if (d->typeValue == FindOption::Value) {
        DataManipulator* command = new DataManipulator(d->replaceCommand);
        command->setParsing(true);
        command->setSheet(d->searchInSheets.currentSheet);
        command->setValue(Value(newText));
        command->add(Region(d->findPos, d->searchInSheets.currentSheet));
    } else if (d->typeValue == FindOption::Note) {
        CommentCommand* command = new CommentCommand(d->replaceCommand);
        command->setComment(newText);
        command->setSheet(d->searchInSheets.currentSheet);
        command->add(Region(d->findPos, d->searchInSheets.currentSheet));
    }
}

void CellToolBase::gotoCell()
{
    QPointer<GotoDialog> dialog = new GotoDialog(canvas()->canvasWidget(), selection());
    dialog->exec();
    delete dialog;
    scrollToCell(selection()->cursor());
}

void CellToolBase::spellCheck()
{
    SpellCheckCommand* command = new SpellCheckCommand(*selection(), canvas());
    command->start();
}

void CellToolBase::inspector()
{
    // useful to inspect objects
    Cell cell(selection()->activeSheet(), selection()->marker());
    QPointer<Calligra::Sheets::Inspector> ins = new Calligra::Sheets::Inspector(cell);
    ins->exec();
    delete ins;
}

void CellToolBase::qTableView()
{
#ifndef NDEBUG
    QPointer<KoDialog> dialog = new KoDialog(canvas()->canvasWidget());
    QTableView* const view = new QTableView(dialog);
    SheetModel* const model = new SheetModel(selection()->activeSheet());
    view->setModel(model);
    dialog->setCaption("Read{Only,Write}TableModel Test");
    dialog->setMainWidget(view);
    dialog->exec();
    delete dialog;
    delete model;
#endif
}

void CellToolBase::sheetFormat()
{
    QPointer<AutoFormatDialog> dialog = new AutoFormatDialog(canvas()->canvasWidget(), selection());
    dialog->exec();
    delete dialog;
}

void CellToolBase::listChoosePopupMenu()
{
    if (!selection()->activeSheet()->fullMap()->isReadWrite()) {
        return;
    }

    delete d->popupListChoose;
    d->popupListChoose = new QMenu();

    Sheet *const sheet = selection()->activeSheet();
    const Cell cursorCell(sheet, selection()->cursor());
    const QString text = cursorCell.userInput();
    const CellStorage *const storage = sheet->fullCellStorage();

    QStringList itemList;
    const Region::ConstIterator end(selection()->constEnd());
    for (Region::ConstIterator it(selection()->constBegin()); it != end; ++it) {
        const QRect range = (*it)->rect();
        if (cursorCell.column() < range.left() || cursorCell.column() > range.right()) {
            continue; // next range
        }
        Cell cell;
        if (range.top() == 1) {
            cell = storage->firstInColumn(cursorCell.column(), CellStorage::Values);
        } else {
            cell = storage->nextInColumn(cursorCell.column(), range.top() - 1, CellStorage::Values);
        }
        while (!cell.isNull() && cell.row() <= range.bottom()) {
            if (!cell.isPartOfMerged() && !(cell == cursorCell)) {
                const QString userInput = cell.userInput();
                if (cell.value().isString() && userInput != text && !userInput.isEmpty()) {
                    if (itemList.indexOf(userInput) == -1) {
                        itemList.append(userInput);
                    }
                }
            }
            cell = storage->nextInColumn(cell.column(), cell.row(), CellStorage::Values);
        }
    }

    for (QStringList::ConstIterator it = itemList.constBegin(); it != itemList.constEnd(); ++it) {
        d->popupListChoose->addAction((*it));
    }

    if (itemList.isEmpty()) {
        return;
    }
    double tx = sheet->columnPosition(selection()->marker().x());
    double ty = sheet->rowPosition(selection()->marker().y());
    double h = cursorCell.height();
    if (sheetView(sheet)->obscuresCells(selection()->marker())) {
        const CellView& cellView = sheetView(sheet)->cellView(selection()->marker().x(), selection()->marker().y());
        h = cellView.cellHeight();
    }
    ty += h;

    if (selection()->activeSheet()->layoutDirection() == Qt::RightToLeft) {
        tx = canvas()->canvasWidget()->width() - tx;
    }

    QPoint p((int)tx, (int)ty);
    QPoint p2 = canvas()->canvasWidget()->mapToGlobal(p);

    if (selection()->activeSheet()->layoutDirection() == Qt::RightToLeft) {
        p2.setX(p2.x() - d->popupListChoose->sizeHint().width() + 1);
    }

    d->popupListChoose->popup(p2);
    connect(d->popupListChoose, &QMenu::triggered,
            this, &CellToolBase::listChooseItemSelected);
}


void CellToolBase::listChooseItemSelected(QAction* action)
{
    const Cell cell(selection()->activeSheet(), selection()->marker());
    if (action->text() == cell.userInput())
        return;

    DataManipulator *command = new DataManipulator;
    command->setSheet(selection()->activeSheet());
    command->setValue(Value(action->text()));
    command->setParsing(true);
    command->add(selection()->marker());
    command->execute(canvas());
}

void CellToolBase::documentSettingsDialog()
{
    QPointer<DocumentSettingsDialog> dialog = new DocumentSettingsDialog(selection(), canvas()->canvasWidget());
    dialog->exec();
    delete dialog;
}

void CellToolBase::breakBeforeColumn(bool enable)
{
    PageBreakCommand *command = new PageBreakCommand();
    command->setSheet(selection()->activeSheet());
    command->setMode(PageBreakCommand::BreakBeforeColumn);
    command->setBreak(enable);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::breakBeforeRow(bool enable)
{
    PageBreakCommand *command = new PageBreakCommand();
    command->setSheet(selection()->activeSheet());
    command->setMode(PageBreakCommand::BreakBeforeRow);
    command->setBreak(enable);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::setExternalEditor(Calligra::Sheets::ExternalEditor *editor)
{
    d->externalEditor = editor;
}
