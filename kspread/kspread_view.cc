/* This file is part of the KDE project
   Copyright (C) 2005-2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
             (C) 2006      Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             (C) 2002-2005 Ariya Hidayat <ariya@kde.org>
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
#include <qbuffer.h>
#include <qbytearray.h>
#include <qclipboard.h>
#include <qcursor.h>
#include <qlayout.h>
#include <q3paintdevicemetrics.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <qsqldatabase.h>
#include <q3listview.h>
#include <qsizepolicy.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <Q3PopupMenu>
#include <QFrame>
#include <QKeyEvent>
#include <QEvent>
#include <Q3GridLayout>
#include <Q3ValueList>
#include <Q3PtrList>
#include <QPixmap>
#include <Q3HBoxLayout>

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
#include <kpassworddialog.h>
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
#include <k3listview.h>
#include <kpushbutton.h>
#include <kxmlguifactory.h>

// KOffice includes
#include <tkcoloractions.h>
#include <kdatatool.h>
#include <KoCharSelectDia.h>
#include <KoCommandHistory.h>
#include <KoMainWindow.h>
#include <KoOasisLoadingContext.h>
#include <KoOasisStore.h>
#include <KoOasisStyles.h>
#include <KoPartSelectAction.h>
#include <KoStoreDrag.h>
#include <KoTabBar.h>
#include <kspread_toolbox.h>
#include <KoTemplateCreateDia.h>
#include <KoZoomAction.h>
#include <ktoolinvocation.h>

// KSpread includes
#include "commands.h"
#include "damages.h"
#include "digest.h"
#include "inspector.h"
#include "kspread_canvas.h"
#include "kspread_editors.h"
#include "kspread_events.h"
#include "kspread_global.h"
#include "kspread_handler.h"
#include "kspread_locale.h"
#include "kspread_map.h"
#include "selection.h"
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
#include "kspread_propertyEditor.h"
#include "kspread_generalProperty.h"

// files generated by uic
// cmake places them in the current build dir
#include "SheetSelectWidget.h"

// KSpread DCOP
#include "KSpreadViewIface.h"

#include "kspread_view.h"

namespace KSpread
{
class ViewActions;

class View::Private
{
public:
    View* view;
    Doc* doc;
    DCOPObject* dcop;

    // the active sheet, may be 0
    // this is the sheet which has the input focus
    Sheet* activeSheet;

    // GUI elements
    QWidget *frame;
    QFrame *toolWidget;
    Canvas *canvas;
    VBorder *vBorderWidget;
    HBorder *hBorderWidget;
    QScrollBar *horzScrollBar;
    QScrollBar *vertScrollBar;
    KoTabBar *tabBar;
    KStatusBarLabel* calcLabel;

    // formulabar, consists of:
    Q3HBoxLayout* formulaBarLayout;
    ComboboxLocationEditWidget *posWidget;
    QAbstractButton* formulaButton;
    QAbstractButton *okButton;
    QAbstractButton *cancelButton;
    KSpread::EditWidget *editWidget;
    Q3GridLayout* viewLayout;
    Q3HBoxLayout* tabScrollBarLayout;

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
    Selection* selection;
    Selection* choice;
    QMap<Sheet*, QPoint> savedAnchors;
    QMap<Sheet*, QPoint> savedMarkers;

    // Find and Replace context. We remember the options and
    // the strings used previously.
    long findOptions;
    QStringList findStrings;
    QStringList replaceStrings;
    FindOption::searchTypeValue typeValue;
    FindOption::searchDirectionValue directionValue;
    // Current "find" operation
    KFind* find;
    KReplace* replace;
    int findLeftColumn;
    int findRightColumn;
    QPoint findPos;
    QPoint findEnd;

    InsertHandler* insertHandler;

    // Insert special character dialog
    KoCharSelectDia* specialCharDlg;

    // Holds a guarded pointer to the transformation toolbox.
    QPointer<KoTransformToolBox> transformToolBox;

    // the last popup menu (may be 0).
    // Since only one popup menu can be opened at once, its pointer is stored here.
    // Delete the old one before you store a pointer to anotheron here.
    Q3PopupMenu *popupMenu;
    int popupMenuFirstToolId;

    Q3PopupMenu *popupRow;
    Q3PopupMenu *popupColumn;
    Q3PopupMenu* popupChild;       // for embedded children
    Q3PopupMenu* popupListChoose;  // for list of choose

    // the child for which the popup menu has been opened.
    Child* popupChildObject;

    // spell-check context
    struct
    {
      KSpell *   kspell;
      Sheet *  firstSpellSheet;
      Sheet *  currentSpellSheet;
      Cell  *  currentCell;
      MacroUndoAction *macroCmdSpellCheck;
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
        Sheet * currentSheet;
        Sheet * firstSheet;
    } searchInSheets;

    // the tools
    struct ToolEntry
    {
      QString command;
      KDataToolInfo info;
    };
    Q3PtrList<ToolEntry> toolList;

    void initActions();
    void adjustActions( bool mode );
    void adjustActions( Sheet* sheet, Cell* cell );
    void adjustWorkbookActions( bool mode );
    void updateButton( Cell *cell, int column, int row);
    QAbstractButton* newIconButton( const char *_file, bool _kbutton = false, QWidget *_parent = 0L );

    PropertyEditor *m_propertyEditor;

    QTimer statusBarOpTimer;
};

class ViewActions
{
public:

    // cell formatting
    KAction* cellLayout;
    KAction *actionExtraProperties;
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
    KToolBarPopupAction* mergeCell;
    KAction* mergeCellHorizontal;
    KAction* mergeCellVertical;
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
    KToggleAction* insertChartFrame;
    KAction* insertPicture;
    KAction* customList;
    KAction* spellChecking;
    KAction* internalTests;
    KAction* inspector;

    // settings
    KoZoomAction* viewZoom;
    KToggleAction* showStatusBar;
    KToggleAction* showTabBar;
    KToggleAction* showFormulaBar;
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


void View::Private::initActions()
{
  actions = new ViewActions;

  KActionCollection* ac = view->actionCollection();

  // -- cell formatting actions --

  actions->cellLayout = new KAction( i18n("Cell Format..."), "cell_layout",
      Qt::CTRL+ Qt::ALT+ Qt::Key_F, view, SLOT( layoutDlg() ), ac, "cellLayout" );
  actions->cellLayout->setToolTip( i18n("Set the cell formatting") );

  actions->actionExtraProperties = new KAction( i18n( "&Properties" ), "penbrush", 0,
      view, SLOT( extraProperties() ), ac, "extra_properties" );

  actions->defaultFormat = new KAction( i18n("Default"),
      0, view, SLOT( defaultSelection() ), ac, "default" );
  actions->defaultFormat->setToolTip( i18n("Resets to the default format") );

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
      ac, "selectFont" );
  QObject::connect( actions->selectFont, SIGNAL( activated( const QString& ) ),
      view, SLOT( fontSelected( const QString& ) ) );

  actions->selectFontSize = new KFontSizeAction( i18n("Select Font Size"),
      ac, "selectFontSize" );
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

  QActionGroup* groupAlign = new QActionGroup( view );
  actions->alignLeft = new KToggleAction( "text_left", i18n("Align Left"),
      ac, "left", groupAlign );
  QObject::connect( actions->alignLeft, SIGNAL( toggled( bool ) ),
      view, SLOT( alignLeft( bool ) ) );
  actions->alignLeft->setToolTip(i18n("Left justify the cell contents"));

  actions->alignCenter = new KToggleAction( "text_center", i18n("Align Center"),
      ac, "center", groupAlign );
  QObject::connect( actions->alignCenter, SIGNAL( toggled( bool ) ),
      view, SLOT( alignCenter( bool ) ) );
  actions->alignCenter->setToolTip(i18n("Center the cell contents"));

  actions->alignRight = new KToggleAction( "text_right", i18n("Align Right"),
      ac, "right", groupAlign );
  QObject::connect( actions->alignRight, SIGNAL( toggled( bool ) ),
      view, SLOT( alignRight( bool ) ) );
  actions->alignRight->setToolTip(i18n("Right justify the cell contents"));

  QActionGroup* groupPos = new QActionGroup( view );
  actions->alignTop = new KToggleAction( "text_top", i18n("Align Top"),
      ac, "top", groupPos );
  QObject::connect( actions->alignTop, SIGNAL( toggled( bool ) ),
      view, SLOT( alignTop( bool ) ) );
  actions->alignTop->setToolTip(i18n("Align cell contents along the top of the cell"));

  actions->alignMiddle = new KToggleAction( "middle", i18n("Align Middle"),
      ac, "middle", groupPos );
  QObject::connect( actions->alignMiddle, SIGNAL( toggled( bool ) ),
      view, SLOT( alignMiddle( bool ) ) );
  actions->alignMiddle->setToolTip(i18n("Align cell contents centered in the cell"));

  actions->alignBottom = new KToggleAction( "text_bottom", i18n("Align Bottom"),
      ac, "bottom", groupPos );
  QObject::connect( actions->alignBottom, SIGNAL( toggled( bool ) ),
      view, SLOT( alignBottom( bool ) ) );
  actions->alignBottom->setToolTip(i18n("Align cell contents along the bottom of the cell"));

  actions->wrapText = new KToggleAction( i18n("Wrap Text"), "multirow",
      0, ac, "multiRow" );
  QObject::connect( actions->wrapText, SIGNAL( toggled( bool ) ),
      view, SLOT( wrapText( bool ) ) );
  actions->wrapText->setToolTip(i18n("Make the cell text wrap onto multiple lines"));

  actions->verticalText = new KToggleAction( i18n("Vertical Text"),"vertical_text" ,
      0 ,ac, "verticaltext" );
  QObject::connect( actions->verticalText, SIGNAL( toggled( bool ) ),
      view, SLOT( verticalText( bool ) ) );
  actions->verticalText->setToolTip(i18n("Print cell contents vertically"));

  actions->increaseIndent = new KAction( i18n("Increase Indent"),
      QApplication::isRightToLeft() ? "format_decreaseindent":"format_increaseindent",
      0, view, SLOT( increaseIndent() ), ac, "increaseindent" );
  actions->increaseIndent->setToolTip(i18n("Increase the indentation"));

  actions->decreaseIndent = new KAction( i18n("Decrease Indent"),
      QApplication::isRightToLeft() ? "format_increaseindent" : "format_decreaseindent",
      0, view, SLOT( decreaseIndent() ), ac, "decreaseindent");
  actions->decreaseIndent->setToolTip(i18n("Decrease the indentation"));

  actions->changeAngle = new KAction( i18n("Change Angle..."),
      0, view, SLOT( changeAngle() ), ac, "changeangle" );
  actions->changeAngle->setToolTip(i18n("Change the angle that cell contents are printed"));

  actions->percent = new KToggleAction( i18n("Percent Format"), "percent",
      0, ac, "percent");
  QObject::connect( actions->percent, SIGNAL( toggled( bool ) ),
      view, SLOT( percent( bool ) ) );
  actions->percent->setToolTip(i18n("Set the cell formatting to look like a percentage"));

  actions->precplus = new KAction( i18n("Increase Precision"), "prec_plus",
      0, view, SLOT( precisionPlus() ), ac, "precplus");
  actions->precplus->setToolTip(i18n("Increase the decimal precision shown onscreen"));

  actions->precminus = new KAction( i18n("Decrease Precision"), "prec_minus",
      0, view, SLOT( precisionMinus() ), ac, "precminus");
  actions->precminus->setToolTip(i18n("Decrease the decimal precision shown onscreen"));

  actions->money = new KToggleAction( i18n("Money Format"), "money",
      0, ac, "money");
  QObject::connect( actions->money, SIGNAL( toggled( bool ) ),
      view, SLOT( moneyFormat( bool ) ) );
  actions->money->setToolTip(i18n("Set the cell formatting to look like your local currency"));

  actions->upper = new KAction( i18n("Upper Case"), "fontsizeup",
      0, view, SLOT( upper() ), ac, "upper" );
  actions->upper->setToolTip(i18n("Convert all letters to upper case"));

  actions->lower = new KAction( i18n("Lower Case"), "fontsizedown",
      0, view, SLOT( lower() ), ac, "lower" );
  actions->lower->setToolTip(i18n("Convert all letters to lower case"));

  actions->firstLetterUpper = new KAction( i18n("Convert First Letter to Upper Case"), "first_letter_upper",
      0, view, SLOT( firstLetterUpper() ),ac, "firstletterupper" );
  actions->firstLetterUpper->setToolTip(i18n("Capitalize the first letter"));

  actions->bgColor = new TKSelectColorAction( i18n("Background Color"),
      TKSelectColorAction::FillColor, ac, "backgroundColor", true );
  QObject::connect(actions->bgColor, SIGNAL( activated() ),
      view, SLOT( changeBackgroundColor() ) );
  actions->bgColor->setDefaultColor(QColor());
  actions->bgColor->setToolTip(i18n("Set the background color"));

  actions->borderLeft = new KAction( i18n("Border Left"), "border_left",
      0, view, SLOT( borderLeft() ), ac, "borderLeft" );
  actions->borderLeft->setToolTip(i18n("Set a left border to the selected area"));

  actions->borderRight = new KAction( i18n("Border Right"), "border_right",
      0, view, SLOT( borderRight() ), ac, "borderRight" );
  actions->borderRight->setToolTip(i18n("Set a right border to the selected area"));

  actions->borderTop = new KAction( i18n("Border Top"), "border_top",
      0, view, SLOT( borderTop() ), ac, "borderTop" );
  actions->borderTop->setToolTip(i18n("Set a top border to the selected area"));

  actions->borderBottom = new KAction( i18n("Border Bottom"), "border_bottom",
      0, view, SLOT( borderBottom() ), ac, "borderBottom" );
  actions->borderBottom->setToolTip(i18n("Set a bottom border to the selected area"));

  actions->borderAll = new KAction( i18n("All Borders"), "border_all",
      0, view, SLOT( borderAll() ), ac, "borderAll" );
  actions->borderAll->setToolTip(i18n("Set a border around all cells in the selected area"));

  actions->borderRemove = new KAction( i18n("Remove Borders"), "border_remove",
      0, view, SLOT( borderRemove() ), ac, "borderRemove" );
  actions->borderRemove->setToolTip(i18n("Remove all borders in the selected area"));

  actions->borderOutline = new KAction( i18n("Border Outline"), ("border_outline"),
      0, view, SLOT( borderOutline() ), ac, "borderOutline" );
  actions->borderOutline->setToolTip(i18n("Set a border to the outline of the selected area"));

  actions->borderColor = new TKSelectColorAction( i18n("Border Color"),
      TKSelectColorAction::LineColor, ac, "borderColor" );
  QObject::connect( actions->borderColor, SIGNAL( activated() ),
      view, SLOT( changeBorderColor() ) );
  actions->borderColor->setToolTip( i18n( "Select a new border color" ) );

  actions->selectStyle = new KSelectAction( i18n( "St&yle" ),
      ac, "stylemenu" );
  actions->selectStyle->setToolTip( i18n( "Apply a predefined style to the selected cells" ) );
  QObject::connect( actions->selectStyle, SIGNAL( activated( const QString & ) ),
      view, SLOT( styleSelected( const QString & ) ) );

  actions->createStyle = new KAction( i18n( "Create Style From Cell..." ),
      0, view, SLOT( createStyleFromCell()), ac, "createStyle" );
  actions->createStyle->setToolTip( i18n( "Create a new style based on the currently selected cell" ) );

  // -- cell operation actions --

  actions->editCell = new KAction( i18n("Modify Cell"),"cell_edit",
      Qt::CTRL+Qt::Key_M, view, SLOT( editCell() ), ac, "editCell" );
  actions->editCell->setToolTip(i18n("Edit the highlighted cell"));

  actions->insertCell = new KAction( i18n("Insert Cells..."), "insertcell",
      0, view, SLOT( slotInsert() ), ac, "insertCell" );
  actions->insertCell->setToolTip(i18n("Insert a blank cell into the spreadsheet"));

  actions->removeCell = new KAction( i18n("Remove Cells..."), "removecell",
      0, view, SLOT( slotRemove() ), ac, "removeCell" );
  actions->removeCell->setToolTip(i18n("Removes the current cell from the spreadsheet"));

  actions->deleteCell = new KAction( i18n("Delete"), "deletecell",
      0, view, SLOT( deleteSelection() ), ac, "delete" );
  actions->deleteCell->setToolTip(i18n("Delete all contents and formatting of the current cell"));

  actions->mergeCell = new KToolBarPopupAction( i18n("Merge Cells"),"mergecell",
      0, view, SLOT( mergeCell() ), ac, "mergecell" );
  actions->mergeCell->setToolTip(i18n("Merge the selected region"));
  actions->mergeCell->plug( actions->mergeCell->popupMenu() );

  actions->mergeCellHorizontal = new KAction( i18n("Merge Cells Horizontally"),"mergecell-horizontal",
      0, view, SLOT( mergeCellHorizontal() ), ac, "mergecellHorizontal" );
  actions->mergeCellHorizontal->setToolTip(i18n("Merge the selected region horizontally"));
  actions->mergeCellHorizontal->plug( actions->mergeCell->popupMenu() );

  actions->mergeCellVertical = new KAction( i18n("Merge Cells Vertically"),"mergecell-vertical",
      0, view, SLOT( mergeCellVertical() ), ac, "mergecellVertical" );
  actions->mergeCellVertical->setToolTip(i18n("Merge the selected region vertically"));
  actions->mergeCellVertical->plug( actions->mergeCell->popupMenu() );

  actions->dissociateCell = new KAction( i18n("Dissociate Cells"),"dissociatecell",
      0, view, SLOT( dissociateCell() ), ac, "dissociatecell" );
  actions->dissociateCell->setToolTip(i18n("Unmerge the selected region"));

  actions->clearText = new KAction( i18n("Text"),
      0, view, SLOT( clearTextSelection() ), ac, "cleartext" );
  actions->clearText->setToolTip(i18n("Remove the contents of the current cell"));

  actions->conditional = new KAction( i18n("Conditional Cell Attributes..."),
      0, view, SLOT( conditional() ), ac, "conditional" );
  actions->conditional->setToolTip(i18n("Set cell format based on certain conditions"));


  actions->clearConditional = new KAction( i18n("Conditional Cell Attributes"),
      0, view, SLOT( clearConditionalSelection() ), ac, "clearconditional" );
  actions->clearConditional->setToolTip(i18n("Remove the conditional cell formatting"));

  actions->validity = new KAction( i18n("Validity..."),
      0, view, SLOT( validity() ), ac, "validity" );
  actions->validity->setToolTip(i18n("Set tests to confirm cell data is valid"));

  actions->clearValidity = new KAction( i18n("Validity"),
      0, view, SLOT( clearValiditySelection() ), ac, "clearvalidity" );
  actions->clearValidity->setToolTip(i18n("Remove the validity tests on this cell"));

  actions->addModifyComment = new KAction( i18n("&Add/Modify Comment..."),"comment",
      0, view, SLOT( addModifyComment() ), ac, "addmodifycomment" );
  actions->addModifyComment->setToolTip(i18n("Edit a comment for this cell"));

  actions->removeComment = new KAction( i18n("&Remove Comment"),"removecomment",
      0,  view, SLOT( removeComment() ), ac, "removecomment" );
  actions->removeComment->setToolTip(i18n("Remove this cell's comment"));

  actions->clearComment = new KAction( i18n("Comment"),
      0, view, SLOT( clearCommentSelection() ), ac, "clearcomment" );
  actions->clearComment->setToolTip(i18n("Remove this cell's comment"));

  // -- column & row actions --

  actions->resizeColumn = new KAction( i18n("Resize Column..."), "resizecol",
      0, view, SLOT( resizeColumn() ), ac, "resizeCol" );
  actions->resizeColumn->setToolTip(i18n("Change the width of a column"));

  actions->insertColumn = new KAction( i18n("Insert Columns"), "insert_table_col",
      0, view, SLOT( insertColumn() ), ac, "insertColumn" );
  actions->insertColumn->setToolTip(i18n("Inserts a new column into the spreadsheet"));

  actions->deleteColumn = new KAction( i18n("Delete Columns"), "delete_table_col",
      0, view, SLOT( deleteColumn() ), ac, "deleteColumn" );
  actions->deleteColumn->setToolTip(i18n("Removes a column from the spreadsheet"));

  actions->hideColumn = new KAction( i18n("Hide Columns"), "hide_table_column",
      0, view, SLOT( hideColumn() ), ac, "hideColumn" );
  actions->hideColumn->setToolTip(i18n("Hide the column from view"));

  actions->showColumn = new KAction( i18n("Show Columns..."), "show_table_column",
      0, view, SLOT( showColumn() ), ac, "showColumn" );
  actions->showColumn->setToolTip(i18n("Show hidden columns"));

  actions->equalizeColumn = new KAction( i18n("Equalize Column"), "adjustcol",
      0, view, SLOT( equalizeColumn() ), ac, "equalizeCol" );
  actions->equalizeColumn->setToolTip(i18n("Resizes selected columns to be the same size"));

  actions->showSelColumns = new KAction( i18n("Show Columns"), "show_sheet_column",
      0, view, SLOT( showSelColumns() ), ac, "showSelColumns" );
  actions->showSelColumns->setToolTip(i18n("Show hidden columns in the selection"));
  actions->showSelColumns->setEnabled(false);

  actions->resizeRow = new KAction( i18n("Resize Row..."), "resizerow",
      0, view, SLOT( resizeRow() ), ac, "resizeRow" );
  actions->resizeRow->setToolTip(i18n("Change the height of a row"));

  actions->insertRow = new KAction( i18n("Insert Rows"), "insert_table_row",
      0, view, SLOT( insertRow() ), ac, "insertRow" );
  actions->insertRow->setToolTip(i18n("Inserts a new row into the spreadsheet"));

  actions->deleteRow = new KAction( i18n("Delete Rows"), "delete_table_row",
      0, view, SLOT( deleteRow() ), ac, "deleteRow" );
  actions->deleteRow->setToolTip(i18n("Removes a row from the spreadsheet"));

  actions->hideRow = new KAction( i18n("Hide Rows"), "hide_table_row",
      0, view, SLOT( hideRow() ), ac, "hideRow" );
  actions->hideRow->setToolTip(i18n("Hide a row from view"));

  actions->showRow = new KAction( i18n("Show Rows..."), "show_table_row",
      0, view, SLOT( showRow() ), ac, "showRow" );
  actions->showRow->setToolTip(i18n("Show hidden rows"));

  actions->equalizeRow = new KAction( i18n("Equalize Row"), "adjustrow",
      0, view, SLOT( equalizeRow() ), ac, "equalizeRow" );
  actions->equalizeRow->setToolTip(i18n("Resizes selected rows to be the same size"));

  actions->showSelRows = new KAction( i18n("Show Rows"), "show_table_row",
      0, view, SLOT( showSelRows() ), ac, "showSelRows" );
  actions->showSelRows->setEnabled(false);
  actions->showSelRows->setToolTip(i18n("Show hidden rows in the selection"));

  actions->adjust = new KAction( i18n("Adjust Row && Column"),
      0, view, SLOT( adjust() ), ac, "adjust" );
  actions->adjust->setToolTip(i18n("Adjusts row/column size so that the contents will fit"));

  // -- sheet/workbook actions --
  actions->sheetProperties = new KAction( i18n("Sheet Properties"),
      0, view, SLOT( sheetProperties() ), ac, "sheetProperties" );
  actions->sheetProperties->setToolTip(i18n("Modify current sheet's properties"));

  actions->insertSheet = new KAction( i18n("Insert Sheet"),"inserttable",
      0, view, SLOT( insertSheet() ), ac, "insertSheet" );
  actions->insertSheet->setToolTip(i18n("Insert a new sheet"));

  // same action as insertSheet, but without 'insert' in the caption
  actions->menuInsertSheet = new KAction( i18n("&Sheet"),"inserttable",
      0, view, SLOT( insertSheet() ), ac, "menuInsertSheet" );
  actions->menuInsertSheet->setToolTip(i18n("Insert a new sheet"));

  actions->removeSheet = new KAction( i18n("Remove Sheet"), "delete_table",
      0, view, SLOT( removeSheet() ), ac, "removeSheet" );
  actions->removeSheet->setToolTip(i18n("Remove the active sheet"));

  actions->renameSheet=new KAction( i18n("Rename Sheet..."),
      0, view, SLOT( slotRename() ), ac, "renameSheet" );
  actions->renameSheet->setToolTip(i18n("Rename the active sheet"));

  actions->showSheet = new KAction(i18n("Show Sheet..."),
      0, view, SLOT( showSheet()), ac, "showSheet" );
  actions->showSheet->setToolTip(i18n("Show a hidden sheet"));

  actions->hideSheet = new KAction(i18n("Hide Sheet"),
      0, view, SLOT( hideSheet() ), ac, "hideSheet" );
  actions->hideSheet->setToolTip(i18n("Hide the active sheet"));

  actions->autoFormat = new KAction( i18n("AutoFormat..."),
      0, view, SLOT( sheetFormat() ), ac, "sheetFormat" );
  actions->autoFormat->setToolTip(i18n("Set the worksheet formatting"));

  actions->areaName = new KAction( i18n("Area Name..."),
      0, view, SLOT( setAreaName() ), ac, "areaname" );
  actions->areaName->setToolTip(i18n("Set a name for a region of the spreadsheet"));

  actions->showArea = new KAction( i18n("Show Area..."),
      0, view, SLOT( showAreaName() ), ac, "showArea" );
  actions->showArea->setToolTip(i18n("Display a named area"));

  actions->insertFunction = new KAction( i18n("&Function..."), "funct",
      0, view, SLOT( insertMathExpr() ), ac, "insertMathExpr" );
  actions->insertFunction->setToolTip(i18n("Insert math expression"));

  actions->insertSeries = new KAction( i18n("&Series..."),"series",
      0, view, SLOT( insertSeries() ), ac, "series");
  actions->insertSeries ->setToolTip(i18n("Insert a series"));

  actions->insertLink = new KAction( i18n("&Link..."), "insert_link",
      0, view, SLOT( insertHyperlink() ), ac, "insertHyperlink" );
  actions->insertLink->setToolTip(i18n("Insert an Internet hyperlink"));

  actions->removeLink = new KAction( i18n("&Remove Link"),
      0, view, SLOT( removeHyperlink() ), ac, "removeHyperlink" );
  actions->removeLink->setToolTip(i18n("Remove a link"));

  actions->insertSpecialChar = new KAction( i18n( "S&pecial Character..." ), "char",
      view, SLOT( insertSpecialChar() ), ac, "insertSpecialChar" );
  actions->insertSpecialChar->setToolTip( i18n( "Insert one or more symbols or letters not found on the keyboard" ) );

  actions->insertPart = new KoPartSelectAction( i18n("&Object"), "frame_query",
      view, SLOT( insertObject() ), ac, "insertPart");
  actions->insertPart->setToolTip(i18n("Insert an object from another program"));

  actions->insertChartFrame = new KToggleAction( i18n("&Chart"), "insert_chart",
      0, view, SLOT( insertChart() ), ac, "insertChart" );
  actions->insertChartFrame->setToolTip(i18n("Insert a chart"));

  actions->insertPicture = new KAction( i18n("&Picture"),
      0, view, SLOT( insertPicture() ), ac, "insertPicture" );
  actions->insertPicture->setToolTip(i18n("Insert a picture"));

#ifndef QT_NO_SQL
  actions->insertFromDatabase = new KAction( i18n("From &Database..."),
      0, view, SLOT( insertFromDatabase() ),  ac, "insertFromDatabase");
  actions->insertFromDatabase->setToolTip(i18n("Insert data from a SQL database"));
#endif

  actions->insertFromTextfile = new KAction( i18n("From &Text File..."),
      0, view,  SLOT( insertFromTextfile() ), ac, "insertFromTextfile");
  actions->insertFromTextfile->setToolTip(i18n("Insert data from a text file to the current cursor position/selection"));

  actions->insertFromClipboard = new KAction( i18n("From &Clipboard..."),
      0, view, SLOT( insertFromClipboard() ), ac, "insertFromClipboard");
  actions->insertFromClipboard->setToolTip(i18n("Insert CSV data from the clipboard to the current cursor position/selection"));

//   actions->transform = new KAction( i18n("Transform Object..."), "rotate",
//       0, view, SLOT( transformPart() ), ac, "transform" );
//   actions->transform->setToolTip(i18n("Rotate the contents of the cell"));
//   actions->transform->setEnabled( false );

  actions->sort = new KAction( i18n("&Sort..."),
      0, view, SLOT( sort() ), ac, "sort" );
  actions->sort->setToolTip(i18n("Sort a group of cells"));

  actions->sortDec = new KAction( i18n("Sort &Decreasing"), "sort_decrease",
      0, view, SLOT( sortDec() ), ac, "sortDec" );
  actions->sortDec->setToolTip(i18n("Sort a group of cells in decreasing (last to first) order"));

  actions->sortInc = new KAction( i18n("Sort &Increasing"), "sort_incr",
      0, view, SLOT( sortInc() ), ac, "sortInc" );
  actions->sortInc->setToolTip(i18n("Sort a group of cells in ascending (first to last) order"));

  actions->paperLayout = new KAction( i18n("Page Layout..."),
      0, view, SLOT( paperLayoutDlg() ), ac, "paperLayout" );
  actions->paperLayout->setToolTip(i18n("Specify the layout of the spreadsheet for a printout"));

  actions->definePrintRange = new KAction( i18n("Define Print Range"),
      0, view, SLOT( definePrintRange() ), ac, "definePrintRange" );
  actions->definePrintRange->setToolTip(i18n("Define the print range in the current sheet"));

  actions->resetPrintRange = new KAction( i18n("Reset Print Range"),
      0, view, SLOT( resetPrintRange() ), ac, "resetPrintRange" );
  actions->definePrintRange->setToolTip(i18n("Define the print range in the current sheet"));

  actions->showPageBorders = new KToggleAction( i18n("Show Page Borders"),
      ac, "showPageBorders");
  actions->showPageBorders->setCheckedState(i18n("Hide Page Borders"));
  QObject::connect( actions->showPageBorders, SIGNAL( toggled( bool ) ),
      view, SLOT( togglePageBorders( bool ) ) );
  actions->showPageBorders->setToolTip( i18n( "Show on the spreadsheet where the page borders will be" ) );

  actions->recalcWorksheet = new KAction( i18n("Recalculate Sheet"),
      Qt::SHIFT + Qt::Key_F9, view, SLOT( recalcWorkSheet() ), ac, "RecalcWorkSheet" );
  actions->recalcWorksheet->setToolTip(i18n("Recalculate the value of every cell in the current worksheet"));

  actions->recalcWorkbook = new KAction( i18n("Recalculate Document"),
      Qt::Key_F9, view, SLOT( recalcWorkBook() ), ac, "RecalcWorkBook" );
  actions->recalcWorkbook->setToolTip(i18n("Recalculate the value of every cell in all worksheets"));

  actions->protectSheet = new KToggleAction( i18n( "Protect &Sheet..." ),
      ac, "protectSheet" );
  actions->protectSheet->setToolTip( i18n( "Protect the sheet from being modified" ) );
  QObject::connect( actions->protectSheet, SIGNAL( toggled( bool ) ),
      view, SLOT( toggleProtectSheet( bool ) ) );

  actions->protectDoc = new KToggleAction( i18n( "Protect &Document..." ),
      ac, "protectDoc" );
  actions->protectDoc->setToolTip( i18n( "Protect the document from being modified" ) );
  QObject::connect( actions->protectDoc, SIGNAL( toggled( bool ) ),
      view, SLOT( toggleProtectDoc( bool ) ) );

  // -- editing actions --

  actions->copy = KStdAction::copy( view, SLOT( copySelection() ), ac, "copy" );
  actions->copy->setToolTip(i18n("Copy the cell object to the clipboard"));

  actions->paste = KStdAction::paste( view, SLOT( paste() ), ac, "paste" );
  actions->paste->setToolTip(i18n("Paste the contents of the clipboard at the cursor"));

  actions->cut = KStdAction::cut( view, SLOT( cutSelection() ), ac, "cut" );
  actions->cut->setToolTip(i18n("Move the cell object to the clipboard"));

  actions->specialPaste = new KAction( i18n("Special Paste..."), "special_paste",
      0, view, SLOT( specialPaste() ), ac, "specialPaste" );
  actions->specialPaste->setToolTip(i18n("Paste the contents of the clipboard with special options"));

  actions->insertCellCopy = new KAction( i18n("Paste with Insertion"), "insertcellcopy",
      0, view, SLOT( slotInsertCellCopy() ), ac, "insertCellCopy" );
  actions->insertCellCopy->setToolTip(i18n("Inserts a cell from the clipboard into the spreadsheet"));

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
  actions->styleDialog->setToolTip( i18n( "Edit and organize cell styles" ) );

  actions->autoSum = new KAction( i18n("Autosum"), "black_sum",
      0, view, SLOT( autoSum() ), ac, "autoSum" );
  actions->autoSum->setToolTip(i18n("Insert the 'sum' function"));

  actions->spellChecking = KStdAction::spelling( view, SLOT( extraSpelling() ),
      ac, "spelling" );
  actions->spellChecking->setToolTip(i18n("Check the spelling"));

  actions->formulaSelection = new KSelectAction(i18n("Formula Selection"),
      ac, "formulaSelection");
  actions->formulaSelection->setToolTip(i18n("Insert a function"));
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
  actions->consolidate->setToolTip(i18n("Create a region of summary data from a group of similar regions"));

  actions->goalSeek = new KAction( i18n("&Goal Seek..."),
      0, view, SLOT( goalSeek() ), ac, "goalSeek" );
  actions->goalSeek->setToolTip( i18n("Repeating calculation to find a specific value") );

  actions->subTotals = new KAction( i18n("&Subtotals..."),
      0, view, SLOT( subtotals() ), ac, "subtotals" );
  actions->subTotals->setToolTip( i18n("Create different kind of subtotals to a list or database") );

  actions->textToColumns = new KAction( i18n("&Text to Columns..."),
      0, view, SLOT( textToColumns() ), ac, "textToColumns" );
  actions->textToColumns->setToolTip( i18n("Expand the content of cells to multiple columns") );

  actions->multipleOperations = new KAction( i18n("&Multiple Operations..."),
      0, view, SLOT( multipleOperations() ), ac, "multipleOperations" );
  actions->multipleOperations->setToolTip( i18n("Apply the same formula to various cells using different values for the parameter") );

  actions->createTemplate = new KAction( i18n( "&Create Template From Document..." ),
      0, view, SLOT( createTemplate() ), ac, "createTemplate" );

  actions->customList = new KAction( i18n("Custom Lists..."),
      0, view, SLOT( sortList() ), ac, "sortlist" );
  actions->customList->setToolTip(i18n("Create custom lists for sorting or autofill"));

  // -- navigation actions --

  actions->gotoCell = new KAction( i18n("Goto Cell..."),"goto",
      0, view, SLOT( gotoCell() ), ac, "gotoCell" );
  actions->gotoCell->setToolTip(i18n("Move to a particular cell"));

  actions->nextSheet = new KAction( i18n("Next Sheet"), "forward",
      Qt::CTRL+Qt::Key_PageDown, view, SLOT( nextSheet() ), ac, "nextSheet");
  actions->nextSheet->setToolTip(i18n("Move to the next sheet"));

  actions->prevSheet = new KAction( i18n("Previous Sheet"), "back",
      Qt::CTRL+Qt::Key_PageUp, view, SLOT( previousSheet() ), ac, "previousSheet");
  actions->prevSheet->setToolTip(i18n("Move to the previous sheet"));

  actions->firstSheet = new KAction( i18n("First Sheet"), "start",
      0, view, SLOT( firstSheet() ), ac, "firstSheet");
  actions->firstSheet->setToolTip(i18n("Move to the first sheet"));

  actions->lastSheet = new KAction( i18n("Last Sheet"), "finish",
      0, view, SLOT( lastSheet() ), ac, "lastSheet");
  actions->lastSheet->setToolTip(i18n("Move to the last sheet"));

  // -- settings actions --

  actions->showStatusBar = new KToggleAction( i18n("Show Status Bar"),
      ac, "showStatusBar" );
  actions->showStatusBar->setCheckedState(i18n("Hide Status Bar"));
  QObject::connect( actions->showStatusBar, SIGNAL( toggled( bool ) ),
      view, SLOT( showStatusBar( bool ) ) );
  actions->showStatusBar->setToolTip(i18n("Show the status bar"));

  actions->showTabBar = new KToggleAction( i18n("Show Tab Bar"),
      ac, "showTabBar" );
  actions->showTabBar->setCheckedState(i18n("Hide Tab Bar"));
  QObject::connect( actions->showTabBar, SIGNAL( toggled( bool ) ),
      view, SLOT( showTabBar( bool ) ) );
  actions->showTabBar->setToolTip(i18n("Show the tab bar"));

  actions->showFormulaBar = new KToggleAction( i18n("Show Formula Bar"),
      ac, "showFormulaBar" );
  actions->showFormulaBar->setCheckedState(i18n("Hide Formula Bar"));
  QObject::connect( actions->showFormulaBar, SIGNAL( toggled( bool ) ),
      view, SLOT( showFormulaBar( bool ) ) );
  actions->showFormulaBar->setToolTip(i18n("Show the formula bar"));

  actions->preference = new KAction( i18n("Configure KSpread..."),"configure",
      0, view, SLOT( preference() ), ac, "preference" );
  actions->preference->setToolTip(i18n("Set various KSpread options"));

  // -- running calculation actions --

  QActionGroup* groupCalc = new QActionGroup( view );
  actions->calcNone = new KToggleAction( i18n("None"), ac, "menu_none", groupCalc );
  QObject::connect( actions->calcNone, SIGNAL( toggled( bool ) ),
      view, SLOT( menuCalc( bool ) ) );
  actions->calcNone->setToolTip(i18n("No calculation"));

  actions->calcSum = new KToggleAction( i18n("Sum"), ac, "menu_sum", groupCalc );
  QObject::connect( actions->calcSum, SIGNAL( toggled( bool ) ),
      view, SLOT( menuCalc( bool ) ) );
  actions->calcSum->setToolTip(i18n("Calculate using sum"));

  actions->calcMin = new KToggleAction( i18n("Min"), ac, "menu_min", groupCalc );
  QObject::connect( actions->calcMin, SIGNAL( toggled( bool ) ),
      view, SLOT( menuCalc( bool ) ) );
  actions->calcMin->setToolTip(i18n("Calculate using minimum"));

  actions->calcMax = new KToggleAction( i18n("Max"), ac, "menu_max", groupCalc );
  QObject::connect( actions->calcMax, SIGNAL( toggled( bool ) ),
      view, SLOT( menuCalc( bool ) ) );
  actions->calcMax->setToolTip(i18n("Calculate using maximum"));

  actions->calcAverage = new KToggleAction( i18n("Average"), ac, "menu_average", groupCalc );
  QObject::connect( actions->calcAverage, SIGNAL( toggled( bool ) ),
      view, SLOT( menuCalc( bool ) ) );
  actions->calcAverage->setToolTip(i18n("Calculate using average"));

  actions->calcCount = new KToggleAction( i18n("Count"), ac, "menu_count", groupCalc );
  QObject::connect( actions->calcCount, SIGNAL( toggled( bool ) ),
      view, SLOT( menuCalc( bool ) ) );
  actions->calcCount->setToolTip(i18n("Calculate using the count"));

  actions->calcCountA = new KToggleAction( i18n("CountA"), ac, "menu_counta", groupCalc );
  QObject::connect( actions->calcCountA, SIGNAL( toggled( bool ) ),
      view, SLOT( menuCalc( bool ) ) );
  actions->calcCountA->setToolTip(i18n("Calculate using the countA"));

  // -- special action, only for developers --

  actions->internalTests = new KAction( i18n("Run Internal Tests..."), "internalTests",
      Qt::CTRL+ Qt::SHIFT + Qt::Key_T, view, SLOT( runInternalTests() ), ac, "internalTests" );
  actions->inspector = new KAction( i18n("Run Inspector..."), "inspector",
      Qt::CTRL+ Qt::SHIFT + Qt::Key_I, view, SLOT( runInspector() ), ac, "inspector" );

  m_propertyEditor = 0;
}

void View::Private::adjustActions( bool mode )
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
  actions->mergeCellHorizontal->setEnabled( false );
  actions->mergeCellVertical->setEnabled( false );
  actions->insertChartFrame->setEnabled( false );
  actions->sortDec->setEnabled( false );
  actions->sortInc->setEnabled( false );
//   actions->transform->setEnabled( false );

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

  formulaButton->setEnabled( mode );

  if ( activeSheet )
  {
    selection->update();
    view->objectSelectedChanged();
  }
}

void View::Private::adjustActions( Sheet* sheet, Cell* cell )
{
  if ( sheet->isProtected() && !cell->isDefault() && cell->format()->notProtected( cell->column(), cell->row() ) )
  {
    if ( selection->isSingular() )
    {
      if ( !actions->bold->isEnabled() )
        adjustActions( true );
    }
    else
    {
      if ( actions->bold->isEnabled() )
        adjustActions( false );
    }
  }
  else if ( sheet->isProtected() )
  {
    if ( actions->bold->isEnabled() )
      adjustActions( false );
  }
}

void View::Private::adjustWorkbookActions( bool mode )
{
  tabBar->setReadOnly( !view->doc()->isReadWrite() || view->doc()->map()->isProtected() );

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
void View::Private::updateButton( Cell *cell, int column, int row)
{
    toolbarLock = true;

    // workaround for bug #59291 (crash upon starting from template)
    // certain Qt and Fontconfig combination fail miserably if can not
    // find the font name (e.g. not installed in the system)
    QStringList fontList;
    KFontChooser::getFontList( fontList, 0 );
    QString fontFamily = cell->format()->textFontFamily( column,row );
    for ( QStringList::Iterator it = fontList.begin(); it != fontList.end(); ++it )
      if ((*it).toLower() == fontFamily.toLower())
      {
        actions->selectFont->setFont( fontFamily );
        break;
      }

      actions->selectFontSize->setFontSize( cell->format()->textFontSize( column, row ) );
      actions->bold->setChecked( cell->format()->textFontBold( column, row ) );
      actions->italic->setChecked( cell->format()->textFontItalic(  column, row) );
      actions->underline->setChecked( cell->format()->textFontUnderline( column, row ) );
      actions->strikeOut->setChecked( cell->format()->textFontStrike( column, row ) );

      actions->alignLeft->setChecked( cell->format()->align( column, row ) == Style::Left );
      actions->alignCenter->setChecked( cell->format()->align( column, row ) == Style::Center );
      actions->alignRight->setChecked( cell->format()->align( column, row ) == Style::Right );

      actions->alignTop->setChecked( cell->format()->alignY( column, row ) == Style::Top );
      actions->alignMiddle->setChecked( cell->format()->alignY( column, row ) == Style::Middle );
      actions->alignBottom->setChecked( cell->format()->alignY( column, row ) == Style::Bottom );

      actions->verticalText->setChecked( cell->format()->verticalText( column,row ) );

      actions->wrapText->setChecked( cell->format()->multiRow( column,row ) );

    FormatType ft = cell->formatType();
    actions->percent->setChecked( ft == Percentage_format );
    actions->money->setChecked( ft == Money_format );

    if ( activeSheet && !activeSheet->isProtected() )
      actions->removeComment->setEnabled( !cell->format()->comment(column,row).isEmpty() );

    if ( activeSheet && !activeSheet->isProtected() )
      actions->decreaseIndent->setEnabled( cell->format()->getIndent( column, row ) > 0.0 );

    toolbarLock = false;
    if ( activeSheet )
      adjustActions( activeSheet, cell );
}

QAbstractButton* View::Private::newIconButton( const char *_file, bool _kbutton, QWidget *_parent )
{
  if ( _parent == 0L )
    _parent = view;

  if ( !_kbutton ) {
    QPushButton* pb = new QPushButton( _parent );
    pb->setIconSet( SmallIconSet(_file) );
    return pb;
  } else {
    QToolButton* pb = new QToolButton( _parent );
    pb->setIconSet( SmallIconSet(_file) );
    return pb;
  }
}

KPSheetSelectPage::KPSheetSelectPage( QWidget *parent )
: KPrintDialogPage(parent),
  gui(new SheetSelectWidget(this))
{
  setTitle(gui->caption());

  //disabling automated sorting
  gui->ListViewAvailable->setSorting(-1);
  gui->ListViewSelected->setSorting(-1);

  //connect buttons
  connect(gui->ButtonSelectAll,SIGNAL(clicked()),this,SLOT(selectAll()));
  connect(gui->ButtonSelect,SIGNAL(clicked()),this,SLOT(select()));
  connect(gui->ButtonRemove,SIGNAL(clicked()),this,SLOT(remove()));
  connect(gui->ButtonRemoveAll,SIGNAL(clicked()),this,SLOT(removeAll()));

  connect(gui->ButtonMoveTop,SIGNAL(clicked()),this,SLOT(moveTop()));
  connect(gui->ButtonMoveUp,SIGNAL(clicked()),this,SLOT(moveUp()));
  connect(gui->ButtonMoveDown,SIGNAL(clicked()),this,SLOT(moveDown()));
  connect(gui->ButtonMoveBottom,SIGNAL(clicked()),this,SLOT(moveBottom()));
}

// KPSheetSelectPage::~KPSheetSelectPage()
// {
// }

void KPSheetSelectPage::getOptions( QMap<QString,QString>& opts, bool /*incldef*/ )
{
  QStringList sheetlist = this->selectedSheets();
  QStringList::iterator it;
  unsigned int i = 0;
  for (it = sheetlist.begin(); it != sheetlist.end(); ++it, i++)
  {
    opts.insert(printOptionForIndex(i),*it);
  }
}

void KPSheetSelectPage::setOptions( const QMap<QString,QString>& opts )
{
  unsigned int i = 0;
  QStringList sheetlist;
  while (opts.contains(printOptionForIndex(i)))
  {
    sheetlist.prepend(opts[printOptionForIndex(i++)]);
  }

  QStringList::iterator it;
  for (it = sheetlist.begin(); it != sheetlist.end(); ++it)
  {
    kDebug() << " adding sheet to list of printed sheets: " << *it << endl;
    this->prependSelectedSheet(*it);
  }
}

bool KPSheetSelectPage::isValid(QString& /*msg*/)
{
  // we print the activeSheet() by default if no sheet is selected,
  // so we return true in any case

//   Q_ASSERT(gui);
//   if (gui->ListViewSelected->childCount() < 1)
//   {
//     msg = i18n("No sheets selected for printing!");
//     return false;
//   }
  return true;
}

QString KPSheetSelectPage::printOptionForIndex(unsigned int index)
{
  return QString("sheetprintorder%1").arg(index);
}

void KPSheetSelectPage::prependAvailableSheet(const QString& sheetname)
{
  Q_ASSERT(gui);
  new Q3ListViewItem(gui->ListViewAvailable,sheetname);
}

void KPSheetSelectPage::prependSelectedSheet(const QString& sheetname)
{
  Q_ASSERT(gui);
  new Q3ListViewItem(gui->ListViewSelected,sheetname);
}

QStringList KPSheetSelectPage::selectedSheets()
{
  Q_ASSERT(gui);
  QStringList list;
  Q3ListViewItem* item = gui->ListViewSelected->firstChild();
  while (item)
  {
    list.append(item->text(0));
    item = item->nextSibling();
  }
  return list;
}

QStringList KPSheetSelectPage::selectedSheets(KPrinter &prt)
{
  QStringList list;
  unsigned int index;
  const QMap<QString,QString>& options = prt.options();
  for (index = 0; options.contains(KPSheetSelectPage::printOptionForIndex(index)); index++)
  {
    list.append(options[KPSheetSelectPage::printOptionForIndex(index)]);
  }
  return list;
}

void KPSheetSelectPage::clearSelection()
{
  gui->ListViewSelected->clear();
}

void KPSheetSelectPage::selectAll()
{
  //we have to add all the stuff in reverse order
  // because inserted items (prependSelectedSheet) are prepended
  QStringList list;
  Q3ListViewItem* item = gui->ListViewAvailable->firstChild();
  while (item)
  {
    list.prepend(item->text(0));
    item = item->nextSibling();
  }
  QStringList::iterator it;
  for (it = list.begin(); it != list.end(); ++it)
  {
    this->prependSelectedSheet(*it);
  }
}

void KPSheetSelectPage::select()
{
  //we have to add all the stuff in reverse order
  // because inserted items (prependSelectedSheet) are prepended
  QStringList list;
  Q3ListViewItem* item = gui->ListViewAvailable->firstChild();
  while (item)
  {
    if (item->isSelected())
      list.prepend(item->text(0));
    item = item->nextSibling();
  }
  QStringList::iterator it;
  for (it = list.begin(); it != list.end(); ++it)
  {
    this->prependSelectedSheet(*it);
  }
}

void KPSheetSelectPage::remove()
{
  Q3ListViewItem* item = gui->ListViewSelected->firstChild();
  Q3ListViewItem* nextitem = NULL;
  while (item)
  {
    nextitem = item->nextSibling();
    if (item->isSelected())
      delete item;
    item = nextitem;
  }
}

void KPSheetSelectPage::removeAll()
{
  gui->ListViewSelected->clear();
}


void KPSheetSelectPage::moveTop()
{
  //this creates a temporary new list (selected first, then rest)
  // which replaces the existing one, to avoid the need of an additional sort column

  QList<Q3ListViewItem*> newlist;
  Q3ListViewItem* item = gui->ListViewSelected->firstChild();
  Q3ListViewItem* nextitem = NULL;
//   kDebug() << "Filling new list with selected items first" << endl;
  while (item)
  {
    nextitem = item->nextSibling();
    if (item->isSelected())
    {
      newlist.prepend(item);
      gui->ListViewSelected->takeItem(item);
    }
    item = nextitem;
  }
//   kDebug() << "Appending the rest" << endl;
  item = gui->ListViewSelected->firstChild();
  while (item)
  {
//     kDebug() << " processing item " << item->text(0) << endl;
    nextitem = item->nextSibling();
    if (!item->isSelected())
    {
      newlist.prepend(item);
      gui->ListViewSelected->takeItem(item);
    }
    item = nextitem;
  }

//   kDebug() << "Refill the view with the correctly ordered list" << endl;
  //the view is empty now, refill in correct order (reversed!!)
  QList<Q3ListViewItem*>::iterator it;
  for (it = newlist.begin(); it != newlist.end(); ++it)
  {
//     kDebug() << " adding " << (*it)->text(0) << endl;
    gui->ListViewSelected->insertItem(*it);
  }
}

void KPSheetSelectPage::moveUp()
{
  //this creates a temporary new list
  // which replaces the existing one, to avoid the need of an additional sort column

  QList<Q3ListViewItem*> newlist;
  Q3ListViewItem* item = gui->ListViewSelected->firstChild();
  Q3ListViewItem* nextitem = NULL;
  while (item)
  {
    nextitem = item->nextSibling();
    if (!item->isSelected())
    {
      while (nextitem && nextitem->isSelected())
      {
        Q3ListViewItem* nextnextitem = nextitem->nextSibling();
        newlist.prepend(nextitem);
        gui->ListViewSelected->takeItem(nextitem);
        nextitem = nextnextitem;
      }
    }

    newlist.prepend(item);
    gui->ListViewSelected->takeItem(item);
    item = nextitem;
  }

//   kDebug() << "Refill the view with the correctly ordered list" << endl;
  //the view is empty now, refill in correct order (reversed!!)
  QList<Q3ListViewItem*>::iterator it;
  for (it = newlist.begin(); it != newlist.end(); ++it)
  {
//     kDebug() << " adding " << (*it)->text(0) << endl;
    gui->ListViewSelected->insertItem(*it);
  }
}

void KPSheetSelectPage::moveDown()
{
  Q3ListViewItem* item = gui->ListViewSelected->lastItem();
//   while (item)
//   {
//     nextitem = item->nextSibling();
//     if (previousitem && previousitem->isSelected())
//     {
//       previousitem->moveItem(item);
//     }
//     previousitem = item;
//     item = nextitem;
//   }
  while (item)
  {
    while (item && !item->isSelected() && item->itemAbove() && item->itemAbove()->isSelected())
    {
      Q3ListViewItem* tempitem = item->itemAbove();
      tempitem->moveItem(item);
    }
    if (item)
      item = item->itemAbove();
  }
}

void KPSheetSelectPage::moveBottom()
{
  //this creates a temporary new list (unselected first, then rest)
  // which replaces the existing one, to avoid the need of an additional sort column

  QList<Q3ListViewItem*> newlist;
  Q3ListViewItem* item = gui->ListViewSelected->firstChild();
  Q3ListViewItem* nextitem = NULL;
//   kDebug() << "Filling new list with unselected items first" << endl;
  while (item)
  {
//     kDebug() << " processing item " << item->text(0) << endl;
    nextitem = item->nextSibling();
    if (!item->isSelected())
    {
      newlist.prepend(item);
      gui->ListViewSelected->takeItem(item);
    }
    item = nextitem;
  }
//   kDebug() << "Appending the rest" << endl;
  item = gui->ListViewSelected->firstChild();
  while (item)
  {
    nextitem = item->nextSibling();
    if (item->isSelected())
    {
      newlist.prepend(item);
      gui->ListViewSelected->takeItem(item);
    }
    item = nextitem;
  }

//   kDebug() << "Refill the view with the correctly ordered list" << endl;
  //the view is empty now, refill in correct order (reversed!!)
  QList<Q3ListViewItem*>::iterator it;
  for (it = newlist.begin(); it != newlist.end(); ++it)
  {
//     kDebug() << " adding " << (*it)->text(0) << endl;
    gui->ListViewSelected->insertItem(*it);
  }
}


/*****************************************************************************
 *
 * View
 *
 *****************************************************************************/

View::View( QWidget *_parent, const char *_name,
    Doc *_doc )
  : KoView( _doc, _parent, _name )
{
    ElapsedTime et( "View constructor" );
    kDebug(36001) << "sizeof(Cell)=" << sizeof(Cell) <<endl;

    d = new Private;
    d->view = this;
    d->doc = _doc;

    d->dcop = 0;

    d->activeSheet = 0;

    d->toolbarLock = false;
    d->loading = false;

    d->selection = new Selection( this );
    d->choice = new Selection( this );
    d->choice->setMultipleSelection(true);
    connect(d->selection, SIGNAL(changed(const Region&)), this, SLOT(slotChangeSelection(const Region&)));
    connect(d->choice, SIGNAL(changed(const Region&)), this, SLOT(slotChangeChoice(const Region&)));

    d->findOptions = 0;
    d->findLeftColumn = 0;
    d->findRightColumn = 0;
    d->typeValue = FindOption::Value;
    d->directionValue = FindOption::Row;
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

    setInstance( Factory::global() );
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
    KoContainerHandler* h = new KoContainerHandler( this, d->canvas );
    connect( h, SIGNAL( popupMenu( KoChild*, const QPoint& ) ), this, SLOT( popupChildMenu( KoChild*, const QPoint& ) ) );


    connect( this, SIGNAL( childSelected( KoDocumentChild* ) ),
             this, SLOT( slotChildSelected( KoDocumentChild* ) ) );
    connect( this, SIGNAL( childUnselected( KoDocumentChild* ) ),
             this, SLOT( slotChildUnselected( KoDocumentChild* ) ) );
    // If a selected part becomes active this is like it is deselected
    // just before.
    connect( this, SIGNAL( childActivated( KoDocumentChild* ) ),
             this, SLOT( slotChildUnselected( KoDocumentChild* ) ) );

    connect( d->canvas, SIGNAL( objectSelectedChanged() ),
             this, SLOT( objectSelectedChanged() ) );

    QObject::connect( doc()->map(), SIGNAL( sig_addSheet( Sheet* ) ), SLOT( slotAddSheet( Sheet* ) ) );

    QObject::connect( doc(), SIGNAL( sig_refreshView(  ) ), this, SLOT( slotRefreshView() ) );

    QObject::connect( doc(), SIGNAL( sig_refreshLocale() ), this, SLOT( refreshLocale()));

    QObject::connect( doc(), SIGNAL( sig_addAreaName( const QString & ) ), d->posWidget, SLOT( slotAddAreaName( const QString & ) ) );

    QObject::connect( doc(), SIGNAL( sig_removeAreaName( const QString & ) ), d->posWidget, SLOT( slotRemoveAreaName( const QString & ) ) );

    QObject::connect( doc(), SIGNAL( damagesFlushed( const Q3ValueList<Damage*>& ) ),
        this, SLOT( handleDamages( const Q3ValueList<Damage*>& ) ) );

    //KoView::setZoom( doc()->zoomedResolutionY() /* KoView only supports one zoom */ ); // initial value
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

    connect (&d->statusBarOpTimer, SIGNAL(timeout()), this, SLOT(calcStatusBarOp()));
}

View::~View()
{
    //  ElapsedTime el( "~View" );
    if ( doc()->isReadWrite() ) // make sure we're not embedded in Konq
        deleteEditor( true );
    if ( !d->transformToolBox.isNull() )
        delete (&*d->transformToolBox);
    /*if (d->calcLabel)
    {
        disconnect(d->calcLabel,SIGNAL(pressed( int )),this,SLOT(statusBarClicked(int)));

        }*/

    delete d->spell.kspell;

    d->canvas->endChoose();
    d->activeSheet = 0; // set the active sheet to 0L so that when during destruction
    // of embedded child documents possible repaints in Sheet are not
    // performed. The repains can happen if you delete an embedded document,
    // which leads to an regionInvalidated() signal emission in KoView, which calls
    // repaint, etc.etc. :-) (Simon)

    delete d->selection;
    delete d->choice;

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
    d->viewLayout = new Q3GridLayout( this, 3, 4 );

    // Vert. Scroll Bar
    d->calcLabel  = 0;
    d->vertScrollBar = new QScrollBar( this, "ScrollBar_2" );
    d->vertScrollBar->setRange( 0, 4096 );
    d->vertScrollBar->setOrientation( Qt::Vertical );
    d->vertScrollBar->setLineStep(60);  //just random guess based on what feels okay
    d->vertScrollBar->setPageStep(60);  //This should be controlled dynamically, depending on how many rows are shown

    // Edit Bar
    d->toolWidget = new QFrame( this );

    d->formulaBarLayout = new Q3HBoxLayout( d->toolWidget );
    d->formulaBarLayout->setMargin( 4 );
    d->formulaBarLayout->addSpacing( 2 );

    d->posWidget = new ComboboxLocationEditWidget( d->toolWidget, this );
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
    d->canvas = new Canvas( this );

    // The line-editor that appears above the sheet and allows to
    // edit the cells content. It knows about the two buttons.
    d->editWidget = new EditWidget( d->toolWidget, d->canvas, d->cancelButton, d->okButton );
    d->editWidget->setFocusPolicy( Qt::StrongFocus );
    d->formulaBarLayout->addWidget( d->editWidget, 2 );
    d->formulaBarLayout->addSpacing( 2 );

    d->canvas->setEditWidget( d->editWidget );

    d->hBorderWidget = new HBorder( this, d->canvas,this );
    d->vBorderWidget = new VBorder( this, d->canvas ,this );

    d->canvas->setFocusPolicy( Qt::StrongFocus );
    QWidget::setFocusPolicy( Qt::StrongFocus );
    setFocusProxy( d->canvas );

    connect( this, SIGNAL( invalidated() ), d->canvas, SLOT( update() ) );

    QWidget* bottomPart = new QWidget( this );
    d->tabScrollBarLayout = new Q3HBoxLayout( bottomPart );
    d->tabScrollBarLayout->setAutoAdd( true );
    d->tabBar = new KoTabBar( bottomPart );
    d->horzScrollBar = new QScrollBar( bottomPart, "ScrollBar_1" );

    d->horzScrollBar->setRange( 0, 4096 );
    d->horzScrollBar->setOrientation( Qt::Horizontal );

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
    d->viewLayout->addWidget( d->toolWidget, 0, 0, 0, 2 );
    d->viewLayout->addWidget( d->hBorderWidget, 1, 1, 1, 2 );
    d->viewLayout->addWidget( d->vBorderWidget, 2, 0 );
    d->viewLayout->addWidget( d->canvas, 2, 1 );
    d->viewLayout->addWidget( d->vertScrollBar, 2, 2 );
    d->viewLayout->addWidget( bottomPart, 3, 3, 0, 2 );

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

Canvas* View::canvasWidget() const
{
    return d->canvas;
}

HBorder* View::hBorderWidget()const
{
    return d->hBorderWidget;
}

VBorder* View::vBorderWidget()const
{
    return d->vBorderWidget;
}

QScrollBar* View::horzScrollBar()const
{
    return d->horzScrollBar;
}

QScrollBar* View::vertScrollBar()const
{
    return d->vertScrollBar;
}

EditWidget* View::editWidget()const
{
    return d->editWidget;
}

ComboboxLocationEditWidget* View::posWidget()const
{
    return d->posWidget;
}

KoTabBar* View::tabBar() const
{
    return d->tabBar;
}

bool View::isLoading() const
{
    return d->loading;
}

Selection* View::selectionInfo() const
{
    return d->selection;
}

Selection* View::choice() const
{
  return d->choice;
}

void View::resetInsertHandle()
{
  d->actions->insertChartFrame->setChecked( false );
 // d->actions->insertPicture->setChecked( false );

  d->insertHandler = 0;
}

bool View::isInsertingObject()
{
    return d->insertHandler;
}

const Sheet* View::activeSheet() const
{
    return d->activeSheet;
}

Sheet* View::activeSheet()
{
    return d->activeSheet;
}

void View::initConfig()
{
    KConfig *config = Factory::global()->config();
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
        doc()->setMoveToValue((KSpread::MoveTo)config->readNumEntry("Move",(int)(Bottom)));
        doc()->setIndentValue( config->readDoubleNumEntry( "Indent", 10.0 ) );
        doc()->setTypeOfCalc((MethodOfCalc)config->readNumEntry("Method of Calc",(int)(SumOfNumber)));
        if ( !doc()->configLoadFromFile() )
            doc()->setShowTabBar(config->readBoolEntry("Tabbar",true));

  doc()->setShowMessageError(config->readBoolEntry( "Msg error" ,false) );

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
 calcStatusBarOp();
}

void View::changeNbOfRecentFiles(int _nb)
{
    if (shell())
        shell()->setMaxRecentItems( _nb );
}

void View::initCalcMenu()
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


void View::recalcWorkBook()
{
  if (!activeSheet())
	  return;

  Sheet * tbl;
  doc()->emitBeginOperation( true );
  for ( tbl = doc()->map()->firstSheet();
        tbl != 0L;
        tbl = doc()->map()->nextSheet() )
  {
   // bool b = tbl->getAutoCalc();
   // tbl->setAutoCalc( true );
    tbl->recalc( /*force recalculation = */ true);
   // tbl->setAutoCalc( b );
  }

  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::refreshLocale()
{
  doc()->emitBeginOperation(true);
  Sheet *tbl;
  for ( tbl = doc()->map()->firstSheet();
        tbl != 0L;
        tbl = doc()->map()->nextSheet() )
  {
    tbl->updateLocale();
  }
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::recalcWorkSheet()
{
  if ( d->activeSheet != 0 )
  {
    doc()->emitBeginOperation( true );
//    bool b = d->activeSheet->getAutoCalc();
//    d->activeSheet->setAutoCalc( true );
    d->activeSheet->recalc( /*force recalculation = */ true);
 //   d->activeSheet->setAutoCalc( b );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}


void View::extraSpelling()
{
  if ( d->spell.kspell )
    return; // Already in progress

  if (d->activeSheet == 0L)
    return;

  d->spell.macroCmdSpellCheck = 0L;
  d->spell.firstSpellSheet    = d->activeSheet;
  d->spell.currentSpellSheet  = d->spell.firstSpellSheet;

  QRect selection = d->selection->selection();

  // if nothing is selected, check every cell
  if (d->selection->isSingular())
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


void View::startKSpell()
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

void View::spellCheckerReplaceAll( const QString &orig, const QString & replacement)
{
    d->spell.replaceAll.append( orig);
    d->spell.replaceAll.append( replacement);
}


void View::spellCheckerIgnoreAll( const QString & word)
{
    doc()->addIgnoreWordAll( word );
}


void View::spellCheckerReady()
{
  if (d->canvas)
    d->canvas->setCursor( Qt::WaitCursor );

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
      if ( d->spell.currentCell && d->spell.currentCell->isDefault() )
        kDebug() << "checking default cell!!" << endl << endl;
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
      Cell * cell = d->spell.currentSpellSheet->cellAt( x, y );

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


void View::spellCleanup()
{
  if ( d->canvas )
    d->canvas->setCursor( Qt::ArrowCursor );

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


bool View::spellSwitchToOtherSheet()
{
  // there is no other sheet
  if ( doc()->map()->count() == 1 )
    return false;

  // for optimization
  Q3PtrList<Sheet> sheetList = doc()->map()->sheetList();

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


void View::spellCheckerMisspelling( const QString &,
                                           const QStringList &,
                                           unsigned int )
{
  // scroll to the cell
  if ( !d->spell.spellCheckSelection )
  {
    d->spell.spellCurrCellX = d->spell.currentCell->column();
    d->spell.spellCurrCellY = d->spell.currentCell->row();
  }

  d->selection->initialize(QPoint(d->spell.spellCurrCellX, d->spell.spellCurrCellY));
}


void View::spellCheckerCorrected( const QString & old, const QString & corr,
                                         unsigned int pos )
{
  Cell * cell;

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

  UndoSetText* undo = new UndoSetText( doc(), d->activeSheet,
                                                     content,
                                                     d->spell.spellCurrCellX,
                                                     d->spell.spellCurrCellY,
                                                     cell->formatType());
  content.replace( pos, old.length(), corr );
  cell->setCellText( content );
  d->editWidget->setText( content );

  if ( !d->spell.macroCmdSpellCheck )
      d->spell.macroCmdSpellCheck = new MacroUndoAction( doc(), i18n("Correct Misspelled Word") );
  d->spell.macroCmdSpellCheck->addCommand( undo );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::spellCheckerDone( const QString & )
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

void View::spellCheckerFinished()
{
  if (d->canvas)
    d->canvas->setCursor( Qt::ArrowCursor );

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
    PreferenceDialog configDlg( this, 0 );
    configDlg.openPage( PreferenceDialog::KS_SPELLING);
    configDlg.exec();
  }
}

void View::initialPosition()
{
    // Loading completed, pick initial worksheet
    Q3PtrListIterator<Sheet> it( doc()->map()->sheetList() );
    for( ; it.current(); ++it )
      addSheet( it.current() );

    Sheet * tbl = 0L;
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
    d->selection->initialize( QPoint(col, row) );

    // Set the initial X and Y offsets for the view.
    d->canvas->setXOffset( doc()->map()->initialXOffset() );
    d->canvas->setYOffset( doc()->map()->initialYOffset() );

    updateBorderButton();
    updateShowSheetMenu();

    d->actions->autoFormat->setEnabled(false);
    d->actions->sort->setEnabled(false);
    d->actions->mergeCell->setEnabled(false);
    d->actions->mergeCellHorizontal->setEnabled(false);
    d->actions->mergeCellVertical->setEnabled(false);
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


void View::updateEditWidgetOnPress()
{
    if (!d->activeSheet)
	    return;

    int column = d->canvas->markerColumn();
    int row    = d->canvas->markerRow();

    Cell* cell = d->activeSheet->cellAt( column, row );
    if ( !cell )
    {
        d->editWidget->setText( "" );
        return;
    }
    if ( d->activeSheet->isProtected() && cell->format()->isHideFormula( column, row ) )
        d->editWidget->setText( cell->strOutText() );
    else if ( d->activeSheet->isProtected() && cell->format()->isHideAll( column, row ) )
        d->editWidget->setText( "" );
    else
        d->editWidget->setText( cell->text() );

    d->updateButton(cell, column, row);
    d->adjustActions( d->activeSheet, cell );
}

void View::updateEditWidget()
{
    if (!d->activeSheet)
	    return;

    int column = d->canvas->markerColumn();
    int row    = d->canvas->markerRow();

    Cell * cell = d->activeSheet->cellAt( column, row );
    bool active = activeSheet()->getShowFormula()
        && !( d->activeSheet->isProtected() && cell && cell->format()->isHideFormula( column, row ) );

    if ( d->activeSheet && !d->activeSheet->isProtected() )
    {
      d->actions->alignLeft->setEnabled(!active);
      d->actions->alignCenter->setEnabled(!active);
      d->actions->alignRight->setEnabled(!active);
    }

    if ( !cell )
    {
        d->editWidget->setText( "" );
        if ( d->activeSheet->isProtected() )
          d->editWidget->setEnabled( false );
        else
          d->editWidget->setEnabled( true );
        return;
    }

    if ( d->activeSheet->isProtected() && cell->format()->isHideFormula( column, row ) )
        d->editWidget->setText( cell->strOutText() );
    else if ( d->activeSheet->isProtected() && cell->format()->isHideAll( column, row ) )
        d->editWidget->setText( "" );
    else
        d->editWidget->setText( cell->text() );

    if ( d->activeSheet->isProtected() && !cell->format()->notProtected( column, row ) )
      d->editWidget->setEnabled( false );
    else
      d->editWidget->setEnabled( true );

    if ( d->canvas->editor() )
    {
      d->canvas->editor()->setEditorFont(cell->format()->textFont(column, row), true);
      d->canvas->editor()->setFocus();
    }
    d->updateButton(cell, column, row);
    d->adjustActions( d->activeSheet, cell );
}

void View::activateFormulaEditor()
{
}

void View::objectSelectedChanged()
{
  if ( d->canvas->isObjectSelected() )
    d->actions->actionExtraProperties->setEnabled( true );
  else
    d->actions->actionExtraProperties->setEnabled( false );
}

void View::updateReadWrite( bool readwrite )
{
    // d->cancelButton->setEnabled( readwrite );
    // d->okButton->setEnabled( readwrite );
  d->editWidget->setEnabled( readwrite );

  QList<KAction*> actions = actionCollection()->actions();
  QList<KAction*>::ConstIterator aIt = actions.begin();
  QList<KAction*>::ConstIterator aEnd = actions.end();
  for (; aIt != aEnd; ++aIt )
    (*aIt)->setEnabled( readwrite );

//   d->actions->transform->setEnabled( false );
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

void View::createTemplate()
{
  int width = 60;
  int height = 60;
  QPixmap pix = doc()->generatePreview(QSize(width, height));

  KTempFile tempFile( QString::null, ".kst" );
  //Check that creation of temp file was successful
  if (tempFile.status() != 0)
  {
	  qWarning("Creation of temprary file to store template failed.");
	  return;
  }

  tempFile.setAutoDelete(true);

  doc()->saveNativeFormat( tempFile.name() );

  KoTemplateCreateDia::createTemplate( "kspread_template", Factory::global(),
                                           tempFile.name(), pix, this );

  Factory::global()->dirs()->addResourceType("kspread_template",
                                                       KStandardDirs::kde_default( "data" ) +
                                                       "kspread/templates/");
}

void View::sheetFormat()
{
    FormatDialog dlg( this );
    dlg.exec();
}

void View::autoSum()
{
	if (!activeSheet())
		return;

    // ######## Torben: Make sure that this can not be called
    // when canvas has a running editor
    if ( d->canvas->editor() )
        return;

  //Get the selected range and remove the current cell from it (as that is
  //where the result of the autosum will be stored - perhaps change
  //this behaviour??)
  Range rg;
  //rg.sheet=activeSheet();
  QRect sel = d->selection->selection(false);

  if (sel.height() > 1)
  {
    if (d->selection->marker().y()==sel.top())
      sel.setTop(sel.top()+1);
    if (d->selection->marker().y()==sel.bottom())
      sel.setBottom(sel.bottom()-1);
  }
  else
  {
    if (sel.width() > 1)
    {
      if (d->selection->marker().x()==sel.left())
        sel.setLeft(sel.left()+1);

      if (d->selection->marker().x()==sel.right())
        sel.setRight(sel.right()-1);
    }
    else
    {
      sel=QRect();

      // only 1 cell selected
      // try to automagically find cells the user wants to sum up

      int start = -1, end = -1;

      if ( (d->selection->marker().y() > 1) && activeSheet()->cellAt(d->selection->marker().x(), d->selection->marker().y()-1)->value().isNumber() )
      {
        // check cells above the current one
        start = end = d->selection->marker().y()-1;
        for (start--; (start > 0) && activeSheet()->cellAt(d->selection->marker().x(), start)->value().isNumber(); start--) ;

        Point startPoint, endPoint;
        startPoint.setRow(start+1);
        startPoint.setColumn(d->selection->marker().x());
        endPoint.setRow(end);
        endPoint.setColumn(d->selection->marker().x());

        QString str = Range(startPoint, endPoint).toString();

        d->canvas->createEditor( Canvas::CellEditor , true , true );
        d->canvas->editor()->setText("=SUM(" + str + ")");
        d->canvas->editor()->setCursorPosition(5 + str.length());
        return;
      }
      else if ( (d->selection->marker().x() > 1) && activeSheet()->cellAt(d->selection->marker().x()-1, d->selection->marker().y())->value().isNumber() )
      {
        // check cells to the left of the current one
        start = end = d->selection->marker().x()-1;
        for (start--; (start > 0) && activeSheet()->cellAt(start, d->selection->marker().y())->value().isNumber(); start--) ;

        Point startPoint, endPoint;
        startPoint.setColumn(start+1);
        startPoint.setRow(d->selection->marker().y());
        endPoint.setColumn(end);
        endPoint.setRow(d->selection->marker().y());

        QString str = Range(startPoint, endPoint).toString();

        d->canvas->createEditor( Canvas::CellEditor , true , true );
        d->canvas->editor()->setText("=SUM(" + str + ")");
        d->canvas->editor()->setCursorPosition(5 + str.length());
        return;
      }
    }
  }

  if ( (sel.width() > 1) && (sel.height() > 1) )
    sel=QRect();

  rg.setRange(sel);

  d->canvas->createEditor( Canvas::CellEditor , true , true );


  if ( (rg.range().isValid() ) && (!rg.range().isEmpty()) )
  {
    d->canvas->editor()->setText( "=SUM("+rg.toString()+")" );
    d->canvas->deleteEditor(true);
  }
  else
  {
    d->canvas->editor()->setText( "=SUM()" );
    d->canvas->editor()->setCursorPosition( 5 );
  }
}

/*
void View::oszilloscope()
{
    QDialog* dlg = new OsziDlg( this );
    dlg->show();
}
*/

void View::changeTextColor()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation(false);
    d->activeSheet->setSelectionTextColor( selectionInfo(), d->actions->textColor->color() );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void View::setSelectionTextColor(const QColor &txtColor)
{
  if (d->activeSheet != 0L)
  {
    doc()->emitBeginOperation(false);
    d->activeSheet->setSelectionTextColor( selectionInfo(), txtColor );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::changeBackgroundColor()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation(false);
    d->activeSheet->setSelectionbgColor( selectionInfo(), d->actions->bgColor->color() );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void View::setSelectionBackgroundColor(const QColor &bgColor)
{
  if (d->activeSheet != 0L)
  {
    doc()->emitBeginOperation(false);
    d->activeSheet->setSelectionbgColor( selectionInfo(), bgColor );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void View::changeBorderColor()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation(false);
    d->activeSheet->setSelectionBorderColor( selectionInfo(), d->actions->borderColor->color() );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void View::setSelectionBorderColor(const QColor &bdColor)
{
  if (d->activeSheet != 0L)
  {
    doc()->emitBeginOperation(false);
    d->activeSheet->setSelectionBorderColor( selectionInfo(), bdColor );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void View::helpUsing()
{
  KToolInvocation::invokeHelp( );
}

void View::enableUndo( bool _b )
{
  KAction* action = actionCollection()->action( "office_undo" );
  if( action ) action->setEnabled( _b );
}

void View::enableRedo( bool _b )
{
  KAction* action = actionCollection()->action( "office_redo" );
  if( action ) action->setEnabled( _b );
}

void View::enableInsertColumn( bool _b )
{
  if ( d->activeSheet && !d->activeSheet->isProtected() )
    d->actions->insertColumn->setEnabled( _b );
}

void View::enableInsertRow( bool _b )
{
  if ( d->activeSheet && !d->activeSheet->isProtected() )
    d->actions->insertRow->setEnabled( _b );
}

void View::deleteColumn()
{
  if ( !d->activeSheet )
    return;

  doc()->emitBeginOperation( false );

  QRect r( d->selection->selection() );

  d->activeSheet->removeColumn( r.left(), ( r.right()-r.left() ) );

  updateEditWidget();
  // Stefan: update the selection after deleting (a) column(s)
  d->selection->update();

  QRect vr( d->activeSheet->visibleRect( d->canvas ) );
  vr.setLeft( r.left() );

  doc()->emitEndOperation( vr );
}

void View::deleteRow()
{
  if ( !d->activeSheet )
    return;

  doc()->emitBeginOperation( false );
  QRect r( d->selection->selection() );
  d->activeSheet->removeRow( r.top(),(r.bottom()-r.top()) );

  updateEditWidget();
  // Stefan: update the selection after deleting (a) column(s)
  d->selection->update();

  QRect vr( d->activeSheet->visibleRect( d->canvas ) );
  vr.setTop( r.top() );

  doc()->emitEndOperation( vr );
}

void View::insertColumn()
{
  if ( !d->activeSheet )
    return;

  doc()->emitBeginOperation( false );
  QRect r( d->selection->selection() );
  d->activeSheet->insertColumn( r.left(), ( r.right()-r.left() ) );

  updateEditWidget();

  QRect vr( d->activeSheet->visibleRect( d->canvas ) );
  vr.setLeft( r.left() - 1 );

  doc()->emitEndOperation( vr );
}

void View::hideColumn()
{
  if ( !d->activeSheet )
    return;

  if ( d->selection->isRowSelected() )
  {
    KMessageBox::error( this, i18n( "Area is too large." ) );
    return;
  }

  d->activeSheet->hideColumn(*selectionInfo());
}

void View::showColumn()
{
  if ( !d->activeSheet )
    return;

  ShowColRow dlg( this, "showCol", ShowColRow::Column );
  dlg.exec();
}

void View::showSelColumns()
{
  if ( !d->activeSheet )
    return;

  d->activeSheet->showColumn(*selectionInfo());
}

void View::insertRow()
{
  if ( !d->activeSheet )
    return;
  doc()->emitBeginOperation( false );
  QRect r( d->selection->selection() );
  d->activeSheet->insertRow( r.top(), ( r.bottom() - r.top() ) );

  updateEditWidget();
  QRect vr( d->activeSheet->visibleRect( d->canvas ) );
  vr.setTop( r.top() - 1 );

  doc()->emitEndOperation( vr );
}

void View::hideRow()
{
  if ( !d->activeSheet )
    return;

  if ( d->selection->isColumnSelected() )
  {
    KMessageBox::error( this, i18n( "Area is too large." ) );
    return;
  }

  d->activeSheet->hideRow(*selectionInfo());
}

void View::showRow()
{
  if ( !d->activeSheet )
    return;

  ShowColRow dlg( this, "showRow", ShowColRow::Row );
  dlg.exec();
}

void View::showSelRows()
{
  if ( !d->activeSheet )
    return;

  d->activeSheet->showRow(*selectionInfo());
}

void View::fontSelected( const QString & _font )
{
  if ( d->toolbarLock )
    return;

  doc()->emitBeginOperation(false);
  if ( d->activeSheet != 0L )
    d->activeSheet->setSelectionFont( d->selection, _font.toLatin1() );

  // Dont leave the focus in the toolbars combo box ...
  if ( d->canvas->editor() )
  {
    Cell * cell = d->activeSheet->cellAt( d->selection->marker() );
    d->canvas->editor()->setEditorFont( cell->format()->textFont( cell->column(), cell->row() ), true );
    d->canvas->editor()->setFocus();
  }
  else
    d->canvas->setFocus();

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::decreaseFontSize()
{
  setSelectionFontSize( -1 );
}

void View::increaseFontSize()
{
  setSelectionFontSize( 1 );
}

void View::setSelectionFontSize( int size )
{
  if ( d->activeSheet != NULL )
  {
    d->activeSheet->setSelectionSize( selectionInfo(), size );
  }
}

void View::lower()
{
  if ( !d->activeSheet  )
    return;

  doc()->emitBeginOperation( false );

  d->activeSheet->setSelectionUpperLower( selectionInfo(), -1 );
  updateEditWidget();

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::upper()
{
  if ( !d->activeSheet  )
    return;

  doc()->emitBeginOperation( false );

  d->activeSheet->setSelectionUpperLower( selectionInfo(), 1 );
  updateEditWidget();

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::firstLetterUpper()
{
  if ( !d->activeSheet  )
    return;
  doc()->emitBeginOperation( false );
  d->activeSheet->setSelectionfirstLetterUpper( selectionInfo() );
  updateEditWidget();

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::verticalText(bool b)
{
  if ( !d->activeSheet  )
    return;

  doc()->emitBeginOperation( false );
  d->activeSheet->setSelectionVerticalText( selectionInfo(), b );
  d->activeSheet->adjustArea(*selectionInfo());
  updateEditWidget(); // TODO Stefan: nescessary?

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::insertSpecialChar()
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

void View::slotSpecialCharDlgClosed()
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

void View::slotSpecialChar( QChar c, const QString & _font )
{
  if ( d->activeSheet )
  {
    QPoint marker( d->selection->marker() );
    Cell * cell = d->activeSheet->nonDefaultCell( marker );
    if ( cell->format()->textFont( marker.x(), marker.y() ).family() != _font )
    {
      cell->format()->setTextFontFamily( _font );
    }
    EditWidget * edit = d->editWidget;
    QKeyEvent ev( QEvent::KeyPress, 0, 0, 0, QString( c ) );
    QApplication::sendEvent( edit, &ev );
  }
}

void View::insertMathExpr()
{
  if ( d->activeSheet == 0L )
    return;

  FormulaDialog * dlg = new FormulaDialog( this, "Function" );
  dlg->show();

  /* TODO - because I search on 'TODO's :-) */
  // #### Is the dialog deleted when it's closed ? (David)
  // Torben thinks that not.
}

void View::formulaSelection( const QString &_math )
{
  if ( d->activeSheet == 0 )
    return;

  if ( _math == i18n("Others...") )
  {
    insertMathExpr();
    return;
  }

  FormulaDialog *dlg = new FormulaDialog( this, "Formula Editor", _math );
  dlg->exec();
}

void View::fontSizeSelected( int _size )
{
  if ( d->toolbarLock )
    return;

  doc()->emitBeginOperation( false );

  if ( d->activeSheet != 0L )
    d->activeSheet->setSelectionFont( selectionInfo(), 0L, _size );

  // Dont leave the focus in the toolbars combo box ...
  if ( d->canvas->editor() )
  {
    Cell * cell = d->activeSheet->cellAt( d->selection->marker() );
    d->canvas->editor()->setEditorFont( cell->format()->textFont( d->canvas->markerColumn(),
                                                                  d->canvas->markerRow() ), true );
    d->canvas->editor()->setFocus();
  }
  else
    d->canvas->setFocus();

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::bold( bool b )
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
    Cell * cell = d->activeSheet->cellAt( col, row );
    d->canvas->editor()->setEditorFont( cell->format()->textFont( col, row ), true );
  }

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::underline( bool b )
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
    Cell * cell = d->activeSheet->cellAt( col, row );
    d->canvas->editor()->setEditorFont( cell->format()->textFont( col, row ), true );
  }

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::strikeOut( bool b )
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
    Cell * cell = d->activeSheet->cellAt( col, row );
    d->canvas->editor()->setEditorFont( cell->format()->textFont( col, row ), true );
  }

  markSelectionAsDirty();
  doc()->emitEndOperation();
}


void View::italic( bool b )
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
    Cell * cell = d->activeSheet->cellAt( col, row );
    d->canvas->editor()->setEditorFont( cell->format()->textFont( col, row ), true );
  }

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::sortInc()
{
  if (!activeSheet())
	  return;

  QRect range = d->selection->selection();
  if ( d->selection->isSingular() )
  {
    KMessageBox::error( this, i18n( "You must select multiple cells." ) );
    return;
  }

  doc()->emitBeginOperation( false );

  // Entire row(s) selected ? Or just one row ?
  if ( d->selection->isRowSelected() || range.top() == range.bottom() )
    activeSheet()->sortByRow( range, range.top(), Sheet::Increase );
  else
    activeSheet()->sortByColumn( range, range.left(), Sheet::Increase );
  updateEditWidget();

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::sortDec()
{
  QRect range = d->selection->selection();
  if ( d->selection->isSingular() )
  {
    KMessageBox::error( this, i18n( "You must select multiple cells." ) );
    return;
  }

  doc()->emitBeginOperation( false );

    // Entire row(s) selected ? Or just one row ?
  if ( d->selection->isRowSelected() || range.top() == range.bottom() )
    activeSheet()->sortByRow( range, range.top(), Sheet::Decrease );
  else
    activeSheet()->sortByColumn( range, range.left(), Sheet::Decrease );
  updateEditWidget();

  markSelectionAsDirty();
  doc()->emitEndOperation();
}


void View::borderBottom()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );

    d->activeSheet->borderBottom( d->selection, d->actions->borderColor->color() );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::setSelectionBottomBorderColor( const QColor & color )
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->borderBottom( selectionInfo(), color );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::borderRight()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( d->activeSheet->layoutDirection()==Sheet::RightToLeft )
      d->activeSheet->borderLeft( d->selection, d->actions->borderColor->color() );
    else
      d->activeSheet->borderRight( d->selection, d->actions->borderColor->color() );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::setSelectionRightBorderColor( const QColor & color )
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( d->activeSheet->layoutDirection()==Sheet::RightToLeft )
      d->activeSheet->borderLeft( selectionInfo(), color );
    else
      d->activeSheet->borderRight( selectionInfo(), color );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::borderLeft()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( d->activeSheet->layoutDirection()==Sheet::RightToLeft )
      d->activeSheet->borderRight( d->selection, d->actions->borderColor->color() );
    else
      d->activeSheet->borderLeft( d->selection, d->actions->borderColor->color() );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::setSelectionLeftBorderColor( const QColor & color )
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( d->activeSheet->layoutDirection()==Sheet::RightToLeft )
      d->activeSheet->borderRight( selectionInfo(), color );
    else
      d->activeSheet->borderLeft( selectionInfo(), color );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::borderTop()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->borderTop( d->selection, d->actions->borderColor->color() );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::setSelectionTopBorderColor( const QColor & color )
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->borderTop( selectionInfo(), color );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::borderOutline()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->borderOutline( d->selection, d->actions->borderColor->color() );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::setSelectionOutlineBorderColor( const QColor & color )
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->borderOutline( selectionInfo(), color );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::borderAll()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->borderAll( d->selection, d->actions->borderColor->color() );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::setSelectionAllBorderColor( const QColor & color )
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->borderAll( selectionInfo(), color );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::borderRemove()
{
  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation(false);
    d->activeSheet->borderRemove( d->selection );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::addSheet( Sheet * _t )
{
  doc()->emitBeginOperation( false );

  insertSheet( _t );

  // Connect some signals
  QObject::connect( _t, SIGNAL( sig_refreshView() ), SLOT( slotRefreshView() ) );
  QObject::connect( _t, SIGNAL( sig_updateView( Sheet* ) ), SLOT( slotUpdateView( Sheet* ) ) );
  QObject::connect( _t->print(), SIGNAL( sig_updateView( Sheet* ) ), SLOT( slotUpdateView( Sheet* ) ) );
  QObject::connect( _t, SIGNAL( sig_updateView( Sheet *, const Region& ) ),
                    SLOT( slotUpdateView( Sheet*, const Region& ) ) );
  QObject::connect( _t, SIGNAL( sig_updateView( EmbeddedObject* )), SLOT( slotUpdateView( EmbeddedObject* ) ) );

  QObject::connect( _t, SIGNAL( sig_updateHBorder( Sheet * ) ),
                    SLOT( slotUpdateHBorder( Sheet * ) ) );
  QObject::connect( _t, SIGNAL( sig_updateVBorder( Sheet * ) ),
                    SLOT( slotUpdateVBorder( Sheet * ) ) );
  QObject::connect( _t, SIGNAL( sig_nameChanged( Sheet*, const QString& ) ),
                    this, SLOT( slotSheetRenamed( Sheet*, const QString& ) ) );
  QObject::connect( _t, SIGNAL( sig_SheetHidden( Sheet* ) ),
                    this, SLOT( slotSheetHidden( Sheet* ) ) );
  QObject::connect( _t, SIGNAL( sig_SheetShown( Sheet* ) ),
                    this, SLOT( slotSheetShown( Sheet* ) ) );
  QObject::connect( _t, SIGNAL( sig_SheetRemoved( Sheet* ) ),
                    this, SLOT( slotSheetRemoved( Sheet* ) ) );
  // ########### Why do these signals not send a pointer to the sheet?
  // This will lead to bugs.
  QObject::connect( _t, SIGNAL( sig_updateChildGeometry( EmbeddedKOfficeObject* ) ),
                    SLOT( slotUpdateChildGeometry( EmbeddedKOfficeObject* ) ) );
  QObject::connect( _t, SIGNAL( sig_maxColumn( int ) ), d->canvas, SLOT( slotMaxColumn( int ) ) );
  QObject::connect( _t, SIGNAL( sig_maxRow( int ) ), d->canvas, SLOT( slotMaxRow( int ) ) );

  if ( !d->loading )
    updateBorderButton();

  if ( !d->activeSheet )
  {
    doc()->emitEndOperation();
    return;
  }
  doc()->emitEndOperation( *selectionInfo() );
}

void View::slotSheetRemoved( Sheet *_t )
{
  doc()->emitBeginOperation( false );

  QString m_sheetName=_t->sheetName();
  d->tabBar->removeTab( _t->sheetName() );
  if (doc()->map()->findSheet( doc()->map()->visibleSheets().first()))
    setActiveSheet( doc()->map()->findSheet( doc()->map()->visibleSheets().first() ));
  else
    d->activeSheet = 0L;

  QList<Reference>::Iterator it;
  QList<Reference> area=doc()->listArea();
  for ( it = area.begin(); it != area.end(); ++it )
  {
    //remove Area Name when sheet target is removed
    if ( (*it).sheet_name == m_sheetName )
    {
      doc()->removeArea( (*it).ref_name );
      //now area name is used in formula
      //so you must recalc sheets when remove areaname
      Sheet * tbl;

      for ( tbl = doc()->map()->firstSheet(); tbl != 0L; tbl = doc()->map()->nextSheet() )
      {
        tbl->refreshRemoveAreaName((*it).ref_name);
      }
    }
  }

  doc()->emitEndOperation( *selectionInfo() );
}

void View::removeAllSheets()
{
  doc()->emitBeginOperation(false);
  d->tabBar->clear();

  setActiveSheet( 0L );

  doc()->emitEndOperation();
}

void View::setActiveSheet( Sheet * _t, bool updateSheet )
{
  if ( _t == d->activeSheet )
    return;

  doc()->emitBeginOperation(false);

  saveCurrentSheetSelection();

  Sheet * oldSheet = d->activeSheet;

  d->activeSheet = _t;

  if ( d->activeSheet == 0L )
  {
    doc()->emitEndOperation();
    return;
  }

  if ( oldSheet && oldSheet->layoutDirection()==Sheet::RightToLeft != d->activeSheet->layoutDirection()==Sheet::RightToLeft )
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
  QMap<Sheet*, QPoint>::Iterator it = d->savedAnchors.find(d->activeSheet);
  QMap<Sheet*, QPoint>::Iterator it2 = d->savedMarkers.find(d->activeSheet);

  // TODO Stefan: store the save markers/anchors in the Selection?
  QPoint newAnchor = (it == d->savedAnchors.end()) ? QPoint(1,1) : *it;
  QPoint newMarker = (it2 == d->savedMarkers.end()) ? QPoint(1,1) : *it2;

  d->selection->clear();
  d->selection->setSheet( d->activeSheet );
  d->selection->initialize(QRect(newMarker, newAnchor));

  d->canvas->scrollToCell(newMarker);
  calcStatusBarOp();

  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::slotSheetRenamed( Sheet* sheet, const QString& old_name )
{
  doc()->emitBeginOperation( false );
  d->tabBar->renameTab( old_name, sheet->sheetName() );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::slotSheetHidden( Sheet* )
{
  doc()->emitBeginOperation(false);
  updateShowSheetMenu();
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::slotSheetShown( Sheet* )
{
  doc()->emitBeginOperation(false);
  d->tabBar->setTabs( doc()->map()->visibleSheets() );
  updateShowSheetMenu();
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::changeSheet( const QString& _name )
{
    if ( activeSheet()->sheetName() == _name )
        return;

    Sheet *t = doc()->map()->findSheet( _name );
    if ( !t )
    {
        kDebug(36001) << "Unknown sheet " << _name << endl;
        return;
    }
    doc()->emitBeginOperation(false);
    d->canvas->closeEditor(); // for selection mode
    setActiveSheet( t, false /* False: Endless loop because of setActiveTab() => do the visual area update manually*/);

    d->canvas->updateEditor(); // for choose mode
    updateEditWidget();
    //refresh toggle button
    updateBorderButton();

    //update visible area
    d->vBorderWidget->repaint();
    d->hBorderWidget->repaint();
    d->canvas->slotMaxColumn( d->activeSheet->maxColumn() );
    d->canvas->slotMaxRow( d->activeSheet->maxRow() );
    t->setRegionPaintDirty( t->visibleRect( d->canvas ) );
    doc()->emitEndOperation();
}

void View::moveSheet( unsigned sheet, unsigned target )
{
    if( doc()->map()->isProtected() ) return;

    QStringList vs = doc()->map()->visibleSheets();

    if( target >= vs.count() )
        doc()->map()->moveSheet( vs[ sheet ], vs[ vs.count()-1 ], false );
    else
        doc()->map()->moveSheet( vs[ sheet ], vs[ target ], true );

    d->tabBar->moveTab( sheet, target );
}

void View::sheetProperties()
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
    dlg->setShowCommentIndicator( d->activeSheet->getShowCommentIndicator() );
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
        command->setShowCommentIndicator( dlg->showCommentIndicator() );
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

void View::insertSheet()
{
  if ( doc()->map()->isProtected() )
  {
    KMessageBox::error( 0, i18n ( "You cannot change a protected sheet." ) );
    return;
  }

  doc()->emitBeginOperation( false );
  d->canvas->closeEditor();
  Sheet * t = doc()->map()->createSheet();
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

void View::hideSheet()
{
  if ( !d->activeSheet )
    return;

  if ( doc()->map()->visibleSheets().count() ==  1)
  {
     KMessageBox::error( this, i18n("You cannot hide the last visible sheet.") );
     return;
  }

  QStringList vs = doc()->map()->visibleSheets();
  int i = vs.indexOf( d->activeSheet->tableName() ) - 1;
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

void View::showSheet()
{
  if ( !d->activeSheet )
    return;

  ShowDialog dlg( this, "Sheet show");
  dlg.exec();
}

void View::copySelection()
{
  if ( !d->activeSheet )
    return;

  if ( canvasWidget()->isObjectSelected() )
  {
    canvasWidget()->copyOasisObjects();
    return;
  }
  if ( !d->canvas->editor() )
  {
    d->activeSheet->copySelection( selectionInfo() );

    updateEditWidget();
  }
  else
    d->canvas->editor()->copy();
}

void View::copyAsText()
{
  if ( !d->activeSheet )
    return;
  d->activeSheet->copyAsText( selectionInfo() );
}


void View::cutSelection()
{
  if ( !d->activeSheet )
    return;
  //don't used this function when we edit a cell.
  doc()->emitBeginOperation(false);

  if ( canvasWidget()->isObjectSelected() )
  {
    canvasWidget()->copyOasisObjects();
    markSelectionAsDirty();
    doc()->emitEndOperation();

    KMacroCommand * macroCommand = 0L;
    Q3PtrListIterator<EmbeddedObject> it( doc()->embeddedObjects() );
    for ( ; it.current() ; ++it )
    {
      if ( it.current()->sheet() == canvasWidget()->activeSheet() && it.current()->isSelected() )
      {
        if( !macroCommand )
          macroCommand = new KMacroCommand( i18n( "Cut Objects" ) );
        RemoveObjectCommand *cmd = new RemoveObjectCommand( it.current(), true );
        macroCommand->addCommand( cmd );
      }
    }
    if ( macroCommand )
    {
      doc()->addCommand( macroCommand );
      canvasWidget()->setMouseSelectedObject( false );
      macroCommand->execute();
    }

    return;
  }
  if ( !d->canvas->editor())
  {
    d->activeSheet->cutSelection( selectionInfo() );
    calcStatusBarOp();
    updateEditWidget();
    }
else
    d->canvas->editor()->cut();

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::paste()
{
  if ( !d->activeSheet )
    return;

  if (!koDocument()->isReadWrite()) // don't paste into a read only document
    return;

  QMimeSource *data = QApplication::clipboard()->data( QClipboard::Clipboard );
  for ( int i=0; data->format(i) != 0; i++ )
    kDebug() << "format:" << data->format(i) << endl;

  if ( data->provides( KoStoreDrag::mimeType("application/vnd.oasis.opendocument.spreadsheet" ) ))
  {
    canvasWidget()->deselectAllObjects();
    QByteArray returnedTypeMime = "application/vnd.oasis.opendocument.spreadsheet";
    QByteArray arr = data->encodedData( returnedTypeMime );
    if( arr.isEmpty() )
      return;
    QBuffer buffer( &arr );
    KoStore * store = KoStore::createStore( &buffer, KoStore::Read );

    KoOasisStore oasisStore( store );
    QDomDocument doc;
    QString errorMessage;
    bool ok = oasisStore.loadAndParse( "content.xml", doc, errorMessage );
    if ( !ok ) {
      kError(32001) << "Error parsing content.xml: " << errorMessage << endl;
      return;
    }

    KoOasisStyles oasisStyles;
    QDomDocument stylesDoc;
    (void)oasisStore.loadAndParse( "styles.xml", stylesDoc, errorMessage );
    // Load styles from style.xml
    oasisStyles.createStyleMap( stylesDoc, true );
    // Also load styles from content.xml
    oasisStyles.createStyleMap( doc, false );

    // from KSpreadDoc::loadOasis:
    QDomElement content = doc.documentElement();
    QDomElement realBody ( KoDom::namedItemNS( content, KoXmlNS::office, "body" ) );
    if ( realBody.isNull() )
    {
      kDebug() << "Invalid OASIS OpenDocument file. No office:body tag found." << endl;
      return;
    }
    QDomElement body = KoDom::namedItemNS( realBody, KoXmlNS::office, "spreadsheet" );

    if ( body.isNull() )
    {
      kError(32001) << "No office:spreadsheet found!" << endl;
      QDomElement childElem;
      QString localName;
      forEachElement( childElem, realBody ) {
        localName = childElem.localName();
      }
      return;
    }

    KoOasisLoadingContext context( d->doc, oasisStyles, store );
    Q_ASSERT( !oasisStyles.officeStyle().isNull() );

    //load in first
    d->doc->styleManager()->loadOasisStyleTemplate( oasisStyles );

//     // TODO check versions and mimetypes etc.
    d->doc->loadOasisAreaName( body );
    d->doc->loadOasisCellValidation( body );

    //Copied from kspread_doc.cc - refactor into one place asap.
    Q3DictIterator<QDomElement> it( oasisStyles.styles("table-cell") );
    Q3Dict<Style> styleElements;
    for (;it.current();++it)
    {
	if ( it.current()->hasAttributeNS( KoXmlNS::style , "name" ) )
	{
		QString name = it.current()->attributeNS( KoXmlNS::style , "name" , QString::null );
		styleElements.insert( name , new Style());
		styleElements[name]->loadOasisStyle( oasisStyles , *(it.current()) );
	}
    }

    // all <sheet:sheet> goes to workbook
    bool result = d->doc->map()->loadOasis( body, context, styleElements );

    //release unused styles (again, copied from kspread_doc.cc, needs to be refactored into one place asap.)
    Q3DictIterator<Style> styleIter( styleElements );
    for (;styleIter.current();++styleIter)
	    if (styleIter.current()->release())
		    delete styleIter.current();

    if (!result)
      return;

  }
  else
  {
    //TODO:  What if the clipboard data is available in both pixmap and OASIS format? (ie. for embedded parts)
    QPixmap clipboardPixmap = QApplication::clipboard()->pixmap( QClipboard::Clipboard );
    if (!clipboardPixmap.isNull())
    {
        d->activeSheet->insertPicture( markerDocumentPosition()  , clipboardPixmap );
    }
  }

  doc()->emitBeginOperation( false );
  if ( !d->canvas->editor() )
  {
      //kDebug(36001) << "Pasting. Rect= " << d->selection->selection(false) << " bytes" << endl;
    d->activeSheet->paste( d->selection->lastRange(), true,
                           Paste::Normal, Paste::OverWrite,
                           false, 0, true );
    calcStatusBarOp();
    updateEditWidget();
  }
  else
  {
    d->canvas->editor()->paste();
  }
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::specialPaste()
{
  if ( !d->activeSheet )
    return;

  SpecialDialog dlg( this, "Special Paste" );
  if ( dlg.exec() )
  {
    if ( d->activeSheet->getAutoCalc() )
    {
      doc()->emitBeginOperation( false );
      d->activeSheet->recalc();
      doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
    }
    calcStatusBarOp();
    updateEditWidget();
  }
}

void View::removeComment()
{
  if ( !d->activeSheet )
        return;

  doc()->emitBeginOperation(false);
  d->activeSheet->setSelectionRemoveComment( selectionInfo() );
  updateEditWidget();

  markSelectionAsDirty();
  doc()->emitEndOperation();
}


void View::changeAngle()
{
  if ( !d->activeSheet )
    return;

  AngleDialog dlg( this, "Angle" ,
                    QPoint( d->canvas->markerColumn(), d->canvas->markerRow() ));
  if ( dlg.exec() )
  {
    //TODO Stefan: where is the angle operation?
    d->activeSheet->adjustArea(*selectionInfo());
  }
}

void View::setSelectionAngle( int angle )
{
  doc()->emitBeginOperation( false );

  if ( d->activeSheet != NULL )
  {
    d->activeSheet->setSelectionAngle( selectionInfo(), angle );
    d->activeSheet->adjustArea(*selectionInfo());
  }

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::mergeCell()
{
  // sanity check
  if( !d->activeSheet )
    return;
  d->activeSheet->mergeCells(*selectionInfo());
}

void View::mergeCellHorizontal()
{
  // sanity check
  if( !d->activeSheet )
    return;
  d->activeSheet->mergeCells(*selectionInfo(), true);
}

void View::mergeCellVertical()
{
  // sanity check
  if( !d->activeSheet )
    return;
  d->activeSheet->mergeCells(*selectionInfo(), false, true);
}

void View::dissociateCell()
{
  // sanity check
  if( !d->activeSheet )
    return;
  d->activeSheet->dissociateCells(*selectionInfo());
}


void View::increaseIndent()
{
  if ( !d->activeSheet )
    return;

  doc()->emitBeginOperation( false );
  d->activeSheet->increaseIndent( d->selection );
  updateEditWidget();

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::decreaseIndent()
{
  if ( !d->activeSheet )
    return;

  doc()->emitBeginOperation( false );
  int column = d->canvas->markerColumn();
  int row = d->canvas->markerRow();

  d->activeSheet->decreaseIndent( d->selection );
  Cell * cell = d->activeSheet->cellAt( column, row );
  if ( cell )
    if ( !d->activeSheet->isProtected() )
      d->actions->decreaseIndent->setEnabled( cell->format()->getIndent( column, row ) > 0.0 );

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::goalSeek()
{
  if ( d->canvas->editor() )
  {
    d->canvas->deleteEditor( true ); // save changes
  }

  GoalSeekDialog * dlg
    = new GoalSeekDialog( this, QPoint( d->canvas->markerColumn(),
                                            d->canvas->markerRow() ),
                              "GoalSeekDialog" );
  dlg->show();
  /* dialog autodeletes itself */
}

void View::subtotals()
{
  if (!activeSheet())
	  return;

  QRect selection( d->selection->selection() );
  if ( ( selection.width() < 2 ) || ( selection.height() < 2 ) )
  {
    KMessageBox::error( this, i18n("You must select multiple cells.") );
    return;
  }

  SubtotalDialog dlg(this, selection, "SubtotalDialog" );
  if ( dlg.exec() )
  {
    doc()->emitBeginOperation( false );

    d->selection->initialize( QRect(dlg.selection().topLeft(), dlg.selection().bottomRight()));//, dlg.sheet() );
    doc()->emitEndOperation( selection );
  }
}

void View::multipleOperations()
{
  if ( d->canvas->editor() )
  {
    d->canvas->deleteEditor( true ); // save changes
  }
  //  MultipleOpDlg * dlg = new MultipleOpDlg( this, "MultipleOpDlg" );
  //  dlg->show();
}

void View::textToColumns()
{
  if (!activeSheet())
	  return;

  d->canvas->closeEditor();

  QRect area=d->selection->selection();

  //Only use the first column
  area.setRight(area.left());

/* if ( area.width() > 1 )
  {
  //Only use the first column

    KMessageBox::error( this, i18n("You must not select an area containing more than one column.") );
    return;
  }*/

  CSVDialog dialog( this, "CSVDialog", area, CSVDialog::Column );
  if( !dialog.cancelled() )
    dialog.exec();
}

void View::consolidate()
{
  d->canvas->closeEditor();
  ConsolidateDialog * dlg = new ConsolidateDialog( this, "ConsolidateDialog" );
  dlg->show();
  // dlg destroys itself
}

void View::sortList()
{
  if (!activeSheet()) return;

  ListDialog dlg( this, "List selection" );
  dlg.exec();
}

void View::gotoCell()
{
  if (!activeSheet()) return;

  GotoDialog dlg( this, "GotoCell" );
  dlg.exec();
}

void View::find()
{
   if (!activeSheet()) return;

    FindDlg dlg( this, "Find", d->findOptions, d->findStrings );
    dlg.setHasSelection( !d->selection->isSingular() );
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
void View::initFindReplace()
{
    KFind* findObj = d->find ? d->find : d->replace;
    Q_ASSERT( findObj );
    connect(findObj, SIGNAL( highlight( const QString &, int, int ) ),
            this, SLOT( slotHighlight( const QString &, int, int ) ) );
    connect(findObj, SIGNAL( findNext() ),
            this, SLOT( findNext() ) );

    bool bck = d->findOptions & KFind::FindBackwards;
    Sheet* currentSheet = d->searchInSheets.currentSheet;

    QRect region = ( d->findOptions & KFind::SelectedText )
                   ? d->selection->selection()
                   : QRect( 1, 1, currentSheet->maxColumn(), currentSheet->maxRow() ); // All cells

    int colStart = !bck ? region.left() : region.right();
    int colEnd = !bck ? region.right() : region.left();
    int rowStart = !bck ? region.top() :region.bottom();
    int rowEnd = !bck ? region.bottom() : region.top();
    if ( d->findOptions & KFind::FromCursor ) {
        QPoint marker( d->selection->marker() );
        colStart = marker.x();
        rowStart = marker.y();
    }
    d->findLeftColumn = region.left();
    d->findRightColumn = region.right();
    d->findPos = QPoint( colStart, rowStart );
    d->findEnd = QPoint( colEnd, rowEnd );
    //kDebug() << k_funcinfo << d->findPos << " to " << d->findEnd << endl;
    //kDebug() << k_funcinfo << "leftcol=" << d->findLeftColumn << " rightcol=" << d->findRightColumn << endl;
}

void View::findNext()
{
    KFind* findObj = d->find ? d->find : d->replace;
    if ( !findObj )  {
        find();
        return;
    }
    KFind::Result res = KFind::NoMatch;
    Cell* cell = findNextCell();
    bool forw = ! ( d->findOptions & KFind::FindBackwards );
    while ( res == KFind::NoMatch && cell )
    {
        if ( findObj->needData() )
        {
            if ( d->typeValue == FindOption::Note )
                findObj->setData( cell->format()->comment( cell->column(), cell->row() ) );
            else
                findObj->setData( cell->text() );
            d->findPos = QPoint( cell->column(), cell->row() );
            //kDebug() << "setData(cell " << d->findPos << ")" << endl;
        }

        // Let KFind inspect the text fragment, and display a dialog if a match is found
        if ( d->find )
            res = d->find->find();
        else
            res = d->replace->replace();

        if ( res == KFind::NoMatch )  {
            // Go to next cell, skipping unwanted cells
            if ( d->directionValue == FindOption::Row )
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
            d->findOptions &= ~KFind::FromCursor;
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

Cell* View::nextFindValidCell( int col, int row )
{
    Cell *cell = d->searchInSheets.currentSheet->cellAt( col, row );
    if ( cell->isDefault() || cell->isObscured() || cell->isFormula() )
        cell = 0L;
    if ( d->typeValue == FindOption::Note && cell && cell->format()->comment(col, row).isEmpty())
        cell = 0L;
    return cell;
}

Cell* View::findNextCell()
{
    // getFirstCellRow / getNextCellRight would be faster at doing that,
    // but it doesn't seem to be easy to combine it with 'start a column d->find.x()'...

    Sheet* sheet = d->searchInSheets.currentSheet;
    Cell* cell = 0L;
    bool forw = ! ( d->findOptions & KFind::FindBackwards );
    int col = d->findPos.x();
    int row = d->findPos.y();
    int maxRow = sheet->maxRow();
    //kDebug() << "findNextCell starting at " << col << "," << row << "   forw=" << forw << endl;

    if ( d->directionValue == FindOption::Row )
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
            //kDebug() << "next row: " << col << "," << row << endl;
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
            //kDebug() << "next row: " << col << "," << row << endl;
        }
    }
    // if ( !cell )
    // No more next cell - TODO go to next sheet (if not looking in a selection)
    // (and make d->findEnd (max,max) in that case...)
    //kDebug() << k_funcinfo << " returning " << cell << endl;
    return cell;
}

void View::findPrevious()
{
    KFind* findObj = d->find ? d->find : d->replace;
    if ( !findObj )  {
        find();
        return;
    }
    //kDebug() << "findPrevious" << endl;
    int opt = d->findOptions;
    bool forw = ! ( opt & KFind::FindBackwards );
    if ( forw )
        d->findOptions = ( opt | KFind::FindBackwards );
    else
        d->findOptions = ( opt & ~KFind::FindBackwards );

    findNext();

    d->findOptions = opt; // restore initial options
}

void View::replace()
{
    SearchDlg dlg( this, "Replace", d->findOptions, d->findStrings, d->replaceStrings );
    dlg.setHasSelection( !d->selection->isSingular() );
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
        UndoChangeAreaTextCell *undo = new UndoChangeAreaTextCell( doc(), d->searchInSheets.currentSheet, region );
        doc()->addCommand( undo );
    }

    findNext();

#if 0
    // Refresh the editWidget
    // TODO - after a replacement only?
    Cell *cell = activeSheet()->cellAt( canvasWidget()->markerColumn(),
                                               canvasWidget()->markerRow() );
    if ( cell->text() != 0L )
        d->editWidget->setText( cell->text() );
    else
        d->editWidget->setText( "" );
#endif
}

void View::slotHighlight( const QString &/*text*/, int /*matchingIndex*/, int /*matchedLength*/ )
{
    d->selection->initialize( d->findPos );
    KDialogBase *baseDialog=0L;
    if ( d->find )
        baseDialog = d->find->findNextDialog();
    else
        baseDialog = d->replace->replaceNextDialog();
    kDebug()<<" baseDialog :"<<baseDialog<<endl;
    QRect globalRect( d->findPos, d->findEnd );
    globalRect.moveTopLeft( canvasWidget()->mapToGlobal( globalRect.topLeft() ) );
    KDialog::avoidArea( baseDialog, QRect( d->findPos, d->findEnd ));
}

void View::slotReplace( const QString &newText, int, int, int )
{
    // Which cell was this again?
    Cell *cell = d->searchInSheets.currentSheet->cellAt( d->findPos );

    // ...now I remember, update it!
    cell->setDisplayDirtyFlag();
    if ( d->typeValue == FindOption::Value )
        cell->setCellText( newText );
    else if ( d->typeValue == FindOption::Note )
      cell->format()->setComment( newText );
    cell->clearDisplayDirtyFlag();
}

void View::conditional()
{
  QRect rect( d->selection->selection() );

  if ( util_isRowOrColumnSelected(rect))
  {
    KMessageBox::error( this, i18n("Area is too large.") );
  }
  else
  {
    ConditionalDialog dlg( this, "ConditionalDialog", rect);
    dlg.exec();
  }
}

void View::validity()
{
  QRect rect( d->selection->selection() );

  if (d->selection->isColumnOrRowSelected())
  {
    KMessageBox::error( this, i18n("Area is too large."));
  }
  else
  {
    DlgValidity dlg( this,"validity",rect);
    dlg.exec();
  }
}


void View::insertSeries()
{
    d->canvas->closeEditor();
    SeriesDlg dlg( this, "Series", QPoint( d->canvas->markerColumn(), d->canvas->markerRow() ) );
    dlg.exec();
}

void View::sort()
{
    if ( d->selection->isSingular() )
    {
        KMessageBox::error( this, i18n("You must select multiple cells.") );
        return;
    }

    SortDialog dlg( this, "Sort" );
    dlg.exec();
}

void View::removeHyperlink()
{
    QPoint marker( d->selection->marker() );
    Cell * cell = d->activeSheet->cellAt( marker );
    if( !cell ) return;
    if( cell->link().isEmpty() ) return;

    LinkCommand* command = new LinkCommand( cell, QString::null, QString::null );
    doc()->addCommand( command );
    command->execute();

  canvasWidget()->setFocus();
  d->editWidget->setText( cell->text() );
}

void View::insertHyperlink()
{
    if (!activeSheet())
    	return;

    d->canvas->closeEditor();

    QPoint marker( d->selection->marker() );
    Cell* cell = d->activeSheet->cellAt( marker );

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
      d->editWidget->setText( cell->text() );
    }
    delete dlg;
}

void View::insertFromDatabase()
{
#ifndef QT_NO_SQL
    d->canvas->closeEditor();

    QRect rect = d->selection->selection();

  QStringList str = QSqlDatabase::drivers();
  if ( str.isEmpty() )
    {
      KMessageBox::error( this, i18n("No database drivers available.  To use this feature you need "
        "to install the necessary Qt 3 database drivers.") );

    return;
    }

    DatabaseDialog dlg(this, rect, "DatabaseDialog");
    dlg.exec();
#endif
}

void View::insertFromTextfile()
{
    d->canvas->closeEditor();
    //KMessageBox::information( this, "Not implemented yet, work in progress...");

    CSVDialog dialog( this, "CSVDialog", d->selection->selection(), CSVDialog::File );
    if( !dialog.cancelled() )
      dialog.exec();
}

void View::insertFromClipboard()
{
    d->canvas->closeEditor();

    CSVDialog dialog( this, "CSVDialog", d->selection->selection(), CSVDialog::Clipboard );
    if( !dialog.cancelled() )
      dialog.exec();
}

void View::setupPrinter( KPrinter &prt )
{
    if (!activeSheet())
	    return;

    SheetPrint* print = d->activeSheet->print();

    //apply page layout parameters
    KoFormat pageFormat = print->paperFormat();

    prt.setPageSize( static_cast<KPrinter::PageSize>( KoPageFormat::printerPageSize( pageFormat ) ) );

    if ( print->orientation() == PG_LANDSCAPE || pageFormat == PG_SCREEN )
        prt.setOrientation( KPrinter::Landscape );
    else
        prt.setOrientation( KPrinter::Portrait );

    prt.setFullPage( true );

    //add possibility to select the sheets to print:
//     kDebug() << "Adding sheet selection page." << endl;
    KPSheetSelectPage* sheetpage = new KPSheetSelectPage();
    prt.addDialogPage(sheetpage);

//     kDebug() << "Iterating through available sheets and initializing list of available sheets." << endl;
    Q3PtrList<Sheet> sheetlist = doc()->map()->sheetList();
    Sheet* sheet = sheetlist.last();
    while ( sheet )
    {
      kDebug() << "Adding " << sheet->sheetName() << endl;
      sheetpage->prependAvailableSheet(sheet->sheetName());
      sheet = sheetlist.prev();
    }
}

void View::print( KPrinter &prt )
{
    if (!activeSheet())
	    return;

    //save the current active sheet for later, so we can restore it at the end
    Sheet* selectedsheet = this->activeSheet();

    //print all sheets in the order given by the print dialog (Sheet Selection)
    QStringList sheetlist = KPSheetSelectPage::selectedSheets(prt);

    if (sheetlist.empty())
    {
      kDebug() << "No sheet for printing selected, printing active sheet" << endl;
      sheetlist.append(d->activeSheet->sheetName());
    }

    QPainter painter;
    painter.begin( &prt );

    bool firstpage = true;

    QStringList::iterator sheetlistiterator;
    for (sheetlistiterator = sheetlist.begin(); sheetlistiterator != sheetlist.end(); ++sheetlistiterator)
    {
        kDebug() << "  printing sheet " << *sheetlistiterator << endl;
        Sheet* sheet = doc()->map()->findSheet(*sheetlistiterator);
        if (sheet == NULL)
        {
          kWarning() << i18n("Sheet %1 could not be found for printing").arg(*sheetlistiterator) << endl;
          continue;
        }

        setActiveSheet(sheet,FALSE);

        SheetPrint* print = d->activeSheet->print();

        if (firstpage)
          firstpage=false;
        else
        {
          kDebug() << " inserting new page" << endl;
          prt.newPage();
        }

        if ( d->canvas->editor() )
        {
            d->canvas->deleteEditor( true ); // save changes
        }

        int oldZoom = doc()->zoom();

        //Comment from KWord
        //   We don't get valid metrics from the printer - and we want a better resolution
        //   anyway (it's the PS driver that takes care of the printer resolution).
        //But KSpread uses fixed 300 dpis, so we can use it.

        Q3PaintDeviceMetrics metrics( &prt );

        int dpiX = metrics.logicalDpiX();
        int dpiY = metrics.logicalDpiY();

        doc()->setZoomAndResolution( int( print->zoom() * 100 ), dpiX, dpiY );

        //store the current setting in a temporary variable
        KoOrientation _orient = print->orientation();

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
                KMessageBox::information( 0,
                i18n("Nothing to print for sheet %1.").arg(
                d->activeSheet->sheetName()) );
                //@todo: make sure we really can comment this out,
                //       what to do with partially broken printouts?
//                 prt.abort();
            }
        }
    }

    painter.end();
    this->setActiveSheet(selectedsheet);
}

void View::insertChart( const QRect& _geometry, KoDocumentEntry& _e )
{
    if ( !d->activeSheet )
      return;

    // Transform the view coordinates to document coordinates
    KoRect unzoomedRect = doc()->unzoomRect( _geometry );
    unzoomedRect.moveBy( d->canvas->xOffset(), d->canvas->yOffset() );

    InsertObjectCommand *cmd = 0;
    if ( d->selection->isColumnOrRowSelected() )
    {
      KMessageBox::error( this, i18n("Area is too large."));
      return;
    }
    else
      cmd = new InsertObjectCommand( unzoomedRect, _e, d->selection->selection(), d->canvas  );

    doc()->addCommand( cmd );
    cmd->execute();
}

void View::insertChild( const QRect& _geometry, KoDocumentEntry& _e )
{
  if ( !d->activeSheet )
    return;

  // Transform the view coordinates to document coordinates
  KoRect unzoomedRect = doc()->unzoomRect( _geometry );
  unzoomedRect.moveBy( d->canvas->xOffset(), d->canvas->yOffset() );

  InsertObjectCommand *cmd = new InsertObjectCommand( unzoomedRect, _e, d->canvas );
  doc()->addCommand( cmd );
  cmd->execute();
}

KoPoint View::markerDocumentPosition()
{
	QPoint marker=selectionInfo()->marker();

	return KoPoint( d->activeSheet->dblColumnPos(marker.x()),
		        d->activeSheet->dblRowPos(marker.y()) );
}

void View::insertPicture()
{
  //Note:  We don't use the usual insert handler here (which allows the user to drag-select the target area
  //for the object) because when inserting raster graphics, it is usually desireable to insert at 100% size,
  //since the graphic won't look right if inserted with an incorrect aspect ratio or if blurred due to the
  //scaling.  If the user wishes to change the size and/or aspect ratio, they can do that afterwards.
  //This behaviour can be seen in other spreadsheets.
  //-- Robert Knight 12/02/06 <robertknight@gmail.com>

  KUrl file = KFileDialog::getImageOpenURL( QString::null, d->canvas );

  if (file.isEmpty())
 	return;

  if ( !d->activeSheet )
    	return;

  InsertObjectCommand *cmd = new InsertObjectCommand( KoRect(markerDocumentPosition(),KoSize(0,0)) , file, d->canvas );
  doc()->addCommand( cmd );
  cmd->execute();
}

void View::slotUpdateChildGeometry( EmbeddedKOfficeObject */*_child*/ )
{
    // ##############
    // TODO
    /*
  if ( _child->sheet() != d->activeSheet )
    return;

  // Find frame for child
  ChildFrame *f = 0L;
  QPtrListIterator<ChildFrame> it( m_lstFrames );
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

void View::toggleProtectDoc( bool mode )
{
   if ( !doc() || !doc()->map() )
     return;

   QByteArray passwd;
   if ( mode )
   {
     int result = KPasswordDialog::getNewPassword( this, passwd, i18n( "Protect Document" ) );
     if ( result != KPasswordDialog::Accepted )
     {
       d->actions->protectDoc->setChecked( false );
       return;
     }

     QByteArray hash( "" );
     QString password( passwd );
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );
     doc()->map()->setProtected( hash );
   }
   else
   {
     int result = KPasswordDialog::getPassword( this, passwd, i18n( "Unprotect Document" ) );
     if ( result != KPasswordDialog::Accepted )
     {
       d->actions->protectDoc->setChecked( true );
       return;
     }

     QByteArray hash( "" );
     QString password( passwd );
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );
     if ( !doc()->map()->checkPassword( hash ) )
     {
       KMessageBox::error( 0, i18n( "Password is incorrect." ) );
       d->actions->protectDoc->setChecked( true );
       return;
     }

     doc()->map()->setProtected( QByteArray() );
   }

   doc()->setModified( true );
   d->adjustWorkbookActions( !mode );
}

void View::toggleProtectSheet( bool mode )
{
   if ( !d->activeSheet )
       return;

   QByteArray passwd;
   if ( mode )
   {
     int result = KPasswordDialog::getNewPassword( this, passwd, i18n( "Protect Sheet" ) );
     if ( result != KPasswordDialog::Accepted )
     {
       d->actions->protectSheet->setChecked( false );
       return;
     }

     QByteArray hash( "" );
     QString password( passwd );
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );

     d->activeSheet->setProtected( hash );
   }
   else
   {
     int result = KPasswordDialog::getPassword( this, passwd, i18n( "Unprotect Sheet" ) );
     if ( result != KPasswordDialog::Accepted )
     {
       d->actions->protectSheet->setChecked( true );
       return;
     }

     QByteArray hash( "" );
     QString password( passwd );
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );

     if ( !d->activeSheet->checkPassword( hash ) )
     {
       KMessageBox::error( 0, i18n( "Password is incorrect." ) );
       d->actions->protectSheet->setChecked( true );
       return;
     }

     d->activeSheet->setProtected( QByteArray() );
   }
   doc()->setModified( true );
   d->adjustActions( !mode );
   doc()->emitBeginOperation();
   // d->activeSheet->setRegionPaintDirty( QRect(QPoint( 0, 0 ), QPoint( KS_colMax, KS_rowMax ) ) );
   refreshView();
   updateEditWidget();
   doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::togglePageBorders( bool mode )
{
  if ( !d->activeSheet )
    return;

  doc()->emitBeginOperation( false );
  d->activeSheet->setShowPageBorders( mode );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::viewZoom( const QString & s )
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

    if (activeSheet())
    {
    	QRect r( d->activeSheet->visibleRect( d->canvas ) );
    	r.setWidth( r.width() + 2 );
    	doc()->emitEndOperation( r );
    }
  }
}

void View::setZoom( int zoom, bool /*updateViews*/ )
{
  kDebug() << "---------SetZoom: " << zoom << endl;

  // Set the zoom in KoView (for embedded views)
  doc()->emitBeginOperation( false );

  doc()->setZoomAndResolution( zoom, KoGlobal::dpiX(), KoGlobal::dpiY());
  //KoView::setZoom( doc()->zoomedResolutionY() /* KoView only supports one zoom */ );

  Q_ASSERT(d->activeSheet);

  if (d->activeSheet)  //this is 0 when viewing a document in konqueror!? (see Q_ASSERT above)
    d->activeSheet->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));

  doc()->refreshInterface();
  doc()->emitEndOperation();
}

void View::showStatusBar( bool b )
{
  doc()->setShowStatusBar( b );
  refreshView();
}

void View::showTabBar( bool b )
{
  doc()->setShowTabBar( b );
  refreshView();
}

void View::showFormulaBar( bool b )
{
  doc()->setShowFormulaBar( b );
  refreshView();
}

void View::preference()
{
  if ( !d->activeSheet )
    return;

  PreferenceDialog dlg( this, "Preference" );
  if ( dlg.exec() )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->refreshPreference();
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void View::addModifyComment()
{
  if ( !d->activeSheet )
    return;

  CommentDialog dlg( this, "comment",
                     QPoint( d->canvas->markerColumn(),
                             d->canvas->markerRow() ) );
  if ( dlg.exec() )
    updateEditWidget();
}

void View::setSelectionComment( QString comment )
{
  if ( d->activeSheet != NULL )
  {
    doc()->emitBeginOperation( false );

    d->activeSheet->setSelectionComment( selectionInfo(), comment.trimmed() );
    updateEditWidget();

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::editCell()
{
  if ( d->canvas->editor() )
    return;

  d->canvas->createEditor(true);
}

bool View::showSheet(const QString& sheetName) {
  Sheet *t=doc()->map()->findSheet(sheetName);
  if ( !t )
  {
    kDebug(36001) << "Unknown sheet " <<sheetName<<  endl;
    return false;
  }
  d->canvas->closeEditor();
  setActiveSheet( t );

  return true;
}

void View::nextSheet()
{
  Sheet * t = doc()->map()->nextSheet( activeSheet() );
  if ( !t )
  {
    kDebug(36001) << "Unknown sheet " <<  endl;
    return;
  }
  d->canvas->closeEditor();
  setActiveSheet( t );
  d->tabBar->setActiveTab( t->sheetName() );
  d->tabBar->ensureVisible( t->sheetName() );
}

void View::previousSheet()
{
  Sheet * t = doc()->map()->previousSheet( activeSheet() );
  if ( !t )
  {
    kDebug(36001) << "Unknown sheet "  << endl;
    return;
  }
  d->canvas->closeEditor();
  setActiveSheet( t );
  d->tabBar->setActiveTab( t->sheetName() );
  d->tabBar->ensureVisible( t->sheetName() );
}

void View::firstSheet()
{
  Sheet *t = doc()->map()->firstSheet();
  if ( !t )
  {
    kDebug(36001) << "Unknown sheet " <<  endl;
    return;
  }
  d->canvas->closeEditor();
  setActiveSheet( t );
  d->tabBar->setActiveTab( t->sheetName() );
  d->tabBar->ensureVisible( t->sheetName() );
}

void View::lastSheet()
{
  Sheet *t = doc()->map()->lastSheet( );
  if ( !t )
  {
    kDebug(36001) << "Unknown sheet " <<  endl;
    return;
  }
  d->canvas->closeEditor();
  setActiveSheet( t );
  d->tabBar->setActiveTab( t->sheetName() );
  d->tabBar->ensureVisible( t->sheetName() );
}

void View::keyPressEvent ( QKeyEvent* _ev )
{
  // Dont eat accelerators
  if ( _ev->modifiers() & ( Qt::AltModifier | Qt::ControlModifier ) )
  {
    if ( _ev->modifiers() & ( Qt::ControlModifier ) )
    {
      switch( _ev->key() )
      {
#ifndef NDEBUG
       case Qt::Key_V: // Ctrl+Shift+V to show debug (similar to KWord)
        if ( _ev->modifiers() & Qt::ShiftModifier )
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

KoDocument * View::hitTest( const QPoint& /*pos*/ )
{
//     // Code copied from KoView::hitTest
//     KoViewChild *viewChild;
//
//     QMatrix m = matrix();
//     m.translate( d->canvas->xOffset() / doc()->zoomedResolutionX(),
//                  d->canvas->yOffset() / doc()->zoomedResolutionY() );
//
//     KoDocumentChild *docChild = selectedChild();
//     if ( docChild )
//     {
//         if ( ( viewChild = child( docChild->document() ) ) )
//         {
//             if ( viewChild->frameRegion( m ).contains( pos ) )
//                 return 0;
//         }
//         else
//             if ( docChild->frameRegion( m ).contains( pos ) )
//                 return 0;
//     }
//
//     docChild = activeChild();
//     if ( docChild )
//     {
//         if ( ( viewChild = child( docChild->document() ) ) )
//         {
//             if ( viewChild->frameRegion( m ).contains( pos ) )
//                 return 0;
//         }
//         else
//             if ( docChild->frameRegion( m ).contains( pos ) )
//                 return 0;
//     }
//
//     QPtrListIterator<KoDocumentChild> it( doc()->children() );
//     for (; it.current(); ++it )
//     {
//         // Is the child document on the visible sheet ?
//         if ( ((EmbeddedKOfficeObject*)it.current())->sheet() == d->activeSheet )
//         {
//             KoDocument *doc = it.current()->hitTest( pos, m );
//             if ( doc )
//                 return doc;
//         }
//     }
//
  return doc();
}

int View::leftBorder() const
{
  return int( d->canvas->doc()->zoomItX( YBORDER_WIDTH ) );
}

int View::rightBorder() const
{
  return d->vertScrollBar->width();
}

int View::topBorder() const
{
  return d->toolWidget->height() + int( d->canvas->doc()->zoomItX( Format::globalRowHeight() + 2 ) );
}

int View::bottomBorder() const
{
  return d->horzScrollBar->height();
}

void View::refreshView()
{
  kDebug() << "refreshing view" << endl;

  Sheet * sheet = activeSheet();
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

  d->tabBar->setReadOnly( !doc()->isReadWrite() || doc()->map()->isProtected() );

  d->toolWidget->setShown( doc()->showFormulaBar() );
  d->editWidget->showEditWidget( doc()->showFormulaBar() );
  d->hBorderWidget->setShown( doc()->showColumnHeader() );
  d->vBorderWidget->setShown( doc()->showRowHeader() );
  d->vertScrollBar->setShown( doc()->showVerticalScrollBar() );
  d->horzScrollBar->setShown( doc()->showHorizontalScrollBar() );
  d->tabBar->setShown( doc()->showTabBar() );
  if ( statusBar() ) statusBar()->setShown( doc()->showStatusBar() );

  d->canvas->updatePosWidget();

  d->hBorderWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  d->hBorderWidget->setMinimumHeight( doc()->zoomItY( Format::globalRowHeight() + 2 ) );
  d->vBorderWidget->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding );
  d->vBorderWidget->setMinimumWidth( doc()->zoomItX( YBORDER_WIDTH ) );

  Sheet::LayoutDirection sheetDir = sheet->layoutDirection();
  bool interfaceIsRTL = QApplication::isRightToLeft();

  kDebug()<<" sheetDir == Sheet::LeftToRight :"<<( sheetDir == Sheet::LeftToRight )<<endl;
  if ((sheetDir == Sheet::LeftToRight && !interfaceIsRTL) ||
      (sheetDir == Sheet::RightToLeft && interfaceIsRTL))
  {
    d->formulaBarLayout->setDirection( Q3BoxLayout::LeftToRight );
    d->viewLayout->setOrigin( Qt::TopLeftCorner );
    d->tabScrollBarLayout->setDirection( Q3BoxLayout::LeftToRight );
    d->tabBar->setReverseLayout( interfaceIsRTL );
  }
  else
  {
    d->formulaBarLayout->setDirection( Q3BoxLayout::RightToLeft );
    d->viewLayout->setOrigin( Qt::TopRightCorner );
    d->tabScrollBarLayout->setDirection( Q3BoxLayout::RightToLeft );
    d->tabBar->setReverseLayout( !interfaceIsRTL );
  }

}

void View::resizeEvent( QResizeEvent * )
{
  refreshView();
}

void View::popupChildMenu( KoChild* child, const QPoint& /*global_pos*/ )
{
    if ( !child )
  return;

    delete d->popupChild;

//     d->popupChildObject = static_cast<EmbeddedKOfficeObject*>(child);
//
//     d->popupChild = new QPopupMenu( this );
//
//     d->popupChild->insertItem( i18n("Delete Embedded Document"), this, SLOT( slotPopupDeleteChild() ) );
//
//     d->popupChild->popup( global_pos );

}

void View::slotPopupDeleteChild()
{
//     if ( !d->popupChildObject || !d->popupChildObject->sheet() )
//   return;

    //Removed popup warning dialog because
    // a) It is annoying from a user's persepective
    // b) The behaviour should be consistant with other KOffice apps

    /*int ret = KMessageBox::warningContinueCancel(this,i18n("You are about to remove this embedded document.\nDo you want to continue?"),i18n("Delete Embedded Document"),KGuiItem(i18n("&Delete"),"editdelete"));
    if ( ret == KMessageBox::Continue )
    {

}*/
//     doc()->emitBeginOperation(false);
//     d->popupChildObject->sheet()->deleteChild( d->popupChildObject );
//     d->popupChildObject = 0;
//     doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::popupColumnMenu( const QPoint & _point )
{
  assert( d->activeSheet );

  if ( !koDocument()->isReadWrite() )
    return;

    delete d->popupColumn ;

    d->popupColumn = new Q3PopupMenu( this );

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
      if (!d->selection->isColumnOrRowSelected())
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

      ColumnFormat* format;
      //kDebug(36001) << "Column: L: " << rect.left() << endl;
      Region::ConstIterator endOfList = d->selection->constEnd();
      for (Region::ConstIterator it = d->selection->constBegin(); it != endOfList; ++it)
      {
        QRect range = (*it)->rect().normalize();
        int col;
        for (col = range.left(); col < range.right(); ++col)
        {
          format = activeSheet()->columnFormat(col);

          if ( format->isHide() )
          {
            d->actions->showSelColumns->setEnabled( true );
            d->actions->showSelColumns->plug( d->popupColumn );
            break;
          }
        }
        if (range.left() > 1 && col == range.right())
        {
          bool allHidden = true;
          for (col = 1; col < range.left(); ++col)
          {
            format = activeSheet()->columnFormat(col);

            allHidden &= format->isHide();
          }
          if (allHidden)
          {
            d->actions->showSelColumns->setEnabled( true );
            d->actions->showSelColumns->plug( d->popupColumn );
            break;
          }
        }
        else
        {
          break;
        }
      }
    }

    QObject::connect( d->popupColumn, SIGNAL(activated( int ) ), this, SLOT( slotActivateTool( int ) ) );

    d->popupColumn->popup( _point );
}

void View::slotPopupAdjustColumn()
{
  if ( !d->activeSheet )
      return;

  d->activeSheet->adjustColumn(*selectionInfo());
}

void View::popupRowMenu( const QPoint & _point )
{
    assert( d->activeSheet );

    if ( !koDocument()->isReadWrite() )
      return;

    delete d->popupRow ;

    d->popupRow= new Q3PopupMenu();

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
      if (!d->selection->isColumnOrRowSelected())
      {
        d->actions->areaName->plug(d->popupRow);
      }

      d->actions->resizeRow->plug( d->popupRow );
      d->popupRow->insertItem( i18n("Adjust Row"), this, SLOT( slotPopupAdjustRow() ) );
      d->popupRow->insertSeparator();
      d->actions->insertRow->plug( d->popupRow );
      d->actions->deleteRow->plug( d->popupRow );
      d->actions->hideRow->plug( d->popupRow );

      d->actions->showSelColumns->setEnabled(false);

      RowFormat* format;
      Region::ConstIterator endOfList = d->selection->constEnd();
      for (Region::ConstIterator it = d->selection->constBegin(); it != endOfList; ++it)
      {
        QRect range = (*it)->rect().normalize();
        int row;
        for (row = range.top(); row < range.bottom(); ++row)
        {
          format = activeSheet()->rowFormat(row);

          if ( format->isHide() )
          {
            d->actions->showSelRows->setEnabled( true );
            d->actions->showSelRows->plug( d->popupRow );
            break;
          }
        }
        if (range.top() > 1 && row == range.bottom())
        {
          bool allHidden = true;
          for (row = 1; row < range.top(); ++row)
          {
            format = activeSheet()->rowFormat(row);

            allHidden &= format->isHide();
          }
          if (allHidden)
          {
            d->actions->showSelRows->setEnabled( true );
            d->actions->showSelRows->plug( d->popupRow );
            break;
          }
        }
        else
        {
          break;
        }
      }
    }

    QObject::connect( d->popupRow, SIGNAL( activated( int ) ), this, SLOT( slotActivateTool( int ) ) );
    d->popupRow->popup( _point );
}

void View::slotPopupAdjustRow()
{
  if ( !d->activeSheet )
      return;

  d->activeSheet->adjustRow(*selectionInfo());
}


void View::slotListChoosePopupMenu( )
{
  if ( !koDocument()->isReadWrite() )
    return;

  assert( d->activeSheet );
  delete d->popupListChoose;

  d->popupListChoose = new Q3PopupMenu();
  int id = 0;
  QRect selection( d->selection->selection() );
  Cell * cell = d->activeSheet->cellAt( d->canvas->markerColumn(), d->canvas->markerRow() );
  QString tmp = cell->text();
  QStringList itemList;

  for ( int col = selection.left(); col <= selection.right(); ++col )
  {
    Cell * c = d->activeSheet->getFirstCellColumn( col );
    while ( c )
    {
      if ( !c->isPartOfMerged()
           && !( col == d->canvas->markerColumn()
                 && c->row() == d->canvas->markerRow()) )
      {
        if ( c->value().isString() && c->text() != tmp && !c->text().isEmpty() )
        {
          if ( itemList.indexOf( c->text() ) == -1 )
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
    &&!c->isPartOfMerged()&& !(col==d->canvas->markerColumn()&& c->row()==d->canvas->markerRow()))
       {
   if (c->isString() && c->text()!=tmp && !c->text().isEmpty())
     {
       if (itemList.indexOf(c->text())==-1)
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

  if ( d->activeSheet->layoutDirection()==Sheet::RightToLeft )
  {
    tx = canvasWidget()->width() - tx;
  }

  QPoint p( (int)tx, (int)ty );
  QPoint p2 = d->canvas->mapToGlobal( p );

  if ( d->activeSheet->layoutDirection()==Sheet::RightToLeft )
  {
    p2.setX( p2.x() - d->popupListChoose->sizeHint().width() + 1 );
  }

  d->popupListChoose->popup( p2 );
  QObject::connect( d->popupListChoose, SIGNAL( activated( int ) ),
                    this, SLOT( slotItemSelected( int ) ) );
}


void View::slotItemSelected( int id )
{
  QString tmp = d->popupListChoose->text( id );
  int x = d->canvas->markerColumn();
  int y = d->canvas->markerRow();
  Cell * cell = d->activeSheet->nonDefaultCell( x, y );

  if ( tmp == cell->text() )
    return;

  doc()->emitBeginOperation( false );

  if ( !doc()->undoLocked() )
  {
    UndoSetText* undo = new UndoSetText( doc(), d->activeSheet, cell->text(),
                                                       x, y, cell->formatType() );
    doc()->addCommand( undo );
  }

  cell->setCellText( tmp );
  d->editWidget->setText( tmp );

  doc()->emitEndOperation( QRect( x, y, 1, 1 ) );
}

void View::openPopupMenu( const QPoint & _point )
{
    assert( d->activeSheet );
    delete d->popupMenu;

    if ( !koDocument()->isReadWrite() )
        return;

    d->popupMenu = new Q3PopupMenu();

    EmbeddedObject *obj;
    if ( d->canvas->isObjectSelected() && ( obj = d->canvas->getObject( d->canvas->mapFromGlobal( _point ), d->activeSheet ) ) && obj->isSelected() )
    {
      d->actions->deleteCell->plug( d->popupMenu );
      d->popupMenu->insertSeparator();
      d->actions->cut->plug( d->popupMenu );
      d->actions->copy->plug( d->popupMenu );
      d->actions->paste->plug( d->popupMenu );
      d->popupMenu->popup( _point );
      d->popupMenu->insertSeparator();
      d->actions->actionExtraProperties->plug( d->popupMenu );
      return;
    }

    Cell * cell = d->activeSheet->cellAt( d->canvas->markerColumn(), d->canvas->markerRow() );

    bool isProtected = d->activeSheet->isProtected();
    if ( !cell->isDefault() && cell->format()->notProtected( d->canvas->markerColumn(), d->canvas->markerRow() )
         && d->selection->isSingular() )
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
      if (!d->selection->isColumnOrRowSelected())
      {
        d->actions->areaName->plug( d->popupMenu );
        d->popupMenu->insertSeparator();
        d->actions->insertCell->plug( d->popupMenu );
        d->actions->removeCell->plug( d->popupMenu );
      }

      d->popupMenu->insertSeparator();
      d->actions->addModifyComment->plug( d->popupMenu );
      if ( !cell->format()->comment(d->canvas->markerColumn(), d->canvas->markerRow()).isEmpty() )
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
      Q3ValueList<KDataToolInfo> tools = KDataToolInfo::query( "QString", "text/plain", doc()->instance() );
      if ( tools.count() > 0 )
      {
        d->popupMenu->insertSeparator();
        Q3ValueList<KDataToolInfo>::Iterator entry = tools.begin();
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
            Private::ToolEntry *t = new Private::ToolEntry;
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

void View::slotActivateTool( int _id )
{
  if (!activeSheet()) return;

  // Is it the id of a tool in the latest popupmenu ?
  if ( _id < d->popupMenuFirstToolId )
    return;

  Private::ToolEntry* entry = d->toolList.at( _id - d->popupMenuFirstToolId );

  KDataTool* tool = entry->info.createTool();
  if ( !tool )
  {
      kDebug(36001) << "Could not create Tool" << endl;
      return;
  }

  QString text = activeSheet()->getWordSpelling( selectionInfo() );

  if ( tool->run( entry->command, &text, "QString", "text/plain") )
  {
      doc()->emitBeginOperation(false);

      activeSheet()->setWordSpelling( selectionInfo(), text);

      Cell *cell = d->activeSheet->cellAt( d->canvas->markerColumn(), d->canvas->markerRow() );
      d->editWidget->setText( cell->text() );

      doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void View::deleteSelection()
{
    if (!activeSheet()) return;

    if ( canvasWidget()->isObjectSelected() )
    {
      deleteSelectedObjects();
      return;
    }

    doc()->emitBeginOperation( false );
    d->activeSheet->deleteSelection( selectionInfo() );
    calcStatusBarOp();
    updateEditWidget();

    markSelectionAsDirty();
    doc()->emitEndOperation();
}

void View::deleteSelectedObjects()
{
  KMacroCommand * macroCommand = 0L;
  Q3PtrListIterator<EmbeddedObject> it( doc()->embeddedObjects() );
  for ( ; it.current() ; ++it )
  {
    if ( it.current()->sheet() == canvasWidget()->activeSheet() && it.current()->isSelected() )
    {
     // d->activeSheet->setRegionPaintDirty( it.
      if( !macroCommand )
        macroCommand = new KMacroCommand( i18n( "Remove Object" ) );
      RemoveObjectCommand *cmd = new RemoveObjectCommand( it.current() );
      macroCommand->addCommand( cmd );
    }
  }
  if ( macroCommand )
  {
    doc()->addCommand( macroCommand );
    canvasWidget()->setMouseSelectedObject( false );
    macroCommand->execute();
  }
}

void View::adjust()
{
  if ( !d->activeSheet )
    return;

  d->activeSheet->adjustArea(*selectionInfo());
}

void View::clearTextSelection()
{
    if (!activeSheet())
	    return;

    doc()->emitBeginOperation( false );
    d->activeSheet->clearTextSelection( selectionInfo() );

    updateEditWidget();

    markSelectionAsDirty();
    doc()->emitEndOperation();
}

void View::clearCommentSelection()
{
    if (!activeSheet())
	    return;

    doc()->emitBeginOperation( false );
    d->activeSheet->setSelectionRemoveComment( selectionInfo() );

    updateEditWidget();

    markSelectionAsDirty();
    doc()->emitEndOperation();
}

void View::clearValiditySelection()
{
    if (!activeSheet())
	    return;

    doc()->emitBeginOperation( false );
    d->activeSheet->clearValiditySelection( selectionInfo() );

    updateEditWidget();

    markSelectionAsDirty();
    doc()->emitEndOperation();
}

void View::clearConditionalSelection()
{
    if (!activeSheet())
	    return;

    doc()->emitBeginOperation( false );
    d->activeSheet->clearConditionalSelection( selectionInfo() );

    updateEditWidget();

    markSelectionAsDirty();
    doc()->emitEndOperation();
}

void View::fillRight()
{
  if (!activeSheet())
	  return;

  doc()->emitBeginOperation( false );
  d->activeSheet->fillSelection( selectionInfo(), Sheet::Right );

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::fillLeft()
{
  if (!activeSheet())
	  return;

  doc()->emitBeginOperation( false );
  d->activeSheet->fillSelection( selectionInfo(), Sheet::Left );

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::fillUp()
{
  if (!activeSheet())
	  return;

  doc()->emitBeginOperation( false );
  d->activeSheet->fillSelection( selectionInfo(), Sheet::Up );

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::fillDown()
{
  if (!activeSheet())
	  return;

  doc()->emitBeginOperation( false );
  d->activeSheet->fillSelection( selectionInfo(), Sheet::Down );

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::defaultSelection()
{
  if (!activeSheet())
	return;

  doc()->emitBeginOperation( false );
  d->activeSheet->defaultSelection( selectionInfo() );

  updateEditWidget();

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::slotInsert()
{
  QRect r( d->selection->selection() );
  InsertDialog dlg( this, "InsertDialog", r, InsertDialog::Insert );
  dlg.exec();
}

void View::slotRemove()
{
  QRect r( d->selection->selection() );
  InsertDialog dlg( this, "Remove", r, InsertDialog::Remove );
  dlg.exec();
}

void View::slotInsertCellCopy()
{
  if ( !d->activeSheet )
    return;

  if ( !d->activeSheet->testAreaPasteInsert() )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->paste( d->selection->lastRange(), true,
                           Paste::Normal, Paste::OverWrite, true );
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
  else
  {
    PasteInsertDialog dlg( this, "Remove", d->selection->selection() );
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

void View::setAreaName()
{
  AreaDialog dlg( this, "Area Name",QPoint(d->canvas->markerColumn(), d->canvas->markerRow()) );
  dlg.exec();
}

void View::showAreaName()
{
  reference dlg( this, "Show Area" );
  dlg.exec();
}

void View::resizeRow()
{
   if (!activeSheet()) return;

  if ( d->selection->isColumnSelected() )
    KMessageBox::error( this, i18n("Area is too large."));
  else
  {
    ResizeRow dlg( this );
    dlg.exec();
  }
}

void View::resizeColumn()
{
  if (!activeSheet()) return;


  if ( d->selection->isRowSelected() )
    KMessageBox::error( this, i18n( "Area is too large." ) );
  else
  {
    ResizeColumn dlg( this );
    dlg.exec();
  }
}

void View::equalizeRow()
{
  if (!activeSheet()) return;

  if ( d->selection->isColumnSelected() )
    KMessageBox::error( this, i18n( "Area is too large." ) );
  else
  {
    doc()->emitBeginOperation( false );
    canvasWidget()->equalizeRow();
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}

void View::equalizeColumn()
{
  if (!activeSheet())
	  return;

  if ( d->selection->isRowSelected() )
    KMessageBox::error( this, i18n( "Area is too large." ) );
  else
  {
    doc()->emitBeginOperation( false );
    canvasWidget()->equalizeColumn();
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}


void View::layoutDlg()
{
  if (!activeSheet())
	  return;

  CellFormatDialog dlg( this, d->activeSheet );
}

void View::extraProperties()
{
    if (!activeSheet())
	    return;
    //d->canvas->setToolEditMode( TEM_MOUSE );

    d->m_propertyEditor = new PropertyEditor( this, "KPrPropertyEditor", d->activeSheet, doc() );
    d->m_propertyEditor->setCaption( i18n( "Properties" ) );

    connect( d->m_propertyEditor, SIGNAL( propertiesOk() ), this, SLOT( propertiesOk() ) );
    d->m_propertyEditor->exec();
    disconnect( d->m_propertyEditor, SIGNAL( propertiesOk() ), this, SLOT( propertiesOk() ) );

    delete d->m_propertyEditor;
    d->m_propertyEditor = 0;
}

void View::propertiesOk()
{
    KCommand *cmd = d->m_propertyEditor->getCommand();

    if ( cmd )
    {
        cmd->execute();
        doc()->addCommand( cmd );
    }
}

void View::styleDialog()
{
  StyleDlg dlg( this, doc()->styleManager() );
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

void View::paperLayoutDlg()
{
  if ( d->canvas->editor() )
  {
    d->canvas->deleteEditor( true ); // save changes
  }
  SheetPrint* print = d->activeSheet->print();

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

  PaperLayout * dlg
    = new PaperLayout( this, "PageLayout", pl, hf,
                              FORMAT_AND_BORDERS | HEADER_AND_FOOTER,
                              unit, d->activeSheet, this );
  dlg->show();
  // dlg destroys itself
}

void View::definePrintRange()
{
  d->activeSheet->print()->definePrintRange( selectionInfo() );
}

void View::resetPrintRange()
{
  d->activeSheet->print()->resetPrintRange();
}

void View::wrapText( bool b )
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

void View::alignLeft( bool b )
{
  if ( d->toolbarLock )
    return;

  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( !b )
      d->activeSheet->setSelectionAlign( selectionInfo(),
                                   Style::HAlignUndefined );
    else
      d->activeSheet->setSelectionAlign( selectionInfo(),
                                   Style::Left );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::alignRight( bool b )
{
  if ( d->toolbarLock )
    return;

  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( !b )
      d->activeSheet->setSelectionAlign( selectionInfo(), Style::HAlignUndefined );
    else
      d->activeSheet->setSelectionAlign( selectionInfo(), Style::Right );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::alignCenter( bool b )
{
  if ( d->toolbarLock )
    return;

  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( !b )
      d->activeSheet->setSelectionAlign( selectionInfo(), Style::HAlignUndefined );
    else
      d->activeSheet->setSelectionAlign( selectionInfo(), Style::Center );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::alignTop( bool b )
{
  if ( d->toolbarLock )
    return;

  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( !b )
      d->activeSheet->setSelectionAlignY( selectionInfo(), Style::VAlignUndefined );
    else
      d->activeSheet->setSelectionAlignY( selectionInfo(), Style::Top );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::alignBottom( bool b )
{
  if ( d->toolbarLock )
    return;

  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( !b )
      d->activeSheet->setSelectionAlignY( selectionInfo(), Style::VAlignUndefined );
    else
      d->activeSheet->setSelectionAlignY( selectionInfo(), Style::Bottom );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::alignMiddle( bool b )
{
  if ( d->toolbarLock )
    return;

  if ( d->activeSheet != 0L )
  {
    doc()->emitBeginOperation( false );
    if ( !b )
      d->activeSheet->setSelectionAlignY( selectionInfo(), Style::VAlignUndefined );
    else
      d->activeSheet->setSelectionAlignY( selectionInfo(), Style::Middle );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::moneyFormat(bool b)
{
  if ( d->toolbarLock )
    return;

  doc()->emitBeginOperation( false );
  if ( d->activeSheet != 0L )
    d->activeSheet->setSelectionMoneyFormat( selectionInfo(), b );
  updateEditWidget();

  markSelectionAsDirty();
  doc()->emitEndOperation();
}

void View::createStyleFromCell()
{
  if ( !d->activeSheet )
    return;

  QPoint p( d->selection->selection().topLeft() );
  Cell * cell = d->activeSheet->nonDefaultCell( p.x(), p.y() );

  bool ok = false;
  QString styleName( "" );

  while( true )
  {
    styleName = KInputDialog::getText( i18n( "Create Style From Cell" ),
                                       i18n( "Enter name:" ), styleName, &ok, this );

    if ( !ok ) // User pushed an OK button.
      return;

    styleName = styleName.trimmed();

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

  CustomStyle * style = new CustomStyle( cell->format()->style(), styleName );

  doc()->styleManager()->m_styles[ styleName ] = style;
  cell->format()->setStyle( style );
  QStringList lst( d->actions->selectStyle->items() );
  lst.push_back( styleName );
  d->actions->selectStyle->setItems( lst );
}

void View::styleSelected( const QString & style )
{
  if (d->activeSheet )
  {
    Style * s = doc()->styleManager()->style( style );

    if ( s )
    {
      doc()->emitBeginOperation(false);
      d->activeSheet->setSelectionStyle( selectionInfo(), s );

      markSelectionAsDirty();
      doc()->emitEndOperation();
    }
  }
}

void View::precisionPlus()
{
  setSelectionPrecision( 1 );
}

void View::precisionMinus()
{
  setSelectionPrecision( -1 );
}

void View::setSelectionPrecision( int delta )
{
  if ( d->activeSheet != NULL )
  {
    doc()->emitBeginOperation( false );
    d->activeSheet->setSelectionPrecision( selectionInfo(), delta );

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::percent( bool b )
{
  if ( d->toolbarLock )
    return;

  doc()->emitBeginOperation( false );
  if ( d->activeSheet != 0L )
    d->activeSheet->setSelectionPercent( selectionInfo() ,b );
  updateEditWidget();

  markSelectionAsDirty();
  doc()->emitEndOperation();
}


void View::insertObject()
{
  if (!activeSheet())
	  return;

  doc()->emitBeginOperation( false );
  KoDocumentEntry e =  d->actions->insertPart->documentEntry();//KoPartSelectDia::selectPart( d->canvas );
  if ( e.isEmpty() )
  {
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
    return;
  }

  //Don't start handles more than once
  delete d->insertHandler;

  d->insertHandler = new InsertPartHandler( this, d->canvas, e );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::insertChart()
{
  if (!activeSheet())
	  return;

  if ( d->selection->isColumnOrRowSelected() )
  {
    KMessageBox::error( this, i18n("Area too large."));
    return;
  }
  Q3ValueList<KoDocumentEntry> vec = KoDocumentEntry::query( true, "'KOfficeChart' in ServiceTypes" );
  if ( vec.isEmpty() )
  {
    KMessageBox::error( this, i18n("No charting component registered.") );
    return;
  }

  //Don't start handles more than once
  delete d->insertHandler;

  doc()->emitBeginOperation( false );

  d->insertHandler = new InsertChartHandler( this, d->canvas, vec[0] );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}



/*
  // TODO Use KoView setScaling/xScaling/yScaling instead
void View::zoomMinus()
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

void View::zoomPlus()
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

void View::removeSheet()
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
    Sheet * tbl = activeSheet();
    KCommand* command = new RemoveSheetCommand( tbl );
    doc()->addCommand( command );
    command->execute();


#if 0
    UndoRemoveSheet * undo = new UndoRemoveSheet( doc(), tbl );
    doc()->addCommand( undo );
    tbl->doc()->map()->takeSheet( tbl );
    doc()->takeSheet( tbl );
#endif
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
  }
}


void View::slotRename()
{

  Sheet * sheet = activeSheet();

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

    newName = newName.simplified();
    int n = newName.indexOf('-');
    if ( n > -1 ) newName[n] = '_';
    n = newName.indexOf('!');
    if ( n > -1 ) newName[n] = '_';
    n = newName.indexOf('$');
    if ( n > -1 ) newName[n] = '_';

    newName = KInputDialog::getText( i18n("Rename Sheet"),i18n("Enter name:"), newName, &ok, this );

    if ( !ok ) return;
  }

  if ( (newName.trimmed()).isEmpty() ) // Sheet name is empty.
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

void View::setText (const QString & _text, bool array)
{
  if ( d->activeSheet == 0L )
    return;

  if (array) {
    // array version
    d->activeSheet->setArrayFormula (d->selection, _text);
  }
  else
  {
    // non-array version
    int x = d->canvas->markerColumn();
    int y = d->canvas->markerRow();

    d->activeSheet->setText( y, x, _text );

    Cell * cell = d->activeSheet->cellAt( x, y );
    if ( cell->value().isString() && !_text.isEmpty() && !_text.at(0).isDigit() && !cell->isFormula() )
      doc()->addStringCompletion( _text );
  }
}

//------------------------------------------------
//
// Document signals
//
//------------------------------------------------

void View::slotAddSheet( Sheet *_sheet )
{
  addSheet( _sheet );
}

void View::slotRefreshView()
{
  refreshView();
  d->canvas->repaint();
  d->vBorderWidget->repaint();
  d->hBorderWidget->repaint();
}

void View::slotUpdateView( Sheet *_sheet )
{
  // Do we display this sheet ?
  if ( ( !activeSheet() ) || ( _sheet != d->activeSheet ) )
    return;

  d->activeSheet->setRegionPaintDirty( d->activeSheet->visibleRect( d->canvas ) );
  doc()->emitEndOperation();
}

void View::slotUpdateView( Sheet * _sheet, const Region& region )
{
  // qDebug("void View::slotUpdateView( Sheet *_sheet, const QRect& %i %i|%i %i )\n",_rect.left(),_rect.top(),_rect.right(),_rect.bottom());

  // Do we display this sheet ?
  if ( _sheet != d->activeSheet )
    return;

  // doc()->emitBeginOperation( false );
  d->activeSheet->setRegionPaintDirty( region );
  doc()->emitEndOperation( region );
}

void View::slotUpdateView( EmbeddedObject *obj )
{
  d->canvas->repaintObject( obj );
}

void View::slotUpdateHBorder( Sheet * _sheet )
{
  // kDebug(36001)<<"void View::slotUpdateHBorder( Sheet *_sheet )\n";

  // Do we display this sheet ?
  if ( _sheet != d->activeSheet )
    return;

  doc()->emitBeginOperation(false);
  d->hBorderWidget->update();
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::slotUpdateVBorder( Sheet *_sheet )
{
  // kDebug("void View::slotUpdateVBorder( Sheet *_sheet )\n";

  // Do we display this sheet ?
  if ( _sheet != d->activeSheet )
    return;

  doc()->emitBeginOperation( false );
  d->vBorderWidget->update();
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::slotChangeSelection(const KSpread::Region& changedRegion)
{
//   kDebug() << *selectionInfo() << endl;

  if (!changedRegion.isValid())
  {
    return;
  }

  doc()->emitBeginOperation( false );

  bool colSelected = d->selection->isColumnSelected();
  bool rowSelected = d->selection->isRowSelected();
  if (d->activeSheet && !d->activeSheet->isProtected())
  {
    // Activate or deactivate some actions.
    d->actions->resizeRow->setEnabled( !colSelected );
    d->actions->equalizeRow->setEnabled( !colSelected );
    d->actions->hideRow->setEnabled( !colSelected );
    d->actions->validity->setEnabled( !colSelected && !rowSelected);
    d->actions->conditional->setEnabled( !colSelected && !rowSelected);
    d->actions->resizeColumn->setEnabled( !rowSelected );
    d->actions->equalizeColumn->setEnabled( !rowSelected );
    d->actions->hideColumn->setEnabled( !rowSelected );
    d->actions->textToColumns->setEnabled( !rowSelected );

    bool simpleSelection = d->selection->isSingular() || colSelected || rowSelected;
    d->actions->autoFormat->setEnabled( !simpleSelection );
    d->actions->sort->setEnabled( !simpleSelection );
    d->actions->mergeCell->setEnabled( !simpleSelection );
    d->actions->mergeCellHorizontal->setEnabled( !simpleSelection );
    d->actions->mergeCellVertical->setEnabled( !simpleSelection );
    d->actions->fillRight->setEnabled( !simpleSelection );
    d->actions->fillUp->setEnabled( !simpleSelection );
    d->actions->fillDown->setEnabled( !simpleSelection );
    d->actions->fillLeft->setEnabled( !simpleSelection );
    d->actions->insertChartFrame->setEnabled( !simpleSelection );
    d->actions->sortDec->setEnabled( !simpleSelection );
    d->actions->sortInc->setEnabled( !simpleSelection);
    d->actions->createStyle->setEnabled( simpleSelection ); // just from one cell

    bool contiguousSelection = d->selection->isContiguous();
    d->actions->subTotals->setEnabled(contiguousSelection);
  }
  d->actions->selectStyle->setCurrentItem( -1 );
  // delayed recalculation of the operation shown in the status bar
  d->statusBarOpTimer.start(250, true);
  // Send some event around. This is read for example
  // by the calculator plugin.
//   SelectionChanged ev(*selectionInfo(), activeSheet()->name());
//   QApplication::sendEvent( this, &ev );

  d->canvas->setSelectionChangePaintDirty( d->activeSheet, changedRegion );
  d->vBorderWidget->update();
  d->hBorderWidget->update();

  if (colSelected || rowSelected)
  {
    doc()->emitEndOperation(/* *selectionInfo() */);
    return;
  }

  d->canvas->validateSelection();

  //Don't scroll to the marker if there is an active embedded object, since this may cause
  //the canvas to scroll so that the object isn't in the visible area.
  //There is still the problem of the object no longer being visible immediately after deactivating the child
  //as the sheet jumps back to the marker though.
  if (!activeChild())
  	d->canvas->scrollToCell(selectionInfo()->marker());

  // Perhaps the user is entering a value in the cell.
  // In this case we may not touch the EditWidget
  if ( !d->canvas->editor() && !d->canvas->chooseMode() )
  {
    updateEditWidgetOnPress();
  }
  d->canvas->updatePosWidget();

  doc()->emitEndOperation(/* *selectionInfo() */);
}

void View::slotChangeChoice(const KSpread::Region& changedRegion)
{
  if (!changedRegion.isValid())
  {
    return;
  }
  doc()->emitBeginOperation( false );
  d->canvas->updateEditor();
  d->canvas->setSelectionChangePaintDirty( d->activeSheet, changedRegion );
  d->canvas->scrollToCell(choice()->marker());
  doc()->emitEndOperation( *choice() );
  kDebug() << "Choice: " << *choice() << endl;
}

void View::calcStatusBarOp()
{
  Sheet * sheet = activeSheet();
  ValueCalc* calc = d->doc->calc();
  Value val;
  QRect tmpRect(d->selection->selection());
  MethodOfCalc tmpMethod = doc()->getTypeOfCalc();
  if ( tmpMethod != NoneCalc )
  {

    Value range = sheet->valueRange (tmpRect.left(), tmpRect.top(),
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
        val = Value (calc->count (range));
        break;
      case Count:
        val = Value (calc->count (range, false));
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

void View::statusBarClicked(int _id)
{
  if ( !koDocument()->isReadWrite() || !factory() )
    return;
  if ( _id == 0 ) //menu calc
  {
    QPoint mousepos = QCursor::pos();
    ((Q3PopupMenu*)factory()->container( "calc_popup" , this ) )->popup( mousepos );
  }
}

void View::menuCalc( bool )
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

  calcStatusBarOp();

  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}


QMatrix View::matrix() const
{
  QMatrix m;
  m.scale( d->doc->zoomedResolutionX(),
           d->doc->zoomedResolutionY() );
  m.translate( - d->canvas->xOffset(), - d->canvas->yOffset() );
  return m;
}

void View::transformPart()
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

void View::slotChildSelected( KoDocumentChild* /*ch*/ )
{
//   if ( d->activeSheet && !d->activeSheet->isProtected() )
//   {
//     d->actions->transform->setEnabled( true );
//
//     if ( !d->transformToolBox.isNull() )
//     {
//         d->transformToolBox->setEnabled( true );
//         d->transformToolBox->setDocumentChild( ch );
//     }
//   }


  doc()->emitBeginOperation( false );
  d->activeSheet->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));

  doc()->emitEndOperation();
  paintUpdates();
}

void View::slotChildUnselected( KoDocumentChild* )
{
//   if ( d->activeSheet && !d->activeSheet->isProtected() )
//   {
//     d->actions->transform->setEnabled( false );
//
//     if ( !d->transformToolBox.isNull() )
//     {
//         d->transformToolBox->setEnabled( false );
//     }
//     deleteEditor( true );
//   }


  doc()->emitBeginOperation( false );
  d->activeSheet->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));
  doc()->emitEndOperation();
  paintUpdates();
}


void View::deleteEditor( bool saveChanges )
{
    doc()->emitBeginOperation( false );
    d->canvas->deleteEditor( saveChanges );

    markSelectionAsDirty();
    doc()->emitEndOperation();
}

DCOPObject * View::dcopObject()
{
  if ( !d->dcop )
    d->dcop = new ViewIface( this );

  return d->dcop;
}

QWidget * View::canvas() const
{
  return canvasWidget();
}

int View::canvasXOffset() const
{
  if (!d->activeSheet)
	  return 0;

  double zoomedResX = d->activeSheet->doc()->zoomedResolutionX();
  return int( canvasWidget()->xOffset() * zoomedResX );
}

int View::canvasYOffset() const
{
  if (!d->activeSheet)
	 return 0;

  double zoomedResY = d->activeSheet->doc()->zoomedResolutionY();
  return int( canvasWidget()->yOffset() * zoomedResY );
}


void View::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
  if ( d->activeSheet )
  {
    doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );

    if ( ev->activated() )
    {
      if ( d->calcLabel )
        calcStatusBarOp();
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

void View::popupTabBarMenu( const QPoint & _point )
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
    static_cast<Q3PopupMenu*>(factory()->container("menupage_popup",this))->popup(_point);
  }
}

void View::updateBorderButton()
{
  //  doc()->emitBeginOperation( false );
  if ( d->activeSheet )
    d->actions->showPageBorders->setChecked( d->activeSheet->isShowPageBorders() );
  //  doc()->emitEndOperation();
}

void View::removeSheet( Sheet *_t )
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

void View::insertSheet( Sheet* sheet )
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

QColor View::borderColor() const
{
  return d->actions->borderColor->color();
}

void View::updateShowSheetMenu()
{
  doc()->emitBeginOperation( false );
  if ( d->activeSheet->isProtected() )
    d->actions->showSheet->setEnabled( false );
  else
    d->actions->showSheet->setEnabled( doc()->map()->hiddenSheets().count() > 0 );
  doc()->emitEndOperation( d->activeSheet->visibleRect( d->canvas ) );
}

void View::closeEditor()
{
  if ( d->activeSheet ) { // #45822
    doc()->emitBeginOperation( false );
    d->canvas->closeEditor();

    markSelectionAsDirty();
    doc()->emitEndOperation();
  }
}

void View::markSelectionAsDirty()
{
    if (!d->activeSheet)
      return;

    d->activeSheet->setRegionPaintDirty( *selectionInfo() );
}

void View::paintUpdates()
{
  /* don't do any begin/end operation here -- this is what is called at an
     endOperation
  */
  d->canvas->paintUpdates();
}

void View::commandExecuted()
{
  updateEditWidget();
  calcStatusBarOp();
}

void View::initialiseMarkerFromSheet( Sheet *_sheet, const QPoint &point )
{
    d->savedMarkers.replace( _sheet, point);
}

QPoint View::markerFromSheet( Sheet *_sheet ) const
{
    QMap<Sheet*, QPoint>::Iterator it2 = d->savedMarkers.find(_sheet);
    QPoint newMarker = (it2 == d->savedMarkers.end()) ? QPoint(1,1) : *it2;
    return newMarker;
}

void View::saveCurrentSheetSelection()
{
    /* save the current selection on this sheet */
    if (d->activeSheet != NULL)
    {
        d->savedAnchors.replace(d->activeSheet, d->selection->anchor());
        kDebug() << " Current scrollbar vert value: " << d->canvas->vertScrollBar()->value() << endl;
        kDebug() << "Saving marker pos: " << d->selection->marker() << endl;
        d->savedMarkers.replace(d->activeSheet, d->selection->marker());
    }
}

void View::handleDamages( const Q3ValueList<Damage*>& damages )
{
    Q3ValueList<Damage*>::ConstIterator it;
    for( it = damages.begin(); it != damages.end(); ++it )
    {
        Damage* damage = *it;
        if( !damage ) continue;

        if( damage->type() == Damage::Cell )
        {
            CellDamage* cd = static_cast<CellDamage*>( damage );
            Cell* damagedCell = cd->cell();
            Sheet* damagedSheet = damagedCell->sheet();
            QRect drect( damagedCell->column(), damagedCell->row(), 1, 1 );
            damagedSheet->setRegionPaintDirty( drect );
            paintUpdates();
        }

        if( damage->type() == Damage::Sheet )
        {
            SheetDamage* sd = static_cast<SheetDamage*>( damage );
            Sheet* damagedSheet = sd->sheet();

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

void View::runInternalTests()
{
    // run various tests, only for developers
    KSpread::TestRunner* runner = new KSpread::TestRunner();
    runner->exec();
    delete runner;
}

void View::runInspector()
{
    // useful to inspect objects
    if(!d->activeSheet) return;
    Cell * cell = d->activeSheet->cellAt( d->selection->marker() );
    KSpread::Inspector* ins = new KSpread::Inspector( cell );
    ins->exec();
    delete ins;
}

QColor View::highlightColor()
{
    return QApplication::palette().active().highlight().light( 175 );
}

} // namespace KSpread

#include "kspread_view.moc"
