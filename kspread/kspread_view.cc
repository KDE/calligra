/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include <qtoolbutton.h>
#include <qtimer.h>
#include <qcursor.h>
#include <qpaintdevicemetrics.h>
#include <qregexp.h>
#include <qlayout.h>

#include <kprocio.h>
#include <kspell.h>
#include <kspelldlg.h>
#include <kstdaction.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <ktempfile.h>
#include <kstandarddirs.h>
#include <tkcoloractions.h>

#include <dcopclient.h>
#include <dcopref.h>

#include <koReplace.h>
#include <koMainWindow.h>
#include <koPartSelectAction.h>
#include <koTemplateCreateDia.h>

#include <kparts/partmanager.h>


#include "kspread_map.h"
#include "kspread_dlg_scripts.h"
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
#include "kspread_handler.h"
#include "kspread_events.h"
#include "kspread_editors.h"
#include "kspread_dlg_format.h"
// #include "kspread_dlg_oszi.h"
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
#include "kspread_dlg_list.h"
#include "kspread_undo.h"
#include <kconfig.h>
#include "handler.h"

#include "KSpreadViewIface.h"
#include <kdebug.h>
#include <kstatusbar.h>
#include <kapplication.h>
#include "kspread_dlg_paperlayout.h"


/*****************************************************************************
 *
 * KSpreadView
 *
 *****************************************************************************/

KSpreadScripts* KSpreadView::m_pGlobalScriptsDialog = 0L;

// non flickering version of KSpell.
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

KSpreadView::KSpreadView( QWidget *_parent, const char *_name, KSpreadDoc* doc ) :
  KoView( doc, _parent, _name )
{
    m_popupMenuFirstToolId = 0;
    kdDebug(36001) << "sizeof(KSpreadCell)=" << sizeof(KSpreadCell) <<endl;
    setInstance( KSpreadFactory::global() );
    if ( doc->isReadWrite() )
      setXMLFile( "kspread.rc" );
    else
      setXMLFile( "kspread_readonly.rc" );

    m_pTable = NULL;
    m_toolbarLock = FALSE;
    m_sbCalcLabel=0;
    m_pDoc = doc;
    m_pPopupMenu = 0;
    m_pPopupColumn = 0;
    m_pPopupRow = 0;
    m_popupChild = 0;
    m_popupListChoose=0;
    m_spell.kspell = 0;
    // a few words to ignore when spell checking

    m_dcop = 0;
    dcopObject(); // build it
    m_bLoading =false;

    m_selectionInfo = new KSpreadSelection(this);

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

    m_pCancelButton = newIconButton( "abort", TRUE, m_pToolWidget );
    hbox->addWidget( m_pCancelButton );
    m_pOkButton = newIconButton( "done", TRUE, m_pToolWidget );
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

    KStatusBar * sb = statusBar();
    Q_ASSERT(sb);
    m_sbCalcLabel = sb ? new KStatusBarLabel( QString::null, 0, sb ) : 0;
    addStatusBarItem( m_sbCalcLabel, 0 );
    if(m_sbCalcLabel)
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

    KSpreadSheet *tbl;
    for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
      addTable( tbl );
    tbl = 0L;
    if ( m_pDoc->isEmbedded() )
    {
        tbl = m_pDoc->displayTable();
    }
    if ( !tbl )
        tbl = m_pDoc->map()->initialActiveTable();
    if (tbl)
      setActiveTable(tbl);
    else
      //activate first table which is not hiding
      setActiveTable(m_pDoc->map()->findTable(m_pTabBar->listshow().first()));

    QObject::connect( m_pDoc, SIGNAL( sig_addTable( KSpreadSheet* ) ), SLOT( slotAddTable( KSpreadSheet* ) ) );


    QObject::connect( m_pDoc, SIGNAL( sig_refreshView(  ) ), this, SLOT( refreshView() ) );

    QObject::connect( m_pDoc, SIGNAL( sig_refreshLocale() ), this, SLOT( refreshLocale()));

    KoView::setZoom( m_pDoc->zoomedResolutionY() /* KoView only supports one zoom */ ); // initial value
    //when kspread is embedded into konqueror apply a zoom=100
    //in konqueror we can't change zoom -- ### TODO ?
    if(!m_pDoc->isReadWrite())
    {
        setZoom( 100, true );
    }

    viewZoom( QString::number( m_pDoc->zoom() ) );

    QStringList list = m_viewZoom->items();
    QString zoomStr = QString::number(m_pDoc->zoom() ) + '%';
    m_viewZoom->setCurrentItem( list.findIndex(zoomStr)  );

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
  m_menuCalcSum->setToolTip(i18n("Calculate using sum"));

  /*******************************/
  m_menuCalcMin = new KToggleAction( i18n("Min"), 0, actionCollection(),
                                     "menu_min");
  connect( m_menuCalcMin, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  m_menuCalcMin->setExclusiveGroup( "Calc" );
  m_menuCalcMin->setToolTip(i18n("Calculate using minimum"));

  /*******************************/
  m_menuCalcMax = new KToggleAction( i18n("Max"), 0, actionCollection(),
                                     "menu_max");
  connect( m_menuCalcMax, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  m_menuCalcMax->setExclusiveGroup( "Calc" );
  m_menuCalcMax->setToolTip(i18n("Calculate using maximum"));

  /*******************************/
  m_menuCalcAverage = new KToggleAction( i18n("Average"), 0, actionCollection(),
                                         "menu_average");
  connect( m_menuCalcAverage, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  m_menuCalcAverage->setExclusiveGroup( "Calc" );
  m_menuCalcAverage->setToolTip(i18n("Calculate using average"));

  /*******************************/
  m_menuCalcCount = new KToggleAction( i18n("Count"), 0, actionCollection(),
                                       "menu_count");
  connect( m_menuCalcCount, SIGNAL( toggled( bool ) ), this,
           SLOT( menuCalc( bool ) ) );
  m_menuCalcCount->setExclusiveGroup( "Calc" );
  m_menuCalcCount->setToolTip(i18n("Calculate using the count"));


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
  KAction* tmpAction = NULL;
  tmpAction = new KAction( i18n("&Function..."), "funct", 0, this,
                           SLOT( insertMathExpr() ), actionCollection(),
                           "insertMathExpr" );
  tmpAction->setToolTip(i18n("Insert math expression."));

  tmpAction = new KAction( i18n("&Series..."),"series", 0, this,
                           SLOT( insertSeries() ), actionCollection(), "series");
  tmpAction ->setToolTip(i18n("Insert a series."));

  tmpAction = new KAction( i18n("&Link..."), 0, this,
                           SLOT( insertHyperlink() ), actionCollection(),
                           "insertHyperlink" );
  tmpAction->setToolTip(i18n("Insert an internet hyperlink."));

  m_insertPart=new KoPartSelectAction( i18n("&Object..."), "frame_query", this,
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
  m_insertFromDatabase->setToolTip(i18n("Insert data from a SQL database"));
#endif

  m_insertFromTextfile = new KAction( i18n("From &Text File..."), 0, this,
                                      SLOT( insertFromTextfile() ),
                                      actionCollection(), "insertFromTextfile");
  m_insertFromTextfile->setToolTip(i18n("Insert data from a text file to the current cursor position/selection"));
  m_insertFromClipboard = new KAction( i18n("From &Clipboard..."), 0, this,
                                      SLOT( insertFromClipboard() ),
                                      actionCollection(), "insertFromClipboard");
  m_insertFromClipboard->setToolTip(i18n("Insert csv data from the clipboard to the current cursor position/selection"));

}

void KSpreadView::initializeEditActions()
{
  m_copy = KStdAction::copy( this, SLOT( copySelection() ), actionCollection(),
                             "copy" );
  m_copy->setToolTip(i18n("Copy the cell object to the clipboard."));

  m_paste = KStdAction::paste( this, SLOT( paste() ), actionCollection(),
                               "paste" );
  m_paste->setToolTip(i18n("Paste the contents of the clipboard at the cursor"));

  m_cut = KStdAction::cut( this, SLOT( cutSelection() ), actionCollection(),
                           "cut" );
  m_cut->setToolTip(i18n("Move the cell object to the clipboard."));

  m_specialPaste = new KAction( i18n("Special Paste..."), "special_paste",0,
                                this, SLOT( specialPaste() ), actionCollection(),
                                "specialPaste" );
  m_specialPaste->setToolTip
    (i18n("Paste the contents of the clipboard with special options."));

  m_undo = KStdAction::undo( this, SLOT( undo() ), actionCollection(), "undo" );
  m_undo->setEnabled( FALSE );
  m_undo->setToolTip(i18n("Undo the previous action."));

  m_redo = KStdAction::redo( this, SLOT( redo() ), actionCollection(), "redo" );
  m_redo->setEnabled( FALSE );
  m_redo->setToolTip(i18n("Redo the action that has been undone."));

  m_find = KStdAction::find(this, SLOT(find()), actionCollection());

  KStdAction::replace(this, SLOT(replace()), actionCollection());
}

void KSpreadView::initializeAreaOperationActions()
{
  m_areaName = new KAction( i18n("Area Name..."), 0, this,
                            SLOT( setAreaName() ), actionCollection(),
                            "areaname" );
  m_areaName->setToolTip(i18n("Set a name for a region of the spreadsheet"));

  m_showArea = new KAction( i18n("Show Area..."), 0, this,
                            SLOT( showAreaName() ), actionCollection(),
                            "showArea" );
  m_showArea->setToolTip(i18n("Display a named area"));

  m_sortList = new KAction( i18n("Custom Lists..."), 0, this,
                            SLOT( sortList() ), actionCollection(),
                            "sortlist" );
  m_sortList->setToolTip(i18n("Create custom lists for sorting or autofill"));

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
  m_goalSeek->setToolTip( i18n("Repeating calculation to find a specific value") );

  m_multipleOperations = new KAction( i18n("&Multiple Operations..."), 0, this,
                            SLOT( multipleOperations() ), actionCollection(), "multipleOperations" );
  m_multipleOperations->setToolTip( i18n("Apply the same formula to various cells using different values for the parameter.") );

  m_subTotals = new KAction( i18n("&Subtotals..."), 0, this,
                             SLOT( subtotals() ), actionCollection(), "subtotals" );
  m_subTotals->setToolTip( i18n("Create different kind of subtotals to a list or database.") );

  m_textToColumns = new KAction( i18n("&Text to Columns..."), 0, this,
                            SLOT( textToColumns() ), actionCollection(), "textToColumns" );
  m_textToColumns->setToolTip( i18n("Expand the content of cells to multiple columns") );

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
  m_showPageBorders->setToolTip("Show on the spreadsheet where the page borders will be.");

  m_viewZoom = new KSelectAction( i18n( "Zoom" ), "viewmag", 0,
                                  actionCollection(), "view_zoom" );

  connect( m_viewZoom, SIGNAL( activated( const QString & ) ),
           this, SLOT( viewZoom( const QString & ) ) );


  m_viewZoom->setEditable(true);

  QStringList lst1;
  lst1 << "33%";
  lst1 << "50%";
  lst1 << "60%";
  lst1 << "75%";
  lst1 << "100%";
  lst1 << "125%";
  lst1 << "150%";
  lst1 << "200%";
  lst1 << "250%";
  lst1 << "350%";
  lst1 << "400%";
  lst1 << "450%";
  lst1 << "500%";

  m_viewZoom->setItems( lst1 );

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


  m_paperLayout = new KAction( i18n("Paper Layout..."), 0, this,
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

  m_insertCellCopy = new KAction( i18n("Paste with Insertion..."),
                                  "insertcellcopy", 0, this,
                                  SLOT( slotInsertCellCopy() ),
                                  actionCollection(), "insertCellCopy" );
  m_insertCellCopy->setToolTip(i18n("Inserts a cell from the clipboard into the spreadsheet."));

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
  m_upper = new KAction( i18n("Upper Case"), "upper", 0, this,
                         SLOT( upper() ), actionCollection(), "upper" );
  m_upper->setToolTip(i18n("Convert all letters to upper case."));

  /*******************************/
  m_lower = new KAction( i18n("Lower Case"), "lower", 0, this,
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
  m_menuInsertTable = new KAction( i18n("Sheet"),"inserttable", 0, this,
                               SLOT( insertTable() ), actionCollection(),
                               "menuInsertTable" );
  m_menuInsertTable->setToolTip(i18n("Insert a new sheet."));

  m_removeTable = new KAction( i18n("Remove Sheet..."), "delete_table",0,this,
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

  m_tableFormat = new KAction( i18n("Sheet Style..."), 0, this,
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

  connect(m_borderColor,SIGNAL(activated()),SLOT(changeBorderColor()));
  m_borderColor->setToolTip(i18n("Select a new border color."));

}

KSpreadView::~KSpreadView()
{
    deleteEditor( true );
    if ( !m_transformToolBox.isNull() )
	delete (&*m_transformToolBox);
    /*if(m_sbCalcLabel)
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
}


void KSpreadView::initConfig()
{
    KConfig *config = KSpreadFactory::global()->config();
    if( config->hasGroup("Parameters" ))
        {
        config->setGroup( "Parameters" );
        m_pDoc->setShowHorizontalScrollBar(config->readBoolEntry("Horiz ScrollBar",true));
        m_pDoc->setShowVerticalScrollBar(config->readBoolEntry("Vert ScrollBar",true));
        m_pDoc->setShowColHeader(config->readBoolEntry("Column Header",true));
        m_pDoc->setShowRowHeader(config->readBoolEntry("Row Header",true));
        m_pDoc->setCompletionMode((KGlobalSettings::Completion)config->readNumEntry("Completion Mode",(int)(KGlobalSettings::CompletionAuto)));
        m_pDoc->setMoveToValue((KSpread::MoveTo)config->readNumEntry("Move",(int)(KSpread::Bottom)));
        m_pDoc->setIndentValue( config->readNumEntry( "Indent", 10.0 ) );
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

 if(  config->hasGroup("KSpread Color" ) )
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
if( config->hasGroup("KSpread Page Layout" ) )
 {
   config->setGroup( "KSpread Page Layout" );
   if( m_pTable->isEmpty())
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
    if(shell())
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
  KSpreadSheet *tbl;
  m_pDoc->emitBeginOperation(true);
    for ( tbl = m_pDoc->map()->firstTable();
	  tbl != 0L;
          tbl = m_pDoc->map()->nextTable() )
    {
      bool b = tbl->getAutoCalc();
      tbl->setAutoCalc(true);
      tbl->recalc();
      tbl->setAutoCalc(b);
    }


  m_pDoc->emitEndOperation();
}

void KSpreadView::refreshLocale()
{
  m_pDoc->emitBeginOperation(true);
  KSpreadSheet *tbl;
  for ( tbl = m_pDoc->map()->firstTable();
        tbl != 0L;
        tbl = m_pDoc->map()->nextTable() ){
    tbl->updateLocale();
  }
  m_pDoc->emitEndOperation();
}

void KSpreadView::recalcWorkSheet()
{
  m_pDoc->emitBeginOperation(true);
  if (m_pTable != 0)
  {
    bool b = m_pTable->getAutoCalc();
    m_pTable->setAutoCalc(true);
    m_pTable->recalc();
    m_pTable->setAutoCalc(b);
  }
  m_pDoc->emitEndOperation();
}


void KSpreadView::extraSpelling()
{
  if (m_spell.kspell)
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
    if(m_pDoc->getKSpellConfig())
    {
        m_pDoc->getKSpellConfig()->setIgnoreList(m_pDoc->spellListIgnoreAll());
#if KDE_VERSION >= 305
        m_pDoc->getKSpellConfig()->setReplaceAllList(m_spell.replaceAll);
#endif

    }
    m_spell.kspell = new KSpreadSpell( this, i18n( "Spell Checking" ), this,
                                     SLOT( spellCheckerReady() ),
                                     m_pDoc->getKSpellConfig() );

  m_spell.kspell->setIgnoreUpperWords(m_pDoc->dontCheckUpperWord());
  m_spell.kspell->setIgnoreTitleCase(m_pDoc->dontCheckTitleCase());

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

#if KDE_VERSION >= 305
  QObject::connect( m_spell.kspell, SIGNAL( replaceall( const QString &  ,  const QString & )), this, SLOT( spellCheckerReplaceAll( const QString &  ,  const QString & )));
#endif

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
      if ( !m_spell.currentCell->isDefault() && m_spell.currentCell->isString() )
      {
        m_spell.kspell->check( m_spell.currentCell->text(), true );

        return;
      }

      m_spell.currentCell = m_spell.currentCell->nextCell();
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
      if (cell->isDefault() || !cell->isString())
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
  if (m_pCanvas)
    m_pCanvas->setCursor( ArrowCursor );

  m_spell.kspell->cleanUp();
  delete m_spell.kspell;
  m_spell.kspell            = 0L;
  m_spell.firstSpellTable   = 0L;
  m_spell.currentSpellTable = 0L;
  m_spell.currentCell       = 0L;
#if KDE_VERSION >= 305
  m_spell.replaceAll.clear();
#endif


  KMessageBox::information( this, i18n( "Spell checking is complete." ) );

  if(m_spell.macroCmdSpellCheck)
      m_pDoc->undoBuffer()->appendUndo( m_spell.macroCmdSpellCheck );
  m_spell.macroCmdSpellCheck=0L;

}


bool KSpreadView::spellSwitchToOtherTable()
{
  // there is no other table
  if(m_pDoc->map()->count()==1)
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
  if( m_spell.currentSpellTable == m_spell.firstSpellTable )
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

  setActiveTable(m_spell.currentSpellTable);

  return true;
}


void KSpreadView::spellCheckerMisspelling( const QString &,
                                           const QStringList &,
                                           unsigned int )
{
  // scroll to the cell
  if (!m_spell.spellCheckSelection)
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

  if(!m_spell.macroCmdSpellCheck)
      m_spell.macroCmdSpellCheck=new KSpreadMacroUndoAction( m_pDoc, i18n("Correct Misspelled Word") );
  m_spell.macroCmdSpellCheck->addCommand(undo);
  m_pDoc->emitEndOperation();
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
#if KDE_VERSION >= 305
    m_spell.replaceAll.clear();
#endif

    if(m_spell.macroCmdSpellCheck)
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
#if KDE_VERSION >= 305
  m_spell.replaceAll.clear();
#endif

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

  if(m_spell.macroCmdSpellCheck)
  {
      m_pDoc->undoBuffer()->appendUndo( m_spell.macroCmdSpellCheck );
  }
  m_spell.macroCmdSpellCheck=0L;


  if(kspellNotConfigured)
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
    if ( col <= 0 ) col = 1;
    int row = m_pDoc->map()->initialMarkerRow();
    if ( row <= 0 ) row = 1;
    m_pCanvas->gotoLocation( col, row );

    //init toggle button
    updateBorderButton();
    updateShowTableMenu();
    m_tableFormat->setEnabled(false);
    m_mergeCell->setEnabled(false);
    m_insertChartFrame->setEnabled(false);

// I don't think we should recalculate everything after loading, this takes much to much time
// and shouldn't be necessary at all - Philipp
//
//     /*recalc all dependent after loading*/
//     KSpreadSheet *tbl;
//     for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
//     {
//         if( tbl->getAutoCalc() )
//             tbl->recalc();
//         tbl->refreshMergedCell();
//     }

    slotUpdateView( activeTable() );
    m_bLoading =true;

    if ( koDocument()->isReadWrite() )
      initConfig();
}


void KSpreadView::updateButton( KSpreadCell *cell, int column, int row)
{
    m_toolbarLock = TRUE;

    QColor color=cell->textColor( column, row );
    if(!color.isValid())
        color=QApplication::palette().active().text();
    m_textColor->setCurrentColor( color );

    color=cell->bgColor(  column, row );

    if(!color.isValid())
        color=QApplication::palette().active().base();

    m_bgColor->setCurrentColor( color );

    m_selectFontSize->setFontSize( cell->textFontSize( column, row ) );
    m_selectFont->setFont( cell->textFontFamily( column,row ) );
    m_bold->setChecked( cell->textFontBold( column, row ) );
    m_italic->setChecked( cell->textFontItalic(  column, row) );
    m_underline->setChecked( cell->textFontUnderline( column, row ) );
    m_strikeOut->setChecked( cell->textFontStrike( column, row ) );

    m_alignLeft->setChecked( cell->align( column, row ) == KSpreadLayout::Left );
    m_alignCenter->setChecked( cell->align( column, row ) == KSpreadLayout::Center );
    m_alignRight->setChecked( cell->align( column, row ) == KSpreadLayout::Right );

    m_alignTop->setChecked( cell->alignY( column, row ) == KSpreadLayout::Top );
    m_alignMiddle->setChecked( cell->alignY( column, row ) == KSpreadLayout::Middle );
    m_alignBottom->setChecked( cell->alignY( column, row ) == KSpreadLayout::Bottom );

    m_verticalText->setChecked( cell->verticalText( column,row ) );

    m_multiRow->setChecked( cell->multiRow( column,row ) );

    KSpreadCell::FormatType ft = cell->formatType();
    m_percent->setChecked( ft == KSpreadCell::Percentage );
    m_money->setChecked( ft == KSpreadCell::Money );

    m_removeComment->setEnabled( !cell->comment(column,row).isEmpty() );

    m_decreaseIndent->setEnabled( cell->getIndent( column, row ) > 0.0 );

    m_toolbarLock = FALSE;

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
    else
        editWidget()->setText( cell->text() );
    updateButton(cell, column, row);

}

void KSpreadView::updateEditWidget()
{
    bool active=activeTable()->getShowFormula();
    m_alignLeft->setEnabled(!active);
    m_alignCenter->setEnabled(!active);
    m_alignRight->setEnabled(!active);


    int column = m_pCanvas->markerColumn();
    int row    = m_pCanvas->markerRow();

    KSpreadCell* cell = m_pTable->cellAt( column, row );
    if ( !cell )
    {
        editWidget()->setText( "" );
        return;
    }

    if ( cell->content() == KSpreadCell::VisualFormula )
    {
        editWidget()->setText( "" );
    }
    else
    {
        editWidget()->setText( cell->text() );
    }


    if ( m_pCanvas->editor() )
    {
      m_pCanvas->editor()->setEditorFont(cell->textFont(column, row), true);
      m_pCanvas->editor()->setFocus();
    }
    updateButton(cell, column, row);
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
  m_showTable->setEnabled( true );
  m_hideTable->setEnabled( true );
  m_gotoCell->setEnabled( true );
  m_viewZoom->setEnabled( true );
  m_showPageBorders->setEnabled( true );
  m_find->setEnabled( true);
  if ( !m_pDoc->isReadWrite())
      m_copy->setEnabled( true );
  //  m_newView->setEnabled( true );
  //m_pDoc->KXMLGUIClient::action( "newView" )->setEnabled( true ); // obsolete (Werner)
  // m_oszi->setEnabled( true );
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
        while ( cell && cell->isNumeric() );

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
        while ( cell && cell->isNumeric() );

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
  m_pDoc->emitEndOperation();
}

void KSpreadView::setSelectionTextColor(const QColor &txtColor)
{
  m_pDoc->emitBeginOperation(false);
  if (m_pTable != 0L)
  {
    m_pTable->setSelectionTextColor( selectionInfo(), txtColor );
  }
  m_pDoc->emitEndOperation();
}

void KSpreadView::changeBackgroundColor()
{
  m_pDoc->emitBeginOperation(false);
  if ( m_pTable != 0L )
  {
    m_pTable->setSelectionbgColor( selectionInfo(), m_bgColor->color() );
  }
  m_pDoc->emitEndOperation();
}

void KSpreadView::setSelectionBackgroundColor(const QColor &bgColor)
{
  m_pDoc->emitBeginOperation(false);
  if (m_pTable != 0L)
  {
    m_pTable->setSelectionbgColor( selectionInfo(), bgColor );
  }
  m_pDoc->emitEndOperation();
}

void KSpreadView::changeBorderColor()
{
  m_pDoc->emitBeginOperation(false);
  if ( m_pTable != 0L )
  {
    m_pTable->setSelectionBorderColor( selectionInfo(), m_borderColor->color() );
  }
  m_pDoc->emitEndOperation();
}

void KSpreadView::setSelectionBorderColor(const QColor &bdColor)
{
  m_pDoc->emitBeginOperation(false);
  if (m_pTable != 0L)
  {
    m_pTable->setSelectionBorderColor( selectionInfo(), bdColor );
  }
  m_pDoc->emitEndOperation();
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
    m_undo->setEnabled( _b );
    m_undo->setText(i18n("Undo: %1").arg(m_pDoc->undoBuffer()->getUndoName()));
}

void KSpreadView::enableRedo( bool _b )
{
    m_redo->setEnabled( _b );
    m_redo->setText(i18n("Redo: %1").arg(m_pDoc->undoBuffer()->getRedoName()));
}

void KSpreadView::undo()
{
  m_pDoc->emitBeginOperation(false);
  m_pDoc->undo();

  updateEditWidget();

  resultOfCalc();
  m_pDoc->emitEndOperation();
}

void KSpreadView::redo()
{
  m_pDoc->emitBeginOperation(false);
  m_pDoc->redo();

  updateEditWidget();
  resultOfCalc();
  m_pDoc->emitEndOperation();
}

void KSpreadView::deleteColumn()
{

  if ( !m_pTable )
    return;

  m_pDoc->emitBeginOperation(false);

  QRect r( m_selectionInfo->selection() );

  m_pTable->removeColumn( r.left(),(r.right()-r.left()) );

  updateEditWidget();
  m_selectionInfo->setSelection(m_selectionInfo->marker(),
                                m_selectionInfo->marker(), m_pTable);
  m_pDoc->emitEndOperation();
}

void KSpreadView::deleteRow()
{
    if ( !m_pTable )
        return;

    m_pDoc->emitBeginOperation(false);
    QRect r( m_selectionInfo->selection() );
    m_pTable->removeRow( r.top(),(r.bottom()-r.top()) );

    updateEditWidget();
    m_selectionInfo->setSelection(m_selectionInfo->marker(),
                                  m_selectionInfo->marker(), m_pTable);
    m_pDoc->emitEndOperation();
}

void KSpreadView::insertColumn()
{
    if ( !m_pTable )
        return;
    m_pDoc->emitBeginOperation(false);
    QRect r( m_selectionInfo->selection() );
    m_pTable->insertColumn( r.left(),(r.right()-r.left()) );


    updateEditWidget();
    m_pDoc->emitEndOperation();
}

void KSpreadView::hideColumn()
{
    if ( !m_pTable )
        return;
    m_pDoc->emitBeginOperation(false);
    QRect r( m_selectionInfo->selection() );
    m_pTable->hideColumn( r.left(),(r.right()-r.left()) );
    m_pDoc->emitEndOperation();
}

void KSpreadView::showColumn()
{
    if ( !m_pTable )
        return;
    m_pDoc->emitBeginOperation(false);
    KSpreadShowColRow dlg( this,"showCol",KSpreadShowColRow::Column);
    dlg.exec();
    m_pDoc->emitEndOperation();
}

void KSpreadView::showSelColumns()
{
    if ( !m_pTable )
        return;

    int i;
    QRect rect = m_selectionInfo->selection();
    ColumnLayout * col;
    QValueList<int>hiddenCols;

    m_pDoc->emitBeginOperation(false);

    for ( i = rect.left(); i <= rect.right(); ++i )
    {
      if (i == 2) // "B"
      {
        col = activeTable()->columnLayout( 1 );
        if ( col->isHide() )
        {
          hiddenCols.append( 1 );
        }
      }

      col = m_pTable->columnLayout( i );
      if ( col->isHide() )
      {
	hiddenCols.append(i);
      }
    }

    if (hiddenCols.count() > 0)
      m_pTable->showColumn(0, -1, hiddenCols);

    m_pDoc->emitEndOperation();
}

void KSpreadView::insertRow()
{
    if ( !m_pTable )
        return;
    m_pDoc->emitBeginOperation(false);
    QRect r( m_selectionInfo->selection() );
    m_pTable->insertRow( r.top(),(r.bottom()-r.top()) );

    updateEditWidget();
    m_pDoc->emitEndOperation();
}

void KSpreadView::hideRow()
{
    if ( !m_pTable )
        return;
    m_pDoc->emitBeginOperation(false);
    QRect r( m_selectionInfo->selection() );
    m_pTable->hideRow( r.top(),(r.bottom()-r.top()) );
    m_pDoc->emitEndOperation();
}

void KSpreadView::showRow()
{
    if ( !m_pTable )
        return;
    m_pDoc->emitBeginOperation(false);
    KSpreadShowColRow dlg( this,"showRow",KSpreadShowColRow::Row);
    dlg.exec();
    m_pDoc->emitEndOperation();
}

void KSpreadView::showSelRows()
{
    if ( !m_pTable )
        return;

    int i;
    QRect rect = m_selectionInfo->selection();
    RowLayout * row;
    QValueList<int>hiddenRows;

    m_pDoc->emitBeginOperation(false);

    for ( i = rect.top(); i <= rect.bottom(); ++i )
    {
      if (i == 2)
      {
        row = activeTable()->rowLayout( 1 );
        if ( row->isHide() )
        {
          hiddenRows.append(1);
        }
      }

      row = m_pTable->rowLayout( i );
      if ( row->isHide() )
      {
	hiddenRows.append(i);
      }
    }

    if (hiddenRows.count() > 0)
      m_pTable->showRow(0, -1, hiddenRows);

    m_pDoc->emitEndOperation();
}

void KSpreadView::fontSelected( const QString &_font )
{
    if ( m_toolbarLock )
        return;

    m_pDoc->emitBeginOperation(false);
    if ( m_pTable != 0L )
      m_pTable->setSelectionFont( m_selectionInfo, _font.latin1() );

    // Dont leave the focus in the toolbars combo box ...
    if ( m_pCanvas->editor() )
    {
      KSpreadCell * cell = m_pTable->cellAt(m_selectionInfo->marker());
      m_pCanvas->editor()->setEditorFont(cell->textFont(cell->column(), cell->row()), true);
      m_pCanvas->editor()->setFocus();
    }
    else
      m_pCanvas->setFocus();

    m_pDoc->emitEndOperation();
}

void KSpreadView::decreaseFontSize()
{
  setSelectionFontSize(-1);
}

void KSpreadView::increaseFontSize()
{
  setSelectionFontSize(1);
}

void KSpreadView::setSelectionFontSize(int size)
{
  if (m_pTable != NULL)
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->setSelectionSize(selectionInfo(), size);
    m_pDoc->emitBeginOperation();
  }
}

void KSpreadView::lower()
{
    if( !m_pTable  )
        return;

    m_pDoc->emitBeginOperation(false);

    m_pTable->setSelectionUpperLower( selectionInfo(), -1 );
    updateEditWidget();

    m_pDoc->emitEndOperation();
}

void KSpreadView::upper()
{
    if( !m_pTable  )
        return;

    m_pDoc->emitBeginOperation(false);

    m_pTable->setSelectionUpperLower( selectionInfo(), 1 );
    updateEditWidget();

    m_pDoc->emitEndOperation();
}

void KSpreadView::firstLetterUpper()
{
    if( !m_pTable  )
        return;
    m_pDoc->emitBeginOperation(false);
    m_pTable->setSelectionfirstLetterUpper( selectionInfo() );
    updateEditWidget();
    m_pDoc->emitEndOperation();
}

void KSpreadView::verticalText(bool b)
{
    if( !m_pTable  )
        return;

    m_pDoc->emitBeginOperation(false);
    m_pTable->setSelectionVerticalText( selectionInfo(), b );
    if( util_isRowSelected(selection()) == FALSE &&
        util_isColumnSelected(selection()) == FALSE )
    {
        m_pCanvas->adjustArea(false);
    }
    updateEditWidget();

    m_pDoc->emitEndOperation();
}

void KSpreadView::insertMathExpr()
{
    if ( m_pTable == 0L )
        return;

    m_pDoc->emitBeginOperation(false);
    KSpreadDlgFormula *dlg=new KSpreadDlgFormula( this, "Function" );
    dlg->show();

    /* TODO - because I search on 'TODO's :-) */
    // #### Is the dialog deleted when it's closed ? (David)
    // Torben thinks that not.

    m_pDoc->emitEndOperation();
}

void KSpreadView::formulaSelection( const QString &_math )
{
    if ( m_pTable == 0 )
        return;

    m_pDoc->emitBeginOperation(false);
    if( _math == i18n("Others...") )
    {
        insertMathExpr();
        m_pDoc->emitEndOperation();
        return;
    }

    KSpreadDlgFormula *dlg=new KSpreadDlgFormula( this, "Formula Editor",_math );
    dlg->exec();

    m_pDoc->emitEndOperation();
}

void KSpreadView::fontSizeSelected( int _size )
{
    if ( m_toolbarLock )
        return;

    m_pDoc->emitBeginOperation(false);

    if ( m_pTable != 0L )
        m_pTable->setSelectionFont( selectionInfo(), 0L, _size );

    // Dont leave the focus in the toolbars combo box ...
    if ( m_pCanvas->editor() )
    {
        KSpreadCell * cell = m_pTable->cellAt( m_selectionInfo->marker() );
        m_pCanvas->editor()->setEditorFont(cell->textFont(m_pCanvas->markerColumn(), m_pCanvas->markerRow()), true);
        m_pCanvas->editor()->setFocus();
    }
    else
        m_pCanvas->setFocus();

    m_pDoc->emitEndOperation();
}

void KSpreadView::bold( bool b )
{
    if ( m_toolbarLock )
        return;
    if ( m_pTable == 0 )
        return;

    m_pDoc->emitBeginOperation(false);

    int col = m_pCanvas->markerColumn();
    int row = m_pCanvas->markerRow();
    m_pTable->setSelectionFont( selectionInfo(), 0L, -1, b );

    if ( m_pCanvas->editor() )
    {
        KSpreadCell * cell = m_pTable->cellAt( col, row );
        m_pCanvas->editor()->setEditorFont(cell->textFont(col, row), true);
    }

    m_pDoc->emitEndOperation();
}

void KSpreadView::underline( bool b )
{
    if ( m_toolbarLock )
        return;
    if ( m_pTable == 0 )
        return;

    m_pDoc->emitBeginOperation(false);

    int col = m_pCanvas->markerColumn();
    int row = m_pCanvas->markerRow();

    m_pTable->setSelectionFont( selectionInfo(), 0L, -1, -1, -1 ,b );
    if ( m_pCanvas->editor() )
    {
        KSpreadCell * cell = m_pTable->cellAt( col, row );
        m_pCanvas->editor()->setEditorFont(cell->textFont(col, row), true);
    }

    m_pDoc->emitEndOperation();
}

void KSpreadView::strikeOut( bool b )
{
    if ( m_toolbarLock )
        return;
    if ( m_pTable == 0 )
        return;

    m_pDoc->emitBeginOperation(false);

    int col = m_pCanvas->markerColumn();
    int row = m_pCanvas->markerRow();

    m_pTable->setSelectionFont( selectionInfo(), 0L, -1, -1, -1 ,-1,b );
    if ( m_pCanvas->editor() )
    {
        KSpreadCell * cell = m_pTable->cellAt( col, row );
        m_pCanvas->editor()->setEditorFont(cell->textFont(col, row), true);
    }

    m_pDoc->emitEndOperation();
}


void KSpreadView::italic( bool b )
{
    if ( m_toolbarLock )
        return;
    if ( m_pTable == 0 )
        return;

    m_pDoc->emitBeginOperation(false);

    int col = m_pCanvas->markerColumn();
    int row = m_pCanvas->markerRow();

    m_pTable->setSelectionFont( selectionInfo(), 0L, -1, -1, b );
    if ( m_pCanvas->editor() )
    {
        KSpreadCell * cell = m_pTable->cellAt( col, row );
        m_pCanvas->editor()->setEditorFont(cell->textFont(col, row), true);
    }

    m_pDoc->emitEndOperation();
}

void KSpreadView::sortInc()
{
    QRect r( m_selectionInfo->selection() );
    if ( m_selectionInfo->singleCellSelection() )
    {
        KMessageBox::error( this, i18n("You must select multiple cells.") );
        return;
    }

    m_pDoc->emitBeginOperation(false);

    // Entire row(s) selected ? Or just one row ?
    if( util_isRowSelected(selection()) || r.top() == r.bottom() )
        activeTable()->sortByRow( selection(), r.top(), KSpreadSheet::Increase );
    else
        activeTable()->sortByColumn( selection(), r.left(), KSpreadSheet::Increase );
    updateEditWidget();

    m_pDoc->emitEndOperation();
}

void KSpreadView::sortDec()
{
    QRect r( m_selectionInfo->selection() );
    if ( m_selectionInfo->singleCellSelection() )
    {
        KMessageBox::error( this, i18n("You must select multiple cells.") );
        return;
    }

    m_pDoc->emitBeginOperation(false);

    // Entire row(s) selected ? Or just one row ?
    if( util_isRowSelected(selection()) || r.top() == r.bottom() )
        activeTable()->sortByRow( selection(), r.top(), KSpreadSheet::Decrease );
    else
        activeTable()->sortByColumn( selection(), r.left(), KSpreadSheet::Decrease );
    updateEditWidget();

    m_pDoc->emitEndOperation();
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
    m_pDoc->emitBeginOperation(false);

    m_pTable->borderBottom( m_selectionInfo, m_borderColor->color());

    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::setSelectionBottomBorderColor(const QColor &color)
{
  if (m_pTable != 0L)
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->borderBottom( selectionInfo(), color );
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::borderRight()
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->borderRight( m_selectionInfo, m_borderColor->color());
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::setSelectionRightBorderColor(const QColor &color)
{
  if (m_pTable != 0L)
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->borderRight( selectionInfo(), color );
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::borderLeft()
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->borderLeft( m_selectionInfo, m_borderColor->color() );
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::setSelectionLeftBorderColor(const QColor &color)
{
  if (m_pTable != 0L)
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->borderLeft( selectionInfo(), color );
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::borderTop()
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->borderTop( m_selectionInfo, m_borderColor->color() );
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::setSelectionTopBorderColor(const QColor &color)
{
  if (m_pTable != 0L)
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->borderTop(selectionInfo(), color);
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::borderOutline()
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->borderOutline( m_selectionInfo, m_borderColor->color());
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::setSelectionOutlineBorderColor(const QColor &color)
{
  if (m_pTable != 0L)
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->borderOutline(selectionInfo(), color);
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::borderAll()
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->borderAll( m_selectionInfo, m_borderColor->color() );
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::setSelectionAllBorderColor(const QColor &color)
{
  if (m_pTable != 0L)
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->borderAll( selectionInfo(), color );
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::borderRemove()
{
  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->borderRemove( m_selectionInfo );
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::addTable( KSpreadSheet *_t )
{
  m_pDoc->emitBeginOperation(false);

  insertTable( _t );

    // Connect some signals
    QObject::connect( _t, SIGNAL( sig_updateView( KSpreadSheet* ) ), SLOT( slotUpdateView( KSpreadSheet* ) ) );
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
    if(m_bLoading)
        updateBorderButton();

    m_pDoc->emitEndOperation();
}

void KSpreadView::slotTableRemoved( KSpreadSheet *_t )
{
  m_pDoc->emitBeginOperation(false);

  QString m_tableName=_t->tableName();
  m_pTabBar->removeTab( _t->tableName() );
  if(m_pDoc->map()->findTable( m_pTabBar->listshow().first()))
    setActiveTable( m_pDoc->map()->findTable( m_pTabBar->listshow().first() ));
  else
    m_pTable = 0L;

  QValueList<Reference>::Iterator it;
  QValueList<Reference> area=doc()->listArea();
  for ( it = area.begin(); it != area.end(); ++it )
  {
        //remove Area Name when table target is removed
        if((*it).table_name==m_tableName)
	  {
	    doc()->removeArea((*it).ref_name);
	    //now area name is used in formula
	    //so you must recalc tables when remove areaname
	    KSpreadSheet *tbl;

	    for ( tbl = doc()->map()->firstTable(); tbl != 0L; tbl = doc()->map()->nextTable() )
	      {
		tbl->refreshRemoveAreaName((*it).ref_name);
	      }
	  }
  }

  m_pDoc->emitEndOperation();
}

void KSpreadView::removeAllTables()
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->removeAllTabs();

  setActiveTable( 0L );

  m_pDoc->emitEndOperation();
}

void KSpreadView::setActiveTable( KSpreadSheet *_t,bool updateTable )
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

  m_pTable = _t;

  if ( m_pTable == 0L )
  {
    m_pDoc->emitEndOperation();
    return;
  }
  m_pDoc->setDisplayTable( m_pTable );
  if(updateTable)
  {
    m_pTabBar->setActiveTab( _t->tableName() );
    m_pVBorderWidget->repaint();
    m_pHBorderWidget->repaint();
    m_pTable->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));
    m_pCanvas->slotMaxColumn( m_pTable->maxColumn() );
    m_pCanvas->slotMaxRow( m_pTable->maxRow() );
  }

  /* see if there was a previous selection on this other table */
  QMapIterator<KSpreadSheet*, QPoint> it = savedAnchors.find(m_pTable);
  QMapIterator<KSpreadSheet*, QPoint> it2 = savedMarkers.find(m_pTable);

  QPoint newAnchor = (it == savedAnchors.end()) ? QPoint(1,1) : *it;
  QPoint newMarker = (it2 == savedMarkers.end()) ? QPoint(1,1) : *it2;
  selectionInfo()->setSelection(newMarker, newAnchor, m_pTable);

  m_pCanvas->scrollToCell(newMarker);
  resultOfCalc();

  m_pDoc->emitEndOperation();
}

void KSpreadView::slotTableRenamed( KSpreadSheet* table, const QString& old_name )
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->renameTab( old_name, table->tableName() );
  m_pDoc->emitEndOperation();
}

void KSpreadView::slotTableHidden( KSpreadSheet* table )
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->hideTable( table->tableName() );
  updateShowTableMenu();
  m_pDoc->emitEndOperation();
}

void KSpreadView::slotTableShown( KSpreadSheet* table )
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->displayTable( table->tableName() );
  updateShowTableMenu();
  m_pDoc->emitEndOperation();
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
    m_pDoc->emitEndOperation();
}

void KSpreadView::slotScrollToFirstTable()
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->scrollFirst();
  m_pDoc->emitEndOperation();
}

void KSpreadView::slotScrollToLeftTable()
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->scrollLeft();
  m_pDoc->emitEndOperation();
}

void KSpreadView::slotScrollToRightTable()
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->scrollRight();
  m_pDoc->emitEndOperation();
}

void KSpreadView::slotScrollToLastTable()
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->scrollLast();
  m_pDoc->emitEndOperation();
}

void KSpreadView::insertTable()
{
  m_pDoc->emitBeginOperation(false);
  m_pCanvas->closeEditor();
  KSpreadSheet *t = m_pDoc->createTable();
  m_pDoc->addTable( t );
  updateEditWidget();
  KSpreadUndoAddTable *undo = new KSpreadUndoAddTable(m_pDoc, t);
  m_pDoc->undoBuffer()->appendUndo( undo );
  setActiveTable( t );
  m_pDoc->emitEndOperation();
}

void KSpreadView::hideTable()
{
    if ( !m_pTable )
       return;
    m_pDoc->emitBeginOperation(false);
    m_pTabBar->hideTable();
    m_pDoc->emitEndOperation();
}

void KSpreadView::showTable()
{
    if ( !m_pTable )
        return;
    m_pDoc->emitBeginOperation(false);
    KSpreadshow dlg( this, "Sheet show");
    dlg.exec();
    m_pDoc->emitEndOperation();
}

void KSpreadView::copySelection()
{
    if ( !m_pTable )
        return;
    if(!m_pCanvas->editor())
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

    if( !m_pCanvas->editor())
    {
        m_pTable->cutSelection( selectionInfo() );
        resultOfCalc();
        updateEditWidget();
    }
    else
        m_pCanvas->editor()->cut();

    m_pDoc->emitEndOperation();
}

void KSpreadView::paste()
{
    if ( !m_pTable )
        return;

    m_pDoc->emitBeginOperation(false);
    if( !m_pCanvas->editor() )
    {
        m_pTable->paste( selection() );
        resultOfCalc();
        updateEditWidget();
    }
    else
    {
        m_pCanvas->editor()->paste();
    }
    m_pDoc->emitEndOperation();
}

void KSpreadView::specialPaste()
{
    if ( !m_pTable )
        return;

    m_pDoc->emitBeginOperation(false);

    KSpreadspecial dlg( this, "Special Paste" );
    if( dlg.exec() )
    {
      if (m_pTable->getAutoCalc())
        m_pTable->recalc();
      resultOfCalc();
      updateEditWidget();
    }

    m_pDoc->emitEndOperation();
}

void KSpreadView::removeComment()
{
  if ( !m_pTable )
        return;

  m_pDoc->emitBeginOperation(false);
  m_pTable->setSelectionRemoveComment( selectionInfo() );
  updateEditWidget();
  m_pDoc->emitEndOperation();
}


void KSpreadView::changeAngle()
{
  if ( !m_pTable )
        return;

  m_pDoc->emitBeginOperation(false);
  KSpreadAngle dlg( this, "Angle" ,
                    QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
  if(dlg.exec())
  {
    if( (util_isRowSelected(selection()) == FALSE) &&
        (util_isColumnSelected(selection()) == FALSE) )
      m_pCanvas->adjustArea(false);
  }
  m_pDoc->emitEndOperation();
}

void KSpreadView::setSelectionAngle(int angle)
{
  m_pDoc->emitBeginOperation(false);

  if (m_pTable != NULL)
  {
    m_pTable->setSelectionAngle(selectionInfo(), angle);

    if (util_isRowSelected(selection()) == false &&
        util_isColumnSelected(selection()) == false)
    {
      m_pCanvas->adjustArea(false);
    }
  }

  m_pDoc->emitEndOperation();
}

void KSpreadView::mergeCell()
{
    if ( !m_pTable )
        return;

    m_pDoc->emitBeginOperation(false);

    if((util_isRowSelected(selection())) ||(util_isColumnSelected(selection())))
    {
        KMessageBox::error( this, i18n("Area too large!"));
    }
    else
    {
        m_pTable->mergeCells( selection() );
        m_pCanvas->gotoLocation( selection().topLeft() );
    }

    m_pDoc->emitEndOperation();
}

void KSpreadView::dissociateCell()
{
    if ( !m_pTable )
        return;

    m_pDoc->emitBeginOperation(false);

    m_pTable->dissociateCell( QPoint( m_pCanvas->markerColumn(),
                                      m_pCanvas->markerRow() ) );
    m_pDoc->emitEndOperation();
}


void KSpreadView::increaseIndent()
{
  if ( !m_pTable )
        return;

  m_pDoc->emitBeginOperation(false);
  m_pTable->increaseIndent( m_selectionInfo );
  updateEditWidget();
  m_pDoc->emitEndOperation();
}

void KSpreadView::decreaseIndent()
{
  if ( !m_pTable )
        return;

  m_pDoc->emitBeginOperation(false);
  int column=m_pCanvas->markerColumn();
  int row=m_pCanvas->markerRow();

  m_pTable->decreaseIndent( m_selectionInfo );
  KSpreadCell* cell = m_pTable->cellAt( column, row );
  if(cell)
      m_decreaseIndent->setEnabled( cell->getIndent( column, row ) > 0.0 );

  m_pDoc->emitEndOperation();
}

void KSpreadView::goalSeek()
{
  if ( m_pCanvas->editor() )
  {
    m_pCanvas->deleteEditor( true ); // save changes
  }

  KSpreadGoalSeekDlg * dlg =
    new KSpreadGoalSeekDlg( this, QPoint(m_pCanvas->markerColumn(),
                                         m_pCanvas->markerRow() ),
                            "KSpreadGoalSeekDlg" );
  dlg->show();
  /* TODO - hanging pointer? */
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
    m_pDoc->emitBeginOperation(false);
    m_selectionInfo->setSelection( dlg.selection().topLeft(),
                                   dlg.selection().bottomRight(),
                                   dlg.table() );
    m_pDoc->emitEndOperation();
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
    if (m_selectionInfo->selection().width() > 1)
    {
        KMessageBox::error( this, i18n("You must not select an area containing more than one column.") );
        return;
    }

    m_pDoc->emitBeginOperation(false);

    KSpreadCSVDialog dialog( this, "KSpreadCSVDialog", m_selectionInfo->selection(), KSpreadCSVDialog::Column );
    dialog.exec();

    m_pDoc->emitEndOperation();
}

void KSpreadView::consolidate()
{
    m_pCanvas->closeEditor();
    KSpreadConsolidate * dlg = new KSpreadConsolidate( this, "Consolidate" );
    dlg->show();
    // dlg destroys itself
    /* TODO - dialog needs to take care of repainting issues
       (begin/end operations)*/
}

void KSpreadView::sortList()
{
  m_pDoc->emitBeginOperation(false);
  KSpreadList dlg(this,"List selection");
  dlg.exec();
  m_pDoc->emitEndOperation();
}

void KSpreadView::gotoCell()
{
  m_pDoc->emitBeginOperation(false);
  KSpreadGotoDlg dlg( this, "GotoCell" );
  dlg.exec();
  m_pDoc->emitEndOperation();
}

void KSpreadView::find()
{
    KoFindDialog dlg( this, "Find", m_findOptions, m_findStrings );
    if ( KoFindDialog::Accepted != dlg.exec() )
    {
        return;
    }
    m_findOptions = dlg.options();
    m_findStrings = dlg.findHistory();

    // Do the finding!
    m_pDoc->emitBeginOperation(false);
    activeTable()->find( dlg.pattern(), dlg.options(), m_pCanvas );
    m_pDoc->emitEndOperation();
}

void KSpreadView::replace()
{
    KoReplaceDialog dlg( this, "Replace", m_findOptions, m_findStrings, m_replaceStrings );
    m_pDoc->emitBeginOperation(false);
    if ( KoReplaceDialog::Accepted != dlg.exec() )
    {
      m_pDoc->emitEndOperation();
      return;
    }
    m_findOptions = dlg.options();
    m_findStrings = dlg.findHistory();
    m_replaceStrings = dlg.replacementHistory();

    // Do the replacement.
    activeTable()->replace( dlg.pattern(), dlg.replacement(), dlg.options(),
                            m_pCanvas );

    // Refresh the editWidget
    KSpreadCell *cell = activeTable()->cellAt( canvasWidget()->markerColumn(),
                                               canvasWidget()->markerRow() );
    if ( cell->text() != 0L )
        editWidget()->setText( cell->text() );
    else
        editWidget()->setText( "" );

    m_pDoc->emitEndOperation();
}

void KSpreadView::conditional()
{
  QRect rect( m_selectionInfo->selection() );

  if( (util_isRowSelected(selection())) || (util_isColumnSelected(selection())) )
  {
    KMessageBox::error( this, i18n("Area too large!"));
  }
  else
  {
    m_pDoc->emitBeginOperation(false);
    KSpreadconditional dlg(this,"conditional",rect);
    dlg.exec();
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::validity()
{
  QRect rect( m_selectionInfo->selection() );

  if( (util_isRowSelected(selection())) || (util_isColumnSelected(selection())) )
  {
    KMessageBox::error( this, i18n("Area too large!"));
  }
  else
  {
    m_pDoc->emitBeginOperation(false);
    KSpreadDlgValidity dlg( this,"validity",rect);
    dlg.exec();
    m_pDoc->emitEndOperation();
  }
}


void KSpreadView::insertSeries()
{
    m_pCanvas->closeEditor();
    KSpreadSeriesDlg dlg( this, "Series", QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
    m_pDoc->emitBeginOperation(false);
    dlg.exec();
    m_pDoc->emitEndOperation();
}

void KSpreadView::sort()
{
    if( m_selectionInfo->singleCellSelection() )
    {
        KMessageBox::error( this, i18n("You must select multiple cells") );
        return;
    }

    KSpreadSortDlg dlg( this, "Sort" );
    m_pDoc->emitBeginOperation(false);
    dlg.exec();
    m_pDoc->emitEndOperation();
}

void KSpreadView::insertHyperlink()
{
    m_pCanvas->closeEditor();

    KSpreadLinkDlg dlg( this, "Insert Link" );
    m_pDoc->emitBeginOperation(false);
    dlg.exec();
    m_pDoc->emitEndOperation();
}

void KSpreadView::insertFromDatabase()
{
#ifndef QT_NO_SQL
    m_pCanvas->closeEditor();

    QRect rect = m_selectionInfo->selection();

    KSpreadDatabaseDlg dlg(this, rect, "KSpreadDatabaseDlg");
    m_pDoc->emitBeginOperation(false);
    dlg.exec();
    m_pDoc->emitEndOperation();
#endif
}

void KSpreadView::insertFromTextfile()
{
    m_pCanvas->closeEditor();
    //KMessageBox::information( this, "Not implemented yet, work in progress...");

    KSpreadCSVDialog dialog( this, "KSpreadCSVDialog", selection(), KSpreadCSVDialog::File );
    m_pDoc->emitBeginOperation(false);
    dialog.exec();
    m_pDoc->emitEndOperation();
}

void KSpreadView::insertFromClipboard()
{
    m_pCanvas->closeEditor();

    KSpreadCSVDialog dialog( this, "KSpreadCSVDialog", m_selectionInfo->selection(), KSpreadCSVDialog::Clipboard );
    m_pDoc->emitBeginOperation(false);
    dialog.exec();
    m_pDoc->emitEndOperation();
}

void KSpreadView::setupPrinter( KPrinter &prt )
{

    //apply page layout parameters
    KoFormat pageFormat = m_pTable->paperFormat();

    prt.setPageSize( static_cast<KPrinter::PageSize>( KoPageFormat::printerPageSize( pageFormat ) ) );

    if ( m_pTable->orientation() == PG_LANDSCAPE || pageFormat == PG_SCREEN )
        prt.setOrientation( KPrinter::Landscape );
    else
        prt.setOrientation( KPrinter::Portrait );

    prt.setFullPage( TRUE );
    prt.setResolution ( 72 );
}

void KSpreadView::print( KPrinter &prt )
{
    if ( m_pCanvas->editor() )
    {
      m_pCanvas->deleteEditor( true ); // save changes
    }

    // ### HACK: disable zooming-when-printing if embedded parts are used.
    // No koffice app supports zooming in paintContent currently.
    // Disable in ALL cases now
    bool doZoom = false;
    /*QPtrListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() && doZoom ; ++fit )
        if ( fit.current()->type() == FT_PART )
            doZoom = false;*/

    int oldZoom = m_pDoc->zoom();

    // We don't get valid metrics from the printer - and we want a better resolution
    // anyway (it's the PS driver that takes care of the printer resolution).
    QPaintDeviceMetrics metrics( &prt );

    //int dpiX = metrics.logicalDpiX();
    //int dpiY = metrics.logicalDpiY();
    int dpiX = doZoom ? 300 : QPaintDevice::x11AppDpiX();
    int dpiY = doZoom ? 300 : QPaintDevice::x11AppDpiY();
    ///////// Changing the dpiX/dpiY is very wrong nowadays. This has no effect on the font size
    ///////// that we give Qt, anymore, so it leads to minuscule fonts in the printout => doZoom==false.

    m_pDoc->setZoomAndResolution( 100, dpiX, dpiY );
    m_pDoc->newZoomAndResolution( false, true /* for printing*/ );

    //store the current setting in a temporary variable
    KoOrientation _orient =  m_pTable->orientation();

    QPainter painter;

    painter.begin( &prt );

    //use the current orientation from print dialog
    if ( prt.orientation() == KPrinter::Landscape )
    {
        m_pTable->setPaperOrientation( PG_LANDSCAPE );
    }
    else
    {
        m_pTable->setPaperOrientation( PG_PORTRAIT );
    }

    painter.scale( (double)metrics.logicalDpiX() / (double)dpiX,
                   (double)metrics.logicalDpiY() / (double)dpiY );

    // Print the table and tell that m_pDoc is NOT embedded.
    m_pTable->print( painter, &prt );

    m_pDoc->setZoomAndResolution( oldZoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );
    m_pDoc->newZoomAndResolution( false, false );

    painter.end();

    //Restore original orientation
    m_pTable->setPaperOrientation( _orient );
}

void KSpreadView::insertChart( const QRect& _geometry, KoDocumentEntry& _e )
{
    if ( !m_pTable )
      return;
    m_pDoc->emitBeginOperation(false);

    // Transform the view coordinates to document coordinates
    KoRect unzoomedRect = m_pDoc->unzoomRect( _geometry );
    unzoomedRect.moveBy( m_pCanvas->xOffset(), m_pCanvas->yOffset() );

    //KOfficeCore cannot handle KoRect directly, so switching to QRect
    QRect unzoomedGeometry = QRect( unzoomedRect.x(),
                                    unzoomedRect.y(),
                                    unzoomedRect.width(),
                                    unzoomedRect.height() );

    if( (util_isRowSelected(selection())) || (util_isColumnSelected(selection())) )
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
    m_pDoc->emitEndOperation();
}

void KSpreadView::insertChild( const QRect& _geometry, KoDocumentEntry& _e )
{
    if ( !m_pTable )
        return;

    m_pDoc->emitBeginOperation(false);

    // Transform the view coordinates to document coordinates
    KoRect unzoomedRect = m_pDoc->unzoomRect( _geometry );
    unzoomedRect.moveBy( m_pCanvas->xOffset(), m_pCanvas->yOffset() );

    //KOfficeCore cannot handle KoRect directly, so switching to QRect
    QRect unzoomedGeometry = QRect( unzoomedRect.x(),
                                    unzoomedRect.y(),
                                    unzoomedRect.width(),
                                    unzoomedRect.height() );


    // Insert the new child in the active table.
    m_pTable->insertChild( unzoomedGeometry, _e );
    m_pDoc->emitEndOperation();
}

void KSpreadView::slotRemoveChild( KSpreadChild *_child )
{
    if ( _child->table() != m_pTable )
	return;

    // Make shure that this child has no active embedded view -> activate ourselfs
    m_pDoc->emitBeginOperation(false);
    partManager()->setActivePart( koDocument(), this );
    partManager()->setSelectedPart( 0 );
    m_pDoc->emitEndOperation();
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

void KSpreadView::togglePageBorders( bool mode )
{
   if ( !m_pTable )
       return;

   m_pDoc->emitBeginOperation(false);
   m_pTable->setShowPageBorders( mode );
   m_pDoc->emitEndOperation();
}

void KSpreadView::viewZoom( const QString & s )
{
  int oldZoom = (int) (zoom() * 100);

  QString z( s );
  bool ok = false;
  int newZoom;

  z = z.replace( QRegExp( "%" ), "" );
  z = z.simplifyWhiteSpace();
  newZoom = z.toInt(&ok);

  if ( !ok || newZoom < 10 ) //zoom should be valid and >10
    newZoom = oldZoom;

  if( newZoom != oldZoom )
  {
    m_pDoc->emitBeginOperation(false);

    m_pCanvas->closeEditor();
    setZoom( newZoom, true );

    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::setZoom( int zoom, bool /*updateViews*/ )
{
  // Set the zoom in KoView (for embedded views)
  m_pDoc->emitBeginOperation(false);
//   KoView::setZoom( (double) zoom / 100 );
//   m_pDoc->setZoom( zoom );
//   m_pDoc->newZoom();

  m_pDoc->setZoomAndResolution( zoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY());
  KoView::setZoom( m_pDoc->zoomedResolutionY() /* KoView only supports one zoom */ );

//  m_pDoc->newZoomAndResolution( updateViews, false )

  m_pCanvas->slotMaxColumn( m_pTable->maxColumn() );
  m_pCanvas->slotMaxRow( m_pTable->maxRow() );

  m_pVBorderWidget->repaint();
  m_pHBorderWidget->repaint();
  m_pTable->setRegionPaintDirty(QRect(QPoint(0,0), QPoint(KS_colMax, KS_rowMax)));

  refreshView();
  m_pDoc->emitEndOperation();
}

void KSpreadView::preference()
{
  if ( !m_pTable )
       return;
  KSpreadpreference dlg( this, "Preference");
  m_pDoc->emitBeginOperation(false);
  if(dlg.exec())
  {
    m_pTable->refreshPreference();
  }
  m_pDoc->emitEndOperation();
}

void KSpreadView::addModifyComment()
{
  if ( !m_pTable )
       return;

  m_pDoc->emitBeginOperation(false);

  KSpreadComment dlg( this, "comment",QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
  if(dlg.exec())
        updateEditWidget();

  m_pDoc->emitEndOperation();
}

void KSpreadView::setSelectionComment(QString comment)
{
  if (m_pTable != NULL)
  {
    m_pDoc->emitBeginOperation(false);

    m_pTable->setSelectionComment( selectionInfo(), comment.stripWhiteSpace() );
    updateEditWidget();

    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::editCell()
{
    if ( m_pCanvas->editor() )
        return;

    m_pCanvas->createEditor();
}

void KSpreadView::nextTable(){

    KSpreadSheet *t = m_pDoc->map()->nextTable( activeTable() );
    if ( !t )
    {
        kdDebug(36001) << "Unknown table " <<  endl;
        return;
    }
    m_pCanvas->closeEditor();
    setActiveTable( t );

}

void KSpreadView::previousTable(){

    KSpreadSheet *t = m_pDoc->map()->previousTable( activeTable() );
    if ( !t )
    {
        kdDebug(36001) << "Unknown table "  << endl;
        return;
    }
    m_pCanvas->closeEditor();
    setActiveTable( t );

}
void KSpreadView::firstTable(){

    KSpreadSheet *t = m_pDoc->map()->firstTable();
    if ( !t )
    {
        kdDebug(36001) << "Unknown table " <<  endl;
        return;
    }
    m_pCanvas->closeEditor();
    setActiveTable( t );

}
void KSpreadView::lastTable(){

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
  if ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) ){

    if ( _ev->state() & ( Qt::ControlButton ) ){

      switch( _ev->key() ){

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

KoDocument* KSpreadView::hitTest( const QPoint &pos )
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
    return m_pToolWidget->height() + int( m_pCanvas->doc()->zoomItX( XBORDER_HEIGHT ) );
}

int KSpreadView::bottomBorder() const
{
    return m_pHorzScrollBar->height();
}

void KSpreadView::refreshView()
{
    bool active=activeTable()->getShowFormula();
    m_alignLeft->setEnabled(!active);
    m_alignCenter->setEnabled(!active);
    m_alignRight->setEnabled(!active);
    active=m_pDoc->getShowFormulaBar();
    editWidget()->showEditWidget(active);
    int posFrame=30;
    if(active)
      posWidget()->show();
    else
      {
      posWidget()->hide();
      posFrame=0;
      }

    m_pToolWidget->show();

    // If this value (30) is changed then topBorder() needs to
    // be changed, too.
    m_pToolWidget->setGeometry( 0, 0, width(), /*30*/posFrame );
    int top = /*30*/posFrame;

    int widthVScrollbar  = m_pVertScrollBar->sizeHint().width();// 16;
    int heightHScrollbar = m_pHorzScrollBar->sizeHint().height();

    if (m_pDoc->getShowTabBar())
    {
      m_pTabBarFirst->setGeometry( 0, height() - heightHScrollbar,
                                   heightHScrollbar, heightHScrollbar );
      m_pTabBarLeft->setGeometry( heightHScrollbar, height() - heightHScrollbar,
                                  heightHScrollbar, heightHScrollbar );
      m_pTabBarRight->setGeometry( heightHScrollbar * 2, height() - heightHScrollbar,
                                   heightHScrollbar, heightHScrollbar );
      m_pTabBarLast->setGeometry( heightHScrollbar * 3, height() - heightHScrollbar,
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
      m_pTabBar->setGeometry( heightHScrollbar * 4, height() - heightHScrollbar,
                              width() - heightHScrollbar * 4, heightHScrollbar );
    else
      m_pTabBar->setGeometry( heightHScrollbar * 4, height() - heightHScrollbar,
                              width() / 2 - heightHScrollbar * 4, heightHScrollbar );
    if(m_pDoc->getShowTabBar())
      m_pTabBar->show();
    else
      m_pTabBar->hide();

    // David's suggestion: move the scrollbars to KSpreadCanvas, but keep those resize statements
    if(m_pDoc->getShowHorizontalScrollBar())
      m_pHorzScrollBar->show();
    else
      m_pHorzScrollBar->hide();

    if(!m_pDoc->getShowTabBar() && !m_pDoc->getShowHorizontalScrollBar())
      m_pVertScrollBar->setGeometry( width() - widthVScrollbar,
                                     top,
                                     widthVScrollbar,
                                     height() - top );
    else
      m_pVertScrollBar->setGeometry( width() - widthVScrollbar,
                                     top,
                                     widthVScrollbar,
                                     height() - heightHScrollbar - top );
    m_pVertScrollBar->setSteps( 20 /*linestep*/, m_pVertScrollBar->height() /*pagestep*/);
    if (m_pDoc->getShowVerticalScrollBar())
      m_pVertScrollBar->show();
    else
    {
      widthVScrollbar = 0;
      m_pVertScrollBar->hide();
    }

    int widthRowHeader = int( m_pCanvas->doc()->zoomItX( YBORDER_WIDTH ) );
    if (m_pDoc->getShowRowHeader())
      m_pVBorderWidget->show();
    else
    {
      widthRowHeader = 0;
      m_pVBorderWidget->hide();
    }

    int heightColHeader = int( m_pCanvas->doc()->zoomItY( XBORDER_HEIGHT ) );
    if(m_pDoc->getShowColHeader())
      m_pHBorderWidget->show();
    else
    {
      heightColHeader = 0;
      m_pHBorderWidget->hide();
    }

    if (statusBar())
    {
      if(m_pDoc->getShowStatusBar())
        statusBar()->show();
      else
        statusBar()->hide();
    }

    m_pHorzScrollBar->setGeometry( width() / 2,
                                   height() - heightHScrollbar,
                                   width() / 2 - widthVScrollbar,
                                   heightHScrollbar );
    m_pHorzScrollBar->setSteps( 20 /*linestep*/, m_pHorzScrollBar->width() /*pagestep*/);

    if(!m_pDoc->getShowTabBar() && !m_pDoc->getShowHorizontalScrollBar())
      m_pFrame->setGeometry( 0, top, width() - widthVScrollbar, height() - top - heightHScrollbar);
    else
      m_pFrame->setGeometry( 0, top, width() - widthVScrollbar,
                             height() - heightHScrollbar - top );
    m_pFrame->show();

    m_pCanvas->setGeometry( widthRowHeader, heightColHeader,
                            m_pFrame->width() -widthRowHeader, m_pFrame->height() - heightColHeader );
    m_pCanvas->updatePosWidget();

    m_pHBorderWidget->setGeometry( widthRowHeader + 1, 0,
                                   m_pFrame->width() - widthRowHeader, heightColHeader );

    m_pVBorderWidget->setGeometry( 0,heightColHeader + 1, widthRowHeader,
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
      m_pDoc->emitEndOperation();
    }
}

void KSpreadView::popupColumnMenu(const QPoint & _point)
{
    assert( m_pTable );

    if ( !koDocument()->isReadWrite() )
      return;

    delete m_pPopupColumn ;

    m_pPopupColumn = new QPopupMenu( this );

    m_cellLayout->plug( m_pPopupColumn );
	m_pPopupColumn->insertSeparator();
    m_cut->plug( m_pPopupColumn );
    m_copy->plug( m_pPopupColumn );
    m_paste->plug( m_pPopupColumn );
    m_specialPaste->plug( m_pPopupColumn );
    m_insertCellCopy->plug( m_pPopupColumn );
    m_pPopupColumn->insertSeparator();
    m_default->plug( m_pPopupColumn );
    // If there is no selection
    if((util_isRowSelected(selection()) == FALSE) && (util_isColumnSelected(selection()) == FALSE) )
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
    ColumnLayout * col;
    QRect rect = m_selectionInfo->selection();
    //kdDebug(36001) << "Column: L: " << rect.left() << endl;
    for ( i = rect.left(); i <= rect.right(); ++i )
    {
      if (i == 2) // "B"
      {
        col = activeTable()->columnLayout( 1 );
        if ( col->isHide() )
        {
          m_showSelColumns->setEnabled(true);
          m_showSelColumns->plug( m_pPopupColumn );
          break;
        }
      }

      col = activeTable()->columnLayout( i );

      if ( col->isHide() )
      {
        m_showSelColumns->setEnabled(true);
        m_showSelColumns->plug( m_pPopupColumn );
        break;
      }
    }

    QObject::connect( m_pPopupColumn, SIGNAL(activated( int ) ), this, SLOT(slotActivateTool( int ) ) );

    m_pPopupColumn->popup( _point );
}

void KSpreadView::slotPopupAdjustColumn()
{
    if ( !m_pTable )
       return;

    m_pDoc->emitBeginOperation(false);
    canvasWidget()->adjustArea();
    m_pDoc->emitEndOperation();
}

void KSpreadView::popupRowMenu(const QPoint & _point )
{
    assert( m_pTable );

    if ( !koDocument()->isReadWrite() )
      return;

    delete m_pPopupRow ;

    m_pPopupRow= new QPopupMenu();

    m_cellLayout->plug( m_pPopupRow );
	m_pPopupRow->insertSeparator();
    m_cut->plug( m_pPopupRow );
    m_copy->plug( m_pPopupRow );
    m_paste->plug( m_pPopupRow );
    m_specialPaste->plug( m_pPopupRow );
    m_insertCellCopy->plug( m_pPopupRow );
    m_pPopupRow->insertSeparator();
    m_default->plug( m_pPopupRow );
    // If there is no selection
    if( (util_isRowSelected(selection()) == FALSE) && (util_isColumnSelected(selection()) == FALSE) )
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
    RowLayout * row;
    QRect rect = m_selectionInfo->selection();
    for ( i = rect.top(); i <= rect.bottom(); ++i )
    {
        //kdDebug(36001) << "popupRow: " << rect.top() << endl;
      if (i == 2)
      {
        row = activeTable()->rowLayout( 1 );
        if ( row->isHide() )
        {
          m_showSelRows->setEnabled(true);
          m_showSelRows->plug( m_pPopupRow );
          break;
        }
      }

      row = activeTable()->rowLayout( i );
      if ( row->isHide() )
      {
        m_showSelRows->setEnabled(true);
        m_showSelRows->plug( m_pPopupRow );
        break;
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
    m_pDoc->emitEndOperation();
}


void KSpreadView::slotListChoosePopupMenu( )
{
 assert( m_pTable );
 delete m_popupListChoose;

 if(!koDocument()->isReadWrite() )
   return;

 m_popupListChoose = new QPopupMenu();
 int id = 0;
 QRect selection( m_selectionInfo->selection() );
 KSpreadCell *cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
 QString tmp=cell->text();
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
       if ( c->isString() && c->text() != tmp && !c->text().isEmpty() )
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
	 if(c->isString() && c->text()!=tmp && !c->text().isEmpty())
	   {
	     if(itemList.findIndex(c->text())==-1)
                 itemList.append(c->text());
	   }

       }
    }
 */

 for ( QStringList::Iterator it = itemList.begin(); it != itemList.end();++it )
   m_popupListChoose->insertItem( (*it), id++ );

 if(id==0)
   return;
 RowLayout *rl = m_pTable->rowLayout( m_pCanvas->markerRow());
 double tx = m_pTable->dblColumnPos( m_pCanvas->markerColumn(), m_pCanvas );
 double ty = m_pTable->dblRowPos(m_pCanvas->markerRow(), m_pCanvas );
 double h = rl->dblHeight( m_pCanvas );
 if ( cell->extraYCells() )
   h = cell->extraHeight();
 ty += h;

 QPoint p( tx, ty );
 QPoint p2 = m_pCanvas->mapToGlobal( p );
 m_popupListChoose->popup( p2 );
 QObject::connect( m_popupListChoose, SIGNAL( activated( int ) ),
		  this, SLOT( slotItemSelected( int ) ) );
}


void KSpreadView::slotItemSelected( int id)
{
  QString tmp=m_popupListChoose->text(id);
  KSpreadCell *cell = m_pTable->nonDefaultCell( m_pCanvas->markerColumn(),
						m_pCanvas->markerRow() );

  if(tmp==cell->text())
    return;

  m_pDoc->emitBeginOperation(false);

  if ( !m_pDoc->undoBuffer()->isLocked() )
    {
      KSpreadUndoSetText* undo = new KSpreadUndoSetText( m_pDoc, m_pTable, cell->text(), m_pCanvas->markerColumn(), m_pCanvas->markerRow(), cell->formatType());
      m_pDoc->undoBuffer()->appendUndo( undo );
    }

  cell->setCellText( tmp, true );
  editWidget()->setText( tmp );

  m_pDoc->emitEndOperation();
}

void KSpreadView::openPopupMenu( const QPoint & _point )
{
    assert( m_pTable );


    delete m_pPopupMenu;

    if(!koDocument()->isReadWrite() )
        return;

    m_pPopupMenu = new QPopupMenu();

    m_cellLayout->plug( m_pPopupMenu );
	m_pPopupMenu->insertSeparator();
    m_cut->plug( m_pPopupMenu );
    m_copy->plug( m_pPopupMenu );
    m_paste->plug( m_pPopupMenu );

    m_specialPaste->plug( m_pPopupMenu );
    m_insertCellCopy->plug( m_pPopupMenu );
    m_pPopupMenu->insertSeparator();
    m_delete->plug( m_pPopupMenu );
    m_adjust->plug( m_pPopupMenu );
    m_default->plug( m_pPopupMenu );

    // If there is no selection
    if( (util_isRowSelected(selection()) == FALSE) && (util_isColumnSelected(selection()) == FALSE) )
    {
      m_areaName->plug( m_pPopupMenu );
      m_pPopupMenu->insertSeparator();
      m_insertCell->plug( m_pPopupMenu );
      m_removeCell->plug( m_pPopupMenu );
    }



    KSpreadCell *cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
    m_pPopupMenu->insertSeparator();
    m_addModifyComment->plug( m_pPopupMenu );
    if( !cell->comment(m_pCanvas->markerColumn(), m_pCanvas->markerRow()).isEmpty() )
    {
        m_removeComment->plug( m_pPopupMenu );
    }


    if(activeTable()->testListChoose(selectionInfo()))
      {
	m_pPopupMenu->insertSeparator();
	m_pPopupMenu->insertItem( i18n("Selection List..."), this, SLOT( slotListChoosePopupMenu() ) );
      }

    // Remove informations about the last tools we offered
    m_lstTools.clear();
    m_lstTools.setAutoDelete( true );

    if(!activeTable()->getWordSpelling( selectionInfo() ).isEmpty())
    {
      m_popupMenuFirstToolId = 10;
      int i = 0;
      QValueList<KDataToolInfo> tools = KDataToolInfo::query( "QString", "text/plain", m_pDoc->instance() );
      if( tools.count() > 0 )
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
  if( _id < m_popupMenuFirstToolId )
    return;

  ToolEntry* entry = m_lstTools.at( _id - m_popupMenuFirstToolId );

  KDataTool* tool = entry->info.createTool();
  if ( !tool )
  {
      kdDebug(36001) << "Could not create Tool" << endl;
      return;
  }

  m_pDoc->emitBeginOperation(false);

  QString text = activeTable()->getWordSpelling( selectionInfo() );

  if ( tool->run( entry->command, &text, "QString", "text/plain") )
  {
      activeTable()->setWordSpelling( selectionInfo(), text);

      KSpreadCell *cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
      editWidget()->setText( cell->text() );
  }

  m_pDoc->emitEndOperation();
}

void KSpreadView::deleteSelection()
{
    Q_ASSERT( m_pTable );

    m_pDoc->emitBeginOperation(false);
    m_pTable->deleteSelection( selectionInfo() );
    resultOfCalc();
    updateEditWidget();
    m_pDoc->emitEndOperation();
}

void KSpreadView::adjust()
{
    if( (util_isRowSelected(selection())) || (util_isColumnSelected(selection())) )
    {
      KMessageBox::error( this, i18n("Area too large!"));
    }
    else
    {
      m_pDoc->emitBeginOperation(false);
      canvasWidget()->adjustArea();
      m_pDoc->emitEndOperation();
    }
}

void KSpreadView::clearTextSelection()
{
    Q_ASSERT( m_pTable );
    m_pDoc->emitBeginOperation(false);
    m_pTable->clearTextSelection( selectionInfo() );

    updateEditWidget();
    m_pDoc->emitEndOperation();
}

void KSpreadView::clearCommentSelection()
{
    Q_ASSERT( m_pTable );
    m_pDoc->emitBeginOperation(false);
    m_pTable->setSelectionRemoveComment( selectionInfo() );

    updateEditWidget();
    m_pDoc->emitEndOperation();
}

void KSpreadView::clearValiditySelection()
{
    Q_ASSERT( m_pTable );
    m_pDoc->emitBeginOperation(false);
    m_pTable->clearValiditySelection( selectionInfo() );

    updateEditWidget();
    m_pDoc->emitEndOperation();
}

void KSpreadView::clearConditionalSelection()
{
    Q_ASSERT( m_pTable );
    m_pDoc->emitBeginOperation(false);
    m_pTable->clearConditionalSelection( selectionInfo() );

    updateEditWidget();
    m_pDoc->emitEndOperation();
}


void KSpreadView::defaultSelection()
{
  Q_ASSERT( m_pTable );
  m_pDoc->emitBeginOperation(false);
  m_pTable->defaultSelection( selectionInfo() );

  updateEditWidget();
  m_pDoc->emitEndOperation();
}

void KSpreadView::slotInsert()
{
    QRect r( selection() );
    KSpreadinsert dlg( this, "Insert", r,KSpreadinsert::Insert );
    m_pDoc->emitBeginOperation(false);
    dlg.exec();
    m_pDoc->emitEndOperation();
}

void KSpreadView::slotRemove()
{
    QRect r( m_selectionInfo->selection() );
    KSpreadinsert dlg( this, "Remove", r,KSpreadinsert::Remove );
    m_pDoc->emitBeginOperation(false);
    dlg.exec();
    m_pDoc->emitEndOperation();
}

void KSpreadView::slotInsertCellCopy()
{
  if ( !m_pTable )
    return;

  m_pDoc->emitBeginOperation(false);
  if( !m_pTable->testAreaPasteInsert())
    m_pTable->paste( selection(), true, Normal, OverWrite, true);
  else
  {
    KSpreadpasteinsert dlg( this, "Remove", selection() );
    dlg.exec();
  }
  if(m_pTable->getAutoCalc())
    m_pTable->recalc();
  updateEditWidget();

  m_pDoc->emitEndOperation();
}

void KSpreadView::setAreaName()
{
    KSpreadarea dlg( this, "Area Name",QPoint(m_pCanvas->markerColumn(), m_pCanvas->markerRow()) );
    m_pDoc->emitBeginOperation(false);
    dlg.exec();
    m_pDoc->emitEndOperation();
}

void KSpreadView::showAreaName()
{
    KSpreadreference dlg( this, "Show Area" );
    m_pDoc->emitBeginOperation(false);
    dlg.exec();
    m_pDoc->emitEndOperation();
}

void KSpreadView::resizeRow()
{
    if( util_isColumnSelected(selection()) )
        KMessageBox::error( this, i18n("Area too large!"));
    else
    {
      m_pDoc->emitBeginOperation(false);
      KSpreadResizeRow dlg( this );
      dlg.exec();
      m_pDoc->emitEndOperation();
    }
}

void KSpreadView::resizeColumn()
{
    if( util_isRowSelected(selection()) )
        KMessageBox::error( this, i18n("Area too large!"));
    else
    {
      m_pDoc->emitBeginOperation(false);
      KSpreadResizeColumn dlg( this );
      dlg.exec();
      m_pDoc->emitEndOperation();
    }
}

void KSpreadView::equalizeRow()
{
    if( util_isColumnSelected(selection()) )
        KMessageBox::error( this, i18n("Area too large!"));
    else
    {
      m_pDoc->emitBeginOperation(false);
      canvasWidget()->equalizeRow();
      m_pDoc->emitEndOperation();
    }
}

void KSpreadView::equalizeColumn()
{
    if( util_isRowSelected(selection()) )
        KMessageBox::error( this, i18n("Area too large!"));
    else
    {
      m_pDoc->emitBeginOperation(false);
      canvasWidget()->equalizeColumn();
      m_pDoc->emitEndOperation();
    }
}


void KSpreadView::layoutDlg()
{
  QRect selection( m_selectionInfo->selection() );

  m_pDoc->emitBeginOperation(false);
  CellLayoutDlg dlg( this, m_pTable, selection.left(), selection.top(),
                     selection.right(), selection.bottom() );
  m_pDoc->emitEndOperation();
}

void KSpreadView::paperLayoutDlg()
{
    if ( m_pCanvas->editor() )
    {
        m_pCanvas->deleteEditor( true ); // save changes
    }

    KoPageLayout pl;
    pl.format = m_pTable->paperFormat();
    pl.orientation = m_pTable->orientation();

    pl.ptWidth = MM_TO_POINT( m_pTable->paperWidth() );
    pl.ptHeight = MM_TO_POINT( m_pTable->paperHeight() );
    pl.ptLeft = MM_TO_POINT( m_pTable->leftBorder() );
    pl.ptRight = MM_TO_POINT(  m_pTable->rightBorder() );
    pl.ptTop = MM_TO_POINT(  m_pTable->topBorder() );
    pl.ptBottom = MM_TO_POINT(  m_pTable->bottomBorder() );

    KoHeadFoot hf;
    hf.headLeft  = m_pTable->localizeHeadFootLine( m_pTable->headLeft()  );
    hf.headRight = m_pTable->localizeHeadFootLine( m_pTable->headRight() );
    hf.headMid   = m_pTable->localizeHeadFootLine( m_pTable->headMid()   );
    hf.footLeft  = m_pTable->localizeHeadFootLine( m_pTable->footLeft()  );
    hf.footRight = m_pTable->localizeHeadFootLine( m_pTable->footRight() );
    hf.footMid   = m_pTable->localizeHeadFootLine( m_pTable->footMid()   );

    KoUnit::Unit unit = doc()->getUnit();

    KSpreadPaperLayout *dlg=new KSpreadPaperLayout( this, "PageLayout", pl, hf, FORMAT_AND_BORDERS | HEADER_AND_FOOTER, unit, m_pTable, this);
    dlg->show();
    // dlg destroys itself

}

void KSpreadView::definePrintRange()
{
    m_pTable->definePrintRange(selectionInfo());
}

void KSpreadView::resetPrintRange()
{
    m_pTable->resetPrintRange();
}

void KSpreadView::multiRow( bool b )
{
  if ( m_toolbarLock )
    return;

  if ( m_pTable != 0L )
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->setSelectionMultiRow( selectionInfo(), b );
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::alignLeft( bool b )
{
  if ( m_toolbarLock )
    return;

  m_pDoc->emitBeginOperation(false);
  if ( m_pTable != 0L )
  {
    if ( !b )
      m_pTable->setSelectionAlign( selectionInfo(),
                                   KSpreadLayout::Undefined );
    else
      m_pTable->setSelectionAlign( selectionInfo(),
                                   KSpreadLayout::Left );
  }
  m_pDoc->emitEndOperation();
}

void KSpreadView::alignRight( bool b )
{
  if ( m_toolbarLock )
    return;

  m_pDoc->emitBeginOperation(false);
  if ( m_pTable != 0L )
  {
    if ( !b )
      m_pTable->setSelectionAlign( selectionInfo(), KSpreadLayout::Undefined );
    else
      m_pTable->setSelectionAlign( selectionInfo(), KSpreadLayout::Right );
  }
  m_pDoc->emitEndOperation();
}

void KSpreadView::alignCenter( bool b )
{
  if ( m_toolbarLock )
    return;

  m_pDoc->emitBeginOperation(false);
  if ( m_pTable != 0L )
  {
    if ( !b )
      m_pTable->setSelectionAlign( selectionInfo(), KSpreadLayout::Undefined );
    else
      m_pTable->setSelectionAlign( selectionInfo(), KSpreadLayout::Center );
  }
  m_pDoc->emitEndOperation();
}

void KSpreadView::alignTop( bool b )
{
  if ( m_toolbarLock )
    return;


  m_pDoc->emitBeginOperation(false);
  if ( !b )
  {
      m_alignMiddle->setChecked( true);
      alignMiddle(true );
      m_pDoc->emitEndOperation();
      return;
  }

  if ( m_pTable != 0L )
    m_pTable->setSelectionAlignY( selectionInfo(), KSpreadLayout::Top );

  m_pDoc->emitEndOperation();
}

void KSpreadView::alignBottom( bool b )
{
  if ( m_toolbarLock )
    return;

  m_pDoc->emitBeginOperation(false);
  if ( !b )
  {
      m_alignMiddle->setChecked( true);
      alignMiddle(true );
      m_pDoc->emitEndOperation();
      return;
  }

  if ( m_pTable != 0L )
    m_pTable->setSelectionAlignY( selectionInfo(), KSpreadLayout::Bottom );

  m_pDoc->emitEndOperation();
}

void KSpreadView::alignMiddle( bool b )
{
  if ( m_toolbarLock )
    return;

  m_pDoc->emitBeginOperation(false);
  if ( !b )
  {
      m_alignMiddle->setChecked( true);
      m_pDoc->emitEndOperation();
      return;
  }

  if ( m_pTable != 0L )
    m_pTable->setSelectionAlignY( selectionInfo(), KSpreadLayout::Middle );

  m_pDoc->emitEndOperation();
}


void KSpreadView::moneyFormat(bool b)
{
  if ( m_toolbarLock )
    return;

  m_pDoc->emitBeginOperation(false);
  if ( m_pTable != 0L )
    m_pTable->setSelectionMoneyFormat( selectionInfo(), b );
  updateEditWidget();
  m_pDoc->emitEndOperation();
}

void KSpreadView::precisionPlus()
{
  setSelectionPrecision(1);
}

void KSpreadView::precisionMinus()
{
  setSelectionPrecision(-1);
}

void KSpreadView::setSelectionPrecision(int delta)
{
  if (m_pTable != NULL )
  {
    m_pDoc->emitBeginOperation(false);
    m_pTable->setSelectionPrecision( selectionInfo(), delta );
    m_pDoc->emitEndOperation();
  }
}

void KSpreadView::percent( bool b)
{
  if ( m_toolbarLock )
    return;

  m_pDoc->emitBeginOperation(false);
  if ( m_pTable != 0L )
    m_pTable->setSelectionPercent( selectionInfo() ,b );
  updateEditWidget();

  m_pDoc->emitEndOperation();
}

void KSpreadView::insertObject()
{
  m_pDoc->emitBeginOperation(false);
  KoDocumentEntry e =  m_insertPart->documentEntry();//KoPartSelectDia::selectPart( m_pCanvas );
  if ( e.isEmpty() )
  {
    m_pDoc->emitEndOperation();
    return;
  }

  (void)new KSpreadInsertHandler( this, m_pCanvas, e );
  m_pDoc->emitEndOperation();
}

void KSpreadView::insertChart()
{
    if( util_isColumnSelected(selection()) || util_isRowSelected(selection()) )
    {
        KMessageBox::error( this, i18n("Area too large!"));
        return;
    }
    QValueList<KoDocumentEntry> vec = KoDocumentEntry::query( "'KOfficeChart' in ServiceTypes" );
    if ( vec.isEmpty() )
    {
        KMessageBox::error( this, i18n("No charting component registered") );
        return;
    }
    m_pDoc->emitBeginOperation(false);

    (void)new KSpreadInsertHandler( this, m_pCanvas, vec[0], TRUE );
    m_pDoc->emitEndOperation();
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
   if ( doc()->map()->count() <= 1||(m_pTabBar->listshow().count()<=1) )
    {
        KNotifyClient::beep();
        KMessageBox::sorry( this, i18n("You cannot delete the only sheet."), i18n("Remove Sheet") ); // FIXME bad english? no english!
        return;
    }
    KNotifyClient::beep();
    int ret = KMessageBox::warningYesNo(this,i18n("You are about to remove the active sheet.\nDo you want to continue?"),i18n("Remove Sheet"));

    if ( ret == KMessageBox::Yes )
    {
      m_pDoc->emitBeginOperation(false);
      if ( m_pCanvas->editor() )
      {
        m_pCanvas->deleteEditor( false );
      }
      m_pDoc->setModified( true );
      KSpreadSheet *tbl = activeTable();
      KSpreadUndoRemoveTable* undo = new KSpreadUndoRemoveTable(m_pDoc, tbl);
      m_pDoc->undoBuffer()->appendUndo( undo );
      tbl->map()->takeTable( tbl );
      doc()->takeTable( tbl );
      m_pDoc->emitEndOperation();

    }
}


void KSpreadView::slotRename()
{
  m_pDoc->emitBeginOperation(false);
  m_pTabBar->slotRename();
  m_pDoc->emitEndOperation();
}

void KSpreadView::setText( const QString& _text )
{
  if ( m_pTable == 0L )
    return;

  m_pDoc->emitBeginOperation(false);
  m_pTable->setText( m_pCanvas->markerRow(), m_pCanvas->markerColumn(), _text );
  KSpreadCell* cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
  if(cell->isString() && !_text.isEmpty() && !_text.at(0).isDigit() && !cell->isFormula())
      m_pDoc->addStringCompletion(_text);

  m_pDoc->emitEndOperation();
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

void KSpreadView::slotUpdateView( KSpreadSheet *_table )
{

    // Do we display this table ?
    if ( _table != m_pTable )
        return;

    m_pDoc->emitBeginOperation(false);

    m_pTable->setRegionPaintDirty(QRect(QPoint(0,0),
                                        QPoint(KS_colMax, KS_rowMax)));

    m_pDoc->emitEndOperation();
}

void KSpreadView::slotUpdateView( KSpreadSheet *_table, const QRect& _rect )
{
    // qDebug("void KSpreadView::slotUpdateView( KSpreadSheet *_table, const QRect& %i %i|%i %i )\n",_rect.left(),_rect.top(),_rect.right(),_rect.bottom());

    // Do we display this table ?
    if ( _table != m_pTable )
        return;

    m_pDoc->emitBeginOperation(false);
    m_pTable->setRegionPaintDirty( _rect );
    m_pDoc->emitEndOperation();
}

void KSpreadView::slotUpdateHBorder( KSpreadSheet *_table )
{
    // kdDebug(36001)<<"void KSpreadView::slotUpdateHBorder( KSpreadSheet *_table )\n";

    // Do we display this table ?
    if ( _table != m_pTable )
        return;

    m_pDoc->emitBeginOperation(false);
    m_pHBorderWidget->update();
    m_pDoc->emitEndOperation();
}

void KSpreadView::slotUpdateVBorder( KSpreadSheet *_table )
{
    // kdDebug("void KSpreadView::slotUpdateVBorder( KSpreadSheet *_table )\n";

    // Do we display this table ?
    if ( _table != m_pTable )
        return;

    m_pDoc->emitBeginOperation(false);
    m_pVBorderWidget->update();
    m_pDoc->emitEndOperation();
}

void KSpreadView::slotChangeSelection( KSpreadSheet *_table,
                                       const QRect &oldSelection,
                                       const QPoint& oldMarker )
{
    m_pDoc->emitBeginOperation(false);
    QRect newSelection = m_selectionInfo->selection();

    // Emit a signal for internal use
    emit sig_selectionChanged( _table, newSelection );

    // Empty selection ?
    // Activate or deactivate some actions.
    bool colSelected = util_isColumnSelected( selection() );
    bool rowSelected = util_isRowSelected( selection() );

    m_resizeRow->setEnabled( !colSelected );
    m_equalizeRow->setEnabled( !colSelected );

    m_resizeColumn->setEnabled( !rowSelected );
    m_equalizeColumn->setEnabled( !rowSelected );

    bool simpleSelection = m_selectionInfo->singleCellSelection() ||
			   colSelected || rowSelected;

    m_tableFormat->setEnabled( !simpleSelection );
    m_mergeCell->setEnabled( !simpleSelection );
    m_insertChartFrame->setEnabled( !simpleSelection );

    resultOfCalc();
    // Send some event around. This is read for example
    // by the calculator plugin.
    KSpreadSelectionChanged ev( newSelection, activeTable()->name() );
    QApplication::sendEvent( this, &ev );

    // Do we display this table ?
    if ( _table != m_pTable )
    {
      m_pDoc->emitEndOperation();
      return;
    }

    m_pCanvas->setSelectionChangePaintDirty(m_pTable, oldSelection, newSelection);

    m_pVBorderWidget->update();
    m_pHBorderWidget->update();
    m_pDoc->emitEndOperation();
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
                if ( c->isNumeric() )
                {
                  double val = c->valueDouble();
                  switch(tmpMethod)
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
              if ( !c->isObscuringForced() && c->isNumeric() )
              {
                double val = c->valueDouble();
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
              if ( !cell->isDefault() && cell->isNumeric() )
              {
                double val = cell->valueDouble();
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

    m_pDoc->emitBeginOperation();
    if ( m_sbCalcLabel )
        m_sbCalcLabel->setText(QString(" ") + tmp + ' ');
    m_pDoc->emitEndOperation();
}

void KSpreadView::statusBarClicked(int _id)
{
    if(!koDocument()->isReadWrite() || !factory())
        return;
    if(_id==0) //menu calc
    {
        QPoint mousepos =QCursor::pos();
        ((QPopupMenu*)factory()->container("calc_popup",this))->popup(mousepos);
    }
}

void KSpreadView::menuCalc(bool)
{
  m_pDoc->emitBeginOperation(false);
    if( m_menuCalcMin->isChecked())
    {
        doc()->setTypeOfCalc(Min);
    }
    else if(m_menuCalcMax->isChecked())
    {
        doc()->setTypeOfCalc(Max);
    }
    else if(m_menuCalcCount->isChecked())
    {
        doc()->setTypeOfCalc(Count);
    }
    else if(m_menuCalcAverage->isChecked())
    {
        doc()->setTypeOfCalc(Average);
    }
    else if(m_menuCalcSum->isChecked())
    {
        doc()->setTypeOfCalc(SumOfNumber);
    }
    else if( m_menuCalcNone->isChecked())
        doc()->setTypeOfCalc(NoneCalc);

    resultOfCalc();

    m_pDoc->emitEndOperation();
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
    m_transform->setEnabled( TRUE );

    if ( !m_transformToolBox.isNull() )
    {
        m_transformToolBox->setEnabled( TRUE );
        m_transformToolBox->setDocumentChild( ch );
    }
}

void KSpreadView::slotChildUnselected( KoDocumentChild* )
{
    m_transform->setEnabled( FALSE );

    if ( !m_transformToolBox.isNull() )
    {
        m_transformToolBox->setEnabled( FALSE );
    }
    deleteEditor( true );
}


void KSpreadView::deleteEditor( bool saveChanges )
{
  m_pDoc->emitBeginOperation(false);
  m_pCanvas->deleteEditor( saveChanges );
  m_pDoc->emitEndOperation();
}

DCOPObject* KSpreadView::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KSpreadViewIface( this );

    return m_dcop;
}

QWidget *KSpreadView::canvas()
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
    m_pDoc->emitBeginOperation(false);

    if ( ev->activated() )
    {
        if ( m_sbCalcLabel )
        {
            resultOfCalc();
        }
    }
    else
    {
        /*if(m_sbCalcLabel)
        {
            disconnect(m_sbCalcLabel,SIGNAL(pressed( int )),this,SLOT(statusBarClicked(int)));
            }*/
    }

    m_pDoc->emitEndOperation();
    KoView::guiActivateEvent( ev );
}

void KSpreadView::openPopupMenuMenuPage( const QPoint & _point )
{
    if(!koDocument()->isReadWrite() || !factory())
        return;
    if( m_pTabBar )
    {
        bool state = (m_pTabBar->listshow().count()>1);
        m_removeTable->setEnabled( state);
        m_hideTable->setEnabled( state );
        static_cast<QPopupMenu*>(factory()->container("menupage_popup",this))->popup(_point);
    }
}

void KSpreadView::updateBorderButton()
{
    m_pDoc->emitBeginOperation(false);
    m_showPageBorders->setChecked( m_pTable->isShowPageBorders() );
    m_pDoc->emitEndOperation();
}

void KSpreadView::removeTable( KSpreadSheet *_t )
{
  m_pDoc->emitBeginOperation(false);
  QString m_tablName=_t->tableName();
  m_pTabBar->removeTab( m_tablName );
  setActiveTable( m_pDoc->map()->findTable( m_pTabBar->listshow().first() ));
  bool state =m_pTabBar->listshow().count()>1;
  m_removeTable->setEnabled( state );
  m_hideTable->setEnabled( state );
  m_pDoc->emitEndOperation();
}

void KSpreadView::insertTable( KSpreadSheet* table )
{
  m_pDoc->emitBeginOperation(false);
  QString tabName = table->tableName();
  if( !table->isHidden() )
  {
    m_pTabBar->addTab(tabName);
  }
  else
  {
    m_pTabBar->addHiddenTab(tabName);
  }
  bool state =m_pTabBar->listshow().count()>1;
  m_removeTable->setEnabled( state );
  m_hideTable->setEnabled( state );
  m_pDoc->emitEndOperation();
}

QColor KSpreadView::borderColor() const
{
  return m_borderColor->color();
}

void KSpreadView::updateShowTableMenu()
{
  m_pDoc->emitBeginOperation(false);
  m_showTable->setEnabled(m_pTabBar->listhide().count()>0);
  m_pDoc->emitEndOperation();
}

void KSpreadView::closeEditor()
{
  m_pDoc->emitBeginOperation(false);
  m_pCanvas->closeEditor();
  m_pDoc->emitEndOperation();
}


void KSpreadView::paintUpdates()
{
  /* don't do any begin/end operation here -- this is what is called at an
     endOperation
  */
  m_pCanvas->paintUpdates();
}

#include "kspread_view.moc"

