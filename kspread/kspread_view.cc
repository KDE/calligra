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

/*****************************************************************************
 *
 * KSpreadView
 *
 *****************************************************************************/

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

KSpreadView::KSpreadView( QWidget *_parent, const char *_name, KSpreadDoc* doc )
  : KoView( doc, _parent, _name )
{
    ElapsedTime et( "KSpreadView constructor" );

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

    // Tab Bar
    m_pTabBarFirst = newIconButton( "tab_first" );
    QObject::connect( m_pTabBarFirst, SIGNAL( clicked() ), SLOT( slotScrollToFirstTable() ) );
    m_pTabBarLeft = newIconButton( "tab_left" );
    QObject::connect( m_pTabBarLeft, SIGNAL( clicked() ), SLOT( slotScrollToLeftTable() ) );
    m_pTabBarRight = newIconButton( "tab_right" );
    QObject::  connect( m_pTabBarRight, SIGNAL( clicked() ), SLOT( slotScrollToRightTable() ) );
    m_pTabBarLast = newIconButton( "tab_last" );
    QObject::connect( m_pTabBarLast, SIGNAL( clicked() ), SLOT( slotScrollToLastTable() ) );

    m_pTabBar = new KSpreadTabBar( this );
    QObject::connect( m_pTabBar, SIGNAL( tabChanged( const QString& ) ), this, SLOT( changeTable( const QString& ) ) );

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

    m_findOptions = 0;
    m_find = 0L;
    m_replace = 0L;

    KStatusBar * sb = statusBar();
    Q_ASSERT(sb);
    m_sbCalcLabel = sb ? new KStatusBarLabel( QString::null, 0, sb ) : 0;
    addStatusBarItem( m_sbCalcLabel, 0 );
    if (m_sbCalcLabel)
        connect(m_sbCalcLabel ,SIGNAL(itemPressed( int )),this,SLOT(statusBarClicked(int)));

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

    QStringList list = m_viewZoom->items();
    QString zoomStr( i18n("%1%").arg( m_pDoc->zoom()) );
    m_viewZoom->setCurrentItem( list.findIndex(zoomStr)  );

    // ## Might be wrong, if doc isn't loaded yet
    m_selectStyle->setItems( m_pDoc->styleManager()->styleNames() );

    // If doc was already loaded, initialize things
    // Otherwise the doc will do it in completeLoading.
    if ( !m_pDoc->map()->tableList().isEmpty() )
        QTimer::singleShot( 0, this, SLOT( initialPosition() ) );
}


void KSpreadView::initializeCalcActions()
{
  //menu calc
  /*******************************/
  m_menuCalcSum = new KToggleAction( i18n("Sum"), 0, actionCollection(),
                                     "menu_sum");
  connect( m_menuCalcSum, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  m_menuCalcSum->setExclusiveGroup( "Calc" );
  m_menuCalcSum->setToolTip(i18n("Calculate using sum."));

  /*******************************/
  m_menuCalcMin = new KToggleAction( i18n("Min"), 0, actionCollection(),
                                     "menu_min");
  connect( m_menuCalcMin, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  m_menuCalcMin->setExclusiveGroup( "Calc" );
  m_menuCalcMin->setToolTip(i18n("Calculate using minimum."));

  /*******************************/
  m_menuCalcMax = new KToggleAction( i18n("Max"), 0, actionCollection(),
                                     "menu_max");
  connect( m_menuCalcMax, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  m_menuCalcMax->setExclusiveGroup( "Calc" );
  m_menuCalcMax->setToolTip(i18n("Calculate using maximum."));

  /*******************************/
  m_menuCalcAverage = new KToggleAction( i18n("Average"), 0, actionCollection(),
                                         "menu_average");
  connect( m_menuCalcAverage, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  m_menuCalcAverage->setExclusiveGroup( "Calc" );
  m_menuCalcAverage->setToolTip(i18n("Calculate using average."));

  /*******************************/
  m_menuCalcCount = new KToggleAction( i18n("Count"), 0, actionCollection(),
                                       "menu_count");
  connect( m_menuCalcCount, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  m_menuCalcCount->setExclusiveGroup( "Calc" );
  m_menuCalcCount->setToolTip(i18n("Calculate using the count."));


  /*******************************/
  m_menuCalcNone = new KToggleAction( i18n("None"), 0, actionCollection(),
                                      "menu_none");
  connect( m_menuCalcNone, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  m_menuCalcNone->setExclusiveGroup( "Calc" );
  m_menuCalcNone->setToolTip(i18n("No calculation"));

  /*******************************/
}


void KSpreadView::initializeInsertActions()
{
  m_insertFunction = new KAction( i18n("&Function..."), "funct", 0, this,
                                  SLOT( insertMathExpr() ), actionCollection(),
                                  "insertMathExpr" );
  m_insertFunction->setToolTip(i18n("Insert math expression."));

  m_insertSeries = new KAction( i18n("&Series..."),"series", 0, this,
                                SLOT( insertSeries() ), actionCollection(), "series");
  m_insertSeries ->setToolTip(i18n("Insert a series."));

  m_insertLink = new KAction( i18n("&Link..."), 0, this,
                              SLOT( insertHyperlink() ), actionCollection(),
                              "insertHyperlink" );
  m_insertLink->setToolTip(i18n("Insert an Internet hyperlink."));

  m_insertSpecialChar = new KAction( i18n( "S&pecial Character..." ), "char", this,
                                     SLOT( insertSpecialChar() ), actionCollection(),
                                     "insertSpecialChar" );
  m_insertSpecialChar->setToolTip( i18n( "Insert one or more symbols or letters not found on the keyboard." ) );


  m_insertPart=new KoPartSelectAction( i18n("&Object"), "frame_query", this,
                                       SLOT( insertObject() ),
                                       actionCollection(), "insertPart");
  m_insertPart->setToolTip(i18n("Insert an object from another program."));

  m_insertChartFrame=new KAction( i18n("&Chart"), "frame_chart", 0, this,
                                  SLOT( insertChart() ), actionCollection(),
                                  "insertChart" );
  m_insertChartFrame->setToolTip(i18n("Insert a chart."));

#ifndef QT_NO_SQL
  m_insertFromDatabase = new KAction( i18n("From &Database..."), 0, this,
                                      SLOT( insertFromDatabase() ),
                                      actionCollection(), "insertFromDatabase");
  m_insertFromDatabase->setToolTip(i18n("Insert data from a SQL database."));
#endif

  m_insertFromTextfile = new KAction( i18n("From &Text File..."), 0, this,
                                      SLOT( insertFromTextfile() ),
                                      actionCollection(), "insertFromTextfile");
  m_insertFromTextfile->setToolTip(i18n("Insert data from a text file to the current cursor position/selection."));
  m_insertFromClipboard = new KAction( i18n("From &Clipboard..."), 0, this,
                                      SLOT( insertFromClipboard() ),
                                      actionCollection(), "insertFromClipboard");
  m_insertFromClipboard->setToolTip(i18n("Insert csv data from the clipboard to the current cursor position/selection."));

}

void KSpreadView::initializeEditActions()
{
  m_copy = KStdAction::copy( this, SLOT( copySelection() ), actionCollection(),
                             "copy" );
  m_copy->setToolTip(i18n("Copy the cell object to the clipboard."));

  m_paste = KStdAction::paste( this, SLOT( paste() ), actionCollection(),
                               "paste" );
  m_paste->setToolTip(i18n("Paste the contents of the clipboard at the cursor."));

  m_cut = KStdAction::cut( this, SLOT( cutSelection() ), actionCollection(),
                           "cut" );
  m_cut->setToolTip(i18n("Move the cell object to the clipboard."));

  m_specialPaste = new KAction( i18n("Special Paste..."), "special_paste",0,
                                this, SLOT( specialPaste() ), actionCollection(),
                                "specialPaste" );
  m_specialPaste->setToolTip
    (i18n("Paste the contents of the clipboard with special options."));

  m_insertCellCopy = new KAction( i18n("Paste with Insertion"),
                                  "insertcellcopy", 0, this,
                                  SLOT( slotInsertCellCopy() ),
                                  actionCollection(), "insertCellCopy" );
  m_insertCellCopy->setToolTip(i18n("Inserts a cell from the clipboard into the spreadsheet."));

  m_undo = KStdAction::undo( this, SLOT( undo() ), actionCollection(), "undo" );
  m_undo->setEnabled( FALSE );
  m_undo->setToolTip(i18n("Undo the previous action."));

  m_redo = KStdAction::redo( this, SLOT( redo() ), actionCollection(), "redo" );
  m_redo->setEnabled( FALSE );
  m_redo->setToolTip(i18n("Redo the action that has been undone."));

  m_findAction = KStdAction::find(this, SLOT(find()), actionCollection());
  /*m_findNext =*/ KStdAction::findNext( this, SLOT( findNext() ), actionCollection() );
  /*m_findPrevious =*/ KStdAction::findPrev( this, SLOT( findPrevious() ), actionCollection() );

  m_replaceAction = KStdAction::replace(this, SLOT(replace()), actionCollection());

  m_fillRight = new KAction( i18n( "&Right" ), 0, 0, this,
                             SLOT( fillRight() ), actionCollection(), "fillRight" );
  m_fillLeft = new KAction( i18n( "&Left" ), 0, 0, this,
                             SLOT( fillLeft() ), actionCollection(), "fillLeft" );
  m_fillDown = new KAction( i18n( "&Down" ), 0, 0, this,
                             SLOT( fillDown() ), actionCollection(), "fillDown" );
  m_fillUp = new KAction( i18n( "&Up" ), 0, 0, this,
                             SLOT( fillUp() ), actionCollection(), "fillUp" );
}

void KSpreadView::initializeAreaOperationActions()
{
  m_areaName = new KAction( i18n("Area Name..."), 0, this,
                            SLOT( setAreaName() ), actionCollection(),
                            "areaname" );
  m_areaName->setToolTip(i18n("Set a name for a region of the spreadsheet."));

  m_showArea = new KAction( i18n("Show Area..."), 0, this,
                            SLOT( showAreaName() ), actionCollection(),
                            "showArea" );
  m_showArea->setToolTip(i18n("Display a named area."));

  m_sortList = new KAction( i18n("Custom Lists..."), 0, this,
                            SLOT( sortList() ), actionCollection(),
                            "sortlist" );
  m_sortList->setToolTip(i18n("Create custom lists for sorting or autofill."));

  m_sort = new KAction( i18n("&Sort..."), 0, this, SLOT( sort() ),
                        actionCollection(), "sort" );
  m_sort->setToolTip(i18n("Sort a group of cells."));

  m_autoSum = new KAction( i18n("Autosum"), "black_sum", 0, this,
                           SLOT( autoSum() ), actionCollection(), "autoSum" );
  m_autoSum->setToolTip(i18n("Insert the 'sum' function"));

  m_sortDec = new KAction( i18n("Sort &Decreasing"), "sort_decrease", 0, this,
                           SLOT( sortDec() ), actionCollection(), "sortDec" );
  m_sortDec->setToolTip(i18n("Sort a group of cells in decreasing (last to first) order."));

  m_sortInc = new KAction( i18n("Sort &Increasing"), "sort_incr", 0, this,
                           SLOT( sortInc() ), actionCollection(), "sortInc" );
  m_sortInc->setToolTip(i18n("Sort a group of cells in ascending (first to last) order."));

  m_goalSeek = new KAction( i18n("&Goal Seek..."), 0, this,
                            SLOT( goalSeek() ), actionCollection(), "goalSeek" );
  m_goalSeek->setToolTip( i18n("Repeating calculation to find a specific value.") );

  m_multipleOperations = new KAction( i18n("&Multiple Operations..."), 0, this,
                            SLOT( multipleOperations() ), actionCollection(), "multipleOperations" );
  m_multipleOperations->setToolTip( i18n("Apply the same formula to various cells using different values for the parameter.") );

  m_subTotals = new KAction( i18n("&Subtotals..."), 0, this,
                             SLOT( subtotals() ), actionCollection(), "subtotals" );
  m_subTotals->setToolTip( i18n("Create different kind of subtotals to a list or database.") );

  m_textToColumns = new KAction( i18n("&Text to Columns..."), 0, this,
                            SLOT( textToColumns() ), actionCollection(), "textToColumns" );
  m_textToColumns->setToolTip( i18n("Expand the content of cells to multiple columns.") );

  m_consolidate = new KAction( i18n("&Consolidate..."), 0, this,
                               SLOT( consolidate() ), actionCollection(),
                               "consolidate" );
  m_consolidate->setToolTip(i18n("Create a region of summary data from a group of similar regions."));
}

void KSpreadView::initializeGlobalOperationActions()
{
  m_recalc_workbook = new KAction( i18n("Recalculate Workbook"), Key_F9, this,
                                   SLOT( recalcWorkBook() ), actionCollection(),
                                   "RecalcWorkBook" );
  m_recalc_workbook->setToolTip(i18n("Recalculate the value of every cell in all worksheets."));

  m_recalc_worksheet = new KAction( i18n("Recalculate Sheet"), SHIFT + Key_F9,
                                    this, SLOT( recalcWorkSheet() ),
                                    actionCollection(), "RecalcWorkSheet" );
  m_recalc_worksheet->setToolTip(i18n("Recalculate the value of every cell in the current worksheet."));

  m_preference = new KAction( i18n("Configure KSpread..."),"configure", 0, this,
                              SLOT( preference() ), actionCollection(),
                              "preference" );
  m_preference->setToolTip(i18n("Set various KSpread options."));

  m_editGlobalScripts = new KAction( i18n("Edit Global Scripts..."), 0, this,
                                     SLOT( editGlobalScripts() ),
                                     actionCollection(), "editGlobalScripts" );
  m_editGlobalScripts->setToolTip("");//i18n("")); /* TODO - what is this? */

  m_editLocalScripts = new KAction( i18n("Edit Local Scripts..."), 0, this,
                                    SLOT( editLocalScripts() ),
                                    actionCollection(), "editLocalScripts" );
  m_editLocalScripts->setToolTip("");//i18n("")); /* TODO - what is this? */

  m_reloadScripts = new KAction( i18n("Reload Scripts"), 0, this,
                                 SLOT( reloadScripts() ), actionCollection(),
                                 "reloadScripts" );
  m_reloadScripts->setToolTip("");//i18n("")); /* TODO - what is this? */

  m_showPageBorders = new KToggleAction( i18n("Show Page Borders"), 0,
                                         actionCollection(), "showPageBorders");
  connect( m_showPageBorders, SIGNAL( toggled( bool ) ), this,
           SLOT( togglePageBorders( bool ) ) );
  m_showPageBorders->setToolTip( i18n( "Show on the spreadsheet where the page borders will be." ) );

  m_protectSheet = new KToggleAction( i18n( "Protect &Sheet..." ), 0,
                                      actionCollection(), "protectSheet" );
  m_protectSheet->setToolTip( i18n( "Protect the sheet from being modified." ) );
  connect( m_protectSheet, SIGNAL( toggled( bool ) ), this,
           SLOT( toggleProtectSheet( bool ) ) );

  m_protectDoc = new KToggleAction( i18n( "Protect &Doc..." ), 0,
                                      actionCollection(), "protectDoc" );
  m_protectDoc->setToolTip( i18n( "Protect the document from being modified." ) );
  connect( m_protectDoc, SIGNAL( toggled( bool ) ), this,
           SLOT( toggleProtectDoc( bool ) ) );

  m_recordChanges = new KToggleAction( i18n( "&Record Changes" ), 0,
                                       actionCollection(), "recordChanges" );
  m_recordChanges->setToolTip( i18n( "Record changes made to this document." ) );
  connect( m_recordChanges, SIGNAL( toggled( bool ) ), this,
           SLOT( toggleRecordChanges( bool ) ) );


  m_protectChanges = new KToggleAction( i18n( "&Protect Changes..." ), 0,
                                        actionCollection(), "protectRecords" );
  m_protectChanges->setToolTip( i18n( "Protect the change records from being accepted or rejected." ) );
  m_protectChanges->setEnabled( false );
  connect( m_protectChanges, SIGNAL( toggled( bool ) ), this,
           SLOT( toggleProtectChanges( bool ) ) );

  m_filterChanges = new KAction( i18n( "&Filter Changes..." ), 0, this,
                                 SLOT( filterChanges() ), actionCollection(),
                                 "filterChanges" );
  m_filterChanges->setToolTip( i18n( "Change display settings for changes." ) );
  m_filterChanges->setEnabled( false );

  m_acceptRejectChanges = new KAction( i18n( "&Accept or Reject..." ), 0, this,
                                       SLOT( acceptRejectChanges() ), actionCollection(),
                                       "acceptRejectChanges" );
  m_acceptRejectChanges->setToolTip( i18n( "Accept or reject changes made to this document." ) );
  m_acceptRejectChanges->setEnabled( false );

  m_commentChanges = new KAction( i18n( "&Comment Changes..." ), 0, this,
                                  SLOT( commentChanges() ), actionCollection(),
                                  "commentChanges" );
  m_commentChanges->setToolTip( i18n( "Add comments to changes you made." ) );
  m_commentChanges->setEnabled( false );

  m_mergeDocument = new KAction( i18n( "&Merge Document..." ), 0, this,
                                 SLOT( mergeDocument() ), actionCollection(),
                                 "mergeDocument" );
  m_mergeDocument->setToolTip( i18n( "Merge this document with a document that recorded changes." ) );

  m_viewZoom = new KSelectAction( i18n( "Zoom" ), "viewmag", 0,
                                  actionCollection(), "view_zoom" );

  connect( m_viewZoom, SIGNAL( activated( const QString & ) ),
           this, SLOT( viewZoom( const QString & ) ) );
  m_viewZoom->setEditable(true);
  changeZoomMenu( m_pDoc->zoom() );

  m_formulaSelection = new KSelectAction(i18n("Formula Selection"), 0,
                                         actionCollection(), "formulaSelection");
  m_formulaSelection->setToolTip(i18n("Insert a function."));
  QStringList lst;
  lst.append( "SUM");
  lst.append( "AVERAGE");
  lst.append( "IF");
  lst.append( "COUNT");
  lst.append( "MIN");
  lst.append( "MAX");
  lst.append( i18n("Others...") );
  ((KSelectAction*) m_formulaSelection)->setItems( lst );
  m_formulaSelection->setComboWidth( 80 );
  m_formulaSelection->setCurrentItem(0);
  connect( m_formulaSelection, SIGNAL( activated( const QString& ) ),
           this, SLOT( formulaSelection( const QString& ) ) );


  m_transform = new KAction( i18n("Transform Object..."), "rotate", 0, this,
                             SLOT( transformPart() ),
                             actionCollection(), "transform" );
  m_transform->setToolTip(i18n("Rotate the contents of the cell."));



  m_transform->setEnabled( FALSE );
  connect( m_transform, SIGNAL( activated() ), this, SLOT( transformPart() ) );


  m_paperLayout = new KAction( i18n("Page Layout..."), 0, this,
                               SLOT( paperLayoutDlg() ), actionCollection(),
                               "paperLayout" );
  m_paperLayout->setToolTip(i18n("Specify the layout of the spreadsheet for a printout."));

  m_definePrintRange = new KAction( i18n("Define Print Range"), 0, this,
                                    SLOT( definePrintRange() ), actionCollection(),
                                    "definePrintRange" );
  m_definePrintRange->setToolTip(i18n("Define the print range in the current sheet."));

  m_resetPrintRange = new KAction( i18n("Reset Print Range"), 0, this,
                                   SLOT( resetPrintRange() ), actionCollection(),
                                   "resetPrintRange" );
  m_definePrintRange->setToolTip(i18n("Define the print range in the current sheet."));

  m_createTemplate = new KAction( i18n( "&Create Template From Document..." ), 0, this,
                                  SLOT( createTemplate() ), actionCollection(), "createTemplate" );

  m_styleDialog = new KAction( i18n( "Style Manager..." ), 0, this, SLOT( styleDialog() ),
                               actionCollection(), "styles" );
  m_styleDialog->setToolTip( i18n( "Edit and organize cell styles." ) );

  m_selectStyle = new KSelectAction( i18n( "St&yle" ), 0,
                                     actionCollection(), "stylemenu" );
  m_selectStyle->setToolTip( i18n( "Apply a predefined style to the selected cells." ) );
  connect( m_selectStyle, SIGNAL( activated( const QString & ) ), this, SLOT( styleSelected( const QString & ) ) );

  m_createStyle = new KAction( i18n( "Create Style From Cell..." ), 0,
                               this, SLOT( createStyleFromCell()), actionCollection(), "createStyle" );
  m_createStyle->setToolTip( i18n( "Create a new style based on the currently selected cell." ) );
}


void KSpreadView::initializeCellOperationActions()
{
  m_editCell = new KAction( i18n("Modify Cell"),"cell_edit", CTRL + Key_M, this,
                            SLOT( editCell() ), actionCollection(), "editCell" );
  m_editCell->setToolTip(i18n("Edit the highlighted cell."));

  m_delete = new KAction( i18n("Delete"),"deletecell", 0, this,

			  SLOT( deleteSelection() ), actionCollection(),
                          "delete" );
  m_delete->setToolTip(i18n("Delete all contents and formatting of the current cell."));

  m_clearText = new KAction( i18n("Text"), 0, this, SLOT( clearTextSelection() ),
                             actionCollection(), "cleartext" );
  m_clearText->setToolTip(i18n("Remove the contents of the current cell."));

  m_gotoCell = new KAction( i18n("Goto Cell..."),"goto", 0, this,
                            SLOT( gotoCell() ), actionCollection(), "gotoCell" );
  m_gotoCell->setToolTip(i18n("Move to a particular cell."));

  m_mergeCell = new KAction( i18n("Merge Cells"),"mergecell" ,0, this,
                             SLOT( mergeCell() ), actionCollection(),
                             "mergecell" );
  m_mergeCell->setToolTip(i18n("Merge the selected region into one large cell."));

  m_dissociateCell = new KAction( i18n("Dissociate Cells"),"dissociatecell" ,0,
                                  this, SLOT( dissociateCell() ),
                                  actionCollection(), "dissociatecell" );
  m_dissociateCell->setToolTip(i18n("Unmerge the current cell."));

  m_removeCell = new KAction( i18n("Remove Cells..."), "removecell", 0, this,
                              SLOT( slotRemove() ), actionCollection(),
                              "removeCell" );
  m_removeCell->setToolTip(i18n("Removes the current cell from the spreadsheet."));

  m_insertCell = new KAction( i18n("Insert Cells..."), "insertcell", 0, this,
                              SLOT( slotInsert() ), actionCollection(),
                              "insertCell" );
  m_insertCell->setToolTip(i18n("Insert a blank cell into the spreadsheet."));

}

void KSpreadView::initializeCellPropertyActions()
{
  m_addModifyComment = new KAction( i18n("&Add/Modify Comment..."),"comment", 0,
                                    this, SLOT( addModifyComment() ),
                                    actionCollection(), "addmodifycomment" );
  m_addModifyComment->setToolTip(i18n("Edit a comment for this cell."));

  m_removeComment = new KAction( i18n("&Remove Comment"),"removecomment", 0,
                                 this, SLOT( removeComment() ),
                                 actionCollection(), "removecomment" );
  m_removeComment->setToolTip(i18n("Remove this cell's comment."));

  m_conditional = new KAction( i18n("Conditional Cell Attributes..."), 0, this,
                               SLOT( conditional() ), actionCollection(),
                               "conditional" );
  m_conditional->setToolTip(i18n("Set cell format based on certain conditions."));

  m_validity = new KAction( i18n("Validity..."), 0, this, SLOT( validity() ),
                            actionCollection(), "validity" );
  m_validity->setToolTip(i18n("Set tests to confirm cell data is valid."));

  m_clearComment = new KAction( i18n("Comment"), 0, this,
                                SLOT( clearCommentSelection() ),
                                actionCollection(), "clearcomment" );
  m_clearComment->setToolTip(i18n("Remove this cell's comment."));

  m_clearValidity = new KAction( i18n("Validity"), 0, this,
                                 SLOT( clearValiditySelection() ),
                                 actionCollection(), "clearvalidity" );
  m_clearValidity->setToolTip(i18n("Remove the validity tests on this cell."));

  m_clearConditional = new KAction( i18n("Conditional Cell Attributes"), 0, this,
                                    SLOT( clearConditionalSelection() ),
                                    actionCollection(), "clearconditional" );
  m_clearConditional->setToolTip(i18n("Remove the conditional cell formatting."));

  m_increaseIndent = new KAction( i18n("Increase Indent"),
                                  QApplication::reverseLayout() ? "format_decreaseindent":"format_increaseindent",0, this,
                                  SLOT( increaseIndent() ), actionCollection(),
                                  "increaseindent" );
  m_increaseIndent->setToolTip(i18n("Increase the indentation."));

  m_decreaseIndent = new KAction( i18n("Decrease Indent"),
                                  QApplication::reverseLayout() ? "format_increaseindent" : "format_decreaseindent" ,0, this,
                                  SLOT( decreaseIndent() ), actionCollection(),
                                  "decreaseindent");
  m_decreaseIndent->setToolTip(i18n("Decrease the indentation."));

  m_multiRow = new KToggleAction( i18n("Multi Row"), "multirow", 0,
                                  actionCollection(), "multiRow" );
  connect( m_multiRow, SIGNAL( toggled( bool ) ), this,
           SLOT( multiRow( bool ) ) );
  m_multiRow->setToolTip(i18n("Make the cell text wrap onto multiple lines."));

  m_cellLayout = new KAction( i18n("Cell Format..."),"cell_layout",
                              CTRL + ALT + Key_F, this, SLOT( layoutDlg() ),
                              actionCollection(), "cellLayout" );
  m_cellLayout->setToolTip(i18n("Set the cell formatting."));

  m_default = new KAction( i18n("Default"), 0, this, SLOT( defaultSelection() ),
                           actionCollection(), "default" );
  m_default->setToolTip(i18n("Resets to the default format."));

  m_bgColor = new TKSelectColorAction( i18n("Background Color"),
                                       TKSelectColorAction::FillColor,
                                       actionCollection(), "backgroundColor",
                                       true );
  connect(m_bgColor,SIGNAL(activated()),SLOT(changeBackgroundColor()));
  m_bgColor->setDefaultColor(QColor());
  m_bgColor->setToolTip(i18n("Set the background color."));

}


void KSpreadView::initializeTextFormatActions()
{
  /*******************************/
  m_percent = new KToggleAction( i18n("Percent Format"), "percent", 0,
                                 actionCollection(), "percent");
  connect( m_percent, SIGNAL( toggled( bool ) ), this, SLOT( percent( bool ) ) );
  m_percent->setToolTip(i18n("Set the cell formatting to look like a percentage."));

  /*******************************/
  m_precplus = new KAction( i18n("Increase Precision"), "prec_plus", 0, this,
                            SLOT( precisionPlus() ), actionCollection(),
                            "precplus");
  m_precplus->setToolTip(i18n("Increase the decimal precision shown onscreen."));

  /*******************************/
  m_precminus = new KAction( i18n("Decrease Precision"), "prec_minus", 0, this,
                             SLOT( precisionMinus() ), actionCollection(),
                             "precminus");
  m_precminus->setToolTip(i18n("Decrease the decimal precision shown onscreen."));

  /*******************************/
  m_money = new KToggleAction( i18n("Money Format"), "money", 0,
                               actionCollection(), "money");
  connect( m_money, SIGNAL( toggled( bool ) ), this,
           SLOT( moneyFormat( bool ) ) );
  m_money->setToolTip(i18n("Set the cell formatting to look like your local currency."));

  /*******************************/
  m_upper = new KAction( i18n("Upper Case"), "fontsizeup", 0, this,
                         SLOT( upper() ), actionCollection(), "upper" );
  m_upper->setToolTip(i18n("Convert all letters to upper case."));

  /*******************************/
  m_lower = new KAction( i18n("Lower Case"), "fontsizedown", 0, this,
                         SLOT( lower() ), actionCollection(), "lower" );
  m_lower->setToolTip(i18n("Convert all letters to lower case."));

  /*******************************/
  m_firstLetterUpper = new KAction( i18n("Convert First Letter to Upper Case"),
                                    "first_letter_upper" ,0, this,
                                    SLOT( firstLetterUpper() ),
                                    actionCollection(), "firstletterupper" );
  m_firstLetterUpper->setToolTip(i18n("Capitalize the first letter."));
}

void KSpreadView::initializeTextLayoutActions()
{
  /*******************************/
  m_alignLeft = new KToggleAction( i18n("Align Left"), "text_left", 0,
                                   actionCollection(), "left");
  connect( m_alignLeft, SIGNAL( toggled( bool ) ), this,
           SLOT( alignLeft( bool ) ) );
  m_alignLeft->setExclusiveGroup( "Align" );
  m_alignLeft->setToolTip(i18n("Left justify the cell contents."));

  /*******************************/
  m_alignCenter = new KToggleAction( i18n("Align Center"), "text_center", 0,
                                     actionCollection(), "center");
  connect( m_alignCenter, SIGNAL( toggled( bool ) ), this,
           SLOT( alignCenter( bool ) ) );
  m_alignCenter->setExclusiveGroup( "Align" );
  m_alignCenter->setToolTip(i18n("Center the cell contents."));

  /*******************************/
  m_alignRight = new KToggleAction( i18n("Align Right"), "text_right", 0,
                                    actionCollection(), "right");
  connect( m_alignRight, SIGNAL( toggled( bool ) ), this,
           SLOT( alignRight( bool ) ) );
  m_alignRight->setExclusiveGroup( "Align" );
  m_alignRight->setToolTip(i18n("Right justify the cell contents."));

  /*******************************/
  m_alignTop = new KToggleAction( i18n("Align Top"), "text_top", 0,
                                  actionCollection(), "top");
  connect( m_alignTop, SIGNAL( toggled( bool ) ), this,
           SLOT( alignTop( bool ) ) );
  m_alignTop->setExclusiveGroup( "Pos" );
  m_alignTop->setToolTip(i18n("Align cell contents along the top of the cell."));

  /*******************************/
  m_alignMiddle = new KToggleAction( i18n("Align Middle"), "middle", 0,
                                     actionCollection(), "middle");
  connect( m_alignMiddle, SIGNAL( toggled( bool ) ), this,
           SLOT( alignMiddle( bool ) ) );
  m_alignMiddle->setExclusiveGroup( "Pos" );
  m_alignMiddle->setToolTip(i18n("Align cell contents centered in the cell."));

  /*******************************/
  m_alignBottom = new KToggleAction( i18n("Align Bottom"), "text_bottom", 0,
                                     actionCollection(), "bottom");
  connect( m_alignBottom, SIGNAL( toggled( bool ) ), this,
           SLOT( alignBottom( bool ) ) );
  m_alignBottom->setExclusiveGroup( "Pos" );
  m_alignBottom->setToolTip(i18n("Align cell contents along the bottom of the cell."));

  /*******************************/
  m_verticalText = new KToggleAction( i18n("Vertical Text"),"vertical_text" ,
                                      0 ,actionCollection(), "verticaltext" );
  connect( m_verticalText, SIGNAL( toggled( bool ) ), this,
           SLOT( verticalText( bool ) ) );
  m_verticalText->setToolTip(i18n("Print cell contents vertically."));

  /*******************************/
  m_changeAngle = new KAction( i18n("Change Angle..."), 0, this,
                               SLOT( changeAngle() ), actionCollection(),
                               "changeangle" );
  m_changeAngle->setToolTip(i18n("Change the angle that cell contents are printed."));
}

void KSpreadView::initializeTextPropertyActions()
{
  /*******************************/
  m_bold = new KToggleAction( i18n("Bold"), "text_bold", CTRL + Key_B,
                              actionCollection(), "bold");
  connect( m_bold, SIGNAL( toggled( bool ) ), this, SLOT( bold( bool ) ) );

  /*******************************/
  m_italic = new KToggleAction( i18n("Italic"), "text_italic", CTRL + Key_I,
                                actionCollection(), "italic");
  connect( m_italic, SIGNAL( toggled( bool ) ), this, SLOT( italic( bool ) ) );

  /*******************************/
  m_underline = new KToggleAction( i18n("Underline"), "text_under",
                                   CTRL + Key_U, actionCollection(),
                                   "underline");
  connect( m_underline, SIGNAL( toggled( bool ) ), this,
           SLOT( underline( bool ) ) );

  /*******************************/
  m_strikeOut = new KToggleAction( i18n("Strike Out"), "text_strike", 0,
                                   actionCollection(), "strikeout");
  connect( m_strikeOut, SIGNAL( toggled( bool ) ), this,
           SLOT( strikeOut( bool ) ) );

  /*******************************/
  m_selectFont = new KFontAction( i18n("Select Font..."), 0, actionCollection(),
                                  "selectFont" );
  connect( m_selectFont, SIGNAL( activated( const QString& ) ), this,
           SLOT( fontSelected( const QString& ) ) );

  /*******************************/
  m_selectFontSize = new KFontSizeAction( i18n("Select Font Size"), 0,
                                          actionCollection(), "selectFontSize" );
  connect( m_selectFontSize, SIGNAL( fontSizeChanged( int ) ), this,
           SLOT( fontSizeSelected( int ) ) );

  /*******************************/
  m_fontSizeUp = new KAction( i18n("Increase Font Size"), "fontsizeup", 0, this,
                              SLOT( increaseFontSize() ), actionCollection(),
                              "increaseFontSize" );

  /*******************************/
  m_fontSizeDown = new KAction( i18n("Decrease Font Size"), "fontsizedown", 0,
                                this, SLOT( decreaseFontSize() ),
                                actionCollection(), "decreaseFontSize" );

  /*******************************/
  m_textColor = new TKSelectColorAction( i18n("Text Color"),
                                         TKSelectColorAction::TextColor,
                                         actionCollection(), "textColor",true );
  connect( m_textColor, SIGNAL(activated()), SLOT(changeTextColor()) );
  m_textColor->setDefaultColor(QColor());

  /*******************************/
}

void KSpreadView::initializeTableActions()
{
  m_insertTable = new KAction( i18n("Insert Sheet"),"inserttable", 0, this,
                               SLOT( insertTable() ), actionCollection(),
                               "insertTable" );
  m_insertTable->setToolTip(i18n("Insert a new sheet."));

  /* basically the same action here, but it's in the insert menu so we don't
     want to also have 'insert' in the caption
  */
  m_menuInsertTable = new KAction( i18n("&Sheet"),"inserttable", 0, this,
                               SLOT( insertTable() ), actionCollection(),
                               "menuInsertTable" );
  m_menuInsertTable->setToolTip(i18n("Insert a new sheet."));

  m_removeTable = new KAction( i18n("Remove Sheet"), "delete_table",0,this,
                               SLOT( removeTable() ), actionCollection(),
                               "removeTable" );
  m_removeTable->setToolTip(i18n("Remove the active sheet."));

  m_renameTable=new KAction( i18n("Rename Sheet..."),0,this,
                             SLOT( slotRename() ), actionCollection(),
                             "renameTable" );
  m_renameTable->setToolTip(i18n("Rename the active sheet."));

  m_nextTable = new KAction( i18n("Next Sheet"), CTRL + Key_PageDown, this,
                             SLOT( nextTable() ), actionCollection(),
                             "nextTable");
  m_nextTable->setToolTip(i18n("Move to the next sheet."));

  m_prevTable = new KAction( i18n("Previous Sheet"), CTRL + Key_PageUp, this,
                             SLOT( previousTable() ), actionCollection(),
                             "previousTable");
  m_prevTable->setToolTip(i18n("Move to the previous sheet."));

  m_firstTable = new KAction( i18n("First Sheet"), 0, this,
                              SLOT( firstTable() ), actionCollection(),
                              "firstTable");
  m_firstTable->setToolTip(i18n("Move to the first sheet."));

  m_lastTable = new KAction( i18n("Last Sheet"), 0, this,
                             SLOT( lastTable() ), actionCollection(),
                             "lastTable");
  m_lastTable->setToolTip(i18n("Move to the last sheet."));

  m_showTable = new KAction(i18n("Show Sheet..."),0 ,this,SLOT( showTable()),
                            actionCollection(), "showTable" );
  m_showTable->setToolTip(i18n("Show a hidden sheet."));

  m_hideTable = new KAction(i18n("Hide Sheet"),0 ,this,SLOT( hideTable()),
                            actionCollection(), "hideTable" );
  m_hideTable->setToolTip(i18n("Hide the active sheet."));

  m_tableFormat = new KAction( i18n("AutoFormat..."), 0, this,
                               SLOT( tableFormat() ), actionCollection(),
                               "tableFormat" );
  m_tableFormat->setToolTip(i18n("Set the worksheet formatting."));
}

void KSpreadView::initializeSpellChecking()
{
  m_spellChecking = KStdAction::spelling( this, SLOT( extraSpelling() ),
                                          actionCollection(), "spelling" );
  m_spellChecking->setToolTip(i18n("Check the spelling."));
}


void KSpreadView::initializeRowColumnActions()
{
  m_adjust = new KAction( i18n("Adjust Row && Column"), 0, this,
                          SLOT( adjust() ), actionCollection(), "adjust" );
  m_adjust->setToolTip(i18n("Adjusts row/column size so that the contents will fit."));

  m_resizeRow = new KAction( i18n("Resize Row..."), "resizerow", 0, this,
                             SLOT( resizeRow() ), actionCollection(),
                             "resizeRow" );
  m_resizeRow->setToolTip(i18n("Change the height of a row."));

  m_resizeColumn = new KAction( i18n("Resize Column..."), "resizecol", 0, this,
                                SLOT( resizeColumn() ), actionCollection(),
                                "resizeCol" );
  m_resizeColumn->setToolTip(i18n("Change the width of a column."));

  m_equalizeRow = new KAction( i18n("Equalize Row"), "adjustrow", 0, this,
                               SLOT( equalizeRow() ), actionCollection(),
                               "equalizeRow" );
  m_equalizeRow->setToolTip(i18n("Resizes selected rows to be the same size."));

  m_equalizeColumn = new KAction( i18n("Equalize Column"), "adjustcol", 0, this,
                                  SLOT( equalizeColumn() ), actionCollection(),
                                  "equalizeCol" );
  m_equalizeColumn->setToolTip(i18n("Resizes selected columns to be the same size."));

  m_deleteColumn = new KAction( i18n("Delete Columns"), "delete_table_col", 0,
                                this, SLOT( deleteColumn() ),
                                actionCollection(), "deleteColumn" );
  m_deleteColumn->setToolTip(i18n("Removes a column from the spreadsheet."));

  m_deleteRow = new KAction( i18n("Delete Rows"), "delete_table_row", 0, this,
                             SLOT( deleteRow() ), actionCollection(),
                             "deleteRow" );
  m_deleteRow->setToolTip(i18n("Removes a row from the spreadsheet."));

  m_insertColumn = new KAction( i18n("Insert Columns"), "insert_table_col" ,
                                0, this, SLOT( insertColumn() ),
                                actionCollection(), "insertColumn" );
  m_insertColumn->setToolTip(i18n("Inserts a new column into the spreadsheet."));

  m_insertRow = new KAction( i18n("Insert Rows"), "insert_table_row", 0, this,
                             SLOT( insertRow() ), actionCollection(),
                             "insertRow" );
  m_insertRow->setToolTip(i18n("Inserts a new row into the spreadsheet."));

  m_hideRow = new KAction( i18n("Hide Rows"), "hide_table_row", 0, this,
                           SLOT( hideRow() ), actionCollection(), "hideRow" );
  m_hideRow->setToolTip(i18n("Hide a row from view."));

  m_showRow = new KAction( i18n("Show Rows..."), "show_table_row", 0, this,
                           SLOT( showRow() ), actionCollection(), "showRow" );
  m_showRow->setToolTip(i18n("Show hidden rows."));

  m_showSelRows = new KAction( i18n("Show Rows"), "show_table_row", 0, this,
                               SLOT( showSelRows() ), actionCollection(),
                               "showSelRows" );
  m_showSelRows->setEnabled(false);
  m_showSelRows->setToolTip(i18n("Show hidden rows in the selection."));

  m_hideColumn = new KAction( i18n("Hide Columns"), "hide_table_column", 0,
                              this, SLOT( hideColumn() ), actionCollection(),
                              "hideColumn" );
  m_hideColumn->setToolTip(i18n("Hide the column from view."));

  m_showColumn = new KAction( i18n("Show Columns..."), "show_table_column", 0,
                              this, SLOT( showColumn() ), actionCollection(),
                              "showColumn" );
  m_showColumn->setToolTip(i18n("Show hidden columns."));

  m_showSelColumns = new KAction( i18n("Show Columns"), "show_table_column",
                                  0, this, SLOT( showSelColumns() ),
                                  actionCollection(), "showSelColumns" );
  m_showSelColumns->setToolTip(i18n("Show hidden columns in the selection."));
  m_showSelColumns->setEnabled(false);

}


void KSpreadView::initializeBorderActions()
{
  m_borderLeft = new KAction( i18n("Border Left"), "border_left", 0, this,
                              SLOT( borderLeft() ), actionCollection(),
                              "borderLeft" );
  m_borderLeft->setToolTip(i18n("Set a left border to the selected area."));

  m_borderRight = new KAction( i18n("Border Right"), "border_right", 0, this,
                               SLOT( borderRight() ), actionCollection(),
                               "borderRight" );
  m_borderRight->setToolTip(i18n("Set a right border to the selected area."));

  m_borderTop = new KAction( i18n("Border Top"), "border_top", 0, this,
                             SLOT( borderTop() ), actionCollection(),
                             "borderTop" );
  m_borderTop->setToolTip(i18n("Set a top border to the selected area."));

  m_borderBottom = new KAction( i18n("Border Bottom"), "border_bottom", 0, this,
                                SLOT( borderBottom() ), actionCollection(),
                                "borderBottom" );
  m_borderBottom->setToolTip(i18n("Set a bottom border to the selected area."));

  m_borderAll = new KAction( i18n("All Borders"), "border_all", 0, this,
                             SLOT( borderAll() ), actionCollection(),
                             "borderAll" );
  m_borderAll->setToolTip(i18n("Set a border around all cells in the selected area."));

  m_borderRemove = new KAction( i18n("Remove Borders"), "border_remove", 0,
                                this, SLOT( borderRemove() ), actionCollection(),
                                "borderRemove" );
  m_borderRemove->setToolTip(i18n("Remove all borders in the selected area."));

  m_borderOutline = new KAction( i18n("Border Outline"), ("border_outline"), 0,
                                 this, SLOT( borderOutline() ),
                                 actionCollection(), "borderOutline" );
  m_borderOutline->setToolTip(i18n("Set a border to the outline of the selected area."));

  m_borderColor = new TKSelectColorAction( i18n("Border Color"),
                                           TKSelectColorAction::LineColor,
                                           actionCollection(), "borderColor" );

  connect( m_borderColor, SIGNAL(activated()), SLOT(changeBorderColor()) );
  m_borderColor->setToolTip( i18n( "Select a new border color." ) );

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
            m_menuCalcSum->setChecked(true);
            break;
        case  Min:
            m_menuCalcMin->setChecked(true);
            break;
        case  Max:
            m_menuCalcMax->setChecked(true);
            break;
        case  Average:
            m_menuCalcAverage->setChecked(true);
            break;
        case  Count:
            m_menuCalcCount->setChecked(true);
            break;
        case  NoneCalc:
            m_menuCalcNone->setChecked(true);
            break;
        default :
            m_menuCalcSum->setChecked(true);
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

  if ( m_pTable )
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
    kdDebug() << "KSpreadView::initialPosition" << endl;

    // Loading completed, pick initial worksheet

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
      tbl = m_pDoc->map()->findTable(m_pTabBar->listshow().first());
      if ( !tbl )
      {
        tbl = m_pDoc->map()->firstTable();
        if ( tbl )
        {
          tbl->setHidden( false );
          QString tabName = tbl->tableName();
          m_pTabBar->addTab( tabName );
          m_pTabBar->removeHiddenTab( tabName );
        }
      }
      setActiveTable( tbl );
    }

    refreshView();

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

    m_tableFormat->setEnabled(false);
    m_sort->setEnabled(false);
    m_mergeCell->setEnabled(false);
    m_createStyle->setEnabled(false);

    m_fillUp->setEnabled( false );
    m_fillRight->setEnabled( false );
    m_fillDown->setEnabled( false );
    m_fillLeft->setEnabled( false );

    m_recordChanges->setChecked( m_pDoc->map()->changes() );
    m_acceptRejectChanges->setEnabled( m_pDoc->map()->changes() );
    m_filterChanges->setEnabled( m_pDoc->map()->changes() );
    m_protectChanges->setEnabled( m_pDoc->map()->changes() );
    m_commentChanges->setEnabled( m_pDoc->map()->changes() );

    // make paint effective:
    m_pDoc->decreaseNumOperation();
    m_insertChartFrame->setEnabled(false);

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
    m_textColor->setCurrentColor( color );

    color=cell->bgColor(  column, row );

    if ( !color.isValid() )
        color = QApplication::palette().active().base();

    m_bgColor->setCurrentColor( color );

    m_selectFontSize->setFontSize( cell->textFontSize( column, row ) );
    m_selectFont->setFont( cell->textFontFamily( column,row ) );
    m_bold->setChecked( cell->textFontBold( column, row ) );
    m_italic->setChecked( cell->textFontItalic(  column, row) );
    m_underline->setChecked( cell->textFontUnderline( column, row ) );
    m_strikeOut->setChecked( cell->textFontStrike( column, row ) );

    m_alignLeft->setChecked( cell->align( column, row ) == KSpreadFormat::Left );
    m_alignCenter->setChecked( cell->align( column, row ) == KSpreadFormat::Center );
    m_alignRight->setChecked( cell->align( column, row ) == KSpreadFormat::Right );

    m_alignTop->setChecked( cell->alignY( column, row ) == KSpreadFormat::Top );
    m_alignMiddle->setChecked( cell->alignY( column, row ) == KSpreadFormat::Middle );
    m_alignBottom->setChecked( cell->alignY( column, row ) == KSpreadFormat::Bottom );

    m_verticalText->setChecked( cell->verticalText( column,row ) );

    m_multiRow->setChecked( cell->multiRow( column,row ) );

    KSpreadCell::FormatType ft = cell->formatType();
    m_percent->setChecked( ft == KSpreadCell::Percentage );
    m_money->setChecked( ft == KSpreadCell::Money );

    if ( m_pTable && !m_pTable->isProtected() )
      m_removeComment->setEnabled( !cell->comment(column,row).isEmpty() );

    if ( m_pTable && !m_pTable->isProtected() )
      m_decreaseIndent->setEnabled( cell->getIndent( column, row ) > 0.0 );

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
      if ( m_bold->isEnabled() )
        adjustActions( false );
    }
    else
    {
      if ( !m_bold->isEnabled() )
        adjustActions( true );
    }
  }
  else if ( table->isProtected() )
  {
    if ( m_bold->isEnabled() )
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
      m_alignLeft->setEnabled(!active);
      m_alignCenter->setEnabled(!active);
      m_alignRight->setEnabled(!active);
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

  m_transform->setEnabled( false );
  m_redo->setEnabled( false );
  m_undo->setEnabled( false );
  if ( !m_pDoc || !m_pDoc->map() || m_pDoc->map()->isProtected() )
  {
    m_showTable->setEnabled( false );
    m_hideTable->setEnabled( false );
  }
  else
  {
    m_showTable->setEnabled( true );
    m_hideTable->setEnabled( true );
  }
  m_gotoCell->setEnabled( true );
  m_viewZoom->setEnabled( true );
  m_showPageBorders->setEnabled( true );
  m_findAction->setEnabled( true);
  m_replaceAction->setEnabled( readwrite );
  if ( !m_pDoc->isReadWrite())
      m_copy->setEnabled( true );
  //  m_newView->setEnabled( true );
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
    m_pTable->setSelectionTextColor( selectionInfo(), m_textColor->color() );
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
    m_pTable->setSelectionbgColor( selectionInfo(), m_bgColor->color() );
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
    m_pTable->setSelectionBorderColor( selectionInfo(), m_borderColor->color() );
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
    m_undo->setEnabled( _b );
  m_undo->setText(i18n("Undo: %1").arg(m_pDoc->undoBuffer()->getUndoName()));
}

void KSpreadView::enableRedo( bool _b )
{
  if ( m_pTable && !m_pTable->isProtected() )
    m_redo->setEnabled( _b );
  m_redo->setText(i18n("Redo: %1").arg(m_pDoc->undoBuffer()->getRedoName()));
}

void KSpreadView::enableInsertColumn( bool _b )
{
  if ( m_pTable && !m_pTable->isProtected() )
    m_insertColumn->setEnabled( _b );
}

void KSpreadView::enableInsertRow( bool _b )
{
  if ( m_pTable && !m_pTable->isProtected() )
    m_insertRow->setEnabled( _b );
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
  QString f( m_selectFont->font() );
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

    m_pTable->borderBottom( m_selectionInfo, m_borderColor->color() );

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
    m_pTable->borderRight( m_selectionInfo, m_borderColor->color() );
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
    m_pTable->borderLeft( m_selectionInfo, m_borderColor->color() );
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
    m_pTable->borderTop( m_selectionInfo, m_borderColor->color() );
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
    m_pTable->borderOutline( m_selectionInfo, m_borderColor->color() );
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
    m_pTable->borderAll( m_selectionInfo, m_borderColor->color() );
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
  if (m_pDoc->map()->findTable( m_pTabBar->listshow().first()))
    setActiveTable( m_pDoc->map()->findTable( m_pTabBar->listshow().first() ));
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
  m_pTabBar->removeAllTabs();

  setActiveTable( 0L );

  m_pDoc->emitEndOperation();
}

void KSpreadView::setActiveTable( KSpreadSheet * _t, bool updateTable )
{
  if ( _t == m_pTable )
    return;

  m_pDoc->emitBeginOperation(false);
  saveCurrentSheetSelection();

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

  // enable/disable state of sheet scroll buttons
  m_pTabBarFirst->setEnabled( m_pTabBar->canScrollLeft() );
  m_pTabBarLeft->setEnabled( m_pTabBar->canScrollLeft() );
  m_pTabBarRight->setEnabled( m_pTabBar->canScrollRight() );
  m_pTabBarLast->setEnabled( m_pTabBar->canScrollRight() );

  m_showPageBorders->setChecked( m_pTable->isShowPageBorders() );
  m_protectSheet->setChecked( m_pTable->isProtected() );
  m_protectDoc->setChecked( m_pDoc->map()->isProtected() );
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
  m_pTabBar->hideTable( table->tableName() );
  updateShowTableMenu();
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::slotTableShown( KSpreadSheet* table )
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->displayTable( table->tableName() );
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

void KSpreadView::slotScrollToFirstTable()
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->scrollFirst();
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::slotScrollToLeftTable()
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->scrollLeft();
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );

  m_pTabBarFirst->setEnabled( m_pTabBar->canScrollLeft() );
  m_pTabBarLeft->setEnabled( m_pTabBar->canScrollLeft() );
  m_pTabBarRight->setEnabled( m_pTabBar->canScrollRight() );
  m_pTabBarLast->setEnabled( m_pTabBar->canScrollRight() );
}

void KSpreadView::slotScrollToRightTable()
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->scrollRight();
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );

  m_pTabBarFirst->setEnabled( m_pTabBar->canScrollLeft() );
  m_pTabBarLeft->setEnabled( m_pTabBar->canScrollLeft() );
  m_pTabBarRight->setEnabled( m_pTabBar->canScrollRight() );
  m_pTabBarLast->setEnabled( m_pTabBar->canScrollRight() );
}

void KSpreadView::slotScrollToLastTable()
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->scrollLast();
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );

  m_pTabBarFirst->setEnabled( m_pTabBar->canScrollLeft() );
  m_pTabBarLeft->setEnabled( m_pTabBar->canScrollLeft() );
  m_pTabBarRight->setEnabled( m_pTabBar->canScrollRight() );
  m_pTabBarLast->setEnabled( m_pTabBar->canScrollRight() );
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

  if ( m_pTabBar->listshow().count() > 1 )
  {
    m_removeTable->setEnabled( true );
    m_hideTable->setEnabled( true );
  }

  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::hideTable()
{
  if ( !m_pTable )
    return;
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->hideTable();
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
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
      m_decreaseIndent->setEnabled( cell->getIndent( column, row ) > 0.0 );

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
       m_protectDoc->setChecked( false );
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
       m_protectDoc->setChecked( true );
       return;
     }

     QCString hash( "" );
     QString password( passwd );
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );
     if ( !m_pDoc->map()->checkPassword( hash ) )
     {
       KMessageBox::error( 0, i18n( "Incorrect password" ) );
       m_protectDoc->setChecked( true );
       return;
     }

     m_pDoc->map()->setProtected( QCString() );
   }

   m_pDoc->setModified( true );
   adjustMapActions( !mode );
}

void KSpreadView::adjustMapActions( bool mode )
{
  m_hideTable->setEnabled( mode );
  m_showTable->setEnabled( mode );
  m_insertTable->setEnabled( mode );
  m_menuInsertTable->setEnabled( mode );
  m_removeTable->setEnabled( mode );

  if ( mode )
  {
    if ( m_pTable && !m_pTable->isProtected() )
    {
      bool state = ( m_pTabBar->listshow().count() > 1 );
      m_removeTable->setEnabled( state );
      m_hideTable->setEnabled( state );
    }
    m_showTable->setEnabled( m_pTabBar->listhide().count() > 0 );
    m_renameTable->setEnabled( m_pTable && !m_pTable->isProtected() );
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
       m_protectSheet->setChecked( false );
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
       m_protectSheet->setChecked( true );
       return;
     }

     QCString hash( "" );
     QString password( passwd );
     if ( password.length() > 0 )
       SHA1::getHash( password, hash );

     if ( !m_pTable->checkPassword( hash ) )
     {
       KMessageBox::error( 0, i18n( "Incorrect password" ) );
       m_protectSheet->setChecked( true );
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
  m_replaceAction->setEnabled( mode );
  m_insertSeries->setEnabled( mode );
  m_insertLink->setEnabled( mode );
  m_insertSpecialChar->setEnabled( mode );
  m_insertFunction->setEnabled( mode );
  m_removeComment->setEnabled( mode );
  m_decreaseIndent->setEnabled( mode );
  m_bold->setEnabled( mode );
  m_italic->setEnabled( mode );
  m_underline->setEnabled( mode );
  m_strikeOut->setEnabled( mode );
  m_percent->setEnabled( mode );
  m_precplus->setEnabled( mode );
  m_precminus->setEnabled( mode );
  m_money->setEnabled( mode );
  m_alignLeft->setEnabled( mode );
  m_alignCenter->setEnabled( mode );
  m_alignRight->setEnabled( mode );
  m_alignTop->setEnabled( mode );
  m_alignMiddle->setEnabled( mode );
  m_alignBottom->setEnabled( mode );
  m_paste->setEnabled( mode );
  m_cut->setEnabled( mode );
  m_specialPaste->setEnabled( mode );
  m_delete->setEnabled( mode );
  m_clearText->setEnabled( mode );
  m_clearComment->setEnabled( mode );
  m_clearValidity->setEnabled( mode );
  m_clearConditional->setEnabled( mode );
  m_recalc_workbook->setEnabled( mode );
  m_recalc_worksheet->setEnabled( mode );
  m_adjust->setEnabled( mode );
  m_editCell->setEnabled( mode );
  if( !mode )
  {
      m_undo->setEnabled( false );
      m_redo->setEnabled( false );
  }
  else
  {
      m_undo->setEnabled( m_pDoc->undoBuffer()->hasUndoActions() );
      m_redo->setEnabled( m_pDoc->undoBuffer()->hasRedoActions() );
  }

  m_paperLayout->setEnabled( mode );
  m_styleDialog->setEnabled( mode );
  m_definePrintRange->setEnabled( mode );
  m_resetPrintRange->setEnabled( mode );
  m_insertFromDatabase->setEnabled( mode );
  m_insertFromTextfile->setEnabled( mode );
  m_insertFromClipboard->setEnabled( mode );
  m_conditional->setEnabled( mode );
  m_validity->setEnabled( mode );
  m_goalSeek->setEnabled( mode );
  m_subTotals->setEnabled( mode );
  m_multipleOperations->setEnabled( mode );
  m_textToColumns->setEnabled( mode );
  m_consolidate->setEnabled( mode );
  m_insertCellCopy->setEnabled( mode );
  m_multiRow->setEnabled( mode );
  m_selectFont->setEnabled( mode );
  m_selectFontSize->setEnabled( mode );
  m_deleteColumn->setEnabled( mode );
  m_hideColumn->setEnabled( mode );
  m_showColumn->setEnabled( mode );
  m_showSelColumns->setEnabled( mode );
  m_insertColumn->setEnabled( mode );
  m_deleteRow->setEnabled( mode );
  m_insertRow->setEnabled( mode );
  m_hideRow->setEnabled( mode );
  m_showRow->setEnabled( mode );
  m_showSelRows->setEnabled( mode );
  m_formulaSelection->setEnabled( mode );
  m_textColor->setEnabled( mode );
  m_bgColor->setEnabled( mode );
  m_cellLayout->setEnabled( mode );
  m_borderLeft->setEnabled( mode );
  m_borderRight->setEnabled( mode );
  m_borderTop->setEnabled( mode );
  m_borderBottom->setEnabled( mode );
  m_borderAll->setEnabled( mode );
  m_borderOutline->setEnabled( mode );
  m_borderRemove->setEnabled( mode );
  m_borderColor->setEnabled( mode );
  m_removeTable->setEnabled( mode );
  m_autoSum->setEnabled( mode );
  //   m_scripts->setEnabled( mode );
  m_default->setEnabled( mode );
  m_areaName->setEnabled( mode );
  m_resizeRow->setEnabled( mode );
  m_resizeColumn->setEnabled( mode );
  m_fontSizeUp->setEnabled( mode );
  m_fontSizeDown->setEnabled( mode );
  m_upper->setEnabled( mode );
  m_lower->setEnabled( mode );
  m_equalizeRow->setEnabled( mode );
  m_equalizeColumn->setEnabled( mode );
  m_verticalText->setEnabled( mode );
  m_addModifyComment->setEnabled( mode );
  m_removeComment->setEnabled( mode );
  m_insertCell->setEnabled( mode );
  m_removeCell->setEnabled( mode );
  m_changeAngle->setEnabled( mode );
  m_dissociateCell->setEnabled( mode );
  m_increaseIndent->setEnabled( mode );
  m_decreaseIndent->setEnabled( mode );
  m_spellChecking->setEnabled( mode );
  m_menuCalcMin->setEnabled( mode );
  m_menuCalcMax->setEnabled( mode );
  m_menuCalcAverage->setEnabled( mode );
  m_menuCalcCount->setEnabled( mode );
  m_menuCalcSum->setEnabled( mode );
  m_menuCalcNone->setEnabled( mode );
  m_insertPart->setEnabled( mode );
  m_createStyle->setEnabled( mode );
  m_selectStyle->setEnabled( mode );

  m_tableFormat->setEnabled( false );
  m_sort->setEnabled( false );
  m_mergeCell->setEnabled( false );
  m_insertChartFrame->setEnabled( false );
  m_sortDec->setEnabled( false );
  m_sortInc->setEnabled( false );
  m_transform->setEnabled( false );

  m_fillRight->setEnabled( false );
  m_fillLeft->setEnabled( false );
  m_fillUp->setEnabled( false );
  m_fillDown->setEnabled( false );

  if ( mode && m_pDoc && m_pDoc->map() && !m_pDoc->map()->isProtected() )
    m_renameTable->setEnabled( true );
  else
    m_renameTable->setEnabled( false );

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

  if ( m_protectChanges->isChecked() )
  {
    if ( !checkChangeRecordPassword() )
      return;
    m_protectChanges->setChecked( false );
  }

  if ( mode )
    m_pTable->map()->startRecordingChanges();
  else
    m_pTable->map()->stopRecordingChanges();

  m_protectChanges->setEnabled( mode );
  m_filterChanges->setEnabled( mode );
  m_acceptRejectChanges->setEnabled( mode );
  m_commentChanges->setEnabled( mode );
}

void KSpreadView::toggleProtectChanges( bool mode )
{
  if ( !m_recordChanges->isChecked() )
  {
    m_protectChanges->setChecked( false );
    return;
  }

   if ( mode )
   {
     QCString passwd;
     int result = KPasswordDialog::getNewPassword( passwd, i18n( "Protect Recorded Changes" ) );
     if ( result != KPasswordDialog::Accepted )
     {
       m_protectChanges->setChecked( false );
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
    m_protectChanges->setChecked( true );
    return false;
  }

  QCString hash( "" );
  QString password( passwd );
  if ( password.length() > 0 )
    SHA1::getHash( password, hash );
  if ( !m_pTable->map()->changes()->checkPassword( hash ) )
  {
    KMessageBox::error( 0, i18n( "Incorrect password" ) );
    m_protectChanges->setChecked( true );
    return false;
  }

  m_pTable->map()->changes()->setProtected( QCString() );
  m_protectChanges->setChecked( false );
  return true;
}

void KSpreadView::filterChanges()
{
  if ( !m_recordChanges->isChecked() )
    return;

  KSpreadFilterDlg dlg( this, m_pTable->map()->changes() );
  dlg.exec();
}

void KSpreadView::acceptRejectChanges()
{
  if ( !m_recordChanges->isChecked() )
    return;

  KSpreadAcceptDlg dlg( this, m_pTable->map()->changes() );
  dlg.exec();
}

void KSpreadView::commentChanges()
{
  if ( !m_recordChanges->isChecked() )
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
  if( m_viewZoom->items().isEmpty() )
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
    m_viewZoom->setItems( lst );
  }

  if( zoom>0 )
  {
    QValueList<int> list;
    bool ok;
    const QStringList itemsList( m_viewZoom->items() );
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
      m_viewZoom->setItems( lst );
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
    m_viewZoom->setCurrentItem( m_viewZoom->items().findIndex( zoomStr ) );

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

  Q_ASSERT(m_pTable);

  if (m_pTable)
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
  if ( !table )
    return;

  bool active = table->getShowFormula();

  if ( table && !table->isProtected() )
  {
    m_alignLeft->setEnabled( !active );
    m_alignCenter->setEnabled( !active );
    m_alignRight->setEnabled( !active );
  }
  active = m_pDoc->getShowFormulaBar();
  editWidget()->showEditWidget( active );

  QString zoomStr( i18n("%1%").arg( m_pDoc->zoom() ) );
  m_viewZoom->setCurrentItem( m_viewZoom->items().findIndex( zoomStr ) );

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

  if (m_pDoc->getShowTabBar())
  {
    m_pTabBarFirst->setGeometry( left, height() - heightHScrollbar,
                                 heightHScrollbar, heightHScrollbar );
    m_pTabBarLeft->setGeometry( left + heightHScrollbar, height() - heightHScrollbar,
                                heightHScrollbar, heightHScrollbar );
    m_pTabBarRight->setGeometry( left + heightHScrollbar * 2, height() - heightHScrollbar,
                                 heightHScrollbar, heightHScrollbar );
    m_pTabBarLast->setGeometry( left + heightHScrollbar * 3, height() - heightHScrollbar,
                                  heightHScrollbar, heightHScrollbar );
    m_pTabBarFirst->show();
    m_pTabBarLeft->show();
    m_pTabBarRight->show();
    m_pTabBarLast->show();
  }
  else
  {
    m_pTabBarFirst->hide();
    m_pTabBarLeft->hide();
    m_pTabBarRight->hide();
    m_pTabBarLast->hide();
  }

  if (!m_pDoc->getShowHorizontalScrollBar())
    m_pTabBar->setGeometry( left + heightHScrollbar * 4, height() - heightHScrollbar,
                            width() - heightHScrollbar * 4, heightHScrollbar );
  else
    m_pTabBar->setGeometry( left + heightHScrollbar * 4, height() - heightHScrollbar,
                            width() / 2 - heightHScrollbar * 4, heightHScrollbar );
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
      m_cellLayout->plug( m_pPopupColumn );
      m_pPopupColumn->insertSeparator();
      m_cut->plug( m_pPopupColumn );
    }
    m_copy->plug( m_pPopupColumn );
    if ( !isProtected )
    {
      m_paste->plug( m_pPopupColumn );
      m_specialPaste->plug( m_pPopupColumn );
      m_insertCellCopy->plug( m_pPopupColumn );
      m_pPopupColumn->insertSeparator();
      m_default->plug( m_pPopupColumn );
      // If there is no selection
      if ((util_isRowSelected(selection()) == FALSE) && (util_isColumnSelected(selection()) == FALSE) )
      {
        m_areaName->plug( m_pPopupColumn );
      }

      m_resizeColumn->plug( m_pPopupColumn );
      m_pPopupColumn->insertItem( i18n("Adjust Column"), this, SLOT(slotPopupAdjustColumn() ) );
      m_pPopupColumn->insertSeparator();
      m_insertColumn->plug( m_pPopupColumn );
      m_deleteColumn->plug( m_pPopupColumn );
      m_hideColumn->plug( m_pPopupColumn );

      m_showSelColumns->setEnabled(false);

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
            m_showSelColumns->setEnabled(true);
            m_showSelColumns->plug( m_pPopupColumn );
            break;
          }
        }

        col = activeTable()->columnFormat( i );

        if ( col->isHide() )
        {
          m_showSelColumns->setEnabled( true );
          m_showSelColumns->plug( m_pPopupColumn );
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
        m_cellLayout->plug( m_pPopupRow );
        m_pPopupRow->insertSeparator();
        m_cut->plug( m_pPopupRow );
    }
    m_copy->plug( m_pPopupRow );
    if ( !isProtected )
    {
      m_paste->plug( m_pPopupRow );
      m_specialPaste->plug( m_pPopupRow );
      m_insertCellCopy->plug( m_pPopupRow );
      m_pPopupRow->insertSeparator();
      m_default->plug( m_pPopupRow );
      // If there is no selection
      if ( (util_isRowSelected(selection()) == FALSE) && (util_isColumnSelected(selection()) == FALSE) )
      {
	m_areaName->plug( m_pPopupRow );
      }

      m_resizeRow->plug( m_pPopupRow );
      m_pPopupRow->insertItem( i18n("Adjust Row"), this, SLOT( slotPopupAdjustRow() ) );
      m_pPopupRow->insertSeparator();
      m_insertRow->plug( m_pPopupRow );
      m_deleteRow->plug( m_pPopupRow );
      m_hideRow->plug( m_pPopupRow );

      m_showSelColumns->setEnabled(false);

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
            m_showSelRows->setEnabled(true);
            m_showSelRows->plug( m_pPopupRow );
            break;
          }
        }

        row = activeTable()->rowFormat( i );
        if ( row->isHide() )
        {
          m_showSelRows->setEnabled(true);
          m_showSelRows->plug( m_pPopupRow );
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
      m_cellLayout->plug( m_pPopupMenu );
      m_pPopupMenu->insertSeparator();
      m_cut->plug( m_pPopupMenu );
    }
    m_copy->plug( m_pPopupMenu );
    if ( !isProtected )
      m_paste->plug( m_pPopupMenu );

    if ( !isProtected )
    {
      m_specialPaste->plug( m_pPopupMenu );
      m_insertCellCopy->plug( m_pPopupMenu );
      m_pPopupMenu->insertSeparator();
      m_delete->plug( m_pPopupMenu );
      m_adjust->plug( m_pPopupMenu );
      m_default->plug( m_pPopupMenu );

      // If there is no selection
      if ( (util_isRowSelected(selection()) == FALSE) && (util_isColumnSelected(selection()) == FALSE) )
      {
        m_areaName->plug( m_pPopupMenu );
        m_pPopupMenu->insertSeparator();
        m_insertCell->plug( m_pPopupMenu );
        m_removeCell->plug( m_pPopupMenu );
      }

      m_pPopupMenu->insertSeparator();
      m_addModifyComment->plug( m_pPopupMenu );
      if ( !cell->comment(m_pCanvas->markerColumn(), m_pCanvas->markerRow()).isEmpty() )
      {
        m_removeComment->plug( m_pPopupMenu );
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

  m_selectStyle->setItems( m_pDoc->styleManager()->styleNames() );
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
  QStringList lst( m_selectStyle->items() );
  lst.push_back( styleName );
  m_selectStyle->setItems( lst );
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
  KoDocumentEntry e =  m_insertPart->documentEntry();//KoPartSelectDia::selectPart( m_pCanvas );
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
  if ( doc()->map()->count() <= 1 || ( m_pTabBar->listshow().count() <= 1 ) )
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
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->slotRename();
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
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
    m_resizeRow->setEnabled( !colSelected );
    m_equalizeRow->setEnabled( !colSelected );
    m_validity->setEnabled( !colSelected && !rowSelected);
    m_conditional->setEnabled( !colSelected && !rowSelected);
    m_resizeColumn->setEnabled( !rowSelected );
    m_equalizeColumn->setEnabled( !rowSelected );
    m_textToColumns->setEnabled( !rowSelected );

    bool simpleSelection = m_selectionInfo->singleCellSelection()
      || colSelected || rowSelected;
    m_tableFormat->setEnabled( !simpleSelection );
    m_sort->setEnabled( !simpleSelection );
    m_mergeCell->setEnabled( !simpleSelection );
    m_fillRight->setEnabled( !simpleSelection );
    m_fillUp->setEnabled( !simpleSelection );
    m_fillDown->setEnabled( !simpleSelection );
    m_fillLeft->setEnabled( !simpleSelection );
    m_insertChartFrame->setEnabled( !simpleSelection );
    m_sortDec->setEnabled( !simpleSelection );
    m_sortInc->setEnabled( !simpleSelection);
    m_createStyle->setEnabled( simpleSelection ); // just from one cell
  }
  m_selectStyle->setCurrentItem( -1 );
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
  if ( m_menuCalcMin->isChecked() )
  {
    doc()->setTypeOfCalc( Min );
  }
  else if ( m_menuCalcMax->isChecked() )
  {
    doc()->setTypeOfCalc( Max );
  }
  else if ( m_menuCalcCount->isChecked() )
  {
    doc()->setTypeOfCalc( Count );
  }
  else if ( m_menuCalcAverage->isChecked() )
  {
    doc()->setTypeOfCalc( Average );
  }
  else if ( m_menuCalcSum->isChecked() )
  {
    doc()->setTypeOfCalc( SumOfNumber );
  }
  else if ( m_menuCalcNone->isChecked() )
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
    m_transform->setEnabled( TRUE );

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
    m_transform->setEnabled( FALSE );

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
  if ( m_pTable )
  {
      m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );

      if ( ev->activated() )
      {
          if ( m_sbCalcLabel )
              resultOfCalc();
      }
      else
      {
          /*if (m_sbCalcLabel)
            {
            disconnect(m_sbCalcLabel,SIGNAL(pressed( int )),this,SLOT(statusBarClicked(int)));
            }*/
      }
  }

  KoView::guiActivateEvent( ev );
}

void KSpreadView::openPopupMenuMenuPage( const QPoint & _point )
{
  if ( !koDocument()->isReadWrite() || !factory() )
    return;
  if ( m_pTabBar )
  {
    bool state = ( m_pTabBar->listshow().count() > 1 );
    if ( m_pTable && m_pTable->isProtected() )
    {
      m_removeTable->setEnabled( false );
      m_hideTable->setEnabled( false );
      m_showTable->setEnabled( false );
    }
    else
    {
      m_removeTable->setEnabled( state);
      m_hideTable->setEnabled( state );
      m_showTable->setEnabled(m_pTabBar->listhide().count() > 0);
    }
    if ( !m_pDoc || !m_pDoc->map() || m_pDoc->map()->isProtected() )
    {
      m_insertTable->setEnabled( false );
      m_renameTable->setEnabled( false );
      m_showTable->setEnabled( false );
      m_hideTable->setEnabled( false );
      m_removeTable->setEnabled( false );
    }
    static_cast<QPopupMenu*>(factory()->container("menupage_popup",this))->popup(_point);
  }
}

void KSpreadView::updateBorderButton()
{
  //  m_pDoc->emitBeginOperation( false );
  if ( m_pTable )
    m_showPageBorders->setChecked( m_pTable->isShowPageBorders() );
  //  m_pDoc->emitEndOperation();
}

void KSpreadView::removeTable( KSpreadSheet *_t )
{
  m_pDoc->emitBeginOperation(false);
  QString m_tablName=_t->tableName();
  m_pTabBar->removeTab( m_tablName );
  setActiveTable( m_pDoc->map()->findTable( m_pTabBar->listshow().first() ));

  bool state = m_pTabBar->listshow().count() > 1;
  m_removeTable->setEnabled( state );
  m_hideTable->setEnabled( state );
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
  else
  {
    m_pTabBar->addHiddenTab( tabName );
  }
  bool state = ( m_pTabBar->listshow().count() > 1 );
  m_removeTable->setEnabled( state );
  m_hideTable->setEnabled( state );
  m_pDoc->emitEndOperation( table->visibleRect( m_pCanvas ) );
}

QColor KSpreadView::borderColor() const
{
  return m_borderColor->color();
}

void KSpreadView::updateShowTableMenu()
{
  m_pDoc->emitBeginOperation( false );
  if ( m_pTable->isProtected() )
    m_showTable->setEnabled( false );
  else
    m_showTable->setEnabled( m_pTabBar->listhide().count() > 0 );
  m_pDoc->emitEndOperation( m_pTable->visibleRect( m_pCanvas ) );
}

void KSpreadView::closeEditor()
{
  if ( m_pTable ) { // #45822
    m_pDoc->emitBeginOperation( false );
    m_pCanvas->closeEditor();
    m_pDoc->emitEndOperation( selectionInfo()->selection() );
  }
}


void KSpreadView::paintUpdates()
{
  /* don't do any begin/end operation here -- this is what is called at an
     endOperation
  */
  m_pCanvas->paintUpdates();
}

QPoint KSpreadView::markerFromSheet( KSpreadSheet *_sheet ) const
{
    QMapIterator<KSpreadSheet*, QPoint> it2 = savedMarkers.find(_sheet);
    QPoint newMarker = (it2 == savedMarkers.end()) ? QPoint(1,1) : *it2;
    return newMarker;
}

void KSpreadView::saveCurrentSheetSelection()
{
    /* save the current selection on this table */
    if (m_pTable != NULL)
    {
        savedAnchors.replace(m_pTable, selectionInfo()->selectionAnchor());
        savedMarkers.replace(m_pTable, selectionInfo()->marker());
    }
}
#include "kspread_view.moc"

