/* This file is part of the KDE project
   Copyright 2006-2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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
   version 2 of the License, or(at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "CellToolBase.h"
#include "CellToolBase_p.h"

// KSpread
#include "ApplicationSettings.h"
#include "AutoFillStrategy.h"
#include "CalculationSettings.h"
#include "Cell.h"
#include "CellEditor.h"
#include "CellToolOptionWidget.h"
#include "CellView.h"
#include "CharSelectDia.h"
#include "Damages.h"
#include "database/Database.h"
#include "DragAndDropStrategy.h"
#include "ExternalEditor.h"
#include "HyperlinkStrategy.h"
#include "tests/inspector.h"
#include "LocationComboBox.h"
#include "Map.h"
#include "MergeStrategy.h"
#include "NamedAreaManager.h"
#include "PasteStrategy.h"
#include "SelectionStrategy.h"
#include "Sheet.h"
#include "SheetView.h"
#include "StyleManager.h"

// commands
#include "commands/AutoFilterCommand.h"
#include "commands/BorderColorCommand.h"
#include "commands/CommentCommand.h"
#include "commands/ConditionCommand.h"
#include "commands/CopyCommand.h"
#include "commands/DataManipulators.h"
#include "commands/DeleteCommand.h"
#include "commands/IndentationCommand.h"
#include "commands/LinkCommand.h"
#include "commands/MergeCommand.h"
#include "commands/PageBreakCommand.h"
#include "commands/PasteCommand.h"
#include "commands/PrecisionCommand.h"
#include "commands/RowColumnManipulators.h"
#include "commands/SortManipulator.h"
#include "commands/SpellCheckCommand.h"
#include "commands/StyleCommand.h"
#include "commands/ValidityCommand.h"

// dialogs
#include "dialogs/AddNamedAreaDialog.h"
#include "dialogs/AngleDialog.h"
#include "dialogs/AutoFormatDialog.h"
#include "dialogs/CommentDialog.h"
#include "dialogs/ConditionalDialog.h"
#include "dialogs/ConsolidateDialog.h"
#include "dialogs/CSVDialog.h"
#include "dialogs/DatabaseDialog.h"
#include "dialogs/DocumentSettingsDialog.h"
#include "dialogs/GoalSeekDialog.h"
#include "dialogs/GotoDialog.h"
#include "dialogs/InsertDialog.h"
#include "dialogs/LayoutDialog.h"
#include "dialogs/LinkDialog.h"
#include "dialogs/ListDialog.h"
#include "dialogs/NamedAreaDialog.h"
#include "dialogs/PasteInsertDialog.h"
#include "dialogs/Resize2Dialog.h"
#include "dialogs/SeriesDialog.h"
#include "dialogs/ShowColRowDialog.h"
#include "dialogs/SortDialog.h"
#include "dialogs/SpecialPasteDialog.h"
#include "dialogs/StyleManagerDialog.h"
#include "dialogs/SubtotalDialog.h"
#include "dialogs/ValidityDialog.h"

// KOffice
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoResourceManager.h>
#include <KoColorPopupAction.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoOdfStylesReader.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoStore.h>
#include <KoViewConverter.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>

// KDE
#include <KAction>
#include <KFind>
#include <KFontAction>
#include <KFontSizeAction>
#include <KIcon>
#include <KInputDialog>
#include <KLocale>
#include <KMessageBox>
#include <KReplace>
#include <KStandardAction>
#include <KToggleAction>

// Qt
#include <QBuffer>
#include <QHash>
#include <QMenu>
#include <QPainter>
#include <QSqlDatabase>

#ifndef NDEBUG
#include <QTableView>
#include "SheetModel.h"
#endif

using namespace KSpread;

CellToolBase::CellToolBase(KoCanvasBase* canvas)
        : KoInteractionTool(canvas)
        , d(new Private(this))
{
    d->cellEditor = 0;
    d->formulaDialog = 0;
    d->specialCharDialog = 0;
    d->optionWidget = 0;
    d->initialized = false;
    d->popupListChoose = 0;
    d->lastEditorWithFocus = EmbeddedEditor;

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
    KAction* action = 0;

    // -- cell style actions --

    action = new KAction(KIcon("cell_layout"), i18n("Cell Format..."), this);
    action->setIconText(i18n("Format"));
    addAction("cellStyle", action);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_F));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(cellStyle()));
    action->setToolTip(i18n("Set the cell formatting"));

    action = new KAction(i18n("Default"), this);
    addAction("setDefaultStyle", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(setDefaultStyle()));
    action->setToolTip(i18n("Resets to the default format"));

    action = new KAction(i18n("Style Manager..."), this);
    addAction("styleDialog", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(styleDialog()));
    action->setToolTip(i18n("Edit and organize cell styles"));

    action = new KSelectAction(i18n("Style"), this);
    addAction("setStyle", action);
    action->setToolTip(i18n("Apply a predefined style to the selected cells"));
    connect(action, SIGNAL(triggered(const QString&)), this, SLOT(setStyle(const QString&)));

    action = new KAction(i18n("Create Style From Cell..."), this);
    action->setIconText(i18n("Style From Cell"));
    addAction("createStyleFromCell", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(createStyleFromCell()));
    action->setToolTip(i18n("Create a new style based on the currently selected cell"));

    // -- font actions --

    action = new KToggleAction(KIcon("format-text-bold"), i18n("Bold"), this);
    addAction("bold", action);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(bold(bool)));

    action = new KToggleAction(KIcon("format-text-italic"), i18n("Italic"), this);
    addAction("italic", action);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(italic(bool)));

    action = new KToggleAction(KIcon("format-text-underline"), i18n("Underline"), this);
    addAction("underline", action);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(underline(bool)));

    action = new KToggleAction(KIcon("format-text-strikethrough"), i18n("Strike Out"), this);
    addAction("strikeOut", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(strikeOut(bool)));

    action = new KFontAction(i18n("Select Font..."), this);
    action->setIconText(i18n("Font"));
    addAction("font", action);
    connect(action, SIGNAL(triggered(const QString&)), this, SLOT(font(const QString&)));

    action = new KFontSizeAction(i18n("Select Font Size"), this);
    action->setIconText(i18n("Font Size"));
    addAction("fontSize", action);
    connect(action, SIGNAL(fontSizeChanged(int)), this, SLOT(fontSize(int)));

    action = new KAction(KIcon("format-font-size-more"), i18n("Increase Font Size"), this);
    addAction("increaseFontSize", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(increaseFontSize()));

    action = new KAction(KIcon("format-font-size-less"), i18n("Decrease Font Size"), this);
    addAction("decreaseFontSize", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(decreaseFontSize()));

    action = new KoColorPopupAction(this);
    action->setIcon(KIcon("format-text-color"));
    action->setText(i18n("Text Color"));
    action->setToolTip(i18n("Set the text color"));
    addAction("textColor", action);
    connect(action, SIGNAL(colorChanged(const KoColor &)), this, SLOT(changeTextColor(const KoColor &)));

    // -- horizontal alignment actions --

    QActionGroup* groupAlign = new QActionGroup(this);
    action = new KToggleAction(KIcon("format-justify-left"), i18n("Align Left"), this);
    action->setIconText(i18n("Left"));
    addAction("alignLeft", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(alignLeft(bool)));
    action->setToolTip(i18n("Left justify the cell contents"));
    action->setActionGroup(groupAlign);

    action = new KToggleAction(KIcon("format-justify-center"), i18n("Align Center"), this);
    action->setIconText(i18n("Center"));
    addAction("alignCenter", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(alignCenter(bool)));
    action->setToolTip(i18n("Center the cell contents"));
    action->setActionGroup(groupAlign);

    action = new KToggleAction(KIcon("format-justify-right"), i18n("Align Right"), this);
    action->setIconText(i18n("Right"));
    addAction("alignRight", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(alignRight(bool)));
    action->setToolTip(i18n("Right justify the cell contents"));
    action->setActionGroup(groupAlign);

    // -- vertical alignment actions --

    QActionGroup* groupPos = new QActionGroup(this);
    action = new KToggleAction(KIcon("text_top"), i18n("Align Top"), this);
    action->setIconText(i18n("Top"));
    addAction("alignTop", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(alignTop(bool)));
    action->setToolTip(i18n("Align cell contents along the top of the cell"));
    action->setActionGroup(groupPos);

    action = new KToggleAction(KIcon("middle"), i18n("Align Middle"), this);
    action->setIconText(i18n("Middle"));
    addAction("alignMiddle", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(alignMiddle(bool)));
    action->setToolTip(i18n("Align cell contents centered in the cell"));
    action->setActionGroup(groupPos);

    action = new KToggleAction(KIcon("text_bottom"), i18n("Align Bottom"), this);
    action->setIconText(i18n("Bottom"));
    addAction("alignBottom", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(alignBottom(bool)));
    action->setToolTip(i18n("Align cell contents along the bottom of the cell"));
    action->setActionGroup(groupPos);

    // -- border actions --

    action = new KAction(KIcon("border_left"), i18n("Border Left"), this);
    action->setIconText(i18n("Left"));
    addAction("borderLeft", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(borderLeft()));
    action->setToolTip(i18n("Set a left border to the selected area"));

    action = new KAction(KIcon("border_right"), i18n("Border Right"), this);
    action->setIconText(i18n("Right"));
    addAction("borderRight", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(borderRight()));
    action->setToolTip(i18n("Set a right border to the selected area"));

    action = new KAction(KIcon("border_top"), i18n("Border Top"), this);
    action->setIconText(i18n("Top"));
    addAction("borderTop", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(borderTop()));
    action->setToolTip(i18n("Set a top border to the selected area"));

    action = new KAction(KIcon("border_bottom"), i18n("Border Bottom"), this);
    action->setIconText(i18n("Bottom"));
    addAction("borderBottom", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(borderBottom()));
    action->setToolTip(i18n("Set a bottom border to the selected area"));

    action = new KAction(KIcon("border_all"), i18n("All Borders"), this);
    action->setIconText(i18n("All"));
    addAction("borderAll", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(borderAll()));
    action->setToolTip(i18n("Set a border around all cells in the selected area"));

    action = new KAction(KIcon("border_remove"), i18n("No Borders"), this);
    action->setIconText(i18n("None"));
    addAction("borderRemove", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(borderRemove()));
    action->setToolTip(i18n("Remove all borders in the selected area"));

    action = new KAction(KIcon(("border_outline")), i18n("Border Outline"), this);
    action->setIconText(i18n("Outline"));
    addAction("borderOutline", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(borderOutline()));
    action->setToolTip(i18n("Set a border to the outline of the selected area"));

    action = new KoColorPopupAction(this);
    action->setIcon(KIcon("format-stroke-color"));
    action->setToolTip(i18n("Select a new border color"));
    action->setText(i18n("Border Color"));
    addAction("borderColor", action);
    connect(action, SIGNAL(colorChanged(const KoColor &)), this, SLOT(borderColor(const KoColor &)));

    // -- text layout actions --

    action = new KToggleAction(KIcon("multirow"), i18n("Wrap Text"), this);
    action->setIconText(i18n("Wrap"));
    addAction("wrapText", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(wrapText(bool)));
    action->setToolTip(i18n("Make the cell text wrap onto multiple lines"));

    action = new KToggleAction(KIcon("vertical_text"), i18n("Vertical Text"), this);
    action->setIconText(i18n("Vertical"));
    addAction("verticalText", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(verticalText(bool)));
    action->setToolTip(i18n("Print cell contents vertically"));

    action = new KAction(KIcon(QApplication::isRightToLeft() ? "format-indent-less" : "format-indent-more"), i18n("Increase Indent"), this);
    addAction("increaseIndentation", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(increaseIndentation()));
    action->setToolTip(i18n("Increase the indentation"));

    action = new KAction(KIcon(QApplication::isRightToLeft() ? "format-indent-more" : "format-indent-less"), i18n("Decrease Indent"), this);
    addAction("decreaseIndentation", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(decreaseIndentation()));
    action->setToolTip(i18n("Decrease the indentation"));

    action = new KAction(i18n("Change Angle..."), this);
    action->setIconText(i18n("Angle"));
    addAction("changeAngle", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(changeAngle()));
    action->setToolTip(i18n("Change the angle that cell contents are printed"));

    // -- value format actions --

    action = new KToggleAction(KIcon("percent"), i18n("Percent Format"), this);
    action->setIconText(i18n("Percent"));
    addAction("percent", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(percent(bool)));
    action->setToolTip(i18n("Set the cell formatting to look like a percentage"));

    action = new KToggleAction(KIcon("money"), i18n("Money Format"), this);
    action->setIconText(i18n("Money"));
    addAction("currency", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(currency(bool)));
    action->setToolTip(i18n("Set the cell formatting to look like your local currency"));

    action = new KAction(KIcon("prec_plus"), i18n("Increase Precision"), this);
    addAction("increasePrecision", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(increasePrecision()));
    action->setToolTip(i18n("Increase the decimal precision shown onscreen"));

    action = new KAction(KIcon("prec_minus"), i18n("Decrease Precision"), this);
    addAction("decreasePrecision", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(decreasePrecision()));
    action->setToolTip(i18n("Decrease the decimal precision shown onscreen"));

    // -- misc style attribute actions --

    action = new KAction(KIcon("fontsizeup"), i18n("Upper Case"), this);
    action->setIconText(i18n("Upper"));
    addAction("toUpperCase", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(toUpperCase()));
    action->setToolTip(i18n("Convert all letters to upper case"));

    action = new KAction(KIcon("fontsizedown"), i18n("Lower Case"), this);
    action->setIconText(i18n("Lower"));
    addAction("toLowerCase", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(toLowerCase()));
    action->setToolTip(i18n("Convert all letters to lower case"));

    action = new KAction(KIcon("first_letter_upper"), i18n("Convert First Letter to Upper Case"), this);
    action->setIconText(i18n("First Letter Upper"));
    addAction("firstLetterToUpperCase", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(firstLetterToUpperCase()));
    action->setToolTip(i18n("Capitalize the first letter"));

    action = new KoColorPopupAction(this);
    action->setIcon(KIcon("format-fill-color"));
    action->setToolTip(i18n("Set the background color"));
    action->setText(i18n("Background Color"));
    addAction("backgroundColor", action);
    connect(action, SIGNAL(colorChanged(const KoColor &)), this, SLOT(changeBackgroundColor(const KoColor &)));

    // -- cell merging actions --

    action = new KAction(KIcon("mergecell"), i18n("Merge Cells"), this);
    addAction("mergeCells", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(mergeCells()));
    action->setToolTip(i18n("Merge the selected region"));

    action = new KAction(KIcon("mergecell-horizontal"), i18n("Merge Cells Horizontally"), this);
    action->setToolTip(i18n("Merge the selected region horizontally"));
    addAction("mergeCellsHorizontal", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(mergeCellsHorizontal()));

    action = new KAction(KIcon("mergecell-vertical"), i18n("Merge Cells Vertically"), this);
    action->setToolTip(i18n("Merge the selected region vertically"));
    addAction("mergeCellsVertical", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(mergeCellsVertical()));

    action = new KAction(KIcon("dissociatecell"), i18n("Dissociate Cells"), this);
    action->setToolTip(i18n("Unmerge the selected region"));
    addAction("dissociateCells", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(dissociateCells()));

    // -- column & row actions --

    action = new KAction(KIcon("resizecol"), i18n("Resize Column..."), this);
    addAction("resizeCol", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(resizeColumn()));
    action->setToolTip(i18n("Change the width of a column"));

    action = new KAction(KIcon("insert_table_col"), i18n("Columns"), this);
    action->setIconText(i18n("Insert Columns"));
    action->setToolTip(i18n("Inserts a new column into the spreadsheet"));
    addAction("insertColumn", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(insertColumn()));

    action = new KAction(KIcon("delete_table_col"), i18n("Columns"), this);
    action->setIconText(i18n("Remove Columns"));
    action->setToolTip(i18n("Removes a column from the spreadsheet"));
    addAction("deleteColumn", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(deleteColumn()));

    action = new KAction(KIcon("hide_table_column"), i18n("Hide Columns"), this);
    addAction("hideColumn", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(hideColumn()));
    action->setToolTip(i18n("Hide the column from this"));

    action = new KAction(KIcon("show_table_column"), i18n("Show Columns..."), this);
    addAction("showColumn", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(slotShowColumnDialog()));
    action->setToolTip(i18n("Show hidden columns"));

    action = new KAction(KIcon("adjustcol"), i18n("Equalize Column"), this);
    addAction("equalizeCol", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(equalizeColumn()));
    action->setToolTip(i18n("Resizes selected columns to be the same size"));

    action = new KAction(KIcon("show_sheet_column"), i18n("Show Columns"), this);
    addAction("showSelColumns", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(showColumn()));
    action->setToolTip(i18n("Show hidden columns in the selection"));
    action->setEnabled(false);

    action = new KAction(KIcon("resizerow"), i18n("Resize Row..."), this);
    addAction("resizeRow", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(resizeRow()));
    action->setToolTip(i18n("Change the height of a row"));

    action = new KAction(KIcon("insert_table_row"), i18n("Rows"), this);
    action->setIconText(i18n("Insert Rows"));
    action->setToolTip(i18n("Inserts a new row into the spreadsheet"));
    addAction("insertRow", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(insertRow()));

    action = new KAction(KIcon("delete_table_row"), i18n("Rows"), this);
    action->setIconText(i18n("Remove Rows"));
    action->setToolTip(i18n("Removes a row from the spreadsheet"));
    addAction("deleteRow", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(deleteRow()));

    action = new KAction(KIcon("hide_table_row"), i18n("Hide Rows"), this);
    addAction("hideRow", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(hideRow()));
    action->setToolTip(i18n("Hide a row from this"));

    action = new KAction(KIcon("show_table_row"), i18n("Show Rows..."), this);
    addAction("showRow", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(slotShowRowDialog()));
    action->setToolTip(i18n("Show hidden rows"));

    action = new KAction(KIcon("adjustrow"), i18n("Equalize Row"), this);
    addAction("equalizeRow", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(equalizeRow()));
    action->setToolTip(i18n("Resizes selected rows to be the same size"));

    action = new KAction(KIcon("show_table_row"), i18n("Show Rows"), this);
    addAction("showSelRows", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(showRow()));
    action->setEnabled(false);
    action->setToolTip(i18n("Show hidden rows in the selection"));

    action = new KAction(i18n("Adjust Row && Column"), this);
    addAction("adjust", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(adjust()));
    action->setToolTip(i18n("Adjusts row/column size so that the contents will fit"));

    // -- cell insert/remove actions --

    action = new KAction(KIcon("insertcell"), i18n("Cells..."), this);
    action->setIconText(i18n("Insert Cells..."));
    action->setToolTip(i18n("Insert a blank cell into the spreadsheet"));
    addAction("insertCell", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(insertCells()));

    action = new KAction(KIcon("removecell"), i18n("Cells..."), this);
    action->setIconText(i18n("Remove Cells..."));
    action->setToolTip(i18n("Removes the cells from the spreadsheet"));
    addAction("deleteCell", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(deleteCells()));

    // -- cell content actions --

    action = new KAction(KIcon("deletecell"), i18n("All"), this);
    action->setIconText(i18n("Clear All"));
    action->setToolTip(i18n("Clear all contents and formatting of the current cell"));
    addAction("clearAll", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(clearAll()));

    action = new KAction(KIcon("edit-clear"), i18n("Contents"), this);
    action->setIconText(i18n("Clear Contents"));
    action->setToolTip(i18n("Remove the contents of the current cell"));
    addAction("clearContents", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(clearContents()));

    action = new KAction(KIcon("comment"), i18n("Comment..."), this);
    action->setToolTip(i18n("Edit a comment for this cell"));
    addAction("comment", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(comment()));

    action = new KAction(KIcon("removecomment"), i18n("Comment"), this);
    action->setIconText(i18n("Remove Comment"));
    action->setToolTip(i18n("Remove this cell's comment"));
    addAction("clearComment", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(clearComment()));

    action = new KAction(i18n("Conditional Styles..."), this);
    action->setToolTip(i18n("Set cell style based on certain conditions"));
    addAction("conditional", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(conditional()));

    action = new KAction(i18n("Conditional Styles"), this);
    action->setIconText(i18n("Remove Conditional Styles"));
    action->setToolTip(i18n("Remove the conditional cell styles"));
    addAction("clearConditional", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(clearConditionalStyles()));

    action = new KAction(KIcon("insert-link"), i18n("&Link..."), this);
    addAction("insertHyperlink", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(insertHyperlink()));
    action->setToolTip(i18n("Insert an Internet hyperlink"));

    action = new KAction(i18n("Link"), this);
    action->setIconText(i18n("Remove Link"));
    action->setToolTip(i18n("Remove a link"));
    addAction("clearHyperlink", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(clearHyperlink()));

    action = new KAction(i18n("Validity..."), this);
    action->setToolTip(i18n("Set tests to confirm cell data is valid"));
    addAction("validity", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(validity()));

    action = new KAction(i18n("Validity"), this);
    action->setIconText(i18n("Remove Validity"));
    action->setToolTip(i18n("Remove the validity tests on this cell"));
    addAction("clearValidity", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(clearValidity()));

    // -- sorting/filtering action --

    action = new KAction(i18n("&Sort..."), this);
    addAction("sort", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(sort()));
    action->setToolTip(i18n("Sort a group of cells"));

    action = new KAction(KIcon("this-sort-descending"), i18n("Sort &Decreasing"), this);
    addAction("sortDec", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(sortDec()));
    action->setToolTip(i18n("Sort a group of cells in decreasing(last to first) order"));

    action = new KAction(KIcon("this-sort-ascending"), i18n("Sort &Increasing"), this);
    addAction("sortInc", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(sortInc()));
    action->setToolTip(i18n("Sort a group of cells in ascending(first to last) order"));

    action = new KAction(KIcon("this-filter"), i18n("&Auto-Filter"), this);
    addAction("autoFilter", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(autoFilter()));
    action->setToolTip(i18n("Add an automatic filter to a cell range"));

    // -- fill actions --

    action = new KAction(/*KIcon("arrow-left"), */i18n("&Left"), this);
    addAction("fillLeft", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fillLeft()));

    action = new KAction(/*KIcon("arrow-right"), */i18n("&Right"), this);
    addAction("fillRight", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fillRight()));

    action = new KAction(/*KIcon("arrow-up"), */i18n("&Up"), this);
    addAction("fillUp", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fillUp()));

    action = new KAction(/*KIcon("arrow-down"), */i18n("&Down"), this);
    addAction("fillDown", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fillDown()));

    action = new KAction(KIcon("black_sum"), i18n("Autosum"), this);
    addAction("autoSum", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(autoSum()));
    action->setToolTip(i18n("Insert the 'sum' function"));

    // -- data insert actions --

    action = new KAction(KIcon("series"), i18n("&Series..."), this);
    addAction("insertSeries", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(insertSeries()));
    action ->setToolTip(i18n("Insert a series"));

    action = new KAction(KIcon("funct"), i18n("&Function..."), this);
    addAction("insertFormula", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(insertFormula()));
    action->setToolTip(i18n("Insert math expression"));

    action = new KAction(KIcon("accessories-character-map"), i18n("S&pecial Character..."), this);
    addAction("insertSpecialChar", action);
    action->setToolTip(i18n("Insert one or more symbols or letters not found on the keyboard"));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(insertSpecialChar()));

#ifndef QT_NO_SQL
    action = new KAction(KIcon("network-server-database"), i18n("From &Database..."), this);
    action->setIconText(i18n("Database"));
    addAction("insertFromDatabase", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(insertFromDatabase()));
    action->setToolTip(i18n("Insert data from a SQL database"));
#endif

    action = new KAction(KIcon("text-plain"), i18n("From &Text File..."), this);
    action->setIconText(i18n("Text File"));
    addAction("insertFromTextfile", action);
    connect(action, SIGNAL(triggered(bool)), this,  SLOT(insertFromTextfile()));
    action->setToolTip(i18n("Insert data from a text file to the current cursor position/selection"));

    action = new KAction(KIcon("klipper"), i18n("From &Clipboard..."), this);
    action->setIconText(i18n("Clipboard"));
    addAction("insertFromClipboard", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(insertFromClipboard()));
    action->setToolTip(i18n("Insert CSV data from the clipboard to the current cursor position/selection"));

    action = new KAction(i18n("&Text to Columns..."), this);
    addAction("textToColumns", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(textToColumns()));
    action->setToolTip(i18n("Expand the content of cells to multiple columns"));

    action = new KAction(i18n("Custom Lists..."), this);
    addAction("sortList", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(sortList()));
    action->setToolTip(i18n("Create custom lists for sorting or autofill"));

    action = new KAction(i18n("&Consolidate..."), this);
    addAction("consolidate", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(consolidate()));
    action->setToolTip(i18n("Create a region of summary data from a group of similar regions"));

    action = new KAction(i18n("&Goal Seek..."), this);
    addAction("goalSeek", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(goalSeek()));
    action->setToolTip(i18n("Repeating calculation to find a specific value"));

    action = new KAction(i18n("&Subtotals..."), this);
    addAction("subtotals", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(subtotals()));
    action->setToolTip(i18n("Create different kind of subtotals to a list or database"));

    action = new KAction(i18n("Area Name..."), this);
    addAction("setAreaName", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(setAreaName()));
    action->setToolTip(i18n("Set a name for a region of the spreadsheet"));

    action = new KAction(i18n("Named Areas..."), this);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_G));
    action->setIconText(i18n("Named Areas"));
    action->setIcon(KIcon("bookmarks"));
    action->setToolTip(i18n("Edit or select named areas"));
    addAction("namedAreaDialog", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(namedAreaDialog()));

    action = new KSelectAction(i18n("Formula Selection"), this);
    addAction("formulaSelection", action);
    action->setToolTip(i18n("Insert a function"));
    QStringList functionList;
    functionList.append("SUM");
    functionList.append("AVERAGE");
    functionList.append("IF");
    functionList.append("COUNT");
    functionList.append("MIN");
    functionList.append("MAX");
    functionList.append(i18n("Others..."));
    static_cast<KSelectAction*>(action)->setItems(functionList);
    static_cast<KSelectAction*>(action)->setComboWidth(80);
    static_cast<KSelectAction*>(action)->setCurrentItem(0);
    connect(action, SIGNAL(triggered(const QString&)), this, SLOT(formulaSelection(const QString&)));

    // -- general editing actions --

    action = new KAction(KIcon("cell_edit"), i18n("Modify Cell"), this);
    addAction("editCell", action);
    action->setShortcuts(QList<QKeySequence>() << Qt::Key_F2 << QKeySequence(Qt::CTRL + Qt::Key_M));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(edit()));
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

    action = new KAction(KIcon("special_paste"), i18n("Special Paste..."), this);
    addAction("specialPaste", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(specialPaste()));
    action->setToolTip(i18n("Paste the contents of the clipboard with special options"));

    action = new KAction(KIcon("insertcellcopy"), i18n("Paste with Insertion"), this);
    addAction("pasteWithInsertion", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(pasteWithInsertion()));
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

    action = new KAction(KIcon("go-jump"), i18n("Goto Cell..."), this);
    action->setIconText(i18n("Goto"));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    addAction("gotoCell", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(gotoCell()));
    action->setToolTip(i18n("Move to a particular cell"));

    action = KStandardAction::spelling(this, SLOT(spellCheck()), this);
    action->setToolTip(i18n("Check the spelling"));
    addAction("tools_spelling", action);

    action = new KAction(KIcon("inspector"), i18n("Run Inspector..."), this);
    addAction("inspector", action);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(inspector()));

#ifndef NDEBUG
    action = new KAction(KIcon("table"), i18n("Show QTableView..."), this);
    addAction("qTableView", action);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(qTableView()));
#endif

    action = new KAction(i18n("Auto-Format..."), this);
    addAction("sheetFormat", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(sheetFormat()));
    action->setToolTip(i18n("Set the worksheet formatting"));

    action = new KAction(i18n("Document Settings..."), this);
    addAction("documentSettingsDialog", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(documentSettingsDialog()));
    action->setToolTip(i18n("Show document settings dialog"));

    action = new KToggleAction(i18n("Break Before Column"), this);
    addAction("format_break_before_column", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(breakBeforeColumn(bool)));
    action->setIconText(i18n("Column Break"));
    action->setToolTip(i18n("Set a manual page break before the column"));

    action = new KToggleAction(i18n("Break Before Row"), this);
    addAction("format_break_before_row", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(breakBeforeRow(bool)));
    action->setIconText(i18n("Row Break"));
    action->setToolTip(i18n("Set a manual page break before the row"));

    // Editor actions:
    // Set up the permutation of the reference fixations action.
    action = new KAction(i18n("Permute reference fixation"), this);
    addAction("permuteFixation", action);
    action->setShortcut(Qt::Key_F4);
    // connect on creation of the embedded editor
    action->setIconText(i18n("Permute fixation"));
    action->setToolTip(i18n("Permute the fixation of the reference at the text cursor"));
}

CellToolBase::~CellToolBase()
{
    delete d->formulaDialog;
    delete d->popupListChoose;
    qDeleteAll(d->popupMenuActions);
    qDeleteAll(actions());
    delete d;
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
    if (dynamic_cast<DragAndDropStrategy*>(currentStrategy())) {
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
    double xpos;
    double ypos;
    const int col = this->selection()->activeSheet()->leftColumn(position.x(), xpos);
    const int row = this->selection()->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > maxCol() || row > maxRow()) {
        kDebug(36005) << "col or row is out of range:" << "col:" << col << " row:" << row;
    } else {
        const Cell cell = Cell(selection()->activeSheet(), col, row).masterCell();
        SheetView* const sheetView = this->sheetView(selection()->activeSheet());

        QString url;
        const CellView cellView = sheetView->cellView(col, row);
        if (selection()->activeSheet()->layoutDirection() == Qt::RightToLeft) {
            url = cellView.testAnchor(cell, cell.width() - position.x() + xpos, position.y() - ypos);
        } else {
            url = cellView.testAnchor(cell, position.x() - xpos, position.y() - ypos);
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
    createEditor(false /* keep content */);
}

void CellToolBase::keyPressEvent(QKeyEvent* event)
{
    register Sheet * const sheet = selection()->activeSheet();
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

    case Qt::Key_Delete:
        d->processDeleteKey(event);
        return;
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
        QApplication::sendEvent(editor(), event);
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

    // Initialize cell style selection action.
    const StyleManager* styleManager = selection()->activeSheet()->map()->styleManager();
    static_cast<KSelectAction*>(this->action("setStyle"))->setItems(styleManager->styleNames());

    // Establish connections.
    connect(selection(), SIGNAL(changed(const Region&)),
            this, SLOT(selectionChanged(const Region&)));
    connect(selection(), SIGNAL(closeEditor(bool, bool)),
            this, SLOT(deleteEditor(bool, bool)));
    connect(selection(), SIGNAL(modified(const Region&)),
            this, SLOT(updateEditor()));
    connect(selection(), SIGNAL(activeSheetChanged(Sheet*)),
            this, SLOT(activeSheetChanged(Sheet*)));
    connect(selection(), SIGNAL(requestFocusEditor()),
            this, SLOT(focusEditorRequested()));
    connect(selection(), SIGNAL(documentReadWriteToggled(bool)),
            this, SLOT(documentReadWriteToggled(bool)));
    connect(selection(), SIGNAL(sheetProtectionToggled(bool)),
            this, SLOT(sheetProtectionToggled(bool)));
}

void CellToolBase::deactivate()
{
    // Disconnect.
    disconnect(selection(), 0, this, 0);
    // close the cell editor
    deleteEditor(true); // save changes
    // clear the selection rectangle
    selection()->update();
}

void CellToolBase::init()
{
}

QWidget* CellToolBase::createOptionWidget()
{
    d->optionWidget = new CellToolOptionWidget(this);

    connect(selection()->activeSheet()->map()->namedAreaManager(), SIGNAL(namedAreaAdded(const QString&)),
            d->optionWidget->locationComboBox(), SLOT(slotAddAreaName(const QString&)));
    connect(selection()->activeSheet()->map()->namedAreaManager(), SIGNAL(namedAreaRemoved(const QString&)),
            d->optionWidget->locationComboBox(), SLOT(slotRemoveAreaName(const QString&)));

    selection()->update(); // initialize the location combobox
    return d->optionWidget;
}

KoInteractionStrategy* CellToolBase::createStrategy(KoPointerEvent* event)
{
    // Get info about where the event occurred.
    QPointF position = event->point - offset(); // the shape offset, not the scrolling one.

    // Autofilling or merging, if the selection handle was hit.
    if (SelectionStrategy::hitTestSelectionSizeGrip(canvas(), selection(), position)) {
        if (event->button() == Qt::LeftButton)
            return new AutoFillStrategy(this, event->point, event->modifiers());
        else if (event->button() == Qt::MidButton)
            return new MergeStrategy(this, event->point, event->modifiers());
    }

    // Pasting with the middle mouse button.
    if (event->button() == Qt::MidButton) {
        return new PasteStrategy(this, event->point, event->modifiers());
    }

    // Check, if the selected area was hit.
    bool hitSelection = false;
    Region::ConstIterator end = selection()->constEnd();
    for (Region::ConstIterator it = selection()->constBegin(); it != end; ++it) {
        const QRect range = (*it)->rect();
        if (selection()->activeSheet()->cellCoordinatesToDocument(range).contains(position)) {
            // Context menu with the right mouse button.
            if (event->button() == Qt::RightButton) {
                // Setup the context menu.
                setPopupActionList(d->popupActionList());
                event->ignore();
                return 0; // Act directly; no further strategy needed.
            }
            hitSelection = true;
            break;
        }
    }

    // In which cell did the user click?
    double xpos;
    double ypos;
    const int col = this->selection()->activeSheet()->leftColumn(position.x(), xpos);
    const int row = this->selection()->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > maxCol() || row > maxRow()) {
        kDebug(36005) << "col or row is out of range:" << "col:" << col << " row:" << row;
    } else {
        // Context menu with the right mouse button.
        if (event->button() == Qt::RightButton) {
            selection()->initialize(QPoint(col, row), selection()->activeSheet());
            // Setup the context menu.
            setPopupActionList(d->popupActionList());
            event->ignore();
            return 0; // Act directly; no further strategy needed.
        } else {
            const Cell cell = Cell(selection()->activeSheet(), col, row).masterCell();
            SheetView* const sheetView = this->sheetView(selection()->activeSheet());

            // Filter button hit.
            const double offsetX = canvas()->canvasController()->canvasOffsetX();
            const double offsetY = canvas()->canvasController()->canvasOffsetY();
            const QPointF p1 = QPointF(xpos, ypos) - offset(); // the shape offset, not the scrolling one.
            const QSizeF s1(cell.width(), cell.height());
            const QRectF cellRect = canvas()->viewConverter()->documentToView(QRectF(p1, s1));
            const QRect cellViewRect = cellRect.translated(offsetX, offsetY).toRect();
            if (sheetView->cellView(col, row).hitTestFilterButton(cell, cellViewRect, event->pos())) {
                Database database = cell.database();
                database.showPopup(canvas()->canvasWidget(), cell, cellViewRect);
                return 0; // Act directly; no further strategy needed.
            }

            // Hyperlink hit.
            QString url;
            const CellView cellView = sheetView->cellView(col, row);
            if (selection()->activeSheet()->layoutDirection() == Qt::RightToLeft) {
                url = cellView.testAnchor(cell, cell.width() - position.x() + xpos, position.y() - ypos);
            } else {
                url = cellView.testAnchor(cell, position.x() - xpos, position.y() - ypos);
            }
            if (!url.isEmpty()) {
                return new HyperlinkStrategy(this, event->point,
                                             event->modifiers(), url, cellView.textRect());
            }
        }
    }

    // Drag & drop, if the selected area was hit.
    const bool controlPressed = event->modifiers() & Qt::ControlModifier;
    if (hitSelection && !controlPressed && !selection()->referenceSelectionMode()) {
        return new DragAndDropStrategy(this, event->point, event->modifiers());
    }

    return new SelectionStrategy(this, event->point, event->modifiers());
}

void CellToolBase::selectionChanged(const Region& region)
{
    Q_UNUSED(region);
    if (!d->optionWidget) {
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
        const Sheet *const sheet = (*it)->sheet();
        if (!sheet) {
            continue;
        }
        const QRect range = (*it)->rect();
        const int column = range.left();
        const int row = range.top();
        columnBreakChecked |= sheet->columnFormat(column)->hasPageBreak();
        columnBreakEnabled |= (column != 1);
        rowBreakChecked |= sheet->rowFormat(row)->hasPageBreak();
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
    d->updateActions(cell);

    if (selection()->activeSheet()->isProtected()) {
        const Style style = cell.style();
        if (style.notProtected()) {
            if (selection()->isSingular()) {
                if (!action("bold")->isEnabled()) {
                    d->setProtectedActionsEnabled(true);
                }
            } else { // more than one cell
                if (action("bold")->isEnabled()) {
                    d->setProtectedActionsEnabled(false);
                }
            }
        } else {
            if (action("bold")->isEnabled()) {
                d->setProtectedActionsEnabled(false);
            }
        }
    }
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
    canvas()->canvasController()->ensureVisible(rect, true);
}

CellEditor* CellToolBase::editor() const
{
    return d->cellEditor;
}

void CellToolBase::setLastEditorWithFocus(Editor editor)
{
    d->lastEditorWithFocus = editor;
}

bool CellToolBase::createEditor(bool clear, bool focus)
{
    const Cell cell(selection()->activeSheet(), selection()->marker());
    if (selection()->activeSheet()->isProtected() && !cell.style().notProtected())
        return false;

    if (!editor()) {
        d->cellEditor = new CellEditor(this, canvas()->canvasWidget());
        d->cellEditor->setEditorFont(cell.style().font(), true, canvas()->viewConverter());
        connect(action("permuteFixation"), SIGNAL(triggered(bool)),
                d->cellEditor, SLOT(permuteFixation()));
        connect(d->cellEditor, SIGNAL(textChanged(const QString &)),
                d->optionWidget->editor(), SLOT(setText(const QString &)));
        connect(d->optionWidget->editor(), SIGNAL(textChanged(const QString &)),
                d->cellEditor, SLOT(setText(const QString &)));
        d->optionWidget->applyButton()->setEnabled(true);
        d->optionWidget->cancelButton()->setEnabled(true);

        double w = cell.width();
        double h = cell.height();
        double min_w = cell.width();
        double min_h = cell.height();

        double xpos = selection()->activeSheet()->columnPosition(selection()->marker().x());
        xpos += canvas()->viewConverter()->viewToDocumentX(canvas()->canvasController()->canvasOffsetX());

        Qt::LayoutDirection sheetDir = selection()->activeSheet()->layoutDirection();
        bool rtlText = cell.displayText().isRightToLeft();

        // if sheet and cell direction don't match, then the editor's location
        // needs to be shifted backwards so that it's right above the cell's text
        if (w > 0 && ((sheetDir == Qt::RightToLeft && !rtlText) ||
                      (sheetDir == Qt::LeftToRight && rtlText)))
            xpos -= w - min_w;

        // paint editor above correct cell if sheet direction is RTL
        if (sheetDir == Qt::RightToLeft) {
            double dwidth = canvas()->viewConverter()->viewToDocumentX(canvas()->canvasWidget()->width());
            double w2 = qMax(w, min_w);
            xpos = dwidth - w2 - xpos;
        }

        double ypos = selection()->activeSheet()->rowPosition(selection()->marker().y());
        ypos += canvas()->viewConverter()->viewToDocumentY(canvas()->canvasController()->canvasOffsetY());

        // Setup the editor's palette.
        const Style style = cell.effectiveStyle();
        QPalette editorPalette(editor()->palette());
        QColor color = style.fontColor();
        if (!color.isValid())
            color = canvas()->canvasWidget()->palette().text().color();
        editorPalette.setColor(QPalette::Text, color);
        color = style.backgroundColor();
        if (!color.isValid())
            color = editorPalette.base().color();
        editorPalette.setColor(QPalette::Background, color);
        editor()->setPalette(editorPalette);

        // apply (table shape) offset
        xpos += offset().x();
        ypos += offset().y();

        const QRectF rect(xpos + 0.5, ypos + 0.5, w - 0.5, h - 0.5); //needed to circumvent rounding issue with height/width
        const QRectF zoomedRect = canvas()->viewConverter()->documentToView(rect);
        editor()->setGeometry(zoomedRect.toRect().adjusted(1, 1, -1, -1));
        editor()->setMinimumSize(QSize((int)canvas()->viewConverter()->documentToViewX(min_w) - 1,
                                       (int)canvas()->viewConverter()->documentToViewY(min_h) - 1));
        editor()->show();

        // Laurent 2001-12-05
        // Don't add focus when we create a new editor and
        // we select text in edit widget otherwise we don't delete
        // selected text.
        if (focus)
            editor()->setFocus();

        // clear the selection rectangle
        selection()->update();
    }

    if (!clear && !cell.isNull())
        editor()->setText(cell.userInput());
    return true;
}

void CellToolBase::deleteEditor(bool saveChanges, bool expandMatrix)
{
    if (!d->cellEditor) {
        return;
    }
    const QString userInput = editor()->toPlainText();
    editor()->hide();
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
    d->optionWidget->applyButton()->setEnabled(false);
    d->optionWidget->cancelButton()->setEnabled(false);
    canvas()->canvasWidget()->setFocus();
}

void CellToolBase::activeSheetChanged(Sheet* sheet)
{
#ifdef NDEBUG
    Q_UNUSED(sheet);
#else
    Q_ASSERT(selection()->activeSheet() == sheet);
#endif
    if (!selection()->referenceSelectionMode()) {
        return;
    }
    if (editor()) {
        if (selection()->originSheet() != selection()->activeSheet()) {
            editor()->hide();
        } else {
            editor()->show();
        }
    }
    focusEditorRequested();
}

void CellToolBase::updateEditor()
{
    if (!d->optionWidget) {
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
        d->optionWidget->editor()->setFocus();
    } else {
        // Focus the last active editor, if on the origin sheet.
        if (d->lastEditorWithFocus == EmbeddedEditor) {
            editor()->setFocus();
        } else {
            d->optionWidget->editor()->setFocus();
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
            for (int i = 0; i < diff; i++) {
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

    Cell cell = Cell(selection()->activeSheet(), selection()->marker());
    if (cell.value().isString() && !text.isEmpty() && !text.at(0).isDigit() && !cell.isFormula()) {
        selection()->activeSheet()->map()->addStringCompletion(text);
    }
}

void CellToolBase::documentReadWriteToggled(bool readWrite)
{
    if (!d->optionWidget) {
        return;
    }
    d->setProtectedActionsEnabled(readWrite);
}

void CellToolBase::sheetProtectionToggled(bool protect)
{
    if (!d->optionWidget) {
        return;
    }
    d->setProtectedActionsEnabled(!protect);
}

void CellToolBase::cellStyle()
{
    CellFormatDialog dialog(canvas()->canvasWidget(), selection());
    dialog.exec();
}

void CellToolBase::setDefaultStyle()
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setDefault();
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::styleDialog()
{
    Map* const map = selection()->activeSheet()->map();
    StyleManager* const styleManager = map->styleManager();
    StyleManagerDialog dialog(canvas()->canvasWidget(), selection(), styleManager);
    dialog.exec();

    static_cast<KSelectAction*>(action("setStyle"))->setItems(styleManager->styleNames());
    if (selection()->activeSheet())
        map->addDamage(new CellDamage(selection()->activeSheet(), Region(1, 1, maxCol(), maxRow()), CellDamage::Appearance));
    canvas()->canvasWidget()->update();
}

void CellToolBase::setStyle(const QString& stylename)
{
    kDebug() << "CellToolBase::setStyle(" << stylename << ")";
    if (selection()->activeSheet()->map()->styleManager()->style(stylename)) {
        StyleCommand* command = new StyleCommand();
        command->setSheet(selection()->activeSheet());
        command->setParentName(stylename);
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
        styleName = KInputDialog::getText(i18n("Create Style From Cell"),
                                          i18n("Enter name:"), styleName, &ok, canvas()->canvasWidget());

        if (!ok) // User pushed an OK button.
            return;

        styleName = styleName.trimmed();

        if (styleName.length() < 1) {
            KMessageBox::sorry(canvas()->canvasWidget(), i18n("The style name cannot be empty."));
            continue;
        }

        if (selection()->activeSheet()->map()->styleManager()->style(styleName) != 0) {
            KMessageBox::sorry(canvas()->canvasWidget(), i18n("A style with this name already exists."));
            continue;
        }
        break;
    }

    const Style cellStyle = cell.style();
    CustomStyle*  style = new CustomStyle(styleName);
    style->merge(cellStyle);

    selection()->activeSheet()->map()->styleManager()->insertStyle(style);
    cell.setStyle(*style);
    QStringList functionList(static_cast<KSelectAction*>(action("setStyle"))->items());
    functionList.push_back(styleName);
    static_cast<KSelectAction*>(action("setStyle"))->setItems(functionList);
}

void CellToolBase::bold(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Font"));
    command->setFontBold(enable);
    command->add(*selection());
    command->execute(canvas());
    if (editor()) {
        const Cell cell = Cell(selection()->activeSheet(), selection()->marker());
        editor()->setEditorFont(cell.style().font(), true, canvas()->viewConverter());
    }
}

void CellToolBase::underline(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Font"));
    command->setFontUnderline(enable);
    command->add(*selection());
    command->execute(canvas());
    if (editor()) {
        const Cell cell = Cell(selection()->activeSheet(), selection()->marker());
        editor()->setEditorFont(cell.style().font(), true, canvas()->viewConverter());
    }
}

void CellToolBase::strikeOut(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Font"));
    command->setFontStrike(enable);
    command->add(*selection());
    command->execute(canvas());
    if (editor()) {
        const Cell cell = Cell(selection()->activeSheet(), selection()->marker());
        editor()->setEditorFont(cell.style().font(), true, canvas()->viewConverter());
    }
}


void CellToolBase::italic(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Font"));
    command->setFontItalic(enable);
    command->add(*selection());
    command->execute(canvas());
    if (editor()) {
        const Cell cell = Cell(selection()->activeSheet(), selection()->marker());
        editor()->setEditorFont(cell.style().font(), true, canvas()->viewConverter());
    }
}

void CellToolBase::font(const QString& font)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Font"));
    command->setFontFamily(font.toLatin1());
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
    command->setText(i18n("Change Font"));
    command->setFontSize(size);
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

void CellToolBase::increaseFontSize()
{
    const Style style = Cell(selection()->activeSheet(), selection()->marker()).style();
    const int size = style.fontSize();

    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Font"));
    command->setFontSize(size + 1);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::decreaseFontSize()
{
    const Style style = Cell(selection()->activeSheet(), selection()->marker()).style();
    const int size = style.fontSize();

    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Font"));
    command->setFontSize(size - 1);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::changeTextColor(const KoColor &color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Text Color"));
    command->setFontColor(color.toQColor());
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::alignLeft(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Horizontal Alignment"));
    command->setHorizontalAlignment(enable ? Style::Left : Style::HAlignUndefined);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::alignRight(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Horizontal Alignment"));
    command->setHorizontalAlignment(enable ? Style::Right : Style::HAlignUndefined);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::alignCenter(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Horizontal Alignment"));
    command->setHorizontalAlignment(enable ? Style::Center : Style::HAlignUndefined);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::alignTop(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Vertical Alignment"));
    command->setVerticalAlignment(enable ? Style::Top : Style::VAlignUndefined);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::alignBottom(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Vertical Alignment"));
    command->setVerticalAlignment(enable ? Style::Bottom : Style::VAlignUndefined);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::alignMiddle(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Vertical Alignment"));
    command->setVerticalAlignment(enable ? Style::Middle : Style::VAlignUndefined);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::borderLeft()
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Border"));
    if (selection()->activeSheet()->layoutDirection() == Qt::RightToLeft)
        command->setRightBorderPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    else
        command->setLeftBorderPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::borderRight()
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Border"));
    if (selection()->activeSheet()->layoutDirection() == Qt::RightToLeft)
        command->setLeftBorderPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    else
        command->setRightBorderPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::borderTop()
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Border"));
    command->setTopBorderPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::borderBottom()
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Border"));
    command->setBottomBorderPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::borderAll()
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Border"));
    command->setTopBorderPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    command->setBottomBorderPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    command->setLeftBorderPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    command->setRightBorderPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    command->setHorizontalPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    command->setVerticalPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::borderRemove()
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Border"));
    command->setTopBorderPen(QPen(Qt::NoPen));
    command->setBottomBorderPen(QPen(Qt::NoPen));
    command->setLeftBorderPen(QPen(Qt::NoPen));
    command->setRightBorderPen(QPen(Qt::NoPen));
    command->setHorizontalPen(QPen(Qt::NoPen));
    command->setVerticalPen(QPen(Qt::NoPen));
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::borderOutline()
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Border"));
    command->setTopBorderPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    command->setBottomBorderPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    command->setLeftBorderPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    command->setRightBorderPen(QPen(canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::borderColor(const KoColor &color)
{
    BorderColorCommand* command = new BorderColorCommand();
    command->setSheet(selection()->activeSheet());
    command->setColor(color.toQColor());
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::wrapText(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Wrap Text"));
    command->setMultiRow(enable);
    command->setVerticalText(false);
    command->setAngle(0);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::verticalText(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Vertical Text"));
    command->setVerticalText(enable);
    command->setMultiRow(false);
    command->setAngle(0);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::increaseIndentation()
{
    IndentationCommand* command = new IndentationCommand();
    command->setSheet(selection()->activeSheet());
    command->add(*selection());
    if (!command->execute())
        delete command;
}

void CellToolBase::decreaseIndentation()
{
    IndentationCommand* command = new IndentationCommand();
    command->setSheet(selection()->activeSheet());
    command->setReverse(true);
    command->add(*selection());
    if (!command->execute())
        delete command;
}

void CellToolBase::changeAngle()
{
    AngleDialog dialog(canvas()->canvasWidget(), selection());
    dialog.exec();
}

void CellToolBase::percent(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Format Percent"));
    command->setFormatType(enable ? Format::Percentage : Format::Generic);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::currency(bool enable)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Format Money"));
    command->setFormatType(enable ? Format::Money : Format::Generic);
    command->setPrecision(enable ?  selection()->activeSheet()->map()->calculationSettings()->locale()->fracDigits() : 0);
    command->add(*selection());
    command->execute(canvas());
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
    command->setReverse(true);
    command->add(*selection());
    if (!command->execute())
        delete command;
}

void CellToolBase::toUpperCase()
{
    CaseManipulator* command = new CaseManipulator;
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Switch to uppercase"));
    command->changeMode(CaseManipulator::Upper);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::toLowerCase()
{
    CaseManipulator* command = new CaseManipulator;
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Switch to lowercase"));
    command->changeMode(CaseManipulator::Lower);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::firstLetterToUpperCase()
{
    CaseManipulator* command = new CaseManipulator;
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("First letter uppercase"));
    command->changeMode(CaseManipulator::FirstUpper);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::changeBackgroundColor(const KoColor &color)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Change Background Color"));
    command->setBackgroundColor(color.toQColor());
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::mergeCells()
{
    // sanity check
    if (selection()->activeSheet()->isProtected()) {
        return;
    }
    if (selection()->activeSheet()->map()->isProtected()) {
        return;
    }
    MergeCommand* const command = new MergeCommand();
    command->setSheet(selection()->activeSheet());
    command->setSelection(selection());
    command->setHorizontalMerge(false);
    command->setVerticalMerge(false);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::mergeCellsHorizontal()
{
    // sanity check
    if (selection()->activeSheet()->isProtected()) {
        return;
    }
    if (selection()->activeSheet()->map()->isProtected()) {
        return;
    }
    MergeCommand* const command = new MergeCommand();
    command->setSheet(selection()->activeSheet());
    command->setHorizontalMerge(true);
    command->setVerticalMerge(false);
    command->setSelection(selection());
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::mergeCellsVertical()
{
    // sanity check
    if (selection()->activeSheet()->isProtected()) {
        return;
    }
    if (selection()->activeSheet()->map()->isProtected()) {
        return;
    }
    MergeCommand* const command = new MergeCommand();
    command->setSheet(selection()->activeSheet());
    command->setHorizontalMerge(false);
    command->setVerticalMerge(true);
    command->setSelection(selection());
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::dissociateCells()
{
    // sanity check
    if (selection()->activeSheet()->isProtected()) {
        return;
    }
    if (selection()->activeSheet()->map()->isProtected()) {
        return;
    }
    MergeCommand* const command = new MergeCommand();
    command->setSheet(selection()->activeSheet());
    command->setReverse(true);
    command->setSelection(selection());
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::resizeColumn()
{
    if (selection()->isRowSelected())
        KMessageBox::error(canvas()->canvasWidget(), i18n("Area is too large."));
    else {
        ResizeColumn dialog(canvas()->canvasWidget(), selection());
        dialog.exec();
    }
}

void CellToolBase::insertColumn()
{
    InsertDeleteColumnManipulator* command = new InsertDeleteColumnManipulator();
    command->setSheet(selection()->activeSheet());
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::deleteColumn()
{
    InsertDeleteColumnManipulator* command = new InsertDeleteColumnManipulator();
    command->setSheet(selection()->activeSheet());
    command->setReverse(true);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::hideColumn()
{
    if (selection()->isRowSelected()) {
        KMessageBox::error(canvas()->canvasWidget(), i18n("Area is too large."));
        return;
    }

    HideShowManipulator* command = new HideShowManipulator();
    command->setSheet(selection()->activeSheet());
    command->setManipulateColumns(true);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::showColumn()
{
    if (selection()->isRowSelected()) {
        KMessageBox::error(canvas()->canvasWidget(), i18n("Area is too large."));
        return;
    }

    HideShowManipulator* command = new HideShowManipulator();
    command->setSheet(selection()->activeSheet());
    command->setManipulateColumns(true);
    command->setReverse(true);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::slotShowColumnDialog()
{
    ShowColRow dialog(canvas()->canvasWidget(), selection(), ShowColRow::Column);
    dialog.exec();
}

void CellToolBase::equalizeColumn()
{
    if (selection()->isRowSelected())
        KMessageBox::error(canvas()->canvasWidget(), i18n("Area is too large."));
    else {
        const QRect range = selection()->lastRange();
        const ColumnFormat* columnFormat = selection()->activeSheet()->columnFormat(range.left());
        double size = columnFormat->width();
        if (range.left() == range.right())
            return;
        for (int i = range.left() + 1; i <= range.right(); ++i)
            size = qMax(selection()->activeSheet()->columnFormat(i)->width(), size);

        if (size != 0.0) {
            ResizeColumnManipulator* command = new ResizeColumnManipulator();
            command->setSheet(selection()->activeSheet());
            command->setSize(qMax(2.0, size));
            command->add(*selection());
            if (!command->execute())
                delete command;
        } else { // hide
            HideShowManipulator* command = new HideShowManipulator();
            command->setSheet(selection()->activeSheet());
            command->setManipulateColumns(true);
            command->add(*selection());
            if (!command->execute())
                delete command;
        }
    }
}

void CellToolBase::adjustColumn()
{
    AdjustColumnRowManipulator* command = new AdjustColumnRowManipulator();
    command->setSheet(selection()->activeSheet());
    command->setAdjustColumn(true);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::resizeRow()
{
    if (selection()->isColumnSelected())
        KMessageBox::error(canvas()->canvasWidget(), i18n("Area is too large."));
    else {
        ResizeRow dialog(canvas()->canvasWidget(), selection());
        dialog.exec();
    }
}

void CellToolBase::insertRow()
{
    InsertDeleteRowManipulator* command = new InsertDeleteRowManipulator();
    command->setSheet(selection()->activeSheet());
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::deleteRow()
{
    InsertDeleteRowManipulator* command = new InsertDeleteRowManipulator();
    command->setSheet(selection()->activeSheet());
    command->setReverse(true);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::hideRow()
{
    if (selection()->isColumnSelected()) {
        KMessageBox::error(canvas()->canvasWidget(), i18n("Area is too large."));
        return;
    }

    HideShowManipulator* command = new HideShowManipulator();
    command->setSheet(selection()->activeSheet());
    command->setManipulateRows(true);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::showRow()
{
    if (selection()->isColumnSelected()) {
        KMessageBox::error(canvas()->canvasWidget(), i18n("Area is too large."));
        return;
    }

    HideShowManipulator* command = new HideShowManipulator();
    command->setSheet(selection()->activeSheet());
    command->setManipulateRows(true);
    command->setReverse(true);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::slotShowRowDialog()
{
    ShowColRow dialog(canvas()->canvasWidget(), selection(), ShowColRow::Row);
    dialog.exec();
}

void CellToolBase::equalizeRow()
{
    if (selection()->isColumnSelected())
        KMessageBox::error(canvas()->canvasWidget(), i18n("Area is too large."));
    else {
        const QRect range = selection()->lastRange();
        const RowFormat* rowFormat = selection()->activeSheet()->rowFormat(range.top());
        double size = rowFormat->height();
        if (range.top() == range.bottom())
            return;
        for (int i = range.top() + 1; i <= range.bottom(); ++i)
            size = qMax(selection()->activeSheet()->rowFormat(i)->height(), size);

        if (size != 0.0) {
            ResizeRowManipulator* command = new ResizeRowManipulator();
            command->setSheet(selection()->activeSheet());
            command->setSize(qMax(2.0, size));
            command->add(*selection());
            if (!command->execute())
                delete command;
        } else { // hide
            HideShowManipulator* command = new HideShowManipulator();
            command->setSheet(selection()->activeSheet());
            command->setManipulateRows(true);
            command->add(*selection());
            if (!command->execute())
                delete command;
        }
    }
}

void CellToolBase::adjustRow()
{
    AdjustColumnRowManipulator* command = new AdjustColumnRowManipulator();
    command->setSheet(selection()->activeSheet());
    command->setAdjustRow(true);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::adjust()
{
    AdjustColumnRowManipulator* command = new AdjustColumnRowManipulator();
    command->setSheet(selection()->activeSheet());
    command->setAdjustColumn(true);
    command->setAdjustRow(true);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::insertCells()
{
    InsertDialog dialog(canvas()->canvasWidget(), selection(), InsertDialog::Insert);
    dialog.exec();
}

void CellToolBase::deleteCells()
{
    InsertDialog dialog(canvas()->canvasWidget(), selection(), InsertDialog::Remove);
    dialog.exec();
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
    command->setText(i18n("Clear Text"));
    // parsing gets set only so that parseUserInput is called as it should be,
    // no actual parsing shall be done
    command->setParsing(true);
    command->setValue(Value(""));
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::comment()
{
    CommentDialog dialog(canvas()->canvasWidget(), selection());
    dialog.exec();
}

void CellToolBase::clearComment()
{
    // TODO Stefan: Actually this check belongs into the command!
    if (selection()->activeSheet()->areaIsEmpty(*selection(), Sheet::Comment))
        return;

    CommentCommand* command = new CommentCommand();
    command->setSheet(selection()->activeSheet());
    command->setText(i18n("Remove Comment"));
    command->setComment(QString());
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::conditional()
{
    ConditionalDialog dialog(canvas()->canvasWidget(), selection());
    dialog.exec();
}

void CellToolBase::clearConditionalStyles()
{
    // TODO Stefan: Actually this check belongs into the command!
    if (selection()->activeSheet()->areaIsEmpty(*selection(), Sheet::ConditionalCellAttribute))
        return;

    CondtionCommand* command = new CondtionCommand();
    command->setSheet(selection()->activeSheet());
    command->setConditionList(QLinkedList<Conditional>());
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::insertHyperlink()
{
    selection()->emitAboutToModify();

    QPoint marker(selection()->marker());
    Cell cell(selection()->activeSheet(), marker);

    LinkDialog* dialog = new LinkDialog(canvas()->canvasWidget(), selection());
    dialog->setWindowTitle(i18n("Insert Link"));
    if (!cell.isNull()) {
        dialog->setText(cell.userInput());
        if (!cell.link().isEmpty()) {
            dialog->setWindowTitle(i18n("Edit Link"));
            dialog->setLink(cell.link());
        }
    }

    if (dialog->exec() == KDialog::Accepted) {
        cell = Cell(selection()->activeSheet(), marker);

        LinkCommand* command = new LinkCommand(cell, dialog->text(), dialog->link());
        canvas()->addCommand(command);

        //refresh editWidget
        selection()->emitModified();
    }
    delete dialog;
}

void CellToolBase::clearHyperlink()
{
    QPoint marker(selection()->marker());
    Cell cell(selection()->activeSheet(), marker);
    if (!cell)
        return;
    if (cell.link().isEmpty())
        return;

    LinkCommand* command = new LinkCommand(cell, QString(), QString());
    canvas()->addCommand(command);

    selection()->emitModified();
}

void CellToolBase::validity()
{
    ValidityDialog dialog(canvas()->canvasWidget(), selection());
    dialog.exec();
}

void CellToolBase::clearValidity()
{
    // TODO Stefan: Actually this check belongs into the command!
    if (selection()->activeSheet()->areaIsEmpty(*selection(), Sheet::Validity))
        return;

    ValidityCommand* command = new ValidityCommand();
    command->setSheet(selection()->activeSheet());
    command->setValidity(Validity()); // empty object removes validity
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::sort()
{
    if (selection()->isSingular()) {
        KMessageBox::error(canvas()->canvasWidget(), i18n("You must select multiple cells."));
        return;
    }

    SortDialog dialog(canvas()->canvasWidget(), selection());
    dialog.exec();
}

void CellToolBase::sortInc()
{
    if (selection()->isSingular()) {
        KMessageBox::error(canvas()->canvasWidget(), i18n("You must select multiple cells."));
        return;
    }

    SortManipulator* command = new SortManipulator();
    command->setSheet(selection()->activeSheet());

    // Entire row(s) selected ? Or just one row ? Sort by columns if yes.
    QRect range = selection()->lastRange();
    bool sortCols = selection()->isRowSelected();
    sortCols = sortCols || (range.top() == range.bottom());
    command->setSortRows(!sortCols);
    command->addCriterion(0, Qt::AscendingOrder, Qt::CaseInsensitive);
    command->add(*selection());
    command->execute(canvas());

    selection()->emitModified();
}

void CellToolBase::sortDec()
{
    if (selection()->isSingular()) {
        KMessageBox::error(canvas()->canvasWidget(), i18n("You must select multiple cells."));
        return;
    }

    SortManipulator* command = new SortManipulator();
    command->setSheet(selection()->activeSheet());

    // Entire row(s) selected ? Or just one row ? Sort by rows if yes.
    QRect range = selection()->lastRange();
    bool sortCols = selection()->isRowSelected();
    sortCols = sortCols || (range.top() == range.bottom());
    command->setSortRows(!sortCols);
    command->addCriterion(0, Qt::DescendingOrder, Qt::CaseInsensitive);
    command->add(*selection());
    command->execute(canvas());

    selection()->emitModified();
}

void CellToolBase::autoFilter()
{
    AutoFilterCommand* command = new AutoFilterCommand();
    command->setSheet(selection()->activeSheet());
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::fillLeft()
{
    FillManipulator* command = new FillManipulator();
    command->setSheet(selection()->activeSheet());
    command->setDirection(FillManipulator::Left);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::fillRight()
{
    FillManipulator* command = new FillManipulator();
    command->setSheet(selection()->activeSheet());
    command->setDirection(FillManipulator::Right);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::fillUp()
{
    FillManipulator* command = new FillManipulator();
    command->setSheet(selection()->activeSheet());
    command->setDirection(FillManipulator::Up);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::fillDown()
{
    FillManipulator* command = new FillManipulator();
    command->setSheet(selection()->activeSheet());
    command->setDirection(FillManipulator::Down);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::autoSum()
{
    selection()->emitAboutToModify();

    //Get the selected range and remove the current cell from it(as that is
    //where the result of the autosum will be stored - perhaps change
    //this behaviour??)
    QRect sel = selection()->lastRange();

    if (sel.height() > 1) {
        if (selection()->marker().y() == sel.top())
            sel.setTop(sel.top() + 1);
        if (selection()->marker().y() == sel.bottom())
            sel.setBottom(sel.bottom() - 1);
    } else {
        if (sel.width() > 1) {
            if (selection()->marker().x() == sel.left())
                sel.setLeft(sel.left() + 1);

            if (selection()->marker().x() == sel.right())
                sel.setRight(sel.right() - 1);
        } else {
            sel = QRect();

            // only 1 cell selected
            // try to automagically find cells the user wants to sum up

            int start = -1, end = -1;

            if ((selection()->marker().y() > 1) && Cell(selection()->activeSheet(), selection()->marker().x(), selection()->marker().y() - 1).value().isNumber()) {
                // check cells above the current one
                start = end = selection()->marker().y() - 1;
                for (start--; (start > 0) && Cell(selection()->activeSheet(), selection()->marker().x(), start).value().isNumber(); start--) ;

                const Region region(QRect(QPoint(selection()->marker().x(), start + 1),
                                          QPoint(selection()->marker().x(), end)), selection()->activeSheet());
                const QString str = region.name(selection()->activeSheet());

                createEditor();
                editor()->setText("=SUM(" + str + ')');
                editor()->setCursorPosition(5 + str.length());
                return;
            } else if ((selection()->marker().x() > 1) && Cell(selection()->activeSheet(), selection()->marker().x() - 1, selection()->marker().y()).value().isNumber()) {
                // check cells to the left of the current one
                start = end = selection()->marker().x() - 1;
                for (start--; (start > 0) && Cell(selection()->activeSheet(), start, selection()->marker().y()).value().isNumber(); start--) ;

                const Region region(QRect(QPoint(start + 1, selection()->marker().y()),
                                          QPoint(end, selection()->marker().y())), selection()->activeSheet());
                const QString str = region.name(selection()->activeSheet());

                createEditor();
                editor()->setText("=SUM(" + str + ')');
                editor()->setCursorPosition(5 + str.length());
                return;
            }
        }
    }

    if ((sel.width() > 1) && (sel.height() > 1))
        sel = QRect();

    createEditor();

    const Region region(sel, selection()->activeSheet());
    if (region.isValid()) {
        editor()->setText("=SUM(" + region.name(selection()->activeSheet()) + ')');
        deleteEditor(true);
    } else {
        selection()->startReferenceSelection();
        editor()->setText("=SUM()");
        editor()->setCursorPosition(5);
    }
}

void CellToolBase::insertSeries()
{
    selection()->emitAboutToModify();
    SeriesDialog dialog(canvas()->canvasWidget(), selection());
    dialog.exec();
}

void CellToolBase::insertSpecialChar()
{
    QString fontFamily = Cell(selection()->activeSheet(), selection()->marker()).style().fontFamily();
    QChar c = ' ';

    if (d->specialCharDialog == 0) {
        d->specialCharDialog = new CharSelectDia(canvas()->canvasWidget(), "SpecialCharDialog", fontFamily, c, false);
        connect(d->specialCharDialog, SIGNAL(insertChar(QChar, const QString&)),
                this, SLOT(specialChar(QChar, const QString&)));
        connect(d->specialCharDialog, SIGNAL(finished()),
                this, SLOT(specialCharDialogClosed()));
    }
    d->specialCharDialog->show();
}

void CellToolBase::specialCharDialogClosed()
{
    if (d->specialCharDialog) {
        disconnect(d->specialCharDialog, SIGNAL(insertChar(QChar, const QString&)),
                   this, SLOT(specialChar(QChar, const QString&)));
        disconnect(d->specialCharDialog, SIGNAL(finished()),
                   this, SLOT(specialCharDialogClosed()));
        d->specialCharDialog->deleteLater();
        d->specialCharDialog = 0;
    }
}

void CellToolBase::specialChar(QChar character, const QString& fontName)
{
    const Style style = Cell(selection()->activeSheet(), selection()->marker()).style();
    if (style.fontFamily() != fontName) {
        Style newStyle;
        newStyle.setFontFamily(fontName);
        selection()->activeSheet()->cellStorage()->setStyle(Region(selection()->marker()), newStyle);
    }
    QKeyEvent keyEvent(QEvent::KeyPress, 0, Qt::NoModifier, QString(character));
    if (!editor()) {
        createEditor();
    }
    QApplication::sendEvent(editor(), &keyEvent);
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
    selection()->emitAboutToModify();

    QStringList str = QSqlDatabase::drivers();
    if (str.isEmpty()) {
        KMessageBox::error(canvas()->canvasWidget(), i18n("No database drivers available.  To use this feature you need "
                           "to install the necessary Qt 3 database drivers."));
        return;
    }

    DatabaseDialog dialog(canvas()->canvasWidget(), selection());
    dialog.exec();
#endif
}

void CellToolBase::insertFromTextfile()
{
    selection()->emitAboutToModify();

    CSVDialog dialog(canvas()->canvasWidget(), selection(), CSVDialog::File);
    dialog.setDecimalSymbol(selection()->activeSheet()->map()->calculationSettings()->locale()->decimalSymbol());
    dialog.setThousandsSeparator(selection()->activeSheet()->map()->calculationSettings()->locale()->thousandsSeparator());
    if (!dialog.canceled())
        dialog.exec();
}

void CellToolBase::insertFromClipboard()
{
    selection()->emitAboutToModify();

    CSVDialog dialog(canvas()->canvasWidget(), selection(), CSVDialog::Clipboard);
    dialog.setDecimalSymbol(selection()->activeSheet()->map()->calculationSettings()->locale()->decimalSymbol());
    dialog.setThousandsSeparator(selection()->activeSheet()->map()->calculationSettings()->locale()->thousandsSeparator());
    if (!dialog.canceled())
        dialog.exec();
}

void CellToolBase::textToColumns()
{
    selection()->emitAboutToModify();

    QRect area = selection()->lastRange();
    area.setRight(area.left()); // only use the first column
    Region oldSelection = *selection(); // store
    selection()->initialize(area);

    CSVDialog dialog(canvas()->canvasWidget(), selection(), CSVDialog::Column);
    dialog.setDecimalSymbol(selection()->activeSheet()->map()->calculationSettings()->locale()->decimalSymbol());
    dialog.setThousandsSeparator(selection()->activeSheet()->map()->calculationSettings()->locale()->thousandsSeparator());
    if (!dialog.canceled())
        dialog.exec();
    else
        selection()->initialize(oldSelection);
}

void CellToolBase::sortList()
{
    ListDialog dialog(canvas()->canvasWidget());
    dialog.exec();
}

void CellToolBase::consolidate()
{
    selection()->emitAboutToModify();
    ConsolidateDialog * dialog = new ConsolidateDialog(canvas()->canvasWidget(), selection());
    dialog->show(); // dialog deletes itself later
}

void CellToolBase::goalSeek()
{
    selection()->emitAboutToModify();

    GoalSeekDialog* dialog = new GoalSeekDialog(canvas()->canvasWidget(), selection());
    dialog->show(); // dialog deletes itself later
}

void CellToolBase::subtotals()
{
    if ((selection()->lastRange().width() < 2) || (selection()->lastRange().height() < 2)) {
        KMessageBox::error(canvas()->canvasWidget(), i18n("You must select multiple cells."));
        return;
    }

    SubtotalDialog dialog(canvas()->canvasWidget(), selection());
    dialog.exec();
}

void CellToolBase::setAreaName()
{
    AddNamedAreaDialog dialog(canvas()->canvasWidget(), selection());
    dialog.exec();
}

void CellToolBase::namedAreaDialog()
{
    NamedAreaDialog dialog(canvas()->canvasWidget(), selection());
    dialog.exec();
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
        createEditor(false /* keep content */);
    } else {
        // Switch focus.
        if (editor()->hasFocus()) {
            d->optionWidget->editor()->setFocus();
        } else {
            editor()->setFocus();
        }
    }
}

void CellToolBase::cut()
{
    if (!editor()) {
        QDomDocument doc = CopyCommand::saveAsXml(*selection(), true);
        doc.documentElement().setAttribute("cut", selection()->Region::name());

        // Save to buffer
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        QTextStream str(&buffer);
        str.setCodec("UTF-8");
        str << doc;
        buffer.close();

        QMimeData* mimeData = new QMimeData();
        mimeData->setText(CopyCommand::saveAsPlainText(*selection()));
        mimeData->setData("application/x-kspread-snippet", buffer.buffer());

        QApplication::clipboard()->setMimeData(mimeData);

        DeleteCommand* command = new DeleteCommand();
        command->setText(i18n("Cut"));
        command->setSheet(selection()->activeSheet());
        command->add(*selection());
        command->execute();
    } else {
        editor()->cut();
    }
    selection()->emitModified();
}

void CellToolBase::copy() const
{
    Selection* selection = const_cast<CellToolBase*>(this)->selection();
    if (!editor()) {
        QDomDocument doc = CopyCommand::saveAsXml(*selection);

        // Save to buffer
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        QTextStream str(&buffer);
        str.setCodec("UTF-8");
        str << doc;
        buffer.close();

        QMimeData* mimeData = new QMimeData();
        mimeData->setText(CopyCommand::saveAsPlainText(*selection));
        mimeData->setData("application/x-kspread-snippet", buffer.buffer());

        QApplication::clipboard()->setMimeData(mimeData);
    } else {
        editor()->copy();
    }
}

bool CellToolBase::paste()
{
    if (!selection()->activeSheet()->map()->isReadWrite()) // don't paste into a read only document
        return false;

    const QMimeData* mimeData = QApplication::clipboard()->mimeData(QClipboard::Clipboard);

    if (mimeData->hasFormat("application/vnd.oasis.opendocument.spreadsheet")) {
        QByteArray returnedTypeMime = "application/vnd.oasis.opendocument.spreadsheet";
        QByteArray arr = mimeData->data(returnedTypeMime);
        if (arr.isEmpty())
            return false;
        QBuffer buffer(&arr);
        KoStore * store = KoStore::createStore(&buffer, KoStore::Read);

        KoOdfReadStore odfStore(store);
        KoXmlDocument doc;
        QString errorMessage;
        bool ok = odfStore.loadAndParse("content.xml", doc, errorMessage);
        if (!ok) {
            kError(32001) << "Error parsing content.xml: " << errorMessage << endl;
            return false;
        }

        KoOdfStylesReader stylesReader;
        KoXmlDocument stylesDoc;
        (void)odfStore.loadAndParse("styles.xml", stylesDoc, errorMessage);
        // Load styles from style.xml
        stylesReader.createStyleMap(stylesDoc, true);
        // Also load styles from content.xml
        stylesReader.createStyleMap(doc, false);

        // from KSpreadDoc::loadOdf:
        KoXmlElement content = doc.documentElement();
        KoXmlElement realBody(KoXml::namedItemNS(content, KoXmlNS::office, "body"));
        if (realBody.isNull()) {
            kDebug(36005) << "Invalid OASIS OpenDocument file. No office:body tag found.";
            return false;
        }
        KoXmlElement body = KoXml::namedItemNS(realBody, KoXmlNS::office, "spreadsheet");

        if (body.isNull()) {
            kError(36005) << "No office:spreadsheet found!" << endl;
            KoXmlElement childElem;
            QString localName;
            forEachElement(childElem, realBody) {
                localName = childElem.localName();
            }
            return false;
        }

        KoOdfLoadingContext context(stylesReader, store);
        Q_ASSERT(!stylesReader.officeStyle().isNull());

        //load in first
        selection()->activeSheet()->map()->styleManager()->loadOdfStyleTemplate(stylesReader);

        // all <sheet:sheet> goes to workbook
        bool result = selection()->activeSheet()->map()->loadOdf(body, context);

        if (!result)
            return false;

        selection()->activeSheet()->map()->namedAreaManager()->loadOdf(body);
    }

    if (!editor()) {
        //kDebug(36005) <<"Pasting. Rect=" << selection()->lastRange() <<" bytes";
        PasteCommand *const command = new PasteCommand();
        command->setSheet(selection()->activeSheet());
        command->add(*selection());
        command->setMimeData(QApplication::clipboard()->mimeData());
        command->setPasteFC(true);
        command->execute(canvas());
        d->updateEditor(Cell(selection()->activeSheet(), selection()->cursor()));
    } else {
        editor()->paste();
    }
    selection()->emitModified();
    return true;
}

void CellToolBase::specialPaste()
{
    SpecialPasteDialog dialog(canvas()->canvasWidget(), selection());
    if (dialog.exec()) {
        selection()->emitModified();
    }
}

void CellToolBase::pasteWithInsertion()
{
    const QMimeData *const mimeData = QApplication::clipboard()->mimeData();
    if (!PasteCommand::unknownShiftDirection(mimeData)) {
        PasteCommand *const command = new PasteCommand();
        command->setSheet(selection()->activeSheet());
        command->add(*selection());
        command->setMimeData(mimeData);
        command->setInsertionMode(PasteCommand::ShiftCells);
        command->execute(canvas());
    } else {
        PasteInsertDialog dialog(canvas()->canvasWidget(), selection());
        dialog.exec();
    }
    d->updateEditor(Cell(selection()->activeSheet(), selection()->cursor()));
}

void CellToolBase::selectAll()
{
    selection()->selectAll();
}

void CellToolBase::find()
{
    FindDlg dialog(canvas()->canvasWidget(), "Find", d->findOptions, d->findStrings);
    dialog.setHasSelection(!selection()->isSingular());
    dialog.setHasCursor(true);
    if (KFindDialog::Accepted != dialog.exec())
        return;

    // Save for next time
    d->findOptions = dialog.options();
    d->findStrings = dialog.findHistory();
    d->typeValue = dialog.searchType();
    d->directionValue = dialog.searchDirection();

    // Create the KFind object
    delete d->find;
    delete d->replace;
    d->find = new KFind(dialog.pattern(), dialog.options(), canvas()->canvasWidget());
    d->replace = 0;
    d->replaceCommand = 0;

    d->searchInSheets.currentSheet = selection()->activeSheet();
    d->searchInSheets.firstSheet = d->searchInSheets.currentSheet;

    initFindReplace();
    findNext();
}

// Initialize a find or replace operation, using d->find or d->replace,
// and d->findOptions.
void CellToolBase::initFindReplace()
{
    KFind* findObj = d->find ? d->find : d->replace;
    Q_ASSERT(findObj);
    connect(findObj, SIGNAL(highlight(const QString &, int, int)),
            this, SLOT(slotHighlight(const QString &, int, int)));
    connect(findObj, SIGNAL(findNext()),
            this, SLOT(findNext()));

    bool bck = d->findOptions & KFind::FindBackwards;
    Sheet* currentSheet = d->searchInSheets.currentSheet;

    QRect region = (d->findOptions & KFind::SelectedText)
                   ? selection()->lastRange()
                   : QRect(1, 1, currentSheet->cellStorage()->columns(), currentSheet->cellStorage()->rows()); // All cells

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
    //kDebug() << d->findPos <<" to" << d->findEnd;
    //kDebug() <<"leftcol=" << d->findLeftColumn <<" rightcol=" << d->findRightColumn;
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
            //kDebug() <<"setData(cell" << d->findPos << ')';
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
    int maxRow = sheet->cellStorage()->rows();
    // kDebug() <<"findNextCell starting at" << col << ',' << row <<"   forw=" << forw;

    if (d->directionValue == FindOption::Row) {
        while (!cell && row != d->findEnd.y() && (forw ? row < maxRow : row >= 0)) {
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
            //kDebug() <<"next row:" << col << ',' << row;
        }
    } else {
        while (!cell && (forw ? col <= d->findRightColumn : col >= d->findLeftColumn)) {
            while (!cell && row != d->findEnd.y() && (forw ? row < maxRow : row >= 0)) {
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
            //kDebug() <<"next row:" << col << ',' << row;
        }
    }
    // if (!cell)
    // No more next cell - TODO go to next sheet (if not looking in a selection)
    // (and make d->findEnd(max, max) in that case...)
    // kDebug() <<" returning" << cell;
    return cell;
}

void CellToolBase::findPrevious()
{
    KFind* findObj = d->find ? d->find : d->replace;
    if (!findObj)  {
        find();
        return;
    }
    //kDebug() <<"findPrevious";
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
    SearchDlg dialog(canvas()->canvasWidget(), "Replace", d->findOptions, d->findStrings, d->replaceStrings);
    dialog.setHasSelection(!selection()->isSingular());
    dialog.setHasCursor(true);
    if (KReplaceDialog::Accepted != dialog.exec())
        return;

    d->findOptions = dialog.options();
    d->findStrings = dialog.findHistory();
    d->replaceStrings = dialog.replacementHistory();
    d->typeValue = dialog.searchType();

    delete d->find;
    delete d->replace;
    d->find = 0;
    // NOTE Stefan: Avoid beginning of line replacements with nothing which
    //              will lead to an infinite loop (Bug #125535). The reason
    //              for this is unclear to me, but who cares and who would
    //              want to do something like this, häh?!
    if (dialog.pattern() == "^" && dialog.replacement().isEmpty())
        return;
    d->replace = new KReplace(dialog.pattern(), dialog.replacement(), dialog.options());

    d->searchInSheets.currentSheet = selection()->activeSheet();
    d->searchInSheets.firstSheet = d->searchInSheets.currentSheet;
    initFindReplace();
    connect(d->replace, SIGNAL(replace(const QString &, int, int, int)),
            this, SLOT(slotReplace(const QString &, int, int, int)));

    d->replaceCommand = new QUndoCommand(i18n("Replace"));

    findNext();

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
    KDialog *dialog = 0;
    if (d->find)
        dialog = d->find->findNextDialog();
    else
        dialog = d->replace->replaceNextDialog();
    kDebug() << " baseDialog :" << dialog;
    QRect globalRect(d->findPos, d->findEnd);
    globalRect.moveTopLeft(canvas()->canvasWidget()->mapToGlobal(globalRect.topLeft()));
    KDialog::avoidArea(dialog, QRect(d->findPos, d->findEnd));
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
    GotoDialog dialog(canvas()->canvasWidget(), selection());
    dialog.exec();
    scrollToCell(selection()->cursor());
}

void CellToolBase::spellCheck()
{
    SpellCheckCommand* command = new SpellCheckCommand(selection(), canvas());
    command->start();
}

void CellToolBase::inspector()
{
    // useful to inspect objects
    Cell cell(selection()->activeSheet(), selection()->marker());
    KSpread::Inspector* ins = new KSpread::Inspector(cell);
    ins->exec();
    delete ins;
}

void CellToolBase::qTableView()
{
#ifndef NDEBUG
    KDialog* const dialog = new KDialog(canvas()->canvasWidget());
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
    AutoFormatDialog dialog(canvas()->canvasWidget(), selection());
    dialog.exec();
}

void CellToolBase::listChoosePopupMenu()
{
    if (!selection()->activeSheet()->map()->isReadWrite()) {
        return;
    }

    delete d->popupListChoose;
    d->popupListChoose = new QMenu();

    const Sheet *const sheet = selection()->activeSheet();
    const Cell cursorCell(sheet, selection()->cursor());
    const QString text = cursorCell.userInput();
    const CellStorage *const storage = sheet->cellStorage();

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

    for (QStringList::Iterator it = itemList.begin(); it != itemList.end(); ++it) {
        d->popupListChoose->addAction((*it));
    }

    if (itemList.isEmpty()) {
        return;
    }
    double tx = selection()->activeSheet()->columnPosition(selection()->marker().x());
    double ty = selection()->activeSheet()->rowPosition(selection()->marker().y());
    double h = cursorCell.height();
    const CellView cellView = sheetView(selection()->activeSheet())->cellView(selection()->marker().x(), selection()->marker().y());
    if (cellView.obscuresCells()) {
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
    connect(d->popupListChoose, SIGNAL(triggered(QAction*)),
            this, SLOT(listChooseItemSelected(QAction*)));
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
    DocumentSettingsDialog dialog(selection(), canvas()->canvasWidget());
    dialog.exec();
}

void CellToolBase::breakBeforeColumn(bool enable)
{
    PageBreakCommand *command = new PageBreakCommand();
    command->setSheet(selection()->activeSheet());
    command->setMode(PageBreakCommand::BreakBeforeColumn);
    command->setReverse(!enable);
    command->add(*selection());
    command->execute(canvas());
}

void CellToolBase::breakBeforeRow(bool enable)
{
    PageBreakCommand *command = new PageBreakCommand();
    command->setSheet(selection()->activeSheet());
    command->setMode(PageBreakCommand::BreakBeforeRow);
    command->setReverse(!enable);
    command->add(*selection());
    command->execute(canvas());
}
