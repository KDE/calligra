/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 - 2003 The KSpread Team
                             www.koffice.org/kspread

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kprinter.h> // has to be first

#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include <qcursor.h>
#include <qlayout.h>
#include <qpaintdevicemetrics.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qtoolbutton.h>

#include <klineeditdlg.h>
#include <kprocio.h>
#include <kspell.h>
#include <kspelldlg.h>
#include <kstdaction.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <ktempfile.h>
#include <kstandarddirs.h>
#include <kpassdlg.h>
#include <tkcoloractions.h>

#include <dcopclient.h>
#include <dcopref.h>

#include <kdebug.h>
#include <kstatusbar.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kfind.h>
#include <kreplace.h>
#include <kfinddialog.h>
#include <kreplacedialog.h>
#include <koCharSelectDia.h>
#include <koMainWindow.h>
#include <koPartSelectAction.h>
#include <koTemplateCreateDia.h>

#include <kparts/partmanager.h>

#include "kspread_changes.h"
#include "kspread_sheetprint.h"
#include "kspread_map.h"
#include "kspread_dlg_scripts.h"
#include "kspread_dlg_changes.h"
#include "kspread_dlg_csv.h"
#include "kspread_dlg_cons.h"
#include "kspread_dlg_database.h"
#include "kspread_dlg_goalseek.h"
//#include "kspread_dlg_multipleop.h"
#include "kspread_dlg_subtotal.h"
#include "kspread_canvas.h"
#include "kspread_tabbar.h"
#include "kspread_dlg_formula.h"
#include "kspread_dlg_special.h"
#include "kspread_dlg_sort.h"
#include "kspread_dlg_anchor.h"
#include "kspread_dlg_layout.h"
#include "kspread_dlg_show.h"
#include "kspread_dlg_insert.h"
#include "kspread_global.h"
#include "kspread_handler.h"
#include "kspread_events.h"
#include "kspread_editors.h"
#include "kspread_dlg_format.h"
#include "kspread_dlg_conditional.h"
#include "kspread_dlg_series.h"
#include "kspread_dlg_reference.h"
#include "kspread_dlg_area.h"
#include "kspread_dlg_resize2.h"
#include "kspread_dlg_preference.h"
#include "kspread_dlg_comment.h"
#include "kspread_dlg_angle.h"
#include "kspread_dlg_goto.h"
#include "kspread_dlg_validity.h"
#include "kspread_dlg_pasteinsert.h"
#include "kspread_dlg_showColRow.h"
#include "kspread_dlg_styles.h"
#include "kspread_dlg_list.h"
#include "kspread_undo.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "handler.h"
#include "digest.h"

#include "KSpreadViewIface.h"
#include "kspread_dlg_paperlayout.h"

KSpreadScripts* KSpreadView::m_pGlobalScriptsDialog = 0L;

// non flickering version of KSpell.
// DF: those fixes have been applied to kde-3.2-pre, so KSpreadSpell
// can go away when koffice requires kde-3.2.
class KSpreadSpell : public KSpell
{
 public:
  KSpreadSpell(QWidget *parent, const QString &caption,
               QObject *receiver, const char *slot, KSpellConfig *kcs=0,
               bool progressbar = FALSE, bool modal = true )
    : KSpell(parent, caption, receiver, slot, kcs, progressbar, modal)
  {
  }

  // override check(...)
  // mostly copied from kdelibs/kspell/kspell.cpp
  // the dialog gets created but it gets only shown if something
  // is misspelled. Otherwise for every cell the dialog would pop up
  // and disappear
  bool check( const QString &_buffer, bool _usedialog = true )
  {
    QString qs;

    usedialog=_usedialog;
    setUpDialog ();
    //set the dialog signal handler
    dialog3slot = SLOT (check3 ());

    kdDebug(750) << "KS: check" << endl;
    origbuffer = _buffer;
    if ( ( totalpos = origbuffer.length() ) == 0 )
    {
      emit done(origbuffer);
      return FALSE;
    }

    // Torben: I corrected the \n\n problem directly in the
    //         origbuffer since I got errors otherwise
    if ( origbuffer.right(2) != "\n\n" )
    {
      if (origbuffer.at(origbuffer.length() - 1) != '\n')
      {
        origbuffer += '\n';
        origbuffer += '\n'; //shouldn't these be removed at some point?
      }
      else
	origbuffer += '\n';
    }

    newbuffer = origbuffer;

    // KProcIO calls check2 when read from ispell
    connect(proc, SIGNAL (readReady(KProcIO *)), this, SLOT (check2(KProcIO *)));

    proc->fputs ("!");

    //lastpos is a position in newbuffer (it has offset in it)
    offset = lastlastline = lastpos = lastline = 0;

    emitProgress ();

    // send first buffer line
    int i = origbuffer.find('\n', 0) + 1;
    qs = origbuffer.mid (0, i);
    cleanFputs(qs, FALSE);

    lastline = i; //the character position, not a line number

    ksdlg->hide();

    return TRUE;
  }

  // mostly copied from kdelibs/kspell/kspell.cpp
  void check2 (KProcIO *)
  {
    int e, tempe;
    QString word;
    QString line;

    do
    {
      tempe = proc->fgets (line); //get ispell's response

      if (tempe > 0)
      {
        e = parseOneResponse(line, word, sugg);
        if ( (e == 3) // mistake
            || (e == 2) ) // replace
        {
          dlgresult =- 1;

          // for multibyte encoding posinline needs correction
          if (ksconfig->encoding() == KS_E_UTF8)
          {
            // convert line to UTF-8, cut at pos, convert back to UCS-2
            // and get string length
            posinline = (QString::fromUtf8(origbuffer.mid(lastlastline,
                                                          lastline - lastlastline).utf8(),
                                           posinline)).length();
          }

          lastpos = posinline + lastlastline + offset;

          //orig is set by parseOneResponse()

          if (e == 2) // replace
          {
            dlgreplacement = word;
            emit corrected (orig, replacement(), lastpos);
            offset += replacement().length() - orig.length();
            newbuffer.replace (lastpos, orig.length(), word);
          }
          else  //MISTAKE
          {
            cwword = word;
            if ( usedialog )
            {
              // show the word in the dialog
              ksdlg->show();
              dialog (word, sugg, SLOT (check3()));
            }
            else
            {
              // No dialog, just emit misspelling and continue
              emit misspelling (word, sugg, lastpos);
              dlgresult = KS_IGNORE;
              check3();
            }
            return;
          }
        }

      }

      emitProgress (); //maybe

    } while (tempe > 0);

    proc->ackRead();


    if (tempe == -1) //we were called, but no data seems to be ready...
      return;

    //If there is more to check, then send another line to ISpell.
    if ((unsigned int)lastline < origbuffer.length())
    {
      int i;
      QString qs;

      lastpos = (lastlastline = lastline) + offset; //do we really want this?
      i = origbuffer.find('\n', lastline)+1;
      qs = origbuffer.mid (lastline, i-lastline);
      cleanFputs (qs, FALSE);
      lastline = i;
      return;
    }
    else
      //This is the end of it all
    {
      ksdlg->hide();
      newbuffer.truncate (newbuffer.length()-2);
      emitProgress();
      emit done (newbuffer);
    }
  }
};

class ViewActions
{
public:

    // cell formatting
    KAction* cellLayout;
    KAction* defaultFormat;
    KSelectAction* selectStyle;
    KToggleAction* bold;
    KToggleAction* italic;
    KToggleAction* underline;
    KToggleAction* strikeOut;
    KToggleAction* percent;
    KAction* precplus;
    KAction* precminus;
    KToggleAction* money;
    KToggleAction* alignLeft;
    KToggleAction* alignCenter;
    KToggleAction* alignRight;
    KToggleAction* alignTop;
    KToggleAction* alignMiddle;
    KToggleAction* alignBottom;
    KToggleAction* multiRow;
    KFontAction* selectFont;
    KFontSizeAction* selectFontSize;
    KToggleAction* verticalText;
    KAction* increaseIndent;
    KAction* decreaseIndent;
    KAction* fontSizeUp;
    KAction* fontSizeDown;
    KAction* upper;
    KAction* lower;
    KAction* firstLetterUpper;
    KAction* changeAngle;
    TKSelectColorAction* textColor;
    TKSelectColorAction* bgColor;
    KAction* borderLeft;
    KAction* borderRight;
    KAction* borderTop;
    KAction* borderBottom;
    KAction* borderAll;
    KAction* borderOutline;
    KAction* borderRemove;
    TKSelectColorAction* borderColor;

    // cell operations
    KAction* editCell;
    KAction* insertCell;
    KAction* removeCell;
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
    KAction* createStyle;

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
    KAction* insertTable;
    KAction* menuInsertTable;
    KAction* removeTable;
    KAction* renameTable;
    KAction* hideTable;
    KAction* showTable;
    KAction* tableFormat;
    KAction* areaName;
    KAction* showArea;
    KAction* insertSeries;
    KAction* insertFunction;
    KAction* insertSpecialChar;
    KAction* insertFromDatabase;
    KAction* insertFromTextfile;
    KAction* insertFromClipboard;
    KAction* transform;
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
    KAction* recalc_worksheet;
    KAction* recalc_workbook;
    KToggleAction* protectSheet;
    KToggleAction* protectDoc;

    // general editing
    KAction* cut;
    KAction* copy;
    KAction* paste;
    KAction* del;
    KAction* specialPaste;
    KAction* insertCellCopy;
    KAction* undo;
    KAction* redo;
    KAction* findAction;
    KAction* replaceAction;

    // navigation
    KAction* gotoCell;
    KAction* nextTable;
    KAction* prevTable;
    KAction* firstTable;
    KAction* lastTable;

    // misc
    KAction* styleDialog;
    KAction* autoSum;
    KSelectAction* formulaSelection;
    KAction* insertLink;
    KSelectAction* viewZoom;
    KAction* sort;
    KAction* consolidate;
    KAction* goalSeek;
    KAction* subTotals;
    KAction* textToColumns;
    KAction* multipleOperations;
    KAction* createTemplate;
    KoPartSelectAction *insertPart;
    KAction* insertChartFrame;
    KAction* sortList;
    KAction* spellChecking;
    KAction* preference;
    KAction* help;

    // running calculation
    KToggleAction* menuCalcMin;
    KToggleAction* menuCalcMax;
    KToggleAction* menuCalcAverage;
    KToggleAction* menuCalcCount;
    KToggleAction* menuCalcSum;
    KToggleAction* menuCalcNone;

    // scripts
    KActionMenu* scripts;
    KAction* editGlobalScripts;
    KAction* editLocalScripts;
    KAction* reloadScripts;

    // revision control
    KToggleAction* recordChanges;
    KToggleAction* protectChanges;
    KAction* filterChanges;
    KAction* acceptRejectChanges;
    KAction* commentChanges;
    KAction* mergeDocument;

};

class ViewPrivate
{
public:
    KSpreadView* view;

    ViewActions* actions;

    void initActions();
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


}


/*****************************************************************************
 *
 * KSpreadView
 *
 *****************************************************************************/

KSpreadView::KSpreadView( QWidget *_parent, const char *_name, KSpreadDoc* doc )
  : KoView( doc, _parent, _name )
{
    ElapsedTime et( "KSpreadView constructor" );

    d = new ViewPrivate;
    d->view = this;

    m_popupMenuFirstToolId = 0;
    kdDebug(36001) << "sizeof(KSpreadCell)=" << sizeof(KSpreadCell) <<endl;
    setInstance( KSpreadFactory::global() );
    if ( doc->isReadWrite() )
      setXMLFile( "kspread.rc" );
    else
      setXMLFile( "kspread_readonly.rc" );

    m_pTable       = NULL;
    m_toolbarLock  = FALSE;
    m_sbCalcLabel  = 0;
    m_pDoc         = doc;
    m_pPopupMenu   = 0;
    m_pPopupColumn = 0;
    m_pPopupRow    = 0;
    m_popupChild   = 0;
    m_popupListChoose = 0;
    m_spell.kspell    = 0;
    // a few words to ignore when spell checking

    m_dcop = 0;
    dcopObject(); // build it
    m_bLoading = false;

    m_pInsertHandle = 0L;

    m_specialCharDlg = 0;

    m_selectionInfo = new KSpreadSelection( this );

    // Vert. Scroll Bar
    m_pVertScrollBar = new QScrollBar( this, "ScrollBar_2" );
    m_pVertScrollBar->setRange( 0, 4096 );
    m_pVertScrollBar->setOrientation( QScrollBar::Vertical );

    // Horz. Scroll Bar
    m_pHorzScrollBar = new QScrollBar( this, "ScrollBar_1" );
    m_pHorzScrollBar->setRange( 0, 4096 );
    m_pHorzScrollBar->setOrientation( QScrollBar::Horizontal );

    m_pTabBar = new KSpread::TabBar( this );
    m_pTabBar->setReadOnly( !m_pDoc->isReadWrite() );
    QObject::connect( m_pTabBar, SIGNAL( tabChanged( const QString& ) ), this, SLOT( changeTable( const QString& ) ) );
    QObject::connect( m_pTabBar, SIGNAL( tabMoved( unsigned, unsigned ) ),
      this, SLOT( moveTable( unsigned, unsigned ) ) );
    QObject::connect( m_pTabBar, SIGNAL( contextMenu( const QPoint& ) ),
      this, SLOT( popupTabBarMenu( const QPoint& ) ) );
    QObject::connect( m_pTabBar, SIGNAL( doubleClicked() ),
      this, SLOT( slotRename() ) );

    // Paper and Border widgets
    m_pFrame = new QWidget( this );
    m_pFrame->raise();

    // Edit Bar
    m_pToolWidget = new QFrame( this );

    QHBoxLayout* hbox = new QHBoxLayout( m_pToolWidget );
    hbox->addSpacing( 2 );

    m_pPosWidget = new KSpreadLocationEditWidget( m_pToolWidget, this );

    m_pPosWidget->setMinimumWidth( 100 );
    hbox->addWidget( m_pPosWidget );
    hbox->addSpacing( 6 );

    m_pCancelButton = newIconButton( "cancel", TRUE, m_pToolWidget );
    hbox->addWidget( m_pCancelButton );
    m_pOkButton = newIconButton( "ok", TRUE, m_pToolWidget );
    hbox->addWidget( m_pOkButton );
    hbox->addSpacing( 6 );

    // The widget on which we display the table
    m_pCanvas = new KSpreadCanvas( m_pFrame, this, doc );

    // The line-editor that appears above the table and allows to
    // edit the cells content. It knows about the two buttons.
    m_pEditWidget = new KSpreadEditWidget( m_pToolWidget, m_pCanvas, m_pCancelButton, m_pOkButton );
    m_pEditWidget->setFocusPolicy( QWidget::StrongFocus );
    hbox->addWidget( m_pEditWidget, 2 );
    hbox->addSpacing( 2 );

    m_pCanvas->setEditWidget( m_pEditWidget );

    m_pHBorderWidget = new KSpreadHBorder( m_pFrame, m_pCanvas,this );
    m_pVBorderWidget = new KSpreadVBorder( m_pFrame, m_pCanvas ,this );

    m_pCanvas->setFocusPolicy( QWidget::StrongFocus );
    QWidget::setFocusPolicy( QWidget::StrongFocus );
    setFocusProxy( m_pCanvas );

    connect( this, SIGNAL( invalidated() ), m_pCanvas, SLOT( update() ) );

    QObject::connect( m_pVertScrollBar, SIGNAL( valueChanged(int) ), m_pCanvas, SLOT( slotScrollVert(int) ) );
    QObject::connect( m_pHorzScrollBar, SIGNAL( valueChanged(int) ), m_pCanvas, SLOT( slotScrollHorz(int) ) );

    // Handler for moving and resizing embedded parts
    ContainerHandler* h = new ContainerHandler( this, m_pCanvas );
    connect( h, SIGNAL( popupMenu( KoChild*, const QPoint& ) ), this, SLOT( popupChildMenu( KoChild*, const QPoint& ) ) );


    connect( this, SIGNAL( childSelected( KoDocumentChild* ) ),
             this, SLOT( slotChildSelected( KoDocumentChild* ) ) );
    connect( this, SIGNAL( childUnselected( KoDocumentChild* ) ),
             this, SLOT( slotChildUnselected( KoDocumentChild* ) ) );
    // If a selected part becomes active this is like it is deselected
    // just before.
    connect( this, SIGNAL( childActivated( KoDocumentChild* ) ),
             this, SLOT( slotChildUnselected( KoDocumentChild* ) ) );

    QTimer::singleShot( 0, this, SLOT( initialPosition() ) );
    m_findOptions = 0;
    m_find = 0L;
    m_replace = 0L;

    KStatusBar * sb = statusBar();
    Q_ASSERT(sb);
    m_sbCalcLabel = sb ? new KStatusBarLabel( QString::null, 0, sb ) : 0;
    addStatusBarItem( m_sbCalcLabel, 0 );
    if (m_sbCalcLabel)
        connect(m_sbCalcLabel ,SIGNAL(itemPressed( int )),this,SLOT(statusBarClicked(int)));

    d->initActions();
    initializeCalcActions();
    initializeInsertActions();
    initializeEditActions();
    initializeAreaOperationActions();
    initializeGlobalOperationActions();
    initializeCellOperationActions();
    initializeCellPropertyActions();
    initializeTextFormatActions();
    initializeTextLayoutActions();
    initializeTextPropertyActions();
    initializeTableActions();
    initializeSpellChecking();
    initializeRowColumnActions();
    initializeBorderActions();

    QPtrListIterator<KSpreadSheet> it( m_pDoc->map()->tableList() );
    for( ; it.current(); ++it )
      addTable( it.current() );

    KSpreadSheet * tbl = 0L;
    if ( m_pDoc->isEmbedded() )
    {
        tbl = m_pDoc->displayTable();
    }

    if ( !tbl )
        tbl = m_pDoc->map()->initialActiveTable();
    if ( tbl )
      setActiveTable( tbl );
    else
    {
      //activate first table which is not hiding
      tbl = m_pDoc->map()->findTable( m_pDoc->map()->visibleSheets().first());
      if ( !tbl )
      {
        tbl = m_pDoc->map()->firstTable();
        if ( tbl )
        {
          tbl->setHidden( false );
          QString tabName = tbl->tableName();
          m_pTabBar->addTab( tabName );
        }
      }
      setActiveTable( tbl );
    }

    QObject::connect( m_pDoc, SIGNAL( sig_addTable( KSpreadSheet* ) ), SLOT( slotAddTable( KSpreadSheet* ) ) );


    QObject::connect( m_pDoc, SIGNAL( sig_refreshView(  ) ), this, SLOT( slotRefreshView() ) );

    QObject::connect( m_pDoc, SIGNAL( sig_refreshLocale() ), this, SLOT( refreshLocale()));

    KoView::setZoom( m_pDoc->zoomedResolutionY() /* KoView only supports one zoom */ ); // initial value
    //when kspread is embedded into konqueror apply a zoom=100
    //in konqueror we can't change zoom -- ### TODO ?
    if (!m_pDoc->isReadWrite())
    {
        setZoom( 100, true );
    }

    viewZoom( QString::number( m_pDoc->zoom() ) );

    QStringList list = d->actions->viewZoom->items();
    QString zoomStr( i18n("%1%").arg( m_pDoc->zoom()) );
    d->actions->viewZoom->setCurrentItem( list.findIndex(zoomStr)  );

    d->actions->selectStyle->setItems( m_pDoc->styleManager()->styleNames() );

    adjustActions( !m_pTable->isProtected() );
    adjustMapActions( !m_pDoc->map()->isProtected() );
}


void KSpreadView::initializeCalcActions()
{
  //menu calc
  /*******************************/
  d->actions->menuCalcSum = new KToggleAction( i18n("Sum"), 0, actionCollection(),
                                     "menu_sum");
  connect( d->actions->menuCalcSum, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  d->actions->menuCalcSum->setExclusiveGroup( "Calc" );
  d->actions->menuCalcSum->setToolTip(i18n("Calculate using sum."));

  /*******************************/
  d->actions->menuCalcMin = new KToggleAction( i18n("Min"), 0, actionCollection(),
                                     "menu_min");
  connect( d->actions->menuCalcMin, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  d->actions->menuCalcMin->setExclusiveGroup( "Calc" );
  d->actions->menuCalcMin->setToolTip(i18n("Calculate using minimum."));

  /*******************************/
  d->actions->menuCalcMax = new KToggleAction( i18n("Max"), 0, actionCollection(),
                                     "menu_max");
  connect( d->actions->menuCalcMax, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  d->actions->menuCalcMax->setExclusiveGroup( "Calc" );
  d->actions->menuCalcMax->setToolTip(i18n("Calculate using maximum."));

  /*******************************/
  d->actions->menuCalcAverage = new KToggleAction( i18n("Average"), 0, actionCollection(),
                                         "menu_average");
  connect( d->actions->menuCalcAverage, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  d->actions->menuCalcAverage->setExclusiveGroup( "Calc" );
  d->actions->menuCalcAverage->setToolTip(i18n("Calculate using average."));

  /*******************************/
  d->actions->menuCalcCount = new KToggleAction( i18n("Count"), 0, actionCollection(),
                                       "menu_count");
  connect( d->actions->menuCalcCount, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  d->actions->menuCalcCount->setExclusiveGroup( "Calc" );
  d->actions->menuCalcCount->setToolTip(i18n("Calculate using the count."));


  /*******************************/
  d->actions->menuCalcNone = new KToggleAction( i18n("None"), 0, actionCollection(),
                                      "menu_none");
  connect( d->actions->menuCalcNone, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  d->actions->menuCalcNone->setExclusiveGroup( "Calc" );
  d->actions->menuCalcNone->setToolTip(i18n("No calculation"));

  /*******************************/
}


void KSpreadView::initializeInsertActions()
{
  d->actions->insertFunction = new KAction( i18n("&Function..."), "funct", 0, this,
                                  SLOT( insertMathExpr() ), actionCollection(),
                                  "insertMathExpr" );
  d->actions->insertFunction->setToolTip(i18n("Insert math expression."));

  d->actions->insertSeries = new KAction( i18n("&Series..."),"series", 0, this,
                                SLOT( insertSeries() ), actionCollection(), "series");
  d->actions->insertSeries ->setToolTip(i18n("Insert a series."));

  d->actions->insertLink = new KAction( i18n("&Link..."), 0, this,
                              SLOT( insertHyperlink() ), actionCollection(),
                              "insertHyperlink" );
  d->actions->insertLink->setToolTip(i18n("Insert an Internet hyperlink."));

  d->actions->insertSpecialChar = new KAction( i18n( "S&pecial Character..." ), "char", this,
                                     SLOT( insertSpecialChar() ), actionCollection(),
                                     "insertSpecialChar" );
  d->actions->insertSpecialChar->setToolTip( i18n( "Insert one or more symbols or letters not found on the keyboard." ) );


  d->actions->insertPart=new KoPartSelectAction( i18n("&Object"), "frame_query", this,
                                       SLOT( insertObject() ),
                                       actionCollection(), "insertPart");
  d->actions->insertPart->setToolTip(i18n("Insert an object from another program."));

  d->actions->insertChartFrame=new KAction( i18n("&Chart"), "frame_chart", 0, this,
                                  SLOT( insertChart() ), actionCollection(),
                                  "insertChart" );
  d->actions->insertChartFrame->setToolTip(i18n("Insert a chart."));

#ifndef QT_NO_SQL
  d->actions->insertFromDatabase = new KAction( i18n("From &Database..."), 0, this,
                                      SLOT( insertFromDatabase() ),
                                      actionCollection(), "insertFromDatabase");
  d->actions->insertFromDatabase->setToolTip(i18n("Insert data from a SQL database."));
#endif

  d->actions->insertFromTextfile = new KAction( i18n("From &Text File..."), 0, this,
                                      SLOT( insertFromTextfile() ),
                                      actionCollection(), "insertFromTextfile");
  d->actions->insertFromTextfile->setToolTip(i18n("Insert data from a text file to the current cursor position/selection."));
  d->actions->insertFromClipboard = new KAction( i18n("From &Clipboard..."), 0, this,
                                      SLOT( insertFromClipboard() ),
                                      actionCollection(), "insertFromClipboard");
  d->actions->insertFromClipboard->setToolTip(i18n("Insert csv data from the clipboard to the current cursor position/selection."));

}

void KSpreadView::initializeEditActions()
{
  d->actions->copy = KStdAction::copy( this, SLOT( copySelection() ), actionCollection(),
                             "copy" );
  d->actions->copy->setToolTip(i18n("Copy the cell object to the clipboard."));

  d->actions->paste = KStdAction::paste( this, SLOT( paste() ), actionCollection(),
                               "paste" );
  d->actions->paste->setToolTip(i18n("Paste the contents of the clipboard at the cursor."));

  d->actions->cut = KStdAction::cut( this, SLOT( cutSelection() ), actionCollection(),
                           "cut" );
  d->actions->cut->setToolTip(i18n("Move the cell object to the clipboard."));

  d->actions->specialPaste = new KAction( i18n("Special Paste..."), "special_paste",0,
                                this, SLOT( specialPaste() ), actionCollection(),
                                "specialPaste" );
  d->actions->specialPaste->setToolTip
    (i18n("Paste the contents of the clipboard with special options."));

  d->actions->insertCellCopy = new KAction( i18n("Paste with Insertion"),
                                  "insertcellcopy", 0, this,
                                  SLOT( slotInsertCellCopy() ),
                                  actionCollection(), "insertCellCopy" );
  d->actions->insertCellCopy->setToolTip(i18n("Inserts a cell from the clipboard into the spreadsheet."));

  d->actions->undo = KStdAction::undo( this, SLOT( undo() ), actionCollection(), "undo" );
  d->actions->undo->setEnabled( FALSE );
  d->actions->undo->setToolTip(i18n("Undo the previous action."));

  d->actions->redo = KStdAction::redo( this, SLOT( redo() ), actionCollection(), "redo" );
  d->actions->redo->setEnabled( FALSE );
  d->actions->redo->setToolTip(i18n("Redo the action that has been undone."));

  d->actions->findAction = KStdAction::find(this, SLOT(find()), actionCollection());
  /*d->actions->findNext =*/ KStdAction::findNext( this, SLOT( findNext() ), actionCollection() );
  /*d->actions->findPrevious =*/ KStdAction::findPrev( this, SLOT( findPrevious() ), actionCollection() );

  d->actions->replaceAction = KStdAction::replace(this, SLOT(replace()), actionCollection());

  d->actions->fillRight = new KAction( i18n( "&Right" ), 0, 0, this,
                             SLOT( fillRight() ), actionCollection(), "fillRight" );
  d->actions->fillLeft = new KAction( i18n( "&Left" ), 0, 0, this,
                             SLOT( fillLeft() ), actionCollection(), "fillLeft" );
  d->actions->fillDown = new KAction( i18n( "&Down" ), 0, 0, this,
                             SLOT( fillDown() ), actionCollection(), "fillDown" );
  d->actions->fillUp = new KAction( i18n( "&Up" ), 0, 0, this,
                             SLOT( fillUp() ), actionCollection(), "fillUp" );
}

void KSpreadView::initializeAreaOperationActions()
{
  d->actions->areaName = new KAction( i18n("Area Name..."), 0, this,
                            SLOT( setAreaName() ), actionCollection(),
                            "areaname" );
  d->actions->areaName->setToolTip(i18n("Set a name for a region of the spreadsheet."));

  d->actions->showArea = new KAction( i18n("Show Area..."), 0, this,
                            SLOT( showAreaName() ), actionCollection(),
                            "showArea" );
  d->actions->showArea->setToolTip(i18n("Display a named area."));

  d->actions->sortList = new KAction( i18n("Custom Lists..."), 0, this,
                            SLOT( sortList() ), actionCollection(),
                            "sortlist" );
  d->actions->sortList->setToolTip(i18n("Create custom lists for sorting or autofill."));

  d->actions->sort = new KAction( i18n("&Sort..."), 0, this, SLOT( sort() ),
                        actionCollection(), "sort" );
  d->actions->sort->setToolTip(i18n("Sort a group of cells."));

  d->actions->autoSum = new KAction( i18n("Autosum"), "black_sum", 0, this,
                           SLOT( autoSum() ), actionCollection(), "autoSum" );
  d->actions->autoSum->setToolTip(i18n("Insert the 'sum' function"));

  d->actions->sortDec = new KAction( i18n("Sort &Decreasing"), "sort_decrease", 0, this,
                           SLOT( sortDec() ), actionCollection(), "sortDec" );
  d->actions->sortDec->setToolTip(i18n("Sort a group of cells in decreasing (last to first) order."));

  d->actions->sortInc = new KAction( i18n("Sort &Increasing"), "sort_incr", 0, this,
                           SLOT( sortInc() ), actionCollection(), "sortInc" );
  d->actions->sortInc->setToolTip(i18n("Sort a group of cells in ascending (first to last) order."));

  d->actions->goalSeek = new KAction( i18n("&Goal Seek..."), 0, this,
                            SLOT( goalSeek() ), actionCollection(), "goalSeek" );
  d->actions->goalSeek->setToolTip( i18n("Repeating calculation to find a specific value.") );

  d->actions->multipleOperations = new KAction( i18n("&Multiple Operations..."), 0, this,
                            SLOT( multipleOperations() ), actionCollection(), "multipleOperations" );
  d->actions->multipleOperations->setToolTip( i18n("Apply the same formula to various cells using different values for the parameter.") );

  d->actions->subTotals = new KAction( i18n("&Subtotals..."), 0, this,
                             SLOT( subtotals() ), actionCollection(), "subtotals" );
  d->actions->subTotals->setToolTip( i18n("Create different kind of subtotals to a list or database.") );

  d->actions->textToColumns = new KAction( i18n("&Text to Columns..."), 0, this,
                            SLOT( textToColumns() ), actionCollection(), "textToColumns" );
  d->actions->textToColumns->setToolTip( i18n("Expand the content of cells to multiple columns.") );

  d->actions->consolidate = new KAction( i18n("&Consolidate..."), 0, this,
                               SLOT( consolidate() ), actionCollection(),
                               "consolidate" );
  d->actions->consolidate->setToolTip(i18n("Create a region of summary data from a group of similar regions."));
}

void KSpreadView::initializeGlobalOperationActions()
{
  d->actions->recalc_workbook = new KAction( i18n("Recalculate Workbook"), Key_F9, this,
                                   SLOT( recalcWorkBook() ), actionCollection(),
                                   "RecalcWorkBook" );
  d->actions->recalc_workbook->setToolTip(i18n("Recalculate the value of every cell in all worksheets."));

  d->actions->recalc_worksheet = new KAction( i18n("Recalculate Sheet"), SHIFT + Key_F9,
                                    this, SLOT( recalcWorkSheet() ),
                                    actionCollection(), "RecalcWorkSheet" );
  d->actions->recalc_worksheet->setToolTip(i18n("Recalculate the value of every cell in the current worksheet."));

  d->actions->preference = new KAction( i18n("Configure KSpread..."),"configure", 0, this,
                              SLOT( preference() ), actionCollection(),
                              "preference" );
  d->actions->preference->setToolTip(i18n("Set various KSpread options."));

  d->actions->editGlobalScripts = new KAction( i18n("Edit Global Scripts..."), 0, this,
                                     SLOT( editGlobalScripts() ),
                                     actionCollection(), "editGlobalScripts" );
  d->actions->editGlobalScripts->setToolTip("");//i18n("")); /* TODO - what is this? */

  d->actions->editLocalScripts = new KAction( i18n("Edit Local Scripts..."), 0, this,
                                    SLOT( editLocalScripts() ),
                                    actionCollection(), "editLocalScripts" );
  d->actions->editLocalScripts->setToolTip("");//i18n("")); /* TODO - what is this? */

  d->actions->reloadScripts = new KAction( i18n("Reload Scripts"), 0, this,
                                 SLOT( reloadScripts() ), actionCollection(),
                                 "reloadScripts" );
  d->actions->reloadScripts->setToolTip("");//i18n("")); /* TODO - what is this? */

  d->actions->showPageBorders = new KToggleAction( i18n("Show Page Borders"), 0,
                                         actionCollection(), "showPageBorders");
  connect( d->actions->showPageBorders, SIGNAL( toggled( bool ) ), this,
           SLOT( togglePageBorders( bool ) ) );
  d->actions->showPageBorders->setToolTip( i18n( "Show on the spreadsheet where the page borders will be." ) );

  d->actions->protectSheet = new KToggleAction( i18n( "Protect &Sheet..." ), 0,
                                      actionCollection(), "protectSheet" );
  d->actions->protectSheet->setToolTip( i18n( "Protect the sheet from being modified." ) );
  connect( d->actions->protectSheet, SIGNAL( toggled( bool ) ), this,
           SLOT( toggleProtectSheet( bool ) ) );

  d->actions->protectDoc = new KToggleAction( i18n( "Protect &Doc..." ), 0,
                                      actionCollection(), "protectDoc" );
  d->actions->protectDoc->setToolTip( i18n( "Protect the document from being modified." ) );
  connect( d->actions->protectDoc, SIGNAL( toggled( bool ) ), this,
           SLOT( toggleProtectDoc( bool ) ) );

  d->actions->recordChanges = new KToggleAction( i18n( "&Record Changes" ), 0,
                                       actionCollection(), "recordChanges" );
  d->actions->recordChanges->setToolTip( i18n( "Record changes made to this document." ) );
  connect( d->actions->recordChanges, SIGNAL( toggled( bool ) ), this,
           SLOT( toggleRecordChanges( bool ) ) );


  d->actions->protectChanges = new KToggleAction( i18n( "&Protect Changes..." ), 0,
                                        actionCollection(), "protectRecords" );
  d->actions->protectChanges->setToolTip( i18n( "Protect the change records from being accepted or rejected." ) );
  d->actions->protectChanges->setEnabled( false );
  connect( d->actions->protectChanges, SIGNAL( toggled( bool ) ), this,
           SLOT( toggleProtectChanges( bool ) ) );

  d->actions->filterChanges = new KAction( i18n( "&Filter Changes..." ), 0, this,
                                 SLOT( filterChanges() ), actionCollection(),
                                 "filterChanges" );
  d->actions->filterChanges->setToolTip( i18n( "Change display settings for changes." ) );
  d->actions->filterChanges->setEnabled( false );

  d->actions->acceptRejectChanges = new KAction( i18n( "&Accept or Reject..." ), 0, this,
                                       SLOT( acceptRejectChanges() ), actionCollection(),
                                       "acceptRejectChanges" );
  d->actions->acceptRejectChanges->setToolTip( i18n( "Accept or reject changes made to this document." ) );
  d->actions->acceptRejectChanges->setEnabled( false );

  d->actions->commentChanges = new KAction( i18n( "&Comment Changes..." ), 0, this,
                                  SLOT( commentChanges() ), actionCollection(),
                                  "commentChanges" );
  d->actions->commentChanges->setToolTip( i18n( "Add comments to changes you made." ) );
  d->actions->commentChanges->setEnabled( false );

  d->actions->mergeDocument = new KAction( i18n( "&Merge Document..." ), 0, this,
                                 SLOT( mergeDocument() ), actionCollection(),
                                 "mergeDocument" );
  d->actions->mergeDocument->setToolTip( i18n( "Merge this document with a document that recorded changes." ) );

  d->actions->viewZoom = new KSelectAction( i18n( "Zoom" ), "viewmag", 0,
                                  actionCollection(), "view_zoom" );

  connect( d->actions->viewZoom, SIGNAL( activated( const QString & ) ),
           this, SLOT( viewZoom( const QString & ) ) );
  d->actions->viewZoom->setEditable(true);
  changeZoomMenu( m_pDoc->zoom() );

  d->actions->formulaSelection = new KSelectAction(i18n("Formula Selection"), 0,
                                         actionCollection(), "formulaSelection");
  d->actions->formulaSelection->setToolTip(i18n("Insert a function."));
  QStringList lst;
  lst.append( "SUM");
  lst.append( "AVERAGE");
  lst.append( "IF");
  lst.append( "COUNT");
  lst.append( "MIN");
  lst.append( "MAX");
  lst.append( i18n("Others...") );
  ((KSelectAction*) d->actions->formulaSelection)->setItems( lst );
  d->actions->formulaSelection->setComboWidth( 80 );
  d->actions->formulaSelection->setCurrentItem(0);
  connect( d->actions->formulaSelection, SIGNAL( activated( const QString& ) ),
           this, SLOT( formulaSelection( const QString& ) ) );


  d->actions->transform = new KAction( i18n("Transform Object..."), "rotate", 0, this,
                             SLOT( transformPart() ),
                             actionCollection(), "transform" );
  d->actions->transform->setToolTip(i18n("Rotate the contents of the cell."));



  d->actions->transform->setEnabled( FALSE );
  connect( d->actions->transform, SIGNAL( activated() ), this, SLOT( transformPart() ) );


  d->actions->paperLayout = new KAction( i18n("Page Layout..."), 0, this,
                               SLOT( paperLayoutDlg() ), actionCollection(),
                               "paperLayout" );
  d->actions->paperLayout->setToolTip(i18n("Specify the layout of the spreadsheet for a printout."));

  d->actions->definePrintRange = new KAction( i18n("Define Print Range"), 0, this,
                                    SLOT( definePrintRange() ), actionCollection(),
                                    "definePrintRange" );
  d->actions->definePrintRange->setToolTip(i18n("Define the print range in the current sheet."));

  d->actions->resetPrintRange = new KAction( i18n("Reset Print Range"), 0, this,
                                   SLOT( resetPrintRange() ), actionCollection(),
                                   "resetPrintRange" );
  d->actions->definePrintRange->setToolTip(i18n("Define the print range in the current sheet."));

  d->actions->createTemplate = new KAction( i18n( "&Create Template From Document..." ), 0, this,
                                  SLOT( createTemplate() ), actionCollection(), "createTemplate" );

  d->actions->styleDialog = new KAction( i18n( "Style Manager..." ), 0, this, SLOT( styleDialog() ),
                               actionCollection(), "styles" );
  d->actions->styleDialog->setToolTip( i18n( "Edit and organize cell styles." ) );

  d->actions->selectStyle = new KSelectAction( i18n( "St&yle" ), 0,
                                     actionCollection(), "stylemenu" );
  d->actions->selectStyle->setToolTip( i18n( "Apply a predefined style to the selected cells." ) );
  connect( d->actions->selectStyle, SIGNAL( activated( const QString & ) ), this, SLOT( styleSelected( const QString & ) ) );

  d->actions->createStyle = new KAction( i18n( "Create Style From Cell..." ), 0,
                               this, SLOT( createStyleFromCell()), actionCollection(), "createStyle" );
  d->actions->createStyle->setToolTip( i18n( "Create a new style based on the currently selected cell." ) );
}


void KSpreadView::initializeCellOperationActions()
{
  d->actions->editCell = new KAction( i18n("Modify Cell"),"cell_edit", CTRL + Key_M, this,
                            SLOT( editCell() ), actionCollection(), "editCell" );
  d->actions->editCell->setToolTip(i18n("Edit the highlighted cell."));

  d->actions->del = new KAction( i18n("Delete"),"deletecell", 0, this,

			  SLOT( deleteSelection() ), actionCollection(),
                          "delete" );
  d->actions->del->setToolTip(i18n("Delete all contents and formatting of the current cell."));

  d->actions->clearText = new KAction( i18n("Text"), 0, this, SLOT( clearTextSelection() ),
                             actionCollection(), "cleartext" );
  d->actions->clearText->setToolTip(i18n("Remove the contents of the current cell."));

  d->actions->gotoCell = new KAction( i18n("Goto Cell..."),"goto", 0, this,
                            SLOT( gotoCell() ), actionCollection(), "gotoCell" );
  d->actions->gotoCell->setToolTip(i18n("Move to a particular cell."));

  d->actions->mergeCell = new KAction( i18n("Merge Cells"),"mergecell" ,0, this,
                             SLOT( mergeCell() ), actionCollection(),
                             "mergecell" );
  d->actions->mergeCell->setToolTip(i18n("Merge the selected region into one large cell."));

  d->actions->dissociateCell = new KAction( i18n("Dissociate Cells"),"dissociatecell" ,0,
                                  this, SLOT( dissociateCell() ),
                                  actionCollection(), "dissociatecell" );
  d->actions->dissociateCell->setToolTip(i18n("Unmerge the current cell."));

  d->actions->removeCell = new KAction( i18n("Remove Cells..."), "removecell", 0, this,
                              SLOT( slotRemove() ), actionCollection(),
                              "removeCell" );
  d->actions->removeCell->setToolTip(i18n("Removes the current cell from the spreadsheet."));

  d->actions->insertCell = new KAction( i18n("Insert Cells..."), "insertcell", 0, this,
                              SLOT( slotInsert() ), actionCollection(),
                              "insertCell" );
  d->actions->insertCell->setToolTip(i18n("Insert a blank cell into the spreadsheet."));

}

void KSpreadView::initializeCellPropertyActions()
{
  d->actions->addModifyComment = new KAction( i18n("&Add/Modify Comment..."),"comment", 0,
                                    this, SLOT( addModifyComment() ),
                                    actionCollection(), "addmodifycomment" );
  d->actions->addModifyComment->setToolTip(i18n("Edit a comment for this cell."));

  d->actions->removeComment = new KAction( i18n("&Remove Comment"),"removecomment", 0,
                                 this, SLOT( removeComment() ),
                                 actionCollection(), "removecomment" );
  d->actions->removeComment->setToolTip(i18n("Remove this cell's comment."));

  d->actions->conditional = new KAction( i18n("Conditional Cell Attributes..."), 0, this,
                               SLOT( conditional() ), actionCollection(),
                               "conditional" );
  d->actions->conditional->setToolTip(i18n("Set cell format based on certain conditions."));

  d->actions->validity = new KAction( i18n("Validity..."), 0, this, SLOT( validity() ),
                            actionCollection(), "validity" );
  d->actions->validity->setToolTip(i18n("Set tests to confirm cell data is valid."));

  d->actions->clearComment = new KAction( i18n("Comment"), 0, this,
                                SLOT( clearCommentSelection() ),
                                actionCollection(), "clearcomment" );
  d->actions->clearComment->setToolTip(i18n("Remove this cell's comment."));

  d->actions->clearValidity = new KAction( i18n("Validity"), 0, this,
                                 SLOT( clearValiditySelection() ),
                                 actionCollection(), "clearvalidity" );
  d->actions->clearValidity->setToolTip(i18n("Remove the validity tests on this cell."));

  d->actions->clearConditional = new KAction( i18n("Conditional Cell Attributes"), 0, this,
                                    SLOT( clearConditionalSelection() ),
                                    actionCollection(), "clearconditional" );
  d->actions->clearConditional->setToolTip(i18n("Remove the conditional cell formatting."));

  d->actions->increaseIndent = new KAction( i18n("Increase Indent"),
                                  QApplication::reverseLayout() ? "format_decreaseindent":"format_increaseindent",0, this,
                                  SLOT( increaseIndent() ), actionCollection(),
                                  "increaseindent" );
  d->actions->increaseIndent->setToolTip(i18n("Increase the indentation."));

  d->actions->decreaseIndent = new KAction( i18n("Decrease Indent"),
                                  QApplication::reverseLayout() ? "format_increaseindent" : "format_decreaseindent" ,0, this,
                                  SLOT( decreaseIndent() ), actionCollection(),
                                  "decreaseindent");
  d->actions->decreaseIndent->setToolTip(i18n("Decrease the indentation."));

  d->actions->multiRow = new KToggleAction( i18n("Multi Row"), "multirow", 0,
                                  actionCollection(), "multiRow" );
  connect( d->actions->multiRow, SIGNAL( toggled( bool ) ), this,
           SLOT( multiRow( bool ) ) );
  d->actions->multiRow->setToolTip(i18n("Make the cell text wrap onto multiple lines."));

  d->actions->bgColor = new TKSelectColorAction( i18n("Background Color"),
                                       TKSelectColorAction::FillColor,
                                       actionCollection(), "backgroundColor",
                                       true );
  connect(d->actions->bgColor,SIGNAL(activated()),SLOT(changeBackgroundColor()));
  d->actions->bgColor->setDefaultColor(QColor());
  d->actions->bgColor->setToolTip(i18n("Set the background color."));

}


void KSpreadView::initializeTextFormatActions()
{
  /*******************************/
  d->actions->percent = new KToggleAction( i18n("Percent Format"), "percent", 0,
                                 actionCollection(), "percent");
  connect( d->actions->percent, SIGNAL( toggled( bool ) ), this, SLOT( percent( bool ) ) );
  d->actions->percent->setToolTip(i18n("Set the cell formatting to look like a percentage."));

  /*******************************/
  d->actions->precplus = new KAction( i18n("Increase Precision"), "prec_plus", 0, this,
                            SLOT( precisionPlus() ), actionCollection(),
                            "precplus");
  d->actions->precplus->setToolTip(i18n("Increase the decimal precision shown onscreen."));

  /*******************************/
  d->actions->precminus = new KAction( i18n("Decrease Precision"), "prec_minus", 0, this,
                             SLOT( precisionMinus() ), actionCollection(),
                             "precminus");
  d->actions->precminus->setToolTip(i18n("Decrease the decimal precision shown onscreen."));

  /*******************************/
  d->actions->money = new KToggleAction( i18n("Money Format"), "money", 0,
                               actionCollection(), "money");
  connect( d->actions->money, SIGNAL( toggled( bool ) ), this,
           SLOT( moneyFormat( bool ) ) );
  d->actions->money->setToolTip(i18n("Set the cell formatting to look like your local currency."));

  /*******************************/
  d->actions->upper = new KAction( i18n("Upper Case"), "fontsizeup", 0, this,
                         SLOT( upper() ), actionCollection(), "upper" );
  d->actions->upper->setToolTip(i18n("Convert all letters to upper case."));

  /*******************************/
  d->actions->lower = new KAction( i18n("Lower Case"), "fontsizedown", 0, this,
                         SLOT( lower() ), actionCollection(), "lower" );
  d->actions->lower->setToolTip(i18n("Convert all letters to lower case."));

  /*******************************/
  d->actions->firstLetterUpper = new KAction( i18n("Convert First Letter to Upper Case"),
                                    "first_letter_upper" ,0, this,
                                    SLOT( firstLetterUpper() ),
                                    actionCollection(), "firstletterupper" );
  d->actions->firstLetterUpper->setToolTip(i18n("Capitalize the first letter."));
}

void KSpreadView::initializeTextLayoutActions()
{
  /*******************************/
  d->actions->alignLeft = new KToggleAction( i18n("Align Left"), "text_left", 0,
                                   actionCollection(), "left");
  connect( d->actions->alignLeft, SIGNAL( toggled( bool ) ), this,
           SLOT( alignLeft( bool ) ) );
  d->actions->alignLeft->setExclusiveGroup( "Align" );
  d->actions->alignLeft->setToolTip(i18n("Left justify the cell contents."));

  /*******************************/
  d->actions->alignCenter = new KToggleAction( i18n("Align Center"), "text_center", 0,
                                     actionCollection(), "center");
  connect( d->actions->alignCenter, SIGNAL( toggled( bool ) ), this,
           SLOT( alignCenter( bool ) ) );
  d->actions->alignCenter->setExclusiveGroup( "Align" );
  d->actions->alignCenter->setToolTip(i18n("Center the cell contents."));

  /*******************************/
  d->actions->alignRight = new KToggleAction( i18n("Align Right"), "text_right", 0,
                                    actionCollection(), "right");
  connect( d->actions->alignRight, SIGNAL( toggled( bool ) ), this,
           SLOT( alignRight( bool ) ) );
  d->actions->alignRight->setExclusiveGroup( "Align" );
  d->actions->alignRight->setToolTip(i18n("Right justify the cell contents."));

  /*******************************/
  d->actions->alignTop = new KToggleAction( i18n("Align Top"), "text_top", 0,
                                  actionCollection(), "top");
  connect( d->actions->alignTop, SIGNAL( toggled( bool ) ), this,
           SLOT( alignTop( bool ) ) );
  d->actions->alignTop->setExclusiveGroup( "Pos" );
  d->actions->alignTop->setToolTip(i18n("Align cell contents along the top of the cell."));

  /*******************************/
  d->actions->alignMiddle = new KToggleAction( i18n("Align Middle"), "middle", 0,
                                     actionCollection(), "middle");
  connect( d->actions->alignMiddle, SIGNAL( toggled( bool ) ), this,
           SLOT( alignMiddle( bool ) ) );
  d->actions->alignMiddle->setExclusiveGroup( "Pos" );
  d->actions->alignMiddle->setToolTip(i18n("Align cell contents centered in the cell."));

  /*******************************/
  d->actions->alignBottom = new KToggleAction( i18n("Align Bottom"), "text_bottom", 0,
                                     actionCollection(), "bottom");
  connect( d->actions->alignBottom, SIGNAL( toggled( bool ) ), this,
           SLOT( alignBottom( bool ) ) );
  d->actions->alignBottom->setExclusiveGroup( "Pos" );
  d->actions->alignBottom->setToolTip(i18n("Align cell contents along the bottom of the cell."));

  /*******************************/
  d->actions->verticalText = new KToggleAction( i18n("Vertical Text"),"vertical_text" ,
                                      0 ,actionCollection(), "verticaltext" );
  connect( d->actions->verticalText, SIGNAL( toggled( bool ) ), this,
           SLOT( verticalText( bool ) ) );
  d->actions->verticalText->setToolTip(i18n("Print cell contents vertically."));

  /*******************************/
  d->actions->changeAngle = new KAction( i18n("Change Angle..."), 0, this,
                               SLOT( changeAngle() ), actionCollection(),
                               "changeangle" );
  d->actions->changeAngle->setToolTip(i18n("Change the angle that cell contents are printed."));
}

void KSpreadView::initializeTextPropertyActions()
{
  /*******************************/
  d->actions->bold = new KToggleAction( i18n("Bold"), "text_bold", CTRL + Key_B,
                              actionCollection(), "bold");
  connect( d->actions->bold, SIGNAL( toggled( bool ) ), this, SLOT( bold( bool ) ) );

  /*******************************/
  d->actions->italic = new KToggleAction( i18n("Italic"), "text_italic", CTRL + Key_I,
                                actionCollection(), "italic");
  connect( d->actions->italic, SIGNAL( toggled( bool ) ), this, SLOT( italic( bool ) ) );

  /*******************************/
  d->actions->underline = new KToggleAction( i18n("Underline"), "text_under",
                                   CTRL + Key_U, actionCollection(),
                                   "underline");
  connect( d->actions->underline, SIGNAL( toggled( bool ) ), this,
           SLOT( underline( bool ) ) );

  /*******************************/
  d->actions->strikeOut = new KToggleAction( i18n("Strike Out"), "text_strike", 0,
                                   actionCollection(), "strikeout");
  connect( d->actions->strikeOut, SIGNAL( toggled( bool ) ), this,
           SLOT( strikeOut( bool ) ) );

  /*******************************/
  d->actions->selectFont = new KFontAction( i18n("Select Font..."), 0, actionCollection(),
                                  "selectFont" );
  connect( d->actions->selectFont, SIGNAL( activated( const QString& ) ), this,
           SLOT( fontSelected( const QString& ) ) );

  /*******************************/
  d->actions->selectFontSize = new KFontSizeAction( i18n("Select Font Size"), 0,
                                          actionCollection(), "selectFontSize" );
  connect( d->actions->selectFontSize, SIGNAL( fontSizeChanged( int ) ), this,
           SLOT( fontSizeSelected( int ) ) );

  /*******************************/
  d->actions->fontSizeUp = new KAction( i18n("Increase Font Size"), "fontsizeup", 0, this,
                              SLOT( increaseFontSize() ), actionCollection(),
                              "increaseFontSize" );

  /*******************************/
  d->actions->fontSizeDown = new KAction( i18n("Decrease Font Size"), "fontsizedown", 0,
                                this, SLOT( decreaseFontSize() ),
                                actionCollection(), "decreaseFontSize" );

  /*******************************/
  d->actions->textColor = new TKSelectColorAction( i18n("Text Color"),
                                         TKSelectColorAction::TextColor,
                                         actionCollection(), "textColor",true );
  connect( d->actions->textColor, SIGNAL(activated()), SLOT(changeTextColor()) );
  d->actions->textColor->setDefaultColor(QColor());

  /*******************************/
}

void KSpreadView::initializeTableActions()
{
  d->actions->insertTable = new KAction( i18n("Insert Sheet"),"inserttable", 0, this,
                               SLOT( insertTable() ), actionCollection(),
                               "insertTable" );
  d->actions->insertTable->setToolTip(i18n("Insert a new sheet."));

  /* basically the same action here, but it's in the insert menu so we don't
     want to also have 'insert' in the caption
  */
  d->actions->menuInsertTable = new KAction( i18n("&Sheet"),"inserttable", 0, this,
                               SLOT( insertTable() ), actionCollection(),
                               "menuInsertTable" );
  d->actions->menuInsertTable->setToolTip(i18n("Insert a new sheet."));

  d->actions->removeTable = new KAction( i18n("Remove Sheet"), "delete_table",0,this,
                               SLOT( removeTable() ), actionCollection(),
                               "removeTable" );
  d->actions->removeTable->setToolTip(i18n("Remove the active sheet."));

  d->actions->renameTable=new KAction( i18n("Rename Sheet..."),0,this,
                             SLOT( slotRename() ), actionCollection(),
                             "renameTable" );
  d->actions->renameTable->setToolTip(i18n("Rename the active sheet."));

  d->actions->nextTable = new KAction( i18n("Next Sheet"), CTRL + Key_PageDown, this,
                             SLOT( nextTable() ), actionCollection(),
                             "nextTable");
  d->actions->nextTable->setToolTip(i18n("Move to the next sheet."));

  d->actions->prevTable = new KAction( i18n("Previous Sheet"), CTRL + Key_PageUp, this,
                             SLOT( previousTable() ), actionCollection(),
                             "previousTable");
  d->actions->prevTable->setToolTip(i18n("Move to the previous sheet."));

  d->actions->firstTable = new KAction( i18n("First Sheet"), 0, this,
                              SLOT( firstTable() ), actionCollection(),
                              "firstTable");
  d->actions->firstTable->setToolTip(i18n("Move to the first sheet."));

  d->actions->lastTable = new KAction( i18n("Last Sheet"), 0, this,
                             SLOT( lastTable() ), actionCollection(),
                             "lastTable");
  d->actions->lastTable->setToolTip(i18n("Move to the last sheet."));

  d->actions->showTable = new KAction(i18n("Show Sheet..."),0 ,this,SLOT( showTable()),
                            actionCollection(), "showTable" );
  d->actions->showTable->setToolTip(i18n("Show a hidden sheet."));

  d->actions->hideTable = new KAction(i18n("Hide Sheet"),0 ,this,SLOT( hideTable()),
                            actionCollection(), "hideTable" );
  d->actions->hideTable->setToolTip(i18n("Hide the active sheet."));

  d->actions->tableFormat = new KAction( i18n("AutoFormat..."), 0, this,
                               SLOT( tableFormat() ), actionCollection(),
                               "tableFormat" );
  d->actions->tableFormat->setToolTip(i18n("Set the worksheet formatting."));
}

void KSpreadView::initializeSpellChecking()
{
  d->actions->spellChecking = KStdAction::spelling( this, SLOT( extraSpelling() ),
                                          actionCollection(), "spelling" );
  d->actions->spellChecking->setToolTip(i18n("Check the spelling."));
}


void KSpreadView::initializeRowColumnActions()
{
  d->actions->adjust = new KAction( i18n("Adjust Row && Column"), 0, this,
                          SLOT( adjust() ), actionCollection(), "adjust" );
  d->actions->adjust->setToolTip(i18n("Adjusts row/column size so that the contents will fit."));

  d->actions->resizeRow = new KAction( i18n("Resize Row..."), "resizerow", 0, this,
                             SLOT( resizeRow() ), actionCollection(),
                             "resizeRow" );
  d->actions->resizeRow->setToolTip(i18n("Change the height of a row."));

  d->actions->resizeColumn = new KAction( i18n("Resize Column..."), "resizecol", 0, this,
                                SLOT( resizeColumn() ), actionCollection(),
                                "resizeCol" );
  d->actions->resizeColumn->setToolTip(i18n("Change the width of a column."));

  d->actions->equalizeRow = new KAction( i18n("Equalize Row"), "adjustrow", 0, this,
                               SLOT( equalizeRow() ), actionCollection(),
                               "equalizeRow" );
  d->actions->equalizeRow->setToolTip(i18n("Resizes selected rows to be the same size."));

  d->actions->equalizeColumn = new KAction( i18n("Equalize Column"), "adjustcol", 0, this,
                                  SLOT( equalizeColumn() ), actionCollection(),
                                  "equalizeCol" );
  d->actions->equalizeColumn->setToolTip(i18n("Resizes selected columns to be the same size."));

  d->actions->deleteColumn = new KAction( i18n("Delete Columns"), "delete_table_col", 0,
                                this, SLOT( deleteColumn() ),
                                actionCollection(), "deleteColumn" );
  d->actions->deleteColumn->setToolTip(i18n("Removes a column from the spreadsheet."));

  d->actions->deleteRow = new KAction( i18n("Delete Rows"), "delete_table_row", 0, this,
                             SLOT( deleteRow() ), actionCollection(),
                             "deleteRow" );
  d->actions->deleteRow->setToolTip(i18n("Removes a row from the spreadsheet."));

  d->actions->insertColumn = new KAction( i18n("Insert Columns"), "insert_table_col" ,
                                0, this, SLOT( insertColumn() ),
                                actionCollection(), "insertColumn" );
  d->actions->insertColumn->setToolTip(i18n("Inserts a new column into the spreadsheet."));

  d->actions->insertRow = new KAction( i18n("Insert Rows"), "insert_table_row", 0, this,
                             SLOT( insertRow() ), actionCollection(),
                             "insertRow" );
  d->actions->insertRow->setToolTip(i18n("Inserts a new row into the spreadsheet."));

  d->actions->hideRow = new KAction( i18n("Hide Rows"), "hide_table_row", 0, this,
                           SLOT( hideRow() ), actionCollection(), "hideRow" );
  d->actions->hideRow->setToolTip(i18n("Hide a row from view."));

  d->actions->showRow = new KAction( i18n("Show Rows..."), "show_table_row", 0, this,
                           SLOT( showRow() ), actionCollection(), "showRow" );
  d->actions->showRow->setToolTip(i18n("Show hidden rows."));

  d->actions->showSelRows = new KAction( i18n("Show Rows"), "show_table_row", 0, this,
                               SLOT( showSelRows() ), actionCollection(),
                               "showSelRows" );
  d->actions->showSelRows->setEnabled(false);
  d->actions->showSelRows->setToolTip(i18n("Show hidden rows in the selection."));

  d->actions->hideColumn = new KAction( i18n("Hide Columns"), "hide_table_column", 0,
                              this, SLOT( hideColumn() ), actionCollection(),
                              "hideColumn" );
  d->actions->hideColumn->setToolTip(i18n("Hide the column from view."));

  d->actions->showColumn = new KAction( i18n("Show Columns..."), "show_table_column", 0,
                              this, SLOT( showColumn() ), actionCollection(),
                              "showColumn" );
  d->actions->showColumn->setToolTip(i18n("Show hidden columns."));

  d->actions->showSelColumns = new KAction( i18n("Show Columns"), "show_table_column",
                                  0, this, SLOT( showSelColumns() ),
                                  actionCollection(), "showSelColumns" );
  d->actions->showSelColumns->setToolTip(i18n("Show hidden columns in the selection."));
  d->actions->showSelColumns->setEnabled(false);

}


void KSpreadView::initializeBorderActions()
{
  d->actions->borderLeft = new KAction( i18n("Border Left"), "border_left", 0, this,
                              SLOT( borderLeft() ), actionCollection(),
                              "borderLeft" );
  d->actions->borderLeft->setToolTip(i18n("Set a left border to the selected area."));

  d->actions->borderRight = new KAction( i18n("Border Right"), "border_right", 0, this,
                               SLOT( borderRight() ), actionCollection(),
                               "borderRight" );
  d->actions->borderRight->setToolTip(i18n("Set a right border to the selected area."));

  d->actions->borderTop = new KAction( i18n("Border Top"), "border_top", 0, this,
                             SLOT( borderTop() ), actionCollection(),
                             "borderTop" );
  d->actions->borderTop->setToolTip(i18n("Set a top border to the selected area."));

  d->actions->borderBottom = new KAction( i18n("Border Bottom"), "border_bottom", 0, this,
                                SLOT( borderBottom() ), actionCollection(),
                                "borderBottom" );
  d->actions->borderBottom->setToolTip(i18n("Set a bottom border to the selected area."));

  d->actions->borderAll = new KAction( i18n("All Borders"), "border_all", 0, this,
                             SLOT( borderAll() ), actionCollection(),
                             "borderAll" );
  d->actions->borderAll->setToolTip(i18n("Set a border around all cells in the selected area."));

  d->actions->borderRemove = new KAction( i18n("Remove Borders"), "border_remove", 0,
                                this, SLOT( borderRemove() ), actionCollection(),
                                "borderRemove" );
  d->actions->borderRemove->setToolTip(i18n("Remove all borders in the selected area."));

  d->actions->borderOutline = new KAction( i18n("Border Outline"), ("border_outline"), 0,
                                 this, SLOT( borderOutline() ),
                                 actionCollection(), "borderOutline" );
  d->actions->borderOutline->setToolTip(i18n("Set a border to the outline of the selected area."));

  d->actions->borderColor = new TKSelectColorAction( i18n("Border Color"),
                                           TKSelectColorAction::LineColor,
                                           actionCollection(), "borderColor" );

  connect( d->actions->borderColor, SIGNAL(activated()), SLOT(changeBorderColor()) );
  d->actions->borderColor->setToolTip( i18n( "Select a new border color." ) );

}

KSpreadView::~KSpreadView()
{
    //  ElapsedTime el( "~KSpreadView" );
    if ( m_pDoc->isReadWrite() ) // make sure we're not embedded in Konq
        deleteEditor( true );
    if ( !m_transformToolBox.isNull() )
        delete (&*m_transformToolBox);
    /*if (m_sbCalcLabel)
    {
        disconnect(m_sbCalcLabel,SIGNAL(pressed( int )),this,SLOT(statusBarClicked(int)));

        }*/

    delete m_selectionInfo;
    delete m_spell.kspell;

    m_pCanvas->endChoose();
    m_pTable = 0; // set the active table to 0L so that when during destruction
    // of embedded child documents possible repaints in KSpreadSheet are not
    // performed. The repains can happen if you delete an embedded document,
    // which leads to an regionInvalidated() signal emission in KoView, which calls
    // repaint, etc.etc. :-) (Simon)

    delete m_pPopupColumn;
    delete m_pPopupRow;
    delete m_pPopupMenu;
    delete m_popupChild;
    delete m_popupListChoose;
    delete m_sbCalcLabel;
    delete m_dcop;

    delete m_pInsertHandle;
    m_pInsertHandle = 0L;

    delete d->actions;
    delete d;
}


void KSpreadView::initConfig()
{
    KConfig *config = KSpreadFactory::global()->config();
    if ( config->hasGroup("Parameters" ))
    {
        config->setGroup( "Parameters" );
        m_pDoc->setShowHorizontalScrollBar(config->readBoolEntry("Horiz ScrollBar",true));
        m_pDoc->setShowVerticalScrollBar(config->readBoolEntry("Vert ScrollBar",true));
        m_pDoc->setShowColHeader(config->readBoolEntry("Column Header",true));
        m_pDoc->setShowRowHeader(config->readBoolEntry("Row Header",true));
        m_pDoc->setCompletionMode((KGlobalSettings::Completion)config->readNumEntry("Completion Mode",(int)(KGlobalSettings::CompletionAuto)));
        m_pDoc->setMoveToValue((KSpread::MoveTo)config->readNumEntry("Move",(int)(KSpread::Bottom)));
        m_pDoc->setIndentValue( config->readDoubleNumEntry( "Indent", 10.0 ) );
        m_pDoc->setTypeOfCalc((MethodOfCalc)config->readNumEntry("Method of Calc",(int)(SumOfNumber)));
	m_pDoc->setShowTabBar(config->readBoolEntry("Tabbar",true));

	m_pDoc->setShowMessageError(config->readBoolEntry( "Msg error" ,false) );

	m_pDoc->setShowCommentIndicator(config->readBoolEntry("Comment Indicator",true));

	m_pDoc->setShowFormulaBar(config->readBoolEntry("Formula bar",true));
        m_pDoc->setShowStatusBar(config->readBoolEntry("Status bar",true));

        changeNbOfRecentFiles(config->readNumEntry("NbRecentFile",10));
        //autosave value is stored as a minute.
        //but default value is stored as seconde.
        m_pDoc->setAutoSave(config->readNumEntry("AutoSave",KoDocument::defaultAutoSave()/60)*60);
        m_pDoc->setBackupFile( config->readBoolEntry("BackupFile",true));
    }

   if (  config->hasGroup("KSpread Color" ) )
   {
     config->setGroup( "KSpread Color" );
     QColor _col(Qt::lightGray);
     _col = config->readColorEntry("GridColor", &_col);
     m_pDoc->changeDefaultGridPenColor(_col);

     QColor _pbCol(Qt::red);
     _pbCol = config->readColorEntry("PageBorderColor", &_pbCol);
     m_pDoc->changePageBorderColor(_pbCol);
   }

// Do we need a Page Layout in the congiguration file? Isn't this already in the template? Philipp
/*
if ( config->hasGroup("KSpread Page Layout" ) )
 {
   config->setGroup( "KSpread Page Layout" );
   if ( m_pTable->isEmpty())
     {
	m_pTable->setPaperFormat((KoFormat)config->readNumEntry("Default size page",1));

	m_pTable->setPaperOrientation((KoOrientation)config->readNumEntry("Default orientation page",0));
	m_pTable->setPaperUnit((KoUnit::Unit)config->readNumEntry("Default unit page",0));
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
            d->actions->menuCalcSum->setChecked(true);
            break;
        case  Min:
            d->actions->menuCalcMin->setChecked(true);
            break;
        case  Max:
            d->actions->menuCalcMax->setChecked(true);
            break;
        case  Average:
            d->actions->menuCalcAverage->setChecked(true);
            break;
        case  Count:
            d->actions->menuCalcCount->setChecked(true);
            break;
        case  NoneCalc:
            d->actions->menuCalcNone->setChecked(true);
            break;
        default :
            d->actions->menuCalcSum->setChecked(true);
            break;
    }

}


void KSpreadView::recalcWorkBook()
{
  KSpreadSheet * tbl;
  m_pDoc->emitBeginOperation( true );
  for ( tbl = m_pDoc->map()->firstTable();
        tbl != 0L;
        tbl = m_pDoc->map()->nextTable() )
  {
    bool b = tbl->getAutoCalc();
    tbl->setAutoCalc( true );
    tbl->recalc();
    tbl->setAutoCalc( b );
  }

  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::refreshLocale()
{
  m_pDoc->emitBeginOperation(true);
  KSpreadSheet *tbl;
  for ( tbl = m_pDoc->map()->firstTable();
        tbl != 0L;
        tbl = m_pDoc->map()->nextTable() )
  {
    tbl->updateLocale();
  }
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::recalcWorkSheet()
{
  m_pDoc->emitBeginOperation( true );
  if ( m_pTable != 0 )
  {
    bool b = m_pTable->getAutoCalc();
    m_pTable->setAutoCalc( true );
    m_pTable->recalc();
    m_pTable->setAutoCalc( b );
  }
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}


void KSpreadView::extraSpelling()
{
  if ( m_spell.kspell )
    return; // Already in progress

  if (m_pTable == 0L)
    return;

  m_spell.macroCmdSpellCheck = 0L;
  m_spell.firstSpellTable    = m_pTable;
  m_spell.currentSpellTable  = m_spell.firstSpellTable;

  QRect selection = m_selectionInfo->selection();

  // if nothing is selected, check every cell
  if (m_selectionInfo->singleCellSelection())
  {
    m_spell.spellStartCellX = 0;
    m_spell.spellStartCellY = 0;
    m_spell.spellEndCellX   = 0;
    m_spell.spellEndCellY   = 0;
    m_spell.spellCheckSelection = false;
    m_spell.currentCell     = m_pTable->firstCell();
  }
  else
  {
    m_spell.spellStartCellX = selection.left();
    m_spell.spellStartCellY = selection.top();
    m_spell.spellEndCellX   = selection.right();
    m_spell.spellEndCellY   = selection.bottom();
    m_spell.spellCheckSelection = true;
    m_spell.currentCell     = 0L;

    // "-1" because X gets increased every time we go into spellCheckReady()
    m_spell.spellCurrCellX = m_spell.spellStartCellX - 1;
    m_spell.spellCurrCellY = m_spell.spellStartCellY;
  }

  startKSpell();
}


void KSpreadView::startKSpell()
{
    if ( m_pDoc->getKSpellConfig() )
    {
        m_pDoc->getKSpellConfig()->setIgnoreList( m_pDoc->spellListIgnoreAll() );
        m_pDoc->getKSpellConfig()->setReplaceAllList( m_spell.replaceAll );

    }
    m_spell.kspell = new KSpreadSpell( this, i18n( "Spell Checking" ), this,
                                       SLOT( spellCheckerReady() ),
                                       m_pDoc->getKSpellConfig() );

  m_spell.kspell->setIgnoreUpperWords( m_pDoc->dontCheckUpperWord() );
  m_spell.kspell->setIgnoreTitleCase( m_pDoc->dontCheckTitleCase() );

  QObject::connect( m_spell.kspell, SIGNAL( death() ),
                    this, SLOT( spellCheckerFinished() ) );
  QObject::connect( m_spell.kspell, SIGNAL( misspelling( const QString &,
                                                         const QStringList &,
                                                         unsigned int) ),
                    this, SLOT( spellCheckerMisspelling( const QString &,
                                                         const QStringList &,
                                                         unsigned int) ) );
  QObject::connect( m_spell.kspell, SIGNAL( corrected( const QString &,
                                                       const QString &,
                                                       unsigned int) ),
                    this, SLOT( spellCheckerCorrected( const QString &,
                                                       const QString &,
                                                       unsigned int ) ) );
  QObject::connect( m_spell.kspell, SIGNAL( done( const QString & ) ),
                    this, SLOT( spellCheckerDone( const QString & ) ) );
  QObject::connect( m_spell.kspell, SIGNAL( ignoreall (const QString & ) ),
                    this, SLOT( spellCheckerIgnoreAll( const QString & ) ) );

  QObject::connect( m_spell.kspell, SIGNAL( replaceall( const QString &  ,  const QString & )), this, SLOT( spellCheckerReplaceAll( const QString &  ,  const QString & )));

}

void KSpreadView::spellCheckerReplaceAll( const QString &orig, const QString & replacement)
{
    m_spell.replaceAll.append( orig);
    m_spell.replaceAll.append( replacement);
}


void KSpreadView::spellCheckerIgnoreAll( const QString & word)
{
    m_pDoc->addIgnoreWordAll( word );
}


void KSpreadView::spellCheckerReady()
{
  if (m_pCanvas)
    m_pCanvas->setCursor( WaitCursor );

  // go on to the next cell
  if (!m_spell.spellCheckSelection)
  {
    // if nothing is selected we have to check every cell
    // we use a different way to make it faster
    while ( m_spell.currentCell )
    {
      // check text only
      if ( m_spell.currentCell->value().isString() )
      {
        m_spell.kspell->check( m_spell.currentCell->text(), true );

        return;
      }

      m_spell.currentCell = m_spell.currentCell->nextCell();
      if ( m_spell.currentCell->isDefault() )
        kdDebug() << "checking default cell!!" << endl << endl;
    }

    if (spellSwitchToOtherTable())
      spellCheckerReady();
    else
      spellCleanup();

    return;
  }

  // if something is selected:

  ++m_spell.spellCurrCellX;
  if (m_spell.spellCurrCellX > m_spell.spellEndCellX)
  {
    m_spell.spellCurrCellX = m_spell.spellStartCellX;
    ++m_spell.spellCurrCellY;
  }

  unsigned int y;
  unsigned int x;

  for ( y = m_spell.spellCurrCellY; y <= m_spell.spellEndCellY; ++y )
  {
    for ( x = m_spell.spellCurrCellX; x <= m_spell.spellEndCellX; ++x )
    {
      KSpreadCell * cell = m_spell.currentSpellTable->cellAt( x, y );

      // check text only
      if (cell->isDefault() || !cell->value().isString())
        continue;

      m_spell.spellCurrCellX = x;
      m_spell.spellCurrCellY = y;

      m_spell.kspell->check( cell->text(), true );

      return;
    }
    m_spell.spellCurrCellX = m_spell.spellStartCellX;
  }

  // if the user selected something to be checked we are done
  // otherwise ask for checking the next table if any
  if (m_spell.spellCheckSelection)
  {
    // Done
    spellCleanup();
  }
  else
  {
    if (spellSwitchToOtherTable())
      spellCheckerReady();
    else
      spellCleanup();
  }
}


void KSpreadView::spellCleanup()
{
  if ( m_pCanvas )
    m_pCanvas->setCursor( ArrowCursor );

  m_spell.kspell->cleanUp();
  delete m_spell.kspell;
  m_spell.kspell            = 0L;
  m_spell.firstSpellTable   = 0L;
  m_spell.currentSpellTable = 0L;
  m_spell.currentCell       = 0L;
  m_spell.replaceAll.clear();


  KMessageBox::information( this, i18n( "Spell checking is complete." ) );

  if ( m_spell.macroCmdSpellCheck )
    m_pDoc->undoBuffer()->appendUndo( m_spell.macroCmdSpellCheck );
  m_spell.macroCmdSpellCheck=0L;
}


bool KSpreadView::spellSwitchToOtherTable()
{
  // there is no other table
  if ( m_pDoc->map()->count() == 1 )
    return false;

  // for optimization
  QPtrList<KSpreadSheet> tableList = m_pDoc->map()->tableList();

  unsigned int curIndex = tableList.findRef(m_spell.currentSpellTable);
  ++curIndex;

  // last table? then start at the beginning
  if ( curIndex >= tableList.count() )
    m_spell.currentSpellTable = tableList.first();
  else
    m_spell.currentSpellTable = tableList.at(curIndex);

  // if the current table is the first one again, we are done.
  if ( m_spell.currentSpellTable == m_spell.firstSpellTable )
  {
    setActiveTable( m_spell.firstSpellTable );
    return false;
  }

  if (m_spell.spellCheckSelection)
  {
    m_spell.spellEndCellX = m_spell.currentSpellTable->maxColumn();
    m_spell.spellEndCellY = m_spell.currentSpellTable->maxRow();

    m_spell.spellCurrCellX = m_spell.spellStartCellX - 1;
    m_spell.spellCurrCellY = m_spell.spellStartCellY;
  }
  else
  {
    m_spell.currentCell = m_spell.currentSpellTable->firstCell();
  }

  if ( KMessageBox::questionYesNo( this,
                                   i18n( "Do you want to check the spelling in the next table?") )
       != KMessageBox::Yes )
    return false;

  setActiveTable( m_spell.currentSpellTable );

  return true;
}


void KSpreadView::spellCheckerMisspelling( const QString &,
                                           const QStringList &,
                                           unsigned int )
{
  // scroll to the cell
  if ( !m_spell.spellCheckSelection )
  {
    m_spell.spellCurrCellX = m_spell.currentCell->column();
    m_spell.spellCurrCellY = m_spell.currentCell->row();
  }

  canvasWidget()->gotoLocation( m_spell.spellCurrCellX, m_spell.spellCurrCellY, activeTable() );
}


void KSpreadView::spellCheckerCorrected( const QString & old, const QString & corr,
                                         unsigned int pos )
{
  KSpreadCell * cell;

  if (m_spell.spellCheckSelection)
  {
    cell = m_spell.currentSpellTable->cellAt( m_spell.spellCurrCellX,
                                              m_spell.spellCurrCellY );
  }
  else
  {
    cell = m_spell.currentCell;
    m_spell.spellCurrCellX = cell->column();
    m_spell.spellCurrCellY = cell->row();
  }

  Q_ASSERT( cell );
  if ( !cell )
    return;

  m_pDoc->emitBeginOperation(false);
  QString content( cell->text() );

  KSpreadUndoSetText* undo = new KSpreadUndoSetText( m_pDoc, m_pTable,
                                                     content,
                                                     m_spell.spellCurrCellX,
                                                     m_spell.spellCurrCellY,
                                                     cell->formatType());
  content.replace( pos, old.length(), corr );
  cell->setCellText( content );
  m_pEditWidget->setText( content );

  if ( !m_spell.macroCmdSpellCheck )
      m_spell.macroCmdSpellCheck = new KSpreadMacroUndoAction( m_pDoc, i18n("Correct Misspelled Word") );
  m_spell.macroCmdSpellCheck->addCommand( undo );
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::spellCheckerDone( const QString & )
{
    int result = m_spell.kspell->dlgResult();

    m_spell.kspell->cleanUp();
    delete m_spell.kspell;
    m_spell.kspell = 0L;

    if ( result != KS_CANCEL && result != KS_STOP )
    {
        if (m_spell.spellCheckSelection)
        {
            if ( (m_spell.spellCurrCellY <= m_spell.spellEndCellY)
                 && (m_spell.spellCurrCellX <= m_spell.spellEndCellX) )
            {
                startKSpell();
                return;
            }
        }
        else
        {
            if ( m_spell.currentCell )
            {
                m_spell.currentCell = m_spell.currentCell->nextCell();

                startKSpell();

                return;
            }
        }
    }
    m_spell.replaceAll.clear();

    if ( m_spell.macroCmdSpellCheck )
    {
        m_pDoc->undoBuffer()->appendUndo( m_spell.macroCmdSpellCheck );
    }
    m_spell.macroCmdSpellCheck=0L;
}

void KSpreadView::spellCheckerFinished()
{
  if (m_pCanvas)
    m_pCanvas->setCursor( ArrowCursor );

  KSpell::spellStatus status = m_spell.kspell->status();
  m_spell.kspell->cleanUp();
  delete m_spell.kspell;
  m_spell.kspell = 0L;
  m_spell.replaceAll.clear();

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

  if (m_spell.macroCmdSpellCheck)
  {
      m_pDoc->undoBuffer()->appendUndo( m_spell.macroCmdSpellCheck );
  }
  m_spell.macroCmdSpellCheck=0L;


  if (kspellNotConfigured)
  {
    KSpreadpreference configDlg( this, 0 );
    configDlg.openPage( KSpreadpreference::KS_SPELLING);
    configDlg.exec();
  }
}

void KSpreadView::initialPosition()
{
    // Set the initial position for the marker as store in the XML file,
    // (1,1) otherwise
    int col = m_pDoc->map()->initialMarkerColumn();
    if ( col <= 0 )
      col = 1;
    int row = m_pDoc->map()->initialMarkerRow();
    if ( row <= 0 )
      row = 1;

    m_pCanvas->gotoLocation( col, row );

    updateBorderButton();
    updateShowTableMenu();

    d->actions->tableFormat->setEnabled(false);
    d->actions->sort->setEnabled(false);
    d->actions->mergeCell->setEnabled(false);
    d->actions->createStyle->setEnabled(false);

    d->actions->fillUp->setEnabled( false );
    d->actions->fillRight->setEnabled( false );
    d->actions->fillDown->setEnabled( false );
    d->actions->fillLeft->setEnabled( false );

    d->actions->recordChanges->setChecked( m_pDoc->map()->changes() );
    d->actions->acceptRejectChanges->setEnabled( m_pDoc->map()->changes() );
    d->actions->filterChanges->setEnabled( m_pDoc->map()->changes() );
    d->actions->protectChanges->setEnabled( m_pDoc->map()->changes() );
    d->actions->commentChanges->setEnabled( m_pDoc->map()->changes() );

    // make paint effective:
    m_pDoc->decreaseNumOperation();
    d->actions->insertChartFrame->setEnabled(false);

    QRect vr( activeTable()->visibleRect( m_pCanvas ) );

    m_pDoc->emitBeginOperation( false );
    activeTable()->setRegionPaintDirty( vr );
    m_pDoc->emitEndOperation( vr );

    m_bLoading = true;

    if ( koDocument()->isReadWrite() )
      initConfig();

    adjustActions( !m_pTable->isProtected() );
    adjustMapActions( !m_pDoc->map()->isProtected() );
}


void KSpreadView::updateButton( KSpreadCell *cell, int column, int row)
{
    m_toolbarLock = TRUE;

    QColor color=cell->textColor( column, row );
    if (!color.isValid())
        color=QApplication::palette().active().text();
    d->actions->textColor->setCurrentColor( color );

    color=cell->bgColor(  column, row );

    if ( !color.isValid() )
        color = QApplication::palette().active().base();

    d->actions->bgColor->setCurrentColor( color );

    d->actions->selectFontSize->setFontSize( cell->textFontSize( column, row ) );
    d->actions->selectFont->setFont( cell->textFontFamily( column,row ) );
    d->actions->bold->setChecked( cell->textFontBold( column, row ) );
    d->actions->italic->setChecked( cell->textFontItalic(  column, row) );
    d->actions->underline->setChecked( cell->textFontUnderline( column, row ) );
    d->actions->strikeOut->setChecked( cell->textFontStrike( column, row ) );

    d->actions->alignLeft->setChecked( cell->align( column, row ) == KSpreadFormat::Left );
    d->actions->alignCenter->setChecked( cell->align( column, row ) == KSpreadFormat::Center );
    d->actions->alignRight->setChecked( cell->align( column, row ) == KSpreadFormat::Right );

    d->actions->alignTop->setChecked( cell->alignY( column, row ) == KSpreadFormat::Top );
    d->actions->alignMiddle->setChecked( cell->alignY( column, row ) == KSpreadFormat::Middle );
    d->actions->alignBottom->setChecked( cell->alignY( column, row ) == KSpreadFormat::Bottom );

    d->actions->verticalText->setChecked( cell->verticalText( column,row ) );

    d->actions->multiRow->setChecked( cell->multiRow( column,row ) );

    KSpreadCell::FormatType ft = cell->formatType();
    d->actions->percent->setChecked( ft == KSpreadCell::Percentage );
    d->actions->money->setChecked( ft == KSpreadCell::Money );

    if ( m_pTable && !m_pTable->isProtected() )
      d->actions->removeComment->setEnabled( !cell->comment(column,row).isEmpty() );

    if ( m_pTable && !m_pTable->isProtected() )
      d->actions->decreaseIndent->setEnabled( cell->getIndent( column, row ) > 0.0 );

    m_toolbarLock = FALSE;
    if ( m_pTable )
      adjustActions( m_pTable, cell );
}

void KSpreadView::adjustActions( KSpreadSheet const * const table,
                                 KSpreadCell const * const cell )
{
  QRect selection = m_selectionInfo->selection();
  if ( table->isProtected() && !cell->isDefault() && cell->notProtected( cell->column(), cell->row() ) )
  {
    if ( ( selection.width() > 1 ) || ( selection.height() > 1 ) )
    {
      if ( d->actions->bold->isEnabled() )
        adjustActions( false );
    }
    else
    {
      if ( !d->actions->bold->isEnabled() )
        adjustActions( true );
    }
  }
  else if ( table->isProtected() )
  {
    if ( d->actions->bold->isEnabled() )
      adjustActions( false );
  }
}

void KSpreadView::updateEditWidgetOnPress()
{
    int column = m_pCanvas->markerColumn();
    int row    = m_pCanvas->markerRow();

    KSpreadCell* cell = m_pTable->cellAt( column, row );
    if ( !cell )
    {
        editWidget()->setText( "" );
        return;
    }
    if ( cell->content() == KSpreadCell::VisualFormula )
        editWidget()->setText( "" );
    else if ( m_pTable->isProtected() && cell->isHideFormula( column, row ) )
        editWidget()->setText( cell->strOutText() );
    else if ( m_pTable->isProtected() && cell->isHideAll( column, row ) )
        editWidget()->setText( "" );
    else
        editWidget()->setText( cell->text() );

    updateButton(cell, column, row);
    adjustActions( m_pTable, cell );
}

void KSpreadView::updateEditWidget()
{
    int column = m_pCanvas->markerColumn();
    int row    = m_pCanvas->markerRow();

    KSpreadCell * cell = m_pTable->cellAt( column, row );
    bool active = activeTable()->getShowFormula()
      && !( m_pTable->isProtected() && cell && cell->isHideFormula( column, row ) );

    if ( m_pTable && !m_pTable->isProtected() )
    {
      d->actions->alignLeft->setEnabled(!active);
      d->actions->alignCenter->setEnabled(!active);
      d->actions->alignRight->setEnabled(!active);
    }

    if ( !cell )
    {
        editWidget()->setText( "" );
        if ( m_pTable->isProtected() )
          editWidget()->setEnabled( false );
        else
          editWidget()->setEnabled( true );
        return;
    }

    if ( cell->content() == KSpreadCell::VisualFormula )
        editWidget()->setText( "" );
    else if ( m_pTable->isProtected() && cell->isHideFormula( column, row ) )
        editWidget()->setText( cell->strOutText() );
    else if ( m_pTable->isProtected() && cell->isHideAll( column, row ) )
        editWidget()->setText( "" );
    else
        editWidget()->setText( cell->text() );

    if ( m_pTable->isProtected() && !cell->notProtected( column, row ) )
      editWidget()->setEnabled( false );
    else
      editWidget()->setEnabled( true );

    if ( m_pCanvas->editor() )
    {
      m_pCanvas->editor()->setEditorFont(cell->textFont(column, row), true);
      m_pCanvas->editor()->setFocus();
    }
    updateButton(cell, column, row);
    adjustActions( m_pTable, cell );
}

void KSpreadView::activateFormulaEditor()
{
}

void KSpreadView::updateReadWrite( bool readwrite )
{
#ifdef __GNUC_
#warning TODO
#endif
    // m_pCancelButton->setEnabled( readwrite );
    // m_pOkButton->setEnabled( readwrite );
  m_pEditWidget->setEnabled( readwrite );

  QValueList<KAction*> actions = actionCollection()->actions();
  QValueList<KAction*>::ConstIterator aIt = actions.begin();
  QValueList<KAction*>::ConstIterator aEnd = actions.end();
  for (; aIt != aEnd; ++aIt )
    (*aIt)->setEnabled( readwrite );

  d->actions->transform->setEnabled( false );
  d->actions->redo->setEnabled( false );
  d->actions->undo->setEnabled( false );
  if ( !m_pDoc || !m_pDoc->map() || m_pDoc->map()->isProtected() )
  {
    d->actions->showTable->setEnabled( false );
    d->actions->hideTable->setEnabled( false );
  }
  else
  {
    d->actions->showTable->setEnabled( true );
    d->actions->hideTable->setEnabled( true );
  }
  d->actions->gotoCell->setEnabled( true );
  d->actions->viewZoom->setEnabled( true );
  d->actions->showPageBorders->setEnabled( true );
  d->actions->findAction->setEnabled( true);
  d->actions->replaceAction->setEnabled( readwrite );
  if ( !m_pDoc->isReadWrite())
      d->actions->copy->setEnabled( true );
  //  d->actions->newView->setEnabled( true );
  //m_pDoc->KXMLGUIClient::action( "newView" )->setEnabled( true ); // obsolete (Werner)
}

void KSpreadView::createTemplate()
{
  int width = 60;
  int height = 60;
  QPixmap pix = m_pDoc->generatePreview(QSize(width, height));

  KTempFile tempFile( QString::null, ".kst" );
  tempFile.setAutoDelete(true);

  m_pDoc->saveNativeFormat( tempFile.name() );

  KoTemplateCreateDia::createTemplate( "kspread_template", KSpreadFactory::global(),
                                           tempFile.name(), pix, this );

  KSpreadFactory::global()->dirs()->addResourceType("kspread_template",
                                                       KStandardDirs::kde_default( "data" ) +
                                                       "kspread/templates/");
}

void KSpreadView::tableFormat()
{
    KSpreadFormatDlg dlg( this );
    dlg.exec();
}

void KSpreadView::autoSum()
{
    // ######## Torben: Make sure that this can not be called
    // when canvas has a running editor
    if ( m_pCanvas->editor() )
        return;

    m_pCanvas->createEditor( KSpreadCanvas::CellEditor );
    m_pCanvas->editor()->setText( "=SUM()" );
    m_pCanvas->editor()->setCursorPosition( 5 );

    // Try to find numbers above
    if ( m_pCanvas->markerRow() > 1 )
    {
        KSpreadCell* cell = 0;
        int r = m_pCanvas->markerRow();
        do
        {
            cell = activeTable()->cellAt( m_pCanvas->markerColumn(), --r );
        }
        while ( cell && cell->value().isNumber() );

        if ( r + 1 < m_pCanvas->markerRow() )
        {
            m_pCanvas->startChoose( QRect( m_pCanvas->markerColumn(), r + 1, 1, m_pCanvas->markerRow() - r - 1 ) );
            return;
        }
    }

    // Try to find numbers left
    if ( m_pCanvas->markerColumn() > 1 )
    {
        KSpreadCell* cell = 0;
        int c = m_pCanvas->markerColumn();
        do
        {
            cell = activeTable()->cellAt( --c, m_pCanvas->markerRow() );
        }
        while ( cell && cell->value().isNumber() );

        if ( c + 1 < m_pCanvas->markerColumn() )
        {
            m_pCanvas->startChoose( QRect( c + 1, m_pCanvas->markerRow(), m_pCanvas->markerColumn() - c - 1, 1 ) );
            return;
        }
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
  m_pDoc->emitBeginOperation(false);
  if ( m_pTable != 0L )
  {
    m_pTable->setSelectionTextColor( selectionInfo(), d->actions->textColor->color() );
  }
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::setSelectionTextColor(const QColor &txtColor)
{
  m_pDoc->emitBeginOperation(false);
  if (m_pTable != 0L)
  {
    m_pTable->setSelectionTextColor( selectionInfo(), txtColor );
  }
  m_pDoc->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::changeBackgroundColor()
{
  m_pDoc->emitBeginOperation(false);
  if ( m_pTable != 0L )
  {
    m_pTable->setSelectionbgColor( selectionInfo(), d->actions->bgColor->color() );
  }
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::setSelectionBackgroundColor(const QColor &bgColor)
{
  m_pDoc->emitBeginOperation(false);
  if (m_pTable != 0L)
  {
    m_pTable->setSelectionbgColor( selectionInfo(), bgColor );
  }
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::changeBorderColor()
{
  m_pDoc->emitBeginOperation(false);
  if ( m_pTable != 0L )
  {
    m_pTable->setSelectionBorderColor( selectionInfo(), d->actions->borderColor->color() );
  }
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::setSelectionBorderColor(const QColor &bdColor)
{
  m_pDoc->emitBeginOperation(false);
  if (m_pTable != 0L)
  {
    m_pTable->setSelectionBorderColor( selectionInfo(), bdColor );
  }
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::helpUsing()
{
  kapp->invokeHelp( );
}

QButton * KSpreadView::newIconButton( const char *_file, bool _kbutton, QWidget *_parent )
{
  if ( _parent == 0L )
    _parent = this;

  QButton *pb;
  if ( !_kbutton )
    pb = new QPushButton( _parent );
  else
    pb = new QToolButton( _parent );
  pb->setPixmap( QPixmap( KSBarIcon(_file) ) );

  return pb;
}

void KSpreadView::enableUndo( bool _b )
{
  if ( m_pTable && !m_pTable->isProtected() )
    d->actions->undo->setEnabled( _b );
  d->actions->undo->setText(i18n("Undo: %1").arg(m_pDoc->undoBuffer()->getUndoName()));
}

void KSpreadView::enableRedo( bool _b )
{
  if ( m_pTable && !m_pTable->isProtected() )
    d->actions->redo->setEnabled( _b );
  d->actions->redo->setText(i18n("Redo: %1").arg(m_pDoc->undoBuffer()->getRedoName()));
}

void KSpreadView::enableInsertColumn( bool _b )
{
  if ( m_pTable && !m_pTable->isProtected() )
    d->actions->insertColumn->setEnabled( _b );
}

void KSpreadView::enableInsertRow( bool _b )
{
  if ( m_pTable && !m_pTable->isProtected() )
    d->actions->insertRow->setEnabled( _b );
}

void KSpreadView::undo()
{
  m_pDoc->emitBeginOperation( false );
  m_pDoc->undo();

  updateEditWidget();

  resultOfCalc();
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::redo()
{
  m_pDoc->emitBeginOperation( false );
  m_pDoc->redo();

  updateEditWidget();
  resultOfCalc();
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::deleteColumn()
{
  if ( !m_pTable )
    return;

  m_pDoc->emitBeginOperation( false );

  QRect r( m_selectionInfo->selection() );

  m_pTable->removeColumn( r.left(), ( r.right()-r.left() ) );

  updateEditWidget();
  m_selectionInfo->setSelection( m_selectionInfo->marker(),
                                 m_selectionInfo->marker(), m_pTable );

  QRect vr( m_pTable->visibleRect( m_pCanvas ) );
  vr.setLeft( r.left() );

  m_pDoc->emitEndOperation( vr );
}

void KSpreadView::deleteRow()
{
  if ( !m_pTable )
    return;

  m_pDoc->emitBeginOperation( false );
  QRect r( m_selectionInfo->selection() );
  m_pTable->removeRow( r.top(),(r.bottom()-r.top()) );

  updateEditWidget();
  m_selectionInfo->setSelection( m_selectionInfo->marker(),
                                 m_selectionInfo->marker(), m_pTable );

  QRect vr( m_pTable->visibleRect( m_pCanvas ) );
  vr.setTop( r.top() );

  m_pDoc->emitEndOperation( vr );
}

void KSpreadView::insertColumn()
{
  if ( !m_pTable )
    return;

  m_pDoc->emitBeginOperation( false );
  QRect r( m_selectionInfo->selection() );
  m_pTable->insertColumn( r.left(), ( r.right()-r.left() ) );

  updateEditWidget();

  QRect vr( m_pTable->visibleRect( m_pCanvas ) );
  vr.setLeft( r.left() - 1 );

  m_pDoc->emitEndOperation( vr );
}

void KSpreadView::hideColumn()
{
  if ( !m_pTable )
    return;
  m_pDoc->emitBeginOperation( false );
  QRect r( m_selectionInfo->selection() );
  m_pTable->hideColumn( r.left(), ( r.right()-r.left() ) );

  QRect vr( m_pTable->visibleRect( m_pCanvas ) );
  vr.setLeft( r.left() );
  m_pDoc->emitEndOperation( vr );
}

void KSpreadView::showColumn()
{
  if ( !m_pTable )
    return;

  KSpreadShowColRow dlg( this, "showCol", KSpreadShowColRow::Column );
  dlg.exec();
}

void KSpreadView::showSelColumns()
{
  if ( !m_pTable )
    return;

  int i;
  QRect rect = m_selectionInfo->selection();
  ColumnFormat * col;
  QValueList<int>hiddenCols;

  m_pDoc->emitBeginOperation( false );

  for ( i = rect.left(); i <= rect.right(); ++i )
  {
    if ( i == 2 ) // "B"
    {
      col = activeTable()->columnFormat( 1 );
      if ( col->isHide() )
      {
        hiddenCols.append( 1 );
      }
    }

    col = m_pTable->columnFormat( i );
    if ( col->isHide() )
    {
      hiddenCols.append( i );
    }
  }

  if ( hiddenCols.count() > 0 )
    m_pTable->showColumn( 0, -1, hiddenCols );

  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::insertRow()
{
  if ( !m_pTable )
    return;
  m_pDoc->emitBeginOperation( false );
  QRect r( m_selectionInfo->selection() );
  m_pTable->insertRow( r.top(), ( r.bottom() - r.top() ) );

  updateEditWidget();
  QRect vr( m_pTable->visibleRect( m_pCanvas ) );
  vr.setTop( r.top() - 1 );

  m_pDoc->emitEndOperation( vr );
}

void KSpreadView::hideRow()
{
  if ( !m_pTable )
    return;

  m_pDoc->emitBeginOperation( false );

  QRect r( m_selectionInfo->selection() );
  m_pTable->hideRow( r.top(), ( r.bottom() - r.top() ) );

  QRect vr( m_pTable->visibleRect( m_pCanvas ) );
  vr.setTop( r.top() );

  m_pDoc->emitEndOperation( vr );
}

void KSpreadView::showRow()
{
  if ( !m_pTable )
    return;

  KSpreadShowColRow dlg( this, "showRow", KSpreadShowColRow::Row );
  dlg.exec();
}

void KSpreadView::showSelRows()
{
  if ( !m_pTable )
    return;

  int i;
  QRect rect( m_selectionInfo->selection() );
  RowFormat * row;
  QValueList<int>hiddenRows;

  m_pDoc->emitBeginOperation( false );

  for ( i = rect.top(); i <= rect.bottom(); ++i )
  {
    if ( i == 2 )
    {
      row = activeTable()->rowFormat( 1 );
      if ( row->isHide() )
      {
        hiddenRows.append( 1 );
      }
    }

    row = m_pTable->rowFormat( i );
    if ( row->isHide() )
    {
      hiddenRows.append( i );
    }
  }

  if ( hiddenRows.count() > 0 )
    m_pTable->showRow( 0, -1, hiddenRows );

  endOperation( rect );
}

void KSpreadView::endOperation( QRect const & rect )
{
  QRect vr( m_pTable->visibleRect( m_pCanvas ) );
  if ( rect.left() > vr.left() )
    vr.setLeft( rect.left() );
  if ( rect.top() > vr.top() )
    vr.setTop( rect.top() );
  if ( rect.right() < vr.right() )
    vr.setRight( rect.right() );
  if ( rect.bottom() < vr.bottom() )
    vr.setBottom( rect.bottom() );

  m_pDoc->emitEndOperation( vr );
}

void KSpreadView::fontSelected( const QString & _font )
{
  if ( m_toolbarLock )
    return;

  m_pDoc->emitBeginOperation(false);
  if ( m_pTable != 0L )
    m_pTable->setSelectionFont( m_selectionInfo, _font.latin1() );

  // Dont leave the focus in the toolbars combo box ...
  if ( m_pCanvas->editor() )
  {
    KSpreadCell * cell = m_pTable->cellAt( m_selectionInfo->marker() );
    m_pCanvas->editor()->setEditorFont( cell->textFont( cell->column(), cell->row() ), true );
    m_pCanvas->editor()->setFocus();
  }
  else
    m_pCanvas->setFocus();

  endOperation( m_selectionInfo->selection() );
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
  if ( m_pTable != NULL )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->setSelectionSize( selectionInfo(), size );
    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::lower()
{
  if ( !m_pTable  )
    return;

  m_pDoc->emitBeginOperation( false );

  m_pTable->setSelectionUpperLower( selectionInfo(), -1 );
  updateEditWidget();

  endOperation( m_selectionInfo->selection() );
}

void KSpreadView::upper()
{
  if ( !m_pTable  )
    return;

  m_pDoc->emitBeginOperation( false );

  m_pTable->setSelectionUpperLower( selectionInfo(), 1 );
  updateEditWidget();

  endOperation( m_selectionInfo->selection() );
}

void KSpreadView::firstLetterUpper()
{
  if ( !m_pTable  )
    return;
  m_pDoc->emitBeginOperation( false );
  m_pTable->setSelectionfirstLetterUpper( selectionInfo() );
  updateEditWidget();
  endOperation( m_selectionInfo->selection() );
}

void KSpreadView::verticalText(bool b)
{
  if ( !m_pTable  )
    return;

  m_pDoc->emitBeginOperation( false );
  m_pTable->setSelectionVerticalText( selectionInfo(), b );
  if ( util_isRowSelected( selection() ) == FALSE
       && util_isColumnSelected( selection() ) == FALSE )
  {
    m_pCanvas->adjustArea( false );
    updateEditWidget();
    endOperation( m_selectionInfo->selection() );
    return;
  }

  m_pDoc->emitEndOperation( QRect( m_selectionInfo->marker(), m_selectionInfo->marker() ) );
}

void KSpreadView::insertSpecialChar()
{
  QString f( d->actions->selectFont->font() );
  QChar c = ' ';

  if ( m_specialCharDlg == 0 )
  {
    m_specialCharDlg = new KoCharSelectDia( this, "insert special char", f, c, false );
    connect( m_specialCharDlg, SIGNAL( insertChar( QChar, const QString & ) ),
             this, SLOT( slotSpecialChar( QChar, const QString & ) ) );
    connect( m_specialCharDlg, SIGNAL( finished() ),
             this, SLOT( slotSpecialCharDlgClosed() ) );
  }
  m_specialCharDlg->show();
}

void KSpreadView::slotSpecialCharDlgClosed()
{
  if ( m_specialCharDlg )
  {
    disconnect( m_specialCharDlg, SIGNAL(insertChar(QChar,const QString &)),
                this, SLOT(slotSpecialChar(QChar,const QString &)));
    disconnect( m_specialCharDlg, SIGNAL( finished() ),
                this, SLOT( slotSpecialCharDlgClosed() ) );
    m_specialCharDlg->deleteLater();
    m_specialCharDlg = 0L;
  }
}

void KSpreadView::slotSpecialChar( QChar c, const QString & _font )
{
  if ( m_pTable )
  {
    QPoint marker( selectionInfo()->marker() );
    KSpreadCell * cell = m_pTable->nonDefaultCell( marker );
    if ( cell->textFont( marker.x(), marker.y() ).family() != _font )
    {
      cell->setTextFontFamily( _font );
    }
    KSpreadEditWidget * edit = m_pCanvas->editWidget();
    QKeyEvent ev( QEvent::KeyPress, 0, 0, 0, QString( c ) );
    QApplication::sendEvent( edit, &ev );
  }
}

void KSpreadView::insertMathExpr()
{
  if ( m_pTable == 0L )
    return;

  KSpreadDlgFormula * dlg = new KSpreadDlgFormula( this, "Function" );
  dlg->show();

  /* TODO - because I search on 'TODO's :-) */
  // #### Is the dialog deleted when it's closed ? (David)
  // Torben thinks that not.
}

void KSpreadView::formulaSelection( const QString &_math )
{
  if ( m_pTable == 0 )
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
  if ( m_toolbarLock )
    return;

  m_pDoc->emitBeginOperation( false );

  if ( m_pTable != 0L )
    m_pTable->setSelectionFont( selectionInfo(), 0L, _size );

  // Dont leave the focus in the toolbars combo box ...
  if ( m_pCanvas->editor() )
  {
    KSpreadCell * cell = m_pTable->cellAt( m_selectionInfo->marker() );
    m_pCanvas->editor()->setEditorFont( cell->textFont( m_pCanvas->markerColumn(),
                                                        m_pCanvas->markerRow() ), true );
    m_pCanvas->editor()->setFocus();
  }
  else
    m_pCanvas->setFocus();

  endOperation( m_selectionInfo->selection() );
}

void KSpreadView::bold( bool b )
{
  if ( m_toolbarLock )
    return;
  if ( m_pTable == 0 )
    return;

  m_pDoc->emitBeginOperation( false );

  int col = m_pCanvas->markerColumn();
  int row = m_pCanvas->markerRow();
  m_pTable->setSelectionFont( selectionInfo(), 0L, -1, b );

  if ( m_pCanvas->editor() )
  {
    KSpreadCell * cell = m_pTable->cellAt( col, row );
    m_pCanvas->editor()->setEditorFont( cell->textFont( col, row ), true );
  }

  endOperation( m_selectionInfo->selection() );
}

void KSpreadView::underline( bool b )
{
  if ( m_toolbarLock )
    return;
  if ( m_pTable == 0 )
    return;

  m_pDoc->emitBeginOperation( false );

  int col = m_pCanvas->markerColumn();
  int row = m_pCanvas->markerRow();

  m_pTable->setSelectionFont( selectionInfo(), 0L, -1, -1, -1 ,b );
  if ( m_pCanvas->editor() )
  {
    KSpreadCell * cell = m_pTable->cellAt( col, row );
    m_pCanvas->editor()->setEditorFont( cell->textFont( col, row ), true );
  }

  endOperation( m_selectionInfo->selection() );
}

void KSpreadView::strikeOut( bool b )
{
  if ( m_toolbarLock )
    return;
  if ( m_pTable == 0 )
    return;

  m_pDoc->emitBeginOperation( false );

  int col = m_pCanvas->markerColumn();
  int row = m_pCanvas->markerRow();

  m_pTable->setSelectionFont( selectionInfo(), 0L, -1, -1, -1 ,-1, b );
  if ( m_pCanvas->editor() )
  {
    KSpreadCell * cell = m_pTable->cellAt( col, row );
    m_pCanvas->editor()->setEditorFont( cell->textFont( col, row ), true );
  }

  endOperation( m_selectionInfo->selection() );
}


void KSpreadView::italic( bool b )
{
  if ( m_toolbarLock )
    return;
  if ( m_pTable == 0 )
    return;

  m_pDoc->emitBeginOperation( false );

  int col = m_pCanvas->markerColumn();
  int row = m_pCanvas->markerRow();

  m_pTable->setSelectionFont( selectionInfo(), 0L, -1, -1, b );
  if ( m_pCanvas->editor() )
  {
    KSpreadCell * cell = m_pTable->cellAt( col, row );
    m_pCanvas->editor()->setEditorFont( cell->textFont( col, row ), true );
  }

  endOperation( m_selectionInfo->selection() );
}

void KSpreadView::sortInc()
{
  QRect r( m_selectionInfo->selection() );
  if ( m_selectionInfo->singleCellSelection() )
  {
    KMessageBox::error( this, i18n( "You must select multiple cells." ) );
    return;
  }

  m_pDoc->emitBeginOperation( false );

  // Entire row(s) selected ? Or just one row ?
  if ( util_isRowSelected( selection() ) || r.top() == r.bottom() )
    activeTable()->sortByRow( selection(), r.top(), KSpreadSheet::Increase );
  else
    activeTable()->sortByColumn( selection(), r.left(), KSpreadSheet::Increase );
  updateEditWidget();

  endOperation( m_selectionInfo->selection() );
}

void KSpreadView::sortDec()
{
  QRect r( m_selectionInfo->selection() );
  if ( m_selectionInfo->singleCellSelection() )
  {
    KMessageBox::error( this, i18n( "You must select multiple cells." ) );
    return;
  }

  m_pDoc->emitBeginOperation( false );

    // Entire row(s) selected ? Or just one row ?
  if ( util_isRowSelected( selection() ) || r.top() == r.bottom() )
    activeTable()->sortByRow( selection(), r.top(), KSpreadSheet::Decrease );
  else
    activeTable()->sortByColumn( selection(), r.left(), KSpreadSheet::Decrease );
  updateEditWidget();

  endOperation( m_selectionInfo->selection() );
}

void KSpreadView::reloadScripts()
{
  // TODO
}

void KSpreadView::runLocalScript()
{
  // TODO
}

void KSpreadView::editGlobalScripts()
{
  if ( KSpreadView::m_pGlobalScriptsDialog == 0L )
    KSpreadView::m_pGlobalScriptsDialog = new KSpreadScripts();
  KSpreadView::m_pGlobalScriptsDialog->show();
  KSpreadView::m_pGlobalScriptsDialog->raise();
}

void KSpreadView::editLocalScripts()
{
  // TODO
  /* if ( !m_pDoc->editPythonCode() )
     {
     KMessageBox::error( i18n( "Could not start editor" ) );
     return;
     } */
}

void KSpreadView::borderBottom()
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );

    m_pTable->borderBottom( m_selectionInfo, d->actions->borderColor->color() );

    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::setSelectionBottomBorderColor( const QColor & color )
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->borderBottom( selectionInfo(), color );
    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::borderRight()
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->borderRight( m_selectionInfo, d->actions->borderColor->color() );
    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::setSelectionRightBorderColor( const QColor & color )
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->borderRight( selectionInfo(), color );
    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::borderLeft()
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->borderLeft( m_selectionInfo, d->actions->borderColor->color() );
    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::setSelectionLeftBorderColor( const QColor & color )
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->borderLeft( selectionInfo(), color );
    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::borderTop()
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->borderTop( m_selectionInfo, d->actions->borderColor->color() );
    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::setSelectionTopBorderColor( const QColor & color )
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->borderTop( selectionInfo(), color );
    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::borderOutline()
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->borderOutline( m_selectionInfo, d->actions->borderColor->color() );
    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::setSelectionOutlineBorderColor( const QColor & color )
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->borderOutline( selectionInfo(), color );
    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::borderAll()
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->borderAll( m_selectionInfo, d->actions->borderColor->color() );
    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::setSelectionAllBorderColor( const QColor & color )
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->borderAll( selectionInfo(), color );
    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::borderRemove()
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->borderRemove( m_selectionInfo );
    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::addTable( KSpreadSheet * _t )
{
  m_pDoc->emitBeginOperation( false );

  insertTable( _t );

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
                    this, SLOT( slotTableRenamed( KSpreadSheet*, const QString& ) ) );
  QObject::connect( _t, SIGNAL( sig_TableHidden( KSpreadSheet* ) ),
                    this, SLOT( slotTableHidden( KSpreadSheet* ) ) );
  QObject::connect( _t, SIGNAL( sig_TableShown( KSpreadSheet* ) ),
                    this, SLOT( slotTableShown( KSpreadSheet* ) ) );
  QObject::connect( _t, SIGNAL( sig_TableRemoved( KSpreadSheet* ) ),
                    this, SLOT( slotTableRemoved( KSpreadSheet* ) ) );
  // ########### Why do these signals not send a pointer to the table?
  // This will lead to bugs.
  QObject::connect( _t, SIGNAL( sig_updateChildGeometry( KSpreadChild* ) ),
                    SLOT( slotUpdateChildGeometry( KSpreadChild* ) ) );
  QObject::connect( _t, SIGNAL( sig_removeChild( KSpreadChild* ) ), SLOT( slotRemoveChild( KSpreadChild* ) ) );
  QObject::connect( _t, SIGNAL( sig_maxColumn( int ) ), m_pCanvas, SLOT( slotMaxColumn( int ) ) );
  QObject::connect( _t, SIGNAL( sig_maxRow( int ) ), m_pCanvas, SLOT( slotMaxRow( int ) ) );

  if ( !m_bLoading )
    updateBorderButton();

  if ( !m_pTable )
  {
    m_pDoc->emitEndOperation();
    return;
  }
  endOperation( m_selectionInfo->selection() );
}

void KSpreadView::slotTableRemoved( KSpreadSheet *_t )
{
  m_pDoc->emitBeginOperation( false );

  QString m_tableName=_t->tableName();
  m_pTabBar->removeTab( _t->tableName() );
  if (m_pDoc->map()->findTable( m_pDoc->map()->visibleSheets().first()))
    setActiveTable( m_pDoc->map()->findTable( m_pDoc->map()->visibleSheets().first() ));
  else
    m_pTable = 0L;

  QValueList<Reference>::Iterator it;
  QValueList<Reference> area=doc()->listArea();
  for ( it = area.begin(); it != area.end(); ++it )
  {
    //remove Area Name when table target is removed
    if ( (*it).table_name == m_tableName )
    {
      doc()->removeArea( (*it).ref_name );
      //now area name is used in formula
      //so you must recalc tables when remove areaname
      KSpreadSheet * tbl;

      for ( tbl = doc()->map()->firstTable(); tbl != 0L; tbl = doc()->map()->nextTable() )
      {
        tbl->refreshRemoveAreaName((*it).ref_name);
      }
    }
  }

  endOperation( m_selectionInfo->selection() );
}

void KSpreadView::removeAllTables()
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->clear();

  setActiveTable( 0L );

  m_pDoc->emitEndOperation();
}

void KSpreadView::setActiveTable( KSpreadSheet * _t, bool updateTable )
{
  if ( _t == m_pTable )
    return;

  m_pDoc->emitBeginOperation(false);

  /* save the current selection on this table */
  if (m_pTable != NULL)
  {
    savedAnchors.replace(m_pTable, selectionInfo()->selectionAnchor());
    savedMarkers.replace(m_pTable, selectionInfo()->marker());
  }

  KSpreadSheet * oldSheet = m_pTable;

  m_pTable = _t;

  if ( m_pTable == 0L )
  {
    m_pDoc->emitEndOperation();
    return;
  }

  if ( oldSheet && oldSheet->isRightToLeft() != m_pTable->isRightToLeft() )
    refreshView();

  m_pDoc->setDisplayTable( m_pTable );
  if ( updateTable )
  {
    m_pTabBar->setActiveTab( _t->tableName() );
    m_pVBorderWidget->repaint();
    m_pHBorderWidget->repaint();
    m_pTable->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));
    m_pCanvas->slotMaxColumn( m_pTable->maxColumn() );
    m_pCanvas->slotMaxRow( m_pTable->maxRow() );
  }

  d->actions->showPageBorders->setChecked( m_pTable->isShowPageBorders() );
  d->actions->protectSheet->setChecked( m_pTable->isProtected() );
  d->actions->protectDoc->setChecked( m_pDoc->map()->isProtected() );
  adjustActions( !m_pTable->isProtected() );
  adjustMapActions( !m_pDoc->map()->isProtected() );

  /* see if there was a previous selection on this other table */
  QMapIterator<KSpreadSheet*, QPoint> it = savedAnchors.find(m_pTable);
  QMapIterator<KSpreadSheet*, QPoint> it2 = savedMarkers.find(m_pTable);

  QPoint newAnchor = (it == savedAnchors.end()) ? QPoint(1,1) : *it;
  QPoint newMarker = (it2 == savedMarkers.end()) ? QPoint(1,1) : *it2;
  selectionInfo()->setSelection(newMarker, newAnchor, m_pTable);
  if( m_pCanvas->chooseMode())
  {
    selectionInfo()->setChooseTable( m_pTable );
    selectionInfo()->setChooseMarker( QPoint(0,0) );
  }

  m_pCanvas->scrollToCell(newMarker);
  resultOfCalc();

  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::slotTableRenamed( KSpreadSheet* table, const QString& old_name )
{
  m_pDoc->emitBeginOperation( false );
  m_pTabBar->renameTab( old_name, table->tableName() );
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::slotTableHidden( KSpreadSheet* table )
{
  m_pDoc->emitBeginOperation(false);
  updateShowTableMenu();
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::slotTableShown( KSpreadSheet* table )
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->setTabs( m_pDoc->map()->visibleSheets() );
  updateShowTableMenu();
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::changeTable( const QString& _name )
{
    if ( activeTable()->tableName() == _name )
        return;

    KSpreadSheet *t = m_pDoc->map()->findTable( _name );
    if ( !t )
    {
        kdDebug(36001) << "Unknown table " << _name << endl;
        return;
    }
    m_pDoc->emitBeginOperation(false);
    m_pCanvas->closeEditor();
    setActiveTable( t, false /* False: Endless loop because of setActiveTab() => do the visual area update manually*/);

    updateEditWidget();
    //refresh toggle button
    updateBorderButton();

    //update visible area
    m_pVBorderWidget->repaint();
    m_pHBorderWidget->repaint();
    t->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));
    m_pCanvas->slotMaxColumn( m_pTable->maxColumn() );
    m_pCanvas->slotMaxRow( m_pTable->maxRow() );
    m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::moveTable( unsigned table, unsigned target )
{
    QStringList vs = m_pDoc->map()->visibleSheets();

    if( target >= vs.count() )
        m_pDoc->map()->moveTable( vs[ table ], vs[ vs.count()-1 ], false );
    else
        m_pDoc->map()->moveTable( vs[ table ], vs[ target ], true );

    m_pTabBar->moveTab( table, target );
}

void KSpreadView::insertTable()
{
  if ( m_pDoc->map()->isProtected() )
  {
    KMessageBox::error( 0, i18n ( "You cannot change a protected sheet" ) );
    return;
  }

  m_pDoc->emitBeginOperation( false );
  m_pCanvas->closeEditor();
  KSpreadSheet * t = m_pDoc->createTable();
  m_pDoc->addTable( t );
  updateEditWidget();
  KSpreadUndoAddTable *undo = new KSpreadUndoAddTable(m_pDoc, t);
  m_pDoc->undoBuffer()->appendUndo( undo );
  setActiveTable( t );

  if ( m_pDoc->map()->visibleSheets().count() > 1 )
  {
    d->actions->removeTable->setEnabled( true );
    d->actions->hideTable->setEnabled( true );
  }

  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::hideTable()
{
  if ( !m_pTable )
    return;

  if ( m_pDoc->map()->visibleSheets().count() ==  1)
  {
     KMessageBox::error( this, i18n("You cannot hide the last visible table.") );
     return;
  }

  QStringList vs = m_pDoc->map()->visibleSheets();
  int i = vs.findIndex( m_pTable->tableName() ) - 1;
  if( i < 0 ) i = 1;
  QString sn = vs[i];

  m_pDoc->emitBeginOperation(false);
  if ( !m_pDoc->undoBuffer()->isLocked() )
  {
    KSpreadUndoHideTable* undo = new KSpreadUndoHideTable( m_pDoc, activeTable() );
    m_pDoc->undoBuffer()->appendUndo( undo );
  }
  m_pTable->hideTable(true);
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );

  m_pTabBar->removeTab( m_pTable->tableName() );
  m_pTabBar->setActiveTab( sn );
}

void KSpreadView::showTable()
{
  if ( !m_pTable )
    return;

  KSpreadshow dlg( this, "Sheet show");
  dlg.exec();
}

void KSpreadView::copySelection()
{
  if ( !m_pTable )
    return;
  if ( !m_pCanvas->editor() )
  {
    m_pTable->copySelection( selectionInfo() );

    updateEditWidget();
  }
  else
    m_pCanvas->editor()->copy();
}

void KSpreadView::copyAsText()
{
  if ( !m_pTable )
    return;
  m_pTable->copyAsText( selectionInfo() );
}


void KSpreadView::cutSelection()
{
  if ( !m_pTable )
    return;
  //don't used this function when we edit a cell.
  m_pDoc->emitBeginOperation(false);

  if ( !m_pCanvas->editor())
  {
    m_pTable->cutSelection( selectionInfo() );
    resultOfCalc();
    updateEditWidget();
    }
  else
    m_pCanvas->editor()->cut();

  endOperation( selectionInfo()->selection() );
}

void KSpreadView::paste()
{
  if ( !m_pTable )
    return;

  if (!koDocument()->isReadWrite()) // don't paste into a read only document
    return;


  m_pDoc->emitBeginOperation( false );
  if ( !m_pCanvas->editor() )
  {
    m_pTable->paste( selection(), true, Normal, OverWrite, false, 0, true );
    resultOfCalc();
    updateEditWidget();
  }
  else
  {
    m_pCanvas->editor()->paste();
  }
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::specialPaste()
{
  if ( !m_pTable )
    return;

  KSpreadspecial dlg( this, "Special Paste" );
  if ( dlg.exec() )
  {
    if ( m_pTable->getAutoCalc() )
    {
      m_pDoc->emitBeginOperation( false );
      m_pTable->recalc();
      m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
    }
    resultOfCalc();
    updateEditWidget();
  }
}

void KSpreadView::removeComment()
{
  if ( !m_pTable )
        return;

  m_pDoc->emitBeginOperation(false);
  m_pTable->setSelectionRemoveComment( selectionInfo() );
  updateEditWidget();
  endOperation( selectionInfo()->selection() );
}


void KSpreadView::changeAngle()
{
  if ( !m_pTable )
    return;

  KSpreadAngle dlg( this, "Angle" ,
                    QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
  if ( dlg.exec() )
  {
    if ( (util_isRowSelected(selection()) == FALSE) &&
        (util_isColumnSelected(selection()) == FALSE) )
    {
      m_pDoc->emitBeginOperation( false );
      m_pCanvas->adjustArea( false );
      endOperation( selectionInfo()->selection() );
    }
  }
}

void KSpreadView::setSelectionAngle( int angle )
{
  m_pDoc->emitBeginOperation( false );

  if ( m_pTable != NULL )
  {
    m_pTable->setSelectionAngle( selectionInfo(), angle );

    if (util_isRowSelected(selection()) == false &&
        util_isColumnSelected(selection()) == false)
    {
      m_pCanvas->adjustArea(false);
    }
  }

  endOperation( selectionInfo()->selection() );
}

void KSpreadView::mergeCell()
{
  if ( !m_pTable )
    return;

  if ( ( util_isRowSelected( selection() ) )
       || ( util_isColumnSelected( selection() ) ) )
  {
    KMessageBox::error( this, i18n( "Area too large!" ) );
  }
  else
  {
    m_pDoc->emitBeginOperation( false );

    m_pTable->mergeCells( selection() );
    //  m_pCanvas->gotoLocation( selection().topLeft() );
    m_selectionInfo->setSelection( selection().topLeft(), selection().topLeft(), m_pTable );

    m_pDoc->decreaseNumOperation();
    //    endOperation( QRect( selection().topLeft(), selection().topLeft() ) );
  }
}

void KSpreadView::dissociateCell()
{
  if ( !m_pTable )
    return;

  m_pDoc->emitBeginOperation( false );

  m_pTable->dissociateCell( QPoint( m_pCanvas->markerColumn(),
                                    m_pCanvas->markerRow() ) );
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}


void KSpreadView::increaseIndent()
{
  if ( !m_pTable )
    return;

  m_pDoc->emitBeginOperation( false );
  m_pTable->increaseIndent( m_selectionInfo );
  updateEditWidget();
  endOperation( m_selectionInfo->selection() );
}

void KSpreadView::decreaseIndent()
{
  if ( !m_pTable )
    return;

  m_pDoc->emitBeginOperation( false );
  int column = m_pCanvas->markerColumn();
  int row = m_pCanvas->markerRow();

  m_pTable->decreaseIndent( m_selectionInfo );
  KSpreadCell * cell = m_pTable->cellAt( column, row );
  if ( cell )
    if ( !m_pTable->isProtected() )
      d->actions->decreaseIndent->setEnabled( cell->getIndent( column, row ) > 0.0 );

  endOperation( m_selectionInfo->selection() );
}

void KSpreadView::goalSeek()
{
  if ( m_pCanvas->editor() )
  {
    m_pCanvas->deleteEditor( true ); // save changes
  }

  KSpreadGoalSeekDlg * dlg
    = new KSpreadGoalSeekDlg( this, QPoint( m_pCanvas->markerColumn(),
                                            m_pCanvas->markerRow() ),
                              "KSpreadGoalSeekDlg" );
  dlg->show();
  /* dialog autodeletes itself */
}

void KSpreadView::subtotals()
{
  QRect selection( m_selectionInfo->selection() );
  if ( ( selection.width() < 2 )
       || ( selection.height() < 2 ) )
  {
    KMessageBox::error( this, i18n("You must select multiple cells.") );
    return;
  }

  KSpreadSubtotalDlg dlg(this, selection, "KSpreadSubtotalDlg" );
  if ( dlg.exec() )
  {
    m_pDoc->emitBeginOperation( false );
    m_selectionInfo->setSelection( dlg.selection().topLeft(),
                                   dlg.selection().bottomRight(),
                                   dlg.table() );
    endOperation( selection );
  }
}

void KSpreadView::multipleOperations()
{
  if ( m_pCanvas->editor() )
  {
    m_pCanvas->deleteEditor( true ); // save changes
  }
  //  KSpreadMultipleOpDlg * dlg = new KSpreadMultipleOpDlg( this, "KSpreadMultipleOpDlg" );
  //  dlg->show();
}

void KSpreadView::textToColumns()
{
  m_pCanvas->closeEditor();
  if ( m_selectionInfo->selection().width() > 1 )
  {
    KMessageBox::error( this, i18n("You must not select an area containing more than one column.") );
    return;
  }

  KSpreadCSVDialog dialog( this, "KSpreadCSVDialog", m_selectionInfo->selection(), KSpreadCSVDialog::Column );
  if( !dialog.cancelled() )
    dialog.exec();
}

void KSpreadView::consolidate()
{
  m_pCanvas->closeEditor();
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
    KFindDialog dlg( this, "Find", m_findOptions, m_findStrings );
    dlg.setHasSelection( !m_selectionInfo->singleCellSelection() );
    dlg.setHasCursor( true );
    if ( KFindDialog::Accepted != dlg.exec() )
        return;

    // Save for next time
    m_findOptions = dlg.options();
    m_findStrings = dlg.findHistory();

    // Create the KFind object
    delete m_find;
    delete m_replace;
    m_find = new KFind( dlg.pattern(), dlg.options(), this );
    m_replace = 0L;

    initFindReplace();
    findNext();
}

// Initialize a find or replace operation, using m_find or m_replace,
// and m_findOptions.
void KSpreadView::initFindReplace()
{
    KFind* findObj = m_find ? m_find : m_replace;
    Q_ASSERT( findObj );
    connect(findObj, SIGNAL( highlight( const QString &, int, int ) ),
            this, SLOT( slotHighlight( const QString &, int, int ) ) );
    connect(findObj, SIGNAL( findNext() ),
            this, SLOT( findNext() ) );

    bool bck = m_findOptions & KFindDialog::FindBackwards;
    KSpreadSheet* currentSheet = activeTable();

    QRect region = ( m_findOptions & KFindDialog::SelectedText )
                   ? m_selectionInfo->selection()
                   : QRect( 1, 1, currentSheet->maxColumn(), currentSheet->maxRow() ); // All cells

    int colStart = !bck ? region.left() : region.right();
    int colEnd = !bck ? region.right() : region.left();
    int rowStart = !bck ? region.top() :region.bottom();
    int rowEnd = !bck ? region.bottom() : region.top();
    if ( m_findOptions & KFindDialog::FromCursor ) {
        QPoint marker( m_selectionInfo->marker() );
        colStart = marker.x();
        rowStart = marker.y();
    }
    m_findLeftColumn = region.left();
    m_findRightColumn = region.right();
    m_findPos = QPoint( colStart, rowStart );
    m_findEnd = QPoint( colEnd, rowEnd );
    //kdDebug() << k_funcinfo << m_findPos << " to " << m_findEnd << endl;
    //kdDebug() << k_funcinfo << "leftcol=" << m_findLeftColumn << " rightcol=" << m_findRightColumn << endl;
}

void KSpreadView::findNext()
{
    KFind* findObj = m_find ? m_find : m_replace;
    if ( !findObj )  {
        find();
        return;
    }
    KFind::Result res = KFind::NoMatch;
    KSpreadCell* cell = findNextCell();
    bool forw = ! ( m_findOptions & KFindDialog::FindBackwards );
    while ( res == KFind::NoMatch && cell )
    {
        if ( findObj->needData() )
        {
            findObj->setData( cell->text() );
            m_findPos = QPoint( cell->column(), cell->row() );
            //kdDebug() << "setData(cell " << m_findPos << ")" << endl;
        }

        // Let KFind inspect the text fragment, and display a dialog if a match is found
        if ( m_find )
            res = m_find->find();
        else
            res = m_replace->replace();

        if ( res == KFind::NoMatch )  {
            // Go to next cell, skipping unwanted cells
            if ( forw )
                ++m_findPos.rx();
            else
                --m_findPos.rx();
            cell = findNextCell();
        }
    }

    if ( res == KFind::NoMatch )
    {
        //emitUndoRedo();
        //removeHighlight();
        if ( findObj->shouldRestart() ) {
            m_findOptions &= ~KFindDialog::FromCursor;
            findObj->resetCounts();
            findNext();
        }
        else { // done, close the 'find next' dialog
            if ( m_find )
                m_find->closeFindNextDialog();
            else
                m_replace->closeReplaceNextDialog();
        }
    }
}

KSpreadCell* KSpreadView::findNextCell()
{
    // getFirstCellRow / getNextCellRight would be faster at doing that,
    // but it doesn't seem to be easy to combine it with 'start a column m_find.x()'...

    KSpreadSheet* sheet = activeTable();
    KSpreadCell* cell = 0L;
    bool forw = ! ( m_findOptions & KFindDialog::FindBackwards );
    int col = m_findPos.x();
    int row = m_findPos.y();
    int maxRow = sheet->maxRow();
    //kdDebug() << "findNextCell starting at " << col << "," << row << "   forw=" << forw << endl;

    while ( !cell && row != m_findEnd.y() && (forw ? row < maxRow : row >= 0) )
    {
        while ( !cell && (forw ? col <= m_findRightColumn : col >= m_findLeftColumn) )
        {
            cell = sheet->cellAt( col, row );
            if ( cell->isDefault() || cell->isObscured() || cell->isFormula() )
                cell = 0L;
            if ( forw ) ++col;
            else --col;
        }
        if ( cell )
            break;
        // Prepare looking in the next row
        if ( forw )  {
            col = m_findLeftColumn;
            ++row;
        } else {
            col = m_findRightColumn;
            --row;
        }
        //kdDebug() << "next row: " << col << "," << row << endl;
    }
    // if ( !cell )
    // No more next cell - TODO go to next sheet (if not looking in a selection)
    // (and make m_findEnd (max,max) in that case...)
    //kdDebug() << k_funcinfo << " returning " << cell << endl;
    return cell;
}

void KSpreadView::findPrevious()
{
    KFind* findObj = m_find ? m_find : m_replace;
    if ( !findObj )  {
        find();
        return;
    }
    //kdDebug() << "findPrevious" << endl;
    int opt = m_findOptions;
    bool forw = ! ( opt & KFindDialog::FindBackwards );
    if ( forw )
        m_findOptions = ( opt | KFindDialog::FindBackwards );
    else
        m_findOptions = ( opt & ~KFindDialog::FindBackwards );

    findNext();

    m_findOptions = opt; // restore initial options
}

void KSpreadView::replace()
{
    KReplaceDialog dlg( this, "Replace", m_findOptions, m_findStrings, m_replaceStrings );
    dlg.setHasSelection( !m_selectionInfo->singleCellSelection() );
    dlg.setHasCursor( true );
    if ( KReplaceDialog::Accepted != dlg.exec() )
      return;

    m_findOptions = dlg.options();
    m_findStrings = dlg.findHistory();
    m_replaceStrings = dlg.replacementHistory();

    delete m_find;
    delete m_replace;
    m_find = 0L;
    m_replace = new KReplace( dlg.pattern(), dlg.replacement(), dlg.options() );
    initFindReplace();
    connect(
        m_replace, SIGNAL( replace( const QString &, int, int, int ) ),
        this, SLOT( slotReplace( const QString &, int, int, int ) ) );

    if ( !m_pDoc->undoBuffer()->isLocked() )
    {
        QRect region( m_findPos, m_findEnd );
        KSpreadUndoChangeAreaTextCell *undo = new KSpreadUndoChangeAreaTextCell( m_pDoc, activeTable(), region );
        m_pDoc->undoBuffer()->appendUndo( undo );
    }

    findNext();

#if 0
    // Refresh the editWidget
    // TODO - after a replacement only?
    KSpreadCell *cell = activeTable()->cellAt( canvasWidget()->markerColumn(),
                                               canvasWidget()->markerRow() );
    if ( cell->text() != 0L )
        editWidget()->setText( cell->text() );
    else
        editWidget()->setText( "" );
#endif
}

void KSpreadView::slotHighlight( const QString &/*text*/, int /*matchingIndex*/, int /*matchedLength*/ )
{
    m_pCanvas->gotoLocation( m_findPos, activeTable() );
#if KDE_IS_VERSION(3,1,90)
    KDialogBase *baseDialog=0L;
    if ( m_find )
        baseDialog = m_find->findNextDialog();
    else
        baseDialog = m_replace->replaceNextDialog();
    kdDebug()<<" baseDialog :"<<baseDialog<<endl;
    QRect globalRect( m_findPos, m_findEnd );
    globalRect.moveTopLeft( canvasWidget()->mapToGlobal( globalRect.topLeft() ) );
    KDialog::avoidArea( baseDialog, QRect( m_findPos, m_findEnd ));
#endif
}

void KSpreadView::slotReplace( const QString &newText, int, int, int )
{
    // Which cell was this again?
    KSpreadCell *cell = activeTable()->cellAt( m_findPos );

    // ...now I remember, update it!
    cell->setDisplayDirtyFlag();
    cell->setCellText( newText );
    cell->clearDisplayDirtyFlag();
}

void KSpreadView::conditional()
{
  QRect rect( m_selectionInfo->selection() );

  if ( (util_isRowSelected(selection())) || (util_isColumnSelected(selection())) )
  {
    KMessageBox::error( this, i18n("Area too large!") );
  }
  else
  {
    KSpreadConditionalDlg dlg( this, "KSpreadConditionalDlg", rect);
    dlg.exec();
  }
}

void KSpreadView::validity()
{
  QRect rect( m_selectionInfo->selection() );

  if ( (util_isRowSelected(selection())) || (util_isColumnSelected(selection())) )
  {
    KMessageBox::error( this, i18n("Area too large!"));
  }
  else
  {
    KSpreadDlgValidity dlg( this,"validity",rect);
    dlg.exec();
  }
}


void KSpreadView::insertSeries()
{
    m_pCanvas->closeEditor();
    KSpreadSeriesDlg dlg( this, "Series", QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    dlg.exec();
}

void KSpreadView::sort()
{
    if ( m_selectionInfo->singleCellSelection() )
    {
        KMessageBox::error( this, i18n("You must select multiple cells") );
        return;
    }

    KSpreadSortDlg dlg( this, "Sort" );
    dlg.exec();
}

void KSpreadView::insertHyperlink()
{
    m_pCanvas->closeEditor();

    KSpreadLinkDlg dlg( this, "Insert Link" );
    dlg.exec();
}

void KSpreadView::insertFromDatabase()
{
#ifndef QT_NO_SQL
    m_pCanvas->closeEditor();

    QRect rect = m_selectionInfo->selection();

    KSpreadDatabaseDlg dlg(this, rect, "KSpreadDatabaseDlg");
    dlg.exec();
#endif
}

void KSpreadView::insertFromTextfile()
{
    m_pCanvas->closeEditor();
    //KMessageBox::information( this, "Not implemented yet, work in progress...");

    KSpreadCSVDialog dialog( this, "KSpreadCSVDialog", selection(), KSpreadCSVDialog::File );
    if( !dialog.cancelled() )
      dialog.exec();
}

void KSpreadView::insertFromClipboard()
{
    m_pCanvas->closeEditor();

    KSpreadCSVDialog dialog( this, "KSpreadCSVDialog", m_selectionInfo->selection(), KSpreadCSVDialog::Clipboard );
    if( !dialog.cancelled() )
      dialog.exec();
}

void KSpreadView::setupPrinter( KPrinter &prt )
{
    KSpreadSheetPrint* print = m_pTable->print();

    //apply page layout parameters
    KoFormat pageFormat = print->paperFormat();

    prt.setPageSize( static_cast<KPrinter::PageSize>( KoPageFormat::printerPageSize( pageFormat ) ) );

    if ( print->orientation() == PG_LANDSCAPE || pageFormat == PG_SCREEN )
        prt.setOrientation( KPrinter::Landscape );
    else
        prt.setOrientation( KPrinter::Portrait );

    prt.setFullPage( TRUE );
    prt.setResolution ( 600 );
}

void KSpreadView::print( KPrinter &prt )
{
    KSpreadSheetPrint* print = m_pTable->print();

    if ( m_pCanvas->editor() )
    {
      m_pCanvas->deleteEditor( true ); // save changes
    }

    int oldZoom = m_pDoc->zoom();

    //Comment from KWord
    //   We don't get valid metrics from the printer - and we want a better resolution
    //   anyway (it's the PS driver that takes care of the printer resolution).
    //But KSpread uses fixed 300 dpis, so we can use it.

    QPaintDeviceMetrics metrics( &prt );

    int dpiX = metrics.logicalDpiX();
    int dpiY = metrics.logicalDpiY();

    m_pDoc->setZoomAndResolution( int( print->zoom() * 100 ), dpiX, dpiY );

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

    m_pDoc->setZoomAndResolution( oldZoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );
    m_pDoc->newZoomAndResolution( true, false );

    // Repaint at correct zoom
    m_pDoc->emitBeginOperation( false );
    setZoom( oldZoom, false );
    m_pDoc->emitEndOperation();

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
    if ( !m_pTable )
      return;

    // Transform the view coordinates to document coordinates
    KoRect unzoomedRect = m_pDoc->unzoomRect( _geometry );
    unzoomedRect.moveBy( m_pCanvas->xOffset(), m_pCanvas->yOffset() );

    //KOfficeCore cannot handle KoRect directly, so switching to QRect
    QRect unzoomedGeometry = unzoomedRect.toQRect();

    if ( (util_isRowSelected(selection())) || (util_isColumnSelected(selection())) )
    {
      KMessageBox::error( this, i18n("Area too large!"));
      m_pTable->insertChart( unzoomedGeometry,
                             _e,
                             QRect( m_pCanvas->markerColumn(),
                                    m_pCanvas->markerRow(),
                                    1,
                                    1 ) );
    }
    else
    {
      // Insert the new child in the active table.
      m_pTable->insertChart( unzoomedGeometry,
                             _e,
                             m_selectionInfo->selection() );
    }
}

void KSpreadView::insertChild( const QRect& _geometry, KoDocumentEntry& _e )
{
  if ( !m_pTable )
    return;

  // Transform the view coordinates to document coordinates
  KoRect unzoomedRect = m_pDoc->unzoomRect( _geometry );
  unzoomedRect.moveBy( m_pCanvas->xOffset(), m_pCanvas->yOffset() );

  //KOfficeCore cannot handle KoRect directly, so switching to QRect
  QRect unzoomedGeometry = unzoomedRect.toQRect();

  // Insert the new child in the active table.
  m_pTable->insertChild( unzoomedGeometry, _e );
}

void KSpreadView::slotRemoveChild( KSpreadChild *_child )
{
  if ( _child->table() != m_pTable )
    return;

  // Make shure that this child has no active embedded view -> activate ourselfs
  m_pDoc->emitBeginOperation( false );
  partManager()->setActivePart( koDocument(), this );
  partManager()->setSelectedPart( 0 );
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::slotUpdateChildGeometry( KSpreadChild */*_child*/ )
{
    // ##############
    // TODO
    /*
  if ( _child->table() != m_pTable )
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
   if ( !m_pDoc || !m_pDoc->map() )
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
     m_pDoc->map()->setProtected( hash );
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
     if ( !m_pDoc->map()->checkPassword( hash ) )
     {
       KMessageBox::error( 0, i18n( "Incorrect password" ) );
       d->actions->protectDoc->setChecked( true );
       return;
     }

     m_pDoc->map()->setProtected( QCString() );
   }

   m_pDoc->setModified( true );
   adjustMapActions( !mode );
}

void KSpreadView::adjustMapActions( bool mode )
{
  d->actions->hideTable->setEnabled( mode );
  d->actions->showTable->setEnabled( mode );
  d->actions->insertTable->setEnabled( mode );
  d->actions->menuInsertTable->setEnabled( mode );
  d->actions->removeTable->setEnabled( mode );

  if ( mode )
  {
    if ( m_pTable && !m_pTable->isProtected() )
    {
      bool state = ( m_pDoc->map()->visibleSheets().count() > 1 );
      d->actions->removeTable->setEnabled( state );
      d->actions->hideTable->setEnabled( state );
    }
    d->actions->showTable->setEnabled( m_pDoc->map()->hiddenSheets().count() > 0 );
    if ( m_pTable->isProtected() )
      d->actions->renameTable->setEnabled( false );
    else
      d->actions->renameTable->setEnabled( true );
  }
  // slotUpdateView( m_pTable );
}

void KSpreadView::toggleProtectSheet( bool mode )
{
   if ( !m_pTable )
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

     m_pTable->setProtected( hash );
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

     if ( !m_pTable->checkPassword( hash ) )
     {
       KMessageBox::error( 0, i18n( "Incorrect password" ) );
       d->actions->protectSheet->setChecked( true );
       return;
     }

     m_pTable->setProtected( QCString() );
   }
   m_pDoc->setModified( true );
   adjustActions( !mode );
   m_pDoc->emitBeginOperation();
   // m_pTable->setRegionPaintDirty( QRect(QPoint( 0, 0 ), QPoint( KS_colMax, KS_rowMax ) ) );
   refreshView();
   updateEditWidget();
   m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::adjustActions( bool mode )
{
  d->actions->replaceAction->setEnabled( mode );
  d->actions->insertSeries->setEnabled( mode );
  d->actions->insertLink->setEnabled( mode );
  d->actions->insertSpecialChar->setEnabled( mode );
  d->actions->insertFunction->setEnabled( mode );
  d->actions->removeComment->setEnabled( mode );
  d->actions->decreaseIndent->setEnabled( mode );
  d->actions->bold->setEnabled( mode );
  d->actions->italic->setEnabled( mode );
  d->actions->underline->setEnabled( mode );
  d->actions->strikeOut->setEnabled( mode );
  d->actions->percent->setEnabled( mode );
  d->actions->precplus->setEnabled( mode );
  d->actions->precminus->setEnabled( mode );
  d->actions->money->setEnabled( mode );
  d->actions->alignLeft->setEnabled( mode );
  d->actions->alignCenter->setEnabled( mode );
  d->actions->alignRight->setEnabled( mode );
  d->actions->alignTop->setEnabled( mode );
  d->actions->alignMiddle->setEnabled( mode );
  d->actions->alignBottom->setEnabled( mode );
  d->actions->paste->setEnabled( mode );
  d->actions->cut->setEnabled( mode );
  d->actions->specialPaste->setEnabled( mode );
  d->actions->del->setEnabled( mode );
  d->actions->clearText->setEnabled( mode );
  d->actions->clearComment->setEnabled( mode );
  d->actions->clearValidity->setEnabled( mode );
  d->actions->clearConditional->setEnabled( mode );
  d->actions->recalc_workbook->setEnabled( mode );
  d->actions->recalc_worksheet->setEnabled( mode );
  d->actions->adjust->setEnabled( mode );
  d->actions->editCell->setEnabled( mode );
  if( !mode )
  {
      d->actions->undo->setEnabled( false );
      d->actions->redo->setEnabled( false );
  }
  else
  {
      d->actions->undo->setEnabled( m_pDoc->undoBuffer()->hasUndoActions() );
      d->actions->redo->setEnabled( m_pDoc->undoBuffer()->hasRedoActions() );
  }

  d->actions->paperLayout->setEnabled( mode );
  d->actions->styleDialog->setEnabled( mode );
  d->actions->definePrintRange->setEnabled( mode );
  d->actions->resetPrintRange->setEnabled( mode );
  d->actions->insertFromDatabase->setEnabled( mode );
  d->actions->insertFromTextfile->setEnabled( mode );
  d->actions->insertFromClipboard->setEnabled( mode );
  d->actions->conditional->setEnabled( mode );
  d->actions->validity->setEnabled( mode );
  d->actions->goalSeek->setEnabled( mode );
  d->actions->subTotals->setEnabled( mode );
  d->actions->multipleOperations->setEnabled( mode );
  d->actions->textToColumns->setEnabled( mode );
  d->actions->consolidate->setEnabled( mode );
  d->actions->insertCellCopy->setEnabled( mode );
  d->actions->multiRow->setEnabled( mode );
  d->actions->selectFont->setEnabled( mode );
  d->actions->selectFontSize->setEnabled( mode );
  d->actions->deleteColumn->setEnabled( mode );
  d->actions->hideColumn->setEnabled( mode );
  d->actions->showColumn->setEnabled( mode );
  d->actions->showSelColumns->setEnabled( mode );
  d->actions->insertColumn->setEnabled( mode );
  d->actions->deleteRow->setEnabled( mode );
  d->actions->insertRow->setEnabled( mode );
  d->actions->hideRow->setEnabled( mode );
  d->actions->showRow->setEnabled( mode );
  d->actions->showSelRows->setEnabled( mode );
  d->actions->formulaSelection->setEnabled( mode );
  d->actions->textColor->setEnabled( mode );
  d->actions->bgColor->setEnabled( mode );
  d->actions->cellLayout->setEnabled( mode );
  d->actions->borderLeft->setEnabled( mode );
  d->actions->borderRight->setEnabled( mode );
  d->actions->borderTop->setEnabled( mode );
  d->actions->borderBottom->setEnabled( mode );
  d->actions->borderAll->setEnabled( mode );
  d->actions->borderOutline->setEnabled( mode );
  d->actions->borderRemove->setEnabled( mode );
  d->actions->borderColor->setEnabled( mode );
  d->actions->removeTable->setEnabled( mode );
  d->actions->autoSum->setEnabled( mode );
  //   d->actions->scripts->setEnabled( mode );
  d->actions->defaultFormat->setEnabled( mode );
  d->actions->areaName->setEnabled( mode );
  d->actions->resizeRow->setEnabled( mode );
  d->actions->resizeColumn->setEnabled( mode );
  d->actions->fontSizeUp->setEnabled( mode );
  d->actions->fontSizeDown->setEnabled( mode );
  d->actions->upper->setEnabled( mode );
  d->actions->lower->setEnabled( mode );
  d->actions->equalizeRow->setEnabled( mode );
  d->actions->equalizeColumn->setEnabled( mode );
  d->actions->verticalText->setEnabled( mode );
  d->actions->addModifyComment->setEnabled( mode );
  d->actions->removeComment->setEnabled( mode );
  d->actions->insertCell->setEnabled( mode );
  d->actions->removeCell->setEnabled( mode );
  d->actions->changeAngle->setEnabled( mode );
  d->actions->dissociateCell->setEnabled( mode );
  d->actions->increaseIndent->setEnabled( mode );
  d->actions->decreaseIndent->setEnabled( mode );
  d->actions->spellChecking->setEnabled( mode );
  d->actions->menuCalcMin->setEnabled( mode );
  d->actions->menuCalcMax->setEnabled( mode );
  d->actions->menuCalcAverage->setEnabled( mode );
  d->actions->menuCalcCount->setEnabled( mode );
  d->actions->menuCalcSum->setEnabled( mode );
  d->actions->menuCalcNone->setEnabled( mode );
  d->actions->insertPart->setEnabled( mode );
  d->actions->createStyle->setEnabled( mode );
  d->actions->selectStyle->setEnabled( mode );

  d->actions->tableFormat->setEnabled( false );
  d->actions->sort->setEnabled( false );
  d->actions->mergeCell->setEnabled( false );
  d->actions->insertChartFrame->setEnabled( false );
  d->actions->sortDec->setEnabled( false );
  d->actions->sortInc->setEnabled( false );
  d->actions->transform->setEnabled( false );

  d->actions->fillRight->setEnabled( false );
  d->actions->fillLeft->setEnabled( false );
  d->actions->fillUp->setEnabled( false );
  d->actions->fillDown->setEnabled( false );

  if ( mode && m_pDoc && m_pDoc->map() && !m_pDoc->map()->isProtected() )
    d->actions->renameTable->setEnabled( true );
  else
    d->actions->renameTable->setEnabled( false );

  canvasWidget()->gotoLocation( m_selectionInfo->marker(), m_pTable );
}

void KSpreadView::toggleRecordChanges( bool mode )
{
  if ( !mode )
  {
    if ( KMessageBox::questionYesNo( this,
                                     i18n( "You are about to exit the change recording mode. All the informations about changes will be lost. Do you want to continue?" ) )
         != KMessageBox::Yes )
      return;
  }

  if ( d->actions->protectChanges->isChecked() )
  {
    if ( !checkChangeRecordPassword() )
      return;
    d->actions->protectChanges->setChecked( false );
  }

  if ( mode )
    m_pTable->map()->startRecordingChanges();
  else
    m_pTable->map()->stopRecordingChanges();

  d->actions->protectChanges->setEnabled( mode );
  d->actions->filterChanges->setEnabled( mode );
  d->actions->acceptRejectChanges->setEnabled( mode );
  d->actions->commentChanges->setEnabled( mode );
}

void KSpreadView::toggleProtectChanges( bool mode )
{
  if ( !d->actions->recordChanges->isChecked() )
  {
    d->actions->protectChanges->setChecked( false );
    return;
  }

   if ( mode )
   {
     QCString passwd;
     int result = KPasswordDialog::getNewPassword( passwd, i18n( "Protect Recorded Changes" ) );
     if ( result != KPasswordDialog::Accepted )
     {
       d->actions->protectChanges->setChecked( false );
       return;
     }

     QCString hash( "" );
     QString password( passwd );
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );
     m_pTable->map()->changes()->setProtected( hash );
   }
   else
   {
     checkChangeRecordPassword();
   }
}

bool KSpreadView::checkChangeRecordPassword()
{
  QCString passwd;
  m_pTable->map()->changes()->password( passwd );
  if ( passwd.isNull() || passwd.length() == 0 )
  {
    m_pTable->map()->changes()->setProtected( QCString() );
    return true;
  }

  int result = KPasswordDialog::getPassword( passwd, i18n( "Unprotect Recorded Changes" ) );
  if ( result != KPasswordDialog::Accepted )
  {
    d->actions->protectChanges->setChecked( true );
    return false;
  }

  QCString hash( "" );
  QString password( passwd );
  if ( password.length() > 0 )
    SHA1::getHash( password, hash );
  if ( !m_pTable->map()->changes()->checkPassword( hash ) )
  {
    KMessageBox::error( 0, i18n( "Incorrect password" ) );
    d->actions->protectChanges->setChecked( true );
    return false;
  }

  m_pTable->map()->changes()->setProtected( QCString() );
  d->actions->protectChanges->setChecked( false );
  return true;
}

void KSpreadView::filterChanges()
{
  if ( !d->actions->recordChanges->isChecked() )
    return;

  KSpreadFilterDlg dlg( this, m_pTable->map()->changes() );
  dlg.exec();
}

void KSpreadView::acceptRejectChanges()
{
  if ( !d->actions->recordChanges->isChecked() )
    return;

  KSpreadAcceptDlg dlg( this, m_pTable->map()->changes() );
  dlg.exec();
}

void KSpreadView::commentChanges()
{
  if ( !d->actions->recordChanges->isChecked() )
    return;

  KSpreadCommentDlg dlg( this, m_pTable->map()->changes() );
  dlg.exec();
}

void KSpreadView::mergeDocument()
{
}

void KSpreadView::togglePageBorders( bool mode )
{
  if ( !m_pTable )
    return;

  m_pDoc->emitBeginOperation( false );
  m_pTable->setShowPageBorders( mode );
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::changeZoomMenu( int zoom )
{
  if( d->actions->viewZoom->items().isEmpty() )
  {
    QStringList lst;
    lst << i18n("%1%").arg("33");
    lst << i18n("%1%").arg("50");
    lst << i18n("%1%").arg("75");
    lst << i18n("%1%").arg("100");
    lst << i18n("%1%").arg("125");
    lst << i18n("%1%").arg("150");
    lst << i18n("%1%").arg("200");
    lst << i18n("%1%").arg("250");
    lst << i18n("%1%").arg("350");
    lst << i18n("%1%").arg("400");
    lst << i18n("%1%").arg("450");
    lst << i18n("%1%").arg("500");
    d->actions->viewZoom->setItems( lst );
  }

  if( zoom>0 )
  {
    QValueList<int> list;
    bool ok;
    const QStringList itemsList( d->actions->viewZoom->items() );
    QRegExp regexp("(\\d+)"); // "Captured" non-empty sequence of digits

    for (QStringList::ConstIterator it = itemsList.begin() ; it != itemsList.end() ; ++it)
    {
      regexp.search(*it);
      const int val=regexp.cap(1).toInt(&ok);
      //zoom : limit inferior=10
      if( ok && val>9 && list.contains(val)==0 )
        list.append( val );

      //necessary at the beginning when we read config
      //this value is not in combo list
      if(list.contains(zoom)==0)
        list.append( zoom );

      qHeapSort( list );

      QStringList lst;
      for (QValueList<int>::Iterator it = list.begin() ; it != list.end() ; ++it)
        lst.append( i18n("%1%").arg(*it) );
      d->actions->viewZoom->setItems( lst );
    }
  }
}

void KSpreadView::viewZoom( const QString & s )
{
  int oldZoom = m_pDoc->zoom();

  bool ok = false;
  QRegExp regexp("(\\d+)"); // "Captured" non-empty sequence of digits
  regexp.search(s);
  int newZoom=regexp.cap(1).toInt(&ok);

//   kdDebug(36001) << "---------viewZoom: " << z << " - " << s << ", newZoom: " << newZoom
//                  << ", oldZoom " << oldZoom << ", " << zoom() << endl;

  if ( !ok || newZoom < 10 ) //zoom should be valid and >10
    newZoom = oldZoom;

  if ( newZoom != oldZoom )
  {
    changeZoomMenu( newZoom );
    QString zoomStr( i18n("%1%").arg( newZoom ) );
    d->actions->viewZoom->setCurrentItem( d->actions->viewZoom->items().findIndex( zoomStr ) );

    m_pDoc->emitBeginOperation( false );

    m_pCanvas->closeEditor();
    setZoom( newZoom, true );

    QRect r( m_pTable->visibleRect( m_pCanvas ) );
    r.setWidth( r.width() + 2 );
    m_pDoc->emitEndOperation( r );
  }
}

void KSpreadView::setZoom( int zoom, bool /*updateViews*/ )
{
  kdDebug() << "---------SetZoom: " << zoom << endl;

  // Set the zoom in KoView (for embedded views)
  m_pDoc->emitBeginOperation( false );

  m_pDoc->setZoomAndResolution( zoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY());
  KoView::setZoom( m_pDoc->zoomedResolutionY() /* KoView only supports one zoom */ );

  m_pTable->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));
  m_pDoc->refreshInterface();

  m_pDoc->emitEndOperation();
}

void KSpreadView::preference()
{
  if ( !m_pTable )
    return;

  KSpreadpreference dlg( this, "Preference" );
  if ( dlg.exec() )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->refreshPreference();
    m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
  }
}

void KSpreadView::addModifyComment()
{
  if ( !m_pTable )
    return;

  KSpreadComment dlg( this, "comment",
                      QPoint( m_pCanvas->markerColumn(),
                              m_pCanvas->markerRow() ) );
  if ( dlg.exec() )
    updateEditWidget();
}

void KSpreadView::setSelectionComment( QString comment )
{
  if ( m_pTable != NULL )
  {
    m_pDoc->emitBeginOperation( false );

    m_pTable->setSelectionComment( selectionInfo(), comment.stripWhiteSpace() );
    updateEditWidget();

    endOperation( m_selectionInfo->selection() );
  }
}

void KSpreadView::editCell()
{
  if ( m_pCanvas->editor() )
    return;

  m_pCanvas->createEditor();
}

bool KSpreadView::showTable(const QString& tableName) {
  KSpreadSheet *t=m_pDoc->map()->findTable(tableName);
  if ( !t )
  {
    kdDebug(36001) << "Unknown table " <<tableName<<  endl;
    return false;
  }
  m_pCanvas->closeEditor();
  setActiveTable( t );

  return true;
}

void KSpreadView::nextTable(){

  KSpreadSheet * t = m_pDoc->map()->nextTable( activeTable() );
  if ( !t )
  {
    kdDebug(36001) << "Unknown table " <<  endl;
    return;
  }
  m_pCanvas->closeEditor();
  setActiveTable( t );
}

void KSpreadView::previousTable()
{
  KSpreadSheet * t = m_pDoc->map()->previousTable( activeTable() );
  if ( !t )
  {
    kdDebug(36001) << "Unknown table "  << endl;
    return;
  }
  m_pCanvas->closeEditor();
  setActiveTable( t );
}

void KSpreadView::firstTable()
{
  KSpreadSheet *t = m_pDoc->map()->firstTable();
  if ( !t )
  {
    kdDebug(36001) << "Unknown table " <<  endl;
    return;
  }
  m_pCanvas->closeEditor();
  setActiveTable( t );
}

void KSpreadView::lastTable()
{
  KSpreadSheet *t = m_pDoc->map()->lastTable( );
  if ( !t )
  {
    kdDebug(36001) << "Unknown table " <<  endl;
    return;
  }
  m_pCanvas->closeEditor();
  setActiveTable( t );
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
       case Key_V: // Ctrl+Shift+V to show debug (similar to KWord)
        if ( _ev->state() & Qt::ShiftButton )
          m_pTable->printDebug();
#endif
       default:
        QWidget::keyPressEvent( _ev );
        return;
      }
    }
    QWidget::keyPressEvent( _ev );
  }
  else
    QApplication::sendEvent( m_pCanvas, _ev );
}

KoDocument * KSpreadView::hitTest( const QPoint &pos )
{
    // Code copied from KoView::hitTest
    KoViewChild *viewChild;

    QWMatrix m = matrix();
    m.translate( m_pCanvas->xOffset() / m_pDoc->zoomedResolutionX(),
                 m_pCanvas->yOffset() / m_pDoc->zoomedResolutionY() );

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

    QPtrListIterator<KoDocumentChild> it( m_pDoc->children() );
    for (; it.current(); ++it )
    {
        // Is the child document on the visible table ?
        if ( ((KSpreadChild*)it.current())->table() == m_pTable )
        {
            KoDocument *doc = it.current()->hitTest( pos, m );
            if ( doc )
                return doc;
        }
    }

    return m_pDoc;
}

int KSpreadView::leftBorder() const
{
  return int( m_pCanvas->doc()->zoomItX( YBORDER_WIDTH ) );
}

int KSpreadView::rightBorder() const
{
  return m_pVertScrollBar->width();
}

int KSpreadView::topBorder() const
{
  return m_pToolWidget->height() + int( m_pCanvas->doc()->zoomItX( KSpreadFormat::globalRowHeight() + 2 ) );
}

int KSpreadView::bottomBorder() const
{
  return m_pHorzScrollBar->height();
}

void KSpreadView::refreshView()
{
  KSpreadSheet * table = activeTable();

  bool active = table->getShowFormula();

  if ( table && !table->isProtected() )
  {
    d->actions->alignLeft->setEnabled( !active );
    d->actions->alignCenter->setEnabled( !active );
    d->actions->alignRight->setEnabled( !active );
  }
  active = m_pDoc->getShowFormulaBar();
  editWidget()->showEditWidget( active );

  QString zoomStr( i18n("%1%").arg( m_pDoc->zoom() ) );
  d->actions->viewZoom->setCurrentItem( d->actions->viewZoom->items().findIndex( zoomStr ) );

  int posFrame = 30;
  if ( active )
    posWidget()->show();
  else
  {
    posWidget()->hide();
    posFrame = 0;
  }

  m_pToolWidget->show();

  // If this value (30) is changed then topBorder() needs to
  // be changed, too.
  m_pToolWidget->setGeometry( 0, 0, width(), /*30*/posFrame );
  int top = /*30*/posFrame;

  int widthVScrollbar  = m_pVertScrollBar->sizeHint().width();// 16;
  int heightHScrollbar = m_pHorzScrollBar->sizeHint().height();

  int left = 0;
  if ( table->isRightToLeft() && m_pDoc->getShowVerticalScrollBar() )
    left = widthVScrollbar;

  if (!m_pDoc->getShowHorizontalScrollBar())
    m_pTabBar->setGeometry( left, height() - heightHScrollbar,
                            width(), heightHScrollbar );
  else
    m_pTabBar->setGeometry( left, height() - heightHScrollbar,
                            width() / 2, heightHScrollbar );
  if ( m_pDoc->getShowTabBar() )
    m_pTabBar->show();
  else
    m_pTabBar->hide();

  // David's suggestion: move the scrollbars to KSpreadCanvas, but keep those resize statements
  if ( m_pDoc->getShowHorizontalScrollBar() )
    m_pHorzScrollBar->show();
  else
    m_pHorzScrollBar->hide();

  left = 0;
  if ( !activeTable()->isRightToLeft() )
    left = width() - widthVScrollbar;

  if ( !m_pDoc->getShowTabBar() && !m_pDoc->getShowHorizontalScrollBar())
    m_pVertScrollBar->setGeometry( left,
                                   top,
                                   widthVScrollbar,
                                   height() - top );
  else
    m_pVertScrollBar->setGeometry( left,
                                   top,
                                   widthVScrollbar,
                                   height() - heightHScrollbar - top );
  m_pVertScrollBar->setSteps( 20 /*linestep*/, m_pVertScrollBar->height() /*pagestep*/);

  if ( m_pDoc->getShowVerticalScrollBar() )
    m_pVertScrollBar->show();
  else
  {
    widthVScrollbar = 0;
    m_pVertScrollBar->hide();
  }

  int widthRowHeader = int( m_pCanvas->doc()->zoomItX( YBORDER_WIDTH ) );
  if ( m_pDoc->getShowRowHeader() )
    m_pVBorderWidget->show();
  else
  {
    widthRowHeader = 0;
    m_pVBorderWidget->hide();
  }

  int heightColHeader = int( m_pCanvas->doc()->zoomItY( KSpreadFormat::globalRowHeight() + 2 ) );
  if ( m_pDoc->getShowColHeader() )
    m_pHBorderWidget->show();
  else
  {
    heightColHeader = 0;
    m_pHBorderWidget->hide();
  }

  if ( statusBar() )
  {
    if ( m_pDoc->getShowStatusBar() )
      statusBar()->show();
    else
      statusBar()->hide();
  }

  if ( table->isRightToLeft() )
  {
    if ( !m_pDoc->getShowTabBar() && !m_pDoc->getShowHorizontalScrollBar() )
      m_pFrame->setGeometry( widthVScrollbar, top, width() - widthVScrollbar, height() - top - heightHScrollbar);
    else
      m_pFrame->setGeometry( widthVScrollbar, top, width() - widthVScrollbar,
                             height() - heightHScrollbar - top );

    m_pHorzScrollBar->setGeometry( width() / 2 + widthVScrollbar,
                                   height() - heightHScrollbar,
                                   width() / 2 - widthVScrollbar,
                                   heightHScrollbar );
    m_pHorzScrollBar->setSteps( 20 /*linestep*/, m_pHorzScrollBar->width() /*pagestep*/);
  }
  else
  {
    if ( !m_pDoc->getShowTabBar() && !m_pDoc->getShowHorizontalScrollBar() )
      m_pFrame->setGeometry( 0, top, width() - widthVScrollbar, height() - top - heightHScrollbar);
    else
      m_pFrame->setGeometry( 0, top, width() - widthVScrollbar,
                             height() - heightHScrollbar - top );
    m_pHorzScrollBar->setGeometry( width() / 2,
                                   height() - heightHScrollbar,
                                   width() / 2 - widthVScrollbar,
                                   heightHScrollbar );
    m_pHorzScrollBar->setSteps( 20 /*linestep*/, m_pHorzScrollBar->width() /*pagestep*/);
  }

  m_pFrame->show();

  if ( !table->isRightToLeft() )
    m_pCanvas->setGeometry( widthRowHeader, heightColHeader,
                            m_pFrame->width() - widthRowHeader, m_pFrame->height() - heightColHeader );
  else
    m_pCanvas->setGeometry( 0, heightColHeader,
                          m_pFrame->width() - widthRowHeader - 1.0, m_pFrame->height() - heightColHeader );

  m_pCanvas->updatePosWidget();

  left = 0;
  if ( table->isRightToLeft() )
  {
    m_pHBorderWidget->setGeometry( 1.0, 0,
                                   m_pFrame->width() - widthRowHeader + 2.0, heightColHeader );

    left = width() - widthRowHeader - widthVScrollbar;
  }
  else
    m_pHBorderWidget->setGeometry( widthRowHeader + 1, 0,
                                   m_pFrame->width() - widthRowHeader, heightColHeader );

  m_pVBorderWidget->setGeometry( left, heightColHeader + 1, widthRowHeader,
                                 m_pFrame->height() - heightColHeader );
}

void KSpreadView::resizeEvent( QResizeEvent * )
{
  refreshView();
}

void KSpreadView::popupChildMenu( KoChild* child, const QPoint& global_pos )
{
    if ( !child )
	return;

    delete m_popupChild;

    m_popupChildObject = static_cast<KSpreadChild*>(child);

    m_popupChild = new QPopupMenu( this );

    m_popupChild->insertItem( i18n("Delete Embedded Document"), this, SLOT( slotPopupDeleteChild() ) );

    m_popupChild->popup( global_pos );
}

void KSpreadView::slotPopupDeleteChild()
{
    if ( !m_popupChildObject || !m_popupChildObject->table() )
	return;
    int ret = KMessageBox::warningYesNo(this,i18n("You are about to remove this embedded document.\nDo you want to continue?"),i18n("Delete Embedded Document"));
    if ( ret == KMessageBox::Yes )
    {
      m_pDoc->emitBeginOperation(false);
      m_popupChildObject->table()->deleteChild( m_popupChildObject );
      m_popupChildObject = 0;
      m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
    }
}

void KSpreadView::popupColumnMenu( const QPoint & _point )
{
  assert( m_pTable );

  if ( !koDocument()->isReadWrite() )
    return;

    delete m_pPopupColumn ;

    m_pPopupColumn = new QPopupMenu( this );

    bool isProtected = m_pTable->isProtected();

    if ( !isProtected )
    {
      d->actions->cellLayout->plug( m_pPopupColumn );
      m_pPopupColumn->insertSeparator();
      d->actions->cut->plug( m_pPopupColumn );
    }
    d->actions->copy->plug( m_pPopupColumn );
    if ( !isProtected )
    {
      d->actions->paste->plug( m_pPopupColumn );
      d->actions->specialPaste->plug( m_pPopupColumn );
      d->actions->insertCellCopy->plug( m_pPopupColumn );
      m_pPopupColumn->insertSeparator();
      d->actions->defaultFormat->plug( m_pPopupColumn );
      // If there is no selection
      if ((util_isRowSelected(selection()) == FALSE) && (util_isColumnSelected(selection()) == FALSE) )
      {
        d->actions->areaName->plug( m_pPopupColumn );
      }

      d->actions->resizeColumn->plug( m_pPopupColumn );
      m_pPopupColumn->insertItem( i18n("Adjust Column"), this, SLOT(slotPopupAdjustColumn() ) );
      m_pPopupColumn->insertSeparator();
      d->actions->insertColumn->plug( m_pPopupColumn );
      d->actions->deleteColumn->plug( m_pPopupColumn );
      d->actions->hideColumn->plug( m_pPopupColumn );

      d->actions->showSelColumns->setEnabled(false);

      int i;
      ColumnFormat * col;
      QRect rect = m_selectionInfo->selection();
      //kdDebug(36001) << "Column: L: " << rect.left() << endl;
      for ( i = rect.left(); i <= rect.right(); ++i )
      {
        if (i == 2) // "B"
        {
          col = activeTable()->columnFormat( 1 );
          if ( col->isHide() )
          {
            d->actions->showSelColumns->setEnabled(true);
            d->actions->showSelColumns->plug( m_pPopupColumn );
            break;
          }
        }

        col = activeTable()->columnFormat( i );

        if ( col->isHide() )
        {
          d->actions->showSelColumns->setEnabled( true );
          d->actions->showSelColumns->plug( m_pPopupColumn );
          break;
        }
      }
    }

    QObject::connect( m_pPopupColumn, SIGNAL(activated( int ) ), this, SLOT( slotActivateTool( int ) ) );

    m_pPopupColumn->popup( _point );
}

void KSpreadView::slotPopupAdjustColumn()
{
    if ( !m_pTable )
       return;

    m_pDoc->emitBeginOperation( false );
    canvasWidget()->adjustArea();
    m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::popupRowMenu( const QPoint & _point )
{
    assert( m_pTable );

    if ( !koDocument()->isReadWrite() )
      return;

    delete m_pPopupRow ;

    m_pPopupRow= new QPopupMenu();

    bool isProtected = m_pTable->isProtected();

    if ( !isProtected )
    {
        d->actions->cellLayout->plug( m_pPopupRow );
        m_pPopupRow->insertSeparator();
        d->actions->cut->plug( m_pPopupRow );
    }
    d->actions->copy->plug( m_pPopupRow );
    if ( !isProtected )
    {
      d->actions->paste->plug( m_pPopupRow );
      d->actions->specialPaste->plug( m_pPopupRow );
      d->actions->insertCellCopy->plug( m_pPopupRow );
      m_pPopupRow->insertSeparator();
      d->actions->defaultFormat->plug( m_pPopupRow );
      // If there is no selection
      if ( (util_isRowSelected(selection()) == FALSE) && (util_isColumnSelected(selection()) == FALSE) )
      {
	d->actions->areaName->plug( m_pPopupRow );
      }

      d->actions->resizeRow->plug( m_pPopupRow );
      m_pPopupRow->insertItem( i18n("Adjust Row"), this, SLOT( slotPopupAdjustRow() ) );
      m_pPopupRow->insertSeparator();
      d->actions->insertRow->plug( m_pPopupRow );
      d->actions->deleteRow->plug( m_pPopupRow );
      d->actions->hideRow->plug( m_pPopupRow );

      d->actions->showSelColumns->setEnabled(false);

      int i;
      RowFormat * row;
      QRect rect = m_selectionInfo->selection();
      for ( i = rect.top(); i <= rect.bottom(); ++i )
      {
        //kdDebug(36001) << "popupRow: " << rect.top() << endl;
        if (i == 2)
        {
          row = activeTable()->rowFormat( 1 );
          if ( row->isHide() )
          {
            d->actions->showSelRows->setEnabled(true);
            d->actions->showSelRows->plug( m_pPopupRow );
            break;
          }
        }

        row = activeTable()->rowFormat( i );
        if ( row->isHide() )
        {
          d->actions->showSelRows->setEnabled(true);
          d->actions->showSelRows->plug( m_pPopupRow );
          break;
        }
      }
    }

    QObject::connect( m_pPopupRow, SIGNAL( activated( int ) ), this, SLOT( slotActivateTool( int ) ) );
    m_pPopupRow->popup( _point );
}

void KSpreadView::slotPopupAdjustRow()
{
    if ( !m_pTable )
       return;

    m_pDoc->emitBeginOperation(false);
    canvasWidget()->adjustArea();
    m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}


void KSpreadView::slotListChoosePopupMenu( )
{
  if ( !koDocument()->isReadWrite() )
    return;

  assert( m_pTable );
  delete m_popupListChoose;

  m_popupListChoose = new QPopupMenu();
  int id = 0;
  QRect selection( m_selectionInfo->selection() );
  KSpreadCell * cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
  QString tmp = cell->text();
  QStringList itemList;

  for ( int col = selection.left(); col <= selection.right(); ++col )
  {
    KSpreadCell * c = m_pTable->getFirstCellColumn( col );
    while ( c )
    {
      if ( !c->isObscuringForced()
           && !( col == m_pCanvas->markerColumn()
                 && c->row() == m_pCanvas->markerRow()) )
      {
        if ( c->value().isString() && c->text() != tmp && !c->text().isEmpty() )
        {
          if ( itemList.findIndex( c->text() ) == -1 )
            itemList.append(c->text());
        }
      }

      c = m_pTable->getNextCellDown( col, c->row() );
    }
  }

  /* TODO: remove this later:
    for( ;c; c = c->nextCell() )
   {
     int col = c->column();
     if ( selection.left() <= col && selection.right() >= col
	  &&!c->isObscuringForced()&& !(col==m_pCanvas->markerColumn()&& c->row()==m_pCanvas->markerRow()))
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
    m_popupListChoose->insertItem( (*it), id++ );

  if ( id == 0 )
    return;
  RowFormat * rl = m_pTable->rowFormat( m_pCanvas->markerRow());
  double tx = m_pTable->dblColumnPos( m_pCanvas->markerColumn(), m_pCanvas );
  double ty = m_pTable->dblRowPos(m_pCanvas->markerRow(), m_pCanvas );
  double h = rl->dblHeight( m_pCanvas );
  if ( cell->extraYCells() )
    h = cell->extraHeight();
  ty += h;

  QPoint p( (int)tx, (int)ty );
  QPoint p2 = m_pCanvas->mapToGlobal( p );
  m_popupListChoose->popup( p2 );
  QObject::connect( m_popupListChoose, SIGNAL( activated( int ) ),
                    this, SLOT( slotItemSelected( int ) ) );
}


void KSpreadView::slotItemSelected( int id )
{
  QString tmp = m_popupListChoose->text( id );
  int x = m_pCanvas->markerColumn();
  int y = m_pCanvas->markerRow();
  KSpreadCell * cell = m_pTable->nonDefaultCell( x, y );

  if ( tmp == cell->text() )
    return;

  m_pDoc->emitBeginOperation( false );

  if ( !m_pDoc->undoBuffer()->isLocked() )
  {
    KSpreadUndoSetText* undo = new KSpreadUndoSetText( m_pDoc, m_pTable, cell->text(),
                                                       x, y, cell->formatType() );
    m_pDoc->undoBuffer()->appendUndo( undo );
  }

  cell->setCellText( tmp, true );
  editWidget()->setText( tmp );

  m_pDoc->emitEndOperation( QRect( x, y, 1, 1 ) );
}

void KSpreadView::openPopupMenu( const QPoint & _point )
{
    assert( m_pTable );
    delete m_pPopupMenu;

    if ( !koDocument()->isReadWrite() )
        return;

    m_pPopupMenu = new QPopupMenu();
    KSpreadCell * cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );

    bool isProtected = m_pTable->isProtected();
    if ( !cell->isDefault() && cell->notProtected( m_pCanvas->markerColumn(), m_pCanvas->markerRow() )
         && ( selection().width() == 1 ) && ( selection().height() == 1 ) )
      isProtected = false;

    if ( !isProtected )
    {
      d->actions->cellLayout->plug( m_pPopupMenu );
      m_pPopupMenu->insertSeparator();
      d->actions->cut->plug( m_pPopupMenu );
    }
    d->actions->copy->plug( m_pPopupMenu );
    if ( !isProtected )
      d->actions->paste->plug( m_pPopupMenu );

    if ( !isProtected )
    {
      d->actions->specialPaste->plug( m_pPopupMenu );
      d->actions->insertCellCopy->plug( m_pPopupMenu );
      m_pPopupMenu->insertSeparator();
      d->actions->del->plug( m_pPopupMenu );
      d->actions->adjust->plug( m_pPopupMenu );
      d->actions->defaultFormat->plug( m_pPopupMenu );

      // If there is no selection
      if ( (util_isRowSelected(selection()) == FALSE) && (util_isColumnSelected(selection()) == FALSE) )
      {
        d->actions->areaName->plug( m_pPopupMenu );
        m_pPopupMenu->insertSeparator();
        d->actions->insertCell->plug( m_pPopupMenu );
        d->actions->removeCell->plug( m_pPopupMenu );
      }

      m_pPopupMenu->insertSeparator();
      d->actions->addModifyComment->plug( m_pPopupMenu );
      if ( !cell->comment(m_pCanvas->markerColumn(), m_pCanvas->markerRow()).isEmpty() )
      {
        d->actions->removeComment->plug( m_pPopupMenu );
      }

      if (activeTable()->testListChoose(selectionInfo()))
      {
	m_pPopupMenu->insertSeparator();
	m_pPopupMenu->insertItem( i18n("Selection List..."), this, SLOT( slotListChoosePopupMenu() ) );
      }
    }

    // Remove informations about the last tools we offered
    m_lstTools.clear();
    m_lstTools.setAutoDelete( true );

    if ( !isProtected && !activeTable()->getWordSpelling( selectionInfo() ).isEmpty() )
    {
      m_popupMenuFirstToolId = 10;
      int i = 0;
      QValueList<KDataToolInfo> tools = KDataToolInfo::query( "QString", "text/plain", m_pDoc->instance() );
      if ( tools.count() > 0 )
      {
        m_pPopupMenu->insertSeparator();
        QValueList<KDataToolInfo>::Iterator entry = tools.begin();
        for( ; entry != tools.end(); ++entry )
        {
          QStringList lst = (*entry).userCommands();
          QStringList::ConstIterator it = lst.begin();

          // ### Torben: Insert pixmaps here, too
          for (; it != lst.end(); ++it )
            m_pPopupMenu->insertItem( *it, m_popupMenuFirstToolId + i++ );

          lst = (*entry).commands();
          it = lst.begin();
          for (; it != lst.end(); ++it )
          {
            ToolEntry *t = new ToolEntry;
            t->command = *it;
            t->info = *entry;
            m_lstTools.append( t );
          }
        }

        QObject::connect( m_pPopupMenu, SIGNAL( activated( int ) ), this, SLOT( slotActivateTool( int ) ) );
      }
    }

    m_pPopupMenu->popup( _point );
}

void KSpreadView::slotActivateTool( int _id )
{
  Q_ASSERT( m_pTable );

  // Is it the id of a tool in the latest popupmenu ?
  if ( _id < m_popupMenuFirstToolId )
    return;

  ToolEntry* entry = m_lstTools.at( _id - m_popupMenuFirstToolId );

  KDataTool* tool = entry->info.createTool();
  if ( !tool )
  {
      kdDebug(36001) << "Could not create Tool" << endl;
      return;
  }

  QString text = activeTable()->getWordSpelling( selectionInfo() );

  if ( tool->run( entry->command, &text, "QString", "text/plain") )
  {
      m_pDoc->emitBeginOperation(false);

      activeTable()->setWordSpelling( selectionInfo(), text);

      KSpreadCell *cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
      editWidget()->setText( cell->text() );

      m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
  }
}

void KSpreadView::deleteSelection()
{
    Q_ASSERT( m_pTable );

    m_pDoc->emitBeginOperation( false );
    m_pTable->deleteSelection( selectionInfo() );
    resultOfCalc();
    updateEditWidget();
    endOperation( selectionInfo()->selection() );
}

void KSpreadView::adjust()
{
    if ( (util_isRowSelected(selection())) || (util_isColumnSelected(selection())) )
    {
      KMessageBox::error( this, i18n("Area too large!"));
    }
    else
    {
      m_pDoc->emitBeginOperation( false );
      canvasWidget()->adjustArea();
      endOperation( selection() );
    }
}

void KSpreadView::clearTextSelection()
{
    Q_ASSERT( m_pTable );
    m_pDoc->emitBeginOperation( false );
    m_pTable->clearTextSelection( selectionInfo() );

    updateEditWidget();
    m_pDoc->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::clearCommentSelection()
{
    Q_ASSERT( m_pTable );
    m_pDoc->emitBeginOperation( false );
    m_pTable->setSelectionRemoveComment( selectionInfo() );

    updateEditWidget();
    m_pDoc->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::clearValiditySelection()
{
    Q_ASSERT( m_pTable );
    m_pDoc->emitBeginOperation( false );
    m_pTable->clearValiditySelection( selectionInfo() );

    updateEditWidget();
    m_pDoc->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::clearConditionalSelection()
{
    Q_ASSERT( m_pTable );
    m_pDoc->emitBeginOperation( false );
    m_pTable->clearConditionalSelection( selectionInfo() );

    updateEditWidget();
    m_pDoc->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::fillRight()
{
  Q_ASSERT( m_pTable );
  m_pDoc->emitBeginOperation( false );
  m_pTable->fillSelection( selectionInfo(), KSpreadSheet::Right );
  m_pDoc->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::fillLeft()
{
  Q_ASSERT( m_pTable );
  m_pDoc->emitBeginOperation( false );
  m_pTable->fillSelection( selectionInfo(), KSpreadSheet::Left );
  m_pDoc->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::fillUp()
{
  Q_ASSERT( m_pTable );
  m_pDoc->emitBeginOperation( false );
  m_pTable->fillSelection( selectionInfo(), KSpreadSheet::Up );
  m_pDoc->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::fillDown()
{
  Q_ASSERT( m_pTable );
  m_pDoc->emitBeginOperation( false );
  m_pTable->fillSelection( selectionInfo(), KSpreadSheet::Down );
  m_pDoc->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::defaultSelection()
{
  Q_ASSERT( m_pTable );
  m_pDoc->emitBeginOperation( false );
  m_pTable->defaultSelection( selectionInfo() );

  updateEditWidget();
  m_pDoc->emitEndOperation( selectionInfo()->selection() );
}

void KSpreadView::slotInsert()
{
  QRect r( selection() );
  KSpreadinsert dlg( this, "Insert", r, KSpreadinsert::Insert );
  dlg.exec();
}

void KSpreadView::slotRemove()
{
  QRect r( m_selectionInfo->selection() );
  KSpreadinsert dlg( this, "Remove", r, KSpreadinsert::Remove );
  dlg.exec();
}

void KSpreadView::slotInsertCellCopy()
{
  if ( !m_pTable )
    return;

  if ( !m_pTable->testAreaPasteInsert() )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->paste( selection(), true, Normal, OverWrite, true );
    m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
  }
  else
  {
    KSpreadpasteinsert dlg( this, "Remove", selection() );
    dlg.exec();
  }

  if ( m_pTable->getAutoCalc() )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->recalc();
    m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
  }
  updateEditWidget();
}

void KSpreadView::setAreaName()
{
  KSpreadarea dlg( this, "Area Name",QPoint(m_pCanvas->markerColumn(), m_pCanvas->markerRow()) );
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
    KMessageBox::error( this, i18n("Area too large!"));
  else
  {
    KSpreadResizeRow dlg( this );
    dlg.exec();
  }
}

void KSpreadView::resizeColumn()
{
  if ( util_isRowSelected( selection() ) )
    KMessageBox::error( this, i18n( "Area too large!" ) );
  else
  {
    KSpreadResizeColumn dlg( this );
    dlg.exec();
  }
}

void KSpreadView::equalizeRow()
{
  if ( util_isColumnSelected( selection() ) )
    KMessageBox::error( this, i18n( "Area too large!" ) );
  else
  {
    m_pDoc->emitBeginOperation( false );
    canvasWidget()->equalizeRow();
    m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
  }
}

void KSpreadView::equalizeColumn()
{
  if ( util_isRowSelected( selection() ) )
    KMessageBox::error( this, i18n( "Area too large!" ) );
  else
  {
    m_pDoc->emitBeginOperation( false );
    canvasWidget()->equalizeColumn();
    m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
  }
}


void KSpreadView::layoutDlg()
{
  QRect selection( m_selectionInfo->selection() );

  if ( m_selectionInfo->singleCellSelection() )
  {
    CellFormatDlg dlg( this, m_pTable, selection.left(), selection.top(),
                       selection.left(), selection.top() );
  }
  else
    CellFormatDlg dlg( this, m_pTable, selection.left(), selection.top(),
                       selection.right(), selection.bottom() );
}

void KSpreadView::styleDialog()
{
  KSpreadStyleDlg dlg( this, m_pDoc->styleManager() );
  dlg.exec();

  d->actions->selectStyle->setItems( m_pDoc->styleManager()->styleNames() );
  if ( m_pTable )
  {
    m_pTable->setLayoutDirtyFlag();
    m_pTable->setRegionPaintDirty( m_pTable->visibleRect( m_pCanvas ) );
  }
  if ( m_pCanvas )
    m_pCanvas->repaint();
}

void KSpreadView::paperLayoutDlg()
{
  if ( m_pCanvas->editor() )
  {
    m_pCanvas->deleteEditor( true ); // save changes
  }
  KSpreadSheetPrint* print = m_pTable->print();

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

  KoUnit::Unit unit = doc()->getUnit();

  KSpreadPaperLayout * dlg
    = new KSpreadPaperLayout( this, "PageLayout", pl, hf,
                              FORMAT_AND_BORDERS | HEADER_AND_FOOTER,
                              unit, m_pTable, this );
  dlg->show();
  // dlg destroys itself
}

void KSpreadView::definePrintRange()
{
  m_pTable->print()->definePrintRange( selectionInfo() );
}

void KSpreadView::resetPrintRange()
{
  m_pTable->print()->resetPrintRange();
}

void KSpreadView::multiRow( bool b )
{
  if ( m_toolbarLock )
    return;

  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->setSelectionMultiRow( selectionInfo(), b );
    m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
  }
}

void KSpreadView::alignLeft( bool b )
{
  if ( m_toolbarLock )
    return;

  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    if ( !b )
      m_pTable->setSelectionAlign( selectionInfo(),
                                   KSpreadFormat::Undefined );
    else
      m_pTable->setSelectionAlign( selectionInfo(),
                                   KSpreadFormat::Left );
    endOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::alignRight( bool b )
{
  if ( m_toolbarLock )
    return;

  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    if ( !b )
      m_pTable->setSelectionAlign( selectionInfo(), KSpreadFormat::Undefined );
    else
      m_pTable->setSelectionAlign( selectionInfo(), KSpreadFormat::Right );

    endOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::alignCenter( bool b )
{
  if ( m_toolbarLock )
    return;

  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    if ( !b )
      m_pTable->setSelectionAlign( selectionInfo(), KSpreadFormat::Undefined );
    else
      m_pTable->setSelectionAlign( selectionInfo(), KSpreadFormat::Center );

    endOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::alignTop( bool b )
{
  if ( m_toolbarLock )
    return;

  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    if ( !b )
      m_pTable->setSelectionAlignY( selectionInfo(), KSpreadFormat::UndefinedY );
    else
      m_pTable->setSelectionAlignY( selectionInfo(), KSpreadFormat::Top );

    endOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::alignBottom( bool b )
{
  if ( m_toolbarLock )
    return;

  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    if ( !b )
      m_pTable->setSelectionAlignY( selectionInfo(), KSpreadFormat::UndefinedY );
    else
      m_pTable->setSelectionAlignY( selectionInfo(), KSpreadFormat::Bottom );

    endOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::alignMiddle( bool b )
{
  if ( m_toolbarLock )
    return;

  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation( false );
    if ( !b )
      m_pTable->setSelectionAlignY( selectionInfo(), KSpreadFormat::UndefinedY );
    else
      m_pTable->setSelectionAlignY( selectionInfo(), KSpreadFormat::Middle );

    endOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::moneyFormat(bool b)
{
  if ( m_toolbarLock )
    return;

  m_pDoc->emitBeginOperation( false );
  if ( m_pTable != 0L )
    m_pTable->setSelectionMoneyFormat( selectionInfo(), b );
  updateEditWidget();
  endOperation( selectionInfo()->selection() );
}

void KSpreadView::createStyleFromCell()
{
  if ( !m_pTable )
    return;

  QPoint p( m_selectionInfo->selection().topLeft() );
  KSpreadCell * cell = m_pTable->nonDefaultCell( p.x(), p.y() );

  bool ok = false;
  QString styleName( "" );

  while( true )
  {
    styleName = KLineEditDlg::getText( i18n( "Create Style From Cell" ),
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

    if ( m_pDoc->styleManager()->style( styleName ) != 0 )
    {
      KNotifyClient::beep();
      KMessageBox::sorry( this, i18n( "A style with this name already exists." ) );
      continue;
    }
    break;
  }

  KSpreadCustomStyle * style = new KSpreadCustomStyle( cell->kspreadStyle(), styleName );

  m_pDoc->styleManager()->m_styles[ styleName ] = style;
  cell->setKSpreadStyle( style );
  QStringList lst( d->actions->selectStyle->items() );
  lst.push_back( styleName );
  d->actions->selectStyle->setItems( lst );
}

void KSpreadView::styleSelected( const QString & style )
{
  if (m_pTable )
  {
    KSpreadStyle * s = m_pDoc->styleManager()->style( style );

    if ( s )
    {
      m_pDoc->emitBeginOperation(false);
      m_pTable->setSelectionStyle( selectionInfo(), s );
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
  if ( m_pTable != NULL )
  {
    m_pDoc->emitBeginOperation( false );
    m_pTable->setSelectionPrecision( selectionInfo(), delta );
    endOperation( selectionInfo()->selection() );
  }
}

void KSpreadView::percent( bool b )
{
  if ( m_toolbarLock )
    return;

  m_pDoc->emitBeginOperation( false );
  if ( m_pTable != 0L )
    m_pTable->setSelectionPercent( selectionInfo() ,b );
  updateEditWidget();

  endOperation( selectionInfo()->selection() );
}

void KSpreadView::insertObject()
{
  m_pDoc->emitBeginOperation( false );
  KoDocumentEntry e =  d->actions->insertPart->documentEntry();//KoPartSelectDia::selectPart( m_pCanvas );
  if ( e.isEmpty() )
  {
    m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
    return;
  }

  //Don't start handles more than once
  if ( m_pInsertHandle )
    delete m_pInsertHandle;

  m_pInsertHandle = new KSpreadInsertHandler( this, m_pCanvas, e );
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::insertChart()
{
  if ( util_isColumnSelected(selection()) || util_isRowSelected(selection()) )
  {
    KMessageBox::error( this, i18n("Area too large!"));
    return;
  }
  QValueList<KoDocumentEntry> vec = KoDocumentEntry::query( true, "'KOfficeChart' in ServiceTypes" );
  if ( vec.isEmpty() )
  {
    KMessageBox::error( this, i18n("No charting component registered") );
    return;
  }

  //Don't start handles more than once
  if ( m_pInsertHandle )
    delete m_pInsertHandle;

  m_pDoc->emitBeginOperation( false );

  m_pInsertHandle = new KSpreadInsertHandler( this, m_pCanvas, vec[0], TRUE );
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}


/*
  // TODO Use KoView setScaling/xScaling/yScaling instead
void KSpreadView::zoomMinus()
{
  if ( m_fZoom <= 0.25 )
    return;

  m_fZoom -= 0.25;

  if ( m_pTable != 0L )
    m_pTable->setLayoutDirtyFlag();

  m_pCanvas->repaint();
  m_pVBorderWidget->repaint();
  m_pHBorderWidget->repaint();
}

void KSpreadView::zoomPlus()
{
  if ( m_fZoom >= 3 )
    return;

  m_fZoom += 0.25;

  if ( m_pTable != 0L )
    m_pTable->setLayoutDirtyFlag();

  m_pCanvas->repaint();
  m_pVBorderWidget->repaint();
  m_pHBorderWidget->repaint();
}
*/

void KSpreadView::removeTable()
{
  if ( doc()->map()->count() <= 1 || ( doc()->map()->visibleSheets().count() <= 1 ) )
  {
    KNotifyClient::beep();
    KMessageBox::sorry( this, i18n("You cannot delete the only sheet."), i18n("Remove Sheet") ); // FIXME bad english? no english!
    return;
  }
  KNotifyClient::beep();
  int ret = KMessageBox::warningYesNo( this, i18n( "You are about to remove the active sheet.\nDo you want to continue?" ),
                                       i18n( "Remove Sheet" ) );

  if ( ret == KMessageBox::Yes )
  {
    m_pDoc->emitBeginOperation( false );
    if ( m_pCanvas->editor() )
    {
      m_pCanvas->deleteEditor( false );
    }
    m_pDoc->setModified( true );
    KSpreadSheet * tbl = activeTable();
    KSpreadUndoRemoveTable * undo = new KSpreadUndoRemoveTable( m_pDoc, tbl );
    m_pDoc->undoBuffer()->appendUndo( undo );
    tbl->map()->takeTable( tbl );
    doc()->takeTable( tbl );
    m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
  }
}


void KSpreadView::slotRename()
{

  KSpreadSheet * table = activeTable();

  bool ok;
  QString activeName = table->tableName();
  QString newName = KLineEditDlg::getText( i18n("Rename Sheet"),i18n("Enter name:"), activeName, &ok, this );

  if( !ok ) return;

  while (!util_validateTableName(newName))
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

    newName = KLineEditDlg::getText( i18n("Rename Sheet"),i18n("Enter name:"), newName, &ok, this );

    if ( !ok ) return;
  }

  if ( (newName.stripWhiteSpace()).isEmpty() ) // Table name is empty.
  {
    KNotifyClient::beep();
    KMessageBox::information( this, i18n("Sheet name cannot be empty."), i18n("Change Sheet Name") );
    // Recursion
    slotRename();
  }
  else if ( newName != activeName ) // Table name changed.
  {
    // Is the name already used
    if ( !table->setTableName( newName ) )
    {
      KNotifyClient::beep();
      KMessageBox::information( this, i18n("This name is already used."), i18n("Change Sheet Name") );
      // Recursion
      slotRename();
      return;
    }

    m_pDoc->emitBeginOperation(false);
    updateEditWidget();
    doc()->setModified( true );
    m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
  }
}

void KSpreadView::setText( const QString & _text )
{
  if ( m_pTable == 0L )
    return;

  int x = m_pCanvas->markerColumn();
  int y = m_pCanvas->markerRow();

  m_pDoc->emitBeginOperation( false );
  m_pTable->setText( y, x, _text );
  KSpreadCell * cell = m_pTable->cellAt( x, y );

  if ( cell->value().isString() && !_text.isEmpty() && !_text.at(0).isDigit() && !cell->isFormula() )
    m_pDoc->addStringCompletion( _text );

  m_pDoc->emitEndOperation( QRect( x, y, 1, 1 ) );
}

//------------------------------------------------
//
// Document signals
//
//------------------------------------------------

void KSpreadView::slotAddTable( KSpreadSheet *_table )
{
  addTable( _table );
}

void KSpreadView::slotRefreshView()
{
  refreshView();
  m_pCanvas->repaint();
  m_pVBorderWidget->repaint();
  m_pHBorderWidget->repaint();
}

void KSpreadView::slotUpdateView( KSpreadSheet *_table )
{
  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  //  m_pDoc->emitBeginOperation( false );

  //  m_pTable->setRegionPaintDirty(QRect(QPoint(0,0),
  //                                      QPoint(KS_colMax, KS_rowMax)));

  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::slotUpdateView( KSpreadSheet * _table, const QRect & _rect )
{
  // qDebug("void KSpreadView::slotUpdateView( KSpreadSheet *_table, const QRect& %i %i|%i %i )\n",_rect.left(),_rect.top(),_rect.right(),_rect.bottom());

  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  // m_pDoc->emitBeginOperation( false );
  m_pTable->setRegionPaintDirty( _rect );
  endOperation( _rect );
}

void KSpreadView::slotUpdateHBorder( KSpreadSheet * _table )
{
  // kdDebug(36001)<<"void KSpreadView::slotUpdateHBorder( KSpreadSheet *_table )\n";

  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  m_pDoc->emitBeginOperation(false);
  m_pHBorderWidget->update();
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::slotUpdateVBorder( KSpreadSheet *_table )
{
  // kdDebug("void KSpreadView::slotUpdateVBorder( KSpreadSheet *_table )\n";

  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  m_pDoc->emitBeginOperation( false );
  m_pVBorderWidget->update();
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::slotChangeSelection( KSpreadSheet *_table,
                                       const QRect &oldSelection,
                                       const QPoint& /* oldMarker*/ )
{
  m_pDoc->emitBeginOperation( false );
  QRect newSelection = m_selectionInfo->selection();

  // Emit a signal for internal use
  emit sig_selectionChanged( _table, newSelection );

  // Empty selection ?
  // Activate or deactivate some actions.
  bool colSelected = util_isColumnSelected( selection() );
  bool rowSelected = util_isRowSelected( selection() );

  if ( m_pTable && !m_pTable->isProtected() )
  {
    d->actions->resizeRow->setEnabled( !colSelected );
    d->actions->equalizeRow->setEnabled( !colSelected );
    d->actions->validity->setEnabled( !colSelected && !rowSelected);
    d->actions->conditional->setEnabled( !colSelected && !rowSelected);
    d->actions->resizeColumn->setEnabled( !rowSelected );
    d->actions->equalizeColumn->setEnabled( !rowSelected );
    d->actions->textToColumns->setEnabled( !rowSelected );

    bool simpleSelection = m_selectionInfo->singleCellSelection()
      || colSelected || rowSelected;
    d->actions->tableFormat->setEnabled( !simpleSelection );
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
  KSpreadSelectionChanged ev( newSelection, activeTable()->name() );
  QApplication::sendEvent( this, &ev );

  // Do we display this table ?
  if ( _table != m_pTable )
  {
    m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
    return;
  }

  m_pCanvas->setSelectionChangePaintDirty( m_pTable, oldSelection, newSelection );

  m_pVBorderWidget->update();
  m_pHBorderWidget->update();
  m_pDoc->emitEndOperation( newSelection );
}

void KSpreadView::resultOfCalc()
{
  KSpreadSheet * table = activeTable();
  double result = 0.0;
  int nbCell = 0;
  QRect tmpRect(m_selectionInfo->selection());
  MethodOfCalc tmpMethod = m_pDoc->getTypeOfCalc() ;
  if ( tmpMethod != NoneCalc )
  {
    if ( util_isColumnSelected(selection()) )
    {
      for ( int col = tmpRect.left(); col <= tmpRect.right(); ++col )
      {
        KSpreadCell * c = table->getFirstCellColumn( col );
        while ( c )
        {
          if ( !c->isObscuringForced() )
          {
            if ( c->value().isNumber() )
            {
              double val = c->value().asFloat();
              switch( tmpMethod )
              {
               case SumOfNumber:
                result += val;
                break;
               case Average:
                result += val;
                break;
               case Min:
                if (result != 0)
                  result = QMIN(val, result);
                else
                  result = val;
                break;
               case Max:
                if (result != 0)
                  result = QMAX(val, result);
                else
                  result = val;
                break;
               case Count:
               case NoneCalc:
                break;
               default:
                break;
              }
              ++nbCell;
            }
          }
          c = table->getNextCellDown( col, c->row() );
        }
      }
    }
    else if ( util_isRowSelected(selection()) )
    {
      for ( int row = tmpRect.top(); row <= tmpRect.bottom(); ++row )
      {
        KSpreadCell * c = table->getFirstCellRow( row );
        while ( c )
        {
          if ( !c->isObscuringForced() && c->value().isNumber() )
          {
            double val = c->value().asFloat();
            switch(tmpMethod )
            {
             case SumOfNumber:
              result += val;
              break;
             case Average:
              result += val;
              break;
             case Min:
              if (result != 0)
                result = QMIN(val, result);
              else
                result = val;
              break;
             case Max:
              if (result != 0)
                result = QMAX(val, result);
              else
                result = val;
              break;
             case Count:
             case NoneCalc:
              break;
             default:
              break;
            }
            ++nbCell;
          }
          c = table->getNextCellRight( c->column(), row );
        }
      }
    }
    else
    {
      int right  = tmpRect.right();
      int bottom = tmpRect.bottom();
      KSpreadCell * cell;

      for ( int i = tmpRect.left(); i <= right; ++i )
        for(int j = tmpRect.top(); j <= bottom; ++j )
        {
          cell = activeTable()->cellAt( i, j );
          if ( !cell->isDefault() && cell->value().isNumber() )
          {
            double val = cell->value().asFloat();
            switch(tmpMethod )
            {
             case SumOfNumber:
              result += val;
              break;
             case Average:
              result += val;
              break;
             case Min:
              if (result != 0)
                result = QMIN(val, result);
              else
                result = val;
              break;
             case Max:
              if (result != 0)
                result = QMAX(val,result);
              else
                result = val;
              break;
             case Count:
             case NoneCalc:
              break;
             default:
              break;
            }
            ++nbCell;
          }
        }
    }
  }
  QString tmp;
  switch(tmpMethod )
  {
   case SumOfNumber:
    tmp = i18n(" Sum: %1").arg(result);
    break;
   case Average:
    result = result/nbCell;
    tmp = i18n("Average: %1").arg(result);
    break;
   case Min:
    tmp = i18n("Min: %1").arg(result);
    break;
   case Max:
    tmp = i18n("Max: %1").arg(result);
    break;
   case Count:
    tmp = i18n("Count: %1").arg(nbCell);
    break;
   case NoneCalc:
    tmp = "";
    break;
  }

  //m_pDoc->emitBeginOperation();
  if ( m_sbCalcLabel )
    m_sbCalcLabel->setText(QString(" ") + tmp + ' ');
  //m_pDoc->emitEndOperation();
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
  m_pDoc->emitBeginOperation(false);
  if ( d->actions->menuCalcMin->isChecked() )
  {
    doc()->setTypeOfCalc( Min );
  }
  else if ( d->actions->menuCalcMax->isChecked() )
  {
    doc()->setTypeOfCalc( Max );
  }
  else if ( d->actions->menuCalcCount->isChecked() )
  {
    doc()->setTypeOfCalc( Count );
  }
  else if ( d->actions->menuCalcAverage->isChecked() )
  {
    doc()->setTypeOfCalc( Average );
  }
  else if ( d->actions->menuCalcSum->isChecked() )
  {
    doc()->setTypeOfCalc( SumOfNumber );
  }
  else if ( d->actions->menuCalcNone->isChecked() )
    doc()->setTypeOfCalc( NoneCalc );

  resultOfCalc();

  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}


QWMatrix KSpreadView::matrix() const
{
  QWMatrix m;
  m.scale( m_pDoc->zoomedResolutionX(),
           m_pDoc->zoomedResolutionY() );
  m.translate( - m_pCanvas->xOffset(), - m_pCanvas->yOffset() );
  return m;
}

void KSpreadView::transformPart()
{
    Q_ASSERT( selectedChild() );

    if ( m_transformToolBox.isNull() )
    {
        m_transformToolBox = new KoTransformToolBox( selectedChild(), topLevelWidget() );
        m_transformToolBox->show();

        m_transformToolBox->setDocumentChild( selectedChild() );
    }
    else
    {
        m_transformToolBox->show();
        m_transformToolBox->raise();
    }
}

void KSpreadView::slotChildSelected( KoDocumentChild* ch )
{
  if ( m_pTable && !m_pTable->isProtected() )
  {
    d->actions->transform->setEnabled( TRUE );

    if ( !m_transformToolBox.isNull() )
    {
        m_transformToolBox->setEnabled( TRUE );
        m_transformToolBox->setDocumentChild( ch );
    }
  }

  m_pDoc->emitBeginOperation( false );
  m_pTable->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));
  m_pDoc->emitEndOperation();
  paintUpdates();
}

void KSpreadView::slotChildUnselected( KoDocumentChild* )
{
  if ( m_pTable && !m_pTable->isProtected() )
  {
    d->actions->transform->setEnabled( FALSE );

    if ( !m_transformToolBox.isNull() )
    {
        m_transformToolBox->setEnabled( FALSE );
    }
    deleteEditor( true );
  }

  m_pDoc->emitBeginOperation( false );
  m_pTable->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));
  m_pDoc->emitEndOperation();
  paintUpdates();
}


void KSpreadView::deleteEditor( bool saveChanges )
{
    m_pDoc->emitBeginOperation( false );
    m_pCanvas->deleteEditor( saveChanges );
    m_pDoc->emitEndOperation( selectionInfo()->selection() );
}

DCOPObject * KSpreadView::dcopObject()
{
  if ( !m_dcop )
    m_dcop = new KSpreadViewIface( this );

  return m_dcop;
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
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );

  if ( ev->activated() )
  {
    if ( m_sbCalcLabel )
    {
      resultOfCalc();
    }
  }
  else
  {
    /*if (m_sbCalcLabel)
      {
      disconnect(m_sbCalcLabel,SIGNAL(pressed( int )),this,SLOT(statusBarClicked(int)));
      }*/
  }

  KoView::guiActivateEvent( ev );
}

void KSpreadView::popupTabBarMenu( const QPoint & _point )
{
  if ( !koDocument()->isReadWrite() || !factory() )
    return;
  if ( m_pTabBar )
  {
    bool state = ( m_pDoc->map()->visibleSheets().count() > 1 );
    if ( m_pTable && m_pTable->isProtected() )
    {
      d->actions->removeTable->setEnabled( false );
      d->actions->hideTable->setEnabled( false );
    }
    else
    {
      d->actions->removeTable->setEnabled( state);
      d->actions->hideTable->setEnabled( state );
    }
    if ( !m_pDoc || !m_pDoc->map() || m_pDoc->map()->isProtected() )
    {
      d->actions->insertTable->setEnabled( false );
      d->actions->renameTable->setEnabled( false );
      d->actions->showTable->setEnabled( false );
      d->actions->hideTable->setEnabled( false );
      d->actions->removeTable->setEnabled( false );
    }
    static_cast<QPopupMenu*>(factory()->container("menupage_popup",this))->popup(_point);
  }
}

void KSpreadView::updateBorderButton()
{
  //  m_pDoc->emitBeginOperation( false );
  if ( m_pTable )
    d->actions->showPageBorders->setChecked( m_pTable->isShowPageBorders() );
  //  m_pDoc->emitEndOperation();
}

void KSpreadView::removeTable( KSpreadSheet *_t )
{
  m_pDoc->emitBeginOperation(false);
  QString m_tablName=_t->tableName();
  m_pTabBar->removeTab( m_tablName );
  setActiveTable( m_pDoc->map()->findTable( m_pDoc->map()->visibleSheets().first() ));

  bool state = m_pDoc->map()->visibleSheets().count() > 1;
  d->actions->removeTable->setEnabled( state );
  d->actions->hideTable->setEnabled( state );
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::insertTable( KSpreadSheet* table )
{
  m_pDoc->emitBeginOperation( false );
  QString tabName = table->tableName();
  if ( !table->isHidden() )
  {
    m_pTabBar->addTab( tabName );
  }

  bool state = ( m_pDoc->map()->visibleSheets().count() > 1 );
  d->actions->removeTable->setEnabled( state );
  d->actions->hideTable->setEnabled( state );
  m_pDoc->emitEndOperation( table->visibleRect( m_pCanvas ) );
}

QColor KSpreadView::borderColor() const
{
  return d->actions->borderColor->color();
}

void KSpreadView::updateShowTableMenu()
{
  m_pDoc->emitBeginOperation( false );
  if ( m_pTable->isProtected() )
    d->actions->showTable->setEnabled( false );
  else
    d->actions->showTable->setEnabled( m_pDoc->map()->hiddenSheets().count() > 0 );
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::closeEditor()
{
  m_pDoc->emitBeginOperation( false );
  m_pCanvas->closeEditor();
  m_pDoc->emitEndOperation( selectionInfo()->selection() );
}


void KSpreadView::paintUpdates()
{
  /* don't do any begin/end operation here -- this is what is called at an
     endOperation
  */
  m_pCanvas->paintUpdates();
}

#include "kspread_view.moc"

