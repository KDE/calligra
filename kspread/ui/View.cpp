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
#include <QPainter>
#include <QPixmap>
#include <QRegExp>
#include <QResizeEvent>
#include <QTimer>
#include <QToolButton>
#include <QSqlDatabase>
#include <QSizePolicy>
//Added by qt3to4:
#include <Q3ValueList>

// KDE includes
#include <kactioncollection.h>
#include <kconfig.h>
#include <kcomponentdata.h>
#include <kdebug.h>
#include <kfind.h>
#include <kfinddialog.h>
#include <kfontaction.h>
#include <kfontsizeaction.h>
#include <kfontdialog.h>
#include <kinputdialog.h>
#include <kmessagebox.h>
#include <knewpassworddialog.h>
#include <kpassivepopup.h>
#include <kpassworddialog.h>
#include <kreplace.h>
#include <kreplacedialog.h>
#include <k3spell.h>
#include <k3spelldlg.h>
#include <kstatusbar.h>
#include <kstandardaction.h>
#include <kstandarddirs.h>
#include <ktemporaryfile.h>
#include <KToggleAction>
#include <kparts/event.h>
#include <k3listview.h>
#include <kpushbutton.h>
#include <kxmlguifactory.h>
#include <kicon.h>

// KOffice includes
#include <kdatatool.h>
#include <KoGlobal.h>
#include <KoCanvasController.h>
#include <KoCanvasResourceProvider.h>
#include <KoCharSelectDia.h>
#include <KoColor.h>
#include <KoMainWindow.h>
#include <KoOasisLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoOdfStylesReader.h>
#include <KoPartSelectAction.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoColorSetAction.h>
#include <KoTabBar.h>
#include <KoToolBox.h>
#include <KoToolBoxFactory.h>
#include <KoToolDocker.h>
#include <KoToolDockerFactory.h>
#include <KoToolManager.h>
#include <KoToolRegistry.h>
// #include <Toolbox.h>
#include <KoTemplateCreateDia.h>
#include <ktoolinvocation.h>
#include <KoZoomAction.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>

// KSpread includes
#include "CellStorage.h"
#include "CellView.h"
#include "Damages.h"
#include "DefaultToolFactory.h"
#include "DependencyManager.h"
#include "Digest.h"
#include "inspector.h"
#include "LoadingInfo.h"
#include "Border.h"
#include "Canvas.h"
#include "Editors.h"
#include "Global.h"
// #include "Handler.h"
#include "Localization.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "Object.h"
#include "RecalcManager.h"
#include "Selection.h"
#include "SheetPrint.h"
#include "SheetShapeContainer.h"
#include "SheetView.h"
#include "Style.h"
#include "StyleManager.h"
#include "StyleStorage.h"
#include "Util.h"
#include "ValueCalc.h"
#include "ValueConverter.h"

// commands
#include "commands/AutoFilterCommand.h"
#include "commands/BorderColorCommand.h"
#include "commands/CommentCommand.h"
#include "commands/ConditionCommand.h"
#include "commands/DataManipulators.h"
#include "commands/DeleteCommand.h"
#include "commands/EmbeddedObjectCommands.h"
#include "commands/IndentationCommand.h"
#include "commands/LinkCommand.h"
#include "commands/MacroCommand.h"
#include "commands/PrecisionCommand.h"
#include "commands/RowColumnManipulators.h"
#include "commands/SheetCommands.h"
#include "commands/SortManipulator.h"
#include "commands/StyleCommand.h"
#include "commands/Undo.h"
#include "commands/ValidityCommand.h"

// dialogs
#include "dialogs/AngleDialog.h"
#include "dialogs/AddNamedAreaDialog.h"
#include "dialogs/CommentDialog.h"
#include "dialogs/ConditionalDialog.h"
#include "dialogs/ConsolidateDialog.h"
#include "dialogs/CSVDialog.h"
#include "dialogs/DatabaseDialog.h"
#include "dialogs/AutoFormatDialog.h"
#include "dialogs/FormulaDialog.h"
#include "dialogs/GoalSeekDialog.h"
#include "dialogs/GotoDialog.h"
#include "dialogs/InsertDialog.h"
#include "dialogs/LayoutDialog.h"
#include "dialogs/ListDialog.h"
#include "dialogs/NamedAreaDialog.h"
//#include "dialogs/PaperLayoutDialog.h"
#include "dialogs/PasteInsertDialog.h"
#include "dialogs/PreferenceDialog.h"
#include "dialogs/Resize2Dialog.h"
#include "dialogs/SeriesDialog.h"
#include "dialogs/ShowDialog.h"
#include "dialogs/ShowColRowDialog.h"
#include "dialogs/SortDialog.h"
#include "dialogs/SpecialPasteDialog.h"
#include "dialogs/StyleManagerDialog.h"
#include "dialogs/SubtotalDialog.h"
#include "dialogs/ValidityDialog.h"
#include "dialogs/FindDialog.h"

#include "PropertyEditor.h"
#include "dialogs/SheetSelectPage.h"
#include "dialogs/LinkDialog.h"
#include "dialogs/SheetPropertiesDialog.h"

// D-Bus
#include "interfaces/ViewAdaptor.h"
#include <QtDBus/QtDBus>

namespace KSpread
{
class ViewActions;

K_GLOBAL_STATIC_WITH_ARGS(DefaultToolFactory, s_defaultToolFactory, (0))

class View::Private
{
public:
    View* view;
    Doc* doc;

    // the active sheet, may be 0
    // this is the sheet which has the input focus
    Sheet* activeSheet;
    QHash<const Sheet*, SheetView*> sheetViews;

    // GUI elements
    QWidget *frame;
    QFrame *toolWidget;
    Canvas *canvas;
    KoCanvasController* canvasController;
    KoZoomController* zoomController;
    KoZoomHandler* zoomHandler;
    VBorder *vBorderWidget;
    HBorder *hBorderWidget;
    SelectAllButton* selectAllButton;
    QScrollBar *horzScrollBar;
    QScrollBar *vertScrollBar;
    KoTabBar *tabBar;
    QLabel* calcLabel;

    // formulabar, consists of:
    QHBoxLayout* formulaBarLayout;
    ComboboxLocationEditWidget *posWidget;
    QAbstractButton* formulaButton;
    QAbstractButton *okButton;
    QAbstractButton *cancelButton;
    KSpread::EditWidget *editWidget;
    QGridLayout* viewLayout;
    QGridLayout* tabScrollBarLayout;

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
    Region oldSelection; // for 'Select All'
    Selection* choice;
    QMap<Sheet*, QPoint> savedAnchors;
    QMap<Sheet*, QPoint> savedMarkers;
    QMap<Sheet*, QPointF> savedOffsets;

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

#if 0 // KSPREAD_KOPART_EMBEDDING
    InsertHandler* insertHandler;
#endif // KSPREAD_KOPART_EMBEDDING

    // Insert special character dialog
    KoCharSelectDia* specialCharDlg;

#if 0 // KSPREAD_KOPART_EMBEDDING
    // Holds a guarded pointer to the transformation toolbox.
    QPointer<KoTransformToolBox> transformToolBox;
#endif // KSPREAD_KOPART_EMBEDDING

    // the last popup menu (may be 0).
    // Since only one popup menu can be opened at once, its pointer is stored here.
    // Delete the old one before you store a pointer to anotheron here.
    QMenu *popupMenu;
    int popupMenuFirstToolId;

    QMenu *popupRow;
    QMenu *popupColumn;
    QMenu* popupChild;       // for embedded children
    QMenu* popupListChoose;  // for list of choose

    // the child for which the popup menu has been opened.
    Child* popupChildObject;

    // spell-check context
    struct
    {
      K3Spell *   kspell;
      Sheet *  firstSpellSheet;
      Sheet *  currentSpellSheet;
      int      currentCellIndex;
      MacroCommand *macroCmdSpellCheck;
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
    QList<ToolEntry*> toolList;

    void initActions();
    void adjustActions( bool mode );
    void adjustActions( Cell cell );
    void adjustWorkbookActions( bool mode );
    QAbstractButton* newIconButton( const char *_file, bool _kbutton = false, QWidget *_parent = 0 );

    PropertyEditor *m_propertyEditor;

    // On timeout this will execute the status bar operation (e.g. SUM).
    // This is delayed to speed up the selection.
    QTimer statusBarOpTimer;

    //Temporary until Qt4.4 provides API for add/set/get of pages
    SheetSelectPage *m_sheetpage;
};

class ViewActions
{
public:

    // cell formatting
    QAction * cellLayout;
    QAction *actionExtraProperties;
    QAction * defaultFormat;
    KToggleAction* bold;
    KToggleAction* italic;
    KToggleAction* underline;
    KToggleAction* strikeOut;
    KFontAction* selectFont;
    KFontSizeAction* selectFontSize;
    QAction * fontSizeUp;
    QAction * fontSizeDown;
    QAction* textColor;
    KToggleAction* alignLeft;
    KToggleAction* alignCenter;
    KToggleAction* alignRight;
    KToggleAction* alignTop;
    KToggleAction* alignMiddle;
    KToggleAction* alignBottom;
    KToggleAction* wrapText;
    KToggleAction* verticalText;
    QAction * increaseIndent;
    QAction * decreaseIndent;
    QAction * changeAngle;
    KToggleAction* percent;
    QAction * precplus;
    QAction * precminus;
    KToggleAction* money;
    QAction * upper;
    QAction * lower;
    QAction * firstLetterUpper;
    QAction* bgColor;
    QAction * borderLeft;
    QAction * borderRight;
    QAction * borderTop;
    QAction * borderBottom;
    QAction * borderAll;
    QAction * borderOutline;
    QAction * borderRemove;
    QAction* borderColor;
    KSelectAction* selectStyle;
    QAction * createStyle;

    // cell operations
    QAction * editCell;
    QAction * insertCell;
    QAction * deleteCell;
    QAction * clearAll;
    QAction * mergeCell;
    QAction * mergeCellHorizontal;
    QAction * mergeCellVertical;
    QAction * dissociateCell;
    QAction * clearContents;
    QAction * conditional;
    QAction * clearConditional;
    QAction * validity;
    QAction * clearValidity;
    QAction * comment;
    QAction * clearComment;

    // column & row operations
    QAction * resizeColumn;
    QAction * insertColumn;
    QAction * deleteColumn;
    QAction * hideColumn;
    QAction * showColumn;
    QAction * equalizeColumn;
    QAction * showSelColumns;
    QAction * resizeRow;
    QAction * insertRow;
    QAction * deleteRow;
    QAction * hideRow;
    QAction * showRow;
    QAction * equalizeRow;
    QAction * showSelRows;
    QAction * adjust;

    // sheet/workbook operations
    QAction * sheetProperties;
    QAction * insertSheet;
    QAction * duplicateSheet;
    QAction * deleteSheet;
    QAction * renameSheet;
    QAction * hideSheet;
    QAction * showSheet;
    QAction * autoFormat;
    QAction * areaName;
    QAction * showArea;
    QAction * insertSeries;
    QAction * insertFunction;
    QAction * insertSpecialChar;
    QAction * insertFromDatabase;
    QAction * insertFromTextfile;
    QAction * insertFromClipboard;
    QAction * transform;
    QAction * sort;
    QAction * sortDec;
    QAction * sortInc;
    QAction * autoFilter;
    QAction * fillRight;
    QAction * fillLeft;
    QAction * fillUp;
    QAction * fillDown;
    QAction * paperLayout;
    QAction * definePrintRange;
    QAction * resetPrintRange;
    KToggleAction* showPageBorders;
    QAction * recalcWorksheet;
    QAction * recalcWorkbook;
    KToggleAction* protectSheet;
    KToggleAction* protectDoc;

    // general editing
    QAction * cut;
    QAction * copy;
    QAction * paste;
    QAction * specialPaste;
    QAction * insertCellCopy;
    QAction * selectAll;
    QAction * find;
    QAction * replace;

    // navigation
    QAction * gotoCell;
    QAction * nextSheet;
    QAction * prevSheet;
    QAction * firstSheet;
    QAction * lastSheet;

    // misc
    QAction * styleDialog;
    QAction * autoSum;
    KSelectAction* formulaSelection;
    QAction * insertLink;
    QAction * clearHyperlink;
    QAction * consolidate;
    QAction * goalSeek;
    QAction * subTotals;
    QAction * textToColumns;
    QAction * multipleOperations;
    QAction * createTemplate;
    KoPartSelectAction *insertPart;
    KToggleAction* insertChartFrame;
    QAction * insertPicture;
    QAction * customList;
    QAction * spellChecking;
    QAction * inspector;

    // settings
    KToggleAction* showStatusBar;
    KToggleAction* showTabBar;
    KToggleAction* showFormulaBar;
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

  // -- cell formatting actions --

  actions->cellLayout  = new KAction(KIcon( "cell_layout" ), i18n("Cell Format..."), view);
  actions->cellLayout->setIconText(i18n("Format"));
  ac->addAction("cellLayout", actions->cellLayout );
  actions->cellLayout->setShortcut( QKeySequence( Qt::CTRL+ Qt::ALT+ Qt::Key_F));
  connect(actions->cellLayout, SIGNAL(triggered(bool)), view, SLOT( layoutDlg() ));
  actions->cellLayout->setToolTip( i18n("Set the cell formatting") );

  actions->actionExtraProperties  = new KAction(KIcon( "penbrush" ), i18n("&Properties"), view);
  ac->addAction("extra_properties", actions->actionExtraProperties );
  connect(actions->actionExtraProperties, SIGNAL(triggered(bool)), view, SLOT( extraProperties() ));

  actions->defaultFormat  = new KAction(i18n("Default"), view);
  ac->addAction("default", actions->defaultFormat );
  connect(actions->defaultFormat, SIGNAL(triggered(bool)),view, SLOT( setDefaultStyle() ));
  actions->defaultFormat->setToolTip( i18n("Resets to the default format") );

  actions->bold  = new KToggleAction(KIcon( "format-text-bold" ), i18n("Bold"), view);
  ac->addAction("bold", actions->bold );
  actions->bold->setShortcut( QKeySequence( Qt::CTRL+Qt::Key_B));
  connect( actions->bold, SIGNAL( toggled( bool) ),
                    view, SLOT( bold( bool ) ) );

  actions->italic  = new KToggleAction(KIcon( "format-text-italic" ), i18n("Italic"), view);
  ac->addAction("italic", actions->italic );
  actions->italic->setShortcut( QKeySequence( Qt::CTRL+Qt::Key_I));
  connect( actions->italic, SIGNAL( toggled( bool) ),
                    view, SLOT( italic( bool ) ) );

  actions->underline  = new KToggleAction(KIcon( "format-text-underline" ), i18n("Underline"), view);
  ac->addAction("underline", actions->underline );
  actions->underline->setShortcut( QKeySequence( Qt::CTRL+Qt::Key_U));
  connect( actions->underline, SIGNAL( toggled( bool) ),
                    view, SLOT( underline( bool ) ) );

  actions->strikeOut  = new KToggleAction(KIcon( "format-text-strikethrough" ), i18n("Strike Out"), view);
  ac->addAction("strikeout", actions->strikeOut );
  connect( actions->strikeOut, SIGNAL( toggled( bool) ),
                    view, SLOT( strikeOut( bool ) ) );

  actions->selectFont  = new KFontAction(i18n("Select Font..."), view);
  actions->selectFont->setIconText(i18n("Font"));
  ac->addAction("selectFont", actions->selectFont );
  connect( actions->selectFont, SIGNAL( triggered( const QString& ) ),
                    view, SLOT( fontSelected( const QString& ) ) );

  actions->selectFontSize  = new KFontSizeAction(i18n("Select Font Size"), view);
  actions->selectFont->setIconText(i18n("Font Size"));
  ac->addAction("selectFontSize", actions->selectFontSize );
  connect( actions->selectFontSize, SIGNAL( fontSizeChanged( int ) ),
                    view, SLOT( fontSizeSelected( int ) ) );

  actions->fontSizeUp  = new KAction(KIcon( "fontsizeup" ), i18n("Increase Font Size"), view);
  ac->addAction("increaseFontSize", actions->fontSizeUp );
  connect(actions->fontSizeUp, SIGNAL(triggered(bool)), view, SLOT( increaseFontSize() ));


  actions->fontSizeDown  = new KAction(KIcon( "fontsizedown" ), i18n("Decrease Font Size"), view);
  ac->addAction("decreaseFontSize", actions->fontSizeDown );
  connect(actions->fontSizeDown, SIGNAL(triggered(bool)), view, SLOT( decreaseFontSize() ));

    actions->textColor = new KAction(KIcon("textcolor"), i18n("Text Color"), view);
    ac->addAction("textColor", actions->textColor);
    connect(actions->textColor, SIGNAL(triggered(bool)), view, SLOT( changeTextColor()));

    // -- horizontal alignment --
    //
    QActionGroup* groupAlign = new QActionGroup( view );
    actions->alignLeft  = new KToggleAction(KIcon("format-justify-left"), i18n("Align Left"), view);
    actions->alignLeft->setIconText(i18n("Left"));
    ac->addAction("left", actions->alignLeft );
    connect( actions->alignLeft, SIGNAL( toggled( bool ) ),
             view, SLOT( alignLeft( bool ) ) );
    actions->alignLeft->setToolTip(i18n("Left justify the cell contents"));
    actions->alignLeft->setActionGroup( groupAlign );

    actions->alignCenter  = new KToggleAction(KIcon("format-justify-center"), i18n("Align Center"), view);
    actions->alignCenter->setIconText(i18n("Center"));
    ac->addAction("center", actions->alignCenter );
    connect( actions->alignCenter, SIGNAL( toggled( bool ) ),
             view, SLOT( alignCenter( bool ) ) );
    actions->alignCenter->setToolTip(i18n("Center the cell contents"));
    actions->alignCenter->setActionGroup( groupAlign );

    actions->alignRight  = new KToggleAction(KIcon("format-justify-right"), i18n("Align Right"), view);
    actions->alignRight->setIconText(i18n("Right"));
    ac->addAction("right", actions->alignRight );
    connect( actions->alignRight, SIGNAL( toggled( bool ) ),
             view, SLOT( alignRight( bool ) ) );
    actions->alignRight->setToolTip(i18n("Right justify the cell contents"));
    actions->alignRight->setActionGroup( groupAlign );


    // -- vertical alignment --
    //
    QActionGroup* groupPos = new QActionGroup( view );
    actions->alignTop  = new KToggleAction(KIcon("text_top"), i18n("Align Top"), view);
    actions->alignTop->setIconText(i18n("Top"));
    ac->addAction("top", actions->alignTop );
    connect( actions->alignTop, SIGNAL( toggled( bool ) ),
             view, SLOT( alignTop( bool ) ) );
    actions->alignTop->setToolTip(i18n("Align cell contents along the top of the cell"));
    actions->alignTop->setActionGroup( groupPos );

    actions->alignMiddle  = new KToggleAction(KIcon("middle"), i18n("Align Middle"), view);
    actions->alignMiddle->setIconText(i18n("Middle"));
    ac->addAction("middle", actions->alignMiddle );
    connect( actions->alignMiddle, SIGNAL( toggled( bool ) ),
             view, SLOT( alignMiddle( bool ) ) );
    actions->alignMiddle->setToolTip(i18n("Align cell contents centered in the cell"));
    actions->alignMiddle->setActionGroup( groupPos );

    actions->alignBottom  = new KToggleAction(KIcon("text_bottom"), i18n("Align Bottom"), view);
    actions->alignBottom->setIconText(i18n("Bottom"));
    ac->addAction("bottom", actions->alignBottom );
    connect( actions->alignBottom, SIGNAL( toggled( bool ) ),
             view, SLOT( alignBottom( bool ) ) );
    actions->alignBottom->setToolTip(i18n("Align cell contents along the bottom of the cell"));
    actions->alignBottom->setActionGroup( groupPos );


  actions->wrapText  = new KToggleAction(KIcon( "multirow" ), i18n("Wrap Text"), view);
  actions->wrapText->setIconText(i18n("Wrap"));
  ac->addAction("multiRow", actions->wrapText );
  connect( actions->wrapText, SIGNAL( toggled( bool ) ),
                    view, SLOT( wrapText( bool ) ) );
  actions->wrapText->setToolTip(i18n("Make the cell text wrap onto multiple lines"));

  actions->verticalText  = new KToggleAction(KIcon("vertical_text"  ), i18n("Vertical Text"), view);
  actions->verticalText->setIconText(i18n("Vertical"));
  ac->addAction("verticaltext", actions->verticalText );
  connect( actions->verticalText, SIGNAL( toggled( bool ) ),
                    view, SLOT( verticalText( bool ) ) );
  actions->verticalText->setToolTip(i18n("Print cell contents vertically"));

  actions->increaseIndent  = new KAction(KIcon( QApplication::isRightToLeft() ? "format_decreaseindent" : "format_increaseindent" ), i18n("Increase Indent"), view);
  ac->addAction("increaseindent", actions->increaseIndent );
  connect(actions->increaseIndent, SIGNAL(triggered(bool)), view, SLOT( increaseIndent() ));
  actions->increaseIndent->setToolTip(i18n("Increase the indentation"));

  actions->decreaseIndent  = new KAction(KIcon( QApplication::isRightToLeft() ? "format_increaseindent" : "format_decreaseindent" ), i18n("Decrease Indent"), view);
  ac->addAction("decreaseindent", actions->decreaseIndent );
  connect(actions->decreaseIndent, SIGNAL(triggered(bool)), view, SLOT( decreaseIndent() ));
  actions->decreaseIndent->setToolTip(i18n("Decrease the indentation"));

  actions->changeAngle  = new KAction(i18n("Change Angle..."), view);
  actions->changeAngle->setIconText(i18n("Angle"));
  ac->addAction("changeangle", actions->changeAngle );
  connect(actions->changeAngle, SIGNAL(triggered(bool)),view, SLOT( changeAngle() ));
  actions->changeAngle->setToolTip(i18n("Change the angle that cell contents are printed"));

  actions->percent  = new KToggleAction(KIcon( "percent" ), i18n("Percent Format"), view);
  actions->percent->setIconText(i18n("Percent"));
  ac->addAction("percent", actions->percent );
  connect( actions->percent, SIGNAL( toggled( bool ) ),
                    view, SLOT( percent( bool ) ) );
  actions->percent->setToolTip(i18n("Set the cell formatting to look like a percentage"));

  actions->precplus  = new KAction(KIcon( "prec_plus" ), i18n("Increase Precision"), view);
  ac->addAction("precplus", actions->precplus );
  connect(actions->precplus, SIGNAL(triggered(bool)), view, SLOT( increasePrecision() ));
  actions->precplus->setToolTip(i18n("Increase the decimal precision shown onscreen"));

  actions->precminus  = new KAction(KIcon( "prec_minus" ), i18n("Decrease Precision"), view);
  ac->addAction("precminus", actions->precminus );
  connect(actions->precminus, SIGNAL(triggered(bool)), view, SLOT( decreasePrecision() ));
  actions->precminus->setToolTip(i18n("Decrease the decimal precision shown onscreen"));

  actions->money  = new KToggleAction(KIcon( "money" ), i18n("Money Format"), view);
  actions->money->setIconText(i18n("Money"));
  ac->addAction("money", actions->money );
  connect( actions->money, SIGNAL( toggled( bool ) ),
                    view, SLOT( moneyFormat( bool ) ) );
  actions->money->setToolTip(i18n("Set the cell formatting to look like your local currency"));

  actions->upper  = new KAction(KIcon( "fontsizeup" ), i18n("Upper Case"), view);
  actions->upper->setIconText(i18n("Upper"));
  ac->addAction("upper", actions->upper );
  connect(actions->upper, SIGNAL(triggered(bool)), view, SLOT( upper() ));
  actions->upper->setToolTip(i18n("Convert all letters to upper case"));

  actions->lower  = new KAction(KIcon( "fontsizedown" ), i18n("Lower Case"), view);
  actions->lower->setIconText(i18n("Lower"));
  ac->addAction("lower", actions->lower );
  connect(actions->lower, SIGNAL(triggered(bool)), view, SLOT( lower() ));
  actions->lower->setToolTip(i18n("Convert all letters to lower case"));

  actions->firstLetterUpper  = new KAction(KIcon("first_letter_upper" ), i18n("Convert First Letter to Upper Case"), view);
  actions->firstLetterUpper->setIconText(i18n("First Letter Upper"));
  ac->addAction("firstletterupper", actions->firstLetterUpper );
  connect(actions->firstLetterUpper, SIGNAL(triggered(bool)), view, SLOT( firstLetterUpper() ));
  actions->firstLetterUpper->setToolTip(i18n("Capitalize the first letter"));

  actions->bgColor = new KAction(KIcon("color_fill"), i18n("Background Color"), view);
  ac->addAction("backgroundColor", actions->bgColor);
  connect(actions->bgColor, SIGNAL(triggered(bool)), view, SLOT(changeBackgroundColor()));
  actions->bgColor->setToolTip(i18n("Set the background color"));

  actions->borderLeft  = new KAction(KIcon( "border_left" ), i18n("Border Left"), view);
  actions->borderLeft->setIconText(i18n("Left"));
  ac->addAction("borderLeft", actions->borderLeft );
  connect(actions->borderLeft, SIGNAL(triggered(bool)), view, SLOT( borderLeft() ));
  actions->borderLeft->setToolTip(i18n("Set a left border to the selected area"));

  actions->borderRight  = new KAction(KIcon( "border_right" ), i18n("Border Right"), view);
  actions->borderRight->setIconText(i18n("Right"));
  ac->addAction("borderRight", actions->borderRight );
  connect(actions->borderRight, SIGNAL(triggered(bool)), view, SLOT( borderRight() ));
  actions->borderRight->setToolTip(i18n("Set a right border to the selected area"));

  actions->borderTop  = new KAction(KIcon( "border_top" ), i18n("Border Top"), view);
  actions->borderTop->setIconText(i18n("Top"));
  ac->addAction("borderTop", actions->borderTop );
  connect(actions->borderTop, SIGNAL(triggered(bool)), view, SLOT( borderTop() ));
  actions->borderTop->setToolTip(i18n("Set a top border to the selected area"));

  actions->borderBottom  = new KAction(KIcon( "border_bottom" ), i18n("Border Bottom"), view);
  actions->borderBottom->setIconText(i18n("Bottom"));
  ac->addAction("borderBottom", actions->borderBottom );
  connect(actions->borderBottom, SIGNAL(triggered(bool)), view, SLOT( borderBottom() ));
  actions->borderBottom->setToolTip(i18n("Set a bottom border to the selected area"));

  actions->borderAll  = new KAction(KIcon( "border_all" ), i18n("All Borders"), view);
  actions->borderAll->setIconText(i18n("All"));
  ac->addAction("borderAll", actions->borderAll );
  connect(actions->borderAll, SIGNAL(triggered(bool)), view, SLOT( borderAll() ));
  actions->borderAll->setToolTip(i18n("Set a border around all cells in the selected area"));

  actions->borderRemove  = new KAction(KIcon( "border_remove" ), i18n("No Borders"), view);
  actions->borderRemove->setIconText(i18n("None"));
  ac->addAction("borderRemove", actions->borderRemove );
  connect(actions->borderRemove, SIGNAL(triggered(bool)), view, SLOT( borderRemove() ));
  actions->borderRemove->setToolTip(i18n("Remove all borders in the selected area"));

  actions->borderOutline  = new KAction(KIcon( ("border_outline") ), i18n("Border Outline"), view);
  actions->borderOutline->setIconText(i18n("Outline"));
  ac->addAction("borderOutline", actions->borderOutline );
  connect(actions->borderOutline, SIGNAL(triggered(bool)), view, SLOT( borderOutline() ));
  actions->borderOutline->setToolTip(i18n("Set a border to the outline of the selected area"));

  actions->borderColor = new KAction(KIcon("color_line"), i18n("Border Color"), view);
  ac->addAction("borderColor", actions->borderColor);
  connect(actions->borderColor, SIGNAL(triggered(bool)), view, SLOT(changeBorderColor()));
  actions->borderColor->setToolTip(i18n("Select a new border color"));

  actions->selectStyle  = new KSelectAction(i18n("Style"), view);
  ac->addAction("stylemenu", actions->selectStyle );
  actions->selectStyle->setToolTip( i18n( "Apply a predefined style to the selected cells" ) );
  connect( actions->selectStyle, SIGNAL( triggered( const QString & ) ),
           view, SLOT( setStyle( const QString & ) ) );

  actions->createStyle  = new KAction(i18n("Create Style From Cell..."), view);
  actions->createStyle->setIconText(i18n("Style From Cell"));
  ac->addAction("createStyle", actions->createStyle );
  connect(actions->createStyle, SIGNAL(triggered(bool)),view, SLOT( createStyleFromCell()));
  actions->createStyle->setToolTip( i18n( "Create a new style based on the currently selected cell" ) );

  // -- cell operation actions --

  actions->editCell = new KAction(KIcon("cell_edit" ), i18n("Modify Cell"), view);
  ac->addAction("editCell", actions->editCell );
  actions->editCell->setShortcut( QKeySequence( Qt::CTRL+Qt::Key_M));
  connect(actions->editCell, SIGNAL(triggered(bool)), view, SLOT( editCell() ));
  actions->editCell->setToolTip(i18n("Edit the highlighted cell"));

  actions->insertCell = new KAction(KIcon("insertcell"), i18n("Cells..."), view);
  actions->insertCell->setIconText(i18n("Insert Cells..."));
  actions->insertCell->setToolTip(i18n("Insert a blank cell into the spreadsheet"));
  ac->addAction("insertCell", actions->insertCell );
  connect(actions->insertCell, SIGNAL(triggered(bool)), view, SLOT(insertCells()));

  actions->deleteCell = new KAction(KIcon("removecell"), i18n("Cells..."), view);
  actions->deleteCell->setIconText(i18n("Remove Cells..."));
  actions->deleteCell->setToolTip(i18n("Removes the cells from the spreadsheet"));
  ac->addAction("deleteCell", actions->deleteCell);
  connect(actions->deleteCell, SIGNAL(triggered(bool)), view, SLOT(deleteCells()));

  actions->clearAll = new KAction(KIcon("deletecell"), i18n("All"), view);
  actions->clearAll->setIconText(i18n("Clear All"));
  actions->clearAll->setToolTip(i18n("Clear all contents and formatting of the current cell"));
  ac->addAction("clearAll", actions->clearAll);
  connect(actions->clearAll, SIGNAL(triggered(bool)), view, SLOT(clearAll()));

  actions->mergeCell = new KAction(KIcon("mergecell"), i18n("Merge Cells"), view);
  ac->addAction("mergecell", actions->mergeCell);
  connect(actions->mergeCell, SIGNAL(triggered(bool)), view, SLOT( mergeCell()));
  actions->mergeCell->setToolTip(i18n("Merge the selected region"));

  actions->mergeCellHorizontal = new KAction(KIcon("mergecell-horizontal"), i18n("Merge Cells Horizontally"), view);
  actions->mergeCellHorizontal->setToolTip(i18n("Merge the selected Region.horizontally"));
  ac->addAction("mergecellHorizontal", actions->mergeCellHorizontal);
  connect(actions->mergeCellHorizontal, SIGNAL(triggered(bool)), view, SLOT( mergeCellHorizontal()));

  actions->mergeCellVertical = new KAction(KIcon("mergecell-vertical"), i18n("Merge Cells Vertically"), view);
  actions->mergeCellVertical->setToolTip(i18n("Merge the selected region vertically"));
  ac->addAction("mergecellVertical", actions->mergeCellVertical);
  connect(actions->mergeCellVertical, SIGNAL(triggered(bool)), view, SLOT(mergeCellVertical()));

  actions->dissociateCell = new KAction(KIcon("dissociatecell"), i18n("Dissociate Cells"), view);
  actions->dissociateCell->setToolTip(i18n("Unmerge the selected region"));
  ac->addAction("dissociatecell", actions->dissociateCell);
  connect(actions->dissociateCell, SIGNAL(triggered(bool)), view, SLOT(dissociateCell()));

  actions->clearContents = new KAction(KIcon("edit-clear"), i18n("Contents"), view);
  actions->clearContents->setIconText(i18n("Clear Contents"));
  actions->clearContents->setToolTip(i18n("Remove the contents of the current cell"));
  ac->addAction("clearContents", actions->clearContents);
  connect(actions->clearContents, SIGNAL(triggered(bool)), view, SLOT(clearContents()));

  actions->conditional = new KAction(i18n("Conditional Styles..."), view);
  actions->conditional->setToolTip(i18n("Set cell style based on certain conditions"));
  ac->addAction("conditional", actions->conditional );
  connect(actions->conditional, SIGNAL(triggered(bool)), view, SLOT(conditional()));

  actions->clearConditional = new KAction(i18n("Conditional Styles"), view);
  actions->clearConditional->setIconText(i18n("Remove Conditional Styles"));
  actions->clearConditional->setToolTip(i18n("Remove the conditional cell styles"));
  ac->addAction("clearConditional", actions->clearConditional );
  connect(actions->clearConditional, SIGNAL(triggered(bool)), view, SLOT(clearConditionalStyles()));

  actions->validity = new KAction(i18n("Validity..."), view);
  actions->validity->setToolTip(i18n("Set tests to confirm cell data is valid"));
  ac->addAction("validity", actions->validity );
  connect(actions->validity, SIGNAL(triggered(bool)), view, SLOT(validity()));

  actions->clearValidity = new KAction(i18n("Validity"), view);
  actions->clearValidity->setIconText(i18n("Remove Validity"));
  actions->clearValidity->setToolTip(i18n("Remove the validity tests on this cell"));
  ac->addAction("clearValidity", actions->clearValidity );
  connect(actions->clearValidity, SIGNAL(triggered(bool)), view, SLOT( clearValidity()));

  actions->comment = new KAction(KIcon("comment"), i18n("Comment..."), view);
  actions->comment->setToolTip(i18n("Edit a comment for this cell"));
  ac->addAction("comment", actions->comment);
  connect(actions->comment, SIGNAL(triggered(bool)), view, SLOT(comment()));

  actions->clearComment = new KAction(KIcon("removecomment"), i18n("Comment"), view);
  actions->clearComment->setIconText(i18n("Remove Comment"));
  actions->clearComment->setToolTip(i18n("Remove this cell's comment"));
  ac->addAction("clearComment", actions->clearComment);
  connect(actions->clearComment, SIGNAL(triggered(bool)),view, SLOT(clearComment()));

  // -- column & row actions --

  actions->resizeColumn  = new KAction(KIcon( "resizecol" ), i18n("Resize Column..."), view);
  ac->addAction("resizeCol", actions->resizeColumn );
  connect(actions->resizeColumn, SIGNAL(triggered(bool)), view, SLOT( resizeColumn() ));
  actions->resizeColumn->setToolTip(i18n("Change the width of a column"));

  actions->insertColumn = new KAction(KIcon("insert_table_col"), i18n("Columns"), view);
  actions->insertColumn->setIconText(i18n("Insert Columns"));
  actions->insertColumn->setToolTip(i18n("Inserts a new column into the spreadsheet"));
  ac->addAction("insertColumn", actions->insertColumn);
  connect(actions->insertColumn, SIGNAL(triggered(bool)), view, SLOT(insertColumn()));

  actions->deleteColumn = new KAction(KIcon("delete_table_col"), i18n("Columns"), view);
  actions->deleteColumn->setIconText(i18n("Remove Columns"));
  actions->deleteColumn->setToolTip(i18n("Removes a column from the spreadsheet"));
  ac->addAction("deleteColumn", actions->deleteColumn);
  connect(actions->deleteColumn, SIGNAL(triggered(bool)), view, SLOT(deleteColumn()));

  actions->hideColumn  = new KAction(KIcon( "hide_table_column" ), i18n("Hide Columns"), view);
  ac->addAction("hideColumn", actions->hideColumn );
  connect(actions->hideColumn, SIGNAL(triggered(bool)), view, SLOT( hideColumn() ));
  actions->hideColumn->setToolTip(i18n("Hide the column from view"));

  actions->showColumn  = new KAction(KIcon( "show_table_column" ), i18n("Show Columns..."), view);
  ac->addAction("showColumn", actions->showColumn );
  connect(actions->showColumn, SIGNAL(triggered(bool)), view, SLOT( slotShowColumnDialog() ));
  actions->showColumn->setToolTip(i18n("Show hidden columns"));

  actions->equalizeColumn  = new KAction(KIcon( "adjustcol" ), i18n("Equalize Column"), view);
  ac->addAction("equalizeCol", actions->equalizeColumn );
  connect(actions->equalizeColumn, SIGNAL(triggered(bool)), view, SLOT( equalizeColumn() ));
  actions->equalizeColumn->setToolTip(i18n("Resizes selected columns to be the same size"));

  actions->showSelColumns  = new KAction(KIcon( "show_sheet_column" ), i18n("Show Columns"), view);
  ac->addAction("showSelColumns", actions->showSelColumns );
  connect(actions->showSelColumns, SIGNAL(triggered(bool)), view, SLOT( showColumn() ));
  actions->showSelColumns->setToolTip(i18n("Show hidden columns in the selection"));
  actions->showSelColumns->setEnabled(false);

  actions->resizeRow  = new KAction(KIcon( "resizerow" ), i18n("Resize Row..."), view);
  ac->addAction("resizeRow", actions->resizeRow );
  connect(actions->resizeRow, SIGNAL(triggered(bool)), view, SLOT( resizeRow() ));
  actions->resizeRow->setToolTip(i18n("Change the height of a row"));

  actions->insertRow = new KAction(KIcon("insert_table_row"), i18n("Rows"), view);
  actions->insertRow->setIconText(i18n("Insert Rows"));
  actions->insertRow->setToolTip(i18n("Inserts a new row into the spreadsheet"));
  ac->addAction("insertRow", actions->insertRow);
  connect(actions->insertRow, SIGNAL(triggered(bool)), view, SLOT(insertRow()));

  actions->deleteRow = new KAction(KIcon("delete_table_row"), i18n("Rows"), view);
  actions->deleteRow->setIconText(i18n("Remove Rows"));
  actions->deleteRow->setToolTip(i18n("Removes a row from the spreadsheet"));
  ac->addAction("deleteRow", actions->deleteRow);
  connect(actions->deleteRow, SIGNAL(triggered(bool)), view, SLOT(deleteRow()));

  actions->hideRow  = new KAction(KIcon( "hide_table_row" ), i18n("Hide Rows"), view);
  ac->addAction("hideRow", actions->hideRow );
  connect(actions->hideRow, SIGNAL(triggered(bool)), view, SLOT( hideRow() ));
  actions->hideRow->setToolTip(i18n("Hide a row from view"));

  actions->showRow  = new KAction(KIcon( "show_table_row" ), i18n("Show Rows..."), view);
  ac->addAction("showRow", actions->showRow );
  connect(actions->showRow, SIGNAL(triggered(bool)), view, SLOT( slotShowRowDialog() ));
  actions->showRow->setToolTip(i18n("Show hidden rows"));

  actions->equalizeRow  = new KAction(KIcon( "adjustrow" ), i18n("Equalize Row"), view);
  ac->addAction("equalizeRow", actions->equalizeRow );
  connect(actions->equalizeRow, SIGNAL(triggered(bool)), view, SLOT( equalizeRow() ));
  actions->equalizeRow->setToolTip(i18n("Resizes selected rows to be the same size"));

  actions->showSelRows  = new KAction(KIcon( "show_table_row" ), i18n("Show Rows"), view);
  ac->addAction("showSelRows", actions->showSelRows );
  connect(actions->showSelRows, SIGNAL(triggered(bool)), view, SLOT( showRow() ));
  actions->showSelRows->setEnabled(false);
  actions->showSelRows->setToolTip(i18n("Show hidden rows in the selection"));

  actions->adjust  = new KAction(i18n("Adjust Row && Column"), view);
  ac->addAction("adjust", actions->adjust );
  connect(actions->adjust, SIGNAL(triggered(bool)),view, SLOT( adjust() ));
  actions->adjust->setToolTip(i18n("Adjusts row/column size so that the contents will fit"));

  // -- sheet/workbook actions --
  actions->sheetProperties  = new KAction(i18n("Sheet Properties"), view);
  ac->addAction("sheetProperties", actions->sheetProperties );
  connect(actions->sheetProperties, SIGNAL(triggered(bool)),view, SLOT( sheetProperties() ));
  actions->sheetProperties->setToolTip(i18n("Modify current sheet's properties"));

    actions->insertSheet = new KAction(KIcon("inserttable"), i18n("Sheet"), view);
    actions->insertSheet->setIconText(i18n("Insert Sheet"));
    actions->insertSheet->setToolTip(i18n("Insert a new sheet"));
    ac->addAction("insertSheet", actions->insertSheet );
    connect(actions->insertSheet, SIGNAL(triggered(bool)), view, SLOT( insertSheet() ));

    actions->duplicateSheet = new KAction(/*KIcon("inserttable"),*/ i18n("Duplicate Sheet"), view);
    actions->duplicateSheet->setToolTip(i18n("Duplicate the selected sheet"));
    ac->addAction("duplicateSheet", actions->duplicateSheet);
    connect(actions->duplicateSheet, SIGNAL(triggered(bool)), view, SLOT(duplicateSheet()));

    actions->deleteSheet = new KAction(KIcon("delete_table"), i18n("Sheet"), view);
    actions->deleteSheet->setIconText(i18n("Remove Sheet"));
    actions->deleteSheet->setToolTip(i18n("Remove the active sheet"));
    ac->addAction("deleteSheet", actions->deleteSheet);
    connect(actions->deleteSheet, SIGNAL(triggered(bool)), view, SLOT(deleteSheet()));

  actions->renameSheet  = new KAction(i18n("Rename Sheet..."), view);
  ac->addAction("renameSheet", actions->renameSheet );
  connect(actions->renameSheet, SIGNAL(triggered(bool)),view, SLOT( slotRename() ));
  actions->renameSheet->setToolTip(i18n("Rename the active sheet"));

  actions->showSheet  = new KAction(i18n("Show Sheet..."), view);
  ac->addAction("showSheet", actions->showSheet );
  connect(actions->showSheet, SIGNAL(triggered(bool)),view, SLOT( showSheet()));
  actions->showSheet->setToolTip(i18n("Show a hidden sheet"));

  actions->hideSheet  = new KAction(i18n("Hide Sheet"), view);
  ac->addAction("hideSheet", actions->hideSheet );
  connect(actions->hideSheet, SIGNAL(triggered(bool)),view, SLOT( hideSheet() ));
  actions->hideSheet->setToolTip(i18n("Hide the active sheet"));

  actions->autoFormat  = new KAction(i18n("Auto-Format..."), view);
  ac->addAction("sheetFormat", actions->autoFormat );
  connect(actions->autoFormat, SIGNAL(triggered(bool)),view, SLOT( sheetFormat() ));
  actions->autoFormat->setToolTip(i18n("Set the worksheet formatting"));

  actions->areaName  = new KAction(i18n("Area Name..."), view);
  ac->addAction("areaname", actions->areaName );
  connect(actions->areaName, SIGNAL(triggered(bool)),view, SLOT( setAreaName() ));
  actions->areaName->setToolTip(i18n("Set a name for a region of the spreadsheet"));

  actions->showArea = new KAction(i18n("Named Areas..."), view);
  actions->showArea->setShortcut( QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_G) );
  actions->showArea->setIconText(i18n("Named Areas"));
  actions->showArea->setIcon(KIcon("bookmark"));
  actions->showArea->setToolTip(i18n("Edit or select named areas"));
  ac->addAction("showArea", actions->showArea);
  connect(actions->showArea, SIGNAL(triggered(bool)),view, SLOT(namedAreaDialog()));

  actions->insertFunction  = new KAction(KIcon( "funct" ), i18n("&Function..."), view);
  ac->addAction("insertMathExpr", actions->insertFunction );
  connect(actions->insertFunction, SIGNAL(triggered(bool)), view, SLOT( insertMathExpr() ));
  actions->insertFunction->setToolTip(i18n("Insert math expression"));

  actions->insertSeries  = new KAction(KIcon("series" ), i18n("&Series..."), view);
  ac->addAction("series", actions->insertSeries );
  connect(actions->insertSeries, SIGNAL(triggered(bool)), view, SLOT( insertSeries() ));
  actions->insertSeries ->setToolTip(i18n("Insert a series"));

  actions->insertLink  = new KAction(KIcon( "insert_link" ), i18n("&Link..."), view);
  ac->addAction("insertHyperlink", actions->insertLink );
  connect(actions->insertLink, SIGNAL(triggered(bool)), view, SLOT( insertHyperlink() ));
  actions->insertLink->setToolTip(i18n("Insert an Internet hyperlink"));

  actions->clearHyperlink = new KAction(i18n("Link"), view);
  actions->clearHyperlink->setIconText(i18n("Remove Link"));
  actions->clearHyperlink->setToolTip(i18n("Remove a link"));
  ac->addAction("clearHyperlink", actions->clearHyperlink);
  connect(actions->clearHyperlink, SIGNAL(triggered(bool)), view, SLOT(clearHyperlink()));

  actions->insertSpecialChar = new KAction(KIcon("char"), i18n("S&pecial Character..."), view);
  ac->addAction("insertSpecialChar", actions->insertSpecialChar);
  actions->insertSpecialChar->setToolTip(i18n("Insert one or more symbols or letters not found on the keyboard"));
  connect(actions->insertSpecialChar, SIGNAL(triggered(bool)), view, SLOT(insertSpecialChar()));

  actions->insertPart = new KoPartSelectAction( i18n("&Object"), "frame_query",      view, SLOT( insertObject() ), ac, "insertPart");
  actions->insertPart->setToolTip(i18n("Insert an object from another program"));

  actions->insertChartFrame  = new KToggleAction(KIcon( "insert_chart" ), i18n("&Chart"), view);
  ac->addAction("insertChart", actions->insertChartFrame );
  connect(actions->insertChartFrame, SIGNAL(triggered(bool)), view, SLOT( insertChart() ));
  actions->insertChartFrame->setToolTip(i18n("Insert a chart"));

  actions->insertPicture  = new KAction(i18n("&Picture"), view);
  ac->addAction("insertPicture", actions->insertPicture );
  connect(actions->insertPicture, SIGNAL(triggered(bool)),view, SLOT( insertPicture() ));
  actions->insertPicture->setToolTip(i18n("Insert a picture"));

#ifndef QT_NO_SQL
  actions->insertFromDatabase  = new KAction(i18n("From &Database..."), view);
  ac->addAction("insertFromDatabase", actions->insertFromDatabase );
  connect(actions->insertFromDatabase, SIGNAL(triggered(bool)),view, SLOT( insertFromDatabase() ));
  actions->insertFromDatabase->setToolTip(i18n("Insert data from a SQL database"));
#endif

  actions->insertFromTextfile  = new KAction(i18n("From &Text File..."), view);
  ac->addAction("insertFromTextfile", actions->insertFromTextfile );
  connect(actions->insertFromTextfile, SIGNAL(triggered(bool)),view,  SLOT( insertFromTextfile() ));
  actions->insertFromTextfile->setToolTip(i18n("Insert data from a text file to the current cursor position/selection"));

  actions->insertFromClipboard  = new KAction(i18n("From &Clipboard..."), view);
  ac->addAction("insertFromClipboard", actions->insertFromClipboard );
  connect(actions->insertFromClipboard, SIGNAL(triggered(bool)),view, SLOT( insertFromClipboard() ));
  actions->insertFromClipboard->setToolTip(i18n("Insert CSV data from the clipboard to the current cursor position/selection"));

//   actions->transform = new KAction( i18n("Transform Object..."), "rotate",
//       0, view, SLOT( transformPart() ), ac, "transform" );
//   actions->transform->setToolTip(i18n("Rotate the contents of the cell"));
//   actions->transform->setEnabled( false );

  actions->sort  = new KAction(i18n("&Sort..."), view);
  ac->addAction("sort", actions->sort );
  connect(actions->sort, SIGNAL(triggered(bool)),view, SLOT( sort() ));
  actions->sort->setToolTip(i18n("Sort a group of cells"));

  actions->sortDec  = new KAction(KIcon( "sort_decrease" ), i18n("Sort &Decreasing"), view);
  ac->addAction("sortDec", actions->sortDec );
  connect(actions->sortDec, SIGNAL(triggered(bool)), view, SLOT( sortDec() ));
  actions->sortDec->setToolTip(i18n("Sort a group of cells in decreasing (last to first) order"));

  actions->sortInc  = new KAction(KIcon( "sort_incr" ), i18n("Sort &Increasing"), view);
  ac->addAction("sortInc", actions->sortInc );
  connect(actions->sortInc, SIGNAL(triggered(bool)), view, SLOT( sortInc() ));
  actions->sortInc->setToolTip(i18n("Sort a group of cells in ascending (first to last) order"));

  actions->autoFilter = new KAction(KIcon("view-filter"), i18n("&Auto-Filter"), view);
  ac->addAction("autoFilter", actions->autoFilter);
  connect(actions->autoFilter, SIGNAL(triggered(bool)), view, SLOT(autoFilter()));
  actions->autoFilter->setToolTip(i18n("Add an automatic filter to a cell range"));

  actions->paperLayout  = new KAction(i18n("Page Layout..."), view);
  ac->addAction("paperLayout", actions->paperLayout );
  connect(actions->paperLayout, SIGNAL(triggered(bool)),view, SLOT( paperLayoutDlg() ));
  actions->paperLayout->setToolTip(i18n("Specify the layout of the spreadsheet for a printout"));

  actions->definePrintRange  = new KAction(i18n("Define Print Range"), view);
  ac->addAction("definePrintRange", actions->definePrintRange );
  connect(actions->definePrintRange, SIGNAL(triggered(bool)),view, SLOT( definePrintRange() ));
  actions->definePrintRange->setToolTip(i18n("Define the print range in the current sheet"));

  actions->resetPrintRange  = new KAction(i18n("Reset Print Range"), view);
  ac->addAction("resetPrintRange", actions->resetPrintRange );
  connect(actions->resetPrintRange, SIGNAL(triggered(bool)),view, SLOT( resetPrintRange() ));
  actions->definePrintRange->setToolTip(i18n("Define the print range in the current sheet"));

  actions->showPageBorders = new KToggleAction(i18n("Page Borders"), view);
  actions->showPageBorders->setToolTip(i18n("Show on the spreadsheet where the page borders will be"));
  ac->addAction("showPageBorders", actions->showPageBorders);
  connect(actions->showPageBorders, SIGNAL(toggled(bool)), view, SLOT(togglePageBorders(bool)));

  actions->recalcWorksheet  = new KAction(i18n("Recalculate Sheet"), view);
  actions->recalcWorksheet->setIcon(KIcon("view-refresh"));
  actions->recalcWorksheet->setIconText(i18n("Recalculate"));
  ac->addAction("RecalcWorkSheet", actions->recalcWorksheet );
  actions->recalcWorksheet->setShortcut( QKeySequence( Qt::SHIFT + Qt::Key_F9));
  connect(actions->recalcWorksheet, SIGNAL(triggered(bool)),view, SLOT( recalcWorkSheet() ));
  actions->recalcWorksheet->setToolTip(i18n("Recalculate the value of every cell in the current worksheet"));

  actions->recalcWorkbook  = new KAction(i18n("Recalculate Document"), view);
  actions->recalcWorkbook->setIcon(KIcon("view-refresh"));
  actions->recalcWorkbook->setIconText(i18n("Recalculate"));
  ac->addAction("RecalcWorkBook", actions->recalcWorkbook );
  actions->recalcWorkbook->setShortcut( QKeySequence( Qt::Key_F9));
  connect(actions->recalcWorkbook, SIGNAL(triggered(bool)),view, SLOT( recalcWorkBook() ));
  actions->recalcWorkbook->setToolTip(i18n("Recalculate the value of every cell in all worksheets"));

  actions->protectSheet  = new KToggleAction(i18n("Protect &Sheet..."), view);
  ac->addAction("protectSheet", actions->protectSheet );
  actions->protectSheet->setToolTip( i18n( "Protect the sheet from being modified" ) );
  connect( actions->protectSheet, SIGNAL( toggled( bool ) ),
                    view, SLOT( toggleProtectSheet( bool ) ) );

  actions->protectDoc  = new KToggleAction(i18n("Protect &Document..."), view);
  ac->addAction("protectDoc", actions->protectDoc );
  actions->protectDoc->setToolTip( i18n( "Protect the document from being modified" ) );
  connect( actions->protectDoc, SIGNAL( toggled( bool ) ), view, SLOT( toggleProtectDoc( bool ) ) );

  // -- editing actions --

  actions->copy = ac->addAction(KStandardAction::Copy,  "copy", view, SLOT( copySelection() ));
  actions->copy->setToolTip(i18n("Copy the cell object to the clipboard"));

  actions->paste = ac->addAction(KStandardAction::Paste,  "paste", view, SLOT( paste() ));
  actions->paste->setToolTip(i18n("Paste the contents of the clipboard at the cursor"));

  actions->cut = ac->addAction(KStandardAction::Cut,  "cut", view, SLOT( cutSelection() ));
  actions->cut->setToolTip(i18n("Move the cell object to the clipboard"));

  actions->specialPaste  = new KAction(KIcon( "special_paste" ), i18n("Special Paste..."), view);
  ac->addAction("specialPaste", actions->specialPaste );
  connect(actions->specialPaste, SIGNAL(triggered(bool)), view, SLOT( specialPaste() ));
  actions->specialPaste->setToolTip(i18n("Paste the contents of the clipboard with special options"));

  actions->insertCellCopy  = new KAction(KIcon( "insertcellcopy" ), i18n("Paste with Insertion"), view);
  ac->addAction("insertCellCopy", actions->insertCellCopy );
  connect(actions->insertCellCopy, SIGNAL(triggered(bool)), view, SLOT( slotInsertCellCopy() ));
  actions->insertCellCopy->setToolTip(i18n("Inserts a cell from the clipboard into the spreadsheet"));

  actions->selectAll = ac->addAction(KStandardAction::SelectAll,  "selectAll", view, SLOT( selectAll() ));
  actions->selectAll->setToolTip(i18n("Selects all cells in the current sheet"));

  actions->find = KStandardAction::find( view, SLOT(find()), ac );
  /*actions->findNext =*/ KStandardAction::findNext( view, SLOT( findNext() ), ac );
  /*actions->findPrevious =*/ KStandardAction::findPrev( view, SLOT( findPrevious() ), ac );

  actions->replace = KStandardAction::replace( view, SLOT(replace()), ac );

    actions->fillRight = new KAction(/*KIcon("arrow-right"), */i18n("&Right"), view);
    ac->addAction("fillRight", actions->fillRight);
    connect(actions->fillRight, SIGNAL(triggered(bool)), view, SLOT(fillRight()));

    actions->fillLeft = new KAction(/*KIcon("arrow-left"), */i18n("&Left"), view);
    ac->addAction("fillLeft", actions->fillLeft);
    connect(actions->fillLeft, SIGNAL(triggered(bool)), view, SLOT(fillLeft()));

    actions->fillDown = new KAction(/*KIcon("arrow-down"), */i18n("&Down"), view);
    ac->addAction("fillDown", actions->fillDown);
    connect(actions->fillDown, SIGNAL(triggered(bool)), view, SLOT(fillDown()));

    actions->fillUp  = new KAction(/*KIcon("arrow-up"), */i18n("&Up"), view);
    ac->addAction("fillUp", actions->fillUp);
    connect(actions->fillUp, SIGNAL(triggered(bool)), view, SLOT(fillUp()));


  // -- misc actions --

  actions->styleDialog  = new KAction(i18n("Style Manager"), view);
  ac->addAction("styles", actions->styleDialog );
  connect(actions->styleDialog, SIGNAL(triggered(bool)),view, SLOT( styleDialog() ));
  actions->styleDialog->setToolTip( i18n( "Edit and organize cell styles" ) );

  actions->autoSum  = new KAction(KIcon( "black_sum" ), i18n("Autosum"), view);
  ac->addAction("autoSum", actions->autoSum );
  connect(actions->autoSum, SIGNAL(triggered(bool)), view, SLOT( autoSum() ));
  actions->autoSum->setToolTip(i18n("Insert the 'sum' function"));

  actions->spellChecking = ac->addAction(KStandardAction::Spelling,  "spelling", view, SLOT( extraSpelling() ));
  actions->spellChecking->setToolTip(i18n("Check the spelling"));

  actions->formulaSelection  = new KSelectAction(i18n("Formula Selection"), view);
  ac->addAction("formulaSelection", actions->formulaSelection );
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
  connect( actions->formulaSelection, SIGNAL( triggered( const QString& ) ),
      view, SLOT( formulaSelection( const QString& ) ) );

  actions->consolidate  = new KAction(i18n("&Consolidate..."), view);
  ac->addAction("consolidate", actions->consolidate );
  connect(actions->consolidate, SIGNAL(triggered(bool)),view, SLOT( consolidate() ));
  actions->consolidate->setToolTip(i18n("Create a region of summary data from a group of similar regions"));

  actions->goalSeek  = new KAction(i18n("&Goal Seek..."), view);
  ac->addAction("goalSeek", actions->goalSeek );
  connect(actions->goalSeek, SIGNAL(triggered(bool)),view, SLOT( goalSeek() ));
  actions->goalSeek->setToolTip( i18n("Repeating calculation to find a specific value") );

  actions->subTotals  = new KAction(i18n("&Subtotals..."), view);
  ac->addAction("subtotals", actions->subTotals );
  connect(actions->subTotals, SIGNAL(triggered(bool)),view, SLOT( subtotals() ));
  actions->subTotals->setToolTip( i18n("Create different kind of subtotals to a list or database") );

  actions->textToColumns  = new KAction(i18n("&Text to Columns..."), view);
  ac->addAction("textToColumns", actions->textToColumns );
  connect(actions->textToColumns, SIGNAL(triggered(bool)),view, SLOT( textToColumns() ));
  actions->textToColumns->setToolTip( i18n("Expand the content of cells to multiple columns") );

  actions->multipleOperations  = new KAction(i18n("&Multiple Operations..."), view);
  ac->addAction("multipleOperations", actions->multipleOperations );
  connect(actions->multipleOperations, SIGNAL(triggered(bool)),view, SLOT( multipleOperations() ));
  actions->multipleOperations->setToolTip( i18n("Apply the same formula to various cells using different values for the parameter") );

  actions->createTemplate  = new KAction(i18n("&Create Template From Document..."), view);
  ac->addAction("createTemplate", actions->createTemplate );
  connect(actions->createTemplate, SIGNAL(triggered(bool)),view, SLOT( createTemplate() ));

  actions->customList  = new KAction(i18n("Custom Lists..."), view);
  ac->addAction("sortlist", actions->customList );
  connect(actions->customList, SIGNAL(triggered(bool)),view, SLOT( sortList() ));
  actions->customList->setToolTip(i18n("Create custom lists for sorting or autofill"));

  // -- navigation actions --

  actions->gotoCell = new KAction(KIcon("go-jump" ), i18n("Goto Cell..."), view);
  actions->gotoCell->setIconText(i18n("Goto"));
  actions->gotoCell->setShortcut( QKeySequence( Qt::CTRL+Qt::Key_G ) );
  ac->addAction("gotoCell", actions->gotoCell );
  connect(actions->gotoCell, SIGNAL(triggered(bool)), view, SLOT( gotoCell() ));
  actions->gotoCell->setToolTip(i18n("Move to a particular cell"));

  actions->nextSheet  = new KAction(KIcon( "go-next" ), i18n("Next Sheet"), view);
  actions->nextSheet->setIconText(i18n("Next"));
  actions->nextSheet->setToolTip(i18n("Move to the next sheet"));
  ac->addAction("nextSheet", actions->nextSheet );
  actions->nextSheet->setShortcut( QKeySequence( Qt::CTRL+Qt::Key_PageDown));
  connect(actions->nextSheet, SIGNAL(triggered(bool)), view, SLOT( nextSheet() ));

  actions->prevSheet  = new KAction(KIcon( "go-previous" ), i18n("Previous Sheet"), view);
  actions->prevSheet->setIconText(i18n("Previous"));
  actions->prevSheet->setToolTip(i18n("Move to the previous sheet"));
  ac->addAction("previousSheet", actions->prevSheet );
  actions->prevSheet->setShortcut( QKeySequence( Qt::CTRL+Qt::Key_PageUp));
  connect(actions->prevSheet, SIGNAL(triggered(bool)), view, SLOT( previousSheet() ));

  actions->firstSheet  = new KAction(KIcon( "go-first" ), i18n("First Sheet"), view);
  actions->firstSheet->setIconText(i18n("First"));
  actions->firstSheet->setToolTip(i18n("Move to the first sheet"));
  ac->addAction("firstSheet", actions->firstSheet );
  connect(actions->firstSheet, SIGNAL(triggered(bool)), view, SLOT( firstSheet() ));

  actions->lastSheet  = new KAction(KIcon( "go-last" ), i18n("Last Sheet"), view);
  actions->lastSheet->setIconText(i18n("Last"));
  actions->lastSheet->setToolTip(i18n("Move to the last sheet"));
  ac->addAction("lastSheet", actions->lastSheet );
  connect(actions->lastSheet, SIGNAL(triggered(bool)), view, SLOT( lastSheet() ));

  // -- settings actions --

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

    actions->showFormulaBar = new KToggleAction(i18n("Formula Bar"), view);
    actions->showFormulaBar->setToolTip(i18n("Show the formula bar"));
    ac->addAction("showFormulaBar", actions->showFormulaBar);
    connect(actions->showFormulaBar, SIGNAL(toggled(bool)),
            view, SLOT(showFormulaBar(bool)));

  actions->preference  = new KAction(KIcon("configure" ), i18n("Configure KSpread..."), view);
  ac->addAction("preference", actions->preference );
  connect(actions->preference, SIGNAL(triggered(bool)), view, SLOT( preference() ));
  actions->preference->setToolTip(i18n("Set various KSpread options"));


    // -- calculation actions --
    //
    QActionGroup* groupCalc = new QActionGroup( view );
    actions->calcNone  = new KToggleAction(i18n("None"), view);
    ac->addAction("menu_none", actions->calcNone );
    connect( actions->calcNone, SIGNAL( toggled( bool ) ),
             view, SLOT( menuCalc( bool ) ) );
    actions->calcNone->setToolTip(i18n("No calculation"));
    actions->calcNone->setActionGroup( groupCalc );

    actions->calcSum  = new KToggleAction(i18n("Sum"), view);
    ac->addAction("menu_sum", actions->calcSum );
    connect( actions->calcSum, SIGNAL( toggled( bool ) ),
             view, SLOT( menuCalc( bool ) ) );
    actions->calcSum->setToolTip(i18n("Calculate using sum"));
    actions->calcSum->setActionGroup( groupCalc );

    actions->calcMin  = new KToggleAction(i18n("Min"), view);
    ac->addAction("menu_min", actions->calcMin );
    connect( actions->calcMin, SIGNAL( toggled( bool ) ),
             view, SLOT( menuCalc( bool ) ) );
    actions->calcMin->setToolTip(i18n("Calculate using minimum"));
    actions->calcMin->setActionGroup( groupCalc );

    actions->calcMax  = new KToggleAction(i18n("Max"), view);
    ac->addAction("menu_max", actions->calcMax );
    connect( actions->calcMax, SIGNAL( toggled( bool ) ),
             view, SLOT( menuCalc( bool ) ) );
    actions->calcMax->setToolTip(i18n("Calculate using maximum"));
    actions->calcMax->setActionGroup( groupCalc );

    actions->calcAverage  = new KToggleAction(i18n("Average"), view);
    ac->addAction("menu_average", actions->calcAverage );
    connect( actions->calcAverage, SIGNAL( toggled( bool ) ),
             view, SLOT( menuCalc( bool ) ) );
    actions->calcAverage->setToolTip(i18n("Calculate using average"));
    actions->calcAverage->setActionGroup( groupCalc );

    actions->calcCount  = new KToggleAction(i18n("Count"), view);
    ac->addAction("menu_count", actions->calcCount );
    connect( actions->calcCount, SIGNAL( toggled( bool ) ),
             view, SLOT( menuCalc( bool ) ) );
    actions->calcCount->setToolTip(i18n("Calculate using the count"));
    actions->calcCount->setActionGroup( groupCalc );

    actions->calcCountA  = new KToggleAction(i18n("CountA"), view);
    ac->addAction("menu_counta", actions->calcCountA );
    connect( actions->calcCountA, SIGNAL( toggled( bool ) ),
             view, SLOT( menuCalc( bool ) ) );
    actions->calcCountA->setToolTip(i18n("Calculate using the countA"));
    actions->calcCountA->setActionGroup( groupCalc );

    // -- special action, only for developers --
    //
    actions->inspector  = new KAction(KIcon( "inspector" ), i18n("Run Inspector..."), view);
    ac->addAction("inspector", actions->inspector );
    actions->inspector->setShortcut( QKeySequence( Qt::CTRL+ Qt::SHIFT + Qt::Key_I));
    connect(actions->inspector, SIGNAL(triggered(bool)), view, SLOT( runInspector() ));

    ac->associateWidget(view->canvasWidget());
    m_propertyEditor = 0;
}

void View::Private::adjustActions( bool mode )
{
  actions->replace->setEnabled( mode );
  actions->insertSeries->setEnabled( mode );
  actions->insertLink->setEnabled( mode );
  actions->insertSpecialChar->setEnabled( mode );
  actions->insertFunction->setEnabled( mode );
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
  actions->selectAll->setEnabled( mode );
  actions->clearAll->setEnabled( mode );
  actions->clearContents->setEnabled( mode );
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
  actions->deleteSheet->setEnabled( mode );
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
  actions->comment->setEnabled( mode );
  actions->insertCell->setEnabled( mode );
  actions->deleteCell->setEnabled( mode );
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
  actions->insertChartFrame->setEnabled( mode );

  actions->autoFormat->setEnabled( false );
  actions->sort->setEnabled( false );
  actions->mergeCell->setEnabled( false );
  actions->mergeCellHorizontal->setEnabled( false );
  actions->mergeCellVertical->setEnabled( false );
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

void View::Private::adjustActions( Cell cell )
{
  toolbarLock = true;

  const Style style = cell.style();

  // workaround for bug #59291 (crash upon starting from template)
  // certain Qt and Fontconfig combination fail miserably if can not
  // find the font name (e.g. not installed in the system)
  QStringList fontList;
  KFontChooser::getFontList( fontList, 0 );
  QString fontFamily = style.fontFamily();
  for ( QStringList::Iterator it = fontList.begin(); it != fontList.end(); ++it )
  {
    if ((*it).toLower() == fontFamily.toLower())
    {
      actions->selectFont->setFont( fontFamily );
      break;
    }
  }

  actions->selectFontSize->setFontSize( style.fontSize() );
  actions->bold->setChecked( style.bold() );
  actions->italic->setChecked( style.italic() );
  actions->underline->setChecked( style.underline() );
  actions->strikeOut->setChecked( style.strikeOut() );

  actions->alignLeft->setChecked( style.halign() == Style::Left );
  actions->alignCenter->setChecked( style.halign() == Style::Center );
  actions->alignRight->setChecked( style.halign() == Style::Right );

  actions->alignTop->setChecked( style.valign() == Style::Top );
  actions->alignMiddle->setChecked( style.valign() == Style::Middle );
  actions->alignBottom->setChecked( style.valign() == Style::Bottom );

  actions->verticalText->setChecked( style.verticalText() );

  actions->wrapText->setChecked( style.wrapText() );

  Format::Type ft = style.formatType();
  actions->percent->setChecked( ft == Format::Percentage );
  actions->money->setChecked( ft == Format::Money );

  if ( activeSheet && !activeSheet->isProtected() )
    actions->clearComment->setEnabled( !cell.comment().isEmpty() );

  if ( activeSheet && !activeSheet->isProtected() )
    actions->decreaseIndent->setEnabled( style.indentation() > 0.0 );

  toolbarLock = false;

  if ( activeSheet && activeSheet->isProtected() && style.notProtected() )
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
  else if ( activeSheet->isProtected() )
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
  actions->duplicateSheet->setEnabled( mode );
  actions->deleteSheet->setEnabled( mode );

  if ( mode )
  {
    if ( activeSheet && !activeSheet->isProtected() )
    {
      bool state = ( view->doc()->map()->visibleSheets().count() > 1 );
      actions->deleteSheet->setEnabled( state );
      actions->hideSheet->setEnabled( state );
    }
    actions->showSheet->setEnabled( view->doc()->map()->hiddenSheets().count() > 0 );
    actions->renameSheet->setEnabled( activeSheet && !activeSheet->isProtected() );
  }
}

QAbstractButton* View::Private::newIconButton( const char *_file, bool _kbutton, QWidget *_parent )
{
  if ( _parent == 0 )
    _parent = view;

  if ( !_kbutton ) {
    QPushButton* pb = new QPushButton( _parent );
    pb->setIcon( KIcon(_file) );
    return pb;
  } else {
    QToolButton* pb = new QToolButton( _parent );
    pb->setIcon( KIcon(_file) );
    return pb;
  }
}


/*****************************************************************************
 *
 * View
 *
 *****************************************************************************/

View::View( QWidget *_parent, Doc *_doc )
    : KoView( _doc, _parent )
    , d( new Private )
{
    ElapsedTime et( "View constructor" );
//     kDebug() <<"sizeof(Cell)=" << sizeof(Cell);

    d->view = this;
    d->doc = _doc;

    d->activeSheet = 0;

    d->toolbarLock = false;
    d->loading = true;

    d->selection = new Selection( this );
    d->choice = new Selection( this );
    d->choice->setMultipleOccurences(true);
    connect(d->selection, SIGNAL(changed(const Region&)), this, SLOT(slotChangeSelection(const Region&)));
    connect(d->choice, SIGNAL(changed(const Region&)), this, SLOT(slotChangeChoice(const Region&)));
    connect(d->choice, SIGNAL(changed(const Region&)), this, SLOT(slotScrollChoice(const Region&)));

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
    d->spell.currentCellIndex = 0;
    d->spell.spellStartCellX = 0;
    d->spell.spellStartCellY = 0;
    d->spell.spellEndCellX   = 0;
    d->spell.spellEndCellY   = 0;
    d->spell.spellCheckSelection = false;

#if 0 // KSPREAD_KOPART_EMBEDDING
    d->insertHandler = 0;
#endif // KSPREAD_KOPART_EMBEDDING
    d->specialCharDlg = 0;

    setComponentData( Factory::global() );
    if ( doc()->isReadWrite() )
      setXMLFile( "kspread.rc" );
    else
      setXMLFile( "kspread_readonly.rc" );

    // GUI Initializations
    initView();

    d->initActions();

#if 0 // KSPREAD_KOPART_EMBEDDING
    // Handler for moving and resizing embedded parts
    KoContainerHandler* h = new KoContainerHandler( this, d->canvas );
    connect( h, SIGNAL( popupMenu( KoChild*, const QPoint& ) ), this, SLOT( popupChildMenu( KoChild*, const QPoint& ) ) );
#endif // KSPREAD_KOPART_EMBEDDING

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

    connect( doc()->map(), SIGNAL( sig_addSheet( Sheet* ) ), SLOT( slotAddSheet( Sheet* ) ) );

    connect( doc(), SIGNAL( sig_refreshView(  ) ), this, SLOT( slotRefreshView() ) );

    connect( doc(), SIGNAL( sig_refreshLocale() ), this, SLOT( refreshLocale()));

    connect(doc()->namedAreaManager(), SIGNAL(namedAreaAdded(const QString&)),
            d->posWidget, SLOT(slotAddAreaName(const QString&)));
    connect(doc()->namedAreaManager(), SIGNAL(namedAreaRemoved(const QString&)),
            d->posWidget, SLOT(slotRemoveAreaName(const QString&)));
    connect(doc(), SIGNAL(damagesFlushed(const QList<Damage*>&)),
            this, SLOT(handleDamages(const QList<Damage*>&)));

    if (!doc()->isReadWrite())
    {
        setZoom( 100, true );
    }

    // ## Might be wrong, if doc isn't loaded yet
    d->actions->selectStyle->setItems( d->doc->styleManager()->styleNames() );

    // Delay the setting of the initial position, because
    // we have to wait for the widget to be shown. Otherwise,
    // we get a wrong widget size.
    // This is the last operation for the "View loading" process.
    // The loading flag will be unset at its end.
    if ( !doc()->map()->sheetList().isEmpty() )
      QTimer::singleShot(50, this, SLOT(initialPosition()));

    connect (&d->statusBarOpTimer, SIGNAL(timeout()), this, SLOT(calcStatusBarOp()));

    new ViewAdaptor(this);
}

View::~View()
{
    KoToolManager::instance()->removeCanvasController( d->canvasController );

    //  ElapsedTime el( "~View" );
    if ( doc()->isReadWrite() ) // make sure we're not embedded in Konq
        deleteEditor( true );
#if 0 // KSPREAD_KOPART_EMBEDDING
    if ( !d->transformToolBox.isNull() )
        delete (&*d->transformToolBox);
#endif // KSPREAD_KOPART_EMBEDDING
    /*if (d->calcLabel)
    {
        disconnect(d->calcLabel,SIGNAL(pressed( int )),this,SLOT(statusBarClicked(int)));

        }*/

    qDeleteAll( d->sheetViews );
    delete d->spell.kspell;

    d->canvas->endChoose();
    d->activeSheet = 0; // set the active sheet to 0 so that when during destruction
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
//     delete d->dcop;

#if 0 // KSPREAD_KOPART_EMBEDDING
    delete d->insertHandler;
    d->insertHandler = 0;
#endif // KSPREAD_KOPART_EMBEDDING

    delete d->actions;
    delete d->zoomController;
    delete d->zoomHandler;
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
    d->viewLayout = new QGridLayout( this );
    d->viewLayout->setMargin(0);
    d->viewLayout->setSpacing(0);

    // Edit Bar
    d->toolWidget = new QFrame( this );

    d->formulaBarLayout = new QHBoxLayout( d->toolWidget );
    d->formulaBarLayout->setMargin( 4 );
    d->formulaBarLayout->setSpacing( 0 );

    d->posWidget = new ComboboxLocationEditWidget( d->toolWidget, this );
    d->posWidget->setMinimumWidth( 100 );
    d->formulaBarLayout->addWidget( d->posWidget );
    d->formulaBarLayout->addSpacing( 4 );

    d->formulaButton = d->newIconButton( "funct", true, d->toolWidget );
    d->formulaBarLayout->addWidget( d->formulaButton );
    connect( d->formulaButton, SIGNAL( clicked() ), SLOT( insertMathExpr() ) );
    d->formulaBarLayout->addSpacing( 2 );

    d->cancelButton = d->newIconButton( "cancel", true, d->toolWidget );
    d->formulaBarLayout->addWidget( d->cancelButton );
    d->okButton = d->newIconButton( "ok", true, d->toolWidget );
    d->formulaBarLayout->addWidget( d->okButton );
    d->formulaBarLayout->addSpacing( 4 );

/****** FIXME TEST new colorset widget (simply remove these lines when testing is done) ****/
    QToolButton *tb = new QToolButton(d->toolWidget);
    tb->setDefaultAction(new KoColorSetAction(d->toolWidget));
    d->formulaBarLayout->addWidget(tb);
    d->formulaBarLayout->addSpacing( 4 );
/****** FIXME TEST end *********/

    // Setup the Canvas and its controller.
    d->canvas = new Canvas( this );
    d->canvasController = new KoCanvasController( this );
    d->canvasController->setCanvas( d->canvas );
    d->canvasController->setCanvasMode( KoCanvasController::Infinite );
    d->canvasController->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    d->canvasController->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    connect( d->canvas, SIGNAL(documentSizeChanged(const QSize&)),
             d->canvasController, SLOT(setDocumentSize(const QSize&)));
    connect( d->canvasController, SIGNAL(moveDocumentOffset(const QPoint&)),
             d->canvas, SLOT(setDocumentOffset(const QPoint&)));

    // Let the selection pointer become a canvas resource.
    QVariant variant;
    variant.setValue<void*>( d->selection );
    d->canvas->resourceProvider()->setResource( Canvas::Selection, variant );

    // Setup the tool dock widget.
    KoToolRegistry::instance()->add(s_defaultToolFactory);
    KoToolManager::instance()->addController( d->canvasController );
    KoToolManager::instance()->registerTools( actionCollection(), d->canvasController );
    KoToolBoxFactory toolBoxFactory( d->canvasController, "KSpread" );
    createDockWidget( &toolBoxFactory );

    // Setup the tool options dock widget.
    KoToolDockerFactory toolDockerFactory;
    KoToolDocker *td =  dynamic_cast<KoToolDocker*>( createDockWidget( &toolDockerFactory ) );
    connect(d->canvasController, SIGNAL(toolOptionWidgetChanged(QWidget*)), td, SLOT(newOptionWidget(QWidget*)));

    // Activate the cell tool.
    KoToolManager::instance()->switchToolRequested(KSPREAD_DEFAULT_TOOL_ID);

    // Setup the zoom controller.
    d->zoomHandler = new KoZoomHandler();
    d->zoomController = new KoZoomController( d->canvasController, d->zoomHandler, actionCollection(), false );
    d->zoomController->zoomAction()->setZoomModes( KoZoomMode::ZOOM_CONSTANT );
    addStatusBarItem( d->zoomController->zoomAction()->createWidget( statusBar() ), 0, true );
    connect( d->zoomController, SIGNAL(zoomChanged(KoZoomMode::Mode, double)),
             this, SLOT(viewZoom(KoZoomMode::Mode, double)) );

    // The line-editor that appears above the sheet and allows to
    // edit the cells content. It knows about the two buttons.
    d->editWidget = new EditWidget( d->toolWidget, d->canvas, d->cancelButton, d->okButton );
    d->editWidget->setFocusPolicy( Qt::StrongFocus );
    d->formulaBarLayout->addWidget( d->editWidget, 2 );

    d->canvas->setEditWidget( d->editWidget );
    d->hBorderWidget = new HBorder( this, d->canvas,this );
    d->vBorderWidget = new VBorder( this, d->canvas ,this );
    d->hBorderWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
    d->vBorderWidget->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding );
    d->selectAllButton = new SelectAllButton( this );
    d->selectAllButton->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

    d->canvas->setFocusPolicy( Qt::StrongFocus );
    QWidget::setFocusPolicy( Qt::StrongFocus );
    setFocusProxy( d->canvas );

    connect( this, SIGNAL( invalidated() ), d->canvas, SLOT( update() ) );

    // Vert. Scroll Bar
    d->calcLabel  = 0;
    d->vertScrollBar = new QScrollBar( this );
    d->canvasController->setVerticalScrollBar( d->vertScrollBar );
//     d->vertScrollBar->setRange( 0, 4096 );
    d->vertScrollBar->setOrientation( Qt::Vertical );
    d->vertScrollBar->setSingleStep(60);  //just random guess based on what feels okay
    d->vertScrollBar->setPageStep(60);  //This should be controlled dynamically, depending on how many rows are shown

    QWidget* bottomPart = new QWidget( this );
    d->tabScrollBarLayout = new QGridLayout( bottomPart );
    d->tabScrollBarLayout->setMargin(0);
    d->tabScrollBarLayout->setSpacing(0);
    d->tabBar = new KoTabBar( 0 );
    d->tabScrollBarLayout->addWidget( d->tabBar, 0, 0 );
    d->horzScrollBar = new QScrollBar( 0 );
    d->canvasController->setHorizontalScrollBar( d->horzScrollBar );
    d->tabScrollBarLayout->addWidget( d->horzScrollBar, 0, 1, 2, 1, Qt::AlignVCenter);

//     d->horzScrollBar->setRange( 0, 4096 );
    d->horzScrollBar->setOrientation( Qt::Horizontal );
    d->horzScrollBar->setSingleStep(60); //just random guess based on what feels okay
    d->horzScrollBar->setPageStep(60);

    connect( d->tabBar, SIGNAL( tabChanged( const QString& ) ), this, SLOT( changeSheet( const QString& ) ) );
    connect( d->tabBar, SIGNAL( tabMoved( unsigned, unsigned ) ),
      this, SLOT( moveSheet( unsigned, unsigned ) ) );
    connect( d->tabBar, SIGNAL( contextMenu( const QPoint& ) ),
      this, SLOT( popupTabBarMenu( const QPoint& ) ) );
    connect( d->tabBar, SIGNAL( doubleClicked() ),
      this, SLOT( slotRename() ) );

    int extent = this->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    if (style()->styleHint(QStyle::SH_ScrollView_FrameOnlyAroundContents)) {
        extent += style()->pixelMetric(QStyle::PM_DefaultFrameWidth) * 2;
    }

    d->viewLayout->setColumnStretch( 1, 10 );
    d->viewLayout->setRowStretch( 2, 10 );
    d->viewLayout->addWidget( d->toolWidget, 0, 0, 1, 3 );
    d->viewLayout->addWidget( d->selectAllButton, 1, 0 );
    d->viewLayout->addWidget( d->hBorderWidget, 1, 1, 1, 1 );
    d->viewLayout->addWidget( d->vBorderWidget, 2, 0 );
    d->viewLayout->addWidget( d->canvasController, 2, 1 );
    d->viewLayout->addWidget( d->vertScrollBar, 1, 2, 2, 1, Qt::AlignHCenter);
    d->viewLayout->addWidget( bottomPart, 3, 0, 1, 2 );
    d->viewLayout->setColumnMinimumWidth(2, extent);
    d->viewLayout->setRowMinimumHeight(3, extent);

    KStatusBar * sb = statusBar();
    d->calcLabel = sb ? new QLabel( sb ) : 0;
    if (d->calcLabel) {
        d->calcLabel->setContextMenuPolicy(Qt::CustomContextMenu);
        addStatusBarItem( d->calcLabel, 0 );
        connect(d->calcLabel ,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(statusBarClicked(QPoint)));
    }

    // signal slot
//     connect( d->vertScrollBar, SIGNAL( valueChanged(int) ), d->canvas, SLOT( slotScrollVert(int) ) );
//     connect( d->horzScrollBar, SIGNAL( valueChanged(int) ), d->canvas, SLOT( slotScrollHorz(int) ) );
}

Canvas* View::canvasWidget() const
{
    return d->canvas;
}

KoCanvasController* View::canvasController() const
{
    return d->canvasController;
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

Selection* View::choice() const
{
  return d->choice;
}

#if 0 // KSPREAD_KOPART_EMBEDDING
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
#endif // KSPREAD_KOPART_EMBEDDING

const Sheet* View::activeSheet() const
{
    return d->activeSheet;
}

Sheet* View::activeSheet()
{
    return d->activeSheet;
}

SheetView* View::sheetView( const Sheet* sheet ) const
{
    if ( !d->sheetViews.contains( sheet ) )
    {
        kDebug(36004) <<"View: Creating SheetView for" << sheet->sheetName();
        d->sheetViews.insert( sheet, new SheetView( sheet ) );
        d->sheetViews[ sheet ]->setPaintDevice( d->canvas );
        d->sheetViews[ sheet ]->setViewConverter( zoomHandler() );
        connect( d->sheetViews[ sheet ], SIGNAL(visibleSizeChanged(const QSizeF&)),
                 d->canvas, SLOT(setDocumentSize(const QSizeF&)) );
        connect( d->sheetViews[ sheet ], SIGNAL(visibleSizeChanged(const QSizeF&)),
                 d->zoomController, SLOT(setDocumentSize(const QSizeF&)) );
        connect( sheet, SIGNAL(visibleSizeChanged()),
                 d->sheetViews[ sheet ], SLOT(updateAccessedCellRange()));
    }
    return d->sheetViews[ sheet ];
}

void View::refreshSheetViews()
{
    const QList<SheetView*> sheetViews = d->sheetViews.values();
    for (int i = 0; i < sheetViews.count(); ++i)
    {
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

void View::initConfig()
{
    KSharedConfigPtr config = Factory::global().config();
    const KConfigGroup parameterGroup = config->group( "Parameters" );
    const bool configFromDoc = doc()->configLoadFromFile();
    if ( !configFromDoc )
    {
        doc()->setShowHorizontalScrollBar(parameterGroup.readEntry("Horiz ScrollBar",true));
        doc()->setShowVerticalScrollBar(parameterGroup.readEntry("Vert ScrollBar",true));
    }
    doc()->setShowColumnHeader(parameterGroup.readEntry("Column Header",true));
    doc()->setShowRowHeader(parameterGroup.readEntry("Row Header",true));
    if ( !configFromDoc )
        doc()->setCompletionMode((KGlobalSettings::Completion)parameterGroup.readEntry("Completion Mode",(int)(KGlobalSettings::CompletionAuto)));
    doc()->setMoveToValue((KSpread::MoveTo)parameterGroup.readEntry("Move",(int)(Bottom)));
    doc()->setIndentValue( parameterGroup.readEntry( "Indent", 10.0 ) );
    doc()->setTypeOfCalc((MethodOfCalc)parameterGroup.readEntry("Method of Calc",(int)(SumOfNumber)));
    if ( !configFromDoc )
        doc()->setShowTabBar(parameterGroup.readEntry("Tabbar",true));

    doc()->setShowMessageError(parameterGroup.readEntry( "Msg error" ,false) );

    doc()->setShowFormulaBar(parameterGroup.readEntry("Formula bar",true));
    doc()->setShowStatusBar(parameterGroup.readEntry("Status bar",true));

    changeNbOfRecentFiles(parameterGroup.readEntry("NbRecentFile",10));
    //autosave value is stored as a minute.
    //but default value is stored as seconde.
    doc()->setAutoSave(parameterGroup.readEntry("AutoSave",KoDocument::defaultAutoSave()/60)*60);
    doc()->setBackupFile( parameterGroup.readEntry("BackupFile",true));

    const KConfigGroup colorGroup = config->group( "KSpread Color" );
    doc()->setGridColor( colorGroup.readEntry( "GridColor", QColor(Qt::lightGray) ) );
    doc()->changePageBorderColor( colorGroup.readEntry( "PageBorderColor", QColor(Qt::red) ) );

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
    doc()->setCaptureAllArrowKeys( config->group( "Editor" ).readEntry( "CaptureAllArrowKeys", true ) );

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
    doc()->recalcManager()->recalcMap();
}

void View::refreshLocale()
{
  foreach ( Sheet* sheet, doc()->map()->sheetList() )
  {
    sheet->updateLocale();
  }
}

void View::recalcWorkSheet()
{
    if ( !activeSheet() )
        return;
    doc()->recalcManager()->recalcSheet( activeSheet() );
}


void View::extraSpelling()
{
  if ( d->spell.kspell )
    return; // Already in progress

  if (d->activeSheet == 0)
    return;

  d->spell.macroCmdSpellCheck = 0;
  d->spell.firstSpellSheet    = d->activeSheet;
  d->spell.currentSpellSheet  = d->spell.firstSpellSheet;

  QRect selection = d->selection->lastRange();

  // if nothing is selected, check every cell
  if (d->selection->isSingular())
  {
    d->spell.spellStartCellX = 0;
    d->spell.spellStartCellY = 0;
    d->spell.spellEndCellX   = 0;
    d->spell.spellEndCellY   = 0;
    d->spell.spellCheckSelection = false;
    d->spell.currentCellIndex = 0;
  }
  else
  {
    d->spell.spellStartCellX = selection.left();
    d->spell.spellStartCellY = selection.top();
    d->spell.spellEndCellX   = selection.right();
    d->spell.spellEndCellY   = selection.bottom();
    d->spell.spellCheckSelection = true;
    d->spell.currentCellIndex = 0;

    // "-1" because X gets increased every time we go into spellCheckReady()
    d->spell.spellCurrCellX = d->spell.spellStartCellX - 1;
    d->spell.spellCurrCellY = d->spell.spellStartCellY;
  }

  startKSpell();
}


void View::startKSpell()
{
#ifdef __GNUC__
#warning TODO KDE4 port to sonnet
#endif
#if 0
    if ( doc()->getKSpellConfig() )
    {
        doc()->getKSpellConfig()->setIgnoreList( doc()->spellListIgnoreAll() );
        doc()->getKSpellConfig()->setReplaceAllList( d->spell.replaceAll );

    }
    d->spell.kspell = new K3Spell( this, i18n( "Spell Checking" ), this,
                                       SLOT( spellCheckerReady() ),
                                       doc()->getKSpellConfig() );

  d->spell.kspell->setIgnoreUpperWords( doc()->dontCheckUpperWord() );
  d->spell.kspell->setIgnoreTitleCase( doc()->dontCheckTitleCase() );

  connect( d->spell.kspell, SIGNAL( death() ),
                    this, SLOT( spellCheckerFinished() ) );
  connect( d->spell.kspell, SIGNAL( misspelling( const QString &,
                                                         const QStringList &,
                                                         unsigned int) ),
                    this, SLOT( spellCheckerMisspelling( const QString &,
                                                         const QStringList &,
                                                         unsigned int) ) );
  connect( d->spell.kspell, SIGNAL( corrected( const QString &,
                                                       const QString &,
                                                       unsigned int) ),
                    this, SLOT( spellCheckerCorrected( const QString &,
                                                       const QString &,
                                                       unsigned int ) ) );
  connect( d->spell.kspell, SIGNAL( done( const QString & ) ),
                    this, SLOT( spellCheckerDone( const QString & ) ) );
  connect( d->spell.kspell, SIGNAL( ignoreall (const QString & ) ),
                    this, SLOT( spellCheckerIgnoreAll( const QString & ) ) );

  connect( d->spell.kspell, SIGNAL( replaceall( const QString &  ,  const QString & )), this, SLOT( spellCheckerReplaceAll( const QString &  ,  const QString & )));
#endif
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
    while ( d->spell.currentCellIndex < d->spell.currentSpellSheet->valueStorage()->count() )
    {
        Sheet* sheet = d->spell.currentSpellSheet;
        const ValueStorage* valueStorage = sheet->valueStorage();
        int index = d->spell.currentCellIndex;
        Cell currentCell( sheet, valueStorage->col( index ), valueStorage->row( index ) );

      // check text only
      if ( currentCell.value().isString() )
      {
        d->spell.kspell->check( currentCell.userInput(), true );

        return;
      }

      d->spell.currentCellIndex++;
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
      Cell cell = Cell( d->spell.currentSpellSheet, x, y );

      // check text only
      if (!cell.value().isString())
        continue;

      d->spell.spellCurrCellX = x;
      d->spell.spellCurrCellY = y;

      d->spell.kspell->check( cell.userInput(), true );

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
  d->spell.kspell            = 0;
  d->spell.firstSpellSheet   = 0;
  d->spell.currentSpellSheet = 0;
  d->spell.currentCellIndex  = 0;
  d->spell.replaceAll.clear();


  KMessageBox::information( this, i18n( "Spell checking is complete." ) );

  if ( d->spell.macroCmdSpellCheck )
    doc()->addCommand (d->spell.macroCmdSpellCheck);
  d->spell.macroCmdSpellCheck=0;
}


bool View::spellSwitchToOtherSheet()
{
  // there is no other sheet
  if ( doc()->map()->count() == 1 )
    return false;

  // for optimization
  QList<Sheet*> sheetList = doc()->map()->sheetList();

  int curIndex = sheetList.lastIndexOf(d->spell.currentSpellSheet);
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
    d->spell.spellEndCellX = d->spell.currentSpellSheet->cellStorage()->columns();
    d->spell.spellEndCellY = d->spell.currentSpellSheet->cellStorage()->rows();

    d->spell.spellCurrCellX = d->spell.spellStartCellX - 1;
    d->spell.spellCurrCellY = d->spell.spellStartCellY;
  }
  else
  {
    d->spell.currentCellIndex = 0;
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
    d->spell.spellCurrCellX = d->spell.currentSpellSheet->valueStorage()->col( d->spell.currentCellIndex );
    d->spell.spellCurrCellY = d->spell.currentSpellSheet->valueStorage()->row( d->spell.currentCellIndex );
  }

  d->selection->initialize(QPoint(d->spell.spellCurrCellX, d->spell.spellCurrCellY));
}


void View::spellCheckerCorrected( const QString & old, const QString & corr, unsigned int pos )
{
    Cell cell;

    if (d->spell.spellCheckSelection)
    {
        cell = Cell( d->spell.currentSpellSheet,
                     d->spell.spellCurrCellX,
                     d->spell.spellCurrCellY );
    }
    else
    {
        Sheet* sheet = d->spell.currentSpellSheet;
        const ValueStorage* valueStorage = sheet->valueStorage();
        int index = d->spell.currentCellIndex;
        cell = Cell( sheet, valueStorage->col( index ), valueStorage->row( index ) );
        d->spell.spellCurrCellX = d->spell.currentSpellSheet->valueStorage()->col( d->spell.currentCellIndex );
        d->spell.spellCurrCellY = d->spell.currentSpellSheet->valueStorage()->row( d->spell.currentCellIndex );
    }

  Q_ASSERT( cell );
  if ( !cell )
    return;

  QString content (cell.userInput());
  content.replace (pos, old.length(), corr);

  DataManipulator *manipulator = new DataManipulator;
  manipulator->setSheet (d->activeSheet);
  manipulator->setText (i18n ("Correct Misspelled Word"));
  manipulator->setValue (Value (content));
  manipulator->setParsing (false);
  manipulator->add (QPoint (cell.column(), cell.row()));
  manipulator->setRegisterUndo (false);
  manipulator->execute ();

  // the manipulator doesn't register itself for the undo, instead, we
  // put all manipulators into one macro action, which will undo everything
  // at once
  if (!d->spell.macroCmdSpellCheck) {
    MacroCommand *mm = new MacroCommand;
    mm->setText (i18n("Correct Misspelled Word"));
    mm->setSheet (d->activeSheet);
    mm->setRegisterUndo (false);
    d->spell.macroCmdSpellCheck = mm;
  }
  d->spell.macroCmdSpellCheck->add (manipulator);
}

void View::spellCheckerDone( const QString & )
{
    int result = d->spell.kspell->dlgResult();

    d->spell.kspell->cleanUp();
    delete d->spell.kspell;
    d->spell.kspell = 0;

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
            if ( ++d->spell.currentCellIndex < d->spell.currentSpellSheet->valueStorage()->count() )
            {
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
    d->spell.macroCmdSpellCheck=0;
}

void View::spellCheckerFinished()
{
  if (d->canvas)
    d->canvas->setCursor( Qt::ArrowCursor );

  K3Spell::spellStatus status = d->spell.kspell->status();
  d->spell.kspell->cleanUp();
  delete d->spell.kspell;
  d->spell.kspell = 0;
  d->spell.replaceAll.clear();

  bool kspellNotConfigured=false;

  if (status == K3Spell::Error)
  {
    KMessageBox::sorry(this, i18n("ISpell could not be started.\n"
                                  "Please make sure you have ISpell properly configured and in your PATH."));
    kspellNotConfigured=true;
  }
  else if (status == K3Spell::Crashed)
  {
    KMessageBox::sorry(this, i18n("ISpell seems to have crashed."));
  }

  if (d->spell.macroCmdSpellCheck)
  {
      doc()->addCommand( d->spell.macroCmdSpellCheck );
  }
  d->spell.macroCmdSpellCheck=0;


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
    foreach ( Sheet* sheet, doc()->map()->sheetList() )
    {
        addSheet( sheet );
    }

    // Set the initial X and Y offsets for the view (OpenDocument loading)
    if ( LoadingInfo* loadingInfo = doc()->loadingInfo() )
    {
        d->savedAnchors = loadingInfo->cursorPositions();
        d->savedMarkers = loadingInfo->cursorPositions();
        d->savedOffsets = loadingInfo->scrollingOffsets();
    }

    Sheet* sheet = doc()->map()->initialActiveSheet();
    if ( !sheet )
    {
        //activate first table which is not hiding
        sheet = doc()->map()->findSheet( doc()->map()->visibleSheets().first());
        if ( !sheet )
        {
            sheet = doc()->map()->sheet( 0 );
            if ( sheet )
            {
                sheet->setHidden( false );
                QString tabName = sheet->sheetName();
                d->tabBar->addTab( tabName );
            }
        }
    }
    setActiveSheet( sheet );

    // Set the initial X and Y offsets for the view (Native format loading)
    if ( !doc()->loadingInfo() )
    {
        const int offsetX = (int)zoomHandler()->documentToViewX( doc()->map()->initialXOffset() );
        const int offsetY = (int)zoomHandler()->documentToViewY( doc()->map()->initialYOffset() );
        // Set the initial position for the marker as stored in the XML file,
        // (1,1) otherwise
        int col = doc()->map()->initialMarkerColumn();
        if ( col <= 0 )
            col = 1;
        int row = doc()->map()->initialMarkerRow();
        if ( row <= 0 )
            row = 1;
        d->canvas->setDocumentOffset( QPoint( offsetX, offsetY ) );
        d->horzScrollBar->setValue( offsetX );
        d->vertScrollBar->setValue( offsetY );
        d->selection->initialize( QPoint(col, row) );
    }

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

    if ( koDocument()->isReadWrite() )
        initConfig();

    d->canvas->setFocus();

    // finish the "View Loading" process
    d->loading = false;
    doc()->deleteLoadingInfo();

    refreshView();
}


void View::updateEditWidgetOnPress()
{
    if (!d->activeSheet)
      return;

    int column = selection()->marker().x();
    int row    = selection()->marker().y();

    Cell cell = Cell( d->activeSheet, column, row );
    if ( !cell )
    {
        d->editWidget->setText( "" );
        return;
    }
    const Style style = cell.style();
    if ( d->activeSheet->isProtected() && style.hideFormula() )
        d->editWidget->setText( cell.displayText() );
    else if ( d->activeSheet->isProtected() && style.hideAll() )
        d->editWidget->setText( "" );
    else
        d->editWidget->setText( cell.userInput() );

    d->adjustActions( cell );
}

void View::updateEditWidget()
{
    if (!d->activeSheet)
      return;

    const int column = selection()->marker().x();
    const int row    = selection()->marker().y();

    const Cell cell = Cell( d->activeSheet, column, row );
    const Style style = cell.style();
    const bool active = activeSheet()->getShowFormula()
        && !( d->activeSheet->isProtected() && style.hideFormula() );

    if ( d->activeSheet && !d->activeSheet->isProtected() )
    {
      d->actions->alignLeft->setEnabled(!active);
      d->actions->alignCenter->setEnabled(!active);
      d->actions->alignRight->setEnabled(!active);
    }

    if ( d->activeSheet->isProtected() && style.hideFormula() )
        d->editWidget->setText( cell.displayText() );
    else if ( d->activeSheet->isProtected() && style.hideAll() )
        d->editWidget->setText( "" );
    else
        d->editWidget->setText( cell.userInput() );

    if ( d->activeSheet->isProtected() && !style.notProtected() )
      d->editWidget->setEnabled( false );
    else
      d->editWidget->setEnabled( true );

    if ( d->canvas->editor() )
    {
      d->canvas->editor()->setEditorFont(style.font(), true, zoomHandler());
      d->canvas->editor()->setFocus();
    }

    d->adjustActions( cell );
}

void View::activateFormulaEditor()
{
}

void View::objectSelectedChanged()
{
#if 0 // KSPREAD_KOPART_EMBEDDING
  if ( d->canvas->isObjectSelected() )
    d->actions->actionExtraProperties->setEnabled( true );
  else
    d->actions->actionExtraProperties->setEnabled( false );
#endif // KSPREAD_KOPART_EMBEDDING
}

void View::updateReadWrite( bool readwrite )
{
    // d->cancelButton->setEnabled( readwrite );
    // d->okButton->setEnabled( readwrite );
  d->editWidget->setEnabled( readwrite );

    const QList<QAction*> actions = actionCollection()->actions();
    for (int i = 0; i < actions.count(); ++i)
    {
        // The action collection contains also the flake tool actions. Skip them.
        if (actions[i]->parent() == this)
            actions[i]->setEnabled( readwrite );
    }

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

  KTemporaryFile tempFile;
  tempFile.setSuffix(".kst");
  //Check that creation of temp file was successful
  if (!tempFile.open())
  {
    qWarning("Creation of temprary file to store template failed.");
    return;
  }

  doc()->saveNativeFormat( tempFile.fileName() );

  KoTemplateCreateDia::createTemplate( "kspread_template", Factory::global(),
                                           tempFile.fileName(), pix, this );

  Factory::global().dirs()->addResourceType("kspread_template",
                                            "data", "kspread/templates/");
}

void View::sheetFormat()
{
    AutoFormatDialog dlg( this );
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
  QRect sel = d->selection->lastRange();

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

      if ( (d->selection->marker().y() > 1) && Cell( activeSheet(),d->selection->marker().x(), d->selection->marker().y()-1).value().isNumber() )
      {
        // check cells above the current one
        start = end = d->selection->marker().y()-1;
        for (start--; (start > 0) && Cell( activeSheet(),d->selection->marker().x(), start).value().isNumber(); start--) ;

        const Region region(QRect(QPoint(d->selection->marker().x(), start + 1),
                                  QPoint(d->selection->marker().x(), end)), activeSheet());
        const QString str = region.name(activeSheet());

        d->canvas->createEditor();
        d->canvas->editor()->setText("=SUM(" + str + ')');
        d->canvas->editor()->setCursorPosition(5 + str.length());
        return;
      }
      else if ( (d->selection->marker().x() > 1) && Cell( activeSheet(),d->selection->marker().x()-1, d->selection->marker().y()).value().isNumber() )
      {
        // check cells to the left of the current one
        start = end = d->selection->marker().x()-1;
        for (start--; (start > 0) && Cell( activeSheet(),start, d->selection->marker().y()).value().isNumber(); start--) ;

        const Region region(QRect(QPoint(start + 1, d->selection->marker().y()),
                                  QPoint(end, d->selection->marker().y())), activeSheet());
        const QString str = region.name(activeSheet());

        d->canvas->createEditor();
        d->canvas->editor()->setText("=SUM(" + str + ')');
        d->canvas->editor()->setCursorPosition(5 + str.length());
        return;
      }
    }
  }

  if ( (sel.width() > 1) && (sel.height() > 1) )
    sel=QRect();

  d->canvas->createEditor();

  const Region region(sel, activeSheet());
  if (region.isValid())
  {
    d->canvas->editor()->setText("=SUM(" + region.name(activeSheet()) + ')');
    d->canvas->deleteEditor(true);
  }
  else
  {
    d->canvas->startChoose();
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
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Text Color") );
    manipulator->setFontColor( d->canvas->resourceProvider()->foregroundColor().toQColor() );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::setSelectionTextColor(const QColor &txtColor)
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Text Color") );
    manipulator->setFontColor( txtColor );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::changeBackgroundColor()
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Background Color") );
    manipulator->setBackgroundColor( d->canvas->resourceProvider()->backgroundColor().toQColor() );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::setSelectionBackgroundColor(const QColor &bgColor)
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Background Color") );
    manipulator->setBackgroundColor( bgColor );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::changeBorderColor()
{
    BorderColorCommand* command = new BorderColorCommand();
    command->setSheet(d->activeSheet);
    command->setColor(d->canvas->resourceProvider()->foregroundColor().toQColor());
    command->add(*selection());
    command->execute();
}

void View::setSelectionBorderColor(const QColor &bdColor)
{
    BorderColorCommand* command = new BorderColorCommand();
    command->setSheet(d->activeSheet);
    command->setColor(bdColor);
    command->add(*selection());
    command->execute();
}

void View::helpUsing()
{
  KToolInvocation::invokeHelp( );
}

void View::enableUndo( bool _b )
{
  QAction* action = actionCollection()->action( "office_undo" );
  if( action ) action->setEnabled( _b );
}

void View::enableRedo( bool _b )
{
  QAction* action = actionCollection()->action( "office_redo" );
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
    InsertDeleteColumnManipulator* manipulator = new InsertDeleteColumnManipulator();
    manipulator->setSheet( d->activeSheet );
    manipulator->setReverse( true );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::deleteRow()
{
    InsertDeleteRowManipulator* manipulator = new InsertDeleteRowManipulator();
    manipulator->setSheet( d->activeSheet );
    manipulator->setReverse( true );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::insertColumn()
{
    InsertDeleteColumnManipulator* manipulator = new InsertDeleteColumnManipulator();
    manipulator->setSheet( d->activeSheet );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::insertRow()
{
    InsertDeleteRowManipulator* manipulator = new InsertDeleteRowManipulator();
    manipulator->setSheet( d->activeSheet );
    manipulator->add( *selection() );
    manipulator->execute();
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

  HideShowManipulator* manipulator = new HideShowManipulator();
  manipulator->setSheet( d->activeSheet );
  manipulator->setManipulateColumns(true);
  manipulator->add( *selection() );
  manipulator->execute();
}

void View::showColumn()
{
  if ( d->selection->isRowSelected() )
  {
    KMessageBox::error( this, i18n( "Area is too large." ) );
    return;
  }

  HideShowManipulator* manipulator = new HideShowManipulator();
  manipulator->setSheet( d->activeSheet );
  manipulator->setManipulateColumns(true);
  manipulator->setReverse( true );
  manipulator->add( *selection() );
  manipulator->execute();
}

void View::slotShowColumnDialog()
{
  if ( !d->activeSheet )
    return;

  ShowColRow dlg( this, "showCol", ShowColRow::Column );
  dlg.exec();
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

  HideShowManipulator* manipulator = new HideShowManipulator();
  manipulator->setSheet( d->activeSheet );
  manipulator->setManipulateRows(true);
  manipulator->add( *selection() );
  manipulator->execute();
}

void View::showRow()
{
  if ( d->selection->isColumnSelected() )
  {
    KMessageBox::error( this, i18n( "Area is too large." ) );
    return;
  }

  HideShowManipulator* manipulator = new HideShowManipulator();
  manipulator->setSheet( d->activeSheet );
  manipulator->setManipulateRows(true);
  manipulator->setReverse(true);
  manipulator->add( *selection() );
  manipulator->execute();
}

void View::slotShowRowDialog()
{
  if ( !d->activeSheet )
    return;

  ShowColRow dlg( this, "showRow", ShowColRow::Row );
  dlg.exec();
}

void View::fontSelected( const QString & _font )
{
    if ( d->toolbarLock )
        return;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Font") );
    manipulator->setFontFamily( _font.toLatin1() );
    manipulator->add( *selection() );
    manipulator->execute();

    // Dont leave the focus in the toolbars combo box ...
    if ( d->canvas->editor() )
    {
        const Style style = Cell( d->activeSheet, selection()->marker() ).style();
        d->canvas->editor()->setEditorFont( style.font(), true, zoomHandler() );
        d->canvas->editor()->setFocus();
    }
    else
        d->canvas->setFocus();
}

void View::decreaseFontSize()
{
  setSelectionFontSize( -1 );
}

void View::increaseFontSize()
{
  setSelectionFontSize( 1 );
}

void View::setSelectionFontSize( int deltaSize )
{
    if ( d->toolbarLock )
        return;

    const Style style = Cell( d->activeSheet, selection()->marker() ).style();
    const int size = style.fontSize();

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Font") );
    manipulator->setFontSize( size + deltaSize );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::lower()
{
  CaseManipulator* manipulator = new CaseManipulator;
  manipulator->setSheet( d->activeSheet );
  manipulator->setText( i18n( "Switch to lowercase" ) );
  manipulator->changeMode( CaseManipulator::Lower );
  manipulator->add( *selection() );
  manipulator->execute();
}

void View::upper()
{
  CaseManipulator* manipulator = new CaseManipulator;
  manipulator->setSheet( d->activeSheet );
  manipulator->setText( i18n( "Switch to uppercase" ) );
  manipulator->changeMode( CaseManipulator::Upper );
  manipulator->add( *selection() );
  manipulator->execute();
}

void View::firstLetterUpper()
{
  CaseManipulator *manipulator = new CaseManipulator;
  manipulator->setSheet( d->activeSheet );
  manipulator->setText( i18n( "First letter uppercase" ) );
  manipulator->changeMode( CaseManipulator::FirstUpper );
  manipulator->add( *selection() );
  manipulator->execute();
}

void View::verticalText(bool b)
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n( "Vertical Text" ) );
    manipulator->setVerticalText( b );
    manipulator->setMultiRow( false );
    manipulator->setAngle( 0 );
    manipulator->add( *selection() );
    manipulator->execute();
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
    d->specialCharDlg = 0;
  }
}

void View::slotSpecialChar( QChar c, const QString & _font )
{
    if ( d->activeSheet )
    {
        QPoint marker( d->selection->marker() );
        const Style style = Cell( d->activeSheet, marker ).style();
        if ( style.fontFamily() != _font )
        {
            Style newStyle;
            newStyle.setFontFamily( _font );
            d->activeSheet->cellStorage()->setStyle( Region(marker), newStyle );
        }
        EditWidget * edit = d->editWidget;
        QKeyEvent keyEvent( QEvent::KeyPress, 0, Qt::NoModifier, QString( c ) );
        QApplication::sendEvent( edit, &keyEvent );
    }
}

void View::insertMathExpr()
{
  if ( d->activeSheet == 0 )
    return;

  FormulaDialog * dlg = new FormulaDialog( this, "Function" );
  dlg->show();

  /* TODO - because I search on 'TODO's :-) */
  // #### Is the dialog deleted when it's closed ? (David)
  // Torben thinks that not.
  // FIXME Stefan: FormulaDialog has no 'delete this' statement.
  //               -> It is deleted, when this view is deleted.
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

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Font") );
    manipulator->setFontSize(_size);
    manipulator->add( *selection() );
    manipulator->execute();

    // Dont leave the focus in the toolbars combo box ...
    if ( d->canvas->editor() )
    {
        Cell cell( d->activeSheet, d->selection->marker() );
        d->canvas->editor()->setEditorFont( cell.style().font(), true, zoomHandler() );
        d->canvas->editor()->setFocus();
    }
    else
        d->canvas->setFocus();
}

void View::bold( bool b )
{
    if ( d->toolbarLock )
        return;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Font") );
    manipulator->setFontBold( b );
    manipulator->add( *selection() );
    manipulator->execute();

    if ( d->canvas->editor() )
    {
        int col = selection()->marker().x();
        int row = selection()->marker().y();
        Cell cell = Cell( d->activeSheet, col, row );
        d->canvas->editor()->setEditorFont( cell.style().font(), true, zoomHandler() );
    }
}

void View::underline( bool b )
{
    if ( d->toolbarLock )
        return;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Font") );
    manipulator->setFontUnderline( b );
    manipulator->add( *selection() );
    manipulator->execute();

    if ( d->canvas->editor() )
    {
        int col = selection()->marker().x();
        int row = selection()->marker().y();
        Cell cell = Cell( d->activeSheet, col, row );
        d->canvas->editor()->setEditorFont( cell.style().font(), true, zoomHandler() );
    }
}

void View::strikeOut( bool b )
{
    if ( d->toolbarLock )
        return;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Font") );
    manipulator->setFontStrike( b );
    manipulator->add( *selection() );
    manipulator->execute();

    if ( d->canvas->editor() )
    {
        int col = selection()->marker().x();
        int row = selection()->marker().y();
        Cell cell = Cell( d->activeSheet, col, row );
        d->canvas->editor()->setEditorFont( cell.style().font(), true, zoomHandler() );
    }
}


void View::italic( bool b )
{
    if ( d->toolbarLock )
        return;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Font") );
    manipulator->setFontItalic( b );
    manipulator->add( *selection() );
    manipulator->execute();

    if ( d->canvas->editor() )
    {
        int col = selection()->marker().x();
        int row = selection()->marker().y();
        Cell cell = Cell( d->activeSheet, col, row );
        d->canvas->editor()->setEditorFont( cell.style().font(), true, zoomHandler() );
    }
}

void View::sortInc()
{
  if (!activeSheet())
    return;

  if (d->selection->isSingular())
  {
    KMessageBox::error( this, i18n( "You must select multiple cells." ) );
    return;
  }

  SortManipulator *sm = new SortManipulator ();
  sm->setSheet (activeSheet());

  // Entire row(s) selected ? Or just one row ? Sort by columns if yes.
  QRect range = d->selection->lastRange();
  bool sortCols = d->selection->isRowSelected();
  sortCols = sortCols || (range.top() == range.bottom());
  sm->setSortRows (!sortCols);
  sm->addSortBy (0, true);  // by first one, ascending order
  sm->add (*d->selection);
  sm->execute ();

  updateEditWidget();
}

void View::sortDec()
{
  if ( d->selection->isSingular() )
  {
    KMessageBox::error( this, i18n( "You must select multiple cells." ) );
    return;
  }

  SortManipulator *sm = new SortManipulator ();
  sm->setSheet (activeSheet());

  // Entire row(s) selected ? Or just one row ? Sort by rows if yes.
  QRect range = d->selection->lastRange();
  bool sortCols = d->selection->isRowSelected();
  sortCols = sortCols || (range.top() == range.bottom());
  sm->setSortRows (!sortCols);
  sm->addSortBy (0, false);  // by first one, descending order
  sm->add (*d->selection);
  sm->execute ();

  updateEditWidget();
}


void View::borderBottom()
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Border") );
    manipulator->setBottomBorderPen( QPen( d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine ) );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::setSelectionBottomBorderColor( const QColor & color )
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Border") );
    manipulator->setBottomBorderPen( QPen( color, 1, Qt::SolidLine ) );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::borderRight()
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Border") );
    if ( d->activeSheet->layoutDirection() == Qt::RightToLeft )
        manipulator->setLeftBorderPen( QPen( d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine ) );
    else
        manipulator->setRightBorderPen( QPen( d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine ) );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::setSelectionRightBorderColor( const QColor & color )
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Border") );
    if ( d->activeSheet->layoutDirection() == Qt::RightToLeft )
        manipulator->setLeftBorderPen( QPen( color, 1, Qt::SolidLine ) );
    else
        manipulator->setRightBorderPen( QPen( color, 1, Qt::SolidLine ) );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::borderLeft()
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Border") );
    if ( d->activeSheet->layoutDirection() == Qt::RightToLeft )
        manipulator->setRightBorderPen( QPen(d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine ) );
    else
        manipulator->setLeftBorderPen( QPen(d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine ) );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::setSelectionLeftBorderColor( const QColor & color )
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Border") );
#if 0
    if ( d->activeSheet->layoutDirection() == Qt::RightToLeft )
        manipulator->setRightBorderPen( QPen( color, 1, Qt::SolidLine ) );
    else
        manipulator->setLeftBorderPen( QPen( color, 1, Qt::SolidLine ) );
#endif
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::borderTop()
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Border") );
    manipulator->setTopBorderPen( QPen( d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine ) );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::setSelectionTopBorderColor( const QColor & color )
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Border") );
    manipulator->setTopBorderPen( QPen( color, 1, Qt::SolidLine ) );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::borderOutline()
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Border") );
    manipulator->setTopBorderPen(QPen( d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    manipulator->setBottomBorderPen(QPen( d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    manipulator->setLeftBorderPen(QPen( d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    manipulator->setRightBorderPen(QPen( d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::setSelectionOutlineBorderColor( const QColor & color )
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Border") );
    manipulator->setTopBorderPen(QPen( color, 1, Qt::SolidLine));
    manipulator->setBottomBorderPen(QPen( color, 1, Qt::SolidLine));
    manipulator->setLeftBorderPen(QPen( color, 1, Qt::SolidLine));
    manipulator->setRightBorderPen(QPen( color, 1, Qt::SolidLine));
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::borderAll()
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Border") );
    manipulator->setTopBorderPen(QPen(d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    manipulator->setBottomBorderPen(QPen(d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    manipulator->setLeftBorderPen(QPen(d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    manipulator->setRightBorderPen(QPen(d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    manipulator->setHorizontalPen(QPen(d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    manipulator->setVerticalPen(QPen(d->canvas->resourceProvider()->foregroundColor().toQColor(), 1, Qt::SolidLine));
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::setSelectionAllBorderColor( const QColor & color )
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Border") );
    manipulator->setTopBorderPen(QPen( color, 1, Qt::SolidLine));
    manipulator->setBottomBorderPen(QPen( color, 1, Qt::SolidLine));
    manipulator->setLeftBorderPen(QPen( color, 1, Qt::SolidLine));
    manipulator->setRightBorderPen(QPen( color, 1, Qt::SolidLine));
    manipulator->setHorizontalPen(QPen( color, 1, Qt::SolidLine));
    manipulator->setVerticalPen(QPen( color, 1, Qt::SolidLine));
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::borderRemove()
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Border") );
    manipulator->setTopBorderPen(QPen(Qt::NoPen));
    manipulator->setBottomBorderPen(QPen(Qt::NoPen));
    manipulator->setLeftBorderPen(QPen(Qt::NoPen));
    manipulator->setRightBorderPen(QPen(Qt::NoPen));
    manipulator->setHorizontalPen(QPen(Qt::NoPen));
    manipulator->setVerticalPen(QPen(Qt::NoPen));
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::addSheet( Sheet * _t )
{
  doc()->emitBeginOperation( false );

  insertSheet( _t );

  // Connect some signals
  connect( _t, SIGNAL( sig_refreshView() ), SLOT( slotRefreshView() ) );
  connect( _t, SIGNAL( sig_updateView( Sheet* ) ), SLOT( slotUpdateView( Sheet* ) ) );
  connect( _t->print(), SIGNAL( sig_updateView( Sheet* ) ), SLOT( slotUpdateView( Sheet* ) ) );
  connect( _t, SIGNAL( sig_updateView( Sheet *, const Region& ) ),
                    SLOT( slotUpdateView( Sheet*, const Region& ) ) );
  connect( _t, SIGNAL( sig_updateView( EmbeddedObject* )), SLOT( slotUpdateView( EmbeddedObject* ) ) );

  connect( _t, SIGNAL( sig_updateHBorder( Sheet * ) ),
                    SLOT( slotUpdateHBorder( Sheet * ) ) );
  connect( _t, SIGNAL( sig_updateVBorder( Sheet * ) ),
                    SLOT( slotUpdateVBorder( Sheet * ) ) );
  connect( _t, SIGNAL( sig_nameChanged( Sheet*, const QString& ) ),
                    this, SLOT( slotSheetRenamed( Sheet*, const QString& ) ) );
  connect( _t, SIGNAL( sig_SheetHidden( Sheet* ) ),
                    this, SLOT( slotSheetHidden( Sheet* ) ) );
  connect( _t, SIGNAL( sig_SheetShown( Sheet* ) ),
                    this, SLOT( slotSheetShown( Sheet* ) ) );
  connect( _t, SIGNAL( sig_SheetRemoved( Sheet* ) ),
                    this, SLOT( slotSheetRemoved( Sheet* ) ) );
  // ########### Why do these signals not send a pointer to the sheet?
  // This will lead to bugs.
  connect( _t, SIGNAL( sig_updateChildGeometry( EmbeddedKOfficeObject* ) ),
                    SLOT( slotUpdateChildGeometry( EmbeddedKOfficeObject* ) ) );

  if ( !d->loading )
    updateBorderButton();

  doc()->emitEndOperation();
}

void View::slotSheetRemoved(Sheet* sheet)
{
    doc()->emitBeginOperation( false );

    d->tabBar->removeTab(sheet->sheetName());
    if (doc()->map()->findSheet(doc()->map()->visibleSheets().first()))
        setActiveSheet(doc()->map()->findSheet(doc()->map()->visibleSheets().first()));
    else
        d->activeSheet = 0;

    doc()->emitEndOperation();
}

void View::removeAllSheets()
{
  doc()->emitBeginOperation(false);
  d->tabBar->clear();

  setActiveSheet( 0 );

  doc()->emitEndOperation();
}

void View::setActiveSheet( Sheet* sheet, bool updateSheet )
{
    if ( sheet == d->activeSheet )
        return;

    doc()->emitBeginOperation( false );

    saveCurrentSheetSelection();

    const Sheet* oldSheet = d->activeSheet;
    d->activeSheet = sheet;

    if ( d->activeSheet == 0 )
    {
        doc()->emitEndOperation();
        return;
    }

    // flake
    // Change the active shape container (layer) and use its shapes.
    d->canvas->shapeManager()->selection()->setActiveLayer( d->activeSheet->shapeContainer() );
    d->canvas->shapeManager()->setShapes( d->activeSheet->shapeContainer()->iterator() );
    // Tell the Canvas about the new visible sheet size.
    sheetView( d->activeSheet )->updateAccessedCellRange();

    if ( oldSheet && oldSheet->layoutDirection() != d->activeSheet->layoutDirection() )
        refreshView();

    if ( updateSheet )
    {
        d->tabBar->setActiveTab( d->activeSheet->sheetName() );
        d->vBorderWidget->repaint();
        d->hBorderWidget->repaint();
        d->selectAllButton->repaint();
    }

    /* see if there was a previous selection on this other sheet */
    QMap<Sheet*, QPoint>::Iterator it = d->savedAnchors.find(d->activeSheet);
    QMap<Sheet*, QPoint>::Iterator it2 = d->savedMarkers.find(d->activeSheet);
    QMap<Sheet*, QPointF>::Iterator it3 = d->savedOffsets.find(d->activeSheet);

    // restore the old anchor and marker
    const QPoint newAnchor = (it == d->savedAnchors.end()) ? QPoint(1,1) : *it;
    const QPoint newMarker = (it2 == d->savedMarkers.end()) ? QPoint(1,1) : *it2;

    d->selection->clear();
    d->selection->setActiveSheet( d->activeSheet );
    d->selection->setOriginSheet( d->activeSheet );
    d->selection->initialize( QRect( newMarker, newAnchor ) );
    d->choice->setActiveSheet( d->activeSheet );

    d->canvas->scrollToCell(newMarker);
    if (it3 != d->savedOffsets.end())
    {
        const QPoint offset = zoomHandler()->documentToView( *it3 ).toPoint();
        d->canvas->setDocumentOffset( offset );
        d->horzScrollBar->setValue( offset.x() );
        d->vertScrollBar->setValue( offset.y() );
    }

    d->actions->showPageBorders->setChecked( d->activeSheet->isShowPageBorders() );
    d->actions->protectSheet->setChecked( d->activeSheet->isProtected() );
    d->actions->protectDoc->setChecked( doc()->map()->isProtected() );
    d->adjustActions( !d->activeSheet->isProtected() );
    d->adjustWorkbookActions( !doc()->map()->isProtected() );

    calcStatusBarOp();

    doc()->emitEndOperation();
}

void View::slotSheetRenamed( Sheet* sheet, const QString& old_name )
{
  doc()->emitBeginOperation( false );
  d->tabBar->renameTab( old_name, sheet->sheetName() );
  doc()->emitEndOperation();
}

void View::slotSheetHidden( Sheet* )
{
  doc()->emitBeginOperation(false);
  updateShowSheetMenu();
  doc()->emitEndOperation();
}

void View::slotSheetShown( Sheet* )
{
  doc()->emitBeginOperation(false);
  d->tabBar->setTabs( doc()->map()->visibleSheets() );
  updateShowSheetMenu();
  doc()->emitEndOperation();
}

void View::changeSheet( const QString& _name )
{
    if ( activeSheet()->sheetName() == _name )
        return;

    Sheet *t = doc()->map()->findSheet( _name );
    if ( !t )
    {
        kDebug() <<"Unknown sheet" << _name;
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
    d->selectAllButton->repaint();
    doc()->emitEndOperation();
}

void View::moveSheet( unsigned sheet, unsigned target )
{
    if( doc()->map()->isProtected() ) return;

    QStringList vs = doc()->map()->visibleSheets();

    if( target >= (uint) vs.count() )
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
    dlg->setAutoCalculationEnabled( d->activeSheet->isAutoCalculationEnabled() );
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
        command->setAutoCalculationEnabled( dlg->autoCalc() );
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
    }

    delete dlg;

    if ( directionChanged )
    {
        // the scrollbar and hborder remain reversed otherwise
        d->horzScrollBar->setValue( d->horzScrollBar->maximum() -
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
  QUndoCommand* command = new AddSheetCommand( t );
  doc()->addCommand( command );
  updateEditWidget();
  setActiveSheet( t );

  if ( doc()->map()->visibleSheets().count() > 1 )
  {
    d->actions->deleteSheet->setEnabled( true );
    d->actions->hideSheet->setEnabled( true );
  }

  doc()->emitEndOperation();
}

void View::duplicateSheet()
{
    if (doc()->map()->isProtected())
    {
        KMessageBox::error(this, i18n("You cannot change a protected sheet."));
        return;
    }

    DuplicateSheetCommand* command = new DuplicateSheetCommand();
    command->setSheet(activeSheet());
    doc()->addCommand(command);

    if (doc()->map()->visibleSheets().count() > 1)
    {
        d->actions->deleteSheet->setEnabled(true);
        d->actions->hideSheet->setEnabled(true);
    }
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
  int i = vs.indexOf( d->activeSheet->sheetName() ) - 1;
  if( i < 0 ) i = 1;
  QString sn = vs[i];

  doc()->emitBeginOperation(false);

  QUndoCommand* command = new HideSheetCommand( activeSheet() );
  doc()->addCommand( command );

  doc()->emitEndOperation();

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

#if 0 // KSPREAD_KOPART_EMBEDDING
  if ( canvasWidget()->isObjectSelected() )
  {
    canvasWidget()->copyOasisObjects();
    return;
  }
#endif // KSPREAD_KOPART_EMBEDDING
  if ( !d->canvas->editor() )
  {
    d->activeSheet->copySelection( selection() );

    updateEditWidget();
  }
  else
    d->canvas->editor()->copy();
}

void View::copyAsText()
{
  if ( !d->activeSheet )
    return;
  d->activeSheet->copyAsText( selection() );
}


void View::cutSelection()
{
    if ( !d->activeSheet )
        return;
    //don't used this function when we edit a cell.
    doc()->emitBeginOperation(false);

#if 0 // KSPREAD_KOPART_EMBEDDING
    if ( canvasWidget()->isObjectSelected() )
    {
        canvasWidget()->copyOasisObjects();
        doc()->emitEndOperation();

        doc()->beginMacro( i18n( "Cut Objects" ) );
        foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
        {
            if ( object->sheet() == canvasWidget()->activeSheet() && object->isSelected() )
            {
                RemoveObjectCommand *cmd = new RemoveObjectCommand( object, true );
                doc()->addCommand( cmd );
            }
        }
        canvasWidget()->setMouseSelectedObject( false );
        doc()->endMacro();

        return;
    }
#endif // KSPREAD_KOPART_EMBEDDING
    if ( !d->canvas->editor() )
    {
        d->activeSheet->cutSelection( selection() );
        calcStatusBarOp();
        updateEditWidget();
    }
    else
    {
        d->canvas->editor()->cut();
    }
    doc()->emitEndOperation();
}

void View::paste()
{
  if ( !d->activeSheet )
    return;

  if (!koDocument()->isReadWrite()) // don't paste into a read only document
    return;

  const QMimeData* mimeData = QApplication::clipboard()->mimeData( QClipboard::Clipboard );

  if ( mimeData->hasFormat( "application/vnd.oasis.opendocument.spreadsheet" ) )
  {
#if 0 // KSPREAD_KOPART_EMBEDDING
    canvasWidget()->deselectAllObjects();
#endif // KSPREAD_KOPART_EMBEDDING
    QByteArray returnedTypeMime = "application/vnd.oasis.opendocument.spreadsheet";
    QByteArray arr = mimeData->data( returnedTypeMime );
    if( arr.isEmpty() )
      return;
    QBuffer buffer( &arr );
    KoStore * store = KoStore::createStore( &buffer, KoStore::Read );

    KoOdfReadStore odfStore( store );
    KoXmlDocument doc;
    QString errorMessage;
    bool ok = odfStore.loadAndParse( "content.xml", doc, errorMessage );
    if ( !ok ) {
      kError(32001) << "Error parsing content.xml: " << errorMessage << endl;
      return;
    }

    KoOdfStylesReader stylesReader;
    KoXmlDocument stylesDoc;
    (void)odfStore.loadAndParse( "styles.xml", stylesDoc, errorMessage );
    // Load styles from style.xml
    stylesReader.createStyleMap( stylesDoc, true );
    // Also load styles from content.xml
    stylesReader.createStyleMap( doc, false );

    // from KSpreadDoc::loadOasis:
    KoXmlElement content = doc.documentElement();
    KoXmlElement realBody ( KoDom::namedItemNS( content, KoXmlNS::office, "body" ) );
    if ( realBody.isNull() )
    {
      kDebug(36005) <<"Invalid OASIS OpenDocument file. No office:body tag found.";
      return;
    }
    KoXmlElement body = KoDom::namedItemNS( realBody, KoXmlNS::office, "spreadsheet" );

    if ( body.isNull() )
    {
      kError(36005) << "No office:spreadsheet found!" << endl;
      KoXmlElement childElem;
      QString localName;
      forEachElement( childElem, realBody ) {
        localName = childElem.localName();
      }
      return;
    }

    KoOasisLoadingContext context( d->doc, stylesReader, store );
    Q_ASSERT( !stylesReader.officeStyle().isNull() );

    //load in first
    d->doc->styleManager()->loadOasisStyleTemplate( stylesReader );

//     // TODO check versions and mimetypes etc.
    d->doc->loadOasisCellValidation( body );

    // all <sheet:sheet> goes to workbook
    bool result = d->doc->map()->loadOasis( body, context );

    if (!result)
      return;

    d->doc->namedAreaManager()->loadOdf(body);
  }
  else
  {
#if 0 // KSPREAD_KOPART_EMBEDDING
    //TODO:  What if the clipboard data is available in both pixmap and OASIS format? (ie. for embedded parts)
    QPixmap clipboardPixmap = QApplication::clipboard()->pixmap( QClipboard::Clipboard );
    if (!clipboardPixmap.isNull())
    {
        d->activeSheet->insertPicture( markerDocumentPosition()  , clipboardPixmap );
    }
#endif // KSPREAD_KOPART_EMBEDDING
  }

  doc()->emitBeginOperation( false );
  if ( !d->canvas->editor() )
  {
      //kDebug(36005) <<"Pasting. Rect=" << d->selection->lastRange() <<" bytes";
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
  doc()->emitEndOperation();
}

void View::specialPaste()
{
  if ( !d->activeSheet )
    return;

  SpecialPasteDialog dlg( this );
  if ( dlg.exec() )
  {
    calcStatusBarOp();
    updateEditWidget();
  }
}

void View::selectAll()
{
    if ( !selection()->isAllSelected() )
    {
        d->oldSelection = *selection();
        selection()->initialize( QRect( QPoint(KS_colMax, KS_rowMax), QPoint(1,1) ) );
    }
    else
    {
        selection()->initialize( d->oldSelection );
        d->oldSelection.clear();
    }
}

void View::changeAngle()
{
  AngleDialog dlg( this, "Angle", selection()->marker() );
  dlg.exec();
}

void View::setSelectionAngle( int angle )
{
    doc()->beginMacro( i18n("Change Angle") );

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setAngle( angle );
    manipulator->add( *selection() );
    doc()->addCommand( manipulator );

    AdjustColumnRowManipulator* manipulator2 = new AdjustColumnRowManipulator();
    manipulator2->setSheet( d->activeSheet );
    manipulator2->setAdjustColumn(true);
    manipulator2->setAdjustRow(true);
    manipulator2->add( *selection() );
    doc()->addCommand( manipulator2 );

    doc()->endMacro();
}

void View::mergeCell()
{
  // sanity check
  if( !d->activeSheet )
    return;
  d->activeSheet->mergeCells(*selection());
}

void View::mergeCellHorizontal()
{
  // sanity check
  if( !d->activeSheet )
    return;
  d->activeSheet->mergeCells(*selection(), true);
}

void View::mergeCellVertical()
{
  // sanity check
  if( !d->activeSheet )
    return;
  d->activeSheet->mergeCells(*selection(), false, true);
}

void View::dissociateCell()
{
  // sanity check
  if( !d->activeSheet )
    return;
  d->activeSheet->dissociateCells(*selection());
}


void View::increaseIndent()
{
    IndentationCommand* command = new IndentationCommand();
    command->setSheet(d->activeSheet);
    command->add(*selection());
    if (!command->execute())
        delete command;
}

void View::decreaseIndent()
{
    IndentationCommand* command = new IndentationCommand();
    command->setSheet(d->activeSheet);
    command->setReverse(true);
    command->add(*selection());
    if (!command->execute())
        delete command;
}

void View::goalSeek()
{
  if ( d->canvas->editor() )
  {
    d->canvas->deleteEditor( true ); // save changes
  }

  GoalSeekDialog * dlg
    = new GoalSeekDialog( this, QPoint( selection()->marker().x(),
                                            selection()->marker().y() ),
                              "GoalSeekDialog" );
  dlg->show();
  /* dialog autodeletes itself */
}

void View::subtotals()
{
  if (!activeSheet())
    return;

  QRect selection( d->selection->lastRange() );
  if ( ( selection.width() < 2 ) || ( selection.height() < 2 ) )
  {
    KMessageBox::error( this, i18n("You must select multiple cells.") );
    return;
  }

  SubtotalDialog dlg( this, selection );
  if ( dlg.exec() )
  {
    doc()->emitBeginOperation( false );

    d->selection->initialize( QRect(dlg.selection().topLeft(), dlg.selection().bottomRight()));//, dlg.sheet() );
    doc()->emitEndOperation();
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

  QRect area=d->selection->lastRange();

  //Only use the first column
  area.setRight(area.left());

/* if ( area.width() > 1 )
  {
  //Only use the first column

    KMessageBox::error( this, i18n("You must not select an area containing more than one column.") );
    return;
  }*/

  CSVDialog dialog( this, area, CSVDialog::Column );
  dialog.setDecimalSymbol(doc()->locale()->decimalSymbol());
  dialog.setThousandsSeparator(doc()->locale()->thousandsSeparator());
  if( !dialog.canceled() )
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
    d->replace = 0;

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
                   ? d->selection->lastRange()
                   : QRect( 1, 1, currentSheet->cellStorage()->columns(), currentSheet->cellStorage()->rows() ); // All cells

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
    //kDebug() << d->findPos <<" to" << d->findEnd;
    //kDebug() <<"leftcol=" << d->findLeftColumn <<" rightcol=" << d->findRightColumn;
}

void View::findNext()
{
    KFind* findObj = d->find ? d->find : d->replace;
    if ( !findObj )  {
        find();
        return;
    }
    KFind::Result res = KFind::NoMatch;
    Cell cell = findNextCell();
    bool forw = ! ( d->findOptions & KFind::FindBackwards );
    while ( res == KFind::NoMatch && !cell.isNull() )
    {
        if ( findObj->needData() )
        {
            if ( d->typeValue == FindOption::Note )
                findObj->setData( cell.comment() );
            else
                findObj->setData( cell.userInput() );
            d->findPos = QPoint( cell.column(), cell.row() );
            //kDebug() <<"setData(cell" << d->findPos << ')';
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

Cell View::nextFindValidCell( int col, int row )
{
    Cell cell = Cell( d->searchInSheets.currentSheet, col, row );
    if ( cell.isDefault() || cell.isPartOfMerged() || cell.isFormula() )
        cell = Cell();
    if ( d->typeValue == FindOption::Note && !cell.isNull() && cell.comment().isEmpty())
        cell = Cell();
    return cell;
}

Cell View::findNextCell()
{
    // cellStorage()->firstInRow / cellStorage()->nextInRow would be faster at doing that,
    // but it doesn't seem to be easy to combine it with 'start a column d->find.x()'...

    Sheet* sheet = d->searchInSheets.currentSheet;
    Cell cell;
    bool forw = ! ( d->findOptions & KFind::FindBackwards );
    int col = d->findPos.x();
    int row = d->findPos.y();
    int maxRow = sheet->cellStorage()->rows();
    //kDebug() <<"findNextCell starting at" << col << ',' << row <<"   forw=" << forw;

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
            if ( !cell.isNull() )
                break;
            // Prepare looking in the next row
            if ( forw )  {
                col = d->findLeftColumn;
                ++row;
            } else {
                col = d->findRightColumn;
                --row;
            }
            //kDebug() <<"next row:" << col << ',' << row;
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
            if ( !cell.isNull() )
                break;
            // Prepare looking in the next col
            if ( forw )  {
                row = 0;
                ++col;
            } else {
                col = maxRow;
                --col;
            }
            //kDebug() <<"next row:" << col << ',' << row;
        }
    }
    // if ( !cell )
    // No more next cell - TODO go to next sheet (if not looking in a selection)
    // (and make d->findEnd (max,max) in that case...)
    //kDebug() <<" returning" << cell;
    return cell;
}

void View::findPrevious()
{
    KFind* findObj = d->find ? d->find : d->replace;
    if ( !findObj )  {
        find();
        return;
    }
    //kDebug() <<"findPrevious";
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
  if (!d->activeSheet)
    return;

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
    d->find = 0;
    // NOTE Stefan: Avoid beginning of line replacements with nothing which
    //              will lead to an infinite loop (Bug #125535). The reason
    //              for this is unclear to me, but who cares and who would
    //              want to do something like this, häh?!
    if (dlg.pattern() == "^" && dlg.replacement().isEmpty())
      return;
    d->replace = new KReplace( dlg.pattern(), dlg.replacement(), dlg.options() );

    d->searchInSheets.currentSheet = activeSheet();
    d->searchInSheets.firstSheet = d->searchInSheets.currentSheet;
    initFindReplace();
    connect( d->replace, SIGNAL( replace( const QString &, int, int, int ) ),
             this, SLOT( slotReplace( const QString &, int, int, int ) ) );

    if ( !doc()->undoLocked() )
    {
        QRect region( d->findPos, d->findEnd );
        //TODO create undo/redo for comment
        UndoChangeAreaTextCell* undo = new UndoChangeAreaTextCell( doc(), d->searchInSheets.currentSheet, Region( region ) );
        doc()->addCommand( undo );
    }

    findNext();

#if 0
    // Refresh the editWidget
    // TODO - after a replacement only?
    Cell cell = Cell( activeSheet(), selection()->marker() );
    if ( cell.userInput() != 0 )
        d->editWidget->setText( cell.userInput() );
    else
        d->editWidget->setText( "" );
#endif
}

void View::slotHighlight( const QString &/*text*/, int /*matchingIndex*/, int /*matchedLength*/ )
{
    d->selection->initialize( d->findPos );
    KDialog *dialog=0;
    if ( d->find )
        dialog = d->find->findNextDialog();
    else
        dialog = d->replace->replaceNextDialog();
    kDebug()<<" baseDialog :"<<dialog;
    QRect globalRect( d->findPos, d->findEnd );
    globalRect.moveTopLeft( canvasWidget()->mapToGlobal( globalRect.topLeft() ) );
    KDialog::avoidArea( dialog, QRect( d->findPos, d->findEnd ));
}

void View::slotReplace( const QString &newText, int, int, int )
{
    // Which cell was this again?
    Cell cell( d->searchInSheets.currentSheet, d->findPos );

    // ...now I remember, update it!
    if ( d->typeValue == FindOption::Value )
        cell.parseUserInput( newText );
    else if ( d->typeValue == FindOption::Note )
        cell.setComment( newText );
}

void View::conditional()
{
    QRect rect( d->selection->lastRange() );
    ConditionalDialog dlg( this, "ConditionalDialog", rect);
    dlg.exec();
}

void View::validity()
{
    QRect rect( d->selection->lastRange() );
    DlgValidity dlg( this,"validity",rect);
    dlg.exec();
}


void View::insertSeries()
{
    d->canvas->closeEditor();
    SeriesDlg dlg( this, "Series", QPoint( selection()->marker().x(), selection()->marker().y() ) );
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

void View::autoFilter()
{
    AutoFilterCommand* command = new AutoFilterCommand();
    command->setSheet( d->activeSheet );
    command->add( *selection() );
    command->execute();
}

void View::clearHyperlink()
{
    QPoint marker( d->selection->marker() );
    Cell cell( d->activeSheet, marker );
    if( !cell ) return;
    if( cell.link().isEmpty() ) return;

    LinkCommand* command = new LinkCommand( cell, QString(), QString() );
    doc()->addCommand( command );

  canvasWidget()->setFocus();
  d->editWidget->setText( cell.userInput() );
}

void View::insertHyperlink()
{
    if (!activeSheet())
      return;

    d->canvas->closeEditor();

    QPoint marker( d->selection->marker() );
    Cell cell( d->activeSheet, marker );

    LinkDialog* dlg = new LinkDialog( this );
    dlg->setWindowTitle( i18n( "Insert Link" ) );
    if ( !cell.isNull() )
    {
      dlg->setText( cell.userInput() );
      if( !cell.link().isEmpty() )
      {
        dlg->setWindowTitle( i18n( "Edit Link" ) );
        dlg->setLink( cell.link() );
      }
    }

    if( dlg->exec() == KDialog::Accepted )
    {
        cell = Cell( d->activeSheet, marker );

        LinkCommand* command = new LinkCommand( cell, dlg->text(), dlg->link() );
        doc()->addCommand( command );

        //refresh editWidget
      canvasWidget()->setFocus();
      d->editWidget->setText( cell.userInput() );
    }
    delete dlg;
}

void View::insertFromDatabase()
{
#ifndef QT_NO_SQL
    d->canvas->closeEditor();

    QRect rect = d->selection->lastRange();

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

    CSVDialog dialog( this, d->selection->lastRange(), CSVDialog::File );
    dialog.setDecimalSymbol(doc()->locale()->decimalSymbol());
    dialog.setThousandsSeparator(doc()->locale()->thousandsSeparator());
    if( !dialog.canceled() )
      dialog.exec();
}

void View::insertFromClipboard()
{
    d->canvas->closeEditor();

    CSVDialog dialog( this, d->selection->lastRange(), CSVDialog::Clipboard );
    dialog.setDecimalSymbol(doc()->locale()->decimalSymbol());
    dialog.setThousandsSeparator(doc()->locale()->thousandsSeparator());
    if( !dialog.canceled() )
      dialog.exec();
}

void View::setupPrinter( QPrinter &printer, QPrintDialog &printDialog )
{
     kDebug() <<"Entering KSpread setupPrinter.";
    if (!activeSheet())
      return;

    SheetPrint* print = d->activeSheet->print();

    //apply page layout parameters
    KoPageFormat::Format pageFormat = print->paperFormat();

    printer.setPageSize( static_cast<QPrinter::PageSize>( KoPageFormat::printerPageSize( pageFormat ) ) );

    if ( print->orientation() == KoPageFormat::Landscape || pageFormat == KoPageFormat::ScreenSize )
        printer.setOrientation( QPrinter::Landscape );
    else
        printer.setOrientation( QPrinter::Portrait );

    printer.setFullPage( true );

    //add possibility to select the sheets to print:
     kDebug() <<"Adding sheet selection page.";
    d->m_sheetpage = new SheetSelectPage();

     kDebug() <<"Iterating through available sheets and initializing list of available sheets.";
    QList<Sheet*> sheetList = doc()->map()->sheetList();
    for ( int i = sheetList.count()-1; i >= 0; --i )
    {
      Sheet* sheet = sheetList[ i ];
      kDebug() <<"Adding" << sheet->sheetName();
      d->m_sheetpage->prependAvailableSheet(sheet->sheetName());
    }
     kDebug() <<"Exiting KSpread setupPrinter.";
}

void View::print( QPrinter &printer, QPrintDialog &printDialog )
{
     kDebug() <<"Entering KSpread print.";
    if (!activeSheet())
      return;

    //save the current active sheet for later, so we can restore it at the end
    Sheet* selectedsheet = this->activeSheet();

    //print all sheets in the order given by the print dialog (Sheet Selection)
    QStringList sheetlist = d->m_sheetpage->selectedSheets();

    if (sheetlist.empty())
    {
      kDebug() <<"No sheet for printing selected, printing active sheet";
      sheetlist.append(d->activeSheet->sheetName());
    }

    QPainter painter;
    painter.begin( &printer );

    bool firstpage = true;

    QStringList::iterator sheetlistiterator;
    for (sheetlistiterator = sheetlist.begin(); sheetlistiterator != sheetlist.end(); ++sheetlistiterator)
    {
        kDebug() <<"  printing sheet" << *sheetlistiterator;
        Sheet* sheet = doc()->map()->findSheet(*sheetlistiterator);
        if (sheet == 0)
        {
          kWarning() << i18n("Sheet %1 could not be found for printing",*sheetlistiterator);
          continue;
        }

        setActiveSheet(sheet,false);

        SheetPrint* print = d->activeSheet->print();

        if (firstpage)
          firstpage=false;
        else
        {
          kDebug() <<" inserting new page";
          printer.newPage();
        }

        if ( d->canvas->editor() )
        {
            d->canvas->deleteEditor( true ); // save changes
        }

        //store the current setting in a temporary variable
        KoPageFormat::Orientation _orient = print->orientation();

        //use the current orientation from print dialog
        if ( printer.orientation() == QPrinter::Landscape )
        {
            print->setPaperOrientation( KoPageFormat::Landscape );
        }
        else
        {
            print->setPaperOrientation( KoPageFormat::Portrait );
        }

        bool result = print->print( painter, &printer );

        //Restore original orientation
        print->setPaperOrientation( _orient );

        // Nothing to print
        if( !result )
        {
            // not required in Qt
            //if( !printer.previewOnly() )
            //{
                KMessageBox::information( 0,
                i18n("Nothing to print for sheet %1.",
                d->activeSheet->sheetName()) );
                //@todo: make sure we really can comment this out,
                //       what to do with partially broken printouts?
//                 printer.abort();
            //}
        }
    }

    painter.end();
    this->setActiveSheet(selectedsheet);
}

QList<QWidget*> View::printDialogPages()
{
    return QList<QWidget*>() << d->m_sheetpage;
}

void View::insertChart( const QRect& _geometry, KoDocumentEntry& _e )
{
    if ( !d->activeSheet )
      return;

    // Transform the view coordinates to document coordinates
    QRectF unzoomedRect = zoomHandler()->viewToDocument( _geometry );
    unzoomedRect.translate( d->canvas->xOffset(), d->canvas->yOffset() );

#if 0 // KSPREAD_KOPART_EMBEDDING
    InsertObjectCommand *cmd = 0;
    if ( d->selection->isColumnOrRowSelected() )
    {
      KMessageBox::error( this, i18n("Area is too large."));
      return;
    }
    else
      cmd = new InsertObjectCommand( unzoomedRect, _e, d->selection->lastRange(), d->canvas  );

    doc()->addCommand( cmd );
#endif // KSPREAD_KOPART_EMBEDDING
}

void View::insertChild( const QRect& _geometry, KoDocumentEntry& _e )
{
#if 0 // KSPREAD_KOPART_EMBEDDING
  if ( !d->activeSheet )
    return;

  // Transform the view coordinates to document coordinates
  QRectF unzoomedRect = zoomHandler()->viewToDocument( _geometry );
  unzoomedRect.translate( d->canvas->xOffset(), d->canvas->yOffset() );

  InsertObjectCommand *cmd = new InsertObjectCommand( unzoomedRect, _e, d->canvas );
  doc()->addCommand( cmd );
#endif // KSPREAD_KOPART_EMBEDDING
}

QPointF View::markerDocumentPosition()
{
  QPoint marker=selection()->marker();

  return QPointF( d->activeSheet->columnPosition(marker.x()),
            d->activeSheet->rowPosition(marker.y()) );
}

void View::insertPicture()
{
#if 0 // KSPREAD_KOPART_EMBEDDING
  //Note:  We don't use the usual insert handler here (which allows the user to drag-select the target area
  //for the object) because when inserting raster graphics, it is usually desireable to insert at 100% size,
  //since the graphic won't look right if inserted with an incorrect aspect ratio or if blurred due to the
  //scaling.  If the user wishes to change the size and/or aspect ratio, they can do that afterwards.
  //This behaviour can be seen in other spreadsheets.
  //-- Robert Knight 12/02/06 <robertknight@gmail.com>

  KUrl file = KFileDialog::getImageOpenUrl( KUrl(), d->canvas );

  if (file.isEmpty())
  return;

  if ( !d->activeSheet )
      return;

  InsertObjectCommand *cmd = new InsertObjectCommand( QRectF(markerDocumentPosition(),QSizeF(0,0)) , file, d->canvas );
  doc()->addCommand( cmd );
#endif // KSPREAD_KOPART_EMBEDDING
}

void View::slotUpdateChildGeometry( EmbeddedKOfficeObject */*_child*/ )
{
    // ##############
    // TODO
    /*
  if ( _child->sheet() != d->activeSheet )
    return;

  // Find frame for child
  ChildFrame *f = 0;
  QPtrListIterator<ChildFrame> it( m_lstFrames );
  for ( ; it.current() && !f; ++it )
    if ( it.current()->child() == _child )
      f = it.current();

  assert( f != 0 );

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

   if ( mode )
   {
     KNewPasswordDialog dlg(this);
     dlg.setPrompt( i18n( "Enter a password.") );
     dlg.setWindowTitle( i18n( "Protect Document" ) );
     if ( dlg.exec() != KPasswordDialog::Accepted )
     {
       d->actions->protectDoc->blockSignals( true );
       d->actions->protectDoc->setChecked( false );
       d->actions->protectDoc->blockSignals( false );
       return;
     }

     QByteArray hash( "" );
     QString password = dlg.password();
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );
     doc()->map()->setProtected( hash );
   }
   else
   {
     KPasswordDialog dlg(this);
     dlg.setPrompt( i18n( "Enter the password.") );
     dlg.setWindowTitle( i18n( "Unprotect Document" ) );
     if ( dlg.exec() != KPasswordDialog::Accepted )
     {
       d->actions->protectDoc->blockSignals( true );
       d->actions->protectDoc->setChecked( true );
       d->actions->protectDoc->blockSignals( false );
       return;
     }

     QByteArray hash( "" );
     QString password( dlg.password() );
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );
     if ( !doc()->map()->checkPassword( hash ) )
     {
       KMessageBox::error( 0, i18n( "Password is incorrect." ) );
       d->actions->protectDoc->blockSignals( true );
       d->actions->protectDoc->setChecked( true );
       d->actions->protectDoc->blockSignals( false );
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

   if ( mode )
   {
     KNewPasswordDialog dlg(this);
     dlg.setPrompt( i18n( "Enter a password.") );
     dlg.setWindowTitle( i18n( "Protect Sheet" ) );
     if ( dlg.exec() != KPasswordDialog::Accepted )
     {
       d->actions->protectSheet->blockSignals( true );
       d->actions->protectSheet->setChecked( false );
       d->actions->protectSheet->blockSignals( false );
       return;
     }

     QByteArray hash( "" );
     QString password = dlg.password();
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );
     d->activeSheet->setProtected( hash );
   }
   else
   {
     KPasswordDialog dlg(this);
     dlg.setPrompt( i18n( "Enter the password.") );
     dlg.setWindowTitle( i18n( "Unprotect Sheet" ) );
     if ( dlg.exec() != KPasswordDialog::Accepted )
     {
       d->actions->protectSheet->blockSignals( true );
       d->actions->protectSheet->setChecked( true );
       d->actions->protectSheet->blockSignals( false );
       return;
     }

     QByteArray hash( "" );
     QString password( dlg.password() );
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );


     if ( !d->activeSheet->checkPassword( hash ) )
     {
       KMessageBox::error( 0, i18n( "Password is incorrect." ) );
       d->actions->protectSheet->blockSignals( true );
       d->actions->protectSheet->setChecked( true );
       d->actions->protectSheet->blockSignals( false );
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
   doc()->emitEndOperation();
}

void View::togglePageBorders( bool mode )
{
  if ( !d->activeSheet )
    return;

  doc()->emitBeginOperation( false );
  d->activeSheet->setShowPageBorders( mode );
  doc()->emitEndOperation();
}

void View::viewZoom( KoZoomMode::Mode mode, double zoom )
{
    Q_ASSERT( mode == KoZoomMode::ZOOM_CONSTANT );
    d->canvas->closeEditor();
    doc()->emitBeginOperation( false );
    doc()->refreshInterface();
    doc()->emitEndOperation();
}

void View::setZoom( int zoom, bool /*updateViews*/ )
{
  kDebug(36005) <<"---------SetZoom:" << zoom;

  // Set the zoom in KoView (for embedded views)
  doc()->emitBeginOperation( false );

  zoomHandler()->setZoomAndResolution( zoom, KoGlobal::dpiX(), KoGlobal::dpiY());
  //KoView::setZoom( zoomHandler()->zoomedResolutionY() /* KoView only supports one zoom */ );

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
    doc()->emitEndOperation();
  }
}

void View::comment()
{
  if ( !d->activeSheet )
    return;

  CommentDialog dlg( this, "comment",
                     QPoint( selection()->marker().x(),
                             selection()->marker().y() ) );
  if ( dlg.exec() )
    updateEditWidget();
}

void View::setSelectionComment( const QString& comment )
{
    CommentCommand* manipulator = new CommentCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n( "Add Comment" ) );
    manipulator->setComment( comment.trimmed() );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::editCell()
{
  if ( d->canvas->editor() )
    return;

  d->canvas->createEditor( false /* keep content */ );
}

bool View::showSheet(const QString& sheetName) {
  Sheet *t=doc()->map()->findSheet(sheetName);
  if ( !t )
  {
    kDebug(36001) <<"Unknown sheet" <<sheetName;
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
    kDebug(36001) <<"Unknown sheet";
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
    kDebug(36001) <<"Unknown sheet";
    return;
  }
  d->canvas->closeEditor();
  setActiveSheet( t );
  d->tabBar->setActiveTab( t->sheetName() );
  d->tabBar->ensureVisible( t->sheetName() );
}

void View::firstSheet()
{
  Sheet *t = doc()->map()->sheet( 0 );
  if ( !t )
  {
    kDebug(36001) <<"Unknown sheet";
    return;
  }
  d->canvas->closeEditor();
  setActiveSheet( t );
  d->tabBar->setActiveTab( t->sheetName() );
  d->tabBar->ensureVisible( t->sheetName() );
}

void View::lastSheet()
{
  Sheet *t = doc()->map()->sheet( doc()->map()->count() - 1 );
  if ( !t )
  {
    kDebug(36001) <<"Unknown sheet";
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
//     m.translate( d->canvas->xOffset() / zoomHandler()->zoomedResolutionX(),
//                  d->canvas->yOffset() / zoomHandler()->zoomedResolutionY() );
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
  return d->vBorderWidget->width();
}

int View::rightBorder() const
{
  return d->vertScrollBar->width();
}

int View::topBorder() const
{
  return d->toolWidget->height() + d->hBorderWidget->height();
}

int View::bottomBorder() const
{
  return d->horzScrollBar->height();
}

void View::refreshView()
{
    if ( d->loading ) // "View Loading" not finished yet
        return;
  kDebug(36004) <<"refreshing view";

  Sheet * sheet = activeSheet();
  if ( !sheet )
    return;

  d->adjustActions( !sheet->isProtected() );

  bool active = sheet->getShowFormula();
  if ( sheet && !sheet->isProtected() )
  {
    d->actions->alignLeft->setEnabled( !active );
    d->actions->alignCenter->setEnabled( !active );
    d->actions->alignRight->setEnabled( !active );
  }

  d->tabBar->setReadOnly( !doc()->isReadWrite() || doc()->map()->isProtected() );

  d->toolWidget->setVisible( doc()->showFormulaBar() );
  d->editWidget->showEditWidget( doc()->showFormulaBar() );
  d->hBorderWidget->setVisible( doc()->showColumnHeader() );
  d->vBorderWidget->setVisible( doc()->showRowHeader() );
  d->selectAllButton->setVisible( doc()->showColumnHeader() && doc()->showRowHeader() );
  d->vertScrollBar->setVisible( doc()->showVerticalScrollBar() );
  d->horzScrollBar->setVisible( doc()->showHorizontalScrollBar() );
  d->tabBar->setVisible( doc()->showTabBar() );
  if ( statusBar() ) statusBar()->setVisible( doc()->showStatusBar() );

  d->canvas->updatePosWidget();

  QFont font( KoGlobal::defaultFont() );
  d->hBorderWidget->setMinimumHeight( qRound( zoomHandler()->zoomItY( font.pointSizeF() + 3 ) ) );
  d->vBorderWidget->setMinimumWidth( qRound( zoomHandler()->zoomItX( YBORDER_WIDTH ) ) );
  d->selectAllButton->setMinimumHeight( qRound( zoomHandler()->zoomItY( font.pointSizeF() + 3 ) ) );
  d->selectAllButton->setMinimumWidth( qRound( zoomHandler()->zoomItX( YBORDER_WIDTH ) ) );

  Qt::LayoutDirection sheetDir = sheet->layoutDirection();
  bool interfaceIsRTL = QApplication::isRightToLeft();

//   kDebug(36004)<<" sheetDir == Qt::LeftToRight :"<<( sheetDir == Qt::LeftToRight );
  if ((sheetDir == Qt::LeftToRight && !interfaceIsRTL) ||
      (sheetDir == Qt::RightToLeft && interfaceIsRTL))
  {
    d->formulaBarLayout->setDirection( QBoxLayout::LeftToRight );
    d->viewLayout->setOriginCorner( Qt::TopLeftCorner );
    d->tabBar->setReverseLayout( interfaceIsRTL );
  }
  else
  {
    d->formulaBarLayout->setDirection( QBoxLayout::RightToLeft );
    d->viewLayout->setOriginCorner( Qt::TopRightCorner );
    d->tabBar->setReverseLayout( !interfaceIsRTL );
  }
}

void View::resizeEvent( QResizeEvent * )
{
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

    /*int ret = KMessageBox::warningContinueCancel(this,i18n("You are about to remove this embedded document.\nDo you want to continue?"),i18n("Delete Embedded Document"),KGuiItem(i18n("&Delete"),"edit-delete"));
    if ( ret == KMessageBox::Continue )
    {

}*/
//     doc()->emitBeginOperation(false);
//     d->popupChildObject->sheet()->deleteChild( d->popupChildObject );
//     d->popupChildObject = 0;
//     doc()->emitEndOperation();
}

void View::popupColumnMenu( const QPoint & _point )
{
  assert( d->activeSheet );

  if ( !koDocument()->isReadWrite() )
    return;

    delete d->popupColumn ;

    d->popupColumn = new QMenu( this );

    bool isProtected = d->activeSheet->isProtected();

    if ( !isProtected )
    {
      d->popupColumn->addAction( d->actions->cellLayout );
      d->popupColumn->addSeparator();
      d->popupColumn->addAction( d->actions->cut );
    }
    d->popupColumn->addAction( d->actions->copy );
    if ( !isProtected )
    {
      d->popupColumn->addAction( d->actions->paste );
      d->popupColumn->addAction( d->actions->specialPaste );
      d->popupColumn->addAction( d->actions->insertCellCopy );
      d->popupColumn->addSeparator();
      d->popupColumn->addAction( d->actions->defaultFormat );
      // If there is no selection
      if (!d->selection->isColumnOrRowSelected())
      {
        d->popupColumn->addAction( d->actions->areaName );
      }

      d->popupColumn->addAction( d->actions->resizeColumn );
      d->popupColumn->addAction(KIcon("adjustcol"), i18n("Adjust Column"), this, SLOT(adjustColumn()));
      d->popupColumn->addSeparator();
      d->popupColumn->addAction(KIcon("insert_table_col"), i18n("Insert Columns"), this, SLOT(insertColumn()));
      d->popupColumn->addAction(KIcon("delete_table_col"), i18n("Remove Columns"), this, SLOT(deleteColumn()));
      d->popupColumn->addAction( d->actions->hideColumn );

      d->actions->showSelColumns->setEnabled(false);

      const ColumnFormat* format;
      //kDebug(36001) <<"Column: L:" << rect.left();
      Region::ConstIterator endOfList = d->selection->constEnd();
      for (Region::ConstIterator it = d->selection->constBegin(); it != endOfList; ++it)
      {
        QRect range = (*it)->rect();
        int col;
        for (col = range.left(); col < range.right(); ++col)
        {
          format = activeSheet()->columnFormat(col);

          if ( format->isHidden() )
          {
            d->actions->showSelColumns->setEnabled( true );
            d->popupColumn->addAction( d->actions->showSelColumns );
            break;
          }
        }
        if (range.left() > 1 && col == range.right())
        {
          bool allHidden = true;
          for (col = 1; col < range.left(); ++col)
          {
            format = activeSheet()->columnFormat(col);

            allHidden &= format->isHidden();
          }
          if (allHidden)
          {
            d->actions->showSelColumns->setEnabled( true );
            d->popupColumn->addAction( d->actions->showSelColumns );
            break;
          }
        }
        else
        {
          break;
        }
      }
    }

    connect( d->popupColumn, SIGNAL(activated( int ) ), this, SLOT( slotActivateTool( int ) ) );

    d->popupColumn->popup( _point );
}

void View::popupRowMenu( const QPoint & _point )
{
    assert( d->activeSheet );

    if ( !koDocument()->isReadWrite() )
      return;

    delete d->popupRow ;

    d->popupRow= new QMenu();

    bool isProtected = d->activeSheet->isProtected();

    if ( !isProtected )
    {
        d->popupRow->addAction( d->actions->cellLayout );
        d->popupRow->addSeparator();
        d->popupRow->addAction( d->actions->cut );
    }
    d->popupRow->addAction( d->actions->copy );
    if ( !isProtected )
    {
      d->popupRow->addAction( d->actions->paste );
      d->popupRow->addAction( d->actions->specialPaste );
      d->popupRow->addAction( d->actions->insertCellCopy );
      d->popupRow->addSeparator();
      d->popupRow->addAction( d->actions->defaultFormat );
      // If there is no selection
      if (!d->selection->isColumnOrRowSelected())
      {
        d->popupRow->addAction( d->actions->areaName );
      }

      d->popupRow->addAction( d->actions->resizeRow );
      d->popupRow->addAction(KIcon("adjustrow"), i18n("Adjust Row"), this, SLOT(adjustRow()));
      d->popupRow->addSeparator();
      d->popupRow->addAction(KIcon("insert_table_row"), i18n("Insert Rows"), this, SLOT(insertRow()));
      d->popupRow->addAction(KIcon("delete_table_row"), i18n("Remove Rows"), this, SLOT(deleteRow()));
      d->popupRow->addAction( d->actions->hideRow );

      d->actions->showSelColumns->setEnabled(false);

      const RowFormat* format;
      Region::ConstIterator endOfList = d->selection->constEnd();
      for (Region::ConstIterator it = d->selection->constBegin(); it != endOfList; ++it)
      {
        QRect range = (*it)->rect();
        int row;
        for (row = range.top(); row < range.bottom(); ++row)
        {
          format = activeSheet()->rowFormat(row);

          if ( format->isHidden() )
          {
            d->actions->showSelRows->setEnabled( true );
            d->popupRow->addAction( d->actions->showSelRows );
            break;
          }
        }
        if (range.top() > 1 && row == range.bottom())
        {
          bool allHidden = true;
          for (row = 1; row < range.top(); ++row)
          {
            format = activeSheet()->rowFormat(row);

            allHidden &= format->isHidden();
          }
          if (allHidden)
          {
            d->actions->showSelRows->setEnabled( true );
            d->popupRow->addAction( d->actions->showSelRows );
            break;
          }
        }
        else
        {
          break;
        }
      }
    }

    connect( d->popupRow, SIGNAL( activated( int ) ), this, SLOT( slotActivateTool( int ) ) );
    d->popupRow->popup( _point );
}


void View::slotListChoosePopupMenu( )
{
  if ( !koDocument()->isReadWrite() )
    return;

  assert( d->activeSheet );
  delete d->popupListChoose;

  d->popupListChoose = new QMenu();
  QRect lastRange( d->selection->lastRange() );
  Cell cell( d->activeSheet, d->selection->marker() );
  QString tmp = cell.userInput();
  QStringList itemList;

  for ( int col = lastRange.left(); col <= lastRange.right(); ++col )
  {
    Cell cell = d->activeSheet->cellStorage()->firstInColumn( col );
    while ( !cell.isNull() )
    {
      if ( !cell.isPartOfMerged()
           && !( col == selection()->marker().x()
                 && cell.row() == selection()->marker().y()) )
      {
        if ( cell.value().isString() && cell.userInput() != tmp && !cell.userInput().isEmpty() )
        {
          if ( itemList.indexOf( cell.userInput() ) == -1 )
            itemList.append(cell.userInput());
        }
      }

      cell = d->activeSheet->cellStorage()->nextInColumn( col, cell.row() );
    }
  }

  /* TODO: remove this later:
    for( ;cell; cell = cell.nextCell() )
   {
     int col = cell.column();
     if ( selection.left() <= col && selection.right() >= col
    &&!cell.isPartOfMerged()&& !(col==selection()->marker().x()&& cell.row()==selection()->marker().y()))
       {
   if (cell.isString() && cell.text()!=tmp && !cell.text().isEmpty())
     {
       if (itemList.indexOf(cell.text())==-1)
                 itemList.append(cell.text());
     }

       }
    }
 */

  for ( QStringList::Iterator it = itemList.begin(); it != itemList.end();++it )
  {
    d->popupListChoose->addAction( (*it) );
  }

  if ( itemList.isEmpty() )
    return;
  double tx = d->activeSheet->columnPosition( selection()->marker().x() );
  double ty = d->activeSheet->rowPosition( selection()->marker().y() );
  double h = cell.height();
  const CellView cellView = sheetView( d->activeSheet )->cellView( selection()->marker().x(), selection()->marker().y() );
  if ( cellView.obscuresCells() )
      h = cellView.cellHeight();
  ty += h;

  if ( d->activeSheet->layoutDirection() == Qt::RightToLeft )
  {
    tx = canvasWidget()->width() - tx;
  }

  QPoint p( (int)tx, (int)ty );
  QPoint p2 = d->canvas->mapToGlobal( p );

  if ( d->activeSheet->layoutDirection() == Qt::RightToLeft )
  {
    p2.setX( p2.x() - d->popupListChoose->sizeHint().width() + 1 );
  }

  d->popupListChoose->popup( p2 );
  connect( d->popupListChoose, SIGNAL( triggered( QAction* ) ),
           this, SLOT( slotItemSelected( QAction* ) ) );
}


void View::slotItemSelected( QAction* action )
{
  QString tmp = action->text();
  int x = selection()->marker().x();
  int y = selection()->marker().y();
  Cell cell( d->activeSheet, x, y );
  if (tmp == cell.userInput())
    return;

  DataManipulator *manipulator = new DataManipulator;
  manipulator->setSheet (d->activeSheet);
  manipulator->setValue (Value (tmp));
  manipulator->setParsing (true);
  manipulator->add (QPoint (x, y));
  manipulator->execute ();
}

void View::openPopupMenu( const QPoint & _point )
{
    assert( d->activeSheet );
    delete d->popupMenu;

    if ( !koDocument()->isReadWrite() )
        return;

    d->popupMenu = new QMenu();

#if 0 // KSPREAD_KOPART_EMBEDDING
    EmbeddedObject *obj;
    if ( d->canvas->isObjectSelected() && ( obj = d->canvas->getObject( d->canvas->mapFromGlobal( _point ), d->activeSheet ) ) && obj->isSelected() )
    {
      d->popupMenu->addAction( d->actions->clearAll );
      d->popupMenu->addSeparator();
      d->popupMenu->addAction( d->actions->cut );
      d->popupMenu->addAction( d->actions->copy );
      d->popupMenu->addAction( d->actions->paste );
      d->popupMenu->popup( _point );
      d->popupMenu->addSeparator();
      d->popupMenu->addAction( d->actions->actionExtraProperties );
      return;
    }
#endif // KSPREAD_KOPART_EMBEDDING

    Cell cell = Cell( d->activeSheet, selection()->marker().x(), selection()->marker().y() );

    bool isProtected = d->activeSheet->isProtected();
    if ( cell.style().notProtected() && d->selection->isSingular() )
      isProtected = false;

    if ( !isProtected )
    {
      d->popupMenu->addAction( d->actions->cellLayout );
      d->popupMenu->addSeparator();
      d->popupMenu->addAction( d->actions->cut );
    }
    d->popupMenu->addAction( d->actions->copy );
    if ( !isProtected )
      d->popupMenu->addAction( d->actions->paste );

    if ( !isProtected )
    {
      d->popupMenu->addAction( d->actions->specialPaste );
      d->popupMenu->addAction( d->actions->insertCellCopy );
      d->popupMenu->addSeparator();
      d->popupMenu->addAction(KIcon("deletecell"), i18n("Clear All"), this, SLOT(clearAll()));
      d->popupMenu->addAction( d->actions->adjust );
      d->popupMenu->addAction( d->actions->defaultFormat );

      // If there is no selection
      if (!d->selection->isColumnOrRowSelected())
      {
        d->popupMenu->addAction( d->actions->areaName );
        d->popupMenu->addSeparator();
        d->popupMenu->addAction(KIcon("insertcell"), i18n("Insert Cells..."), this, SLOT(insertCells()));
        d->popupMenu->addAction(KIcon("removecell"), i18n("Remove Cells..."), this, SLOT(deleteCells()));
      }

      d->popupMenu->addSeparator();
      d->popupMenu->addAction( d->actions->comment );
      if ( !Cell( activeSheet(), d->selection->marker() ).comment().isEmpty() )
      {
        d->popupMenu->addAction( d->actions->clearComment );
      }

      if (activeSheet()->testListChoose(selection()))
      {
            d->popupMenu->addSeparator();
            d->popupMenu->addAction( i18n("Selection List..."), this, SLOT( slotListChoosePopupMenu() ) );
      }
    }

    // Remove information about the last tools we offered
    qDeleteAll( d->toolList );
    d->toolList.clear();

    if ( !isProtected && !activeSheet()->wordSpelling( selection() ).isEmpty() )
    {
      d->popupMenuFirstToolId = 10;
      int i = 0;
      QList<KDataToolInfo> tools = KDataToolInfo::query( "QString", "text/plain", doc()->componentData() );
      if ( tools.count() > 0 )
      {
        d->popupMenu->addSeparator();
        QList<KDataToolInfo>::Iterator entry = tools.begin();
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

        connect( d->popupMenu, SIGNAL( activated( int ) ), this, SLOT( slotActivateTool( int ) ) );
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
      kDebug(36001) <<"Could not create Tool";
      return;
  }

  QString text = activeSheet()->wordSpelling( selection() );

  if ( tool->run( entry->command, &text, "QString", "text/plain") )
    activeSheet()->setWordSpelling (selection(), text);
}

void View::clearAll()
{
    DeleteCommand* command = new DeleteCommand();
    command->setSheet( activeSheet() );
    command->add( *selection() );
    command->execute();

#if 0 // KSPREAD_KOPART_EMBEDDING
    if ( canvasWidget()->isObjectSelected() )
    {
      deleteSelectedObjects();
      return;
    }
#endif
}

void View::deleteSelectedObjects()
{
#if 0 // KSPREAD_KOPART_EMBEDDING
    doc()->beginMacro( i18n( "Remove Object" ) );
    foreach ( EmbeddedObject* object, doc()->embeddedObjects() )
    {
        if ( object->sheet() == canvasWidget()->activeSheet() && object->isSelected() )
        {
            RemoveObjectCommand *cmd = new RemoveObjectCommand( object );
            doc()->addCommand( cmd );
        }
    }
    canvasWidget()->setMouseSelectedObject( false );
    doc()->endMacro();
#endif // KSPREAD_KOPART_EMBEDDING
}

void View::adjust()
{
  AdjustColumnRowManipulator* manipulator = new AdjustColumnRowManipulator();
  manipulator->setSheet( d->activeSheet );
  manipulator->setAdjustColumn(true);
  manipulator->setAdjustRow(true);
  manipulator->add( *selection() );
  manipulator->execute();
}

void View::clearContents()
{
  // TODO Stefan: Actually this check belongs into the manipulator!
  if ( d->activeSheet->areaIsEmpty( *selection() ) )
    return;

  DataManipulator* manipulator = new DataManipulator();
  manipulator->setSheet( d->activeSheet );
  manipulator->setText( i18n( "Clear Text" ) );
  // parsing gets set only so that parseUserInput is called as it should be,
  // no actual parsing shall be done
  manipulator->setParsing( true );
  manipulator->setValue( Value( "" ) );
  manipulator->add( *selection() );
  manipulator->execute();
}

void View::clearComment()
{
    // TODO Stefan: Actually this check belongs into the manipulator!
    if ( d->activeSheet->areaIsEmpty( *selection(), Sheet::Comment ) )
        return;

    CommentCommand* manipulator = new CommentCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n( "Remove Comment" ) );
    manipulator->setComment( QString() );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::clearValidity()
{
  // TODO Stefan: Actually this check belongs into the manipulator!
  if ( d->activeSheet->areaIsEmpty( *selection(), Sheet::Validity ) )
    return;

  ValidityCommand* manipulator = new ValidityCommand();
  manipulator->setSheet( d->activeSheet );
  manipulator->setValidity( Validity() ); // empty object removes validity
  manipulator->add( *selection() );
  manipulator->execute();
}

void View::clearConditionalStyles()
{
  // TODO Stefan: Actually this check belongs into the manipulator!
  if ( d->activeSheet->areaIsEmpty( *selection(), Sheet::ConditionalCellAttribute ) )
    return;

  CondtionCommand* manipulator = new CondtionCommand();
  manipulator->setSheet( d->activeSheet );
  manipulator->setConditionList( QLinkedList<Conditional>() );
  manipulator->add( *selection() );
  manipulator->execute();
}

void View::fillRight()
{
  if (!activeSheet())
    return;

  FillManipulator *fm = new FillManipulator ();
  fm->setSheet (activeSheet());
  fm->setDirection (FillManipulator::Right);
  fm->add (*d->selection);
  fm->execute ();
}

void View::fillLeft()
{
  if (!activeSheet())
    return;

  FillManipulator *fm = new FillManipulator ();
  fm->setSheet (activeSheet());
  fm->setDirection (FillManipulator::Left);
  fm->add (*d->selection);
  fm->execute ();
}

void View::fillUp()
{
  if (!activeSheet())
    return;

  FillManipulator *fm = new FillManipulator ();
  fm->setSheet (activeSheet());
  fm->setDirection (FillManipulator::Up);
  fm->add (*d->selection);
  fm->execute ();
}

void View::fillDown()
{
  if (!activeSheet())
    return;

  FillManipulator *fm = new FillManipulator ();
  fm->setSheet (activeSheet());
  fm->setDirection (FillManipulator::Down);
  fm->add (*d->selection);
  fm->execute ();
}

void View::setDefaultStyle()
{
    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setDefault();
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::insertCells()
{
  QRect r( d->selection->lastRange() );
  InsertDialog dlg( this, "InsertDialog", r, InsertDialog::Insert );
  dlg.exec();
}

void View::deleteCells()
{
  QRect r( d->selection->lastRange() );
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
    doc()->emitEndOperation();
  }
  else
  {
    PasteInsertDialog dlg( this, "Remove", d->selection->lastRange() );
    dlg.exec();
  }
  updateEditWidget();
}

void View::setAreaName()
{
    AddNamedAreaDialog dialog(this);
    dialog.exec();
}

void View::namedAreaDialog()
{
    NamedAreaDialog dialog(this);
    dialog.exec();
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

void View::adjustRow()
{
  AdjustColumnRowManipulator* manipulator = new AdjustColumnRowManipulator();
  manipulator->setSheet( d->activeSheet );
  manipulator->setAdjustRow(true);
  manipulator->add( *selection() );
  manipulator->execute();
}

void View::adjustColumn()
{
  AdjustColumnRowManipulator* manipulator = new AdjustColumnRowManipulator();
  manipulator->setSheet( d->activeSheet );
  manipulator->setAdjustColumn(true);
  manipulator->add( *selection() );
  manipulator->execute();
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
    doc()->emitEndOperation();
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
    doc()->emitEndOperation();
  }
}


void View::layoutDlg()
{
    if (!activeSheet())
        return;
    CellFormatDialog dialog(this, d->activeSheet);
    dialog.exec();
}

void View::extraProperties()
{
    if (!activeSheet())
      return;
    //d->canvas->setToolEditMode( TEM_MOUSE );
#if 0 // KSPREAD_KOPART_EMBEDDING
    d->m_propertyEditor = new PropertyEditor( this, "KPrPropertyEditor", d->activeSheet, doc() );
    d->m_propertyEditor->setWindowTitle( i18n( "Properties" ) );

    connect( d->m_propertyEditor, SIGNAL( propertiesOk() ), this, SLOT( propertiesOk() ) );
    d->m_propertyEditor->exec();
    disconnect( d->m_propertyEditor, SIGNAL( propertiesOk() ), this, SLOT( propertiesOk() ) );

    delete d->m_propertyEditor;
    d->m_propertyEditor = 0;
#endif // KSPREAD_KOPART_EMBEDDING
}

void View::propertiesOk()
{
#if 0 // KSPREAD_KOPART_EMBEDDING
    d->m_propertyEditor->executeCommand();
#endif // KSPREAD_KOPART_EMBEDDING
}

void View::styleDialog()
{
    StyleManagerDialog dialog( this, doc()->styleManager() );
    dialog.exec();

    d->actions->selectStyle->setItems( doc()->styleManager()->styleNames() );
    if (d->activeSheet)
        doc()->addDamage(new CellDamage(d->activeSheet, Region(1, 1, KS_colMax, KS_rowMax), CellDamage::Appearance));
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

  pl.width =  print->paperWidth();
  pl.height = print->paperHeight();
  pl.left =   print->leftBorder();
  pl.right =  print->rightBorder();
  pl.top =    print->topBorder();
  pl.bottom = print->bottomBorder();

  KoHeadFoot hf;
  hf.headLeft  = print->localizeHeadFootLine( print->headLeft()  );
  hf.headRight = print->localizeHeadFootLine( print->headRight() );
  hf.headMid   = print->localizeHeadFootLine( print->headMid()   );
  hf.footLeft  = print->localizeHeadFootLine( print->footLeft()  );
  hf.footRight = print->localizeHeadFootLine( print->footRight() );
  hf.footMid   = print->localizeHeadFootLine( print->footMid()   );

  KoUnit unit = doc()->unit();
#ifdef __GNUC__
#warning KDE4 porting needed
#endif
// TODO create a nice dialog with only the properties that KSpread needs
#if 0
  PaperLayout * dlg
    = new PaperLayout( this, "PageLayout", pl, hf,
                              FORMAT_AND_BORDERS | HEADER_AND_FOOTER,
                              unit, d->activeSheet, this );
  dlg->show();
  // dlg destroys itself
#endif
}

void View::definePrintRange()
{
  d->activeSheet->print()->definePrintRange( selection() );
}

void View::resetPrintRange()
{
  d->activeSheet->print()->resetPrintRange();
}

void View::wrapText( bool b )
{
    if ( d->toolbarLock )
        return;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n( "Wrap Text" ) );
    manipulator->setMultiRow( b );
    manipulator->setVerticalText( false );
    manipulator->setAngle( 0 );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::alignLeft( bool b )
{
    if ( d->toolbarLock )
        return;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Horizontal Alignment") );
    manipulator->setHorizontalAlignment( b ? Style::Left : Style::HAlignUndefined );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::alignRight( bool b )
{
    if ( d->toolbarLock )
        return;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Horizontal Alignment") );
    manipulator->setHorizontalAlignment( b ? Style::Right : Style::HAlignUndefined );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::alignCenter( bool b )
{
    if ( d->toolbarLock )
        return;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Horizontal Alignment") );
    manipulator->setHorizontalAlignment( b ? Style::Center : Style::HAlignUndefined );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::alignTop( bool b )
{
    if ( d->toolbarLock )
        return;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Vertical Alignment") );
    manipulator->setVerticalAlignment( b ? Style::Top : Style::VAlignUndefined );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::alignBottom( bool b )
{
    if ( d->toolbarLock )
        return;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Vertical Alignment") );
    manipulator->setVerticalAlignment( b ? Style::Bottom : Style::VAlignUndefined );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::alignMiddle( bool b )
{
    if ( d->toolbarLock )
        return;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n("Change Vertical Alignment") );
    manipulator->setVerticalAlignment( b ? Style::Middle : Style::VAlignUndefined );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::moneyFormat(bool b)
{
    if ( d->toolbarLock )
        return;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n( "Format Money" ) );
    manipulator->setFormatType( b ? Format::Money : Format::Generic );
    manipulator->setPrecision( b ?  d->doc->locale()->fracDigits() : 0 );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::createStyleFromCell()
{
    if ( !d->activeSheet )
        return;

    QPoint p( d->selection->marker() );
    Cell cell = Cell( d->activeSheet, p.x(), p.y() );

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
            KMessageBox::sorry( this, i18n( "The style name cannot be empty." ) );
            continue;
        }

        if ( doc()->styleManager()->style( styleName ) != 0 )
        {
            KMessageBox::sorry( this, i18n( "A style with this name already exists." ) );
            continue;
        }
        break;
    }

    const Style cellStyle = cell.style();
    CustomStyle * style = new CustomStyle( styleName );
    style->merge( cellStyle );

    doc()->styleManager()->insertStyle( style );
    cell.setStyle( *style );
    QStringList lst( d->actions->selectStyle->items() );
    lst.push_back( styleName );
    d->actions->selectStyle->setItems( lst );
}

void View::setStyle( const QString & stylename )
{
    kDebug() <<"View::setStyle(" << stylename <<" )";
    if ( doc()->styleManager()->style( stylename ) )
    {
        StyleCommand* manipulator = new StyleCommand();
        manipulator->setSheet( d->activeSheet );
        manipulator->setParentName( stylename );
        manipulator->add( *selection() );
        manipulator->execute();
    }
}

void View::increasePrecision()
{
    PrecisionCommand* command = new PrecisionCommand();
    command->setSheet(d->activeSheet);
    command->add(*selection());
    if (!command->execute())
        delete command;
}

void View::decreasePrecision()
{
    PrecisionCommand* command = new PrecisionCommand();
    command->setSheet(d->activeSheet);
    command->setReverse(true);
    command->add(*selection());
    if (!command->execute())
        delete command;
}

void View::percent( bool b )
{
    if ( d->toolbarLock )
        return;

    StyleCommand* manipulator = new StyleCommand();
    manipulator->setSheet( d->activeSheet );
    manipulator->setText( i18n( "Format Percent" ) );
    manipulator->setFormatType( b ? Format::Percentage : Format::Generic );
    manipulator->add( *selection() );
    manipulator->execute();
}

void View::insertObject()
{
#if 0 // KSPREAD_KOPART_EMBEDDING
  if (!activeSheet())
    return;

  doc()->emitBeginOperation( false );
  KoDocumentEntry e =  d->actions->insertPart->documentEntry();//KoPartSelectDia::selectPart( d->canvas );
  if ( e.isEmpty() )
  {
    doc()->emitEndOperation();
    return;
  }

  //Don't start handles more than once
  delete d->insertHandler;

  d->insertHandler = new InsertPartHandler( this, d->canvas, e );
  doc()->emitEndOperation();
#endif // KSPREAD_KOPART_EMBEDDING
}

void View::insertChart()
{
#if 0 // KSPREAD_KOPART_EMBEDDING
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
  doc()->emitEndOperation();
#endif // KSPREAD_KOPART_EMBEDDING
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
  d->vBorderWidget->repaint();
  d->hBorderWidget->repaint();
}

void View::zoomPlus()
{
  if ( m_fZoom >= 3 )
    return;

  m_fZoom += 0.25;

  if ( d->activeSheet != 0 )
    d->activeSheet->setLayoutDirtyFlag();

  d->canvas->repaint();
  d->vBorderWidget->repaint();
  d->hBorderWidget->repaint();
}
*/

void View::deleteSheet()
{
  if ( doc()->map()->count() <= 1 || ( doc()->map()->visibleSheets().count() <= 1 ) )
  {
    KMessageBox::sorry( this, i18n("You cannot delete the only sheet."), i18n("Remove Sheet") );
    return;
  }
  int ret = KMessageBox::warningContinueCancel( this, i18n( "You are about to remove the active sheet.\nDo you want to continue?" ),
                                       i18n( "Remove Sheet" ),KGuiItem(i18n("&Delete"),"edit-delete") );

  if ( ret == KMessageBox::Continue )
  {
    doc()->emitBeginOperation( false );
    if ( d->canvas->editor() )
    {
      d->canvas->deleteEditor( false );
    }
    doc()->setModified( true );
    Sheet * tbl = activeSheet();
    QUndoCommand* command = new RemoveSheetCommand( tbl );
    doc()->addCommand( command );


#if 0
    UndoRemoveSheet * undo = new UndoRemoveSheet( doc(), tbl );
    doc()->addCommand( undo );
    tbl->doc()->map()->takeSheet( tbl );
    doc()->takeSheet( tbl );
#endif
    doc()->emitEndOperation();
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

  if ( (newName.trimmed()).isEmpty() ) // Sheet name is empty.
  {
    KMessageBox::information( this, i18n("Sheet name cannot be empty."), i18n("Change Sheet Name") );
    // Recursion
    slotRename();
  }
  else if ( newName != activeName ) // Sheet name changed.
  {
    // Is the name already used
    if ( doc()->map()->findSheet( newName ) )
    {
      KMessageBox::information( this, i18n("This name is already used."), i18n("Change Sheet Name") );
      // Recursion
      slotRename();
      return;
    }

    QUndoCommand* command = new RenameSheetCommand( sheet, newName );
    doc()->addCommand( command );

    //sheet->setSheetName( newName );

    doc()->emitBeginOperation(false);
    updateEditWidget();
    doc()->setModified( true );
    doc()->emitEndOperation();
  }
}

void View::setText( const QString& text, bool expandMatrix )
{
    DataManipulator* manipulator = new DataManipulator();
    manipulator->setSheet( activeSheet() );
    manipulator->setValue( Value( text ) );
    manipulator->setParsing( true );
    manipulator->setExpandMatrix( expandMatrix );
    manipulator->add(expandMatrix ? *selection() : Region(selection()->cursor(), activeSheet()));
    manipulator->execute();

    if ( expandMatrix && selection()->isSingular() )
        selection()->initialize( *manipulator );

    Cell cell = Cell( activeSheet(), selection()->marker() );
    if ( cell.value().isString() && !text.isEmpty() && !text.at(0).isDigit() && !cell.isFormula() )
        doc()->addStringCompletion( text );
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
  d->selectAllButton->repaint();
}

void View::slotUpdateView( Sheet *_sheet )
{
  // Do we display this sheet ?
  if ( ( !activeSheet() ) || ( _sheet != d->activeSheet ) )
    return;

  d->activeSheet->setRegionPaintDirty( d->canvas->visibleCells() );
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
  doc()->emitEndOperation();
}

void View::slotUpdateView( EmbeddedObject *obj )
{
#if 0 // KSPREAD_KOPART_EMBEDDING
  d->canvas->repaintObject( obj );
#endif // KSPREAD_KOPART_EMBEDDING
}

void View::slotUpdateHBorder( Sheet * _sheet )
{
  // kDebug(36001)<<"void View::slotUpdateHBorder( Sheet *_sheet )";

  // Do we display this sheet ?
  if ( _sheet != d->activeSheet )
    return;

  doc()->emitBeginOperation(false);
  d->hBorderWidget->update();
  doc()->emitEndOperation();
}

void View::slotUpdateVBorder( Sheet *_sheet )
{
  // kDebug("void View::slotUpdateVBorder( Sheet *_sheet )";

  // Do we display this sheet ?
  if ( _sheet != d->activeSheet )
    return;

  doc()->emitBeginOperation( false );
  d->vBorderWidget->update();
  doc()->emitEndOperation();
}

void View::slotChangeSelection(const KSpread::Region& changedRegion)
{
  if (!changedRegion.isValid())
    return;

  bool colSelected = d->selection->isColumnSelected();
  bool rowSelected = d->selection->isRowSelected();
  if (d->activeSheet && !d->activeSheet->isProtected())
  {
    // Activate or deactivate some actions.
    d->actions->insertRow->setEnabled( !colSelected );
    d->actions->deleteRow->setEnabled( !colSelected );
    d->actions->resizeRow->setEnabled( !colSelected );
    d->actions->equalizeRow->setEnabled( !colSelected );
    d->actions->hideRow->setEnabled( !colSelected );
    d->actions->validity->setEnabled( !colSelected && !rowSelected);
    d->actions->conditional->setEnabled( !colSelected && !rowSelected);
    d->actions->insertColumn->setEnabled( !rowSelected );
    d->actions->deleteColumn->setEnabled( !rowSelected );
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
    d->actions->sortDec->setEnabled( !simpleSelection );
    d->actions->sortInc->setEnabled( !simpleSelection);
    d->actions->createStyle->setEnabled( simpleSelection ); // just from one cell

    bool contiguousSelection = d->selection->isContiguous();
    d->actions->subTotals->setEnabled(contiguousSelection);
  }
  d->actions->selectStyle->setCurrentItem( -1 );
  // delayed recalculation of the operation shown in the status bar
  d->statusBarOpTimer.setSingleShot(true);
  d->statusBarOpTimer.start(250);

  if ( !d->loading )
    doc()->addDamage( new SelectionDamage( changedRegion ) );
  d->vBorderWidget->update();
  d->hBorderWidget->update();
  d->selectAllButton->update();

  if (colSelected || rowSelected)
    return;

  d->canvas->validateSelection();

  //Don't scroll to the marker if there is an active embedded object, since this may cause
  //the canvas to scroll so that the object isn't in the visible area.
  //There is still the problem of the object no longer being visible immediately after deactivating the child
  //as the sheet jumps back to the marker though.
  if (!activeChild())
    d->canvas->scrollToCell(selection()->marker());

  // Perhaps the user is entering a value in the cell.
  // In this case we may not touch the EditWidget
  if ( !d->canvas->editor() && !d->canvas->chooseMode() )
    updateEditWidgetOnPress();
  d->canvas->updatePosWidget();
}

void View::slotChangeChoice(const KSpread::Region& changedRegion)
{
  if (!changedRegion.isValid())
  {
    return;
  }
  doc()->emitBeginOperation( false );
  d->canvas->updateEditor();
  doc()->addDamage(new SelectionDamage(changedRegion));
  doc()->emitEndOperation();
  kDebug(36002) <<"Choice:" << *choice();
}

void View::slotScrollChoice(const KSpread::Region& changedRegion)
{
    if ( !changedRegion.isValid() )
        return;
    d->canvas->scrollToCell( choice()->marker() );
}

void View::calcStatusBarOp()
{
  Sheet * sheet = activeSheet();
  ValueCalc* calc = d->doc->calc();
  Value val;
  MethodOfCalc tmpMethod = doc()->getTypeOfCalc();
  if ( tmpMethod != NoneCalc )
  {
    Value range = sheet->cellStorage()->valueRegion( *d->selection );
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

  if ( d->calcLabel )
    d->calcLabel->setText(QString(' ') + tmp + ' ');
}

void View::statusBarClicked(const QPoint&)
{
  QPoint mousepos = QCursor::pos();
  if ( koDocument()->isReadWrite() && factory() )
    if( QMenu* menu = dynamic_cast<QMenu*>( factory()->container( "calc_popup" , this ) ) )
      menu->popup( mousepos );
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

  doc()->emitEndOperation();
}


QMatrix View::matrix() const
{
  QMatrix m;
  m.scale( zoomHandler()->zoomedResolutionX(),
           zoomHandler()->zoomedResolutionY() );
  m.translate( - d->canvas->xOffset(), - d->canvas->yOffset() );
  return m;
}

void View::transformPart()
{
#if 0 // KSPREAD_KOPART_EMBEDDING
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
#endif // KSPREAD_KOPART_EMBEDDING
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
  d->activeSheet->setRegionPaintDirty(QRect(QPoint(1,1), QPoint(KS_colMax, KS_rowMax)));
  doc()->emitEndOperation();
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
  d->activeSheet->setRegionPaintDirty(QRect(QPoint(1,1), QPoint(KS_colMax, KS_rowMax)));
  doc()->emitEndOperation();
}


void View::deleteEditor( bool saveChanges )
{
    doc()->emitBeginOperation( false );
    d->canvas->deleteEditor( saveChanges );

    markSelectionAsDirty();
    doc()->emitEndOperation();
}

QWidget* View::canvas() const
{
    return d->canvas;
}

void View::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
  if ( d->activeSheet )
  {
    doc()->emitEndOperation();

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
    if (!koDocument()->isReadWrite() || !factory())
        return;
    if (d->tabBar)
    {
        QMenu* const menu = static_cast<QMenu*>(factory()->container("menupage_popup", this));

        QAction* insertSheet = new KAction(KIcon("inserttable"), i18n("Insert Sheet"), this);
        insertSheet->setToolTip(i18n("Remove the active sheet"));
        connect(insertSheet, SIGNAL(triggered(bool)), this, SLOT(insertSheet()));
        menu->insertAction(d->actions->duplicateSheet, insertSheet);

        QAction* deleteSheet = new KAction(KIcon("delete_table"), i18n("Remove Sheet"), this);
        deleteSheet->setToolTip(i18n("Remove the active sheet"));
        connect(deleteSheet, SIGNAL(triggered(bool)), this, SLOT(deleteSheet()));
        menu->insertAction(d->actions->hideSheet, deleteSheet);

        bool state = (doc()->map()->visibleSheets().count() > 1);
        if (d->activeSheet && d->activeSheet->isProtected())
        {
            deleteSheet->setEnabled(false);
            d->actions->hideSheet->setEnabled(false);
            d->actions->showSheet->setEnabled(false);
        }
        else
        {
            deleteSheet->setEnabled(state);
            d->actions->hideSheet->setEnabled(state);
            d->actions->showSheet->setEnabled(doc()->map()->hiddenSheets().count() > 0);
        }
        if (!doc() || !doc()->map() || doc()->map()->isProtected())
        {
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
  d->actions->deleteSheet->setEnabled( state );
  d->actions->hideSheet->setEnabled( state );
  doc()->emitEndOperation();
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
  d->actions->deleteSheet->setEnabled( state );
  d->actions->hideSheet->setEnabled( state );
  doc()->emitEndOperation();
}

QColor View::borderColor() const
{
    return d->canvas->resourceProvider()->foregroundColor().toQColor();
}

void View::updateShowSheetMenu()
{
  doc()->emitBeginOperation( false );
  if ( d->activeSheet->isProtected() )
    d->actions->showSheet->setEnabled( false );
  else
    d->actions->showSheet->setEnabled( doc()->map()->hiddenSheets().count() > 0 );
  doc()->emitEndOperation();
}

void View::closeEditor()
{
    if (!d->activeSheet) // #45822
        return;
    d->canvas->closeEditor();
    markSelectionAsDirty();
}

void View::markSelectionAsDirty()
{
    doc()->addDamage(new SelectionDamage(d->canvas->chooseMode() ? *choice() : *selection()));
}

void View::paintUpdates()
{
  /* don't do any begin/end operation here -- this is what is called at an
     endOperation
  */
  d->canvas->update();
}

void View::initialiseMarkerFromSheet( Sheet* sheet, const QPoint& point )
{
  d->savedMarkers.remove( sheet );
  d->savedMarkers.insert( sheet, point );
}

QPoint View::markerFromSheet( Sheet* sheet ) const
{
  QMap<Sheet*, QPoint>::Iterator it = d->savedMarkers.find(sheet);
  QPoint newMarker = (it == d->savedMarkers.end()) ? QPoint(1,1) : *it;
  return newMarker;
}

QPointF View::offsetFromSheet( Sheet* sheet ) const
{
  QMap<Sheet*, QPointF>::Iterator it = d->savedOffsets.find(sheet);
  QPointF offset = (it == d->savedOffsets.end()) ? QPointF() : *it;
  return offset;
}

void View::saveCurrentSheetSelection()
{
    /* save the current selection on this sheet */
    if (d->activeSheet != 0)
    {
      d->savedAnchors.remove(d->activeSheet);
      d->savedAnchors.insert(d->activeSheet, d->selection->anchor());
      kDebug(36005) <<" Current scrollbar vert value:" << d->vertScrollBar->value();
      kDebug(36005) <<"Saving marker pos:" << d->selection->marker();
      d->savedMarkers.remove(d->activeSheet);
      d->savedMarkers.insert(d->activeSheet, d->selection->marker());
      d->savedOffsets.remove(d->activeSheet);
      d->savedOffsets.insert(d->activeSheet, QPointF(d->canvas->xOffset(),
                                                     d->canvas->yOffset()));
    }
}

void View::handleDamages( const QList<Damage*>& damages )
{
    bool refreshView = false;
    QRegion paintRegion;
    bool paintClipped = true;

    QList<Damage*>::ConstIterator end(damages.end());
    for( QList<Damage*>::ConstIterator it = damages.begin(); it != end; ++it )
    {
        Damage* damage = *it;
        if( !damage ) continue;

        if( damage->type() == Damage::Cell )
        {
            CellDamage* cellDamage = static_cast<CellDamage*>( damage );
            kDebug(36007) <<"Processing\t" << *cellDamage;
            Sheet* const damagedSheet = cellDamage->sheet();
            const Region region = cellDamage->region();

            if ( cellDamage->changes() & CellDamage::Appearance )
            {
                sheetView( damagedSheet )->invalidateRegion( region );
                paintClipped = false;
            }
            continue;
        }

        if( damage->type() == Damage::Sheet )
        {
            SheetDamage* sheetDamage = static_cast<SheetDamage*>( damage );
            kDebug(36007) <<"Processing\t" << *sheetDamage;

            if ( sheetDamage->changes() & SheetDamage::PropertiesChanged )
            {
                d->activeSheet->setRegionPaintDirty( d->canvas->visibleCells() );
                refreshView = true;
            }
            if ( sheetDamage->changes() & SheetDamage::ColumnsChanged )
                hBorderWidget()->update();
            if ( sheetDamage->changes() & SheetDamage::RowsChanged )
                vBorderWidget()->update();
            continue;
        }

        if( damage->type() == Damage::Selection )
        {
            SelectionDamage* selectionDamage = static_cast<SelectionDamage*>( damage );
            kDebug(36007) <<"Processing\t" << *selectionDamage;
            const Region region = selectionDamage->region();

            if ( paintClipped )
            {
                const QRectF rect = canvasWidget()->cellCoordinatesToView( region.boundingRect() );
                paintRegion += rect.toRect().adjusted( -3, -3, 4, 4 );
            }
            continue;
        }

        kDebug(36007) <<"Unhandled\t" << *damage;
    }

    // At last repaint the dirty cells.
    if ( paintClipped )
        canvas()->update( paintRegion );
    else
        canvas()->update();
    if ( refreshView )
        this->refreshView();
}

void View::runInspector()
{
    // useful to inspect objects
    if(!d->activeSheet) return;
    Cell cell( d->activeSheet, d->selection->marker() );
    KSpread::Inspector* ins = new KSpread::Inspector( cell );
    ins->exec();
    delete ins;
}

} // namespace KSpread

#include "View.moc"
