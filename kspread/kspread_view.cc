/* This file is part of the KDE project
   Copyright (C) 2002-2005 Ariya Hidayat <ariya@kde.org>
             (C) 1999-2003 Laurent Montel <montel@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002-2003 John Dailey <dailey@vt.edu>
             (C) 1999-2003 David Faure <faure@kde.org>
             (C) 1999-2001 Simon Hausmann <hausmann@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include <kprinter.h> // has to be first

// standard C/C++ includes
#include <assert.h>
#include <stdlib.h>
#include <time.h>

// Qt includes
#include <qcursor.h>
#include <qlayout.h>
#include <qpaintdevicemetrics.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <qsqldatabase.h>

// KDE includes
#include <dcopclient.h>
#include <dcopref.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfind.h>
#include <kfinddialog.h>
#include <kfontdialog.h>
#include <kinputdialog.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <kpassdlg.h>
#include <kprocio.h>
#include <kreplace.h>
#include <kreplacedialog.h>
#include <kspell.h>
#include <kspelldlg.h>
#include <kstatusbar.h>
#include <kstdaction.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <kparts/partmanager.h>

// KOffice includes
#include <tkcoloractions.h>
#include <kdatatool.h>
#include <koCharSelectDia.h>
#include <kocommandhistory.h>
#include <koMainWindow.h>
#include <koPartSelectAction.h>
#include <kotabbar.h>
#include <kspread_toolbox.h>
#include <koTemplateCreateDia.h>
#include <kozoomaction.h>

// KSpread includes
#include "commands.h"
#include "damages.h"
#include "digest.h"
#include "handler.h"
#include "inspector.h"
#include "kspread_canvas.h"
#include "kspread_editors.h"
#include "kspread_events.h"
#include "kspread_global.h"
#include "kspread_handler.h"
#include "kspread_locale.h"
#include "kspread_map.h"
#include "kspread_selection.h"
#include "kspread_sheetprint.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "kspread_undo.h"
#include "testrunner.h"
#include "valuecalc.h"
#include "valueconverter.h"

// dialogs
#include "dialogs/kspread_dlg_angle.h"
#include "dialogs/kspread_dlg_area.h"
#include "dialogs/kspread_dlg_comment.h"
#include "dialogs/kspread_dlg_conditional.h"
#include "dialogs/kspread_dlg_cons.h"
#include "dialogs/kspread_dlg_csv.h"
#include "dialogs/kspread_dlg_database.h"
#include "dialogs/kspread_dlg_format.h"
#include "dialogs/kspread_dlg_formula.h"
#include "dialogs/kspread_dlg_goalseek.h"
#include "dialogs/kspread_dlg_goto.h"
#include "dialogs/kspread_dlg_insert.h"
#include "dialogs/kspread_dlg_layout.h"
#include "dialogs/kspread_dlg_list.h"
//#include "dialogs/kspread_dlg_multipleop.h"
#include "dialogs/kspread_dlg_paperlayout.h"
#include "dialogs/kspread_dlg_pasteinsert.h"
#include "dialogs/kspread_dlg_preference.h"
#include "dialogs/kspread_dlg_reference.h"
#include "dialogs/kspread_dlg_resize2.h"
#include "dialogs/kspread_dlg_series.h"
#include "dialogs/kspread_dlg_show.h"
#include "dialogs/kspread_dlg_showColRow.h"
#include "dialogs/kspread_dlg_sort.h"
#include "dialogs/kspread_dlg_special.h"
#include "dialogs/kspread_dlg_styles.h"
#include "dialogs/kspread_dlg_subtotal.h"
#include "dialogs/kspread_dlg_validity.h"
#include "dialogs/link.h"
#include "dialogs/sheet_properties.h"
#include "dialogs/kspread_dlg_find.h"

// KSpread DCOP
#include "KSpreadViewIface.h"

using namespace KSpread;

class ViewActions;

class ViewPrivate
{
public:
    KSpreadView* view;
    KSpreadDoc* doc;
    DCOPObject* dcop;

    // the active sheet, may be 0
    // this is the sheet which has the input focus
    KSpreadSheet* activeSheet;

    // GUI elements
    QWidget *frame;
    QFrame *toolWidget;
    KSpreadCanvas *canvas;
    KSpreadVBorder *vBorderWidget;
    KSpreadHBorder *hBorderWidget;
    QScrollBar *horzScrollBar;
    QScrollBar *vertScrollBar;
    KoTabBar *tabBar;
    KStatusBarLabel* calcLabel;

    // formulabar, consists of:
    QHBoxLayout* formulaBarLayout;
    KSpreadComboboxLocationEditWidget *posWidget;
    QButton* formulaButton;
    QButton *okButton;
    QButton *cancelButton;
    KSpreadEditWidget *editWidget;
    QGridLayout* viewLayout;
    QHBoxLayout* tabScrollBarLayout;

    // all UI actions
    ViewActions* actions;

    // If updateEditWidget is called it changes some KToggleActions.
    // That causes them to emit a signal. If this lock is true, then these
    // signals are ignored.
    bool toolbarLock;

    // if true, kspread is still loading the document
    // don't try to refresh the view
    bool loading;

    // selection/marker
    KSpreadSelection* selectionInfo;
    QMap<KSpreadSheet*, QPoint> savedAnchors;
    QMap<KSpreadSheet*, QPoint> savedMarkers;

    // Find and Replace context. We remember the options and
    // the strings used previously.
    long findOptions;
    QStringList findStrings;
    QStringList replaceStrings;
    KSpreadFindOption::searchTypeValue typeValue;
    KSpreadFindOption::searchDirectionValue directionValue;
    // Current "find" operation
    KFind* find;
    KReplace* replace;
    int findLeftColumn;
    int findRightColumn;
    QPoint findPos;
    QPoint findEnd;

    KSpreadInsertHandler* insertHandler;

    // Insert special character dialog
    KoCharSelectDia* specialCharDlg;

    // Holds a guarded pointer to the transformation toolbox.
    QGuardedPtr<KoTransformToolBox> transformToolBox;

    // the last popup menu (may be 0).
    // Since only one popup menu can be opened at once, its pointer is stored here.
    // Delete the old one before you store a pointer to anotheron here.
    QPopupMenu *popupMenu;
    int popupMenuFirstToolId;

    QPopupMenu *popupRow;
    QPopupMenu *popupColumn;
    QPopupMenu* popupChild;       // for embedded children
    QPopupMenu* popupListChoose;  // for list of choose

    // the child for which the popup menu has been opened.
    KSpreadChild* popupChildObject;

    // spell-check context
    struct
    {
      KSpell *   kspell;
      KSpreadSheet *  firstSpellSheet;
      KSpreadSheet *  currentSpellSheet;
      KSpreadCell  *  currentCell;
      KSpreadMacroUndoAction *macroCmdSpellCheck;
      unsigned int    spellCurrCellX;
      unsigned int    spellCurrCellY;
      unsigned int    spellStartCellX;
      unsigned int    spellStartCellY;
      unsigned int    spellEndCellX;
      unsigned int    spellEndCellY;
      bool            spellCheckSelection;
      QStringList replaceAll;
    } spell;

    struct
    {
        KSpreadSheet * currentSheet;
        KSpreadSheet * firstSheet;
    } searchInSheets;

    // the tools
    struct ToolEntry
    {
      QString command;
      KDataToolInfo info;
    };
    QPtrList<ToolEntry> toolList;

    void initActions();
    void adjustActions( bool mode );
    void adjustActions( KSpreadSheet* sheet, KSpreadCell* cell );
    void adjustWorkbookActions( bool mode );
    void updateButton( KSpreadCell *cell, int column, int row);
    QButton* newIconButton( const char *_file, bool _kbutton = false, QWidget *_parent = 0L );
};

class ViewActions
{
public:

    // cell formatting
    KAction* cellLayout;
    KAction* defaultFormat;
    KToggleAction* bold;
    KToggleAction* italic;
    KToggleAction* underline;
    KToggleAction* strikeOut;
    KFontAction* selectFont;
    KFontSizeAction* selectFontSize;
    KAction* fontSizeUp;
    KAction* fontSizeDown;
    TKSelectColorAction* textColor;
    KToggleAction* alignLeft;
    KToggleAction* alignCenter;
    KToggleAction* alignRight;
    KToggleAction* alignTop;
    KToggleAction* alignMiddle;
    KToggleAction* alignBottom;
    KToggleAction* wrapText;
    KToggleAction* verticalText;
    KAction* increaseIndent;
    KAction* decreaseIndent;
    KAction* changeAngle;
    KToggleAction* percent;
    KAction* precplus;
    KAction* precminus;
    KToggleAction* money;
    KAction* upper;
    KAction* lower;
    KAction* firstLetterUpper;
    TKSelectColorAction* bgColor;
    KAction* borderLeft;
    KAction* borderRight;
    KAction* borderTop;
    KAction* borderBottom;
    KAction* borderAll;
    KAction* borderOutline;
    KAction* borderRemove;
    TKSelectColorAction* borderColor;
    KSelectAction* selectStyle;
    KAction* createStyle;

    // cell operations
    KAction* editCell;
    KAction* insertCell;
    KAction* removeCell;
    KAction* deleteCell;
    KAction* mergeCell;
    KAction* dissociateCell;
    KAction* clearText;
    KAction* conditional;
    KAction* clearConditional;
    KAction* validity;
    KAction* clearValidity;
    KAction* addModifyComment;
    KAction* removeComment;
    KAction* clearComment;

    // column & row operations
    KAction* resizeColumn;
    KAction* insertColumn;
    KAction* deleteColumn;
    KAction* hideColumn;
    KAction* showColumn;
    KAction* equalizeColumn;
    KAction* showSelColumns;
    KAction* resizeRow;
    KAction* insertRow;
    KAction* deleteRow;
    KAction* hideRow;
    KAction* showRow;
    KAction* equalizeRow;
    KAction* showSelRows;
    KAction* adjust;

    // sheet/workbook operations
    KAction* sheetProperties;
    KAction* insertSheet;
    KAction* menuInsertSheet;
    KAction* removeSheet;
    KAction* renameSheet;
    KAction* hideSheet;
    KAction* showSheet;
    KAction* autoFormat;
    KAction* areaName;
    KAction* showArea;
    KAction* insertSeries;
    KAction* insertFunction;
    KAction* insertSpecialChar;
    KAction* insertFromDatabase;
    KAction* insertFromTextfile;
    KAction* insertFromClipboard;
    KAction* transform;
    KAction* sort;
    KAction* sortDec;
    KAction* sortInc;
    KAction* fillRight;
    KAction* fillLeft;
    KAction* fillUp;
    KAction* fillDown;
    KAction* paperLayout;
    KAction* definePrintRange;
    KAction* resetPrintRange;
    KToggleAction* showPageBorders;
    KAction* recalcWorksheet;
    KAction* recalcWorkbook;
    KToggleAction* protectSheet;
    KToggleAction* protectDoc;

    // general editing
    KAction* cut;
    KAction* copy;
    KAction* paste;
    KAction* specialPaste;
    KAction* insertCellCopy;
    KAction* find;
    KAction* replace;

    // navigation
    KAction* gotoCell;
    KAction* nextSheet;
    KAction* prevSheet;
    KAction* firstSheet;
    KAction* lastSheet;

    // misc
    KAction* styleDialog;
    KAction* autoSum;
    KSelectAction* formulaSelection;
    KAction* insertLink;
    KAction* removeLink;
    KAction* consolidate;
    KAction* goalSeek;
    KAction* subTotals;
    KAction* textToColumns;
    KAction* multipleOperations;
    KAction* createTemplate;
    KoPartSelectAction *insertPart;
    KAction* insertChartFrame;
    KAction* customList;
    KAction* spellChecking;
    KAction* internalTests;
    KAction* inspector;

    // settings
    KoZoomAction* viewZoom;
    KToggleAction* showStatusBar;
    KToggleAction* showTabBar;
    KToggleAction* showFormulaBar;
    KToggleAction* showCommentIndicator;
    KAction* preference;

    // running calculation
    KToggleAction* calcNone;
    KToggleAction* calcMin;
    KToggleAction* calcMax;
    KToggleAction* calcAverage;
    KToggleAction* calcCount;
    KToggleAction* calcSum;
    KToggleAction* calcCountA;
};


void ViewPrivate::initActions()
{
  actions = new ViewActions;

  KActionCollection* ac = view->actionCollection();

  // -- cell formatting actions --

  actions->cellLayout = new KAction( i18n("Cell Format..."), "cell_layout",
      Qt::CTRL+ Qt::ALT+ Qt::Key_F, view, SLOT( layoutDlg() ), ac, "cellLayout" );
  actions->cellLayout->setToolTip( i18n("Set the cell formatting.") );

  actions->defaultFormat = new KAction( i18n("Default"),
      0, view, SLOT( defaultSelection() ), ac, "default" );
  actions->defaultFormat->setToolTip( i18n("Resets to the default format.") );

  actions->bold = new KToggleAction( i18n("Bold"), "text_bold",
      Qt::CTRL+Qt::Key_B, ac, "bold");
  QObject::connect( actions->bold, SIGNAL( toggled( bool) ),
      view, SLOT( bold( bool ) ) );

  actions->italic = new KToggleAction( i18n("Italic"), "text_italic",
      Qt::CTRL+Qt::Key_I, ac, "italic");
  QObject::connect( actions->italic, SIGNAL( toggled( bool) ),
      view, SLOT( italic( bool ) ) );

  actions->underline = new KToggleAction( i18n("Underline"), "text_under",
      Qt::CTRL+Qt::Key_U, ac, "underline");
  QObject::connect( actions->underline, SIGNAL( toggled( bool) ),
      view, SLOT( underline( bool ) ) );

  actions->strikeOut = new KToggleAction( i18n("Strike Out"), "text_strike",
      0, ac, "strikeout");
  QObject::connect( actions->strikeOut, SIGNAL( toggled( bool) ),
      view, SLOT( strikeOut( bool ) ) );

  actions->selectFont = new KFontAction( i18n("Select Font..."),
      0, ac, "selectFont" );
  QObject::connect( actions->selectFont, SIGNAL( activated( const QString& ) ),
      view, SLOT( fontSelected( const QString& ) ) );

  actions->selectFontSize = new KFontSizeAction( i18n("Select Font Size"),
      0, ac, "selectFontSize" );
  QObject::connect( actions->selectFontSize, SIGNAL( fontSizeChanged( int ) ),
      view, SLOT( fontSizeSelected( int ) ) );

  actions->fontSizeUp = new KAction( i18n("Increase Font Size"), "fontsizeup",
      0, view, SLOT( increaseFontSize() ), ac,  "increaseFontSize" );

  actions->fontSizeDown = new KAction( i18n("Decrease Font Size"), "fontsizedown",
      0, view, SLOT( decreaseFontSize() ), ac, "decreaseFontSize" );

  actions->textColor = new TKSelectColorAction( i18n("Text Color"),
      TKSelectColorAction::TextColor, view, SLOT( changeTextColor() ),
      ac, "textColor",true );
  actions->textColor->setDefaultColor(QColor());

  actions->alignLeft = new KToggleAction( i18n("Align Left"), "text_left",
      0, ac, "left");
  QObject::connect( actions->alignLeft, SIGNAL( toggled( bool ) ),
      view, SLOT( alignLeft( bool ) ) );
  actions->alignLeft->setExclusiveGroup( "Align" );
  actions->alignLeft->setToolTip(i18n("Left justify the cell contents."));

  actions->alignCenter = new KToggleAction( i18n("Align Center"), "text_center",
      0, ac, "center");
  QObject::connect( actions->alignCenter, SIGNAL( toggled( bool ) ),
      view, SLOT( alignCenter( bool ) ) );
  actions->alignCenter->setExclusiveGroup( "Align" );
  actions->alignCenter->setToolTip(i18n("Center the cell contents."));

  actions->alignRight = new KToggleAction( i18n("Align Right"), "text_right",
      0, ac, "right");
  QObject::connect( actions->alignRight, SIGNAL( toggled( bool ) ),
      view, SLOT( alignRight( bool ) ) );
  actions->alignRight->setExclusiveGroup( "Align" );
  actions->alignRight->setToolTip(i18n("Right justify the cell contents."));

  actions->alignTop = new KToggleAction( i18n("Align Top"), "text_top",
      0, ac, "top");
  QObject::connect( actions->alignTop, SIGNAL( toggled( bool ) ),
      view, SLOT( alignTop( bool ) ) );
  actions->alignTop->setExclusiveGroup( "Pos" );
  actions->alignTop->setToolTip(i18n("Align cell contents along the top of the cell."));

  actions->alignMiddle = new KToggleAction( i18n("Align Middle"), "middle",
      0, ac, "middle");
  QObject::connect( actions->alignMiddle, SIGNAL( toggled( bool ) ),
      view, SLOT( alignMiddle( bool ) ) );
  actions->alignMiddle->setExclusiveGroup( "Pos" );
  actions->alignMiddle->setToolTip(i18n("Align cell contents centered in the cell."));

  actions->alignBottom = new KToggleAction( i18n("Align Bottom"), "text_bottom",
      0, ac, "bottom");
  QObject::connect( actions->alignBottom, SIGNAL( toggled( bool ) ),
      view, SLOT( alignBottom( bool ) ) );
  actions->alignBottom->setExclusiveGroup( "Pos" );
  actions->alignBottom->setToolTip(i18n("Align cell contents along the bottom of the cell."));

  actions->wrapText = new KToggleAction( i18n("Wrap Text"), "multirow",
      0, ac, "multiRow" );
  QObject::connect( actions->wrapText, SIGNAL( toggled( bool ) ),
      view, SLOT( wrapText( bool ) ) );
  actions->wrapText->setToolTip(i18n("Make the cell text wrap onto multiple lines."));

  actions->verticalText = new KToggleAction( i18n("Vertical Text"),"vertical_text" ,
      0 ,ac, "verticaltext" );
  QObject::connect( actions->verticalText, SIGNAL( toggled( bool ) ),
      view, SLOT( verticalText( bool ) ) );
  actions->verticalText->setToolTip(i18n("Print cell contents vertically."));

  actions->increaseIndent = new KAction( i18n("Increase Indent"),
      QApplication::reverseLayout() ? "format_decreaseindent":"format_increaseindent",
      0, view, SLOT( increaseIndent() ), ac, "increaseindent" );
  actions->increaseIndent->setToolTip(i18n("Increase the indentation."));

  actions->decreaseIndent = new KAction( i18n("Decrease Indent"),
      QApplication::reverseLayout() ? "format_increaseindent" : "format_decreaseindent",
      0, view, SLOT( decreaseIndent() ), ac, "decreaseindent");
  actions->decreaseIndent->setToolTip(i18n("Decrease the indentation."));

  actions->changeAngle = new KAction( i18n("Change Angle..."),
      0, view, SLOT( changeAngle() ), ac, "changeangle" );
  actions->changeAngle->setToolTip(i18n("Change the angle that cell contents are printed."));

  actions->percent = new KToggleAction( i18n("Percent Format"), "percent",
      0, ac, "percent");
  QObject::connect( actions->percent, SIGNAL( toggled( bool ) ),
      view, SLOT( percent( bool ) ) );
  actions->percent->setToolTip(i18n("Set the cell formatting to look like a percentage."));

  actions->precplus = new KAction( i18n("Increase Precision"), "prec_plus",
      0, view, SLOT( precisionPlus() ), ac, "precplus");
  actions->precplus->setToolTip(i18n("Increase the decimal precision shown onscreen."));

  actions->precminus = new KAction( i18n("Decrease Precision"), "prec_minus",
      0, view, SLOT( precisionMinus() ), ac, "precminus");
  actions->precminus->setToolTip(i18n("Decrease the decimal precision shown onscreen."));

  actions->money = new KToggleAction( i18n("Money Format"), "money",
      0, ac, "money");
  QObject::connect( actions->money, SIGNAL( toggled( bool ) ),
      view, SLOT( moneyFormat( bool ) ) );
  actions->money->setToolTip(i18n("Set the cell formatting to look like your local currency."));

  actions->upper = new KAction( i18n("Upper Case"), "fontsizeup",
      0, view, SLOT( upper() ), ac, "upper" );
  actions->upper->setToolTip(i18n("Convert all letters to upper case."));

  actions->lower = new KAction( i18n("Lower Case"), "fontsizedown",
      0, view, SLOT( lower() ), ac, "lower" );
  actions->lower->setToolTip(i18n("Convert all letters to lower case."));

  actions->firstLetterUpper = new KAction( i18n("Convert First Letter to Upper Case"), "first_letter_upper",
      0, view, SLOT( firstLetterUpper() ),ac, "firstletterupper" );
  actions->firstLetterUpper->setToolTip(i18n("Capitalize the first letter."));

  actions->bgColor = new TKSelectColorAction( i18n("Background Color"),
      TKSelectColorAction::FillColor, ac, "backgroundColor", true );
  QObject::connect(actions->bgColor, SIGNAL( activated() ),
      view, SLOT( changeBackgroundColor() ) );
  actions->bgColor->setDefaultColor(QColor());
  actions->bgColor->setToolTip(i18n("Set the background color."));

  actions->borderLeft = new KAction( i18n("Border Left"), "border_left",
      0, view, SLOT( borderLeft() ), ac, "borderLeft" );
  actions->borderLeft->setToolTip(i18n("Set a left border to the selected area."));

  actions->borderRight = new KAction( i18n("Border Right"), "border_right",
      0, view, SLOT( borderRight() ), ac, "borderRight" );
  actions->borderRight->setToolTip(i18n("Set a right border to the selected area."));

  actions->borderTop = new KAction( i18n("Border Top"), "border_top",
      0, view, SLOT( borderTop() ), ac, "borderTop" );
  actions->borderTop->setToolTip(i18n("Set a top border to the selected area."));

  actions->borderBottom = new KAction( i18n("Border Bottom"), "border_bottom",
      0, view, SLOT( borderBottom() ), ac, "borderBottom" );
  actions->borderBottom->setToolTip(i18n("Set a bottom border to the selected area."));

  actions->borderAll = new KAction( i18n("All Borders"), "border_all",
      0, view, SLOT( borderAll() ), ac, "borderAll" );
  actions->borderAll->setToolTip(i18n("Set a border around all cells in the selected area."));

  actions->borderRemove = new KAction( i18n("Remove Borders"), "border_remove",
      0, view, SLOT( borderRemove() ), ac, "borderRemove" );
  actions->borderRemove->setToolTip(i18n("Remove all borders in the selected area."));

  actions->borderOutline = new KAction( i18n("Border Outline"), ("border_outline"),
      0, view, SLOT( borderOutline() ), ac, "borderOutline" );
  actions->borderOutline->setToolTip(i18n("Set a border to the outline of the selected area."));

  actions->borderColor = new TKSelectColorAction( i18n("Border Color"),
      TKSelectColorAction::LineColor, ac, "borderColor" );
  QObject::connect( actions->borderColor, SIGNAL( activated() ),
      view, SLOT( changeBorderColor() ) );
  actions->borderColor->setToolTip( i18n( "Select a new border color." ) );

  actions->selectStyle = new KSelectAction( i18n( "St&yle" ),
      0, ac, "stylemenu" );
  actions->selectStyle->setToolTip( i18n( "Apply a predefined style to the selected cells." ) );
  QObject::connect( actions->selectStyle, SIGNAL( activated( const QString & ) ),
      view, SLOT( styleSelected( const QString & ) ) );

  actions->createStyle = new KAction( i18n( "Create Style From Cell..." ),
      0, view, SLOT( createStyleFromCell()), ac, "createStyle" );
  actions->createStyle->setToolTip( i18n( "Create a new style based on the currently selected cell." ) );

  // -- cell operation actions --

  actions->editCell = new KAction( i18n("Modify Cell"),"cell_edit",
      Qt::CTRL+Qt::Key_M, view, SLOT( editCell() ), ac, "editCell" );
  actions->editCell->setToolTip(i18n("Edit the highlighted cell."));

  actions->insertCell = new KAction( i18n("Insert Cells..."), "insertcell",
      0, view, SLOT( slotInsert() ), ac, "insertCell" );
  actions->insertCell->setToolTip(i18n("Insert a blank cell into the spreadsheet."));

  actions->removeCell = new KAction( i18n("Remove Cells..."), "removecell",
      0, view, SLOT( slotRemove() ), ac, "removeCell" );
  actions->removeCell->setToolTip(i18n("Removes the current cell from the spreadsheet."));

  actions->deleteCell = new KAction( i18n("Delete"), "deletecell",
      0, view, SLOT( deleteSelection() ), ac, "delete" );
  actions->deleteCell->setToolTip(i18n("Delete all contents and formatting of the current cell."));

  actions->mergeCell = new KAction( i18n("Merge Cells"),"mergecell",
      0, view, SLOT( mergeCell() ), ac, "mergecell" );
  actions->mergeCell->setToolTip(i18n("Merge the selected region into one large cell."));

  actions->dissociateCell = new KAction( i18n("Dissociate Cells"),"dissociatecell",
      0, view, SLOT( dissociateCell() ), ac, "dissociatecell" );
  actions->dissociateCell->setToolTip(i18n("Unmerge the current cell."));

  actions->clearText = new KAction( i18n("Text"),
      0, view, SLOT( clearTextSelection() ), ac, "cleartext" );
  actions->clearText->setToolTip(i18n("Remove the contents of the current cell."));

  actions->conditional = new KAction( i18n("Conditional Cell Attributes..."),
      0, view, SLOT( conditional() ), ac, "conditional" );
  actions->conditional->setToolTip(i18n("Set cell format based on certain conditions."));


  actions->clearConditional = new KAction( i18n("Conditional Cell Attributes"),
      0, view, SLOT( clearConditionalSelection() ), ac, "clearconditional" );
  actions->clearConditional->setToolTip(i18n("Remove the conditional cell formatting."));

  actions->validity = new KAction( i18n("Validity..."),
      0, view, SLOT( validity() ), ac, "validity" );
  actions->validity->setToolTip(i18n("Set tests to confirm cell data is valid."));

  actions->clearValidity = new KAction( i18n("Validity"),
      0, view, SLOT( clearValiditySelection() ), ac, "clearvalidity" );
  actions->clearValidity->setToolTip(i18n("Remove the validity tests on this cell."));

  actions->addModifyComment = new KAction( i18n("&Add/Modify Comment..."),"comment",
      0, view, SLOT( addModifyComment() ), ac, "addmodifycomment" );
  actions->addModifyComment->setToolTip(i18n("Edit a comment for this cell."));

  actions->removeComment = new KAction( i18n("&Remove Comment"),"removecomment",
      0,  view, SLOT( removeComment() ), ac, "removecomment" );
  actions->removeComment->setToolTip(i18n("Remove this cell's comment."));

  actions->clearComment = new KAction( i18n("Comment"),
      0, view, SLOT( clearCommentSelection() ), ac, "clearcomment" );
  actions->clearComment->setToolTip(i18n("Remove this cell's comment."));

  // -- column & row actions --

  actions->resizeColumn = new KAction( i18n("Resize Column..."), "resizecol",
      0, view, SLOT( resizeColumn() ), ac, "resizeCol" );
  actions->resizeColumn->setToolTip(i18n("Change the width of a column."));

  actions->insertColumn = new KAction( i18n("Insert Columns"), "insert_table_col",
      0, view, SLOT( insertColumn() ), ac, "insertColumn" );
  actions->insertColumn->setToolTip(i18n("Inserts a new column into the spreadsheet."));

  actions->deleteColumn = new KAction( i18n("Delete Columns"), "delete_table_col",
      0, view, SLOT( deleteColumn() ), ac, "deleteColumn" );
  actions->deleteColumn->setToolTip(i18n("Removes a column from the spreadsheet."));

  actions->hideColumn = new KAction( i18n("Hide Columns"), "hide_table_column",
      0, view, SLOT( hideColumn() ), ac, "hideColumn" );
  actions->hideColumn->setToolTip(i18n("Hide the column from view."));

  actions->showColumn = new KAction( i18n("Show Columns..."), "show_table_column",
      0, view, SLOT( showColumn() ), ac, "showColumn" );
  actions->showColumn->setToolTip(i18n("Show hidden columns."));

  actions->equalizeColumn = new KAction( i18n("Equalize Column"), "adjustcol",
      0, view, SLOT( equalizeColumn() ), ac, "equalizeCol" );
  actions->equalizeColumn->setToolTip(i18n("Resizes selected columns to be the same size."));

  actions->showSelColumns = new KAction( i18n("Show Columns"), "show_sheet_column",
      0, view, SLOT( showSelColumns() ), ac, "showSelColumns" );
  actions->showSelColumns->setToolTip(i18n("Show hidden columns in the selection."));
  actions->showSelColumns->setEnabled(false);

  actions->resizeRow = new KAction( i18n("Resize Row..."), "resizerow",
      0, view, SLOT( resizeRow() ), ac, "resizeRow" );
  actions->resizeRow->setToolTip(i18n("Change the height of a row."));

  actions->insertRow = new KAction( i18n("Insert Rows"), "insert_table_row",
      0, view, SLOT( insertRow() ), ac, "insertRow" );
  actions->insertRow->setToolTip(i18n("Inserts a new row into the spreadsheet."));

  actions->deleteRow = new KAction( i18n("Delete Rows"), "delete_table_row",
      0, view, SLOT( deleteRow() ), ac, "deleteRow" );
  actions->deleteRow->setToolTip(i18n("Removes a row from the spreadsheet."));

  actions->hideRow = new KAction( i18n("Hide Rows"), "hide_table_row",
      0, view, SLOT( hideRow() ), ac, "hideRow" );
  actions->hideRow->setToolTip(i18n("Hide a row from view."));

  actions->showRow = new KAction( i18n("Show Rows..."), "show_table_row",
      0, view, SLOT( showRow() ), ac, "showRow" );
  actions->showRow->setToolTip(i18n("Show hidden rows."));

  actions->equalizeRow = new KAction( i18n("Equalize Row"), "adjustrow",
      0, view, SLOT( equalizeRow() ), ac, "equalizeRow" );
  actions->equalizeRow->setToolTip(i18n("Resizes selected rows to be the same size."));

  actions->showSelRows = new KAction( i18n("Show Rows"), "show_table_row",
      0, view, SLOT( showSelRows() ), ac, "showSelRows" );
  actions->showSelRows->setEnabled(false);
  actions->showSelRows->setToolTip(i18n("Show hidden rows in the selection."));

  actions->adjust = new KAction( i18n("Adjust Row && Column"),
      0, view, SLOT( adjust() ), ac, "adjust" );
  actions->adjust->setToolTip(i18n("Adjusts row/column size so that the contents will fit."));

  // -- sheet/workbook actions --
  actions->sheetProperties = new KAction( i18n("Sheet Properties"),
      0, view, SLOT( sheetProperties() ), ac, "sheetProperties" );
  actions->sheetProperties->setToolTip(i18n("Modify current sheet's properties."));

  actions->insertSheet = new KAction( i18n("Insert Sheet"),"inserttable",
      0, view, SLOT( insertSheet() ), ac, "insertSheet" );
  actions->insertSheet->setToolTip(i18n("Insert a new sheet."));

  // same action as insertSheet, but without 'insert' in the caption
  actions->menuInsertSheet = new KAction( i18n("&Sheet"),"inserttable",
      0, view, SLOT( insertSheet() ), ac, "menuInsertSheet" );
  actions->menuInsertSheet->setToolTip(i18n("Insert a new sheet."));

  actions->removeSheet = new KAction( i18n("Remove Sheet"), "delete_table",
      0, view, SLOT( removeSheet() ), ac, "removeSheet" );
  actions->removeSheet->setToolTip(i18n("Remove the active sheet."));

  actions->renameSheet=new KAction( i18n("Rename Sheet..."),
      0, view, SLOT( slotRename() ), ac, "renameSheet" );
  actions->renameSheet->setToolTip(i18n("Rename the active sheet."));

  actions->showSheet = new KAction(i18n("Show Sheet..."),
      0, view, SLOT( showSheet()), ac, "showSheet" );
  actions->showSheet->setToolTip(i18n("Show a hidden sheet."));

  actions->hideSheet = new KAction(i18n("Hide Sheet"),
      0, view, SLOT( hideSheet() ), ac, "hideSheet" );
  actions->hideSheet->setToolTip(i18n("Hide the active sheet."));

  actions->autoFormat = new KAction( i18n("AutoFormat..."),
      0, view, SLOT( sheetFormat() ), ac, "sheetFormat" );
  actions->autoFormat->setToolTip(i18n("Set the worksheet formatting."));

  actions->areaName = new KAction( i18n("Area Name..."),
      0, view, SLOT( setAreaName() ), ac, "areaname" );
  actions->areaName->setToolTip(i18n("Set a name for a region of the spreadsheet."));

  actions->showArea = new KAction( i18n("Show Area..."),
      0, view, SLOT( showAreaName() ), ac, "showArea" );
  actions->showArea->setToolTip(i18n("Display a named area."));

  actions->insertFunction = new KAction( i18n("&Function..."), "funct",
      0, view, SLOT( insertMathExpr() ), ac, "insertMathExpr" );
  actions->insertFunction->setToolTip(i18n("Insert math expression."));

  actions->insertSeries = new KAction( i18n("&Series..."),"series",
      0, view, SLOT( insertSeries() ), ac, "series");
  actions->insertSeries ->setToolTip(i18n("Insert a series."));

  actions->insertLink = new KAction( i18n("&Link..."), "insert_link",
      0, view, SLOT( insertHyperlink() ), ac, "insertHyperlink" );
  actions->insertLink->setToolTip(i18n("Insert an Internet hyperlink."));

  actions->removeLink = new KAction( i18n("&Remove Link"),
      0, view, SLOT( removeHyperlink() ), ac, "removeHyperlink" );
  actions->removeLink->setToolTip(i18n("Remove a link."));

  actions->insertSpecialChar = new KAction( i18n( "S&pecial Character..." ), "char",
      view, SLOT( insertSpecialChar() ), ac, "insertSpecialChar" );
  actions->insertSpecialChar->setToolTip( i18n( "Insert one or more symbols or letters not found on the keyboard." ) );

  actions->insertPart = new KoPartSelectAction( i18n("&Object"), "frame_query",
      view, SLOT( insertObject() ), ac, "insertPart");
  actions->insertPart->setToolTip(i18n("Insert an object from another program."));

  actions->insertChartFrame = new KAction( i18n("&Chart"), "insert_chart",
      0, view, SLOT( insertChart() ), ac, "insertChart" );
  actions->insertChartFrame->setToolTip(i18n("Insert a chart."));

#ifndef QT_NO_SQL
  actions->insertFromDatabase = new KAction( i18n("From &Database..."),
      0, view, SLOT( insertFromDatabase() ),  ac, "insertFromDatabase");
  actions->insertFromDatabase->setToolTip(i18n("Insert data from a SQL database."));
#endif

  actions->insertFromTextfile = new KAction( i18n("From &Text File..."),
      0, view,  SLOT( insertFromTextfile() ), ac, "insertFromTextfile");
  actions->insertFromTextfile->setToolTip(i18n("Insert data from a text file to the current cursor position/selection."));

  actions->insertFromClipboard = new KAction( i18n("From &Clipboard..."),
      0, view, SLOT( insertFromClipboard() ), ac, "insertFromClipboard");
  actions->insertFromClipboard->setToolTip(i18n("Insert CSV data from the clipboard to the current cursor position/selection."));

  actions->transform = new KAction( i18n("Transform Object..."), "rotate",
      0, view, SLOT( transformPart() ), ac, "transform" );
  actions->transform->setToolTip(i18n("Rotate the contents of the cell."));
  actions->transform->setEnabled( false );

  actions->sort = new KAction( i18n("&Sort..."),
      0, view, SLOT( sort() ), ac, "sort" );
  actions->sort->setToolTip(i18n("Sort a group of cells."));

  actions->sortDec = new KAction( i18n("Sort &Decreasing"), "sort_decrease",
      0, view, SLOT( sortDec() ), ac, "sortDec" );
  actions->sortDec->setToolTip(i18n("Sort a group of cells in decreasing (last to first) order."));

  actions->sortInc = new KAction( i18n("Sort &Increasing"), "sort_incr",
      0, view, SLOT( sortInc() ), ac, "sortInc" );
  actions->sortInc->setToolTip(i18n("Sort a group of cells in ascending (first to last) order."));

  actions->paperLayout = new KAction( i18n("Page Layout..."),
      0, view, SLOT( paperLayoutDlg() ), ac, "paperLayout" );
  actions->paperLayout->setToolTip(i18n("Specify the layout of the spreadsheet for a printout."));

  actions->definePrintRange = new KAction( i18n("Define Print Range"),
      0, view, SLOT( definePrintRange() ), ac, "definePrintRange" );
  actions->definePrintRange->setToolTip(i18n("Define the print range in the current sheet."));

  actions->resetPrintRange = new KAction( i18n("Reset Print Range"),
      0, view, SLOT( resetPrintRange() ), ac, "resetPrintRange" );
  actions->definePrintRange->setToolTip(i18n("Define the print range in the current sheet."));

  actions->showPageBorders = new KToggleAction( i18n("Show Page Borders"),
      0, ac, "showPageBorders");
  actions->showPageBorders->setCheckedState(i18n("Hide Page Borders"));
  QObject::connect( actions->showPageBorders, SIGNAL( toggled( bool ) ),
      view, SLOT( togglePageBorders( bool ) ) );
  actions->showPageBorders->setToolTip( i18n( "Show on the spreadsheet where the page borders will be." ) );

  actions->recalcWorksheet = new KAction( i18n("Recalculate Sheet"),
      Qt::SHIFT + Qt::Key_F9, view, SLOT( recalcWorkSheet() ), ac, "RecalcWorkSheet" );
  actions->recalcWorksheet->setToolTip(i18n("Recalculate the value of every cell in the current worksheet."));

  actions->recalcWorkbook = new KAction( i18n("Recalculate Workbook"),
      Qt::Key_F9, view, SLOT( recalcWorkBook() ), ac, "RecalcWorkBook" );
  actions->recalcWorkbook->setToolTip(i18n("Recalculate the value of every cell in all worksheets."));

  actions->protectSheet = new KToggleAction( i18n( "Protect &Sheet..." ),
      0, ac, "protectSheet" );
  actions->protectSheet->setToolTip( i18n( "Protect the sheet from being modified." ) );
  QObject::connect( actions->protectSheet, SIGNAL( toggled( bool ) ),
      view, SLOT( toggleProtectSheet( bool ) ) );

  actions->protectDoc = new KToggleAction( i18n( "Protect &Doc..." ),
      0, ac, "protectDoc" );
  actions->protectDoc->setToolTip( i18n( "Protect the document from being modified." ) );
  QObject::connect( actions->protectDoc, SIGNAL( toggled( bool ) ),
      view, SLOT( toggleProtectDoc( bool ) ) );

  // -- editing actions --

  actions->copy = KStdAction::copy( view, SLOT( copySelection() ), ac, "copy" );
  actions->copy->setToolTip(i18n("Copy the cell object to the clipboard."));

  actions->paste = KStdAction::paste( view, SLOT( paste() ), ac, "paste" );
  actions->paste->setToolTip(i18n("Paste the contents of the clipboard at the cursor."));

  actions->cut = KStdAction::cut( view, SLOT( cutSelection() ), ac, "cut" );
  actions->cut->setToolTip(i18n("Move the cell object to the clipboard."));

  actions->specialPaste = new KAction( i18n("Special Paste..."), "special_paste",
      0, view, SLOT( specialPaste() ), ac, "specialPaste" );
  actions->specialPaste->setToolTip(i18n("Paste the contents of the clipboard with special options."));

  actions->insertCellCopy = new KAction( i18n("Paste with Insertion"), "insertcellcopy",
      0, view, SLOT( slotInsertCellCopy() ), ac, "insertCellCopy" );
  actions->insertCellCopy->setToolTip(i18n("Inserts a cell from the clipboard into the spreadsheet."));

  actions->find = KStdAction::find( view, SLOT(find()), ac );
  /*actions->findNext =*/ KStdAction::findNext( view, SLOT( findNext() ), ac );
  /*actions->findPrevious =*/ KStdAction::findPrev( view, SLOT( findPrevious() ), ac );

  actions->replace = KStdAction::replace( view, SLOT(replace()), ac );

  actions->fillRight = new KAction( i18n( "&Right" ), 0,
      0, view, SLOT( fillRight() ), ac, "fillRight" );

  actions->fillLeft = new KAction( i18n( "&Left" ), 0,
      0, view, SLOT( fillLeft() ), ac, "fillLeft" );

  actions->fillDown = new KAction( i18n( "&Down" ), 0,
      0, view, SLOT( fillDown() ), ac, "fillDown" );

  actions->fillUp = new KAction( i18n( "&Up" ), 0,
      0, view, SLOT( fillUp() ), ac, "fillUp" );

  // -- misc actions --

  actions->styleDialog = new KAction( i18n( "Style Manager" ),
      0, view, SLOT( styleDialog() ), ac, "styles" );
  actions->styleDialog->setToolTip( i18n( "Edit and organize cell styles." ) );

  actions->autoSum = new KAction( i18n("Autosum"), "black_sum",
      0, view, SLOT( autoSum() ), ac, "autoSum" );
  actions->autoSum->setToolTip(i18n("Insert the 'sum' function"));

  actions->spellChecking = KStdAction::spelling( view, SLOT( extraSpelling() ),
      ac, "spelling" );
  actions->spellChecking->setToolTip(i18n("Check the spelling."));

  actions->formulaSelection = new KSelectAction(i18n("Formula Selection"),
      0, ac, "formulaSelection");
  actions->formulaSelection->setToolTip(i18n("Insert a function."));
  QStringList lst;
  lst.append( "SUM");
  lst.append( "AVERAGE");
  lst.append( "IF");
  lst.append( "COUNT");
  lst.append( "MIN");
  lst.append( "MAX");
  lst.append( i18n("Others...") );
  ((KSelectAction*) actions->formulaSelection)->setItems( lst );
  actions->formulaSelection->setComboWidth( 80 );
  actions->formulaSelection->setCurrentItem(0);
  QObject::connect( actions->formulaSelection, SIGNAL( activated( const QString& ) ),
      view, SLOT( formulaSelection( const QString& ) ) );

  actions->viewZoom = new KoZoomAction( i18n( "Zoom" ), "viewmag", 0, ac, "view_zoom" );
  QObject::connect( actions->viewZoom, SIGNAL( zoomChanged( const QString & ) ),
      view, SLOT( viewZoom( const QString & ) ) );

  actions->consolidate = new KAction( i18n("&Consolidate..."),
      0, view, SLOT( consolidate() ), ac, "consolidate" );
  actions->consolidate->setToolTip(i18n("Create a region of summary data from a group of similar regions."));

  actions->goalSeek = new KAction( i18n("&Goal Seek..."),
      0, view, SLOT( goalSeek() ), ac, "goalSeek" );
  actions->goalSeek->setToolTip( i18n("Repeating calculation to find a specific value.") );

  actions->subTotals = new KAction( i18n("&Subtotals..."),
      0, view, SLOT( subtotals() ), ac, "subtotals" );
  actions->subTotals->setToolTip( i18n("Create different kind of subtotals to a list or database.") );

  actions->textToColumns = new KAction( i18n("&Text to Columns..."),
      0, view, SLOT( textToColumns() ), ac, "textToColumns" );
  actions->textToColumns->setToolTip( i18n("Expand the content of cells to multiple columns.") );

  actions->multipleOperations = new KAction( i18n("&Multiple Operations..."),
      0, view, SLOT( multipleOperations() ), ac, "multipleOperations" );
  actions->multipleOperations->setToolTip( i18n("Apply the same formula to various cells using different values for the parameter.") );

  actions->createTemplate = new KAction( i18n( "&Create Template From Document..." ),
      0, view, SLOT( createTemplate() ), ac, "createTemplate" );

  actions->customList = new KAction( i18n("Custom Lists..."),
      0, view, SLOT( sortList() ), ac, "sortlist" );
  actions->customList->setToolTip(i18n("Create custom lists for sorting or autofill."));

  // -- navigation actions --

  actions->gotoCell = new KAction( i18n("Goto Cell..."),"goto",
      0, view, SLOT( gotoCell() ), ac, "gotoCell" );
  actions->gotoCell->setToolTip(i18n("Move to a particular cell."));

  actions->nextSheet = new KAction( i18n("Next Sheet"), "forward",
      Qt::CTRL+Qt::Key_PageDown, view, SLOT( nextSheet() ), ac, "nextSheet");
  actions->nextSheet->setToolTip(i18n("Move to the next sheet."));

  actions->prevSheet = new KAction( i18n("Previous Sheet"), "back",
      Qt::CTRL+Qt::Key_PageUp, view, SLOT( previousSheet() ), ac, "previousSheet");
  actions->prevSheet->setToolTip(i18n("Move to the previous sheet."));

  actions->firstSheet = new KAction( i18n("First Sheet"), "start",
      0, view, SLOT( firstSheet() ), ac, "firstSheet");
  actions->firstSheet->setToolTip(i18n("Move to the first sheet."));

  actions->lastSheet = new KAction( i18n("Last Sheet"), "finish",
      0, view, SLOT( lastSheet() ), ac, "lastSheet");
  actions->lastSheet->setToolTip(i18n("Move to the last sheet."));

  // -- settings actions --

  actions->showStatusBar = new KToggleAction( i18n("Show Status Bar"),
      0, ac, "showStatusBar" );
  actions->showStatusBar->setCheckedState(i18n("Hide Status Bar"));
  QObject::connect( actions->showStatusBar, SIGNAL( toggled( bool ) ),
      view, SLOT( showStatusBar( bool ) ) );
  actions->showStatusBar->setToolTip(i18n("Show the status bar."));

  actions->showTabBar = new KToggleAction( i18n("Show Tab Bar"),
      0, ac, "showTabBar" );
  actions->showTabBar->setCheckedState(i18n("Hide Tab Bar"));
  QObject::connect( actions->showTabBar, SIGNAL( toggled( bool ) ),
      view, SLOT( showTabBar( bool ) ) );
  actions->showTabBar->setToolTip(i18n("Show the tab bar."));

  actions->showFormulaBar = new KToggleAction( i18n("Show Formula Bar"),
      0, ac, "showFormulaBar" );
  actions->showFormulaBar->setCheckedState(i18n("Hide Formula Bar"));
  QObject::connect( actions->showFormulaBar, SIGNAL( toggled( bool ) ),
      view, SLOT( showFormulaBar( bool ) ) );
  actions->showFormulaBar->setToolTip(i18n("Show the formula bar."));

  actions->showCommentIndicator = new KToggleAction( i18n("Show Comment Indicator"),
      0, ac, "showCommentIndicator" );
  actions->showCommentIndicator->setCheckedState(i18n("Hide Comment Indicator"));
  QObject::connect( actions->showCommentIndicator, SIGNAL( toggled( bool ) ),
      view, SLOT( showCommentIndicator( bool ) ) );
  actions->showCommentIndicator->setToolTip(i18n("Show indicator for cells with comments."));

  actions->preference = new KAction( i18n("Configure KSpread..."),"configure",
      0, view, SLOT( preference() ), ac, "preference" );
  actions->preference->setToolTip(i18n("Set various KSpread options."));

  // -- running calculation actions --

  actions->calcNone = new KToggleAction( i18n("None"), 0, ac, "menu_none");
  QObject::connect( actions->calcNone, SIGNAL( toggled( bool ) ),
      view, SLOT( menuCalc( bool ) ) );
  actions->calcNone->setExclusiveGroup( "Calc" );
  actions->calcNone->setToolTip(i18n("No calculation"));

  actions->calcSum = new KToggleAction( i18n("Sum"), 0, ac, "menu_sum");
  QObject::connect( actions->calcSum, SIGNAL( toggled( bool ) ),
      view, SLOT( menuCalc( bool ) ) );
  actions->calcSum->setExclusiveGroup( "Calc" );
  actions->calcSum->setToolTip(i18n("Calculate using sum."));

  actions->calcMin = new KToggleAction( i18n("Min"), 0, ac, "menu_min");
  QObject::connect( actions->calcMin, SIGNAL( toggled( bool ) ),
      view, SLOT( menuCalc( bool ) ) );
  actions->calcMin->setExclusiveGroup( "Calc" );
  actions->calcMin->setToolTip(i18n("Calculate using minimum."));

  actions->calcMax = new KToggleAction( i18n("Max"), 0, ac, "menu_max");
  QObject::connect( actions->calcMax, SIGNAL( toggled( bool ) ),
      view, SLOT( menuCalc( bool ) ) );
  actions->calcMax->setExclusiveGroup( "Calc" );
  actions->calcMax->setToolTip(i18n("Calculate using maximum."));

  actions->calcAverage = new KToggleAction( i18n("Average"), 0, ac, "menu_average");
  QObject::connect( actions->calcAverage, SIGNAL( toggled( bool ) ),
      view, SLOT( menuCalc( bool ) ) );
  actions->calcAverage->setExclusiveGroup( "Calc" );
  actions->calcAverage->setToolTip(i18n("Calculate using average."));

  actions->calcCount = new KToggleAction( i18n("Count"), 0, ac, "menu_count");
  QObject::connect( actions->calcCount, SIGNAL( toggled( bool ) ),
      view, SLOT( menuCalc( bool ) ) );
  actions->calcCount->setExclusiveGroup( "Calc" );
  actions->calcCount->setToolTip(i18n("Calculate using the count."));

  actions->calcCountA = new KToggleAction( i18n("CountA"), 0, ac, "menu_counta");
  QObject::connect( actions->calcCountA, SIGNAL( toggled( bool ) ),
      view, SLOT( menuCalc( bool ) ) );
  actions->calcCountA->setExclusiveGroup( "Calc" );
  actions->calcCountA->setToolTip(i18n("Calculate using the countA."));

  // -- special action, only for developers --

  actions->internalTests = new KAction( i18n("Run Internal Tests..."), "internalTests",
      Qt::CTRL+ Qt::SHIFT + Qt::Key_T, view, SLOT( runInternalTests() ), ac, "internalTests" );
  actions->inspector = new KAction( i18n("Run Inspector..."), "inspector",
      Qt::CTRL+ Qt::SHIFT + Qt::Key_I, view, SLOT( runInspector() ), ac, "inspector" );
}

void ViewPrivate::adjustActions( bool mode )
{
  actions->replace->setEnabled( mode );
  actions->insertSeries->setEnabled( mode );
  actions->insertLink->setEnabled( mode );
  actions->insertSpecialChar->setEnabled( mode );
  actions->insertFunction->setEnabled( mode );
  actions->removeComment->setEnabled( mode );
  actions->decreaseIndent->setEnabled( mode );
  actions->bold->setEnabled( mode );
  actions->italic->setEnabled( mode );
  actions->underline->setEnabled( mode );
  actions->strikeOut->setEnabled( mode );
  actions->percent->setEnabled( mode );
  actions->precplus->setEnabled( mode );
  actions->precminus->setEnabled( mode );
  actions->money->setEnabled( mode );
  actions->alignLeft->setEnabled( mode );
  actions->alignCenter->setEnabled( mode );
  actions->alignRight->setEnabled( mode );
  actions->alignTop->setEnabled( mode );
  actions->alignMiddle->setEnabled( mode );
  actions->alignBottom->setEnabled( mode );
  actions->paste->setEnabled( mode );
  actions->cut->setEnabled( mode );
  actions->specialPaste->setEnabled( mode );
  actions->deleteCell->setEnabled( mode );
  actions->clearText->setEnabled( mode );
  actions->clearComment->setEnabled( mode );
  actions->clearValidity->setEnabled( mode );
  actions->clearConditional->setEnabled( mode );
  actions->recalcWorkbook->setEnabled( mode );
  actions->recalcWorksheet->setEnabled( mode );
  actions->adjust->setEnabled( mode );
  actions->editCell->setEnabled( mode );
  actions->paperLayout->setEnabled( mode );
  actions->styleDialog->setEnabled( mode );
  actions->definePrintRange->setEnabled( mode );
  actions->resetPrintRange->setEnabled( mode );
  actions->insertFromDatabase->setEnabled( mode );
  actions->insertFromTextfile->setEnabled( mode );
  actions->insertFromClipboard->setEnabled( mode );
  actions->conditional->setEnabled( mode );
  actions->validity->setEnabled( mode );
  actions->goalSeek->setEnabled( mode );
  actions->subTotals->setEnabled( mode );
  actions->multipleOperations->setEnabled( mode );
  actions->textToColumns->setEnabled( mode );
  actions->consolidate->setEnabled( mode );
  actions->insertCellCopy->setEnabled( mode );
  actions->wrapText->setEnabled( mode );
  actions->selectFont->setEnabled( mode );
  actions->selectFontSize->setEnabled( mode );
  actions->deleteColumn->setEnabled( mode );
  actions->hideColumn->setEnabled( mode );
  actions->showColumn->setEnabled( mode );
  actions->showSelColumns->setEnabled( mode );
  actions->insertColumn->setEnabled( mode );
  actions->deleteRow->setEnabled( mode );
  actions->insertRow->setEnabled( mode );
  actions->hideRow->setEnabled( mode );
  actions->showRow->setEnabled( mode );
  actions->showSelRows->setEnabled( mode );
  actions->formulaSelection->setEnabled( mode );
  actions->textColor->setEnabled( mode );
  actions->bgColor->setEnabled( mode );
  actions->cellLayout->setEnabled( mode );
  actions->borderLeft->setEnabled( mode );
  actions->borderRight->setEnabled( mode );
  actions->borderTop->setEnabled( mode );
  actions->borderBottom->setEnabled( mode );
  actions->borderAll->setEnabled( mode );
  actions->borderOutline->setEnabled( mode );
  actions->borderRemove->setEnabled( mode );
  actions->borderColor->setEnabled( mode );
  actions->removeSheet->setEnabled( mode );
  actions->autoSum->setEnabled( mode );
  actions->defaultFormat->setEnabled( mode );
  actions->areaName->setEnabled( mode );
  actions->resizeRow->setEnabled( mode );
  actions->resizeColumn->setEnabled( mode );
  actions->fontSizeUp->setEnabled( mode );
  actions->fontSizeDown->setEnabled( mode );
  actions->upper->setEnabled( mode );
  actions->lower->setEnabled( mode );
  actions->equalizeRow->setEnabled( mode );
  actions->equalizeColumn->setEnabled( mode );
  actions->verticalText->setEnabled( mode );
  actions->addModifyComment->setEnabled( mode );
  actions->removeComment->setEnabled( mode );
  actions->insertCell->setEnabled( mode );
  actions->removeCell->setEnabled( mode );
  actions->changeAngle->setEnabled( mode );
  actions->dissociateCell->setEnabled( mode );
  actions->increaseIndent->setEnabled( mode );
  actions->decreaseIndent->setEnabled( mode );
  actions->spellChecking->setEnabled( mode );
  actions->calcMin->setEnabled( mode );
  actions->calcMax->setEnabled( mode );
  actions->calcAverage->setEnabled( mode );
  actions->calcCount->setEnabled( mode );
  actions->calcCountA->setEnabled( mode );
  actions->calcSum->setEnabled( mode );
  actions->calcNone->setEnabled( mode );
  actions->insertPart->setEnabled( mode );
  actions->createStyle->setEnabled( mode );
  actions->selectStyle->setEnabled( mode );

  actions->autoFormat->setEnabled( false );
  actions->sort->setEnabled( false );
  actions->mergeCell->setEnabled( false );
  actions->insertChartFrame->setEnabled( false );
  actions->sortDec->setEnabled( false );
  actions->sortInc->setEnabled( false );
  actions->transform->setEnabled( false );

  actions->fillRight->setEnabled( false );
  actions->fillLeft->setEnabled( false );
  actions->fillUp->setEnabled( false );
  actions->fillDown->setEnabled( false );

  if ( mode && !view->doc()->map()->isProtected() )
    actions->renameSheet->setEnabled( true );
  else
    actions->renameSheet->setEnabled( false );

  actions->showStatusBar->setChecked( view->doc()->showStatusBar() );
  actions->showTabBar->setChecked( view->doc()->showTabBar() );
  actions->showFormulaBar->setChecked( view->doc()->showFormulaBar() );
  actions->showCommentIndicator->setChecked( view->doc()->showCommentIndicator() );

  formulaButton->setEnabled( mode );

  if ( activeSheet )
    view->canvasWidget()->gotoLocation( selectionInfo->marker(), activeSheet );
}

void ViewPrivate::adjustActions( KSpreadSheet* sheet, KSpreadCell* cell )
{
  QRect selection = selectionInfo->selection();
  if ( sheet->isProtected() && !cell->isDefault() && cell->notProtected( cell->column(), cell->row() ) )
  {
    if ( ( selection.width() > 1 ) || ( selection.height() > 1 ) )
    {
      if ( actions->bold->isEnabled() )
        adjustActions( false );
    }
    else
    {
      if ( !actions->bold->isEnabled() )
        adjustActions( true );
    }
  }
  else if ( sheet->isProtected() )
  {
    if ( actions->bold->isEnabled() )
      adjustActions( false );
  }
}

void ViewPrivate::adjustWorkbookActions( bool mode )
{
  actions->hideSheet->setEnabled( mode );
  actions->showSheet->setEnabled( mode );
  actions->insertSheet->setEnabled( mode );
  actions->menuInsertSheet->setEnabled( mode );
  actions->removeSheet->setEnabled( mode );

  if ( mode )
  {
    if ( activeSheet && !activeSheet->isProtected() )
    {
      bool state = ( view->doc()->map()->visibleSheets().count() > 1 );
      actions->removeSheet->setEnabled( state );
      actions->hideSheet->setEnabled( state );
    }
    actions->showSheet->setEnabled( view->doc()->map()->hiddenSheets().count() > 0 );
    actions->renameSheet->setEnabled( activeSheet && !activeSheet->isProtected() );
  }
}

// TODO this should be merged with adjustActions
void ViewPrivate::updateButton( KSpreadCell *cell, int column, int row)
{
    toolbarLock = true;

    // workaround for bug #59291 (crash upon starting from template)
    // certain Qt and Fontconfig combination fail miserably if can not
    // find the font name (e.g. not installed in the system)
    QStringList fontList;
    KFontChooser::getFontList( fontList, 0 );
    QString fontFamily = cell->textFontFamily( column,row );
    for ( QStringList::Iterator it = fontList.begin(); it != fontList.end(); ++it )
      if ((*it).lower() == fontFamily.lower())
      {
        actions->selectFont->setFont( fontFamily );
        break;
      }

    actions->selectFontSize->setFontSize( cell->textFontSize( column, row ) );
    actions->bold->setChecked( cell->textFontBold( column, row ) );
    actions->italic->setChecked( cell->textFontItalic(  column, row) );
    actions->underline->setChecked( cell->textFontUnderline( column, row ) );
    actions->strikeOut->setChecked( cell->textFontStrike( column, row ) );

    actions->alignLeft->setChecked( cell->align( column, row ) == KSpreadFormat::Left );
    actions->alignCenter->setChecked( cell->align( column, row ) == KSpreadFormat::Center );
    actions->alignRight->setChecked( cell->align( column, row ) == KSpreadFormat::Right );

    actions->alignTop->setChecked( cell->alignY( column, row ) == KSpreadFormat::Top );
    actions->alignMiddle->setChecked( cell->alignY( column, row ) == KSpreadFormat::Middle );
    actions->alignBottom->setChecked( cell->alignY( column, row ) == KSpreadFormat::Bottom );

    actions->verticalText->setChecked( cell->verticalText( column,row ) );

    actions->wrapText->setChecked( cell->multiRow( column,row ) );

    FormatType ft = cell->formatType();
    actions->percent->setChecked( ft == Percentage_format );
    actions->money->setChecked( ft == Money_format );

    if ( activeSheet && !activeSheet->isProtected() )
      actions->removeComment->setEnabled( !cell->comment(column,row).isEmpty() );

    if ( activeSheet && !activeSheet->isProtected() )
      actions->decreaseIndent->setEnabled( cell->getIndent( column, row ) > 0.0 );

    toolbarLock = false;
    if ( activeSheet )
      adjustActions( activeSheet, cell );
}



QButton* ViewPrivate::newIconButton( const char *_file, bool _kbutton, QWidget *_parent )
{
  if ( _parent == 0L )
    _parent = view;

  QButton *pb;
  if ( !_kbutton )
    pb = new QPushButton( _parent );
  else
    pb = new QToolButton( _parent );
  pb->setPixmap( QPixmap( KSBarIcon(_file) ) );

  return pb;
}

/*****************************************************************************
 *
 * KSpreadView
 *
 *****************************************************************************/

KSpreadView::KSpreadView( QWidget *_parent, const char *_name,
    KSpreadDoc *_doc )
  : KoView( _doc, _parent, _name )
{
    ElapsedTime et( "KSpreadView constructor" );
    kdDebug(36001) << "sizeof(KSpreadCell)=" << sizeof(KSpreadCell) <<endl;

    d = new ViewPrivate;
    d->view = this;
    d->doc = _doc;

    d->dcop = 0;

    d->activeSheet = 0;

    d->toolbarLock = false;
    d->loading = false;

    d->selectionInfo = new KSpreadSelection( this );

    d->findOptions = 0;
    d->findLeftColumn = 0;
    d->findRightColumn = 0;
    d->typeValue = KSpreadFindOption::Value;
    d->directionValue = KSpreadFindOption::Row;
    d->find = 0;
    d->replace = 0;

    d->popupMenuFirstToolId = 0;
    d->popupMenu   = 0;
    d->popupColumn = 0;
    d->popupRow    = 0;
    d->popupChild   = 0;
    d->popupListChoose = 0;
    d->popupChildObject = 0;

    d->searchInSheets.currentSheet = 0;
    d->searchInSheets.firstSheet = 0;

    // spell-check context
    d->spell.kspell = 0;
    d->spell.macroCmdSpellCheck = 0;
    d->spell.firstSpellSheet = 0;
    d->spell.currentSpellSheet = 0;
    d->spell.currentCell = 0;
    d->spell.spellStartCellX = 0;
    d->spell.spellStartCellY = 0;
    d->spell.spellEndCellX   = 0;
    d->spell.spellEndCellY   = 0;
    d->spell.spellCheckSelection = false;

    d->insertHandler = 0L;
    d->specialCharDlg = 0;

    setInstance( KSpreadFactory::global() );
    if ( doc()->isReadWrite() )
      setXMLFile( "kspread.rc" );
    else
      setXMLFile( "kspread_readonly.rc" );

    // build the DCOP object
    dcopObject();

    connect( doc()->commandHistory(), SIGNAL( commandExecuted() ),
        this, SLOT( commandExecuted() ) );

    // GUI Initializations
    initView();

    d->initActions();


    // Handler for moving and resizing embedded parts
    ContainerHandler* h = new ContainerHandler( this, d->canvas );
    connect( h, SIGNAL( popupMenu( KoChild*, const QPoint& ) ), this, SLOT( popupChildMenu( KoChild*, const QPoint& ) ) );


    connect( this, SIGNAL( childSelected( KoDocumentChild* ) ),
             this, SLOT( slotChildSelected( KoDocumentChild* ) ) );
    connect( this, SIGNAL( childUnselected( KoDocumentChild* ) ),
             this, SLOT( slotChildUnselected( KoDocumentChild* ) ) );
    // If a selected part becomes active this is like it is deselected
    // just before.
    connect( this, SIGNAL( childActivated( KoDocumentChild* ) ),
             this, SLOT( slotChildUnselected( KoDocumentChild* ) ) );

    QObject::connect( doc()->map(), SIGNAL( sig_addSheet( KSpreadSheet* ) ), SLOT( slotAddSheet( KSpreadSheet* ) ) );

    QObject::connect( doc(), SIGNAL( sig_refreshView(  ) ), this, SLOT( slotRefreshView() ) );

    QObject::connect( doc(), SIGNAL( sig_refreshLocale() ), this, SLOT( refreshLocale()));

    QObject::connect( doc(), SIGNAL( sig_addAreaName( const QString & ) ), d->posWidget, SLOT( slotAddAreaName( const QString & ) ) );

    QObject::connect( doc(), SIGNAL( sig_removeAreaName( const QString & ) ), d->posWidget, SLOT( slotRemoveAreaName( const QString & ) ) );

    QObject::connect( doc(), SIGNAL( damagesFlushed( const QValueList<KSpread::Damage*>& ) ),
        this, SLOT( handleDamages( const QValueList<KSpread::Damage*>& ) ) );

    KoView::setZoom( doc()->zoomedResolutionY() /* KoView only supports one zoom */ ); // initial value
    //when kspread is embedded into konqueror apply a zoom=100
    //in konqueror we can't change zoom -- ### TODO ?
    if (!doc()->isReadWrite())
    {
        setZoom( 100, true );
    }

    viewZoom( QString::number( doc()->zoom() ) );

    // ## Might be wrong, if doc isn't loaded yet
    d->actions->selectStyle->setItems( d->doc->styleManager()->styleNames() );

    // If doc was already loaded, initialize things
    // Otherwise the doc will do it in completeLoading.
    if ( !doc()->map()->sheetList().isEmpty() )
        initialPosition();
}

KSpreadView::~KSpreadView()
{
    //  ElapsedTime el( "~KSpreadView" );
    if ( doc()->isReadWrite() ) // make sure we're not embedded in Konq
        deleteEditor( true );
    if ( !d->transformToolBox.isNull() )
        delete (&*d->transformToolBox);
    /*if (d->calcLabel)
    {
        disconnect(d->calcLabel,SIGNAL(pressed( int )),this,SLOT(statusBarClicked(int)));

        }*/

    delete d->selectionInfo;
    delete d->spell.kspell;

    d->canvas->endChoose();
    d->activeSheet = 0; // set the active sheet to 0L so that when during destruction
    // of embedded child documents possible repaints in KSpreadSheet are not
    // performed. The repains can happen if you delete an embedded document,
    // which leads to an regionInvalidated() signal emission in KoView, which calls
    // repaint, etc.etc. :-) (Simon)

    delete d->popupColumn;
    delete d->popupRow;
    delete d->popupMenu;
    delete d->popupChild;
    delete d->popupListChoose;
    delete d->calcLabel;
    delete d->dcop;

    delete d->insertHandler;
    d->insertHandler = 0L;

    delete d->actions;
    delete d;
}

KSpreadDoc* KSpreadView::doc()
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

void KSpreadView::initView()
{
    d->viewLayout = new QGridLayout( this, 3, 4 );

    // Vert. Scroll Bar
    d->calcLabel  = 0;
    d->vertScrollBar = new QScrollBar( this, "ScrollBar_2" );
    d->vertScrollBar->setRange( 0, 4096 );
    d->vertScrollBar->setOrientation( QScrollBar::Vertical );
    d->vertScrollBar->setLineStep(60);  //just random guess based on what feels okay
    d->vertScrollBar->setPageStep(60);  //This should be controlled dynamically, depending on how many rows are shown

    // Edit Bar
    d->toolWidget = new QFrame( this );

    d->formulaBarLayout = new QHBoxLayout( d->toolWidget );
    d->formulaBarLayout->setMargin( 4 );
    d->formulaBarLayout->addSpacing( 2 );

    d->posWidget = new KSpreadComboboxLocationEditWidget( d->toolWidget, this );
    d->posWidget->setMinimumWidth( 100 );
    d->formulaBarLayout->addWidget( d->posWidget );
    d->formulaBarLayout->addSpacing( 6 );

    d->formulaButton = d->newIconButton( "funct", true, d->toolWidget );
    d->formulaBarLayout->addWidget( d->formulaButton );
    d->formulaBarLayout->addSpacing( 2 );
    connect( d->formulaButton, SIGNAL( clicked() ), SLOT( insertMathExpr() ) );

    d->cancelButton = d->newIconButton( "cancel", true, d->toolWidget );
    d->formulaBarLayout->addWidget( d->cancelButton );
    d->okButton = d->newIconButton( "ok", true, d->toolWidget );
    d->formulaBarLayout->addWidget( d->okButton );
    d->formulaBarLayout->addSpacing( 6 );

    // The widget on which we display the sheet
    d->canvas = new KSpreadCanvas( this );

    // The line-editor that appears above the sheet and allows to
    // edit the cells content. It knows about the two buttons.
    d->editWidget = new KSpreadEditWidget( d->toolWidget, d->canvas, d->cancelButton, d->okButton );
    d->editWidget->setFocusPolicy( QWidget::StrongFocus );
    d->formulaBarLayout->addWidget( d->editWidget, 2 );
    d->formulaBarLayout->addSpacing( 2 );

    d->canvas->setEditWidget( d->editWidget );

    d->hBorderWidget = new KSpreadHBorder( this, d->canvas,this );
    d->vBorderWidget = new KSpreadVBorder( this, d->canvas ,this );

    d->canvas->setFocusPolicy( QWidget::StrongFocus );
    QWidget::setFocusPolicy( QWidget::StrongFocus );
    setFocusProxy( d->canvas );

    connect( this, SIGNAL( invalidated() ), d->canvas, SLOT( update() ) );

    QWidget* bottomPart = new QWidget( this );
    d->tabScrollBarLayout = new QHBoxLayout( bottomPart );
    d->tabScrollBarLayout->setAutoAdd( true );
    d->tabBar = new KoTabBar( bottomPart );
    d->horzScrollBar = new QScrollBar( bottomPart, "ScrollBar_1" );

    d->horzScrollBar->setRange( 0, 4096 );
    d->horzScrollBar->setOrientation( QScrollBar::Horizontal );

    d->horzScrollBar->setLineStep(60); //just random guess based on what feels okay
    d->horzScrollBar->setPageStep(60);

    QObject::connect( d->tabBar, SIGNAL( tabChanged( const QString& ) ), this, SLOT( changeSheet( const QString& ) ) );
    QObject::connect( d->tabBar, SIGNAL( tabMoved( unsigned, unsigned ) ),
      this, SLOT( moveSheet( unsigned, unsigned ) ) );
    QObject::connect( d->tabBar, SIGNAL( contextMenu( const QPoint& ) ),
      this, SLOT( popupTabBarMenu( const QPoint& ) ) );
    QObject::connect( d->tabBar, SIGNAL( doubleClicked() ),
      this, SLOT( slotRename() ) );

    d->viewLayout->setColStretch( 1, 10 );
    d->viewLayout->setRowStretch( 2, 10 );
    d->viewLayout->addMultiCellWidget( d->toolWidget, 0, 0, 0, 2 );
    d->viewLayout->addMultiCellWidget( d->hBorderWidget, 1, 1, 1, 2 );
    d->viewLayout->addWidget( d->vBorderWidget, 2, 0 );
    d->viewLayout->addWidget( d->canvas, 2, 1 );
    d->viewLayout->addWidget( d->vertScrollBar, 2, 2 );
    d->viewLayout->addMultiCellWidget( bottomPart, 3, 3, 0, 2 );

    KStatusBar * sb = statusBar();
    Q_ASSERT(sb);
    d->calcLabel = sb ? new KStatusBarLabel( QString::null, 0, sb ) : 0;
    addStatusBarItem( d->calcLabel, 0 );
    if (d->calcLabel)
        connect(d->calcLabel ,SIGNAL(itemPressed( int )),this,SLOT(statusBarClicked(int)));

    // signal slot
    QObject::connect( d->vertScrollBar, SIGNAL( valueChanged(int) ), d->canvas, SLOT( slotScrollVert(int) ) );
    QObject::connect( d->horzScrollBar, SIGNAL( valueChanged(int) ), d->canvas, SLOT( slotScrollHorz(int) ) );

}

KSpreadCanvas* KSpreadView::canvasWidget() const
{
    return d->canvas;
}

KSpreadHBorder* KSpreadView::hBorderWidget()const
{
    return d->hBorderWidget;
}

KSpreadVBorder* KSpreadView::vBorderWidget()const
{
    return d->vBorderWidget;
}

QScrollBar* KSpreadView::horzScrollBar()const
{
    return d->horzScrollBar;
}

QScrollBar* KSpreadView::vertScrollBar()const
{
    return d->vertScrollBar;
}

KSpreadEditWidget* KSpreadView::editWidget()const
{
    return d->editWidget;
}

KSpreadComboboxLocationEditWidget* KSpreadView::posWidget()const
{
    return d->posWidget;
}

KoTabBar* KSpreadView::tabBar() const
{
    return d->tabBar;
}

bool KSpreadView::isLoading() const
{
    return d->loading;
}

KSpreadSelection* KSpreadView::selectionInfo() const
{
    return d->selectionInfo;
}

QRect KSpreadView::selection(bool extend) const
{
    return selectionInfo()->selection(extend);
}

QPoint KSpreadView::marker() const
{
    return selectionInfo()->marker();
}

void KSpreadView::resetInsertHandle()
{
    d->insertHandler = 0L;
}

const KSpreadSheet* KSpreadView::activeSheet() const
{
    return d->activeSheet;
}

KSpreadSheet* KSpreadView::activeSheet()
{
    return d->activeSheet;
}

void KSpreadView::initConfig()
{
    KConfig *config = KSpreadFactory::global()->config();
    if ( config->hasGroup("Parameters" ))
    {
        config->setGroup( "Parameters" );
        if ( !doc()->configLoadFromFile() )
            doc()->setShowHorizontalScrollBar(config->readBoolEntry("Horiz ScrollBar",true));
        if ( !doc()->configLoadFromFile() )
            doc()->setShowVerticalScrollBar(config->readBoolEntry("Vert ScrollBar",true));
        doc()->setShowColHeader(config->readBoolEntry("Column Header",true));
        doc()->setShowRowHeader(config->readBoolEntry("Row Header",true));
        if ( !doc()->configLoadFromFile() )
            doc()->setCompletionMode((KGlobalSettings::Completion)config->readNumEntry("Completion Mode",(int)(KGlobalSettings::CompletionAuto)));
        doc()->setMoveToValue((KSpread::MoveTo)config->readNumEntry("Move",(int)(KSpread::Bottom)));
        doc()->setIndentValue( config->readDoubleNumEntry( "Indent", 10.0 ) );
        doc()->setTypeOfCalc((MethodOfCalc)config->readNumEntry("Method of Calc",(int)(SumOfNumber)));
        if ( !doc()->configLoadFromFile() )
            doc()->setShowTabBar(config->readBoolEntry("Tabbar",true));

	doc()->setShowMessageError(config->readBoolEntry( "Msg error" ,false) );

	doc()->setShowCommentIndicator(config->readBoolEntry("Comment Indicator",true));

	doc()->setShowFormulaBar(config->readBoolEntry("Formula bar",true));
        doc()->setShowStatusBar(config->readBoolEntry("Status bar",true));

        changeNbOfRecentFiles(config->readNumEntry("NbRecentFile",10));
        //autosave value is stored as a minute.
        //but default value is stored as seconde.
        doc()->setAutoSave(config->readNumEntry("AutoSave",KoDocument::defaultAutoSave()/60)*60);
        doc()->setBackupFile( config->readBoolEntry("BackupFile",true));
    }

   if (  config->hasGroup("KSpread Color" ) )
   {
     config->setGroup( "KSpread Color" );
     QColor _col(Qt::lightGray);
     _col = config->readColorEntry("GridColor", &_col);
     doc()->setGridColor(_col);

     QColor _pbCol(Qt::red);
     _pbCol = config->readColorEntry("PageBorderColor", &_pbCol);
     doc()->changePageBorderColor(_pbCol);
   }

// Do we need a Page Layout in the congiguration file? Isn't this already in the template? Philipp
/*
if ( config->hasGroup("KSpread Page Layout" ) )
 {
   config->setGroup( "KSpread Page Layout" );
   if ( d->activeSheet->isEmpty())
     {
	d->activeSheet->setPaperFormat((KoFormat)config->readNumEntry("Default size page",1));

	d->activeSheet->setPaperOrientation((KoOrientation)config->readNumEntry("Default orientation page",0));
	d->activeSheet->setPaperUnit((KoUnit::Unit)config->readNumEntry("Default unit page",0));
     }
 }
*/

 initCalcMenu();
 resultOfCalc();
}

void KSpreadView::changeNbOfRecentFiles(int _nb)
{
    if (shell())
        shell()->setMaxRecentItems( _nb );
}

void KSpreadView::initCalcMenu()
{
    switch( doc()->getTypeOfCalc())
    {
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


void KSpreadView::recalcWorkBook()
{
  KSpreadSheet * tbl;
  doc()->emitBeginOperation( true );
  for ( tbl = doc()->map()->firstSheet();
        tbl != 0L;
        tbl = doc()->map()->nextSheet() )
  {
    bool b = tbl->getAutoCalc();
    tbl->setAutoCalc( true );
    tbl->recalc();
    tbl->setAutoCalc( b );
  }

  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::refreshLocale()
{
  doc()->emitBeginOperation(true);
  KSpreadSheet *tbl;
  for ( tbl = doc()->map()->firstSheet();
        tbl != 0L;
        tbl = doc()->map()->nextSheet() )
  {
    tbl->updateLocale();
  }
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::recalcWorkSheet()
{
  if ( d->activeSheet != 0 )
  {
    doc()->emitBeginOperation( true );
    bool b = d->activeSheet->getAutoCalc();
    d->activeSheet->setAutoCalc( true );
    d->activeSheet->recalc();
    d->activeSheet->setAutoCalc( b );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}


void KSpreadView::extraSpelling()
{
  if ( d->spell.kspell )
    return; // Already in progress

  if (d->activeSheet == 0L)
    return;

  d->spell.macroCmdSpellCheck = 0L;
  d->spell.firstSpellSheet    = d->activeSheet;
  d->spell.currentSpellSheet  = d->spell.firstSpellSheet;

  QRect selection = d->selectionInfo->selection();

  // if nothing is selected, check every cell
  if (d->selectionInfo->singleCellSelection())
  {
    d->spell.spellStartCellX = 0;
    d->spell.spellStartCellY = 0;
    d->spell.spellEndCellX   = 0;
    d->spell.spellEndCellY   = 0;
    d->spell.spellCheckSelection = false;
    d->spell.currentCell     = d->activeSheet->firstCell();
  }
  else
  {
    d->spell.spellStartCellX = selection.left();
    d->spell.spellStartCellY = selection.top();
    d->spell.spellEndCellX   = selection.right();
    d->spell.spellEndCellY   = selection.bottom();
    d->spell.spellCheckSelection = true;
    d->spell.currentCell     = 0L;

    // "-1" because X gets increased every time we go into spellCheckReady()
    d->spell.spellCurrCellX = d->spell.spellStartCellX - 1;
    d->spell.spellCurrCellY = d->spell.spellStartCellY;
  }

  startKSpell();
}


void KSpreadView::startKSpell()
{
    if ( doc()->getKSpellConfig() )
    {
        doc()->getKSpellConfig()->setIgnoreList( doc()->spellListIgnoreAll() );
        doc()->getKSpellConfig()->setReplaceAllList( d->spell.replaceAll );

    }
    d->spell.kspell = new KSpell( this, i18n( "Spell Checking" ), this,
                                       SLOT( spellCheckerReady() ),
                                       doc()->getKSpellConfig() );

  d->spell.kspell->setIgnoreUpperWords( doc()->dontCheckUpperWord() );
  d->spell.kspell->setIgnoreTitleCase( doc()->dontCheckTitleCase() );

  QObject::connect( d->spell.kspell, SIGNAL( death() ),
                    this, SLOT( spellCheckerFinished() ) );
  QObject::connect( d->spell.kspell, SIGNAL( misspelling( const QString &,
                                                         const QStringList &,
                                                         unsigned int) ),
                    this, SLOT( spellCheckerMisspelling( const QString &,
                                                         const QStringList &,
                                                         unsigned int) ) );
  QObject::connect( d->spell.kspell, SIGNAL( corrected( const QString &,
                                                       const QString &,
                                                       unsigned int) ),
                    this, SLOT( spellCheckerCorrected( const QString &,
                                                       const QString &,
                                                       unsigned int ) ) );
  QObject::connect( d->spell.kspell, SIGNAL( done( const QString & ) ),
                    this, SLOT( spellCheckerDone( const QString & ) ) );
  QObject::connect( d->spell.kspell, SIGNAL( ignoreall (const QString & ) ),
                    this, SLOT( spellCheckerIgnoreAll( const QString & ) ) );

  QObject::connect( d->spell.kspell, SIGNAL( replaceall( const QString &  ,  const QString & )), this, SLOT( spellCheckerReplaceAll( const QString &  ,  const QString & )));

}

void KSpreadView::spellCheckerReplaceAll( const QString &orig, const QString & replacement)
{
    d->spell.replaceAll.append( orig);
    d->spell.replaceAll.append( replacement);
}


void KSpreadView::spellCheckerIgnoreAll( const QString & word)
{
    doc()->addIgnoreWordAll( word );
}


void KSpreadView::spellCheckerReady()
{
  if (d->canvas)
    d->canvas->setCursor( WaitCursor );

  // go on to the next cell
  if (!d->spell.spellCheckSelection)
  {
    // if nothing is selected we have to check every cell
    // we use a different way to make it faster
    while ( d->spell.currentCell )
    {
      // check text only
      if ( d->spell.currentCell->value().isString() )
      {
        d->spell.kspell->check( d->spell.currentCell->text(), true );

        return;
      }

      d->spell.currentCell = d->spell.currentCell->nextCell();
      if ( d->spell.currentCell->isDefault() )
        kdDebug() << "checking default cell!!" << endl << endl;
    }

    if (spellSwitchToOtherSheet())
      spellCheckerReady();
    else
      spellCleanup();

    return;
  }

  // if something is selected:

  ++d->spell.spellCurrCellX;
  if (d->spell.spellCurrCellX > d->spell.spellEndCellX)
  {
    d->spell.spellCurrCellX = d->spell.spellStartCellX;
    ++d->spell.spellCurrCellY;
  }

  unsigned int y;
  unsigned int x;

  for ( y = d->spell.spellCurrCellY; y <= d->spell.spellEndCellY; ++y )
  {
    for ( x = d->spell.spellCurrCellX; x <= d->spell.spellEndCellX; ++x )
    {
      KSpreadCell * cell = d->spell.currentSpellSheet->cellAt( x, y );

      // check text only
      if (cell->isDefault() || !cell->value().isString())
        continue;

      d->spell.spellCurrCellX = x;
      d->spell.spellCurrCellY = y;

      d->spell.kspell->check( cell->text(), true );

      return;
    }
    d->spell.spellCurrCellX = d->spell.spellStartCellX;
  }

  // if the user selected something to be checked we are done
  // otherwise ask for checking the next sheet if any
  if (d->spell.spellCheckSelection)
  {
    // Done
    spellCleanup();
  }
  else
  {
    if (spellSwitchToOtherSheet())
      spellCheckerReady();
    else
      spellCleanup();
  }
}


void KSpreadView::spellCleanup()
{
  if ( d->canvas )
    d->canvas->setCursor( ArrowCursor );

  d->spell.kspell->cleanUp();
  delete d->spell.kspell;
  d->spell.kspell            = 0L;
  d->spell.firstSpellSheet   = 0L;
  d->spell.currentSpellSheet = 0L;
  d->spell.currentCell       = 0L;
  d->spell.replaceAll.clear();


  KMessageBox::information( this, i18n( "Spell checking is complete." ) );

  if ( d->spell.macroCmdSpellCheck )
    doc()->addCommand( d->spell.macroCmdSpellCheck );
  d->spell.macroCmdSpellCheck=0L;
}


bool KSpreadView::spellSwitchToOtherSheet()
{
  // there is no other sheet
  if ( doc()->map()->count() == 1 )
    return false;

  // for optimization
  QPtrList<KSpreadSheet> sheetList = doc()->map()->sheetList();

  unsigned int curIndex = sheetList.findRef(d->spell.currentSpellSheet);
  ++curIndex;

  // last sheet? then start at the beginning
  if ( curIndex >= sheetList.count() )
    d->spell.currentSpellSheet = sheetList.first();
  else
    d->spell.currentSpellSheet = sheetList.at(curIndex);

  // if the current sheet is the first one again, we are done.
  if ( d->spell.currentSpellSheet == d->spell.firstSpellSheet )
  {
    setActiveSheet( d->spell.firstSpellSheet );
    return false;
  }

  if (d->spell.spellCheckSelection)
  {
    d->spell.spellEndCellX = d->spell.currentSpellSheet->maxColumn();
    d->spell.spellEndCellY = d->spell.currentSpellSheet->maxRow();

    d->spell.spellCurrCellX = d->spell.spellStartCellX - 1;
    d->spell.spellCurrCellY = d->spell.spellStartCellY;
  }
  else
  {
    d->spell.currentCell = d->spell.currentSpellSheet->firstCell();
  }

  if ( KMessageBox::questionYesNo( this,
                                   i18n( "Do you want to check the spelling in the next sheet?") )
       != KMessageBox::Yes )
    return false;

  setActiveSheet( d->spell.currentSpellSheet );

  return true;
}


void KSpreadView::spellCheckerMisspelling( const QString &,
                                           const QStringList &,
                                           unsigned int )
{
  // scroll to the cell
  if ( !d->spell.spellCheckSelection )
  {
    d->spell.spellCurrCellX = d->spell.currentCell->column();
    d->spell.spellCurrCellY = d->spell.currentCell->row();
  }

  canvasWidget()->gotoLocation( d->spell.spellCurrCellX, d->spell.spellCurrCellY, activeSheet() );
}


void KSpreadView::spellCheckerCorrected( const QString & old, const QString & corr,
                                         unsigned int pos )
{
  KSpreadCell * cell;

  if (d->spell.spellCheckSelection)
  {
    cell = d->spell.currentSpellSheet->cellAt( d->spell.spellCurrCellX,
                                              d->spell.spellCurrCellY );
  }
  else
  {
    cell = d->spell.currentCell;
    d->spell.spellCurrCellX = cell->column();
    d->spell.spellCurrCellY = cell->row();
  }

  Q_ASSERT( cell );
  if ( !cell )
    return;

  doc()->emitBeginOperation(false);
  QString content( cell->text() );

  KSpreadUndoSetText* undo = new KSpreadUndoSetText( doc(), d->activeSheet,
                                                     content,
                                                     d->spell.spellCurrCellX,
                                                     d->spell.spellCurrCellY,
                                                     cell->formatType());
  content.replace( pos, old.length(), corr );
  cell->setCellText( content );
  d->editWidget->setText( content );

  if ( !d->spell.macroCmdSpellCheck )
      d->spell.macroCmdSpellCheck = new KSpreadMacroUndoAction( doc(), i18n("Correct Misspelled Word") );
  d->spell.macroCmdSpellCheck->addCommand( undo );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::spellCheckerDone( const QString & )
{
    int result = d->spell.kspell->dlgResult();

    d->spell.kspell->cleanUp();
    delete d->spell.kspell;
    d->spell.kspell = 0L;

    if ( result != KS_CANCEL && result != KS_STOP )
    {
        if (d->spell.spellCheckSelection)
        {
            if ( (d->spell.spellCurrCellY <= d->spell.spellEndCellY)
                 && (d->spell.spellCurrCellX <= d->spell.spellEndCellX) )
            {
                startKSpell();
                return;
            }
        }
        else
        {
            if ( d->spell.currentCell )
            {
                d->spell.currentCell = d->spell.currentCell->nextCell();

                startKSpell();

                return;
            }
        }
    }
    d->spell.replaceAll.clear();

    if ( d->spell.macroCmdSpellCheck )
    {
        doc()->addCommand( d->spell.macroCmdSpellCheck );
    }
    d->spell.macroCmdSpellCheck=0L;
}

void KSpreadView::spellCheckerFinished()
{
  if (d->canvas)
    d->canvas->setCursor( ArrowCursor );

  KSpell::spellStatus status = d->spell.kspell->status();
  d->spell.kspell->cleanUp();
  delete d->spell.kspell;
  d->spell.kspell = 0L;
  d->spell.replaceAll.clear();

  bool kspellNotConfigured=false;

  if (status == KSpell::Error)
  {
    KMessageBox::sorry(this, i18n("ISpell could not be started.\n"
                                  "Please make sure you have ISpell properly configured and in your PATH."));
    kspellNotConfigured=true;
  }
  else if (status == KSpell::Crashed)
  {
    KMessageBox::sorry(this, i18n("ISpell seems to have crashed."));
  }

  if (d->spell.macroCmdSpellCheck)
  {
      doc()->addCommand( d->spell.macroCmdSpellCheck );
  }
  d->spell.macroCmdSpellCheck=0L;


  if (kspellNotConfigured)
  {
    KSpreadpreference configDlg( this, 0 );
    configDlg.openPage( KSpreadpreference::KS_SPELLING);
    configDlg.exec();
  }
}

void KSpreadView::initialPosition()
{
    kdDebug() << "KSpreadView::initialPosition" << endl;

    // Loading completed, pick initial worksheet
    QPtrListIterator<KSpreadSheet> it( doc()->map()->sheetList() );
    for( ; it.current(); ++it )
      addSheet( it.current() );

    KSpreadSheet * tbl = 0L;
    if ( doc()->isEmbedded() )
    {
        tbl = doc()->displaySheet();
    }

    if ( !tbl )
        tbl = doc()->map()->initialActiveSheet();
    if ( tbl )
      setActiveSheet( tbl );
    else
    {
      //activate first table which is not hiding
      tbl = doc()->map()->findSheet( doc()->map()->visibleSheets().first());
      if ( !tbl )
      {
        tbl = doc()->map()->firstSheet();
        if ( tbl )
        {
          tbl->setHidden( false );
          QString tabName = tbl->sheetName();
          d->tabBar->addTab( tabName );
        }
      }
      setActiveSheet( tbl );
    }

    refreshView();

    // Set the initial position for the marker as store in the XML file,
    // (1,1) otherwise
    int col = doc()->map()->initialMarkerColumn();
    if ( col <= 0 )
      col = 1;
    int row = doc()->map()->initialMarkerRow();
    if ( row <= 0 )
      row = 1;
    d->canvas->gotoLocation( col, row );

    // Set the initial X and Y offsets for the view.
    d->canvas->setXOffset( doc()->map()->initialXOffset() );
    d->canvas->setYOffset( doc()->map()->initialYOffset() );

    updateBorderButton();
    updateShowSheetMenu();

    d->actions->autoFormat->setEnabled(false);
    d->actions->sort->setEnabled(false);
    d->actions->mergeCell->setEnabled(false);
    d->actions->createStyle->setEnabled(false);

    d->actions->fillUp->setEnabled( false );
    d->actions->fillRight->setEnabled( false );
    d->actions->fillDown->setEnabled( false );
    d->actions->fillLeft->setEnabled( false );

    // make paint effective:
    doc()->decreaseNumOperation();
    d->actions->insertChartFrame->setEnabled(false);

    QRect vr( activeSheet()->visibleRect( d->canvas ) );

    doc()->emitBeginOperation( false );
    activeSheet()->setRegionPaintDirty( vr );
    doc()->emitEndOperation( vr );

    d->loading = true;

    if ( koDocument()->isReadWrite() )
      initConfig();

    d->adjustActions( !d->activeSheet->isProtected() );
    d->adjustWorkbookActions( !doc()->map()->isProtected() );
}


void KSpreadView::updateEditWidgetOnPress()
{
    int column = d->canvas->markerColumn();
    int row    = d->canvas->markerRow();

    KSpreadCell* cell = d->activeSheet->cellAt( column, row );
    if ( !cell )
    {
        editWidget()->setText( "" );
        return;
    }
    if ( d->activeSheet->isProtected() && cell->isHideFormula( column, row ) )
        editWidget()->setText( cell->strOutText() );
    else if ( d->activeSheet->isProtected() && cell->isHideAll( column, row ) )
        editWidget()->setText( "" );
    else
        editWidget()->setText( cell->text() );

    d->updateButton(cell, column, row);
    d->adjustActions( d->activeSheet, cell );
}

void KSpreadView::updateEditWidget()
{
    int column = d->canvas->markerColumn();
    int row    = d->canvas->markerRow();

    KSpreadCell * cell = d->activeSheet->cellAt( column, row );
    bool active = activeSheet()->getShowFormula()
      && !( d->activeSheet->isProtected() && cell && cell->isHideFormula( column, row ) );

    if ( d->activeSheet && !d->activeSheet->isProtected() )
    {
      d->actions->alignLeft->setEnabled(!active);
      d->actions->alignCenter->setEnabled(!active);
      d->actions->alignRight->setEnabled(!active);
    }

    if ( !cell )
    {
        editWidget()->setText( "" );
        if ( d->activeSheet->isProtected() )
          editWidget()->setEnabled( false );
        else
          editWidget()->setEnabled( true );
        return;
    }

    if ( d->activeSheet->isProtected() && cell->isHideFormula( column, row ) )
        editWidget()->setText( cell->strOutText() );
    else if ( d->activeSheet->isProtected() && cell->isHideAll( column, row ) )
        editWidget()->setText( "" );
    else
        editWidget()->setText( cell->text() );

    if ( d->activeSheet->isProtected() && !cell->notProtected( column, row ) )
      editWidget()->setEnabled( false );
    else
      editWidget()->setEnabled( true );

    if ( d->canvas->editor() )
    {
      d->canvas->editor()->setEditorFont(cell->textFont(column, row), true);
      d->canvas->editor()->setFocus();
    }
    d->updateButton(cell, column, row);
    d->adjustActions( d->activeSheet, cell );
}

void KSpreadView::activateFormulaEditor()
{
}

void KSpreadView::updateReadWrite( bool readwrite )
{
    // d->cancelButton->setEnabled( readwrite );
    // d->okButton->setEnabled( readwrite );
  d->editWidget->setEnabled( readwrite );

  QValueList<KAction*> actions = actionCollection()->actions();
  QValueList<KAction*>::ConstIterator aIt = actions.begin();
  QValueList<KAction*>::ConstIterator aEnd = actions.end();
  for (; aIt != aEnd; ++aIt )
    (*aIt)->setEnabled( readwrite );

  d->actions->transform->setEnabled( false );
  if ( !doc() || !doc()->map() || doc()->map()->isProtected() )
  {
    d->actions->showSheet->setEnabled( false );
    d->actions->hideSheet->setEnabled( false );
  }
  else
  {
    d->actions->showSheet->setEnabled( true );
    d->actions->hideSheet->setEnabled( true );
  }
  d->actions->gotoCell->setEnabled( true );
  d->actions->viewZoom->setEnabled( true );
  d->actions->showPageBorders->setEnabled( true );
  d->actions->find->setEnabled( true);
  d->actions->replace->setEnabled( readwrite );
  if ( !doc()->isReadWrite())
      d->actions->copy->setEnabled( true );
  //  d->actions->newView->setEnabled( true );
  //doc()->KXMLGUIClient::action( "newView" )->setEnabled( true ); // obsolete (Werner)
}

void KSpreadView::createTemplate()
{
  int width = 60;
  int height = 60;
  QPixmap pix = doc()->generatePreview(QSize(width, height));

  KTempFile tempFile( QString::null, ".kst" );
  tempFile.setAutoDelete(true);

  doc()->saveNativeFormat( tempFile.name() );

  KoTemplateCreateDia::createTemplate( "kspread_template", KSpreadFactory::global(),
                                           tempFile.name(), pix, this );

  KSpreadFactory::global()->dirs()->addResourceType("kspread_template",
                                                       KStandardDirs::kde_default( "data" ) +
                                                       "kspread/templates/");
}

void KSpreadView::sheetFormat()
{
    KSpreadFormatDlg dlg( this );
    dlg.exec();
}

void KSpreadView::autoSum()
{
    // ######## Torben: Make sure that this can not be called
    // when canvas has a running editor
    if ( d->canvas->editor() )
        return;

	//Get the selected range and remove the current cell from it (as that is
	//where the result of the autosum will be stored - perhaps change
	//this behaviour??)
	KSpreadRange rg;
	//rg.sheet=activeSheet();
	QRect sel=selection(false);

	if (sel.height() > 1)
	{
		if (marker().y()==sel.top())
			sel.setTop(sel.top()+1);
		if (marker().y()==sel.bottom())
			sel.setBottom(sel.bottom()-1);
	}
	else
	{
		if (sel.width() > 1)
		{
			if (marker().x()==sel.left())
				sel.setLeft(sel.left()+1);

			if (marker().x()==sel.right())
				sel.setRight(sel.right()-1);
		}
		else
		{
			sel=QRect();
		}
	}

	if ( (sel.width() > 1) && (sel.height() > 1) )
		sel=QRect();

	rg.range=sel;

    d->canvas->createEditor( KSpreadCanvas::CellEditor );

	if ( (rg.range.isValid() ) && (!rg.range.isEmpty()) )
	{
    		d->canvas->editor()->setText( "=SUM("+rg.toString()+")" );
		d->canvas->deleteEditor(true);
	}
	else
	{
		d->canvas->editor()->setText( "=SUM( )" );
    		d->canvas->editor()->setCursorPosition( 5 );
	}
}

/*
void KSpreadView::oszilloscope()
{
    QDialog* dlg = new KSpreadOsziDlg( this );
    dlg->show();
}
*/

void KSpreadView::changeTextColor()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation(false);
    d->activeSheet->setSelectionTextColor( selectionInfo(), d->actions->textColor->color() );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void KSpreadView::setSelectionTextColor(const QColor &txtColor)
{
  if (d->activeSheet != 0L)
  {
    doc()->emitBeginOperation(false);
    d->activeSheet->setSelectionTextColor( selectionInfo(), txtColor );
    doc()->emitEndOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::changeBackgroundColor()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation(false);
    d->activeSheet->setSelectionbgColor( selectionInfo(), d->actions->bgColor->color() );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void KSpreadView::setSelectionBackgroundColor(const QColor &bgColor)
{
  if (d->activeSheet != 0L)
  {
    doc()->emitBeginOperation(false);
    d->activeSheet->setSelectionbgColor( selectionInfo(), bgColor );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void KSpreadView::changeBorderColor()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation(false);
    d->activeSheet->setSelectionBorderColor( selectionInfo(), d->actions->borderColor->color() );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void KSpreadView::setSelectionBorderColor(const QColor &bdColor)
{
  if (d->activeSheet != 0L)
  {
    doc()->emitBeginOperation(false);
    d->activeSheet->setSelectionBorderColor( selectionInfo(), bdColor );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void KSpreadView::helpUsing()
{
  kapp->invokeHelp( );
}

void KSpreadView::enableUndo( bool _b )
{
  KAction* action = actionCollection()->action( "office_undo" );
  if( action ) action->setEnabled( _b );
}

void KSpreadView::enableRedo( bool _b )
{
  KAction* action = actionCollection()->action( "office_redo" );
  if( action ) action->setEnabled( _b );
}

void KSpreadView::enableInsertColumn( bool _b )
{
  if ( d->activeSheet && !d->activeSheet->isProtected() )
    d->actions->insertColumn->setEnabled( _b );
}

void KSpreadView::enableInsertRow( bool _b )
{
  if ( d->activeSheet && !d->activeSheet->isProtected() )
    d->actions->insertRow->setEnabled( _b );
}

void KSpreadView::deleteColumn()
{
  if ( !d->activeSheet )
    return;

  doc()->emitBeginOperation( false );

  QRect r( d->selectionInfo->selection() );

  d->activeSheet->removeColumn( r.left(), ( r.right()-r.left() ) );

  updateEditWidget();
  d->selectionInfo->setSelection( d->selectionInfo->marker(),
                                 d->selectionInfo->marker(), d->activeSheet );

  QRect vr( d->activeSheet->visibleRect( d->canvas ) );
  vr.setLeft( r.left() );

  doc()->emitEndOperation( vr );
}

void KSpreadView::deleteRow()
{
  if ( !d->activeSheet )
    return;

  doc()->emitBeginOperation( false );
  QRect r( d->selectionInfo->selection() );
  d->activeSheet->removeRow( r.top(),(r.bottom()-r.top()) );

  updateEditWidget();
  d->selectionInfo->setSelection( d->selectionInfo->marker(),
                                 d->selectionInfo->marker(), d->activeSheet );

  QRect vr( d->activeSheet->visibleRect( d->canvas ) );
  vr.setTop( r.top() );

  doc()->emitEndOperation( vr );
}

void KSpreadView::insertColumn()
{
  if ( !d->activeSheet )
    return;

  doc()->emitBeginOperation( false );
  QRect r( d->selectionInfo->selection() );
  d->activeSheet->insertColumn( r.left(), ( r.right()-r.left() ) );

  updateEditWidget();

  QRect vr( d->activeSheet->visibleRect( d->canvas ) );
  vr.setLeft( r.left() - 1 );

  doc()->emitEndOperation( vr );
}

void KSpreadView::hideColumn()
{
  if ( !d->activeSheet )
    return;
  doc()->emitBeginOperation( false );
  QRect r( d->selectionInfo->selection() );
  d->activeSheet->hideColumn( r.left(), ( r.right()-r.left() ) );

  QRect vr( d->activeSheet->visibleRect( d->canvas ) );
  vr.setLeft( r.left() );
  doc()->emitEndOperation( vr );
}

void KSpreadView::showColumn()
{
  if ( !d->activeSheet )
    return;

  KSpreadShowColRow dlg( this, "showCol", KSpreadShowColRow::Column );
  dlg.exec();
}

void KSpreadView::showSelColumns()
{
  if ( !d->activeSheet )
    return;

  int i;
  QRect rect = d->selectionInfo->selection();
  ColumnFormat * col;
  QValueList<int>hiddenCols;

  doc()->emitBeginOperation( false );

  for ( i = rect.left(); i <= rect.right(); ++i )
  {
    if ( i == 2 ) // "B"
    {
      col = activeSheet()->columnFormat( 1 );
      if ( col->isHide() )
      {
        hiddenCols.append( 1 );
      }
    }

    col = d->activeSheet->columnFormat( i );
    if ( col->isHide() )
    {
      hiddenCols.append( i );
    }
  }

  if ( hiddenCols.count() > 0 )
    d->activeSheet->showColumn( 0, -1, hiddenCols );

  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::insertRow()
{
  if ( !d->activeSheet )
    return;
  doc()->emitBeginOperation( false );
  QRect r( d->selectionInfo->selection() );
  d->activeSheet->insertRow( r.top(), ( r.bottom() - r.top() ) );

  updateEditWidget();
  QRect vr( d->activeSheet->visibleRect( d->canvas ) );
  vr.setTop( r.top() - 1 );

  doc()->emitEndOperation( vr );
}

void KSpreadView::hideRow()
{
  if ( !d->activeSheet )
    return;

  doc()->emitBeginOperation( false );

  QRect r( d->selectionInfo->selection() );
  d->activeSheet->hideRow( r.top(), ( r.bottom() - r.top() ) );

  QRect vr( d->activeSheet->visibleRect( d->canvas ) );
  vr.setTop( r.top() );

  doc()->emitEndOperation( vr );
}

void KSpreadView::showRow()
{
  if ( !d->activeSheet )
    return;

  KSpreadShowColRow dlg( this, "showRow", KSpreadShowColRow::Row );
  dlg.exec();
}

void KSpreadView::showSelRows()
{
  if ( !d->activeSheet )
    return;

  int i;
  QRect rect( d->selectionInfo->selection() );
  RowFormat * row;
  QValueList<int>hiddenRows;

  doc()->emitBeginOperation( false );

  for ( i = rect.top(); i <= rect.bottom(); ++i )
  {
    if ( i == 2 )
    {
      row = activeSheet()->rowFormat( 1 );
      if ( row->isHide() )
      {
        hiddenRows.append( 1 );
      }
    }

    row = d->activeSheet->rowFormat( i );
    if ( row->isHide() )
    {
      hiddenRows.append( i );
    }
  }

  if ( hiddenRows.count() > 0 )
    d->activeSheet->showRow( 0, -1, hiddenRows );

  endOperation( rect );
}

void KSpreadView::endOperation( QRect const & rect )
{
  QRect vr( d->activeSheet->visibleRect( d->canvas ) );
  if ( rect.left() > vr.left() )
    vr.setLeft( rect.left() );
  if ( rect.top() > vr.top() )
    vr.setTop( rect.top() );
  if ( rect.right() < vr.right() )
    vr.setRight( rect.right() );
  if ( rect.bottom() < vr.bottom() )
    vr.setBottom( rect.bottom() );

  doc()->emitEndOperation( vr );
}

void KSpreadView::fontSelected( const QString & _font )
{
  if ( d->toolbarLock )
    return;

  doc()->emitBeginOperation(false);
  if ( d->activeSheet != 0L )
    d->activeSheet->setSelectionFont( d->selectionInfo, _font.latin1() );

  // Dont leave the focus in the toolbars combo box ...
  if ( d->canvas->editor() )
  {
    KSpreadCell * cell = d->activeSheet->cellAt( d->selectionInfo->marker() );
    d->canvas->editor()->setEditorFont( cell->textFont( cell->column(), cell->row() ), true );
    d->canvas->editor()->setFocus();
  }
  else
    d->canvas->setFocus();

  endOperation( d->selectionInfo->selection() );
}

void KSpreadView::decreaseFontSize()
{
  setSelectionFontSize( -1 );
}

void KSpreadView::increaseFontSize()
{
  setSelectionFontSize( 1 );
}

void KSpreadView::setSelectionFontSize( int size )
{
  if ( d->activeSheet != NULL )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->setSelectionSize( selectionInfo(), size );
    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::lower()
{
  if ( !d->activeSheet  )
    return;

  doc()->emitBeginOperation( false );

  d->activeSheet->setSelectionUpperLower( selectionInfo(), -1 );
  updateEditWidget();

  endOperation( d->selectionInfo->selection() );
}

void KSpreadView::upper()
{
  if ( !d->activeSheet  )
    return;

  doc()->emitBeginOperation( false );

  d->activeSheet->setSelectionUpperLower( selectionInfo(), 1 );
  updateEditWidget();

  endOperation( d->selectionInfo->selection() );
}

void KSpreadView::firstLetterUpper()
{
  if ( !d->activeSheet  )
    return;
  doc()->emitBeginOperation( false );
  d->activeSheet->setSelectionfirstLetterUpper( selectionInfo() );
  updateEditWidget();
  endOperation( d->selectionInfo->selection() );
}

void KSpreadView::verticalText(bool b)
{
  if ( !d->activeSheet  )
    return;

  doc()->emitBeginOperation( false );
  d->activeSheet->setSelectionVerticalText( selectionInfo(), b );
  if ( util_isRowSelected( selection() ) == false
       && util_isColumnSelected( selection() ) == false )
  {
    d->canvas->adjustArea( false );
    updateEditWidget();
    endOperation( d->selectionInfo->selection() );
    return;
  }

  doc()->emitEndOperation( QRect( d->selectionInfo->marker(), d->selectionInfo->marker() ) );
}

void KSpreadView::insertSpecialChar()
{
  QString f( d->actions->selectFont->font() );
  QChar c = ' ';

  if ( d->specialCharDlg == 0 )
  {
    d->specialCharDlg = new KoCharSelectDia( this, "insert special char", f, c, false );
    connect( d->specialCharDlg, SIGNAL( insertChar( QChar, const QString & ) ),
             this, SLOT( slotSpecialChar( QChar, const QString & ) ) );
    connect( d->specialCharDlg, SIGNAL( finished() ),
             this, SLOT( slotSpecialCharDlgClosed() ) );
  }
  d->specialCharDlg->show();
}

void KSpreadView::slotSpecialCharDlgClosed()
{
  if ( d->specialCharDlg )
  {
    disconnect( d->specialCharDlg, SIGNAL(insertChar(QChar,const QString &)),
                this, SLOT(slotSpecialChar(QChar,const QString &)));
    disconnect( d->specialCharDlg, SIGNAL( finished() ),
                this, SLOT( slotSpecialCharDlgClosed() ) );
    d->specialCharDlg->deleteLater();
    d->specialCharDlg = 0L;
  }
}

void KSpreadView::slotSpecialChar( QChar c, const QString & _font )
{
  if ( d->activeSheet )
  {
    QPoint marker( selectionInfo()->marker() );
    KSpreadCell * cell = d->activeSheet->nonDefaultCell( marker );
    if ( cell->textFont( marker.x(), marker.y() ).family() != _font )
    {
      cell->setTextFontFamily( _font );
    }
    KSpreadEditWidget * edit = d->canvas->editWidget();
    QKeyEvent ev( QEvent::KeyPress, 0, 0, 0, QString( c ) );
    QApplication::sendEvent( edit, &ev );
  }
}

void KSpreadView::insertMathExpr()
{
  if ( d->activeSheet == 0L )
    return;

  KSpreadDlgFormula * dlg = new KSpreadDlgFormula( this, "Function" );
  dlg->show();

  /* TODO - because I search on 'TODO's :-) */
  // #### Is the dialog deleted when it's closed ? (David)
  // Torben thinks that not.
}

void KSpreadView::formulaSelection( const QString &_math )
{
  if ( d->activeSheet == 0 )
    return;

  if ( _math == i18n("Others...") )
  {
    insertMathExpr();
    return;
  }

  KSpreadDlgFormula *dlg = new KSpreadDlgFormula( this, "Formula Editor", _math );
  dlg->exec();
}

void KSpreadView::fontSizeSelected( int _size )
{
  if ( d->toolbarLock )
    return;

  doc()->emitBeginOperation( false );

  if ( d->activeSheet != 0L )
    d->activeSheet->setSelectionFont( selectionInfo(), 0L, _size );

  // Dont leave the focus in the toolbars combo box ...
  if ( d->canvas->editor() )
  {
    KSpreadCell * cell = d->activeSheet->cellAt( d->selectionInfo->marker() );
    d->canvas->editor()->setEditorFont( cell->textFont( d->canvas->markerColumn(),
                                                        d->canvas->markerRow() ), true );
    d->canvas->editor()->setFocus();
  }
  else
    d->canvas->setFocus();

  endOperation( d->selectionInfo->selection() );
}

void KSpreadView::bold( bool b )
{
  if ( d->toolbarLock )
    return;
  if ( d->activeSheet == 0 )
    return;

  doc()->emitBeginOperation( false );

  int col = d->canvas->markerColumn();
  int row = d->canvas->markerRow();
  d->activeSheet->setSelectionFont( selectionInfo(), 0L, -1, b );

  if ( d->canvas->editor() )
  {
    KSpreadCell * cell = d->activeSheet->cellAt( col, row );
    d->canvas->editor()->setEditorFont( cell->textFont( col, row ), true );
  }

  endOperation( d->selectionInfo->selection() );
}

void KSpreadView::underline( bool b )
{
  if ( d->toolbarLock )
    return;
  if ( d->activeSheet == 0 )
    return;

  doc()->emitBeginOperation( false );

  int col = d->canvas->markerColumn();
  int row = d->canvas->markerRow();

  d->activeSheet->setSelectionFont( selectionInfo(), 0L, -1, -1, -1 ,b );
  if ( d->canvas->editor() )
  {
    KSpreadCell * cell = d->activeSheet->cellAt( col, row );
    d->canvas->editor()->setEditorFont( cell->textFont( col, row ), true );
  }

  endOperation( d->selectionInfo->selection() );
}

void KSpreadView::strikeOut( bool b )
{
  if ( d->toolbarLock )
    return;
  if ( d->activeSheet == 0 )
    return;

  doc()->emitBeginOperation( false );

  int col = d->canvas->markerColumn();
  int row = d->canvas->markerRow();

  d->activeSheet->setSelectionFont( selectionInfo(), 0L, -1, -1, -1 ,-1, b );
  if ( d->canvas->editor() )
  {
    KSpreadCell * cell = d->activeSheet->cellAt( col, row );
    d->canvas->editor()->setEditorFont( cell->textFont( col, row ), true );
  }

  endOperation( d->selectionInfo->selection() );
}


void KSpreadView::italic( bool b )
{
  if ( d->toolbarLock )
    return;
  if ( d->activeSheet == 0 )
    return;

  doc()->emitBeginOperation( false );

  int col = d->canvas->markerColumn();
  int row = d->canvas->markerRow();

  d->activeSheet->setSelectionFont( selectionInfo(), 0L, -1, -1, b );
  if ( d->canvas->editor() )
  {
    KSpreadCell * cell = d->activeSheet->cellAt( col, row );
    d->canvas->editor()->setEditorFont( cell->textFont( col, row ), true );
  }

  endOperation( d->selectionInfo->selection() );
}

void KSpreadView::sortInc()
{
  QRect r( d->selectionInfo->selection() );
  if ( d->selectionInfo->singleCellSelection() )
  {
    KMessageBox::error( this, i18n( "You must select multiple cells." ) );
    return;
  }

  doc()->emitBeginOperation( false );

  // Entire row(s) selected ? Or just one row ?
  if ( util_isRowSelected( selection() ) || r.top() == r.bottom() )
    activeSheet()->sortByRow( selection(), r.top(), KSpreadSheet::Increase );
  else
    activeSheet()->sortByColumn( selection(), r.left(), KSpreadSheet::Increase );
  updateEditWidget();

  endOperation( d->selectionInfo->selection() );
}

void KSpreadView::sortDec()
{
  QRect r( d->selectionInfo->selection() );
  if ( d->selectionInfo->singleCellSelection() )
  {
    KMessageBox::error( this, i18n( "You must select multiple cells." ) );
    return;
  }

  doc()->emitBeginOperation( false );

    // Entire row(s) selected ? Or just one row ?
  if ( util_isRowSelected( selection() ) || r.top() == r.bottom() )
    activeSheet()->sortByRow( selection(), r.top(), KSpreadSheet::Decrease );
  else
    activeSheet()->sortByColumn( selection(), r.left(), KSpreadSheet::Decrease );
  updateEditWidget();

  endOperation( d->selectionInfo->selection() );
}


void KSpreadView::borderBottom()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );

    d->activeSheet->borderBottom( d->selectionInfo, d->actions->borderColor->color() );

    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::setSelectionBottomBorderColor( const QColor & color )
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->borderBottom( selectionInfo(), color );
    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::borderRight()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( d->activeSheet->layoutDirection()==KSpreadSheet::RightToLeft )
      d->activeSheet->borderLeft( d->selectionInfo, d->actions->borderColor->color() );
    else
      d->activeSheet->borderRight( d->selectionInfo, d->actions->borderColor->color() );
    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::setSelectionRightBorderColor( const QColor & color )
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( d->activeSheet->layoutDirection()==KSpreadSheet::RightToLeft )
      d->activeSheet->borderLeft( selectionInfo(), color );
    else
      d->activeSheet->borderRight( selectionInfo(), color );
    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::borderLeft()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( d->activeSheet->layoutDirection()==KSpreadSheet::RightToLeft )
      d->activeSheet->borderRight( d->selectionInfo, d->actions->borderColor->color() );
    else
      d->activeSheet->borderLeft( d->selectionInfo, d->actions->borderColor->color() );
    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::setSelectionLeftBorderColor( const QColor & color )
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( d->activeSheet->layoutDirection()==KSpreadSheet::RightToLeft )
      d->activeSheet->borderRight( selectionInfo(), color );
    else
      d->activeSheet->borderLeft( selectionInfo(), color );
    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::borderTop()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->borderTop( d->selectionInfo, d->actions->borderColor->color() );
    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::setSelectionTopBorderColor( const QColor & color )
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->borderTop( selectionInfo(), color );
    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::borderOutline()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->borderOutline( d->selectionInfo, d->actions->borderColor->color() );
    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::setSelectionOutlineBorderColor( const QColor & color )
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->borderOutline( selectionInfo(), color );
    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::borderAll()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->borderAll( d->selectionInfo, d->actions->borderColor->color() );
    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::setSelectionAllBorderColor( const QColor & color )
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->borderAll( selectionInfo(), color );
    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::borderRemove()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation(false);
    d->activeSheet->borderRemove( d->selectionInfo );
    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::addSheet( KSpreadSheet * _t )
{
  doc()->emitBeginOperation( false );

  insertSheet( _t );

  // Connect some signals
  QObject::connect( _t, SIGNAL( sig_refreshView() ), SLOT( slotRefreshView() ) );
  QObject::connect( _t, SIGNAL( sig_updateView( KSpreadSheet* ) ), SLOT( slotUpdateView( KSpreadSheet* ) ) );
  QObject::connect( _t->print(), SIGNAL( sig_updateView( KSpreadSheet* ) ), SLOT( slotUpdateView( KSpreadSheet* ) ) );
  QObject::connect( _t, SIGNAL( sig_updateView( KSpreadSheet *, const QRect& ) ),
                    SLOT( slotUpdateView( KSpreadSheet*, const QRect& ) ) );
  QObject::connect( _t, SIGNAL( sig_updateHBorder( KSpreadSheet * ) ),
                    SLOT( slotUpdateHBorder( KSpreadSheet * ) ) );
  QObject::connect( _t, SIGNAL( sig_updateVBorder( KSpreadSheet * ) ),
                    SLOT( slotUpdateVBorder( KSpreadSheet * ) ) );
  QObject::connect( _t, SIGNAL( sig_nameChanged( KSpreadSheet*, const QString& ) ),
                    this, SLOT( slotSheetRenamed( KSpreadSheet*, const QString& ) ) );
  QObject::connect( _t, SIGNAL( sig_SheetHidden( KSpreadSheet* ) ),
                    this, SLOT( slotSheetHidden( KSpreadSheet* ) ) );
  QObject::connect( _t, SIGNAL( sig_SheetShown( KSpreadSheet* ) ),
                    this, SLOT( slotSheetShown( KSpreadSheet* ) ) );
  QObject::connect( _t, SIGNAL( sig_SheetRemoved( KSpreadSheet* ) ),
                    this, SLOT( slotSheetRemoved( KSpreadSheet* ) ) );
  // ########### Why do these signals not send a pointer to the sheet?
  // This will lead to bugs.
  QObject::connect( _t, SIGNAL( sig_updateChildGeometry( KSpreadChild* ) ),
                    SLOT( slotUpdateChildGeometry( KSpreadChild* ) ) );
  QObject::connect( _t, SIGNAL( sig_removeChild( KSpreadChild* ) ), SLOT( slotRemoveChild( KSpreadChild* ) ) );
  QObject::connect( _t, SIGNAL( sig_maxColumn( int ) ), d->canvas, SLOT( slotMaxColumn( int ) ) );
  QObject::connect( _t, SIGNAL( sig_maxRow( int ) ), d->canvas, SLOT( slotMaxRow( int ) ) );

  if ( !d->loading )
    updateBorderButton();

  if ( !d->activeSheet )
  {
    doc()->emitEndOperation();
    return;
  }
  endOperation( d->selectionInfo->selection() );
}

void KSpreadView::slotSheetRemoved( KSpreadSheet *_t )
{
  doc()->emitBeginOperation( false );

  QString m_sheetName=_t->sheetName();
  d->tabBar->removeTab( _t->sheetName() );
  if (doc()->map()->findSheet( doc()->map()->visibleSheets().first()))
    setActiveSheet( doc()->map()->findSheet( doc()->map()->visibleSheets().first() ));
  else
    d->activeSheet = 0L;

  QValueList<Reference>::Iterator it;
  QValueList<Reference> area=doc()->listArea();
  for ( it = area.begin(); it != area.end(); ++it )
  {
    //remove Area Name when sheet target is removed
    if ( (*it).sheet_name == m_sheetName )
    {
      doc()->removeArea( (*it).ref_name );
      //now area name is used in formula
      //so you must recalc sheets when remove areaname
      KSpreadSheet * tbl;

      for ( tbl = doc()->map()->firstSheet(); tbl != 0L; tbl = doc()->map()->nextSheet() )
      {
        tbl->refreshRemoveAreaName((*it).ref_name);
      }
    }
  }

  endOperation( d->selectionInfo->selection() );
}

void KSpreadView::removeAllSheets()
{
  doc()->emitBeginOperation(false);
  d->tabBar->clear();

  setActiveSheet( 0L );

  doc()->emitEndOperation();
}

void KSpreadView::setActiveSheet( KSpreadSheet * _t, bool updateSheet )
{
  if ( _t == d->activeSheet )
    return;

  doc()->emitBeginOperation(false);

  saveCurrentSheetSelection();

  KSpreadSheet * oldSheet = d->activeSheet;

  d->activeSheet = _t;

  if ( d->activeSheet == 0L )
  {
    doc()->emitEndOperation();
    return;
  }

  if ( oldSheet && oldSheet->layoutDirection()==KSpreadSheet::RightToLeft != d->activeSheet->layoutDirection()==KSpreadSheet::RightToLeft )
    refreshView();

  doc()->setDisplaySheet( d->activeSheet );
  if ( updateSheet )
  {
    d->tabBar->setActiveTab( _t->sheetName() );
    d->vBorderWidget->repaint();
    d->hBorderWidget->repaint();
    d->activeSheet->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));
    d->canvas->slotMaxColumn( d->activeSheet->maxColumn() );
    d->canvas->slotMaxRow( d->activeSheet->maxRow() );
  }

  d->actions->showPageBorders->setChecked( d->activeSheet->isShowPageBorders() );
  d->actions->protectSheet->setChecked( d->activeSheet->isProtected() );
  d->actions->protectDoc->setChecked( doc()->map()->isProtected() );
  d->adjustActions( !d->activeSheet->isProtected() );
  d->adjustWorkbookActions( !doc()->map()->isProtected() );

  /* see if there was a previous selection on this other sheet */
  QMapIterator<KSpreadSheet*, QPoint> it = d->savedAnchors.find(d->activeSheet);
  QMapIterator<KSpreadSheet*, QPoint> it2 = d->savedMarkers.find(d->activeSheet);

  QPoint newAnchor = (it == d->savedAnchors.end()) ? QPoint(1,1) : *it;
  QPoint newMarker = (it2 == d->savedMarkers.end()) ? QPoint(1,1) : *it2;
  selectionInfo()->setSelection(newMarker, newAnchor, d->activeSheet);
  if( d->canvas->chooseMode())
  {
    selectionInfo()->setChooseSheet( d->activeSheet );
    selectionInfo()->setChooseMarker( QPoint(0,0) );
  }

  d->canvas->scrollToCell(newMarker);
  resultOfCalc();

  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::slotSheetRenamed( KSpreadSheet* sheet, const QString& old_name )
{
  doc()->emitBeginOperation( false );
  d->tabBar->renameTab( old_name, sheet->sheetName() );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::slotSheetHidden( KSpreadSheet* )
{
  doc()->emitBeginOperation(false);
  updateShowSheetMenu();
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::slotSheetShown( KSpreadSheet* )
{
  doc()->emitBeginOperation(false);
  d->tabBar->setTabs( doc()->map()->visibleSheets() );
  updateShowSheetMenu();
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::changeSheet( const QString& _name )
{
    if ( activeSheet()->sheetName() == _name )
        return;

    KSpreadSheet *t = doc()->map()->findSheet( _name );
    if ( !t )
    {
        kdDebug(36001) << "Unknown sheet " << _name << endl;
        return;
    }
    doc()->emitBeginOperation(false);
    d->canvas->closeEditor();
    setActiveSheet( t, false /* False: Endless loop because of setActiveTab() => do the visual area update manually*/);

    updateEditWidget();
    //refresh toggle button
    updateBorderButton();

    //update visible area
    d->vBorderWidget->repaint();
    d->hBorderWidget->repaint();
    t->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));
    d->canvas->slotMaxColumn( d->activeSheet->maxColumn() );
    d->canvas->slotMaxRow( d->activeSheet->maxRow() );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::moveSheet( unsigned sheet, unsigned target )
{
    QStringList vs = doc()->map()->visibleSheets();

    if( target >= vs.count() )
        doc()->map()->moveSheet( vs[ sheet ], vs[ vs.count()-1 ], false );
    else
        doc()->map()->moveSheet( vs[ sheet ], vs[ target ], true );

    d->tabBar->moveTab( sheet, target );
}

void KSpreadView::sheetProperties()
{
    // sanity check, shouldn't happen
    if( doc()->map()->isProtected() ) return;
    if( d->activeSheet->isProtected() ) return;

    bool directionChanged = false;

    SheetPropertiesDialog* dlg = new SheetPropertiesDialog( this );
    dlg->setLayoutDirection( d->activeSheet->layoutDirection() );
    dlg->setAutoCalc( d->activeSheet->getAutoCalc() );
    dlg->setShowGrid( d->activeSheet->getShowGrid() );
    dlg->setShowPageBorders( d->activeSheet->isShowPageBorders() );
    dlg->setShowFormula( d->activeSheet->getShowFormula() );
    dlg->setHideZero( d->activeSheet->getHideZero() );
    dlg->setShowFormulaIndicator( d->activeSheet->getShowFormulaIndicator() );
    dlg->setColumnAsNumber( d->activeSheet->getShowColumnNumber() );
    dlg->setLcMode( d->activeSheet->getLcMode() );
    dlg->setCapitalizeFirstLetter( d->activeSheet->getFirstLetterUpper() );

    if( dlg->exec() )
    {
        SheetPropertiesCommand* command = new SheetPropertiesCommand( doc(), d->activeSheet );

        if ( d->activeSheet->layoutDirection() != dlg->layoutDirection() )
            directionChanged = true;

        command->setLayoutDirection( dlg->layoutDirection() );
        command->setAutoCalc( dlg->autoCalc() );
        command->setShowGrid( dlg->showGrid() );
        command->setShowPageBorders( dlg->showPageBorders() );
        command->setShowFormula( dlg->showFormula() );
        command->setHideZero( dlg->hideZero() );
        command->setShowFormulaIndicator( dlg->showFormulaIndicator() );
        command->setColumnAsNumber( dlg->columnAsNumber() );
        command->setLcMode( dlg->lcMode() );
        command->setCapitalizeFirstLetter( dlg->capitalizeFirstLetter() );
        doc()->addCommand( command );
        command->execute();
    }

    delete dlg;

    if ( directionChanged )
    {
        // the scrollbar and hborder remain reversed otherwise
        d->horzScrollBar->setValue( d->horzScrollBar->maxValue() -
                                            d->horzScrollBar->value() );
        d->hBorderWidget->update();
    }
}

void KSpreadView::insertSheet()
{
  if ( doc()->map()->isProtected() )
  {
    KMessageBox::error( 0, i18n ( "You cannot change a protected sheet." ) );
    return;
  }

  doc()->emitBeginOperation( false );
  d->canvas->closeEditor();
  KSpreadSheet * t = doc()->map()->createSheet();
  KCommand* command = new AddSheetCommand( t );
  doc()->addCommand( command );
  updateEditWidget();
  setActiveSheet( t );

  if ( doc()->map()->visibleSheets().count() > 1 )
  {
    d->actions->removeSheet->setEnabled( true );
    d->actions->hideSheet->setEnabled( true );
  }

  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::hideSheet()
{
  if ( !d->activeSheet )
    return;

  if ( doc()->map()->visibleSheets().count() ==  1)
  {
     KMessageBox::error( this, i18n("You cannot hide the last visible sheet.") );
     return;
  }

  QStringList vs = doc()->map()->visibleSheets();
  int i = vs.findIndex( d->activeSheet->tableName() ) - 1;
  if( i < 0 ) i = 1;
  QString sn = vs[i];

  doc()->emitBeginOperation(false);

  KCommand* command = new HideSheetCommand( activeSheet() );
  doc()->addCommand( command );
  command->execute();

  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );

  d->tabBar->removeTab( d->activeSheet->sheetName() );
  d->tabBar->setActiveTab( sn );
}

void KSpreadView::showSheet()
{
  if ( !d->activeSheet )
    return;

  KSpreadshow dlg( this, "Sheet show");
  dlg.exec();
}

void KSpreadView::copySelection()
{
  if ( !d->activeSheet )
    return;
  if ( !d->canvas->editor() )
  {
    d->activeSheet->copySelection( selectionInfo() );

    updateEditWidget();
  }
  else
    d->canvas->editor()->copy();
}

void KSpreadView::copyAsText()
{
  if ( !d->activeSheet )
    return;
  d->activeSheet->copyAsText( selectionInfo() );
}


void KSpreadView::cutSelection()
{
  if ( !d->activeSheet )
    return;
  //don't used this function when we edit a cell.
  doc()->emitBeginOperation(false);

  if ( !d->canvas->editor())
  {
    d->activeSheet->cutSelection( selectionInfo() );
    resultOfCalc();
    updateEditWidget();
    }
  else
    d->canvas->editor()->cut();

  endOperation( selectionInfo()->selection() );
}

void KSpreadView::paste()
{
  if ( !d->activeSheet )
    return;

  if (!koDocument()->isReadWrite()) // don't paste into a read only document
    return;


  doc()->emitBeginOperation( false );
  if ( !d->canvas->editor() )
  {
      //kdDebug(36001) << "Pasting. Rect= " << selection(false) << " bytes" << endl;
    d->activeSheet->paste( selection(false), true, Normal, OverWrite, false, 0, true );
    resultOfCalc();
    updateEditWidget();
  }
  else
  {
    d->canvas->editor()->paste();
  }
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::specialPaste()
{
  if ( !d->activeSheet )
    return;

  KSpreadspecial dlg( this, "Special Paste" );
  if ( dlg.exec() )
  {
    if ( d->activeSheet->getAutoCalc() )
    {
      doc()->emitBeginOperation( false );
      d->activeSheet->recalc();
      doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
    }
    resultOfCalc();
    updateEditWidget();
  }
}

void KSpreadView::removeComment()
{
  if ( !d->activeSheet )
        return;

  doc()->emitBeginOperation(false);
  d->activeSheet->setSelectionRemoveComment( selectionInfo() );
  updateEditWidget();
  endOperation( selectionInfo()->selection() );
}


void KSpreadView::changeAngle()
{
  if ( !d->activeSheet )
    return;

  KSpreadAngle dlg( this, "Angle" ,
                    QPoint( d->canvas->markerColumn(), d->canvas->markerRow() ));
  if ( dlg.exec() )
  {
    if ( (util_isRowSelected(selection()) == false) &&
        (util_isColumnSelected(selection()) == false) )
    {
      doc()->emitBeginOperation( false );
      d->canvas->adjustArea( false );
      endOperation( selectionInfo()->selection() );
    }
  }
}

void KSpreadView::setSelectionAngle( int angle )
{
  doc()->emitBeginOperation( false );

  if ( d->activeSheet != NULL )
  {
    d->activeSheet->setSelectionAngle( selectionInfo(), angle );

    if (util_isRowSelected(selection()) == false &&
        util_isColumnSelected(selection()) == false)
    {
      d->canvas->adjustArea(false);
    }
  }

  endOperation( selectionInfo()->selection() );
}

void KSpreadView::mergeCell()
{
  // sanity check
  if( !d->activeSheet )
    return;
  if( d->activeSheet->isProtected() )
    return;
  if( doc()->map()->isProtected() )
    return;

  if ( ( util_isRowSelected( selection() ) )
       || ( util_isColumnSelected( selection() ) ) )
  {
    KMessageBox::error( this, i18n( "Area is too large." ) );
    return;
  }

  QPoint topLeft = selection().topLeft();
  KSpreadCell *cell = d->activeSheet->nonDefaultCell( topLeft );
  KCommand* command = new MergeCellCommand( cell, selection().width() - 1,
      selection().height() - 1);
  doc()->addCommand( command );
  command->execute();
}

void KSpreadView::dissociateCell()
{
  // sanity check
  if( !d->activeSheet )
    return;
  if( d->activeSheet->isProtected() )
    return;
  if( doc()->map()->isProtected() )
    return;

  KSpreadCell* cell = d->activeSheet->nonDefaultCell( QPoint( d->canvas->markerColumn(),
      d->canvas->markerRow() ) );
  KCommand* command = new DissociateCellCommand( cell );
  doc()->addCommand( command );
  command->execute();
}


void KSpreadView::increaseIndent()
{
  if ( !d->activeSheet )
    return;

  doc()->emitBeginOperation( false );
  d->activeSheet->increaseIndent( d->selectionInfo );
  updateEditWidget();
  endOperation( d->selectionInfo->selection() );
}

void KSpreadView::decreaseIndent()
{
  if ( !d->activeSheet )
    return;

  doc()->emitBeginOperation( false );
  int column = d->canvas->markerColumn();
  int row = d->canvas->markerRow();

  d->activeSheet->decreaseIndent( d->selectionInfo );
  KSpreadCell * cell = d->activeSheet->cellAt( column, row );
  if ( cell )
    if ( !d->activeSheet->isProtected() )
      d->actions->decreaseIndent->setEnabled( cell->getIndent( column, row ) > 0.0 );

  endOperation( d->selectionInfo->selection() );
}

void KSpreadView::goalSeek()
{
  if ( d->canvas->editor() )
  {
    d->canvas->deleteEditor( true ); // save changes
  }

  KSpreadGoalSeekDlg * dlg
    = new KSpreadGoalSeekDlg( this, QPoint( d->canvas->markerColumn(),
                                            d->canvas->markerRow() ),
                              "KSpreadGoalSeekDlg" );
  dlg->show();
  /* dialog autodeletes itself */
}

void KSpreadView::subtotals()
{
  QRect selection( d->selectionInfo->selection() );
  if ( ( selection.width() < 2 )
       || ( selection.height() < 2 ) )
  {
    KMessageBox::error( this, i18n("You must select multiple cells.") );
    return;
  }

  KSpreadSubtotalDlg dlg(this, selection, "KSpreadSubtotalDlg" );
  if ( dlg.exec() )
  {
    doc()->emitBeginOperation( false );
    d->selectionInfo->setSelection( dlg.selection().topLeft(),
                                   dlg.selection().bottomRight(),
                                   dlg.sheet() );
    endOperation( selection );
  }
}

void KSpreadView::multipleOperations()
{
  if ( d->canvas->editor() )
  {
    d->canvas->deleteEditor( true ); // save changes
  }
  //  KSpreadMultipleOpDlg * dlg = new KSpreadMultipleOpDlg( this, "KSpreadMultipleOpDlg" );
  //  dlg->show();
}

void KSpreadView::textToColumns()
{
  d->canvas->closeEditor();

  QRect area=d->selectionInfo->selection();

  //Only use the first column
  area.setRight(area.left());

/* if ( d->selectionInfo->selection().width() > 1 )
  {
	//Only use the first column

    KMessageBox::error( this, i18n("You must not select an area containing more than one column.") );
    return;
  }*/

  KSpreadCSVDialog dialog( this, "KSpreadCSVDialog", area, KSpreadCSVDialog::Column );
  if( !dialog.cancelled() )
    dialog.exec();
}

void KSpreadView::consolidate()
{
  d->canvas->closeEditor();
  KSpreadConsolidate * dlg = new KSpreadConsolidate( this, "Consolidate" );
  dlg->show();
  // dlg destroys itself
}

void KSpreadView::sortList()
{
  KSpreadList dlg( this, "List selection" );
  dlg.exec();
}

void KSpreadView::gotoCell()
{
  KSpreadGotoDlg dlg( this, "GotoCell" );
  dlg.exec();
}

void KSpreadView::find()
{
    KSpreadFindDlg dlg( this, "Find", d->findOptions, d->findStrings );
    dlg.setHasSelection( !d->selectionInfo->singleCellSelection() );
    dlg.setHasCursor( true );
    if ( KFindDialog::Accepted != dlg.exec() )
        return;

    // Save for next time
    d->findOptions = dlg.options();
    d->findStrings = dlg.findHistory();
    d->typeValue = dlg.searchType();
    d->directionValue = dlg.searchDirection();

    // Create the KFind object
    delete d->find;
    delete d->replace;
    d->find = new KFind( dlg.pattern(), dlg.options(), this );
    d->replace = 0L;

    d->searchInSheets.currentSheet = activeSheet();
    d->searchInSheets.firstSheet = d->searchInSheets.currentSheet;

    initFindReplace();
    findNext();
}

// Initialize a find or replace operation, using d->find or d->replace,
// and d->findOptions.
void KSpreadView::initFindReplace()
{
    KFind* findObj = d->find ? d->find : d->replace;
    Q_ASSERT( findObj );
    connect(findObj, SIGNAL( highlight( const QString &, int, int ) ),
            this, SLOT( slotHighlight( const QString &, int, int ) ) );
    connect(findObj, SIGNAL( findNext() ),
            this, SLOT( findNext() ) );

    bool bck = d->findOptions & KFindDialog::FindBackwards;
    KSpreadSheet* currentSheet = d->searchInSheets.currentSheet;

    QRect region = ( d->findOptions & KFindDialog::SelectedText )
                   ? d->selectionInfo->selection()
                   : QRect( 1, 1, currentSheet->maxColumn(), currentSheet->maxRow() ); // All cells

    int colStart = !bck ? region.left() : region.right();
    int colEnd = !bck ? region.right() : region.left();
    int rowStart = !bck ? region.top() :region.bottom();
    int rowEnd = !bck ? region.bottom() : region.top();
    if ( d->findOptions & KFindDialog::FromCursor ) {
        QPoint marker( d->selectionInfo->marker() );
        colStart = marker.x();
        rowStart = marker.y();
    }
    d->findLeftColumn = region.left();
    d->findRightColumn = region.right();
    d->findPos = QPoint( colStart, rowStart );
    d->findEnd = QPoint( colEnd, rowEnd );
    //kdDebug() << k_funcinfo << d->findPos << " to " << d->findEnd << endl;
    //kdDebug() << k_funcinfo << "leftcol=" << d->findLeftColumn << " rightcol=" << d->findRightColumn << endl;
}

void KSpreadView::findNext()
{
    KFind* findObj = d->find ? d->find : d->replace;
    if ( !findObj )  {
        find();
        return;
    }
    KFind::Result res = KFind::NoMatch;
    KSpreadCell* cell = findNextCell();
    bool forw = ! ( d->findOptions & KFindDialog::FindBackwards );
    while ( res == KFind::NoMatch && cell )
    {
        if ( findObj->needData() )
        {
            if ( d->typeValue == KSpreadFindOption::Note )
                findObj->setData( cell->comment( cell->column(), cell->row() ) );
            else
                findObj->setData( cell->text() );
            d->findPos = QPoint( cell->column(), cell->row() );
            //kdDebug() << "setData(cell " << d->findPos << ")" << endl;
        }

        // Let KFind inspect the text fragment, and display a dialog if a match is found
        if ( d->find )
            res = d->find->find();
        else
            res = d->replace->replace();

        if ( res == KFind::NoMatch )  {
            // Go to next cell, skipping unwanted cells
            if ( d->directionValue == KSpreadFindOption::Row )
            {
                if ( forw )
                    ++d->findPos.rx();
                else
                    --d->findPos.rx();
            }
            else
            {
               if ( forw )
                    ++d->findPos.ry();
                else
                    --d->findPos.ry();
             }
            cell = findNextCell();
        }
    }

    if ( res == KFind::NoMatch )
    {
        //emitUndoRedo();
        //removeHighlight();
        if ( findObj->shouldRestart() ) {
            d->findOptions &= ~KFindDialog::FromCursor;
            findObj->resetCounts();
            findNext();
        }
        else { // done, close the 'find next' dialog
            if ( d->find )
                d->find->closeFindNextDialog();
            else
                d->replace->closeReplaceNextDialog();
        }
    }
}

KSpreadCell* KSpreadView::nextFindValidCell( int col, int row )
{
    KSpreadCell *cell = d->searchInSheets.currentSheet->cellAt( col, row );
    if ( cell->isDefault() || cell->isObscured() || cell->isFormula() )
        cell = 0L;
    if ( d->typeValue == KSpreadFindOption::Note && cell && cell->comment(col, row).isEmpty())
        cell = 0L;
    return cell;
}

KSpreadCell* KSpreadView::findNextCell()
{
    // getFirstCellRow / getNextCellRight would be faster at doing that,
    // but it doesn't seem to be easy to combine it with 'start a column d->find.x()'...

    KSpreadSheet* sheet = d->searchInSheets.currentSheet;
    KSpreadCell* cell = 0L;
    bool forw = ! ( d->findOptions & KFindDialog::FindBackwards );
    int col = d->findPos.x();
    int row = d->findPos.y();
    int maxRow = sheet->maxRow();
    //kdDebug() << "findNextCell starting at " << col << "," << row << "   forw=" << forw << endl;

    if ( d->directionValue == KSpreadFindOption::Row )
    {
        while ( !cell && row != d->findEnd.y() && (forw ? row < maxRow : row >= 0) )
        {
            while ( !cell && (forw ? col <= d->findRightColumn : col >= d->findLeftColumn) )
            {
                cell = nextFindValidCell( col, row );
                if ( forw ) ++col;
                else --col;
            }
            if ( cell )
                break;
            // Prepare looking in the next row
            if ( forw )  {
                col = d->findLeftColumn;
                ++row;
            } else {
                col = d->findRightColumn;
                --row;
            }
            //kdDebug() << "next row: " << col << "," << row << endl;
        }
    }
    else
    {
        while ( !cell && (forw ? col <= d->findRightColumn : col >= d->findLeftColumn) )
        {
            while ( !cell && row != d->findEnd.y() && (forw ? row < maxRow : row >= 0) )
            {
                cell = nextFindValidCell( col, row );
                if ( forw ) ++row;
                else --row;
            }
            if ( cell )
                break;
            // Prepare looking in the next col
            if ( forw )  {
                row = 0;
                ++col;
            } else {
                col = maxRow;
                --col;
            }
            //kdDebug() << "next row: " << col << "," << row << endl;
        }
    }
    // if ( !cell )
    // No more next cell - TODO go to next sheet (if not looking in a selection)
    // (and make d->findEnd (max,max) in that case...)
    //kdDebug() << k_funcinfo << " returning " << cell << endl;
    return cell;
}

void KSpreadView::findPrevious()
{
    KFind* findObj = d->find ? d->find : d->replace;
    if ( !findObj )  {
        find();
        return;
    }
    //kdDebug() << "findPrevious" << endl;
    int opt = d->findOptions;
    bool forw = ! ( opt & KFindDialog::FindBackwards );
    if ( forw )
        d->findOptions = ( opt | KFindDialog::FindBackwards );
    else
        d->findOptions = ( opt & ~KFindDialog::FindBackwards );

    findNext();

    d->findOptions = opt; // restore initial options
}

void KSpreadView::replace()
{
    KSpreadSearchDlg dlg( this, "Replace", d->findOptions, d->findStrings, d->replaceStrings );
    dlg.setHasSelection( !d->selectionInfo->singleCellSelection() );
    dlg.setHasCursor( true );
    if ( KReplaceDialog::Accepted != dlg.exec() )
      return;

    d->findOptions = dlg.options();
    d->findStrings = dlg.findHistory();
    d->replaceStrings = dlg.replacementHistory();
    d->typeValue = dlg.searchType();

    delete d->find;
    delete d->replace;
    d->find = 0L;
    d->replace = new KReplace( dlg.pattern(), dlg.replacement(), dlg.options() );
    initFindReplace();
    connect(
        d->replace, SIGNAL( replace( const QString &, int, int, int ) ),
        this, SLOT( slotReplace( const QString &, int, int, int ) ) );

    if ( !doc()->undoLocked() )
    {
        QRect region( d->findPos, d->findEnd );
        //TODO create undo/redo for comment
        KSpreadUndoChangeAreaTextCell *undo = new KSpreadUndoChangeAreaTextCell( doc(), d->searchInSheets.currentSheet, region );
        doc()->addCommand( undo );
    }

    findNext();

#if 0
    // Refresh the editWidget
    // TODO - after a replacement only?
    KSpreadCell *cell = activeSheet()->cellAt( canvasWidget()->markerColumn(),
                                               canvasWidget()->markerRow() );
    if ( cell->text() != 0L )
        editWidget()->setText( cell->text() );
    else
        editWidget()->setText( "" );
#endif
}

void KSpreadView::slotHighlight( const QString &/*text*/, int /*matchingIndex*/, int /*matchedLength*/ )
{
    d->canvas->gotoLocation( d->findPos, d->searchInSheets.currentSheet );
    KDialogBase *baseDialog=0L;
    if ( d->find )
        baseDialog = d->find->findNextDialog();
    else
        baseDialog = d->replace->replaceNextDialog();
    kdDebug()<<" baseDialog :"<<baseDialog<<endl;
    QRect globalRect( d->findPos, d->findEnd );
    globalRect.moveTopLeft( canvasWidget()->mapToGlobal( globalRect.topLeft() ) );
    KDialog::avoidArea( baseDialog, QRect( d->findPos, d->findEnd ));
}

void KSpreadView::slotReplace( const QString &newText, int, int, int )
{
    // Which cell was this again?
    KSpreadCell *cell = d->searchInSheets.currentSheet->cellAt( d->findPos );

    // ...now I remember, update it!
    cell->setDisplayDirtyFlag();
    if ( d->typeValue == KSpreadFindOption::Value )
        cell->setCellText( newText );
    else if ( d->typeValue == KSpreadFindOption::Note )
        cell->setComment( newText );
    cell->clearDisplayDirtyFlag();
}

void KSpreadView::conditional()
{
  QRect rect( d->selectionInfo->selection() );

  if ( util_isRowOrColumnSelected(selection()))
  {
    KMessageBox::error( this, i18n("Area is too large.") );
  }
  else
  {
    KSpreadConditionalDlg dlg( this, "KSpreadConditionalDlg", rect);
    dlg.exec();
  }
}

void KSpreadView::validity()
{
  QRect rect( d->selectionInfo->selection() );

  if ( (util_isRowSelected(selection())) || (util_isColumnSelected(selection())) )
  {
    KMessageBox::error( this, i18n("Area is too large."));
  }
  else
  {
    KSpreadDlgValidity dlg( this,"validity",rect);
    dlg.exec();
  }
}


void KSpreadView::insertSeries()
{
    d->canvas->closeEditor();
    KSpreadSeriesDlg dlg( this, "Series", QPoint( d->canvas->markerColumn(), d->canvas->markerRow() ) );
    dlg.exec();
}

void KSpreadView::sort()
{
    if ( d->selectionInfo->singleCellSelection() )
    {
        KMessageBox::error( this, i18n("You must select multiple cells.") );
        return;
    }

    KSpreadSortDlg dlg( this, "Sort" );
    dlg.exec();
}

void KSpreadView::removeHyperlink()
{
    QPoint marker( selectionInfo()->marker() );
    KSpreadCell * cell = d->activeSheet->cellAt( marker );
    if( !cell ) return;
    if( cell->link().isEmpty() ) return;

    LinkCommand* command = new LinkCommand( cell, QString::null, QString::null );
    doc()->addCommand( command );
    command->execute();

	canvasWidget()->setFocus();
	editWidget()->setText( cell->text() );
}

void KSpreadView::insertHyperlink()
{
    d->canvas->closeEditor();

    QPoint marker( selectionInfo()->marker() );
    KSpreadCell* cell = d->activeSheet->cellAt( marker );

    LinkDialog* dlg = new LinkDialog( this );
    dlg->setCaption( i18n( "Insert Link" ) );
    if( cell )
    {
      dlg->setText( cell->text() );
      if( !cell->link().isEmpty() )
      {
        dlg->setCaption( i18n( "Edit Link" ) );
        dlg->setLink( cell->link() );
      }
    }

    if( dlg->exec() == KDialog::Accepted )
    {
        cell = d->activeSheet->nonDefaultCell( marker );

        LinkCommand* command = new LinkCommand( cell, dlg->text(), dlg->link() );
        doc()->addCommand( command );
        command->execute();

        //refresh editWidget
	    canvasWidget()->setFocus();
	    editWidget()->setText( cell->text() );
    }
    delete dlg;
}

void KSpreadView::insertFromDatabase()
{
#ifndef QT_NO_SQL
    d->canvas->closeEditor();

    QRect rect = d->selectionInfo->selection();

	QStringList str = QSqlDatabase::drivers();
 	if ( str.isEmpty() )
  	{
    	KMessageBox::error( this, i18n("No database drivers available.  To use this feature you need "
				"to install the necessary Qt 3 database drivers.") );

		return;
  	}

    KSpreadDatabaseDlg dlg(this, rect, "KSpreadDatabaseDlg");
    dlg.exec();
#endif
}

void KSpreadView::insertFromTextfile()
{
    d->canvas->closeEditor();
    //KMessageBox::information( this, "Not implemented yet, work in progress...");

    KSpreadCSVDialog dialog( this, "KSpreadCSVDialog", selection(), KSpreadCSVDialog::File );
    if( !dialog.cancelled() )
      dialog.exec();
}

void KSpreadView::insertFromClipboard()
{
    d->canvas->closeEditor();

    KSpreadCSVDialog dialog( this, "KSpreadCSVDialog", d->selectionInfo->selection(), KSpreadCSVDialog::Clipboard );
    if( !dialog.cancelled() )
      dialog.exec();
}

void KSpreadView::setupPrinter( KPrinter &prt )
{
    KSpreadSheetPrint* print = d->activeSheet->print();

    //apply page layout parameters
    KoFormat pageFormat = print->paperFormat();

    prt.setPageSize( static_cast<KPrinter::PageSize>( KoPageFormat::printerPageSize( pageFormat ) ) );

    if ( print->orientation() == PG_LANDSCAPE || pageFormat == PG_SCREEN )
        prt.setOrientation( KPrinter::Landscape );
    else
        prt.setOrientation( KPrinter::Portrait );

    prt.setFullPage( true );
}

void KSpreadView::print( KPrinter &prt )
{
    KSpreadSheetPrint* print = d->activeSheet->print();

    if ( d->canvas->editor() )
    {
      d->canvas->deleteEditor( true ); // save changes
    }

    int oldZoom = doc()->zoom();

    //Comment from KWord
    //   We don't get valid metrics from the printer - and we want a better resolution
    //   anyway (it's the PS driver that takes care of the printer resolution).
    //But KSpread uses fixed 300 dpis, so we can use it.

    QPaintDeviceMetrics metrics( &prt );

    int dpiX = metrics.logicalDpiX();
    int dpiY = metrics.logicalDpiY();

    doc()->setZoomAndResolution( int( print->zoom() * 100 ), dpiX, dpiY );

    //store the current setting in a temporary variable
    KoOrientation _orient = print->orientation();

    QPainter painter;

    painter.begin( &prt );

    //use the current orientation from print dialog
    if ( prt.orientation() == KPrinter::Landscape )
    {
        print->setPaperOrientation( PG_LANDSCAPE );
    }
    else
    {
        print->setPaperOrientation( PG_PORTRAIT );
    }

    bool result = print->print( painter, &prt );

    //Restore original orientation
    print->setPaperOrientation( _orient );

    doc()->setZoomAndResolution( oldZoom, KoGlobal::dpiX(), KoGlobal::dpiY() );
    doc()->newZoomAndResolution( true, false );

    // Repaint at correct zoom
    doc()->emitBeginOperation( false );
    setZoom( oldZoom, false );
    doc()->emitEndOperation();

    // Nothing to print
    if( !result )
    {
      if( !prt.previewOnly() )
      {
        KMessageBox::information( 0, i18n("Nothing to print.") );
        prt.abort();
      }
    }

    painter.end();
}

void KSpreadView::insertChart( const QRect& _geometry, KoDocumentEntry& _e )
{
    if ( !d->activeSheet )
      return;

    // Transform the view coordinates to document coordinates
    KoRect unzoomedRect = doc()->unzoomRect( _geometry );
    unzoomedRect.moveBy( d->canvas->xOffset(), d->canvas->yOffset() );

    //KOfficeCore cannot handle KoRect directly, so switching to QRect
    QRect unzoomedGeometry = unzoomedRect.toQRect();

    if ( (util_isRowSelected(selection())) || (util_isColumnSelected(selection())) )
    {
      KMessageBox::error( this, i18n("Area is too large."));
      d->activeSheet->insertChart( unzoomedGeometry,
                             _e,
                             QRect( d->canvas->markerColumn(),
                                    d->canvas->markerRow(),
                                    1,
                                    1 ) );
    }
    else
    {
      // Insert the new child in the active sheet.
      d->activeSheet->insertChart( unzoomedGeometry,
                             _e,
                             d->selectionInfo->selection() );
    }
}

void KSpreadView::insertChild( const QRect& _geometry, KoDocumentEntry& _e )
{
  if ( !d->activeSheet )
    return;

  // Transform the view coordinates to document coordinates
  KoRect unzoomedRect = doc()->unzoomRect( _geometry );
  unzoomedRect.moveBy( d->canvas->xOffset(), d->canvas->yOffset() );

  //KOfficeCore cannot handle KoRect directly, so switching to QRect
  QRect unzoomedGeometry = unzoomedRect.toQRect();

  // Insert the new child in the active sheet.
  d->activeSheet->insertChild( unzoomedGeometry, _e );
}

void KSpreadView::slotRemoveChild( KSpreadChild *_child )
{
  if ( _child->sheet() != d->activeSheet )
    return;

  // Make shure that this child has no active embedded view -> activate ourselfs
  doc()->emitBeginOperation( false );
  partManager()->setActivePart( koDocument(), this );
  partManager()->setSelectedPart( 0 );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::slotUpdateChildGeometry( KSpreadChild */*_child*/ )
{
    // ##############
    // TODO
    /*
  if ( _child->sheet() != d->activeSheet )
    return;

  // Find frame for child
  KSpreadChildFrame *f = 0L;
  QPtrListIterator<KSpreadChildFrame> it( m_lstFrames );
  for ( ; it.current() && !f; ++it )
    if ( it.current()->child() == _child )
      f = it.current();

  assert( f != 0L );

  // Are we already up to date ?
  if ( _child->geometry() == f->partGeometry() )
    return;

  // TODO zooming
  f->setPartGeometry( _child->geometry() );
    */
}

void KSpreadView::toggleProtectDoc( bool mode )
{
   if ( !doc() || !doc()->map() )
     return;

   QCString passwd;
   if ( mode )
   {
     int result = KPasswordDialog::getNewPassword( passwd, i18n( "Protect Document" ) );
     if ( result != KPasswordDialog::Accepted )
     {
       d->actions->protectDoc->setChecked( false );
       return;
     }

     QCString hash( "" );
     QString password( passwd );
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );
     doc()->map()->setProtected( hash );
   }
   else
   {
     int result = KPasswordDialog::getPassword( passwd, i18n( "Unprotect Document" ) );
     if ( result != KPasswordDialog::Accepted )
     {
       d->actions->protectDoc->setChecked( true );
       return;
     }

     QCString hash( "" );
     QString password( passwd );
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );
     if ( !doc()->map()->checkPassword( hash ) )
     {
       KMessageBox::error( 0, i18n( "Password is incorrect." ) );
       d->actions->protectDoc->setChecked( true );
       return;
     }

     doc()->map()->setProtected( QCString() );
   }

   doc()->setModified( true );
   d->adjustWorkbookActions( !mode );
}

void KSpreadView::toggleProtectSheet( bool mode )
{
   if ( !d->activeSheet )
       return;

   QCString passwd;
   if ( mode )
   {
     int result = KPasswordDialog::getNewPassword( passwd, i18n( "Protect Sheet" ) );
     if ( result != KPasswordDialog::Accepted )
     {
       d->actions->protectSheet->setChecked( false );
       return;
     }

     QCString hash( "" );
     QString password( passwd );
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );

     d->activeSheet->setProtected( hash );
   }
   else
   {
     int result = KPasswordDialog::getPassword( passwd, i18n( "Unprotect Sheet" ) );
     if ( result != KPasswordDialog::Accepted )
     {
       d->actions->protectSheet->setChecked( true );
       return;
     }

     QCString hash( "" );
     QString password( passwd );
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );

     if ( !d->activeSheet->checkPassword( hash ) )
     {
       KMessageBox::error( 0, i18n( "Password is incorrect." ) );
       d->actions->protectSheet->setChecked( true );
       return;
     }

     d->activeSheet->setProtected( QCString() );
   }
   doc()->setModified( true );
   d->adjustActions( !mode );
   doc()->emitBeginOperation();
   // d->activeSheet->setRegionPaintDirty( QRect(QPoint( 0, 0 ), QPoint( KS_colMax, KS_rowMax ) ) );
   refreshView();
   updateEditWidget();
   doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::togglePageBorders( bool mode )
{
  if ( !d->activeSheet )
    return;

  doc()->emitBeginOperation( false );
  d->activeSheet->setShowPageBorders( mode );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::viewZoom( const QString & s )
{
  int oldZoom = doc()->zoom();

  bool ok = false;
  QRegExp regexp("(\\d+)"); // "Captured" non-empty sequence of digits
  regexp.search(s);
  int newZoom=regexp.cap(1).toInt(&ok);
  if ( !ok || newZoom < 10 ) //zoom should be valid and >10
    newZoom = oldZoom;
  if ( newZoom != oldZoom )
  {
    d->actions->viewZoom->setZoom( newZoom );

    doc()->emitBeginOperation( false );

    d->canvas->closeEditor();
    setZoom( newZoom, true );

    QRect r( d->activeSheet->visibleRect( d->canvas ) );
    r.setWidth( r.width() + 2 );
    doc()->emitEndOperation( r );
  }
}

void KSpreadView::setZoom( int zoom, bool /*updateViews*/ )
{
  kdDebug() << "---------SetZoom: " << zoom << endl;

  // Set the zoom in KoView (for embedded views)
  doc()->emitBeginOperation( false );

  doc()->setZoomAndResolution( zoom, KoGlobal::dpiX(), KoGlobal::dpiY());
  KoView::setZoom( doc()->zoomedResolutionY() /* KoView only supports one zoom */ );

  Q_ASSERT(d->activeSheet);

  if (d->activeSheet)  //this is 0 when viewing a document in konqueror!? (see Q_ASSERT above)
    d->activeSheet->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));

  doc()->refreshInterface();
  doc()->emitEndOperation();
}

void KSpreadView::showStatusBar( bool b )
{
  doc()->setShowStatusBar( b );
  refreshView();
}

void KSpreadView::showTabBar( bool b )
{
  doc()->setShowTabBar( b );
  refreshView();
}

void KSpreadView::showFormulaBar( bool b )
{
  doc()->setShowFormulaBar( b );
  refreshView();
}

void KSpreadView::showCommentIndicator( bool b )
{
  doc()->setShowCommentIndicator( b );

  d->adjustActions( !d->activeSheet->isProtected() );

  doc()->emitBeginOperation( false );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  refreshView();
}

void KSpreadView::preference()
{
  if ( !d->activeSheet )
    return;

  KSpreadpreference dlg( this, "Preference" );
  if ( dlg.exec() )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->refreshPreference();
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void KSpreadView::addModifyComment()
{
  if ( !d->activeSheet )
    return;

  KSpreadComment dlg( this, "comment",
                      QPoint( d->canvas->markerColumn(),
                              d->canvas->markerRow() ) );
  if ( dlg.exec() )
    updateEditWidget();
}

void KSpreadView::setSelectionComment( QString comment )
{
  if ( d->activeSheet != NULL )
  {
    doc()->emitBeginOperation( false );

    d->activeSheet->setSelectionComment( selectionInfo(), comment.stripWhiteSpace() );
    updateEditWidget();

    endOperation( d->selectionInfo->selection() );
  }
}

void KSpreadView::editCell()
{
  if ( d->canvas->editor() )
    return;

  d->canvas->createEditor();
}

bool KSpreadView::showSheet(const QString& sheetName) {
  KSpreadSheet *t=doc()->map()->findSheet(sheetName);
  if ( !t )
  {
    kdDebug(36001) << "Unknown sheet " <<sheetName<<  endl;
    return false;
  }
  d->canvas->closeEditor();
  setActiveSheet( t );

  return true;
}

void KSpreadView::nextSheet(){

  KSpreadSheet * t = doc()->map()->nextSheet( activeSheet() );
  if ( !t )
  {
    kdDebug(36001) << "Unknown sheet " <<  endl;
    return;
  }
  d->canvas->closeEditor();
  setActiveSheet( t );
}

void KSpreadView::previousSheet()
{
  KSpreadSheet * t = doc()->map()->previousSheet( activeSheet() );
  if ( !t )
  {
    kdDebug(36001) << "Unknown sheet "  << endl;
    return;
  }
  d->canvas->closeEditor();
  setActiveSheet( t );
}

void KSpreadView::firstSheet()
{
  KSpreadSheet *t = doc()->map()->firstSheet();
  if ( !t )
  {
    kdDebug(36001) << "Unknown sheet " <<  endl;
    return;
  }
  d->canvas->closeEditor();
  setActiveSheet( t );
}

void KSpreadView::lastSheet()
{
  KSpreadSheet *t = doc()->map()->lastSheet( );
  if ( !t )
  {
    kdDebug(36001) << "Unknown sheet " <<  endl;
    return;
  }
  d->canvas->closeEditor();
  setActiveSheet( t );
}

void KSpreadView::keyPressEvent ( QKeyEvent* _ev )
{
  // Dont eat accelerators
  if ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) )
  {
    if ( _ev->state() & ( Qt::ControlButton ) )
    {
      switch( _ev->key() )
      {
#ifndef NDEBUG
       case Qt::Key_V: // Ctrl+Shift+V to show debug (similar to KWord)
        if ( _ev->state() & Qt::ShiftButton )
          d->activeSheet->printDebug();
#endif
       default:
        QWidget::keyPressEvent( _ev );
        return;
      }
    }
    QWidget::keyPressEvent( _ev );
  }
  else
   	QApplication::sendEvent( d->canvas, _ev );
}

KoDocument * KSpreadView::hitTest( const QPoint &pos )
{
    // Code copied from KoView::hitTest
    KoViewChild *viewChild;

    QWMatrix m = matrix();
    m.translate( d->canvas->xOffset() / doc()->zoomedResolutionX(),
                 d->canvas->yOffset() / doc()->zoomedResolutionY() );

    KoDocumentChild *docChild = selectedChild();
    if ( docChild )
    {
        if ( ( viewChild = child( docChild->document() ) ) )
        {
            if ( viewChild->frameRegion( m ).contains( pos ) )
                return 0;
        }
        else
            if ( docChild->frameRegion( m ).contains( pos ) )
                return 0;
    }

    docChild = activeChild();
    if ( docChild )
    {
        if ( ( viewChild = child( docChild->document() ) ) )
        {
            if ( viewChild->frameRegion( m ).contains( pos ) )
                return 0;
        }
        else
            if ( docChild->frameRegion( m ).contains( pos ) )
                return 0;
    }

    QPtrListIterator<KoDocumentChild> it( doc()->children() );
    for (; it.current(); ++it )
    {
        // Is the child document on the visible sheet ?
        if ( ((KSpreadChild*)it.current())->sheet() == d->activeSheet )
        {
            KoDocument *doc = it.current()->hitTest( pos, m );
            if ( doc )
                return doc;
        }
    }

    return doc();
}

int KSpreadView::leftBorder() const
{
  return int( d->canvas->doc()->zoomItX( YBORDER_WIDTH ) );
}

int KSpreadView::rightBorder() const
{
  return d->vertScrollBar->width();
}

int KSpreadView::topBorder() const
{
  return d->toolWidget->height() + int( d->canvas->doc()->zoomItX( KSpreadFormat::globalRowHeight() + 2 ) );
}

int KSpreadView::bottomBorder() const
{
  return d->horzScrollBar->height();
}

void KSpreadView::refreshView()
{
  kdDebug() << "refreshing view" << endl;

  KSpreadSheet * sheet = activeSheet();
  if ( !sheet )
    return;

  d->adjustActions( !sheet->isProtected() );
  d->actions->viewZoom->setZoom( doc()->zoom() );

  bool active = sheet->getShowFormula();
  if ( sheet && !sheet->isProtected() )
  {
    d->actions->alignLeft->setEnabled( !active );
    d->actions->alignCenter->setEnabled( !active );
    d->actions->alignRight->setEnabled( !active );
  }

  d->tabBar->setReadOnly( !doc()->isReadWrite() );

  d->toolWidget->setShown( doc()->showFormulaBar() );
  editWidget()->showEditWidget( doc()->showFormulaBar() );
  d->hBorderWidget->setShown( doc()->showColumnHeader() );
  d->vBorderWidget->setShown( doc()->showRowHeader() );
  d->vertScrollBar->setShown( doc()->showVerticalScrollBar() );
  d->horzScrollBar->setShown( doc()->showHorizontalScrollBar() );
  d->tabBar->setShown( doc()->showTabBar() );
  if ( statusBar() ) statusBar()->setShown( doc()->showStatusBar() );

  d->canvas->updatePosWidget();

  d->hBorderWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  d->hBorderWidget->setMinimumHeight( doc()->zoomItY( KSpreadFormat::globalRowHeight() + 2 ) );
  d->vBorderWidget->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding );
  d->vBorderWidget->setMinimumWidth( doc()->zoomItX( YBORDER_WIDTH ) );

  KSpreadSheet::LayoutDirection sheetDir = sheet->layoutDirection();
  bool interfaceIsRTL = QApplication::reverseLayout();

  kdDebug()<<" sheetDir == KSpreadSheet::LeftToRight :"<<( sheetDir == KSpreadSheet::LeftToRight )<<endl;
  if ((sheetDir == KSpreadSheet::LeftToRight && !interfaceIsRTL) ||
      (sheetDir == KSpreadSheet::RightToLeft && interfaceIsRTL))
  {
    d->formulaBarLayout->setDirection( QBoxLayout::LeftToRight );
    d->viewLayout->setOrigin( QGridLayout::TopLeft );
    d->tabScrollBarLayout->setDirection( QBoxLayout::LeftToRight );
    d->tabBar->setReverseLayout( interfaceIsRTL );
  }
  else
  {
    d->formulaBarLayout->setDirection( QBoxLayout::RightToLeft );
    d->viewLayout->setOrigin( QGridLayout::TopRight );
    d->tabScrollBarLayout->setDirection( QBoxLayout::RightToLeft );
    d->tabBar->setReverseLayout( !interfaceIsRTL );
  }

}

void KSpreadView::resizeEvent( QResizeEvent * )
{
  refreshView();
}

void KSpreadView::popupChildMenu( KoChild* child, const QPoint& global_pos )
{
    if ( !child )
	return;

    delete d->popupChild;

    d->popupChildObject = static_cast<KSpreadChild*>(child);

    d->popupChild = new QPopupMenu( this );

    d->popupChild->insertItem( i18n("Delete Embedded Document"), this, SLOT( slotPopupDeleteChild() ) );

    d->popupChild->popup( global_pos );
}

void KSpreadView::slotPopupDeleteChild()
{
    if ( !d->popupChildObject || !d->popupChildObject->sheet() )
	return;

    //Removed popup warning dialog because
    // a) It is annoying from a user's persepective
    // b) The behaviour should be consistant with other KOffice apps

    /*int ret = KMessageBox::warningContinueCancel(this,i18n("You are about to remove this embedded document.\nDo you want to continue?"),i18n("Delete Embedded Document"),KGuiItem(i18n("&Delete"),"editdelete"));
    if ( ret == KMessageBox::Continue )
    {

}*/
    doc()->emitBeginOperation(false);
    d->popupChildObject->sheet()->deleteChild( d->popupChildObject );
    d->popupChildObject = 0;
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::popupColumnMenu( const QPoint & _point )
{
  assert( d->activeSheet );

  if ( !koDocument()->isReadWrite() )
    return;

    delete d->popupColumn ;

    d->popupColumn = new QPopupMenu( this );

    bool isProtected = d->activeSheet->isProtected();

    if ( !isProtected )
    {
      d->actions->cellLayout->plug( d->popupColumn );
      d->popupColumn->insertSeparator();
      d->actions->cut->plug( d->popupColumn );
    }
    d->actions->copy->plug( d->popupColumn );
    if ( !isProtected )
    {
      d->actions->paste->plug( d->popupColumn );
      d->actions->specialPaste->plug( d->popupColumn );
      d->actions->insertCellCopy->plug( d->popupColumn );
      d->popupColumn->insertSeparator();
      d->actions->defaultFormat->plug( d->popupColumn );
      // If there is no selection
      if ((util_isRowSelected(selection()) == false) && (util_isColumnSelected(selection()) == FALSE) )
      {
        d->actions->areaName->plug( d->popupColumn );
      }

      d->actions->resizeColumn->plug( d->popupColumn );
      d->popupColumn->insertItem( i18n("Adjust Column"), this, SLOT(slotPopupAdjustColumn() ) );
      d->popupColumn->insertSeparator();
      d->actions->insertColumn->plug( d->popupColumn );
      d->actions->deleteColumn->plug( d->popupColumn );
      d->actions->hideColumn->plug( d->popupColumn );

      d->actions->showSelColumns->setEnabled(false);

      int i;
      ColumnFormat * col;
      QRect rect = d->selectionInfo->selection();
      //kdDebug(36001) << "Column: L: " << rect.left() << endl;
      for ( i = rect.left(); i <= rect.right(); ++i )
      {
        if (i == 2) // "B"
        {
          col = activeSheet()->columnFormat( 1 );
          if ( col->isHide() )
          {
            d->actions->showSelColumns->setEnabled(true);
            d->actions->showSelColumns->plug( d->popupColumn );
            break;
          }
        }

        col = activeSheet()->columnFormat( i );

        if ( col->isHide() )
        {
          d->actions->showSelColumns->setEnabled( true );
          d->actions->showSelColumns->plug( d->popupColumn );
          break;
        }
      }
    }

    QObject::connect( d->popupColumn, SIGNAL(activated( int ) ), this, SLOT( slotActivateTool( int ) ) );

    d->popupColumn->popup( _point );
}

void KSpreadView::slotPopupAdjustColumn()
{
    if ( !d->activeSheet )
       return;

    doc()->emitBeginOperation( false );
    canvasWidget()->adjustArea();
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::popupRowMenu( const QPoint & _point )
{
    assert( d->activeSheet );

    if ( !koDocument()->isReadWrite() )
      return;

    delete d->popupRow ;

    d->popupRow= new QPopupMenu();

    bool isProtected = d->activeSheet->isProtected();

    if ( !isProtected )
    {
        d->actions->cellLayout->plug( d->popupRow );
        d->popupRow->insertSeparator();
        d->actions->cut->plug( d->popupRow );
    }
    d->actions->copy->plug( d->popupRow );
    if ( !isProtected )
    {
      d->actions->paste->plug( d->popupRow );
      d->actions->specialPaste->plug( d->popupRow );
      d->actions->insertCellCopy->plug( d->popupRow );
      d->popupRow->insertSeparator();
      d->actions->defaultFormat->plug( d->popupRow );
      // If there is no selection
      if ( (util_isRowSelected(selection()) == false) && (util_isColumnSelected(selection()) == FALSE) )
      {
	d->actions->areaName->plug( d->popupRow );
      }

      d->actions->resizeRow->plug( d->popupRow );
      d->popupRow->insertItem( i18n("Adjust Row"), this, SLOT( slotPopupAdjustRow() ) );
      d->popupRow->insertSeparator();
      d->actions->insertRow->plug( d->popupRow );
      d->actions->deleteRow->plug( d->popupRow );
      d->actions->hideRow->plug( d->popupRow );

      d->actions->showSelColumns->setEnabled(false);

      int i;
      RowFormat * row;
      QRect rect = d->selectionInfo->selection();
      for ( i = rect.top(); i <= rect.bottom(); ++i )
      {
        //kdDebug(36001) << "popupRow: " << rect.top() << endl;
        if (i == 2)
        {
          row = activeSheet()->rowFormat( 1 );
          if ( row->isHide() )
          {
            d->actions->showSelRows->setEnabled(true);
            d->actions->showSelRows->plug( d->popupRow );
            break;
          }
        }

        row = activeSheet()->rowFormat( i );
        if ( row->isHide() )
        {
          d->actions->showSelRows->setEnabled(true);
          d->actions->showSelRows->plug( d->popupRow );
          break;
        }
      }
    }

    QObject::connect( d->popupRow, SIGNAL( activated( int ) ), this, SLOT( slotActivateTool( int ) ) );
    d->popupRow->popup( _point );
}

void KSpreadView::slotPopupAdjustRow()
{
    if ( !d->activeSheet )
       return;

    doc()->emitBeginOperation(false);
    canvasWidget()->adjustArea();
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}


void KSpreadView::slotListChoosePopupMenu( )
{
  if ( !koDocument()->isReadWrite() )
    return;

  assert( d->activeSheet );
  delete d->popupListChoose;

  d->popupListChoose = new QPopupMenu();
  int id = 0;
  QRect selection( d->selectionInfo->selection() );
  KSpreadCell * cell = d->activeSheet->cellAt( d->canvas->markerColumn(), d->canvas->markerRow() );
  QString tmp = cell->text();
  QStringList itemList;

  for ( int col = selection.left(); col <= selection.right(); ++col )
  {
    KSpreadCell * c = d->activeSheet->getFirstCellColumn( col );
    while ( c )
    {
      if ( !c->isObscuringForced()
           && !( col == d->canvas->markerColumn()
                 && c->row() == d->canvas->markerRow()) )
      {
        if ( c->value().isString() && c->text() != tmp && !c->text().isEmpty() )
        {
          if ( itemList.findIndex( c->text() ) == -1 )
            itemList.append(c->text());
        }
      }

      c = d->activeSheet->getNextCellDown( col, c->row() );
    }
  }

  /* TODO: remove this later:
    for( ;c; c = c->nextCell() )
   {
     int col = c->column();
     if ( selection.left() <= col && selection.right() >= col
	  &&!c->isObscuringForced()&& !(col==d->canvas->markerColumn()&& c->row()==d->canvas->markerRow()))
       {
	 if (c->isString() && c->text()!=tmp && !c->text().isEmpty())
	   {
	     if (itemList.findIndex(c->text())==-1)
                 itemList.append(c->text());
	   }

       }
    }
 */

  for ( QStringList::Iterator it = itemList.begin(); it != itemList.end();++it )
    d->popupListChoose->insertItem( (*it), id++ );

  if ( id == 0 )
    return;
  RowFormat * rl = d->activeSheet->rowFormat( d->canvas->markerRow());
  double tx = d->activeSheet->dblColumnPos( d->canvas->markerColumn(), d->canvas );
  double ty = d->activeSheet->dblRowPos(d->canvas->markerRow(), d->canvas );
  double h = rl->dblHeight( d->canvas );
  if ( cell->extraYCells() )
    h = cell->extraHeight();
  ty += h;

  if ( d->activeSheet->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    tx = canvasWidget()->width() - tx;
  }

  QPoint p( (int)tx, (int)ty );
  QPoint p2 = d->canvas->mapToGlobal( p );

  if ( d->activeSheet->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    p2.setX( p2.x() - d->popupListChoose->sizeHint().width() + 1 );
  }

  d->popupListChoose->popup( p2 );
  QObject::connect( d->popupListChoose, SIGNAL( activated( int ) ),
                    this, SLOT( slotItemSelected( int ) ) );
}


void KSpreadView::slotItemSelected( int id )
{
  QString tmp = d->popupListChoose->text( id );
  int x = d->canvas->markerColumn();
  int y = d->canvas->markerRow();
  KSpreadCell * cell = d->activeSheet->nonDefaultCell( x, y );

  if ( tmp == cell->text() )
    return;

  doc()->emitBeginOperation( false );

  if ( !doc()->undoLocked() )
  {
    KSpreadUndoSetText* undo = new KSpreadUndoSetText( doc(), d->activeSheet, cell->text(),
                                                       x, y, cell->formatType() );
    doc()->addCommand( undo );
  }

  cell->setCellText( tmp );
  editWidget()->setText( tmp );

  doc()->emitEndOperation( QRect( x, y, 1, 1 ) );
}

void KSpreadView::openPopupMenu( const QPoint & _point )
{
    assert( d->activeSheet );
    delete d->popupMenu;

    if ( !koDocument()->isReadWrite() )
        return;

    d->popupMenu = new QPopupMenu();
    KSpreadCell * cell = d->activeSheet->cellAt( d->canvas->markerColumn(), d->canvas->markerRow() );

    bool isProtected = d->activeSheet->isProtected();
    if ( !cell->isDefault() && cell->notProtected( d->canvas->markerColumn(), d->canvas->markerRow() )
         && ( selection().width() == 1 ) && ( selection().height() == 1 ) )
      isProtected = false;

    if ( !isProtected )
    {
      d->actions->cellLayout->plug( d->popupMenu );
      d->popupMenu->insertSeparator();
      d->actions->cut->plug( d->popupMenu );
    }
    d->actions->copy->plug( d->popupMenu );
    if ( !isProtected )
      d->actions->paste->plug( d->popupMenu );

    if ( !isProtected )
    {
      d->actions->specialPaste->plug( d->popupMenu );
      d->actions->insertCellCopy->plug( d->popupMenu );
      d->popupMenu->insertSeparator();
      d->actions->deleteCell->plug( d->popupMenu );
      d->actions->adjust->plug( d->popupMenu );
      d->actions->defaultFormat->plug( d->popupMenu );

      // If there is no selection
      if ( (util_isRowSelected(selection()) == false) && (util_isColumnSelected(selection()) == FALSE) )
      {
        d->actions->areaName->plug( d->popupMenu );
        d->popupMenu->insertSeparator();
        d->actions->insertCell->plug( d->popupMenu );
        d->actions->removeCell->plug( d->popupMenu );
      }

      d->popupMenu->insertSeparator();
      d->actions->addModifyComment->plug( d->popupMenu );
      if ( !cell->comment(d->canvas->markerColumn(), d->canvas->markerRow()).isEmpty() )
      {
        d->actions->removeComment->plug( d->popupMenu );
      }

      if (activeSheet()->testListChoose(selectionInfo()))
      {
	d->popupMenu->insertSeparator();
	d->popupMenu->insertItem( i18n("Selection List..."), this, SLOT( slotListChoosePopupMenu() ) );
      }
    }

    // Remove informations about the last tools we offered
    d->toolList.clear();
    d->toolList.setAutoDelete( true );

    if ( !isProtected && !activeSheet()->getWordSpelling( selectionInfo() ).isEmpty() )
    {
      d->popupMenuFirstToolId = 10;
      int i = 0;
      QValueList<KDataToolInfo> tools = KDataToolInfo::query( "QString", "text/plain", doc()->instance() );
      if ( tools.count() > 0 )
      {
        d->popupMenu->insertSeparator();
        QValueList<KDataToolInfo>::Iterator entry = tools.begin();
        for( ; entry != tools.end(); ++entry )
        {
          QStringList lst = (*entry).userCommands();
          QStringList::ConstIterator it = lst.begin();

          // ### Torben: Insert pixmaps here, too
          for (; it != lst.end(); ++it )
            d->popupMenu->insertItem( *it, d->popupMenuFirstToolId + i++ );

          lst = (*entry).commands();
          it = lst.begin();
          for (; it != lst.end(); ++it )
          {
            ViewPrivate::ToolEntry *t = new ViewPrivate::ToolEntry;
            t->command = *it;
            t->info = *entry;
            d->toolList.append( t );
          }
        }

        QObject::connect( d->popupMenu, SIGNAL( activated( int ) ), this, SLOT( slotActivateTool( int ) ) );
      }
    }

    d->popupMenu->popup( _point );
}

void KSpreadView::slotActivateTool( int _id )
{
  Q_ASSERT( d->activeSheet );

  // Is it the id of a tool in the latest popupmenu ?
  if ( _id < d->popupMenuFirstToolId )
    return;

  ViewPrivate::ToolEntry* entry = d->toolList.at( _id - d->popupMenuFirstToolId );

  KDataTool* tool = entry->info.createTool();
  if ( !tool )
  {
      kdDebug(36001) << "Could not create Tool" << endl;
      return;
  }

  QString text = activeSheet()->getWordSpelling( selectionInfo() );

  if ( tool->run( entry->command, &text, "QString", "text/plain") )
  {
      doc()->emitBeginOperation(false);

      activeSheet()->setWordSpelling( selectionInfo(), text);

      KSpreadCell *cell = d->activeSheet->cellAt( d->canvas->markerColumn(), d->canvas->markerRow() );
      editWidget()->setText( cell->text() );

      doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void KSpreadView::deleteSelection()
{
    Q_ASSERT( d->activeSheet );

    doc()->emitBeginOperation( false );
    d->activeSheet->deleteSelection( selectionInfo() );
    resultOfCalc();
    updateEditWidget();
    endOperation( selectionInfo()->selection() );
}

void KSpreadView::adjust()
{
    if ( (util_isRowSelected(selection())) || (util_isColumnSelected(selection())) )
    {
      KMessageBox::error( this, i18n("Area is too large."));
    }
    else
    {
      doc()->emitBeginOperation( false );
      canvasWidget()->adjustArea();
      endOperation( selection() );
    }
}

void KSpreadView::clearTextSelection()
{
    Q_ASSERT( d->activeSheet );
    doc()->emitBeginOperation( false );
    d->activeSheet->clearTextSelection( selectionInfo() );

    updateEditWidget();
    doc()->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::clearCommentSelection()
{
    Q_ASSERT( d->activeSheet );
    doc()->emitBeginOperation( false );
    d->activeSheet->setSelectionRemoveComment( selectionInfo() );

    updateEditWidget();
    doc()->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::clearValiditySelection()
{
    Q_ASSERT( d->activeSheet );
    doc()->emitBeginOperation( false );
    d->activeSheet->clearValiditySelection( selectionInfo() );

    updateEditWidget();
    doc()->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::clearConditionalSelection()
{
    Q_ASSERT( d->activeSheet );
    doc()->emitBeginOperation( false );
    d->activeSheet->clearConditionalSelection( selectionInfo() );

    updateEditWidget();
    doc()->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::fillRight()
{
  Q_ASSERT( d->activeSheet );
  doc()->emitBeginOperation( false );
  d->activeSheet->fillSelection( selectionInfo(), KSpreadSheet::Right );
  doc()->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::fillLeft()
{
  Q_ASSERT( d->activeSheet );
  doc()->emitBeginOperation( false );
  d->activeSheet->fillSelection( selectionInfo(), KSpreadSheet::Left );
  doc()->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::fillUp()
{
  Q_ASSERT( d->activeSheet );
  doc()->emitBeginOperation( false );
  d->activeSheet->fillSelection( selectionInfo(), KSpreadSheet::Up );
  doc()->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::fillDown()
{
  Q_ASSERT( d->activeSheet );
  doc()->emitBeginOperation( false );
  d->activeSheet->fillSelection( selectionInfo(), KSpreadSheet::Down );
  doc()->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::defaultSelection()
{
  Q_ASSERT( d->activeSheet );
  doc()->emitBeginOperation( false );
  d->activeSheet->defaultSelection( selectionInfo() );

  updateEditWidget();
  doc()->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::slotInsert()
{
  QRect r( selection() );
  KSpreadinsert dlg( this, "Insert", r, KSpreadinsert::Insert );
  dlg.exec();
}

void KSpreadView::slotRemove()
{
  QRect r( d->selectionInfo->selection() );
  KSpreadinsert dlg( this, "Remove", r, KSpreadinsert::Remove );
  dlg.exec();
}

void KSpreadView::slotInsertCellCopy()
{
  if ( !d->activeSheet )
    return;

  if ( !d->activeSheet->testAreaPasteInsert() )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->paste( selection(), true, Normal, OverWrite, true );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
  else
  {
    KSpreadpasteinsert dlg( this, "Remove", selection() );
    dlg.exec();
  }

  if ( d->activeSheet->getAutoCalc() )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->recalc();
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
  updateEditWidget();
}

void KSpreadView::setAreaName()
{
  KSpreadarea dlg( this, "Area Name",QPoint(d->canvas->markerColumn(), d->canvas->markerRow()) );
  dlg.exec();
}

void KSpreadView::showAreaName()
{
  KSpreadreference dlg( this, "Show Area" );
  dlg.exec();
}

void KSpreadView::resizeRow()
{
  if ( util_isColumnSelected(selection()) )
    KMessageBox::error( this, i18n("Area is too large."));
  else
  {
    KSpreadResizeRow dlg( this );
    dlg.exec();
  }
}

void KSpreadView::resizeColumn()
{
  if ( util_isRowSelected( selection() ) )
    KMessageBox::error( this, i18n( "Area is too large." ) );
  else
  {
    KSpreadResizeColumn dlg( this );
    dlg.exec();
  }
}

void KSpreadView::equalizeRow()
{
  if ( util_isColumnSelected( selection() ) )
    KMessageBox::error( this, i18n( "Area is too large." ) );
  else
  {
    doc()->emitBeginOperation( false );
    canvasWidget()->equalizeRow();
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void KSpreadView::equalizeColumn()
{
  if ( util_isRowSelected( selection() ) )
    KMessageBox::error( this, i18n( "Area is too large." ) );
  else
  {
    doc()->emitBeginOperation( false );
    canvasWidget()->equalizeColumn();
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}


void KSpreadView::layoutDlg()
{
  QRect selection( d->selectionInfo->selection() );

  if ( d->selectionInfo->singleCellSelection() )
  {
    CellFormatDlg dlg( this, d->activeSheet, selection.left(), selection.top(),
                       selection.left(), selection.top() );
  }
  else
    CellFormatDlg dlg( this, d->activeSheet, selection.left(), selection.top(),
                       selection.right(), selection.bottom() );
}

void KSpreadView::styleDialog()
{
  KSpreadStyleDlg dlg( this, doc()->styleManager() );
  dlg.exec();

  d->actions->selectStyle->setItems( doc()->styleManager()->styleNames() );
  if ( d->activeSheet )
  {
    d->activeSheet->setLayoutDirtyFlag();
    d->activeSheet->setRegionPaintDirty( d->activeSheet->visibleRect( d->canvas ) );
  }
  if ( d->canvas )
    d->canvas->repaint();
}

void KSpreadView::paperLayoutDlg()
{
  if ( d->canvas->editor() )
  {
    d->canvas->deleteEditor( true ); // save changes
  }
  KSpreadSheetPrint* print = d->activeSheet->print();

  KoPageLayout pl;
  pl.format = print->paperFormat();
  pl.orientation = print->orientation();

  pl.ptWidth =  MM_TO_POINT( print->paperWidth() );
  pl.ptHeight = MM_TO_POINT( print->paperHeight() );
  pl.ptLeft =   MM_TO_POINT( print->leftBorder() );
  pl.ptRight =  MM_TO_POINT( print->rightBorder() );
  pl.ptTop =    MM_TO_POINT( print->topBorder() );
  pl.ptBottom = MM_TO_POINT( print->bottomBorder() );

  KoHeadFoot hf;
  hf.headLeft  = print->localizeHeadFootLine( print->headLeft()  );
  hf.headRight = print->localizeHeadFootLine( print->headRight() );
  hf.headMid   = print->localizeHeadFootLine( print->headMid()   );
  hf.footLeft  = print->localizeHeadFootLine( print->footLeft()  );
  hf.footRight = print->localizeHeadFootLine( print->footRight() );
  hf.footMid   = print->localizeHeadFootLine( print->footMid()   );

  KoUnit::Unit unit = doc()->unit();

  KSpreadPaperLayout * dlg
    = new KSpreadPaperLayout( this, "PageLayout", pl, hf,
                              FORMAT_AND_BORDERS | HEADER_AND_FOOTER,
                              unit, d->activeSheet, this );
  dlg->show();
  // dlg destroys itself
}

void KSpreadView::definePrintRange()
{
  d->activeSheet->print()->definePrintRange( selectionInfo() );
}

void KSpreadView::resetPrintRange()
{
  d->activeSheet->print()->resetPrintRange();
}

void KSpreadView::wrapText( bool b )
{
  if ( d->toolbarLock )
    return;

  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->setSelectionMultiRow( selectionInfo(), b );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void KSpreadView::alignLeft( bool b )
{
  if ( d->toolbarLock )
    return;

  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( !b )
      d->activeSheet->setSelectionAlign( selectionInfo(),
                                   KSpreadFormat::Undefined );
    else
      d->activeSheet->setSelectionAlign( selectionInfo(),
                                   KSpreadFormat::Left );
    endOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::alignRight( bool b )
{
  if ( d->toolbarLock )
    return;

  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( !b )
      d->activeSheet->setSelectionAlign( selectionInfo(), KSpreadFormat::Undefined );
    else
      d->activeSheet->setSelectionAlign( selectionInfo(), KSpreadFormat::Right );

    endOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::alignCenter( bool b )
{
  if ( d->toolbarLock )
    return;

  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( !b )
      d->activeSheet->setSelectionAlign( selectionInfo(), KSpreadFormat::Undefined );
    else
      d->activeSheet->setSelectionAlign( selectionInfo(), KSpreadFormat::Center );

    endOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::alignTop( bool b )
{
  if ( d->toolbarLock )
    return;

  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( !b )
      d->activeSheet->setSelectionAlignY( selectionInfo(), KSpreadFormat::UndefinedY );
    else
      d->activeSheet->setSelectionAlignY( selectionInfo(), KSpreadFormat::Top );

    endOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::alignBottom( bool b )
{
  if ( d->toolbarLock )
    return;

  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( !b )
      d->activeSheet->setSelectionAlignY( selectionInfo(), KSpreadFormat::UndefinedY );
    else
      d->activeSheet->setSelectionAlignY( selectionInfo(), KSpreadFormat::Bottom );

    endOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::alignMiddle( bool b )
{
  if ( d->toolbarLock )
    return;

  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( !b )
      d->activeSheet->setSelectionAlignY( selectionInfo(), KSpreadFormat::UndefinedY );
    else
      d->activeSheet->setSelectionAlignY( selectionInfo(), KSpreadFormat::Middle );

    endOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::moneyFormat(bool b)
{
  if ( d->toolbarLock )
    return;

  doc()->emitBeginOperation( false );
  if ( d->activeSheet != 0L )
    d->activeSheet->setSelectionMoneyFormat( selectionInfo(), b );
  updateEditWidget();
  endOperation( selectionInfo()->selection() );
}

void KSpreadView::createStyleFromCell()
{
  if ( !d->activeSheet )
    return;

  QPoint p( d->selectionInfo->selection().topLeft() );
  KSpreadCell * cell = d->activeSheet->nonDefaultCell( p.x(), p.y() );

  bool ok = false;
  QString styleName( "" );

  while( true )
  {
    styleName = KInputDialog::getText( i18n( "Create Style From Cell" ),
                                       i18n( "Enter name:" ), styleName, &ok, this );

    if ( !ok ) // User pushed an OK button.
      return;

    styleName = styleName.stripWhiteSpace();

    if ( styleName.length() < 1 )
    {
      KNotifyClient::beep();
      KMessageBox::sorry( this, i18n( "The style name cannot be empty." ) );
      continue;
    }

    if ( doc()->styleManager()->style( styleName ) != 0 )
    {
      KNotifyClient::beep();
      KMessageBox::sorry( this, i18n( "A style with this name already exists." ) );
      continue;
    }
    break;
  }

  KSpreadCustomStyle * style = new KSpreadCustomStyle( cell->kspreadStyle(), styleName );

  doc()->styleManager()->m_styles[ styleName ] = style;
  cell->setKSpreadStyle( style );
  QStringList lst( d->actions->selectStyle->items() );
  lst.push_back( styleName );
  d->actions->selectStyle->setItems( lst );
}

void KSpreadView::styleSelected( const QString & style )
{
  if (d->activeSheet )
  {
    KSpreadStyle * s = doc()->styleManager()->style( style );

    if ( s )
    {
      doc()->emitBeginOperation(false);
      d->activeSheet->setSelectionStyle( selectionInfo(), s );
      endOperation( selectionInfo()->selection() );
    }
  }
}

void KSpreadView::precisionPlus()
{
  setSelectionPrecision( 1 );
}

void KSpreadView::precisionMinus()
{
  setSelectionPrecision( -1 );
}

void KSpreadView::setSelectionPrecision( int delta )
{
  if ( d->activeSheet != NULL )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->setSelectionPrecision( selectionInfo(), delta );
    endOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::percent( bool b )
{
  if ( d->toolbarLock )
    return;

  doc()->emitBeginOperation( false );
  if ( d->activeSheet != 0L )
    d->activeSheet->setSelectionPercent( selectionInfo() ,b );
  updateEditWidget();

  endOperation( selectionInfo()->selection() );
}

void KSpreadView::insertObject()
{
  doc()->emitBeginOperation( false );
  KoDocumentEntry e =  d->actions->insertPart->documentEntry();//KoPartSelectDia::selectPart( d->canvas );
  if ( e.isEmpty() )
  {
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
    return;
  }

  //Don't start handles more than once
  delete d->insertHandler;

  d->insertHandler = new KSpreadInsertHandler( this, d->canvas, e );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::insertChart()
{
  if ( util_isColumnSelected(selection()) || util_isRowSelected(selection()) )
  {
    KMessageBox::error( this, i18n("Area too large."));
    return;
  }
  QValueList<KoDocumentEntry> vec = KoDocumentEntry::query( true, "'KOfficeChart' in ServiceTypes" );
  if ( vec.isEmpty() )
  {
    KMessageBox::error( this, i18n("No charting component registered.") );
    return;
  }

  //Don't start handles more than once
  delete d->insertHandler;

  doc()->emitBeginOperation( false );

  d->insertHandler = new KSpreadInsertHandler( this, d->canvas, vec[0], true );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}


/*
  // TODO Use KoView setScaling/xScaling/yScaling instead
void KSpreadView::zoomMinus()
{
  if ( m_fZoom <= 0.25 )
    return;

  m_fZoom -= 0.25;

  if ( d->activeSheet != 0L )
    d->activeSheet->setLayoutDirtyFlag();

  d->canvas->repaint();
  d->vBorderWidget->repaint();
  d->hBorderWidget->repaint();
}

void KSpreadView::zoomPlus()
{
  if ( m_fZoom >= 3 )
    return;

  m_fZoom += 0.25;

  if ( d->activeSheet != 0L )
    d->activeSheet->setLayoutDirtyFlag();

  d->canvas->repaint();
  d->vBorderWidget->repaint();
  d->hBorderWidget->repaint();
}
*/

void KSpreadView::removeSheet()
{
  if ( doc()->map()->count() <= 1 || ( doc()->map()->visibleSheets().count() <= 1 ) )
  {
    KNotifyClient::beep();
    KMessageBox::sorry( this, i18n("You cannot delete the only sheet."), i18n("Remove Sheet") ); // FIXME bad english? no english!
    return;
  }
  KNotifyClient::beep();
  int ret = KMessageBox::warningContinueCancel( this, i18n( "You are about to remove the active sheet.\nDo you want to continue?" ),
                                       i18n( "Remove Sheet" ),KGuiItem(i18n("&Delete"),"editdelete") );

  if ( ret == KMessageBox::Continue )
  {
    doc()->emitBeginOperation( false );
    if ( d->canvas->editor() )
    {
      d->canvas->deleteEditor( false );
    }
    doc()->setModified( true );
    KSpreadSheet * tbl = activeSheet();
    KCommand* command = new RemoveSheetCommand( tbl );
    doc()->addCommand( command );
    command->execute();


#if 0
    KSpreadUndoRemoveSheet * undo = new KSpreadUndoRemoveSheet( doc(), tbl );
    doc()->addCommand( undo );
    tbl->doc()->map()->takeSheet( tbl );
    doc()->takeSheet( tbl );
#endif
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}


void KSpreadView::slotRename()
{

  KSpreadSheet * sheet = activeSheet();

  if( sheet->isProtected() )
  {
      KMessageBox::error( 0, i18n ( "You cannot change a protected sheet." ) );
      return;
  }

  bool ok;
  QString activeName = sheet->sheetName();
  QString newName = KInputDialog::getText( i18n("Rename Sheet"),i18n("Enter name:"), activeName, &ok, this );

  if( !ok ) return;

  while (!util_validateSheetName(newName))
  {
    KNotifyClient::beep();
    KMessageBox::information( this, i18n("Sheet name contains illegal characters. Only numbers and letters are allowed."),
      i18n("Change Sheet Name") );

    newName = newName.simplifyWhiteSpace();
    int n = newName.find('-');
    if ( n > -1 ) newName[n] = '_';
    n = newName.find('!');
    if ( n > -1 ) newName[n] = '_';
    n = newName.find('$');
    if ( n > -1 ) newName[n] = '_';

    newName = KInputDialog::getText( i18n("Rename Sheet"),i18n("Enter name:"), newName, &ok, this );

    if ( !ok ) return;
  }

  if ( (newName.stripWhiteSpace()).isEmpty() ) // Sheet name is empty.
  {
    KNotifyClient::beep();
    KMessageBox::information( this, i18n("Sheet name cannot be empty."), i18n("Change Sheet Name") );
    // Recursion
    slotRename();
  }
  else if ( newName != activeName ) // Sheet name changed.
  {
    // Is the name already used
    if ( doc()->map()->findSheet( newName ) )
    {
      KNotifyClient::beep();
      KMessageBox::information( this, i18n("This name is already used."), i18n("Change Sheet Name") );
      // Recursion
      slotRename();
      return;
    }

    KCommand* command = new RenameSheetCommand( sheet, newName );
    doc()->addCommand( command );
    command->execute();

    //sheet->setSheetName( newName );

    doc()->emitBeginOperation(false);
    updateEditWidget();
    doc()->setModified( true );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void KSpreadView::setText (const QString & _text, bool array)
{
  if ( d->activeSheet == 0L )
    return;

  if (array) {
    // array version
    QRect r = d->canvas->selection();
    int x = r.left();
    int y = r.top();
    int xl = r.right() - r.left() + 1;
    int yl = r.bottom() - r.top() + 1;
    doc()->emitBeginOperation( false );
    d->activeSheet->setArrayFormula (y, x, yl, xl, _text);
    doc()->emitEndOperation( QRect( x, y, xl, yl ) );
  }
  else
  {
    // non-array version
    int x = d->canvas->markerColumn();
    int y = d->canvas->markerRow();

    doc()->emitBeginOperation( false );
    d->activeSheet->setText( y, x, _text );
    KSpreadCell * cell = d->activeSheet->cellAt( x, y );

    if ( cell->value().isString() && !_text.isEmpty() && !_text.at(0).isDigit() && !cell->isFormula() )
      doc()->addStringCompletion( _text );

    doc()->emitEndOperation( QRect( x, y, 1, 1 ) );
  }
}

//------------------------------------------------
//
// Document signals
//
//------------------------------------------------

void KSpreadView::slotAddSheet( KSpreadSheet *_sheet )
{
  addSheet( _sheet );
}

void KSpreadView::slotRefreshView()
{
  refreshView();
  d->canvas->repaint();
  d->vBorderWidget->repaint();
  d->hBorderWidget->repaint();
}

void KSpreadView::slotUpdateView( KSpreadSheet *_sheet )
{
  // Do we display this sheet ?
  if ( _sheet != d->activeSheet )
    return;

  //  doc()->emitBeginOperation( false );

  //  d->activeSheet->setRegionPaintDirty(QRect(QPoint(0,0),
  //                                      QPoint(KS_colMax, KS_rowMax)));

  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::slotUpdateView( KSpreadSheet * _sheet, const QRect & _rect )
{
  // qDebug("void KSpreadView::slotUpdateView( KSpreadSheet *_sheet, const QRect& %i %i|%i %i )\n",_rect.left(),_rect.top(),_rect.right(),_rect.bottom());

  // Do we display this sheet ?
  if ( _sheet != d->activeSheet )
    return;

  // doc()->emitBeginOperation( false );
  d->activeSheet->setRegionPaintDirty( _rect );
  endOperation( _rect );
}

void KSpreadView::slotUpdateHBorder( KSpreadSheet * _sheet )
{
  // kdDebug(36001)<<"void KSpreadView::slotUpdateHBorder( KSpreadSheet *_sheet )\n";

  // Do we display this sheet ?
  if ( _sheet != d->activeSheet )
    return;

  doc()->emitBeginOperation(false);
  d->hBorderWidget->update();
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::slotUpdateVBorder( KSpreadSheet *_sheet )
{
  // kdDebug("void KSpreadView::slotUpdateVBorder( KSpreadSheet *_sheet )\n";

  // Do we display this sheet ?
  if ( _sheet != d->activeSheet )
    return;

  doc()->emitBeginOperation( false );
  d->vBorderWidget->update();
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::slotChangeSelection( KSpreadSheet *_sheet,
                                       const QRect &oldSelection,
                                       const QPoint& /* oldMarker*/ )
{
  doc()->emitBeginOperation( false );
  QRect newSelection = d->selectionInfo->selection();

  // Emit a signal for internal use
  emit sig_selectionChanged( _sheet, newSelection );

  // Empty selection ?
  // Activate or deactivate some actions.
  bool colSelected = util_isColumnSelected( selection() );
  bool rowSelected = util_isRowSelected( selection() );

  if ( d->activeSheet && !d->activeSheet->isProtected() )
  {
    d->actions->resizeRow->setEnabled( !colSelected );
    d->actions->equalizeRow->setEnabled( !colSelected );
    d->actions->validity->setEnabled( !colSelected && !rowSelected);
    d->actions->conditional->setEnabled( !colSelected && !rowSelected);
    d->actions->resizeColumn->setEnabled( !rowSelected );
    d->actions->equalizeColumn->setEnabled( !rowSelected );
    d->actions->textToColumns->setEnabled( !rowSelected );

    bool simpleSelection = d->selectionInfo->singleCellSelection()
      || colSelected || rowSelected;
    d->actions->autoFormat->setEnabled( !simpleSelection );
    d->actions->sort->setEnabled( !simpleSelection );
    d->actions->mergeCell->setEnabled( !simpleSelection );
    d->actions->fillRight->setEnabled( !simpleSelection );
    d->actions->fillUp->setEnabled( !simpleSelection );
    d->actions->fillDown->setEnabled( !simpleSelection );
    d->actions->fillLeft->setEnabled( !simpleSelection );
    d->actions->insertChartFrame->setEnabled( !simpleSelection );
    d->actions->sortDec->setEnabled( !simpleSelection );
    d->actions->sortInc->setEnabled( !simpleSelection);
    d->actions->createStyle->setEnabled( simpleSelection ); // just from one cell
  }
  d->actions->selectStyle->setCurrentItem( -1 );
  resultOfCalc();
  // Send some event around. This is read for example
  // by the calculator plugin.
  KSpreadSelectionChanged ev( newSelection, activeSheet()->name() );
  QApplication::sendEvent( this, &ev );

  // Do we display this sheet ?
  if ( _sheet != d->activeSheet )
  {
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
    return;
  }

  d->canvas->setSelectionChangePaintDirty( d->activeSheet, oldSelection, newSelection );

  d->vBorderWidget->update();
  d->hBorderWidget->update();
  doc()->emitEndOperation( newSelection );
}

void KSpreadView::resultOfCalc()
{
  KSpreadSheet * sheet = activeSheet();
  ValueCalc* calc = d->doc->calc();
  KSpreadValue val;
  QRect tmpRect(d->selectionInfo->selection());
  MethodOfCalc tmpMethod = doc()->getTypeOfCalc() ;
  if ( tmpMethod != NoneCalc )
  {

    KSpreadValue range = sheet->valueRange (tmpRect.left(), tmpRect.top(),
        tmpRect.right(), tmpRect.bottom());
    switch (tmpMethod)
    {
      case SumOfNumber:
        val = calc->sum (range);
      break;
      case Average:
        val = calc->avg (range);
      break;
      case Min:
        val = calc->min (range);
      break;
      case Max:
        val = calc->max (range);
      break;
      case CountA:
        val = KSpreadValue (calc->count (range));
        break;
      case Count:
        val = KSpreadValue (calc->count (range, false));
      case NoneCalc:
      break;
      default:
      break;
    }

  }

  QString res = d->doc->converter()->asString (val).asString ();
  QString tmp;
  switch(tmpMethod )
  {
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

  //doc()->emitBeginOperation();
  if ( d->calcLabel )
    d->calcLabel->setText(QString(" ") + tmp + ' ');
  //doc()->emitEndOperation();
}

void KSpreadView::statusBarClicked(int _id)
{
  if ( !koDocument()->isReadWrite() || !factory() )
    return;
  if ( _id == 0 ) //menu calc
  {
    QPoint mousepos = QCursor::pos();
    ((QPopupMenu*)factory()->container( "calc_popup" , this ) )->popup( mousepos );
  }
}

void KSpreadView::menuCalc( bool )
{
  doc()->emitBeginOperation(false);
  if ( d->actions->calcMin->isChecked() )
  {
    doc()->setTypeOfCalc( Min );
  }
  else if ( d->actions->calcMax->isChecked() )
  {
    doc()->setTypeOfCalc( Max );
  }
  else if ( d->actions->calcCount->isChecked() )
  {
    doc()->setTypeOfCalc( Count );
  }
  else if ( d->actions->calcAverage->isChecked() )
  {
    doc()->setTypeOfCalc( Average );
  }
  else if ( d->actions->calcSum->isChecked() )
  {
    doc()->setTypeOfCalc( SumOfNumber );
  }
  else if ( d->actions->calcCountA->isChecked() )
  {
    doc()->setTypeOfCalc( CountA );
  }
  else if ( d->actions->calcNone->isChecked() )
    doc()->setTypeOfCalc( NoneCalc );

  resultOfCalc();

  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}


QWMatrix KSpreadView::matrix() const
{
  QWMatrix m;
  m.scale( d->doc->zoomedResolutionX(),
           d->doc->zoomedResolutionY() );
  m.translate( - d->canvas->xOffset(), - d->canvas->yOffset() );
  return m;
}

void KSpreadView::transformPart()
{
    Q_ASSERT( selectedChild() );

    if ( d->transformToolBox.isNull() )
    {
        d->transformToolBox = new KoTransformToolBox( selectedChild(), topLevelWidget() );
        d->transformToolBox->show();

        d->transformToolBox->setDocumentChild( selectedChild() );
    }
    else
    {
        d->transformToolBox->show();
        d->transformToolBox->raise();
    }
}

void KSpreadView::slotChildSelected( KoDocumentChild* ch )
{
  if ( d->activeSheet && !d->activeSheet->isProtected() )
  {
    d->actions->transform->setEnabled( true );

    if ( !d->transformToolBox.isNull() )
    {
        d->transformToolBox->setEnabled( true );
        d->transformToolBox->setDocumentChild( ch );
    }
  }

  doc()->emitBeginOperation( false );
  d->activeSheet->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));
  doc()->emitEndOperation();
  paintUpdates();
}

void KSpreadView::slotChildUnselected( KoDocumentChild* )
{
  if ( d->activeSheet && !d->activeSheet->isProtected() )
  {
    d->actions->transform->setEnabled( false );

    if ( !d->transformToolBox.isNull() )
    {
        d->transformToolBox->setEnabled( false );
    }
    deleteEditor( true );
  }

  doc()->emitBeginOperation( false );
  d->activeSheet->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));
  doc()->emitEndOperation();
  paintUpdates();
}


void KSpreadView::deleteEditor( bool saveChanges )
{
    doc()->emitBeginOperation( false );
    d->canvas->deleteEditor( saveChanges );
    doc()->emitEndOperation( selectionInfo()->selection() );
}

DCOPObject * KSpreadView::dcopObject()
{
  if ( !d->dcop )
    d->dcop = new KSpreadViewIface( this );

  return d->dcop;
}

QWidget * KSpreadView::canvas()
{
  return canvasWidget();
}

int KSpreadView::canvasXOffset() const
{
  return int( canvasWidget()->xOffset() );
}

int KSpreadView::canvasYOffset() const
{
  return int( canvasWidget()->yOffset() );
}


void KSpreadView::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
  if ( d->activeSheet )
  {
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );

    if ( ev->activated() )
    {
      if ( d->calcLabel )
        resultOfCalc();
    }
    else
    {
      /*if (d->calcLabel)
        {
        disconnect(d->calcLabel,SIGNAL(pressed( int )),this,SLOT(statusBarClicked(int)));
        }*/
    }
  }

  KoView::guiActivateEvent( ev );
}

void KSpreadView::popupTabBarMenu( const QPoint & _point )
{
  if ( !koDocument()->isReadWrite() || !factory() )
    return;
  if ( d->tabBar )
  {
    bool state = ( doc()->map()->visibleSheets().count() > 1 );
    if ( d->activeSheet && d->activeSheet->isProtected() )
    {
      d->actions->removeSheet->setEnabled( false );
      d->actions->hideSheet->setEnabled( false );
      d->actions->showSheet->setEnabled( false );
    }
    else
    {
      d->actions->removeSheet->setEnabled( state);
      d->actions->hideSheet->setEnabled( state );
      d->actions->showSheet->setEnabled( doc()->map()->hiddenSheets().count()>0 );
    }
    if ( !doc() || !doc()->map() || doc()->map()->isProtected() )
    {
      d->actions->insertSheet->setEnabled( false );
      d->actions->renameSheet->setEnabled( false );
      d->actions->showSheet->setEnabled( false );
      d->actions->hideSheet->setEnabled( false );
      d->actions->removeSheet->setEnabled( false );
    }
    static_cast<QPopupMenu*>(factory()->container("menupage_popup",this))->popup(_point);
  }
}

void KSpreadView::updateBorderButton()
{
  //  doc()->emitBeginOperation( false );
  if ( d->activeSheet )
    d->actions->showPageBorders->setChecked( d->activeSheet->isShowPageBorders() );
  //  doc()->emitEndOperation();
}

void KSpreadView::removeSheet( KSpreadSheet *_t )
{
  doc()->emitBeginOperation(false);
  QString m_tablName=_t->sheetName();
  d->tabBar->removeTab( m_tablName );
  setActiveSheet( doc()->map()->findSheet( doc()->map()->visibleSheets().first() ));

  bool state = doc()->map()->visibleSheets().count() > 1;
  d->actions->removeSheet->setEnabled( state );
  d->actions->hideSheet->setEnabled( state );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::insertSheet( KSpreadSheet* sheet )
{
  doc()->emitBeginOperation( false );
  QString tabName = sheet->sheetName();
  if ( !sheet->isHidden() )
  {
    d->tabBar->addTab( tabName );
  }

  bool state = ( doc()->map()->visibleSheets().count() > 1 );
  d->actions->removeSheet->setEnabled( state );
  d->actions->hideSheet->setEnabled( state );
  doc()->emitEndOperation( sheet->visibleRect( d->canvas ) );
}

QColor KSpreadView::borderColor() const
{
  return d->actions->borderColor->color();
}

void KSpreadView::updateShowSheetMenu()
{
  doc()->emitBeginOperation( false );
  if ( d->activeSheet->isProtected() )
    d->actions->showSheet->setEnabled( false );
  else
    d->actions->showSheet->setEnabled( doc()->map()->hiddenSheets().count() > 0 );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void KSpreadView::closeEditor()
{
  if ( d->activeSheet ) { // #45822
    doc()->emitBeginOperation( false );
    d->canvas->closeEditor();
    doc()->emitEndOperation( selectionInfo()->selection() );
  }
}


void KSpreadView::paintUpdates()
{
  /* don't do any begin/end operation here -- this is what is called at an
     endOperation
  */
  d->canvas->paintUpdates();
}

void KSpreadView::commandExecuted()
{
  updateEditWidget();
  resultOfCalc();
}

void KSpreadView::initialiseMarkerFromSheet( KSpreadSheet *_sheet, const QPoint &point )
{
    d->savedMarkers.replace( _sheet, point);
}

QPoint KSpreadView::markerFromSheet( KSpreadSheet *_sheet ) const
{
    QMapIterator<KSpreadSheet*, QPoint> it2 = d->savedMarkers.find(_sheet);
    QPoint newMarker = (it2 == d->savedMarkers.end()) ? QPoint(1,1) : *it2;
    return newMarker;
}

void KSpreadView::saveCurrentSheetSelection()
{
    /* save the current selection on this sheet */
    if (d->activeSheet != NULL)
    {
        d->savedAnchors.replace(d->activeSheet, selectionInfo()->selectionAnchor());
        kdDebug() << " Current scrollbar vert value: " << d->canvas->vertScrollBar()->value() << endl;
        kdDebug() << "Saving marker pos: " << selectionInfo()->marker() << endl;
        d->savedMarkers.replace(d->activeSheet, selectionInfo()->marker());
    }
}

void KSpreadView::handleDamages( const QValueList<Damage*>& damages )
{
    QValueList<Damage*>::ConstIterator it;
    for( it = damages.begin(); it != damages.end(); ++it )
    {
        Damage* damage = *it;
        if( !damage ) continue;

        if( damage->type() == Damage::Cell )
        {
            CellDamage* cd = static_cast<CellDamage*>( damage );
            KSpreadCell* damagedCell = cd->cell();
            KSpreadSheet* damagedSheet = damagedCell->sheet();
            QRect drect( damagedCell->column(), damagedCell->row(), 1, 1 );
            damagedSheet->setRegionPaintDirty( drect );
            paintUpdates();
        }

        if( damage->type() == Damage::Sheet )
        {
            SheetDamage* sd = static_cast<SheetDamage*>( damage );
            KSpreadSheet* damagedSheet = sd->sheet();

            if( sd->action() == SheetDamage::PropertiesChanged )
            {
                CellBinding  * b = 0;
                for ( b = damagedSheet->firstCellBinding(); b != 0;
                    b = damagedSheet->nextCellBinding() )
                        b->cellChanged( 0 );

                d->activeSheet->setRegionPaintDirty( QRect(QPoint(0,0),
                    QPoint(KS_colMax, KS_rowMax)));

                paintUpdates();
                refreshView();
            }

        }

    }
}

void KSpreadView::runInternalTests()
{
    // run various tests, only for developers
    KSpread::TestRunner* runner = new KSpread::TestRunner();
    runner->exec();
    delete runner;
}

void KSpreadView::runInspector()
{
    // useful to inspect objects
    if(!d->activeSheet) return;
    KSpreadCell * cell = d->activeSheet->cellAt( d->selectionInfo->marker() );
    KSpread::Inspector* ins = new KSpread::Inspector( cell );
    ins->exec();
    delete ins;
}



#include "kspread_view.moc"

