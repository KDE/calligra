/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001 David Faure <faure@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

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
 * Boston, MA 02110-1301, USA
*/

#undef Unsorted

#include "KWView.h"

#include "KWordViewIface.h"
#include "KWConfigFootNoteDia.h"
#include "defs.h"
#include "KWDeleteDia.h"
#include "KWDocStruct.h"
#include "KWFootNoteDia.h"
#include "KWInsertDia.h"
#include "KWAnchor.h"
#include "KoTextBookmark.h"
#include "KWCanvas.h"
#include "KWCommand.h"
#include "KWConfig.h"
#include "KWCreateBookmarkDia.h"
#include "KWDocument.h"
#include "KWEditPersonnalExpression.h"
#include "KWFormulaFrameSet.h"
#include "KWFrame.h"
#include "KWPictureFrameSet.h"
#include "KWFrameStyle.h"
#include "KWFrameStyleManager.h"
#include "KWImportStyleDia.h"
#include "KWInsertPageDia.h"
#include "KWInsertPicDia.h"
#include "KWPartFrameSet.h"
#include "KWStyleManager.h"
#include "KWTableFrameSet.h"
#include "KWTableStyle.h"
#include "KWTableStyleManager.h"
#include "KWTextDocument.h"
#include "KWVariable.h"
#include "KWViewMode.h"
#include "KWMailMergeDataBase.h"
#include "KWMailMergeLabelAction.h"
#include "KWResizeTableDia.h"
#include "KWFindReplace.h"
#include "KWSortDia.h"
#include "KWSplitCellDia.h"
#include "KWTableDia.h"
#include "KWCollectFramesetsVisitor.h"
#include "KWOasisLoader.h"
#include "KWOasisSaver.h"
#include "KWFrameList.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include "KWFrameViewManager.h"
#include "KWFrameView.h"
#include "KWStatisticsDialog.h"

#include <kformuladocument.h>
#include <kformulamimesource.h>

#include <KoRichText.h>
#include <KoAutoFormat.h>
#include <KoAutoFormatDia.h>
#include <KoChangeCaseDia.h>
#include <KoCharSelectDia.h>
#include <KoCommentDia.h>
#include <KoCreateStyleDia.h>
#include <KoDocumentInfo.h>
#include <KoFontDia.h>
#include <KoFrame.h>
#include <KoInsertLink.h>
#include <KoMainWindow.h>
#include <KoParagDia.h>
#include <KoPartSelectAction.h>
#include <KoPictureFilePreview.h>
#include <KoSearchDia.h>
#include <KoStore.h>
#include <KoStoreDrag.h>
#include <KoTemplateCreateDia.h>
#include <KoCompletionDia.h>
#include <KoVariable.h>
#include <KoCustomVariablesDia.h>
#include <KoTextObject.h>
#include <tkcoloractions.h>

#include <kparts/partmanager.h>
#include <kaccelgen.h>
#include <kcolordialog.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kinputdialog.h>
#include <kio/netaccess.h>
#include <kmessagebox.h>
#include <kparts/event.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kstdaccel.h>
#include <kstdaction.h>
#include <ktempfile.h>
#include <kurldrag.h>
#include <kdeversion.h>
#include <kiconloader.h>

#include <qclipboard.h>
#include <qapplication.h>
#include <q3groupbox.h>
#include <qlayout.h>
#include <q3paintdevicemetrics.h>
#include <q3progressdialog.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qbuffer.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <Q3PopupMenu>
#include <Q3StrList>
#include <Q3GridLayout>
#include <Q3CString>
#include <Q3ValueList>
#include <Q3PtrList>
#include <QPixmap>

#include <stdlib.h>

#include <kspell2/dialog.h>
#include <kspell2/defaultdictionary.h>
#include <q3tl.h>
#include <kvbox.h>
#include "KoSpell.h"

using namespace KSpell2;

/******************************************************************/
/* Class: TableInfo                                               */
/******************************************************************/
class TableInfo {
    public:
        TableInfo( const Q3ValueList<KWFrameView*>& selectedFrames ) {
            m_protectContent = false;
            //m_views = selectedFrames;
            int amountSelected = 0;
            m_cell = 0;
            QMap<KWTableFrameSet*, Q3ValueList<unsigned int> > tableRows, tableCols;

            Q3ValueList<KWFrameView*>::const_iterator framesIterator = selectedFrames.begin();
            for(;framesIterator != selectedFrames.end(); ++framesIterator) {
                KWFrameView *view = *framesIterator;
                if(!view->selected()) continue;
                KWFrameSet *fs = view->frame()->frameSet();
                Q_ASSERT(fs);
                KWTableFrameSet::Cell *cell = dynamic_cast<KWTableFrameSet::Cell*>(fs);
                if(cell == 0) continue;
                amountSelected++;
                if(cell->protectContent())
                    m_protectContent=true;

                if(! tableRows.contains(fs->groupmanager())) { // create empty lists.
                    Q3ValueList<unsigned int> rows;
                    for(unsigned int i=fs->groupmanager()->getRows(); i != 0; i--)
                        rows.append(0);
                    tableRows.insert(fs->groupmanager(), rows);
                    Q3ValueList<unsigned int> cols;
                    for(unsigned int i=fs->groupmanager()->getColumns(); i != 0; i--)
                        cols.append(0);
                    tableCols.insert(fs->groupmanager(), cols);
                }
                Q3ValueList<unsigned int> rows = tableRows[fs->groupmanager()];
                for(unsigned int r=cell->firstRow(); r <= cell->lastRow(); r++)
                    rows[r] = rows[r] + 1;
                tableRows[fs->groupmanager()] = rows;
                Q3ValueList<unsigned int> columns = tableCols[fs->groupmanager()];
                for(unsigned int c=cell->firstColumn(); c <= cell->lastColumn(); c++)
                    columns[c] = columns[c] + 1;
                tableCols[fs->groupmanager()] = columns;

                if(m_cell == 0 || m_cell->firstRow() > cell->firstRow() ||
                        m_cell->firstRow() == cell->firstRow() &&
                        m_cell->firstColumn() > cell->firstColumn())
                    m_cell = cell;
            }

            m_selected = amountSelected != 0;
            m_oneCellSelected = amountSelected == 1;
            if(amountSelected == 0) return;

            for(QMapIterator<KWTableFrameSet*, Q3ValueList<unsigned int> > iter = tableRows.begin();
                    iter != tableRows.end(); ++iter) {
                Q3ValueList<unsigned int> rows = iter.data();
                Q3ValueListIterator<unsigned int> rowsIter = rows.begin();
                for(int x=0;rowsIter != rows.end(); ++rowsIter, x++)
                    if(*rowsIter == iter.key()->getColumns())
                        m_rows.append(x);

                Q3ValueList<unsigned int> columns = tableCols[iter.key()];
                Q3ValueListIterator<unsigned int> colsIter = columns.begin();
                for(int x=0;colsIter != columns.end(); ++colsIter, x++)
                    if(*colsIter == iter.key()->getRows())
                        m_columns.append(x);
            }
        }

        int tableCellsSelected() const { return m_selected; }
        int amountRowsSelected() const { return m_rows.count(); }
        int amountColumnsSelected() const { return m_columns.count(); }
        bool oneCellSelected() const { return m_oneCellSelected; }
        bool protectContentEnabled() const { return m_protectContent; }
        Q3ValueList<uint> selectedRows() const { return m_rows; }
        Q3ValueList<uint> selectedColumns() const { return m_columns; }
        KWTableFrameSet::Cell *firstSelectedCell() const { return m_cell; }
    private:
        //QValueList<KWFrameView*> m_views;
        bool m_oneCellSelected, m_selected, m_protectContent;
        Q3ValueList<uint> m_rows, m_columns;
        KWTableFrameSet::Cell *m_cell;
};

/******************************************************************/
/* Class: KWView                                                  */
/******************************************************************/
KWView::KWView( const QString& viewMode, QWidget *parent, const char *name, KWDocument* doc )
    : KoView( doc, parent, name )
{
    m_doc = doc;
    m_gui = 0;

    m_dcop = 0;
    dcopObject(); // build it
    m_fsInline=0;
    m_spell.kospell = 0;
    m_spell.dlg = 0;
    m_broker = Broker::openBroker( KSharedConfig::openConfig( "kwordrc" ) );
    m_spell.macroCmdSpellCheck=0L;
    m_spell.textIterator = 0L;
    m_currentPage = m_doc->pageManager()->page(m_doc->startPage());
    m_specialCharDlg=0L;
    m_searchEntry = 0L;
    m_replaceEntry = 0L;
    m_findReplace = 0L;
    m_fontDlg = 0L;
    m_paragDlg = 0L;
    m_tableSplit.columns = 1;
    m_tableSplit.rows = 1;

    m_actionList.setAutoDelete( true );
    m_variableActionList.setAutoDelete( true );
    // Default values.
    m_zoomViewModeNormal.m_zoom = m_doc->zoom();
    m_zoomViewModeNormal.m_zoomMode = m_doc->zoomMode();
    m_zoomViewModePreview.m_zoom = 33; // TODO: bad to leave hardcoded...
    m_zoomViewModePreview.m_zoomMode = KoZoomMode::ZOOM_CONSTANT;

    m_viewFrameBorders = m_doc->viewFrameBorders();
    KoView::setZoom( m_doc->zoomedResolutionY() /* KoView only supports one zoom */ ); // initial value
    //m_viewTableGrid = true;

    setInstance( KWFactory::instance() );
    if ( !m_doc->isReadWrite() )
        setXMLFile( "kword_readonly.rc" );
    else
        setXMLFile( "kword.rc" );


    QObject::connect( this, SIGNAL( embeddImage( const QString & ) ),
                      this, SLOT( slotEmbedImage( const QString & ) ) );

    setKeyCompression( TRUE );
    setAcceptDrops( TRUE );

    setupActions();

    m_gui = new KWGUI( viewMode, this, this );
    m_gui->setGeometry( 0, 0, width(), height() );
    m_gui->show();

    m_sbPageLabel = 0;
    m_sbModifiedLabel = 0;
    m_sbFramesLabel = 0;
    m_sbOverwriteLabel = 0;
    m_sbZoomLabel = 0;
    m_sbUnitLabel = 0;
    if ( KStatusBar* sb = statusBar() ) // No statusbar in e.g. konqueror
    {
        m_sbPageLabel = new KStatusBarLabel( QString::null, 0, sb );
        m_sbPageLabel->setAlignment( AlignLeft | AlignVCenter );
        addStatusBarItem( m_sbPageLabel, 0 );

        m_sbModifiedLabel = new KStatusBarLabel( "   ", 0, sb );
        m_sbModifiedLabel->setAlignment( AlignLeft | AlignVCenter );
        addStatusBarItem( m_sbModifiedLabel, 0 );

        m_sbFramesLabel = new KStatusBarLabel( QString::null, 0, sb );
        m_sbFramesLabel->setAlignment( AlignLeft | AlignVCenter );
        addStatusBarItem( m_sbFramesLabel, 1 );

        m_sbOverwriteLabel = new KStatusBarLabel( ' ' + i18n( "INSRT" ) + ' ', 0, sb );
        m_sbOverwriteLabel->setAlignment( AlignHCenter | AlignVCenter );
        addStatusBarItem( m_sbOverwriteLabel, 0 );

        m_sbZoomLabel = new KStatusBarLabel( ' ' + QString::number( m_doc->zoom() ) + "% ", 0, sb );
        m_sbZoomLabel->setAlignment( AlignHCenter | AlignVCenter );
        addStatusBarItem( m_sbZoomLabel, 0 );

        m_sbUnitLabel = new KStatusBarLabel( ' ' + KoUnit::unitDescription( m_doc->unit() ) + ' ', 0, sb );
        m_sbUnitLabel->setAlignment( AlignHCenter | AlignVCenter );
        addStatusBarItem( m_sbUnitLabel, 0 );
    }

    connect( m_doc, SIGNAL( modified( bool ) ),
             this, SLOT( documentModified( bool )) );

    connect( m_doc, SIGNAL( numPagesChanged() ),
             this, SLOT( numPagesChanged()) );

    connect( m_doc, SIGNAL( pageLayoutChanged( const KoPageLayout& ) ),
             this, SLOT( slotPageLayoutChanged( const KoPageLayout& )) );

    connect( m_doc, SIGNAL( docStructureChanged(int) ),
             this, SLOT( docStructChanged(int)) );

    connect( m_doc, SIGNAL( unitChanged(KoUnit::Unit) ),
             this, SLOT( slotUnitChanged(KoUnit::Unit) ) );

    connect( m_doc, SIGNAL( sig_refreshMenuCustomVariable()),
             this, SLOT( refreshCustomMenu()));

    connect( m_doc, SIGNAL( completed() ),
             this, SLOT( slotDocumentLoadingCompleted() ) );

    connect( frameViewManager(), SIGNAL(sigFrameSelectionChanged()),
             this, SLOT( frameSelectedChanged()));

    connect( frameViewManager(), SIGNAL(sigFrameSetRenamed()),
             this, SLOT( updateFrameStatusBarItem()));

    connect( QApplication::clipboard(), SIGNAL( dataChanged() ),
             this, SLOT( clipboardDataChanged() ) );

    connect( m_gui->canvasWidget(), SIGNAL(currentFrameSetEditChanged()),
             this, SLOT(slotFrameSetEditChanged()) );

    connect( m_gui->canvasWidget(), SIGNAL( currentMouseModeChanged(int) ),
             this, SLOT( showMouseMode(int) ) );

    connect( m_gui->canvasWidget(), SIGNAL( overwriteModeChanged( bool ) ),
             this, SLOT( changeOverwriteMode( bool ) ) );

    // Cut and copy are directly connected to the selectionChanged signal
    if ( m_doc->isReadWrite() )
    {
        connect( m_gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
                 this, SLOT(slotChangeCutState(bool )) );
        connect (m_gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
                 this, SLOT(slotChangeCaseState(bool )));
    }
    else
    {
        m_actionEditCut->setEnabled( false );
        m_actionChangeCase->setEnabled( false );
    }

    connect( m_gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
             m_actionEditCopy, SLOT(setEnabled(bool)) );

    //connect (m_gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
    //         m_actionCreateStyleFromSelection, SLOT(setEnabled(bool)));

    connect (m_gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
             m_actionConvertToTextBox, SLOT(setEnabled(bool)));
    connect (m_gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
             m_actionAddPersonalExpression, SLOT(setEnabled(bool )));
    connect (m_gui->canvasWidget(), SIGNAL(selectionChanged(bool)),
             m_actionSortText, SLOT(setEnabled(bool )));

    connect( m_gui->canvasWidget(), SIGNAL(docStructChanged(int)),
             this, SLOT(docStructChanged(int)));

    connect( m_gui->canvasWidget(), SIGNAL(updateRuler()),
             this, SLOT(slotUpdateRuler()));

    if ( shell() )
    {
        connect( shell(), SIGNAL( documentSaved()), m_doc,SLOT(slotDocumentInfoModifed() ) );
        changeNbOfRecentFiles( m_doc->maxRecentFiles() );
    }

    m_gui->canvasWidget()->updateCurrentFormat();
    setFocusProxy( m_gui->canvasWidget() );

    //when kword is embedded into konqueror apply a zoom=100
    //in konqueror we can't change zoom -- ### TODO ?
    if(!m_doc->isReadWrite())
    {
        setZoom( 100, true );
        slotUpdateRuler();
        initGui();
    }

    // Determine initial scroll position
    // We do this delayed, so that the GUI has been fully constructed
    // (and e.g. the statusbar can repaint).
    QTimer::singleShot( 0, this, SLOT( slotSetInitialPosition() ) );
    QTimer::singleShot( 0, this, SLOT( updateZoom() ) );
}

KWView::~KWView()
{
    delete m_tableActionList.first(); // the first one is the separator.
    clearSpellChecker();

    delete m_searchEntry;
    m_searchEntry = 0L;
    delete m_replaceEntry;
    m_replaceEntry = 0L;
    if ( m_specialCharDlg )
        m_specialCharDlg->closeDialog(); // will call slotSpecialCharDlgClosed

    // Abort any find/replace
    delete m_findReplace;
    // Delete gui while we still exist ( it needs documentDeleted() )
    delete m_gui;
    delete m_sbPageLabel;
    delete m_sbFramesLabel;
    delete m_fsInline;
    delete m_dcop;
    delete m_fontDlg;
    delete m_paragDlg;
}

DCOPObject* KWView::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KWordViewIface( this );

    return m_dcop;
}

void KWView::slotChangeCutState(bool b)
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit && edit->textFrameSet()->protectContent())
        m_actionEditCut->setEnabled( false );
    else
        m_actionEditCut->setEnabled( b );
}

void KWView::slotChangeCaseState(bool b)
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit && edit->textFrameSet()->protectContent())
        m_actionChangeCase->setEnabled( false );
    else
        m_actionChangeCase->setEnabled( b );
}

void KWView::slotSetInitialPosition()
{
    KWTextFrameSetEdit* textedit = dynamic_cast<KWTextFrameSetEdit *>(m_gui->canvasWidget()->currentFrameSetEdit());
    if ( textedit )
        textedit->ensureCursorVisible();
    else
        m_gui->canvasWidget()->setContentsPos( 0, 0 );
}

void KWView::changeNbOfRecentFiles(int nb)
{
    if ( shell() ) // 0 when embedded into konq !
        shell()->setMaxRecentItems( nb );
}

KWViewMode* KWView::viewMode() const
{
    return m_gui->canvasWidget()->viewMode();
}

void KWView::initGui()
{
    clipboardDataChanged();
    if ( m_gui )
        m_gui->showGUI();
    showMouseMode( KWCanvas::MM_EDIT );
    initGUIButton();
    m_actionFormatDecreaseIndent->setEnabled(false);
    //setNoteType(m_doc->getNoteType(), false);

    m_actionFormatColor->setCurrentColor( Qt::black );

    updateGridButton();

    //refresh zoom combobox
    updateZoomControls();


    // This is probably to emit currentMouseModeChanged and set the cursor
    m_gui->canvasWidget()->setMouseMode( m_gui->canvasWidget()->mouseMode() );

    bool editingFormula = dynamic_cast<KWFormulaFrameSetEdit *>( m_gui->canvasWidget()->currentFrameSetEdit() ) != 0;
    //showFormulaToolbar( FALSE ); // not called, to avoid creating the formula-document if not necessary
    if(shell())
        shell()->showToolbar( "formula_toolbar", editingFormula );

    if ( !editingFormula )
    {
       kWordDocument()->formulaDocumentWrapper()->setEnabled(false);
       kWordDocument()->formulaDocumentWrapper()->enableMatrixActions(false);
       kWordDocument()->formulaDocumentWrapper()->getSyntaxHighlightingAction()->setEnabled(false);
    }

    // Prevention against applyMainWindowSettings hiding the statusbar
    if ( KStatusBar* sb = statusBar() )
        sb->show();

    updatePageInfo();
    slotFrameSetEditChanged();
    frameSelectedChanged();
    updateTocActionText(m_doc->tocPresent());
    //at the beginning m_actionBackgroundColor should be active
    m_actionBackgroundColor->setEnabled(true);
    updateBgSpellCheckingState();
    updateDirectCursorButton();
    m_actionCreateFrameStyle->setEnabled(false);
}


void KWView::updateBgSpellCheckingState()
{
    m_actionAllowBgSpellCheck->setChecked( m_doc->backgroundSpellCheckEnabled() );
}


void KWView::initGUIButton()
{
    m_actionViewFrameBorders->setChecked( viewFrameBorders() );
    m_actionViewFormattingChars->setChecked( m_doc->viewFormattingChars() );
    m_actionShowDocStruct->setChecked(m_doc->showdocStruct());
    m_actionShowRuler->setChecked(m_doc->showRuler());

    updateHeaderFooterButton();
    m_actionAllowAutoFormat->setChecked( m_doc->allowAutoFormat() );

    if ( !m_doc->isEmbedded() ) {
        QString mode = viewMode()->type();
        if (mode=="ModePreview")
            m_actionViewPreviewMode->setChecked(true);
        else if (mode=="ModeText")
            m_actionViewTextMode->setChecked(true);
        else //if (mode=="ModeNormal")
            m_actionViewPageMode->setChecked(true);
        switchModeView();
    }
}

void KWView::setupActions()
{
    // The actions here are grouped by menu, because this helps noticing
    // accelerator clashes.

    // -------------- File menu
    m_actionExtraCreateTemplate = new KAction( i18n( "&Create Template From Document..." ), 0,
                                             this, SLOT( extraCreateTemplate() ),
                                             actionCollection(), "extra_template" );
    m_actionExtraCreateTemplate->setToolTip( i18n( "Save this document and use it later as a template" ) );
    m_actionExtraCreateTemplate->setWhatsThis( i18n( "You can save this document as a template.<br><br>You can use this new template as a starting point for another document." ) );

    m_actionFileStatistics = new KAction( i18n( "Statistics" ), 0, this, SLOT( fileStatistics() ), actionCollection(), "file_statistics" );
    m_actionFileStatistics->setToolTip( i18n( "Sentence, word and letter counts for this document" ) );
    m_actionFileStatistics->setWhatsThis( i18n( "Information on the number of letters, words, syllables and sentences for this document.<p>Evaluates readability using the Flesch reading score." ) );
    // -------------- Edit actions
    m_actionEditCut = KStdAction::cut( this, SLOT( editCut() ), actionCollection(), "edit_cut" );
    m_actionEditCopy = KStdAction::copy( this, SLOT( editCopy() ), actionCollection(), "edit_copy" );
    m_actionEditPaste = KStdAction::paste( this, SLOT( editPaste() ), actionCollection(), "edit_paste" );
    m_actionEditFind = KStdAction::find( this, SLOT( editFind() ), actionCollection(), "edit_find" );
    m_actionEditFindNext = KStdAction::findNext( this, SLOT( editFindNext() ), actionCollection(), "edit_findnext" );
    m_actionEditFindPrevious = KStdAction::findPrev( this, SLOT( editFindPrevious() ), actionCollection(), "edit_findprevious" );
    m_actionEditReplace = KStdAction::replace( this, SLOT( editReplace() ), actionCollection(), "edit_replace" );
    m_actionEditSelectAll = KStdAction::selectAll( this, SLOT( editSelectAll() ), actionCollection(), "edit_selectall" );
    new KAction( i18n( "Select All Frames" ), 0, this, SLOT( editSelectAllFrames() ), actionCollection(), "edit_selectallframes" );
    m_actionEditSelectCurrentFrame = new KAction( i18n( "Select Frame" ), 0,
        0, this, SLOT( editSelectCurrentFrame() ),
        actionCollection(), "edit_selectcurrentframe" );
    m_actionSpellCheck = KStdAction::spelling( this, SLOT( slotSpellCheck() ), actionCollection(), "extra_spellcheck" );
    m_actionDeletePage = new KAction( i18n( "Delete Page" ), "delslide", 0,
                                    this, SLOT( deletePage() ),
                                    actionCollection(), "delete_page" );
    kDebug() <<  m_doc->pageCount() <<  " " << (m_doc->processingType() == KWDocument::DTP) << endl;

    (void) new KAction( i18n( "Configure Mai&l Merge..." ), "configure",0,
                        this, SLOT( editMailMergeDataBase() ),
                        actionCollection(), "edit_sldatabase" );


    (void) new KWMailMergeLabelAction::KWMailMergeLabelAction( i18n("Drag Mail Merge Variable"), 0,
                    this, SLOT(editMailMergeDataBase()), actionCollection(), "mailmerge_draglabel" );

//    (void) new KWMailMergeComboAction::KWMailMergeComboAction(i18n("Insert Mailmerge Var"),0,this,SLOT(JWJWJW()),actionCollection(),"mailmerge_varchooser");

    // -------------- Frame menu
    m_actionEditDelFrame = new KAction( i18n( "&Delete Frame" ), 0,
                                      this, SLOT( editDeleteFrame() ),
                                      actionCollection(), "edit_delframe" );
    m_actionEditDelFrame->setToolTip( i18n( "Delete the currently selected frame(s)" ) );
    m_actionEditDelFrame->setWhatsThis( i18n( "Delete the currently selected frame(s)." ) );

    m_actionCreateLinkedFrame = new KAction( i18n( "Create Linked Copy" ), 0, this, SLOT( createLinkedFrame() ), actionCollection(), "create_linked_frame" );
    m_actionCreateLinkedFrame->setToolTip( i18n( "Create a copy of the current frame, always showing the same contents" ) );
    m_actionCreateLinkedFrame->setWhatsThis( i18n("Create a copy of the current frame, that remains linked to it. This means they always show the same contents: modifying the contents in such a frame will update all its linked copies.") );

    m_actionRaiseFrame = new KAction( i18n( "Ra&ise Frame" ), "raise",
                                    Qt::CTRL +Qt::SHIFT+ Qt::Key_R, this, SLOT( raiseFrame() ),
                                    actionCollection(), "raiseframe" );
    m_actionRaiseFrame->setToolTip( i18n( "Raise the currently selected frame so that it appears above all the other frames" ) );
    m_actionRaiseFrame->setWhatsThis( i18n( "Raise the currently selected frame so that it appears above all the other frames. This is only useful if frames overlap each other. If multiple frames are selected they are all raised in turn." ) );

    m_actionLowerFrame = new KAction( i18n( "&Lower Frame" ), "lower",
                                    Qt::CTRL +Qt::SHIFT+ Qt::Key_L, this, SLOT( lowerFrame() ),
                                    actionCollection(), "lowerframe" );
    m_actionLowerFrame->setToolTip( i18n( "Lower the currently selected frame so that it disappears under any frame that overlaps it" ) );
    m_actionLowerFrame->setWhatsThis( i18n( "Lower the currently selected frame so that it disappears under any frame that overlaps it. If multiple frames are selected they are all lowered in turn." ) );

    m_actionBringToFront= new KAction( i18n( "Bring to Front" ), "bring_forward",
                                          0, this, SLOT( bringToFront() ),
                                          actionCollection(), "bring_tofront_frame" );

    m_actionSendBackward= new KAction( i18n( "Send to Back" ), "send_backward",
                                          0, this, SLOT( sendToBack() ),
                                          actionCollection(), "send_toback_frame" );


    // -------------- View menu

    if ( !m_doc->isEmbedded() ) {

        m_actionViewTextMode = new KToggleAction( i18n( "Text Mode" ), 0,
                                                  this, SLOT( viewTextMode() ),
                                                  actionCollection(), "view_textmode" );
        m_actionViewTextMode->setToolTip( i18n( "Only show the text of the document" ) );
        m_actionViewTextMode->setWhatsThis( i18n( "Do not show any pictures, formatting or layout. KWord will display only the text for editing." ) );

        m_actionViewTextMode->setExclusiveGroup( "viewmodes" );
        m_actionViewPageMode = new KToggleAction( i18n( "&Page Mode" ), 0,
                                                  this, SLOT( viewPageMode() ),
                                                  actionCollection(), "view_pagemode" );
        m_actionViewPageMode->setWhatsThis( i18n( "Switch to page mode.<br><br> Page mode is designed to make editing your text easy.<br><br>This function is most frequently used to return to text editing after switching to preview mode." ) );
        m_actionViewPageMode->setToolTip( i18n( "Switch to page editing mode" ) );

        m_actionViewPageMode->setExclusiveGroup( "viewmodes" );
        m_actionViewPageMode->setChecked( true );
        m_actionViewPreviewMode = new KToggleAction( i18n( "Pre&view Mode" ), 0,
                                                     this, SLOT( viewPreviewMode() ),
                                                     actionCollection(), "view_previewmode" );
        m_actionViewPreviewMode->setWhatsThis( i18n( "Zoom out from your document to get a look at several pages of your document.<br><br>The number of pages per line can be customized." ) );
        m_actionViewPreviewMode->setToolTip( i18n( "Zoom out to a multiple page view" ) );

        m_actionViewPreviewMode->setExclusiveGroup( "viewmodes" );
    }
    else // no viewmode switching when embedded; at least "Page" makes no sense
    {
        m_actionViewTextMode = 0;
        m_actionViewPageMode = 0;
        m_actionViewPreviewMode = 0;
    }

    m_actionViewFormattingChars = new KToggleAction( i18n( "&Formatting Characters" ), 0,
                                                   this, SLOT( slotViewFormattingChars() ),
                                                   actionCollection(), "view_formattingchars" );
    m_actionViewFormattingChars->setToolTip( i18n( "Toggle the display of non-printing characters" ) );
    m_actionViewFormattingChars->setWhatsThis( i18n( "Toggle the display of non-printing characters.<br><br>When this is enabled, KWord shows you tabs, spaces, carriage returns and other non-printing characters." ) );

    m_actionViewFrameBorders = new KToggleAction( i18n( "Frame &Borders" ), 0,
                                                   this, SLOT( slotViewFrameBorders() ),
                                                   actionCollection(), "view_frameborders" );
    m_actionViewFrameBorders->setToolTip( i18n( "Turns the border display on and off" ) );
    m_actionViewFrameBorders->setWhatsThis( i18n( "Turns the border display on and off.<br><br>The borders are never printed. This option is useful to see how the document will appear on the printed page." ) );

    m_actionViewHeader = new KToggleAction( i18n( "Enable Document &Headers" ), 0,
                                          this, SLOT( viewHeader() ),
                                          actionCollection(), "format_header" );
    m_actionViewHeader->setCheckedState(i18n("Disable Document &Headers"));
    m_actionViewHeader->setToolTip( i18n( "Shows and hides header display" ) );
    m_actionViewHeader->setWhatsThis( i18n( "Selecting this option toggles the display of headers in KWord.<br><br>Headers are special frames at the top of each page which can contain page numbers or other information." ) );

    m_actionViewFooter = new KToggleAction( i18n( "Enable Document Foo&ters" ), 0,
                                          this, SLOT( viewFooter() ),
                                          actionCollection(), "format_footer" );
    m_actionViewFooter->setCheckedState(i18n("Disable Document Foo&ters"));
    m_actionViewFooter->setToolTip( i18n( "Shows and hides footer display" ) );
    m_actionViewFooter->setWhatsThis( i18n( "Selecting this option toggles the display of footers in KWord. <br><br>Footers are special frames at the bottom of each page which can contain page numbers or other information." ) );

    m_actionViewZoom = new KSelectAction( i18n( "Zoom" ), "viewmag", 0,
                                        actionCollection(), "view_zoom" );

    connect( m_actionViewZoom, SIGNAL( activated( const QString & ) ),
             this, SLOT( viewZoom( const QString & ) ) );
    m_actionViewZoom->setEditable(true);
    changeZoomMenu( );

    // -------------- Insert menu
    m_actionInsertSpecialChar = new KAction( i18n( "Sp&ecial Character..." ), "char",
                        Qt::ALT + Qt::SHIFT + Qt::Key_C,
                        this, SLOT( insertSpecialChar() ),
                        actionCollection(), "insert_specialchar" );
    m_actionInsertSpecialChar->setToolTip( i18n( "Insert one or more symbols or letters not found on the keyboard" ) );
    m_actionInsertSpecialChar->setWhatsThis( i18n( "Insert one or more symbols or letters not found on the keyboard." ) );

    m_actionInsertFrameBreak = new KAction( QString::null, Qt::CTRL + Qt::Key_Return,
                                          this, SLOT( insertFrameBreak() ),
                                          actionCollection(), "insert_framebreak" );
    if ( m_doc->processingType() == KWDocument::WP ) {
        m_actionInsertFrameBreak->setText( i18n( "Page Break" ) );
        m_actionInsertFrameBreak->setToolTip( i18n( "Force the remainder of the text into the next page" ) );
        m_actionInsertFrameBreak->setWhatsThis( i18n( "This inserts a non-printing character at the current cursor position. All text after this point will be moved into the next page." ) );
    } else {
        m_actionInsertFrameBreak->setText( i18n( "&Hard Frame Break" ) );
        m_actionInsertFrameBreak->setToolTip( i18n( "Force the remainder of the text into the next frame" ) );
        m_actionInsertFrameBreak->setWhatsThis( i18n( "This inserts a non-printing character at the current cursor position. All text after this point will be moved into the next frame in the frameset." ) );
    }

    /*actionInsertPage =*/ new KAction( m_doc->processingType() == KWDocument::WP ? i18n( "Page" ) : i18n( "Page..." ), "page", 0,
                                    this, SLOT( insertPage() ),
                                    actionCollection(), "insert_page" );

    m_actionInsertLink = new KAction( i18n( "Link..." ), 0,
                                    this, SLOT( insertLink() ),
                                    actionCollection(), "insert_link" );
    m_actionInsertLink->setToolTip( i18n( "Insert a Web address, email address or hyperlink to a file" ) );
    m_actionInsertLink->setWhatsThis( i18n( "Insert a Web address, email address or hyperlink to a file." ) );

    m_actionInsertComment = new KAction( i18n( "Comment..." ), 0,
                                    this, SLOT( insertComment() ),
                                    actionCollection(), "insert_comment" );
    m_actionInsertComment->setToolTip( i18n( "Insert a comment about the selected text" ) );
    m_actionInsertComment->setWhatsThis( i18n( "Insert a comment about the selected text. These comments are not designed to appear on the final page." ) );

    m_actionEditComment = new KAction( i18n("Edit Comment..."), 0,
                                  this,SLOT(editComment()),
                                  actionCollection(), "edit_comment");
    m_actionEditComment->setToolTip( i18n( "Change the content of a comment" ) );
    m_actionEditComment->setWhatsThis( i18n( "Change the content of a comment." ) );

    m_actionRemoveComment = new KAction( i18n("Remove Comment"), 0,
                                     this,SLOT(removeComment()),
                                     actionCollection(), "remove_comment");
    m_actionRemoveComment->setToolTip( i18n( "Remove the selected document comment" ) );
    m_actionRemoveComment->setWhatsThis( i18n( "Remove the selected document comment." ) );
    m_actionCopyTextOfComment = new KAction( i18n("Copy Text of Comment..."), 0,
                                  this,SLOT(copyTextOfComment()),
                                  actionCollection(), "copy_text_comment");


    m_actionInsertFootEndNote = new KAction( i18n( "&Footnote/Endnote..." ), 0,
                                           this, SLOT( insertFootNote() ),
                                           actionCollection(), "insert_footendnote" );
    m_actionInsertFootEndNote->setToolTip( i18n( "Insert a footnote referencing the selected text" ) );
    m_actionInsertFootEndNote->setWhatsThis( i18n( "Insert a footnote referencing the selected text." ) );

    m_actionInsertContents = new KAction( i18n( "Table of &Contents" ), 0,
                                        this, SLOT( insertContents() ),
                                        actionCollection(), "insert_contents" );
    m_actionInsertContents->setToolTip( i18n( "Insert table of contents at the current cursor position" ) );
    m_actionInsertContents->setWhatsThis( i18n( "Insert table of contents at the current cursor position." ) );

    m_variableDefMap.clear();
    actionInsertVariable = new KActionMenu( i18n( "&Variable" ),
                                            actionCollection(), "insert_variable" );

    // The last argument is only needed if a submenu is to be created
    addVariableActions( VT_FIELD, KoFieldVariable::actionTexts(), actionInsertVariable, i18n("Document &Information") );
    addVariableActions( VT_DATE, KoDateVariable::actionTexts(), actionInsertVariable, i18n("&Date") );
    addVariableActions( VT_TIME, KoTimeVariable::actionTexts(), actionInsertVariable, i18n("&Time") );
    addVariableActions( VT_PGNUM, KoPageVariable::actionTexts(), actionInsertVariable, i18n("&Page") );
    addVariableActions( VT_STATISTIC, KWStatisticVariable::actionTexts(), actionInsertVariable, i18n("&Statistic") );

    m_actionInsertCustom = new KActionMenu( i18n( "&Custom" ),
                                            actionCollection(), "insert_custom" );
    actionInsertVariable->insert(m_actionInsertCustom);

    //addVariableActions( VT_CUSTOM, KWCustomVariable::actionTexts(), actionInsertVariable, QString::null );

    addVariableActions( VT_MAILMERGE, KoMailMergeVariable::actionTexts(), actionInsertVariable, QString::null );

    actionInsertVariable->popupMenu()->insertSeparator();
    m_actionRefreshAllVariable = new KAction( i18n( "&Refresh All Variables" ), 0,
                                    this, SLOT( refreshAllVariable() ),
                                    actionCollection(), "refresh_all_variable" );
    m_actionRefreshAllVariable->setToolTip( i18n( "Update all variables to current values" ) );
    m_actionRefreshAllVariable->setWhatsThis( i18n( "Update all variables in the document to current values.<br><br>This will update page numbers, dates or any other variables that need updating." ) );

    actionInsertVariable->insert(m_actionRefreshAllVariable);

    m_actionInsertExpression = new KActionMenu( i18n( "&Expression" ),
                                            actionCollection(), "insert_expression" );
    loadexpressionActions( m_actionInsertExpression);

    m_actionToolsCreateText = new KToggleAction( i18n( "Te&xt Frame" ), "frame_text", Qt::Key_F10 /*same as kpr*/,
                                               this, SLOT( toolsCreateText() ),
                                               actionCollection(), "tools_createtext" );
    m_actionToolsCreateText->setToolTip( i18n( "Create a new text frame" ) );
    m_actionToolsCreateText->setWhatsThis( i18n( "Create a new text frame." ) );

    m_actionToolsCreateText->setExclusiveGroup( "tools" );
    m_actionInsertFormula = new KAction( i18n( "For&mula" ), "frame_formula", Qt::Key_F4,
                                       this, SLOT( insertFormula() ),
                                       actionCollection(), "tools_formula" );
    m_actionInsertFormula->setToolTip( i18n( "Insert a formula into a new frame" ) );
    m_actionInsertFormula->setWhatsThis( i18n( "Insert a formula into a new frame." ) );

    m_actionInsertTable = new KAction( i18n( "&Table..." ), "inline_table",
                        Qt::Key_F5,
                        this, SLOT( insertTable() ),
                        actionCollection(), "insert_table" );
    m_actionInsertTable->setToolTip( i18n( "Create a table" ) );
    m_actionInsertTable->setWhatsThis( i18n( "Create a table.<br><br>The table can either exist in a frame of its own or inline." ) );

    m_actionToolsCreatePix = new KToggleAction( i18n( "P&icture..." ), "frame_image", // or inline_image ?
                                              Qt::SHIFT + Qt::Key_F5 /*same as kpr*/,
                                              this, SLOT( insertPicture() ),
                                              actionCollection(), "insert_picture" );
    m_actionToolsCreatePix->setToolTip( i18n( "Create a new frame for a picture" ) );
    m_actionToolsCreatePix->setWhatsThis( i18n( "Create a new frame for a picture or diagram." ) );
    m_actionToolsCreatePix->setExclusiveGroup( "tools" );

    m_actionToolsCreatePart = new KoPartSelectAction( i18n( "&Object Frame" ), "frame_query",
                                                    this, SLOT( toolsPart() ),
                                                    actionCollection(), "tools_part" );
    m_actionToolsCreatePart->setToolTip( i18n( "Insert an object into a new frame" ) );
    m_actionToolsCreatePart->setWhatsThis( i18n( "Insert an object into a new frame." ) );

    m_actionInsertFile = new KAction( i18n( "Fi&le..." ), 0,
                                   this, SLOT( insertFile() ),
                                   actionCollection(), "insert_file" );


    // ------------------------- Format menu
    m_actionFormatFont = new KAction( i18n( "&Font..." ), Qt::ALT + Qt::CTRL + Qt::Key_F,
                                    this, SLOT( formatFont() ),
                                    actionCollection(), "format_font" );
    m_actionFormatFont->setToolTip( i18n( "Change character size, font, boldface, italics etc." ) );
    m_actionFormatFont->setWhatsThis( i18n( "Change the attributes of the currently selected characters." ) );

    m_actionFormatParag = new KAction( i18n( "&Paragraph..." ), Qt::ALT + Qt::CTRL + Qt::Key_P,
                                     this, SLOT( formatParagraph() ),
                                     actionCollection(), "format_paragraph" );
    m_actionFormatParag->setToolTip( i18n( "Change paragraph margins, text flow, borders, bullets, numbering etc." ) );
    m_actionFormatParag->setWhatsThis( i18n( "Change paragraph margins, text flow, borders, bullets, numbering etc.<p>Select text in multiple paragraphs to change the formatting of all selected paragraphs.<p>If no text is selected, the paragraph where the cursor is located will be changed." ) );

    m_actionFormatFrameSet = new KAction( i18n( "F&rame/Frameset Properties" ), 0,
                                     this, SLOT( formatFrameSet() ),
                                     actionCollection(), "format_frameset" );
    m_actionFormatFrameSet->setToolTip( i18n( "Alter frameset properties" ) );
    m_actionFormatFrameSet->setWhatsThis( i18n( "Alter frameset properties.<p>Currently you can change the frame background." ) );

    m_actionFormatPage = new KAction( i18n( "Page &Layout..." ), 0,
                        this, SLOT( formatPage() ),
                        actionCollection(), "format_page" );
    m_actionFormatPage->setToolTip( i18n( "Change properties of entire page" ) );
    m_actionFormatPage->setWhatsThis( i18n( "Change properties of the entire page.<p>Currently you can change paper size, paper orientation, header and footer sizes, and column settings." ) );


    m_actionFormatFrameStylist = new KAction( i18n( "&Frame Style Manager" ), 0 /*shortcut?*/,
                                this, SLOT( extraFrameStylist() ),
                                actionCollection(), "frame_stylist" );
    m_actionFormatFrameStylist->setToolTip( i18n( "Change attributes of framestyles" ) );
    m_actionFormatFrameStylist->setWhatsThis( i18n( "Change background and borders of framestyles.<p>Multiple framestyles can be changed using the dialog box." ) );


    m_actionFormatStylist = new KAction( i18n( "&Style Manager" ), Qt::ALT + Qt::CTRL + Qt::Key_S,
                        this, SLOT( extraStylist() ),
                        actionCollection(), "format_stylist" );
    m_actionFormatStylist->setToolTip( i18n( "Change attributes of styles" ) );
    m_actionFormatStylist->setWhatsThis( i18n( "Change font and paragraph attributes of styles.<p>Multiple styles can be changed using the dialog box." ) );

    m_actionFormatFontSize = new KFontSizeAction( i18n( "Font Size" ), 0,
                                              actionCollection(), "format_fontsize" );
    connect( m_actionFormatFontSize, SIGNAL( fontSizeChanged( int ) ),
             this, SLOT( textSizeSelected( int ) ) );

    m_actionFontSizeIncrease = new KAction( i18n("Increase Font Size"), "fontsizeup", Qt::CTRL + Qt::Key_Greater, this, SLOT( increaseFontSize() ), actionCollection(), "increase_fontsize" );
    m_actionFontSizeDecrease = new KAction( i18n("Decrease Font Size"), "fontsizedown", Qt::CTRL + Qt::Key_Less, this, SLOT( decreaseFontSize() ), actionCollection(), "decrease_fontsize" );

#ifdef KFONTACTION_HAS_CRITERIA_ARG
    m_actionFormatFontFamily = new KFontAction( KFontChooser::SmoothScalableFonts,
                                              i18n( "Font Family" ), 0,
                                              actionCollection(), "format_fontfamily" );
#else
    m_actionFormatFontFamily = new KFontAction( i18n( "Font Family" ), 0,
                                              actionCollection(), "format_fontfamily" );
#endif
    connect( m_actionFormatFontFamily, SIGNAL( activated( const QString & ) ),
             this, SLOT( textFontSelected( const QString & ) ) );

    m_actionFormatStyleMenu = new KActionMenu( i18n( "St&yle" ), 0,
                                           actionCollection(), "format_stylemenu" );
    m_actionFormatStyle = new KSelectAction( i18n( "St&yle" ), 0,
                                           actionCollection(), "format_style" );
    // In fact, binding a key to this action will simply re-apply the current style. Why not.
    //m_actionFormatStyle->setShortcutConfigurable( false );
    connect( m_actionFormatStyle, SIGNAL( activated( int ) ),
             this, SLOT( textStyleSelected( int ) ) );
    updateStyleList();

    m_actionFormatDefault=new KAction( i18n( "Default Format" ), 0,
                                          this, SLOT( textDefaultFormat() ),
                                          actionCollection(), "text_default" );
    m_actionFormatDefault->setToolTip( i18n( "Change font and paragraph attributes to their default values" ) );
    m_actionFormatDefault->setWhatsThis( i18n( "Change font and paragraph attributes to their default values." ) );

    // ----------------------- More format actions, for the toolbar only

    m_actionFormatBold = new KToggleAction( i18n( "&Bold" ), "text_bold", Qt::CTRL + Qt::Key_B,
                                           this, SLOT( textBold() ),
                                           actionCollection(), "format_bold" );
    m_actionFormatItalic = new KToggleAction( i18n( "&Italic" ), "text_italic", Qt::CTRL + Qt::Key_I,
                                           this, SLOT( textItalic() ),
                                           actionCollection(), "format_italic" );
    m_actionFormatUnderline = new KToggleAction( i18n( "&Underline" ), "text_under", Qt::CTRL + Qt::Key_U,
                                           this, SLOT( textUnderline() ),
                                           actionCollection(), "format_underline" );
    m_actionFormatStrikeOut = new KToggleAction( i18n( "&Strike Out" ), "text_strike", 0 ,
                                           this, SLOT( textStrikeOut() ),
                                           actionCollection(), "format_strike" );

    m_actionFormatAlignLeft = new KToggleAction( i18n( "Align &Left" ), "text_left", Qt::CTRL + Qt::Key_L,
                                       this, SLOT( textAlignLeft() ),
                                       actionCollection(), "format_alignleft" );
    m_actionFormatAlignLeft->setExclusiveGroup( "align" );
    m_actionFormatAlignLeft->setChecked( TRUE );
    m_actionFormatAlignCenter = new KToggleAction( i18n( "Align &Center" ), "text_center", Qt::CTRL + Qt::ALT + Qt::Key_C,
                                         this, SLOT( textAlignCenter() ),
                                         actionCollection(), "format_aligncenter" );
    m_actionFormatAlignCenter->setExclusiveGroup( "align" );
    m_actionFormatAlignRight = new KToggleAction( i18n( "Align &Right" ), "text_right", Qt::CTRL + Qt::ALT + Qt::Key_R,
                                        this, SLOT( textAlignRight() ),
                                        actionCollection(), "format_alignright" );
    m_actionFormatAlignRight->setExclusiveGroup( "align" );
    m_actionFormatAlignBlock = new KToggleAction( i18n( "Align &Block" ), "text_block", Qt::CTRL + Qt::Key_J,
                                        this, SLOT( textAlignBlock() ),
                                        actionCollection(), "format_alignblock" );
    m_actionFormatAlignBlock->setExclusiveGroup( "align" );

    m_actionFormatSpacingSingle = new KToggleAction( i18n( "Line Spacing &1" ), "spacesimple", Qt::CTRL + Qt::Key_1,
                                           this, SLOT( textSpacingSingle() ),
                                           actionCollection(), "format_spacingsingle" );
    m_actionFormatSpacingSingle->setExclusiveGroup( "spacing" );
    m_actionFormatSpacingOneAndHalf = new KToggleAction( i18n( "Line Spacing 1.&5" ), "spacedouble", Qt::CTRL + Qt::Key_5,
                                       this, SLOT( textSpacingOneAndHalf() ),
                                       actionCollection(), "format_spacing15" );
    m_actionFormatSpacingOneAndHalf->setExclusiveGroup( "spacing" );
    m_actionFormatSpacingDouble = new KToggleAction( i18n( "Line Spacing &2" ), "spacetriple", Qt::CTRL + Qt::Key_2,
                                           this, SLOT( textSpacingDouble() ),
                                           actionCollection(), "format_spacingdouble" );
    m_actionFormatSpacingDouble->setExclusiveGroup( "spacing" );

    m_actionFormatSuper = new KToggleAction( i18n( "Superscript" ), "super", 0,
                                              this, SLOT( textSuperScript() ),
                                              actionCollection(), "format_super" );
    //m_actionFormatSuper->setExclusiveGroup( "valign" );
    m_actionFormatSub = new KToggleAction( i18n( "Subscript" ), "sub", 0,
                                              this, SLOT( textSubScript() ),
                                              actionCollection(), "format_sub" );
    //m_actionFormatSub->setExclusiveGroup( "valign" );

    m_actionFormatIncreaseIndent= new KAction( i18n( "Increase Indent" ),
            QApplication::isRightToLeft() ? "format_decreaseindent" : "format_increaseindent", 0,
                                             this, SLOT( textIncreaseIndent() ),
                                             actionCollection(), "format_increaseindent" );

    m_actionFormatDecreaseIndent= new KAction( i18n( "Decrease Indent" ),
                                             QApplication::isRightToLeft() ? "format_increaseindent" :"format_decreaseindent", 0,
                                             this, SLOT( textDecreaseIndent() ),
                                             actionCollection(), "format_decreaseindent" );

    m_actionFormatColor = new TKSelectColorAction( i18n( "Text Color..." ), TKSelectColorAction::TextColor,
                                     this, SLOT( textColor() ),
                                     actionCollection(), "format_color", true );
    m_actionFormatColor->setDefaultColor(QColor());


    //actionFormatList = new KToggleAction( i18n( "List" ), "enumList", 0,
    //                                          this, SLOT( textList() ),
    //                                          actionCollection(), "format_list" );
    //actionFormatList->setExclusiveGroup( "style" );

    m_actionFormatNumber = new KActionMenu( i18n( "Number" ),
                                          "enumList", actionCollection(), "format_number" );
    m_actionFormatNumber->setDelayed( false );
    m_actionFormatBullet = new KActionMenu( i18n( "Bullet" ),
                                          "unsortedList", actionCollection(), "format_bullet" );
    m_actionFormatBullet->setDelayed( false );
    Q3PtrList<KoCounterStyleWidget::StyleRepresenter> stylesList;
    KoCounterStyleWidget::makeCounterRepresenterList( stylesList );
    Q3PtrListIterator<KoCounterStyleWidget::StyleRepresenter> styleIt( stylesList );
    for ( ; styleIt.current() ; ++styleIt ) {
        // Dynamically create toggle-actions for each list style.
        // This approach allows to edit toolbars and extract separate actions from this menu
        KToggleAction* act = new KToggleAction( styleIt.current()->name(), /*TODO icon,*/
                                                0, this, SLOT( slotCounterStyleSelected() ),
                                                actionCollection(), QString("counterstyle_%1").arg( styleIt.current()->style() ).latin1() );
        act->setExclusiveGroup( "counterstyle" );
        // Add to the right menu: both for "none", bullet for bullets, numbers otherwise
        if ( styleIt.current()->style() == KoParagCounter::STYLE_NONE ) {
            m_actionFormatBullet->insert( act );
            m_actionFormatNumber->insert( act );
        } else if ( styleIt.current()->isBullet() )
            m_actionFormatBullet->insert( act );
        else
            m_actionFormatNumber->insert( act );
    }

    // ---------------------------- frame toolbar actions

    m_actionFrameStyleMenu = new KActionMenu( i18n( "Fra&mestyle" ), 0,
                                           actionCollection(), "frame_stylemenu" );
    m_actionFrameStyle = new KSelectAction( i18n( "Framest&yle" ), 0,
                                           actionCollection(), "frame_style" );
    connect( m_actionFrameStyle, SIGNAL( activated( int ) ),
             this, SLOT( frameStyleSelected( int ) ) );
    updateFrameStyleList();
    m_actionBorderOutline = new KToggleAction( i18n( "Border Outline" ), "borderoutline",
                            0, this, SLOT( borderOutline() ), actionCollection(), "border_outline" );
    m_actionBorderLeft = new KToggleAction( i18n( "Border Left" ), "borderleft",
                            0, this, SLOT( borderLeft() ), actionCollection(), "border_left" );
    m_actionBorderRight = new KToggleAction( i18n( "Border Right" ), "borderright",
                            0, this, SLOT( borderRight() ), actionCollection(), "border_right" );
    m_actionBorderTop = new KToggleAction( i18n( "Border Top" ), "bordertop",
                            0, this, SLOT( borderTop() ), actionCollection(), "border_top" );
    m_actionBorderBottom = new KToggleAction( i18n( "Border Bottom" ), "borderbottom",
                            0, this, SLOT( borderBottom() ),  actionCollection(), "border_bottom" );
    m_actionBorderStyle = new KSelectAction( i18n( "Border Style" ),
                            0,  actionCollection(), "border_style" );

    QStringList lst;
    lst << KoBorder::getStyle( KoBorder::SOLID );
    lst << KoBorder::getStyle( KoBorder::DASH );
    lst << KoBorder::getStyle( KoBorder::DOT );
    lst << KoBorder::getStyle( KoBorder::DASH_DOT );
    lst << KoBorder::getStyle( KoBorder::DASH_DOT_DOT );
    lst << KoBorder::getStyle( KoBorder::DOUBLE_LINE );
    m_actionBorderStyle->setItems( lst );
    m_actionBorderWidth = new KSelectAction( i18n( "Border Width" ), 0,
                                                 actionCollection(), "border_width" );
    lst.clear();
    for ( unsigned int i = 1; i < 10; i++ )
        lst << QString::number( i );
    m_actionBorderWidth->setItems( lst );
    m_actionBorderWidth->setCurrentItem( 0 );

    m_actionBorderColor = new TKSelectColorAction( i18n("Border Color"), TKSelectColorAction::LineColor, actionCollection(), "border_color", true );
    m_actionBorderColor->setDefaultColor(QColor());


    m_actionBackgroundColor = new TKSelectColorAction( i18n( "Text Background Color..." ), TKSelectColorAction::FillColor, actionCollection(),"border_backgroundcolor", true);
    m_actionBackgroundColor->setToolTip( i18n( "Change background color for currently selected text" ) );
    m_actionBackgroundColor->setWhatsThis( i18n( "Change background color for currently selected text." ) );

    connect(m_actionBackgroundColor,SIGNAL(activated()),SLOT(backgroundColor() ));
    m_actionBackgroundColor->setDefaultColor(QColor());

    // ---------------------- Table menu
    m_actionTablePropertiesMenu = new KAction( i18n( "&Properties" ), 0,
                               this, SLOT( tableProperties() ),
                               actionCollection(), "table_propertiesmenu" );
    m_actionTablePropertiesMenu->setToolTip( i18n( "Adjust properties of the current table" ) );
    m_actionTablePropertiesMenu->setWhatsThis( i18n( "Adjust properties of the current table." ) );

    m_actionTableInsertRow = new KAction( i18n( "&Insert Row..." ), "insert_table_row", 0,
                               this, SLOT( tableInsertRow() ),
                               actionCollection(), "table_insrow" );
    m_actionTableInsertRow->setToolTip( i18n( "Insert one or more rows at cursor location" ) );
    m_actionTableInsertRow->setWhatsThis( i18n( "Insert one or more rows at current cursor location." ) );

    m_actionTableInsertCol = new KAction( i18n( "I&nsert Column..." ), "insert_table_col", 0,
                               this, SLOT( tableInsertCol() ),
                               actionCollection(), "table_inscol" );
    m_actionTableInsertCol->setToolTip( i18n( "Insert one or more columns into the current table" ) );
    m_actionTableInsertCol->setWhatsThis( i18n( "Insert one or more columns into the current table." ) );

    m_actionTableDelRow = new KAction( 0, "delete_table_row", 0,
                                     this, SLOT( tableDeleteRow() ),
                                     actionCollection(), "table_delrow" );
    m_actionTableDelRow->setToolTip( i18n( "Delete selected rows from the current table" ) );
    m_actionTableDelRow->setWhatsThis( i18n( "Delete selected rows from the current table." ) );

    m_actionTableDelCol = new KAction( 0, "delete_table_col", 0,
                                     this, SLOT( tableDeleteCol() ),
                                     actionCollection(), "table_delcol" );
    m_actionTableDelCol->setToolTip( i18n( "Delete selected columns from the current table" ) );
    m_actionTableDelCol->setWhatsThis( i18n( "Delete selected columns from the current table." ) );

    m_actionTableResizeCol = new KAction( i18n( "Resize Column..." ), 0,
                               this, SLOT( tableResizeCol() ),
                               actionCollection(), "table_resizecol" );
    m_actionTableResizeCol->setToolTip( i18n( "Change the width of the currently selected column" ) );
    m_actionTableResizeCol->setWhatsThis( i18n( "Change the width of the currently selected column." ) );


    m_actionTableJoinCells = new KAction( i18n( "&Join Cells" ), 0,
                                        this, SLOT( tableJoinCells() ),
                                        actionCollection(), "table_joincells" );
    m_actionTableJoinCells->setToolTip( i18n( "Join two or more cells into one large cell" ) );
    m_actionTableJoinCells->setWhatsThis( i18n( "Join two or more cells into one large cell.<p>This is a good way to create titles and labels within a table." ) );

    m_actionTableSplitCells= new KAction( i18n( "&Split Cell..." ), 0,
                                         this, SLOT( tableSplitCells() ),
                                         actionCollection(), "table_splitcells" );
    m_actionTableSplitCells->setToolTip( i18n( "Split one cell into two or more cells" ) );
    m_actionTableSplitCells->setWhatsThis( i18n( "Split one cell into two or more cells.<p>Cells can be split horizontally, vertically or both directions at once." ) );

    m_actionTableProtectCells= new KToggleAction( i18n( "Protect Cells" ), 0, 0, 0,
                                         actionCollection(), "table_protectcells" );
    m_actionTableProtectCells->setToolTip( i18n( "Prevent changes to content of selected cells" ) );
    connect (m_actionTableProtectCells, SIGNAL( toggled(bool) ), this,
            SLOT( tableProtectCells(bool) ));

    m_actionTableProtectCells->setWhatsThis( i18n( "Toggles cell protection on and off.<br><br>When cell protection is on, the user can not alter the content or formatting of the text within the cell." ) );

    m_actionTableUngroup = new KAction( i18n( "&Ungroup Table" ), 0,
                                      this, SLOT( tableUngroupTable() ),
                                      actionCollection(), "table_ungroup" );
    m_actionTableUngroup->setToolTip( i18n( "Break a table into individual frames" ) );
    m_actionTableUngroup->setWhatsThis( i18n( "Break a table into individual frames<p>Each frame can be moved independently around the page." ) );

    m_actionTableDelete = new KAction( i18n( "Delete &Table" ), 0,
                                     this, SLOT( tableDelete() ),
                                     actionCollection(), "table_delete" );
    m_actionTableDelete->setToolTip( i18n( "Delete the entire table" ) );
    m_actionTableDelete->setWhatsThis( i18n( "Deletes all cells and the content within the cells of the currently selected table." ) );


    m_actionTableStylist = new KAction( i18n( "T&able Style Manager" ), 0,
                        this, SLOT( tableStylist() ),
                        actionCollection(), "table_stylist" );
    m_actionTableStylist->setToolTip( i18n( "Change attributes of tablestyles" ) );
    m_actionTableStylist->setWhatsThis( i18n( "Change textstyle and framestyle of the tablestyles.<p>Multiple tablestyles can be changed using the dialog box." ) );

    m_actionTableStyleMenu = new KActionMenu( i18n( "Table&style" ), 0,
                                           actionCollection(), "table_stylemenu" );
    m_actionTableStyle = new KSelectAction( i18n( "Table&style" ), 0,
                                           actionCollection(), "table_style" );
    connect( m_actionTableStyle, SIGNAL( activated( int ) ),
             this, SLOT( tableStyleSelected( int ) ) );
    updateTableStyleList();

    m_actionConvertTableToText = new KAction( i18n( "Convert Table to Text" ), 0,
                                            this, SLOT( convertTableToText() ),
                                            actionCollection(), "convert_table_to_text" );
    m_actionSortText= new KAction( i18n( "Sort Text..." ), 0,
                                 this, SLOT( sortText() ),
                                 actionCollection(), "sort_text" );

    m_actionAddPersonalExpression= new KAction( i18n( "Add Expression" ), 0,
                                              this, SLOT( addPersonalExpression() ),
                                              actionCollection(), "add_personal_expression" );


    // ---------------------- Tools menu


    m_actionAllowAutoFormat = new KToggleAction( i18n( "Enable Autocorrection" ), 0,
                                             this, SLOT( slotAllowAutoFormat() ),
                                          actionCollection(), "enable_autocorrection" );
    m_actionAllowAutoFormat->setCheckedState(i18n("Disable Autocorrection"));
    m_actionAllowAutoFormat->setToolTip( i18n( "Toggle autocorrection on and off" ) );
    m_actionAllowAutoFormat->setWhatsThis( i18n( "Toggle autocorrection on and off." ) );

    m_actionAutoFormat = new KAction( i18n( "Configure &Autocorrection..." ), 0,
                        this, SLOT( extraAutoFormat() ),
                        actionCollection(), "configure_autocorrection" );
    m_actionAutoFormat->setToolTip( i18n( "Change autocorrection options" ) );
    m_actionAutoFormat->setWhatsThis( i18n( "Change autocorrection options including:<p> <UL><LI><P>exceptions to autocorrection</P> <LI><P>add/remove autocorrection replacement text</P> <LI><P>and basic autocorrection options</P>." ) );

    m_actionEditCustomVarsEdit = new KAction( i18n( "Custom &Variables..." ), 0,
                                        this, SLOT( editCustomVars() ), // TODO: new dialog w add etc.
                                        actionCollection(), "custom_vars" );

    m_actionEditPersonnalExpr=new KAction( i18n( "Edit &Personal Expressions..." ), 0,
                                         this, SLOT( editPersonalExpr() ),
                                     actionCollection(), "personal_expr" );
    m_actionEditPersonnalExpr->setToolTip( i18n( "Add or change one or more personal expressions" ) );
    m_actionEditPersonnalExpr->setWhatsThis( i18n( "Add or change one or more personal expressions.<p>Personal expressions are a way to quickly insert commonly used phrases or text into your document." ) );

    m_actionChangeCase=new KAction( i18n( "Change Case..." ), 0,
                                     this, SLOT( changeCaseOfText() ),
                                     actionCollection(), "change_case" );
    m_actionChangeCase->setToolTip( i18n( "Alter the capitalization of selected text" ) );
    m_actionChangeCase->setWhatsThis( i18n( "Alter the capitalization of selected text to one of five pre-defined patterns.<p>You can also switch all letters from upper case to lower case and from lower case to upper case in one move." ) );

    //------------------------ Settings menu
    m_actionConfigure = KStdAction::preferences(this, SLOT(configure()), actionCollection(), "configure" );

    //------------------------ Menu frameSet
    KAction *actionChangePicture=new KAction( i18n( "Change Picture..." ),"frame_image",0,
                                     this, SLOT( changePicture() ),
                                     actionCollection(), "change_picture" );
    actionChangePicture->setToolTip( i18n( "Change the picture in the currently selected frame" ) );
    actionChangePicture->setWhatsThis( i18n( "You can specify a different picture in the current frame.<br><br>KWord automatically resizes the new picture to fit within the old frame." ) );

    m_actionConfigureHeaderFooter=new KAction( i18n( "Configure Header/Footer..." ), 0,
                                     this, SLOT( configureHeaderFooter() ),
                                     actionCollection(), "configure_headerfooter" );
    m_actionConfigureHeaderFooter->setToolTip( i18n( "Configure the currently selected header or footer" ) );
    m_actionConfigureHeaderFooter->setWhatsThis( i18n( "Configure the currently selected header or footer." ) );

    m_actionInlineFrame = new KToggleAction( i18n( "Inline Frame" ), 0,
                                            this, SLOT( inlineFrame() ),
                                            actionCollection(), "inline_frame" );
    m_actionInlineFrame->setToolTip( i18n( "Convert current frame to an inline frame" ) );
    m_actionInlineFrame->setWhatsThis( i18n( "Convert the current frame to an inline frame.<br><br>Place the inline frame within the text at the point nearest to the frames current position." ) );

    m_actionOpenLink = new KAction( i18n( "Open Link" ), 0,
                                     this, SLOT( openLink() ),
                                     actionCollection(), "open_link" );
    m_actionOpenLink->setToolTip( i18n( "Open the link with the appropriate application" ) );
    m_actionOpenLink->setWhatsThis( i18n( "Open the link with the appropriate application.<br><br>Web addresses are opened in a browser.<br>Email addresses begin a new message addressed to the link.<br>File links are opened by the appropriate viewer or editor." ) );

    m_actionChangeLink=new KAction( i18n("Change Link..."), 0,
                                  this,SLOT(changeLink()),
                                  actionCollection(), "change_link");
    m_actionChangeLink->setToolTip( i18n( "Change the content of the currently selected link" ) );
    m_actionChangeLink->setWhatsThis( i18n( "Change the details of the currently selected link." ) );

    m_actionCopyLink = new KAction( i18n( "Copy Link" ), 0,
                                     this, SLOT( copyLink() ),
                                     actionCollection(), "copy_link" );

    m_actionAddLinkToBookmak = new KAction( i18n( "Add to Bookmark" ), 0,
                                     this, SLOT( addToBookmark() ),
                                     actionCollection(), "add_to_bookmark" );

    m_actionRemoveLink = new KAction( i18n( "Remove Link" ), 0,
                                     this, SLOT( removeLink() ),
                                     actionCollection(), "remove_link" );

    m_actionShowDocStruct = new KToggleAction( i18n( "Show Doc Structure" ), 0,
                                            this, SLOT( showDocStructure() ),
                                            actionCollection(), "show_docstruct" );
    m_actionShowDocStruct->setCheckedState(i18n("Hide Doc Structure"));
    m_actionShowDocStruct->setToolTip( i18n( "Open document structure sidebar" ) );
    m_actionShowDocStruct->setWhatsThis( i18n( "Open document structure sidebar.<p>This sidebar helps you organize your document and quickly find pictures, tables etc." ) );

    m_actionShowRuler = new KToggleAction( i18n( "Show Rulers" ), 0,
            this, SLOT( showRuler() ),
            actionCollection(), "show_ruler" );
    m_actionShowRuler->setCheckedState(i18n("Hide Rulers"));
    m_actionShowRuler->setToolTip( i18n( "Shows or hides rulers" ) );
    m_actionShowRuler->setWhatsThis( i18n("The rulers are the white measuring spaces top and left of the "
                    "document. The rulers show the position and width of pages and of frames and can "
                    "be used to position tabulators among others.<p>Uncheck this to disable "
                    "the rulers from being displayed." ) );

    m_actionViewShowGrid = new KToggleAction( i18n( "Show Grid" ), 0,
                                            this, SLOT( viewGrid() ),
                                            actionCollection(), "view_grid" );
    m_actionViewShowGrid->setCheckedState(i18n("Hide Grid"));

    m_actionViewSnapToGrid= new KToggleAction( i18n( "Snap to Grid" ), 0,
                                             this, SLOT(viewSnapToGrid() ),
                                             actionCollection(), "view_snaptogrid" );

    m_actionConfigureCompletion = new KAction( i18n( "Configure C&ompletion..." ), 0,
                        this, SLOT( configureCompletion() ),
                        actionCollection(), "configure_completion" );
    m_actionConfigureCompletion->setToolTip( i18n( "Change the words and options for autocompletion" ) );
    m_actionConfigureCompletion->setWhatsThis( i18n( "Add words or change the options for autocompletion." ) );


    // ------------------- Actions with a key binding and no GUI item
    new KAction( i18n( "Insert Non-Breaking Space" ), Qt::CTRL+Qt::Key_Space,
                 this, SLOT( slotNonbreakingSpace() ), actionCollection(), "nonbreaking_space" );
    new KAction( i18n( "Insert Non-Breaking Hyphen" ), Qt::CTRL+Qt::SHIFT+Qt::Key_Minus,
                 this, SLOT( slotNonbreakingHyphen() ), actionCollection(), "nonbreaking_hyphen" );
    new KAction( i18n( "Insert Soft Hyphen" ), Qt::CTRL+Qt::Key_Minus,
                 this, SLOT( slotSoftHyphen() ), actionCollection(), "soft_hyphen" );
    new KAction( i18n( "Line Break" ), Qt::SHIFT+Qt::Key_Return,
                 this, SLOT( slotLineBreak() ), actionCollection(), "line_break" );

    new KAction( i18n( "Completion" ), KStdAccel::shortcut(KStdAccel::TextCompletion), this, SLOT( slotCompletion() ), actionCollection(), "completion" );

    new KAction( i18n( "Increase Numbering Level" ), Qt::ALT+Qt::Key_Right,
                 this, SLOT( slotIncreaseNumberingLevel() ), actionCollection(), "increase_numbering_level" );
    new KAction( i18n( "Decrease Numbering Level" ), Qt::ALT+Qt::Key_Left,
                 this, SLOT( slotDecreaseNumberingLevel() ), actionCollection(), "decrease_numbering_level" );


    // --------
    m_actionEditCustomVars = new KAction( i18n( "Edit Variable..." ), 0,
                                        this, SLOT( editCustomVariable() ),
                                        actionCollection(), "edit_customvars" );
    m_actionApplyAutoFormat= new KAction( i18n( "Apply Autocorrection" ), 0,
                                        this, SLOT( applyAutoFormat() ),
                                        actionCollection(), "apply_autoformat" );
    m_actionApplyAutoFormat->setToolTip( i18n( "Manually force KWord to scan the entire document and apply autocorrection" ) );
    m_actionApplyAutoFormat->setWhatsThis( i18n( "Manually force KWord to scan the entire document and apply autocorrection." ) );

    m_actionCreateStyleFromSelection = new KAction( i18n( "Create Style From Selection..." ), 0,
                                        this, SLOT( createStyleFromSelection()),
                                        actionCollection(), "create_style" );
    m_actionCreateStyleFromSelection->setToolTip( i18n( "Create a new style based on the currently selected text" ) );
    m_actionCreateStyleFromSelection->setWhatsThis( i18n( "Create a new style based on the currently selected text." ) ); // ## "on the current paragraph, taking the formatting from where the cursor is. Selecting text isn't even needed."

    m_actionConfigureFootEndNote = new KAction( i18n( "&Footnote..." ), 0,
                                        this, SLOT( configureFootEndNote()),
                                        actionCollection(), "format_footendnote" );
    m_actionConfigureFootEndNote->setToolTip( i18n( "Change the look of footnotes" ) );
    m_actionConfigureFootEndNote->setWhatsThis( i18n( "Change the look of footnotes." ) );

    m_actionEditFootEndNote= new KAction( i18n("Edit Footnote"), 0,
                                        this, SLOT( editFootEndNote()),
                                        actionCollection(), "edit_footendnote" );
    m_actionEditFootEndNote->setToolTip( i18n( "Change the content of the currently selected footnote" ) );
    m_actionEditFootEndNote->setWhatsThis( i18n( "Change the content of the currently selected footnote." ) );


    m_actionChangeFootNoteType = new KAction( i18n("Change Footnote/Endnote Parameter"), 0,
                                        this, SLOT( changeFootNoteType() ),
                                            actionCollection(), "change_footendtype");

    m_actionSavePicture= new KAction( i18n("Save Picture As..."), 0,
                                    this, SLOT( savePicture() ),
                                    actionCollection(), "save_picture");
    m_actionSavePicture->setToolTip( i18n( "Save the picture in a separate file" ) );
    m_actionSavePicture->setWhatsThis( i18n( "Save the picture in the currently selected frame in a separate file, outside the KWord document." ) );

    m_actionAllowBgSpellCheck = new KToggleAction( i18n( "Autospellcheck" ), 0,
                                            this, SLOT( autoSpellCheck() ),
                                            actionCollection(), "tool_auto_spellcheck" );


    m_actionGoToFootEndNote = new KAction( QString::null /*set dynamically*/, 0,
                                            this, SLOT( goToFootEndNote() ),
                                            actionCollection(), "goto_footendnote" );

    // Document Structure Area popup menu.
    m_actionDocStructEdit = new KAction( i18n( "Edit Text" ), 0,
                                         this, SLOT( docStructEdit() ),
                                         actionCollection(), "docstruct_edit" );
    m_actionDocStructSelect = new KAction( i18n( "Show" ), 0,
                                           this, SLOT( docStructSelect() ),
                                           actionCollection(), "docstruct_select" );
    m_actionDocStructDelete = new KAction( i18n( "Delete Frame" ), 0,
                                           this, SLOT( docStructDelete() ),
                                           actionCollection(), "docstruct_delete" );
    m_actionDocStructProperties = new KAction( i18n( "Properties" ), 0,
                                               this, SLOT( docStructProperties() ),
                                               actionCollection(), "docstruct_properties" );

    m_actionAddBookmark= new KAction( i18n( "&Bookmark..." ), 0,
                                            this, SLOT( addBookmark() ),
                                            actionCollection(), "add_bookmark" );
    m_actionSelectBookmark= new KAction( i18n( "Select &Bookmark..." ), 0,
                                            this, SLOT( selectBookmark() ),
                                            actionCollection(), "select_bookmark" );

    m_actionImportStyle= new KAction( i18n( "Import Styles..." ), 0,
                                            this, SLOT( importStyle() ),
                                            actionCollection(), "import_style" );

    m_actionCreateFrameStyle = new KAction( i18n( "&Create Framestyle From Frame..." ), 0,
                                        this, SLOT( createFrameStyle()),
                                        actionCollection(), "create_framestyle" );
    m_actionCreateFrameStyle->setToolTip( i18n( "Create a new style based on the currently selected frame" ) );
    m_actionCreateFrameStyle->setWhatsThis( i18n( "Create a new framestyle based on the currently selected frame." ) );

#if 0 // re-enable after fixing
    m_actionInsertDirectCursor = new KToggleAction( i18n( "Type Anywhere Cursor" ), 0,
                                                  this, SLOT( insertDirectCursor() ),
                                                  actionCollection(), "direct_cursor" );
#endif

    m_actionConvertToTextBox = new KAction( i18n( "Convert to Text Box" ), 0,
                                                  this, SLOT( convertToTextBox() ),
                                                  actionCollection(), "convert_to_text_box" );


    m_actionSpellIgnoreAll = new KAction( i18n( "Ignore All" ), 0,
                                        this, SLOT( slotAddIgnoreAllWord() ),
                                        actionCollection(), "ignore_all" );

    m_actionAddWordToPersonalDictionary=new KAction( i18n( "Add Word to Dictionary" ),0,
                                                   this, SLOT( addWordToDictionary() ),
                                                   actionCollection(), "add_word_to_dictionary" );

    m_actionEmbeddedStoreInternal=new KToggleAction( i18n( "Store Document Internally" ),0,
                                            this, SLOT( embeddedStoreInternal() ),
                                            actionCollection(), "embedded_store_internal" );

    m_actionGoToDocumentStructure=new KAction( i18n( "Go to Document Structure" ), KShortcut("Alt+1"),
                                             this, SLOT( goToDocumentStructure() ),
                                             actionCollection(), "goto_document_structure" );
    m_actionGoToDocument=new KAction( i18n( "Go to Document" ), KShortcut("Alt+2"),
                                    this, SLOT( goToDocument() ),
                                    actionCollection(), "goto_document" );

    // For RMB inside a cell, see KWFrameView::showPopup
    // This isn't a dynamic list; it's only plugged/unplugged depending on the context.
    // If you change the contents of that list, check ~KWView.
    m_tableActionList.append( new KActionSeparator(actionCollection()) );
    m_tableActionList.append( m_actionTableInsertRow );
    m_tableActionList.append( m_actionTableDelRow );
    m_tableActionList.append( m_actionTableInsertCol );
    m_tableActionList.append( m_actionTableDelCol );
}

void KWView::refreshMenuExpression()
{
    loadexpressionActions( m_actionInsertExpression);
}

void KWView::updateGridButton()
{
    m_actionViewShowGrid->setChecked( m_doc->showGrid() );
    m_actionViewSnapToGrid->setChecked ( m_doc->snapToGrid() );
}

void KWView::loadexpressionActions( KActionMenu * parentMenu)
{
    KActionPtrList lst = actionCollection()->actions("expression-action");
    Q3ValueList<KAction *> actions = lst;
    Q3ValueList<KAction *>::ConstIterator it = lst.begin();
    Q3ValueList<KAction *>::ConstIterator end = lst.end();
    // Delete all actions but keep their shortcuts in mind
    QMap<QString, KShortcut> personalShortCuts;
    for (; it != end; ++it )
    {
        personalShortCuts.insert( (*it)->text(), (*it)->shortcut() );
        delete *it;
    }

    parentMenu->popupMenu()->clear();
    QStringList path = m_doc->personalExpressionPath();
    QStringList files;
    for ( QStringList::Iterator it = path.begin(); it != path.end(); ++it )
    {
        QDir dir( *it );
        if ( dir.exists() )
        {
            QStringList tmp = dir.entryList("*.xml");
            for ( QStringList::Iterator it2 = tmp.begin(); it2 != tmp.end(); ++it2 )
                files.append( QString( (*it) + (*it2) ));
        }
    }

    //QStringList files = KWFactory::instance()->dirs()->findAllResources( "expression", "*.xml", TRUE );
    int i = 0;
    int nbFile = 0;
    for( QStringList::Iterator it = files.begin(); it != files.end(); ++it,nbFile++ )
        createExpressionActions( parentMenu,*it, i,(nbFile<(int)files.count()-1), personalShortCuts );
}

void KWView::createExpressionActions( KActionMenu * parentMenu,const QString& filename,int &i, bool insertSepar, const QMap<QString, KShortcut>& personalShortCut )
{
    QFile file( filename );
    if ( !file.exists() || !file.open( QIODevice::ReadOnly ) )
        return;

    QDomDocument doc;
    doc.setContent( &file );
    file.close();

    bool expressionExist =false;
    QDomNode n = doc.documentElement().firstChild();
    for( ; !n.isNull(); n = n.nextSibling() )
    {
        if ( n.isElement() )
        {
            QDomElement e = n.toElement();
            if ( e.tagName() == "Type" )
            {
                expressionExist =true;
                QString group = i18n( e.namedItem( "TypeName" ).toElement().text().utf8() );
                KActionMenu * subMenu = new KActionMenu( group, actionCollection() );
                parentMenu->insert( subMenu );

                QDomNode n2 = e.firstChild();
                for( ; !n2.isNull(); n2 = n2.nextSibling() )
                {

                    if ( n2.isElement() )
                    {
                        QDomElement e2 = n2.toElement();
                        if ( e2.tagName() == "Expression" )
                        {
                            QString text = i18n( e2.namedItem( "Text" ).toElement().text().utf8() );
                            KAction * act = new KAction( text, 0, this, SLOT( insertExpression() ),
                                                         actionCollection(),
                                                         QString("expression-action_%1").arg(i).latin1() );
                            if ( personalShortCut.contains(text) )
                                act->setShortcut( personalShortCut[text] );
                            i++;
                            act->setGroup("expression-action");
                            subMenu->insert( act );
                        }
                    }
                }
            }
        }
    }
    if(expressionExist && insertSepar)
        parentMenu->popupMenu()->insertSeparator();
}

void KWView::insertExpression()
{
 KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KAction * act = (KAction *)(sender());
        edit->insertExpression(act->text());
    }
}


void KWView::addVariableActions( int type, const QStringList & texts,
                                 KActionMenu * parentMenu, const QString & menuText )
{
    // Single items go directly into parentMenu.
    // For multiple items we create a submenu.
    if ( texts.count() > 1 && !menuText.isEmpty() )
    {
        KActionMenu * subMenu = new KActionMenu( menuText, actionCollection() );
        parentMenu->insert( subMenu );
        parentMenu = subMenu;
    }
    QStringList::ConstIterator it = texts.begin();
    for ( int i = 0; it != texts.end() ; ++it, ++i )
    {
        if ( !(*it).isEmpty() ) // in case of removed subtypes or placeholders
        {
            VariableDef v;
            v.type = type;
            v.subtype = i;
            Q3CString actionName;
            actionName.sprintf( "var-action-%d-%d", type, i );
            KAction * act = new KAction( (*it), 0, this, SLOT( insertVariable() ),
                                         actionCollection(), actionName );
            // Mainly for KEditToolbar
            act->setToolTip( i18n( "Insert variable \"%1\" into the text" ).arg( *it ) );
            m_variableDefMap.insert( act, v );
            parentMenu->insert( act );
        }
    }
}

void KWView::refreshCustomMenu()
{
    KActionPtrList lst2 = actionCollection()->actions("custom-variable-action");
    Q3ValueList<KAction *> actions = lst2;
    Q3ValueList<KAction *>::ConstIterator it2 = lst2.begin();
    Q3ValueList<KAction *>::ConstIterator end = lst2.end();
    QMap<QString, KShortcut> shortCuts;

    for (; it2 != end; ++it2 )
    {
        shortCuts.insert((*it2)->text(), (*it2)->shortcut());
        delete *it2;
    }

    delete m_actionInsertCustom;
    m_actionInsertCustom = new KActionMenu( i18n( "&Custom" ),
                                            actionCollection(), "insert_custom" );
    actionInsertVariable->insert(m_actionInsertCustom, 0);

    m_actionInsertCustom->popupMenu()->clear();
    Q3PtrListIterator<KoVariable> it( m_doc->variableCollection()->getVariables() );
    KAction * act=0;
    QStringList lst;
    QString varName;
    int i = 0;
    for ( ; it.current() ; ++it )
    {
        KoVariable *var = it.current();
        if ( var->type() == VT_CUSTOM )
        {
            varName=( (KoCustomVariable*) var )->name();
            if ( !lst.contains( varName) )
            {
                 lst.append( varName );
                 Q3CString name = QString("custom-action_%1").arg(i).latin1();
                 act = new KAction( varName, shortCuts[varName], this, SLOT( insertCustomVariable() ),actionCollection(), name );
                 act->setGroup( "custom-variable-action" );
                 m_actionInsertCustom->insert( act );
                 i++;
            }
        }
    }
    bool state=!lst.isEmpty();
    if(state)
        m_actionInsertCustom->popupMenu()->insertSeparator();

    act = new KAction( i18n("New..."), 0, this, SLOT( insertNewCustomVariable() ), actionCollection(),QString("custom-action_%1").arg(i).latin1());
    act->setGroup( "custom-variable-action" );


    m_actionEditCustomVarsEdit->setEnabled( state );

    m_actionInsertCustom->insert( act );

}


void KWView::insertCustomVariable()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KAction * act = (KAction *)(sender());
        edit->insertCustomVariable(act->text());
    }
}

void KWView::insertNewCustomVariable()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->insertVariable( VT_CUSTOM, 0 );
}

void KWView::showFormulaToolbar( bool show )
{
    m_doc->formulaDocument()->setEnabled( show );
    m_doc->formulaDocumentWrapper()->enableMatrixActions( show );
    m_doc->formulaDocumentWrapper()->getSyntaxHighlightingAction()->setEnabled( true );
    if(shell())
      shell()->showToolbar( "formula_toolbar", show );
}

void KWView::updatePageInfo()
{
    if ( m_sbPageLabel )
    {
        KWFrameSetEdit * edit = m_gui->canvasWidget()->currentFrameSetEdit();
        if ( edit && edit->currentFrame() )
            m_currentPage = m_doc->pageManager()->page(edit->currentFrame());
        else {
            KWFrameView *view = frameViewManager()->selectedFrame();
            if(view)
                m_currentPage = m_doc->pageManager()->page(view->frame());
        }
        /*kDebug() << (void*)this << " KWView::updatePageInfo "
                  << " edit: " << edit << " " << ( edit?edit->frameSet()->name():QString::null)
                  << " currentFrame: " << (edit?edit->currentFrame():0L)
                  << " m_currentPage=" << currentPage() << " m_sbPageLabel=" << m_sbPageLabel
                  << endl;*/

        QString oldText = m_sbPageLabel->text();
        QString newText;
        if ( viewMode()->hasPages() )
            newText = ' ' + i18n( "Page %1 of %2" ).arg(m_currentPage->pageNumber())
                .arg(m_doc->pageCount()) + ' ';

        if ( newText != oldText )
        {
            m_sbPageLabel->setText( newText );
            // Need to repaint immediately. Otherwise when deleting 100 pages
            // at once, there's no feedback.
            m_sbPageLabel->repaint();
        }
    }
    slotUpdateRuler();
}

void KWView::numPagesChanged()
{
     docStructChanged(TextFrames);
     updatePageInfo();
     int pages = m_doc->pageCount();
     kDebug() <<  pages <<  " " << (m_doc->processingType() == KWDocument::DTP) << endl;
     refreshDeletePageAction();
}

void KWView::updateFrameStatusBarItem()
{
    KStatusBar * sb = statusBar();
    int nbFrame=frameViewManager()->selectedFrames().count();
    if ( m_doc->showStatusBar() && sb && nbFrame > 0 )
    {
        if ( nbFrame == 1 )
        {
            KoUnit::Unit unit = m_doc->unit();
            QString unitName = m_doc->unitName();
            KWFrame * frame = frameViewManager()->selectedFrames()[0]->frame();
            m_sbFramesLabel->setText( ' ' + i18n( "Statusbar info", "%1: %2, %3 - %4, %5 (width: %6, height: %7)" )
                    .arg( frame->frameSet()->name() )
                    .arg( KoUnit::toUserStringValue( frame->left(), unit ) )
                    .arg( KoUnit::toUserStringValue( frame->top() - m_doc->pageManager()->topOfPage(
                                m_doc->pageManager()->pageNumber(frame->rect()) ), unit) )
                    .arg( KoUnit::toUserStringValue( frame->right(), unit ) )
                    .arg( KoUnit::toUserStringValue( frame->bottom(), unit ) )
                    .arg( KoUnit::toUserStringValue( frame->width(), unit ) )
                    .arg( KoUnit::toUserStringValue( frame->height(), unit ) ) );
        } else
            m_sbFramesLabel->setText( ' ' + i18n( "%1 frames selected" ).arg( nbFrame ) );
    }
    else if ( sb && m_sbFramesLabel )
        m_sbFramesLabel->setText( QString::null );
}

void KWView::setTemporaryStatusBarText(const QString &text)
{
    if ( statusBar() && m_sbFramesLabel )
        m_sbFramesLabel->setText( text );
}

void KWView::clipboardDataChanged()
{
    if ( !m_gui || !m_doc->isReadWrite() )
    {
        m_actionEditPaste->setEnabled(false);
        return;
    }
    KWFrameSetEdit * edit = m_gui->canvasWidget()->currentFrameSetEdit();
    // Is there plain text in the clipboard ?
    if ( edit && !QApplication::clipboard()->text().isEmpty() )
    {
        m_actionEditPaste->setEnabled(true);
        return;
    }
    QMimeSource *data = QApplication::clipboard()->data();
    const int provides = checkClipboard( data );
    if ( provides & ( ProvidesImage | ProvidesOasis | ProvidesFormula ) )
        m_actionEditPaste->setEnabled( true );
    else
    {
        // Plain text requires a framesetedit
        m_actionEditPaste->setEnabled( edit && ( provides & ProvidesPlainText ) );
    }
}

int KWView::checkClipboard( QMimeSource *data )
{
    int provides = 0;
    Q3ValueList<Q3CString> formats;
    const char* fmt;
    for (int i=0; (fmt = data->format(i)); i++)
        formats.append( Q3CString( fmt ) );

#if 0 // not needed anymore
    // QImageDrag::canDecode( data ) is very very slow in Qt 2 (n*m roundtrips)
    // Workaround....
    Q3StrList fileFormats = QImageIO::inputFormats();
    for ( fileFormats.first() ; fileFormats.current() && !provides ; fileFormats.next() )
    {
        Q3CString format = fileFormats.current();
        Q3CString type = "image/" + format.lower();
        if ( ( formats.findIndex( type ) != -1 ) )
            provides |= ProvidesImage;
    }
#endif
    if ( Q3ImageDrag::canDecode( data ) )
        provides |= ProvidesImage;
    if ( formats.findIndex( KFormula::MimeSource::selectionMimeType() ) != -1 )
        provides |= ProvidesFormula;
    if ( formats.findIndex( "text/plain" ) != -1 )
        provides |= ProvidesPlainText;
    Q3CString returnedTypeMime = KoTextObject::providesOasis( data );
    if ( !returnedTypeMime.isEmpty() )
        provides |= ProvidesOasis;
    //kDebug(32001) << "KWView::checkClipboard provides=" << provides << endl;
    return provides;
}

/*=========================== file print =======================*/
void KWView::setupPrinter( KPrinter &prt )
{
    //recalc time and date variable before to print
    //it's necessary otherwise we print a document
    //with a bad date and time
    //TODO call once this function
    m_doc->recalcVariables(  VT_TIME );
    m_doc->recalcVariables(  VT_DATE );
    m_doc->recalcVariables(  VT_STATISTIC );

    prt.setPageSelection( KPrinter::ApplicationSide );
    prt.setCurrentPage( currentPage() );
    prt.setMinMax( m_doc->startPage(), m_doc->lastPage() );

    KoPageLayout pgLayout = m_doc->pageLayout();

    prt.setPageSize( static_cast<KPrinter::PageSize>( KoPageFormat::printerPageSize( pgLayout.format ) ) );

    if ( pgLayout.orientation == PG_LANDSCAPE || pgLayout.format == PG_SCREEN )
        prt.setOrientation( KPrinter::Landscape );
    else
        prt.setOrientation( KPrinter::Portrait );
}

void KWView::print( KPrinter &prt )
{
    bool displayFieldCode = m_doc->variableCollection()->variableSetting()->displayFieldCode();
    if ( displayFieldCode )
    {
        m_doc->variableCollection()->variableSetting()->setDisplayFieldCode(false);
        m_doc->recalcVariables(  VT_ALL );
    }

// Don't repaint behind the print dialog until we're done zooming/unzooming the doc
    m_gui->canvasWidget()->setUpdatesEnabled(false);
    m_gui->canvasWidget()->viewport()->setCursor( waitCursor );

    prt.setFullPage( true );

    // ### HACK: disable zooming-when-printing if embedded parts are used.
    // No koffice app supports zooming in paintContent currently.
    // Disable in ALL cases now
    bool doZoom = false;
    /*QPtrListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() && doZoom ; ++fit )
        if ( fit.current()->type() == FT_PART )
            doZoom = false;*/

    int oldZoom = m_doc->zoom();
    // We don't get valid metrics from the printer - and we want a better resolution
    // anyway (it's the PS driver that takes care of the printer resolution).
    Q3PaintDeviceMetrics metrics( &prt );

    //int dpiX = metrics.logicalDpiX();
    //int dpiY = metrics.logicalDpiY();
    int dpiX = doZoom ? 300 : KoGlobal::dpiX();
    int dpiY = doZoom ? 300 : KoGlobal::dpiY();
    ///////// Changing the dpiX/dpiY is very wrong nowadays. This has no effect on the font size
    ///////// that we give Qt, anymore, so it leads to minuscule fonts in the printout => doZoom==false.
    m_doc->setZoomAndResolution( 100, dpiX, dpiY );
    m_doc->newZoomAndResolution( false, true /* for printing*/ );
    //kDebug() << "KWView::print metrics: " << metrics.logicalDpiX() << "," << metrics.logicalDpiY() << endl;
    //kDebug() << "x11AppDPI: " << KoGlobal::dpiX() << "," << KoGlobal::dpiY() << endl;

    bool serialLetter = FALSE;

    Q3PtrList<KoVariable> vars = m_doc->variableCollection()->getVariables();
    KoVariable *v = 0;
    for ( v = vars.first(); v; v = vars.next() ) {
        if ( v->type() == VT_MAILMERGE ) {
            serialLetter = TRUE;
            break;
        }
    }

    if ( !m_doc->mailMergeDataBase() ) serialLetter=FALSE;
    else
    {
        m_doc->mailMergeDataBase()->refresh(false);
        if (m_doc->mailMergeDataBase()->getNumRecords() == 0 )  serialLetter = FALSE;
    }

    //float left_margin = 0.0;
    //float top_margin = 0.0;

    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter hf;
    m_doc->getPageLayout( pgLayout, cl, hf );
    KoPageLayout oldPGLayout = pgLayout;

    if ( pgLayout.format == PG_SCREEN )
    {
        //left_margin = 25.8;
        //top_margin = 15.0;
        pgLayout.ptLeft += 25.8;         // Not sure why we need this....
        pgLayout.ptRight += 15.0;
        m_doc->setPageLayout( pgLayout, cl, hf, false );
    }

    QPainter painter;
    painter.begin( &prt );

    kDebug(32001) << "KWView::print scaling by " << (double)metrics.logicalDpiX() / (double)dpiX
                   << "," << (double)metrics.logicalDpiY() / (double)dpiY << endl;
    painter.scale( (double)metrics.logicalDpiX() / (double)dpiX,
                   (double)metrics.logicalDpiY() / (double)dpiY );

    bool canceled = false;
    // Breaks wysiwyg, obviously - trying without
//#define KW_PASS_PAINTER_TO_QRT
#ifdef KW_PASS_PAINTER_TO_QRT
    int paragraphs = 0;
    fit.toFirst();
    for ( ; fit.current() ; ++fit )
        if ( fit.current()->isVisible() )
            paragraphs += fit.current()->paragraphs();
    kDebug() << "KWView::print total paragraphs: " << paragraphs << endl;

    // This can take a lot of time (reformatting everything), so a progress dialog is needed
    Q3ProgressDialog progress( i18n( "Printing..." ), i18n( "Cancel" ), paragraphs, this );
    progress.setProgress( 0 );
    int processedParags = 0;
    fit.toFirst();
    for ( ; fit.current() ; ++fit )
        if ( fit.current()->isVisible() )
        {
            qApp->processEvents();
            if ( progress.wasCanceled() ) {
                canceled = true;
                break;
            }

            kDebug() << "KWView::print preparePrinting " << fit.current()->name() << endl;
            fit.current()->preparePrinting( &painter, &progress, processedParags );
        }
#endif

    if ( !canceled )
    {
        if ( !serialLetter )
            m_gui->canvasWidget()->print( &painter, &prt );
        else
        {
            for ( int i = 0; i < m_doc->mailMergeDataBase()->getNumRecords(); ++i ) {
                m_doc->setMailMergeRecord( i );
                m_doc->variableCollection()->recalcVariables(VT_MAILMERGE);
                m_gui->canvasWidget()->print( &painter, &prt );
                if ( i < m_doc->mailMergeDataBase()->getNumRecords() - 1 )
                    prt.newPage();
            }
            m_doc->setMailMergeRecord( -1 );
        }
    }

    if ( pgLayout.format == PG_SCREEN )
        m_doc->setPageLayout( oldPGLayout, cl, hf, false );

#ifdef KW_PASS_PAINTER_TO_QRT
    fit.toFirst();
    for ( ; fit.current() ; ++fit )
        if ( fit.current()->isVisible() )
            fit.current()->preparePrinting( 0L, 0L, processedParags );
#endif

    m_doc->setZoomAndResolution( oldZoom, KoGlobal::dpiX(), KoGlobal::dpiY() );
    m_doc->newZoomAndResolution( false, false );
    kDebug() << "KWView::print zoom&res reset" << endl;

    m_gui->canvasWidget()->setUpdatesEnabled(true);
    m_gui->canvasWidget()->viewport()->setCursor( ibeamCursor );
    m_doc->repaintAllViews();

    if ( displayFieldCode )
    {
        m_doc->variableCollection()->variableSetting()->setDisplayFieldCode(true);
        m_doc->recalcVariables(  VT_ALL );
    }
    else
        m_doc->variableCollection()->recalcVariables(VT_MAILMERGE);

    painter.end(); // this is what triggers the printing
    m_doc->variableCollection()->variableSetting()->setLastPrintingDate(QDateTime::currentDateTime());
    m_doc->recalcVariables( VT_DATE );
}

void KWView::showFormat( const KoTextFormat &currentFormat )
{
    // update the gui with the current format.
    //kDebug() << "KWView::setFormat font family=" << currentFormat.font().family() << endl;
    m_actionFormatFontFamily->setFont( currentFormat.font().family() );
    m_actionFormatFontSize->setFontSize( currentFormat.pointSize() );
    m_actionFormatBold->setChecked( currentFormat.font().bold());
    m_actionFormatItalic->setChecked( currentFormat.font().italic() );
    m_actionFormatUnderline->setChecked( currentFormat.underline());
    m_actionFormatStrikeOut->setChecked( currentFormat.strikeOut());
    QColor col=currentFormat.textBackgroundColor();
    //m_actionBackgroundColor->setEnabled(true);
    m_actionBackgroundColor->setCurrentColor( col.isValid() ? col : QApplication::palette().color( QPalette::Active, QColorGroup::Base ));

    if ( m_gui /* if not in constructor */ && frameViewManager()->selectedFrames().count() > 0)
        m_actionBackgroundColor->setText(i18n("Frame Background Color..."));
    else
        m_actionBackgroundColor->setText(i18n("Text Background Color..."));
    switch(currentFormat.vAlign())
      {
      case KoTextFormat::AlignSuperScript:
        {
          m_actionFormatSub->setChecked( false );
          m_actionFormatSuper->setChecked( true );
          break;
        }
      case KoTextFormat::AlignSubScript:
        {
          m_actionFormatSub->setChecked( true );
          m_actionFormatSuper->setChecked( false );
          break;
        }
      case KoTextFormat::AlignNormal:
      default:
        {
          m_actionFormatSub->setChecked( false );
          m_actionFormatSuper->setChecked( false );
          break;
        }
      }

}

void KWView::showRulerIndent( double leftMargin, double firstLine, double rightMargin, bool rtl )
{
  KoRuler * hRuler = m_gui ? m_gui->getHorzRuler() : 0;
  if ( hRuler )
  {
      hRuler->setFirstIndent( KoUnit::toUserValue( firstLine, m_doc->unit() ) );
      hRuler->setLeftIndent( KoUnit::toUserValue( leftMargin, m_doc->unit() ) );
      hRuler->setRightIndent( KoUnit::toUserValue( rightMargin, m_doc->unit() ) );
      hRuler->setDirection( rtl );
      m_actionFormatDecreaseIndent->setEnabled( leftMargin>0);
  }
}

void KWView::showAlign( int align ) {
    switch ( align ) {
        case Qt::AlignLeft: // In left-to-right mode it's align left. TODO: alignright if text->isRightToLeft()
            kWarning() << k_funcinfo << "shouldn't be called with AlignAuto" << endl;
            // fallthrough
        case Qt::AlignLeft:
            m_actionFormatAlignLeft->setChecked( TRUE );
            break;
        case Qt::AlignHCenter:
            m_actionFormatAlignCenter->setChecked( TRUE );
            break;
        case Qt::AlignRight:
            m_actionFormatAlignRight->setChecked( TRUE );
            break;
        case Qt::AlignJustify:
            m_actionFormatAlignBlock->setChecked( TRUE );
            break;
    }
}

void KWView::showSpacing( int spacing ) {
  switch ( spacing )
  {
    case KoParagLayout::LS_SINGLE:
      m_actionFormatSpacingSingle->setChecked( TRUE );
      break;
    case KoParagLayout::LS_ONEANDHALF:
      m_actionFormatSpacingOneAndHalf->setChecked( TRUE );
      break;
    case KoParagLayout::LS_DOUBLE:
      m_actionFormatSpacingDouble->setChecked( TRUE );
      break;
    default:
      m_actionFormatSpacingSingle->setChecked( FALSE );
      m_actionFormatSpacingOneAndHalf->setChecked( FALSE );
      m_actionFormatSpacingDouble->setChecked( FALSE );
  }
}

void KWView::showCounter( KoParagCounter &c )
{
    QString styleStr("counterstyle_");
    styleStr += QString::number( c.style() );
    //kDebug() << "KWView::showCounter styleStr=" << styleStr << endl;
    KToggleAction* act = static_cast<KToggleAction *>( actionCollection()->action( styleStr.latin1() ) );
    Q_ASSERT( act );
    if ( act )
        act->setChecked( true );
}

void KWView::updateBorderButtons( const KoBorder& left, const KoBorder& right,
                               const KoBorder& top, const KoBorder& bottom )
{
    m_actionBorderLeft->setChecked( left.penWidth() > 0 );
    m_actionBorderRight->setChecked( right.penWidth() > 0 );
    m_actionBorderTop->setChecked( top.penWidth() > 0 );
    m_actionBorderBottom->setChecked( bottom.penWidth() > 0 );
    m_actionBorderOutline->setChecked(
            m_actionBorderLeft->isChecked() &&
            m_actionBorderRight->isChecked() &&
            m_actionBorderTop->isChecked() &&
            m_actionBorderBottom->isChecked());

    KoBorder border = left;
    if(left.penWidth() > 0)
        border = left;
    else if(right.penWidth() > 0)
        border = right;
    else if(top.penWidth() > 0)
        border = top;
    else if(bottom.penWidth() > 0)
        border = bottom;
    else
        return;// then don't update since they are all empty.

    m_actionBorderWidth->setCurrentItem( (int)border.penWidth() - 1 );
    m_actionBorderStyle->setCurrentItem( (int)border.getStyle() );
    m_actionBorderColor->setCurrentColor( border.color );
}

void KWView::updateReadWrite( bool readwrite )
{
    // First disable or enable everything
    Q3ValueList<KAction*> actions = actionCollection()->actions();
    // Also grab actions from the document
    actions += m_doc->actionCollection()->actions();
    Q3ValueList<KAction*>::ConstIterator aIt = actions.begin();
    Q3ValueList<KAction*>::ConstIterator aEnd = actions.end();
    for (; aIt != aEnd; ++aIt )
        (*aIt)->setEnabled( readwrite );

    if ( !readwrite )
    {
        // Readonly -> re-enable a few harmless actions
        m_actionFileStatistics->setEnabled( true );
        m_actionExtraCreateTemplate->setEnabled( true );
        m_actionViewPageMode->setEnabled( true );
        m_actionViewPreviewMode->setEnabled( true );

        m_actionViewTextMode->setEnabled( true );
        m_actionShowRuler->setEnabled( true );
        m_actionEditFind->setEnabled( true );
        m_actionViewFormattingChars->setEnabled( true );
        m_actionViewFrameBorders->setEnabled( true );
        // that's not readonly, in fact, it modifies the doc
        //m_actionViewHeader->setEnabled( true );
        //m_actionViewFooter->setEnabled( true );
        m_actionViewZoom->setEnabled( true );
        m_actionInsertComment->setEnabled( true );
        m_actionAllowAutoFormat->setEnabled( true );
        m_actionShowDocStruct->setEnabled( true );
        m_actionConfigureCompletion->setEnabled( true );
        m_actionFormatBullet->setEnabled(true);
        m_actionFormatNumber->setEnabled( true);
        m_actionSelectBookmark->setEnabled( true );
        KAction* act = actionCollection()->action("edit_sldatabase");
        if (act)
            act->setEnabled( true );

        // In fact the new view could be readwrite, so this is too dangerous
        // (e.g. during spellchecking or during search-n-replace)
        //act = actionCollection()->action("view_newview");
        //if (act)
        //    act->setEnabled( true );
    }
    else
    {
        frameSelectedChanged();
        slotFrameSetEditChanged();
        refreshCustomMenu();
        refreshDeletePageAction();
        // Correctly enable or disable undo/redo actions again
        m_doc->commandHistory()->updateActions();
    }
}

void KWView::refreshDeletePageAction()
{
    m_actionDeletePage->setEnabled( m_doc->pageCount() > 1 && m_doc->processingType() == KWDocument::DTP );
}

void KWView::showMouseMode( int mouseMode )
{
    switch ( mouseMode ) {
    case KWCanvas::MM_EDIT:
    case KWCanvas::MM_CREATE_TABLE:
    case KWCanvas::MM_CREATE_FORMULA:
    case KWCanvas::MM_CREATE_PART:
        // No tool to activate for this mode -> deselect all the others
        m_actionToolsCreateText->setChecked( false );
        m_actionToolsCreatePix->setChecked( false );
        //m_actionToolsCreatePart->setChecked( false );
        break;
    case KWCanvas::MM_CREATE_TEXT:
        m_actionToolsCreateText->setChecked( true );
        break;
    case KWCanvas::MM_CREATE_PIX:
        m_actionToolsCreatePix->setChecked( true );
        break;
        //case KWCanvas::MM_CREATE_PART:
        //m_actionToolsCreatePart->setChecked( true );
        break;
    }

    m_actionTableJoinCells->setEnabled( FALSE );
    m_actionTableSplitCells->setEnabled( FALSE );
    m_actionTableProtectCells->setEnabled( false );
    m_actionFormatFrameSet->setEnabled(FALSE);
    m_actionTablePropertiesMenu->setEnabled( false );
    m_actionConvertTableToText->setEnabled( false );
}

void KWView::showStyle( const QString & styleName )
{
    KoParagStyle* style = m_doc->styleCollection()->findStyle( styleName );
    if ( style ) {
        int pos = m_doc->styleCollection()->indexOf( style );
        // Select style in combo
        m_actionFormatStyle->setCurrentItem( pos );
        // Check the appropriate action among the m_actionFormatStyleMenu actions
        KToggleAction* act = dynamic_cast<KToggleAction *>(actionCollection()->action(style->name().utf8()));
        if ( act )
            act->setChecked( true );
    }
}

// used to avoid action naming conflicts
static const char* PARAGSTYLE_ACTION_PREFIX = "paragstyle_";
static const char* FRAMESTYLE_ACTION_PREFIX = "framestyle_";
static const char* TABLESTYLE_ACTION_PREFIX = "tablestyle_";

void KWView::updateStyleList()
{
    QString currentStyle = m_actionFormatStyle->currentText();
    // Generate list of styles
    const QStringList lst = m_doc->styleCollection()->displayNameList();
    const int pos = lst.findIndex( currentStyle );
    // Fill the combo - using a KSelectAction
    m_actionFormatStyle->setItems( lst );
    if ( pos > -1 )
        m_actionFormatStyle->setCurrentItem( pos );

    // Fill the menu - using a KActionMenu, so that it's possible to bind keys
    // to individual actions
    QStringList lstWithAccels;
    // Generate unique accelerators for the menu items
    KAccelGen::generate( lst, lstWithAccels );
    QMap<QString, KShortcut> shortCuts;

    KActionPtrList lst2 = actionCollection()->actions("styleList");
    Q3ValueList<KAction *> actions = lst2;
    Q3ValueList<KAction *>::ConstIterator it = lst2.begin();
    const Q3ValueList<KAction *>::ConstIterator end = lst2.end();
    for (; it != end; ++it )
    {
        shortCuts.insert( QString::fromUtf8( (*it)->name() ), (*it)->shortcut() );
        m_actionFormatStyleMenu->remove( *it );
        delete *it;
    }
    uint i = 0;
    for ( QStringList::Iterator it = lstWithAccels.begin(); it != lstWithAccels.end(); ++it, ++i )
    {
        // The list lst was created (unsorted) from the style collection, so we have still the same order.
        KoParagStyle *style = m_doc->styleCollection()->styleAt( i );
        if ( style )
        {
            QString name = PARAGSTYLE_ACTION_PREFIX + style->name();
            KToggleAction* act = new KToggleAction( (*it),
                                     shortCuts[name], this, SLOT( slotStyleSelected() ),
                                     actionCollection(), name.utf8() );
            act->setGroup( "styleList" );
            act->setExclusiveGroup( "styleListAction" );
            act->setToolTip( i18n( "Apply a paragraph style" ) );
            m_actionFormatStyleMenu->insert( act );
        }
        else
            kWarning() << "No style found for " << *it << endl;
    }
}

// Called when selecting a style in the Format / Style menu
void KWView::slotStyleSelected()
{
    QString actionName = QString::fromUtf8(sender()->name());
    const QString prefix = PARAGSTYLE_ACTION_PREFIX;
    if ( actionName.startsWith( prefix ) ) {
        actionName = actionName.mid( prefix.length() );
        kDebug(32001) << "KWView::slotStyleSelected " << actionName << endl;
        textStyleSelected( m_doc->styleCollection()->findStyle( actionName ) );
    }
}

void KWView::updateFrameStyleList()
{
    // Remember selected style (by name; better would be by pointer, but what if it got deleted?)
    // This is all in case the index of the selected style has changed.
    const QString currentStyle = m_actionFrameStyle->currentText();
    // Generate list of styles
    const QStringList lst = m_doc->frameStyleCollection()->displayNameList();
    const int pos = lst.findIndex( currentStyle );
    // Fill the combo
    m_actionFrameStyle->setItems( lst );
    if ( pos > -1 )
        m_actionFrameStyle->setCurrentItem( pos );

    // Fill the menu - using a KActionMenu, so that it's possible to bind keys
    // to individual actions
    QStringList lstWithAccels;
    // Generate unique accelerators for the menu items
    KAccelGen::generate( lst, lstWithAccels );
    QMap<QString, KShortcut> shortCuts; // style (internal) name -> shortcut


    KActionPtrList lst2 = actionCollection()->actions("frameStyleList");
    Q3ValueList<KAction *> actions = lst2;
    Q3ValueList<KAction *>::ConstIterator it = lst2.begin();
    Q3ValueList<KAction *>::ConstIterator end = lst2.end();
    for (; it != end; ++it )
    {
        shortCuts.insert( QString::fromUtf8( (*it)->name() ), (*it)->shortcut() );
        m_actionFrameStyleMenu->remove( *it );
        delete *it;
    }

    uint i = 0;
    for ( QStringList::Iterator it = lstWithAccels.begin(); it != lstWithAccels.end(); ++it, ++i )
    {
        // The list lst was created (unsorted) from the frame style collection, so we have still the same order.
        KWFrameStyle *style = m_doc->frameStyleCollection()->frameStyleAt( i );
        if ( style )
        {
            QString name = FRAMESTYLE_ACTION_PREFIX + style->name();
            KToggleAction* act = new KToggleAction( (*it),
                                                    shortCuts[name], // KDE4: use value()
                                                    this, SLOT( slotFrameStyleSelected() ),
                                                    actionCollection(), name.utf8() /*KDE4: remove conversion*/ );
            act->setGroup( "frameStyleList" );
            act->setExclusiveGroup( "frameStyleList" );
            act->setToolTip( i18n( "Apply a frame style" ) );
            m_actionFrameStyleMenu->insert( act );
        }
        else
            kWarning() << "No frame style found for " << *it << endl;
    }
}


void KWView::updateTableStyleList()
{
    const QString currentStyle = m_actionTableStyle->currentText();
    // Generate list of styles
    const QStringList lst = m_doc->tableStyleCollection()->displayNameList();
    const int pos = lst.findIndex( currentStyle );
    // Fill the combo
    m_actionTableStyle->setItems( lst );
    if ( pos > -1 )
        m_actionTableStyle->setCurrentItem( pos );

    // Fill the menu - using a KActionMenu, so that it's possible to bind keys
    // to individual actions
    QStringList lstWithAccels;
    // Generate unique accelerators for the menu items
    KAccelGen::generate( lst, lstWithAccels );
    QMap<QString, KShortcut> shortCuts;

    Q3ValueList<KAction *> actions = actionCollection()->actions("tableStyleList");
    Q3ValueList<KAction *>::ConstIterator it = actions.begin();
    const Q3ValueList<KAction *>::ConstIterator end = actions.end();
    for (; it != end; ++it )
    {
        shortCuts.insert( QString::fromUtf8( (*it)->name() ), (*it)->shortcut() );
        m_actionTableStyleMenu->remove( *it );
        delete *it;
    }

    uint i = 0;
    for ( QStringList::Iterator it = lstWithAccels.begin(); it != lstWithAccels.end(); ++it, ++i )
    {
        // The list lst was created (unsorted) from the table style collection, so we have still the same order.
        KWTableStyle *style = m_doc->tableStyleCollection()->tableStyleAt( i );
        if ( style )
        {
            QString name = TABLESTYLE_ACTION_PREFIX + style->name();
            KToggleAction* act = new KToggleAction( (*it),
                                     shortCuts[name], this, SLOT( slotTableStyleSelected() ),
                                     actionCollection(), name.utf8() );
            act->setExclusiveGroup( "tableStyleList" );
            act->setGroup( "tableStyleList" );
            act->setToolTip( i18n( "Apply a table style" ) );
            m_actionTableStyleMenu->insert( act );
        }
        else
            kWarning() << "No table style found for " << *it << endl;
    }
}

void KWView::editCut()
{
    KWFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->cut();
    else {
        Q3DragObject *drag = m_doc->dragSelected( frameViewManager()->selectedFrames() );
        QApplication::clipboard()->setData( drag );
        deleteFrame(false);
    }
}

void KWView::editCopy()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->copy();
    else {
        Q3DragObject *drag = m_doc->dragSelected( frameViewManager()->selectedFrames() );
        QApplication::clipboard()->setData( drag );
    }
}

void KWView::editPaste()
{
    QMimeSource *data = QApplication::clipboard()->data();
    pasteData( data, false );
}

// paste or drop
void KWView::pasteData( QMimeSource* data, bool drop )
{
    int provides = checkClipboard( data );
    Q_ASSERT( provides != 0 );

    // formula must be the first as a formula is also available as image
    if ( provides & ProvidesFormula ) {
        KWFrameSetEdit * edit = m_gui->canvasWidget()->currentFrameSetEdit();
        if ( edit && edit->frameSet()->type() == FT_FORMULA ) {
            edit->pasteData( data, ProvidesFormula, drop );
        }
        else {
            insertFormula( data );
        }
    }
    else // pasting text and/or frames
    {
        deselectAllFrames();
        // let the user select paste format if the clipboard contains an image URL
        if ( (provides & ProvidesImage) && (provides & ProvidesPlainText) && !( provides & ProvidesOasis ) )
        {
            QStringList list;
            list.append( i18n("Image") );
            list.append( i18n("Plain text") );
            bool ok;
            QString result = KInputDialog::getItem( i18n("Paste"), i18n("Select paste format:"), list, 0, false, &ok );
            if (!ok)
                return;
            if ( result == list.first() )
            {
                provides = ProvidesImage;
                data = QApplication::clipboard()->data();
            }
        }
        KWTextFrameSetEdit * edit = currentTextEdit();
        if ( edit && ( provides & ProvidesPlainText ) ) {
            edit->pasteData( data, provides, drop );
        } else if ( provides & ProvidesOasis ) {
            // Not editing a frameset? We can't paste plain text then... only entire frames.
            Q3CString returnedTypeMime = KoTextObject::providesOasis( data );
            if ( !returnedTypeMime.isEmpty() )
            {
                const QByteArray arr = data->encodedData( returnedTypeMime );
                if( !arr.isEmpty() )
                {
                    QBuffer buffer( arr );
                    KoStore * store = KoStore::createStore( &buffer, KoStore::Read );
                    KWOasisLoader oasisLoader( m_doc );
                    Q3ValueList<KWFrame *> frames = oasisLoader.insertOasisData( store, 0 /* no cursor */ );
                    delete store;
                    Q3ValueList<KWFrame *>::ConstIterator it = frames.begin();
                    KMacroCommand* macroCmd = 0L;
                    for ( ; it != frames.end() ; ++it )
                    {
                        if ( !macroCmd )
                            macroCmd = new KMacroCommand( i18n( "Paste" ) );

                        KWCreateFrameCommand *cmd = new KWCreateFrameCommand( i18n( "Paste" ), *it );
                        macroCmd->addCommand( cmd );

                        frameViewManager()->view(*it)->setSelected(true);
                        (*it)->frameSet()->updateFrames();
                    }

                    if ( macroCmd ) {
                        m_doc->addCommand( macroCmd );
                        KWFrameList::recalcAllFrames(m_doc);
                        return;
                    }
                }
            }
        }
        if ( provides & ProvidesImage )
        { // providesImage, must be after providesOasis
            KoPoint docPoint( m_currentPage->leftMargin(),
                    m_currentPage->offsetInDocument() + m_currentPage->topMargin());
            m_gui->canvasWidget()->pasteImage( data, docPoint );
        }
    }
}

void KWView::editSelectAll()
{
    KWFrameSetEdit * edit = m_gui->canvasWidget()->currentFrameSetEdit();
    if ( edit )
        edit->selectAll();
}

void KWView::editSelectAllFrames()
{
    m_gui->canvasWidget()->selectAllFrames( true );
}

void KWView::editSelectCurrentFrame()
{
    if (!m_gui) return;
    KWFrameSetEdit* edit = m_gui->canvasWidget()->currentFrameSetEdit();
    KWFrame* frame = edit->currentFrame();
    if (!frame) return;
    KWFrameView *view = frameViewManager()->view(frame);
    if (!view) return;
    view->setSelected(true);
}

void KWView::editFind()
{
    if (!m_searchEntry)
        m_searchEntry = new KoSearchContext();
    KWTextFrameSetEdit * edit = currentTextEdit();
    bool hasSelection = edit && edit->textFrameSet()->hasSelection();
    bool hasCursor = edit != 0L;

    KoSearchDia dialog( m_gui->canvasWidget(), "find", m_searchEntry, hasSelection, hasCursor );
    if ( dialog.exec() == QDialog::Accepted )
    {
        delete m_findReplace;
        m_findReplace = new KWFindReplace( m_gui->canvasWidget(), &dialog, m_gui->canvasWidget()->kWordDocument()->visibleTextObjects(viewMode()), edit);
        editFindNext();
    }
}

void KWView::editReplace()
{
    if (!m_searchEntry)
        m_searchEntry = new KoSearchContext();
    if (!m_replaceEntry)
        m_replaceEntry = new KoSearchContext();

    KWTextFrameSetEdit * edit = currentTextEdit();
    bool hasSelection = edit && edit->textFrameSet()->hasSelection();
    bool hasCursor = edit != 0L;

    KoReplaceDia dialog( m_gui->canvasWidget(), "replace", m_searchEntry, m_replaceEntry, hasSelection, hasCursor );
    if ( dialog.exec() == QDialog::Accepted )
    {
        delete m_findReplace;
        m_findReplace = new KWFindReplace( m_gui->canvasWidget(), &dialog, m_gui->canvasWidget()->kWordDocument()->visibleTextObjects(viewMode()), edit);
        editFindNext();
    }
}

void KWView::editFindNext()
{
    if ( !m_findReplace ) // shouldn't be called before find or replace is activated
    {
        editFind();
        return;
    }
    (void) m_findReplace->findNext();
}

void KWView::editFindPrevious()
{
    if ( !m_findReplace ) // shouldn't be called before find or replace is activated
    {
        editFind();
        return;
    }
    (void) m_findReplace->findPrevious();
}

void KWView::adjustZOrderOfSelectedFrames(MoveFrameType moveType) {
    KMacroCommand* macroCmd = 0L;
    // For each selected frame...
    Q3ValueList<KWFrameView*> selectedFrames = frameViewManager()->selectedFrames();
    if(selectedFrames.count()==0) return;

    KWPage *page = m_doc->pageManager()->page(selectedFrames[0]->frame());
    Q3PtrList<KWFrame> frames;
    Q3ValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
    while(framesIterator != selectedFrames.end()) {
        // include all frames in case of table.
        frames.append((*framesIterator)->frame());
        KWFrameSet *table = (*framesIterator)->frame()->frameSet()->groupmanager();
        if(table) {
            for (Q3PtrListIterator<KWFrame> cellIt(table->frameIterator()  ); cellIt.current() ; ++cellIt ) {
                KWFrame *frame = cellIt.current();
                if(page->rect().contains(*frame) && !frames.contains(frame))
                    frames.append(frame);
            }
        }
        ++framesIterator;
    }

    int lowestZOrder=10000;
    QString actionName;
    framesIterator = selectedFrames.begin();
    while(framesIterator != selectedFrames.end()) {
        KWFrame* frame = (*framesIterator)->frame();
        int newZOrder=0;
        switch(moveType) {
            case RaiseFrame:
                newZOrder=raiseFrame(frames,frame);
                actionName=i18n("Raise Frame");
                break;
            case LowerFrame:
                newZOrder=lowerFrame(frames,frame);
                actionName=i18n("Lower Frame");
                break;
            case BringToFront:
                newZOrder=bringToFront(frames,frame);
                actionName=i18n("Bring to Front");
                break;
            case SendToBack:
                newZOrder=sendToBack(frames,frame);
                actionName=i18n("Send to Back");
                break;
        }

        if ( newZOrder != frame->zOrder() ) { // only if changed.
            lowestZOrder=qMin(lowestZOrder, newZOrder);

            KWFrame* frameCopy = frame->getCopy();
            frame->setZOrder( newZOrder );
            frame->frameStack()->update();

            KWFramePropertiesCommand* cmd = new KWFramePropertiesCommand( QString::null, frameCopy, frame);
            if(!macroCmd)
                macroCmd = new KMacroCommand( actionName );
            macroCmd->addCommand(cmd);

            frameCopy = frame->getCopy();
            frame->setZOrder( newZOrder );

            cmd = new KWFramePropertiesCommand( QString::null, frameCopy, frame );
            if(!macroCmd)
                macroCmd = new KMacroCommand( actionName );
            macroCmd->addCommand(cmd);
        }
        ++framesIterator;
    }

    if ( macroCmd )
    {
        m_doc->addCommand(macroCmd);
        // Calling updateFrames() on the selected frames' framesets isn't enough,
        // we also need other framesets to notice the new frames on top.
        m_doc->updateAllFrames();
        m_doc->layout();
        m_doc->repaintAllViews();
    }

    if(lowestZOrder != 10000 && m_doc->processingType() == KWDocument::WP) {
        m_doc->lowerMainFrames( page->pageNumber(), lowestZOrder );
    }
}

// Make room for refZOrder, by raising all z-orders above it by 1
void KWView::increaseAllZOrdersAbove(int refZOrder, int pageNum, const Q3PtrList<KWFrame>& frameSelection) {
    Q3PtrList<KWFrame> framesInPage = m_doc->framesInPage( pageNum, false );
    for ( Q3PtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt ) {
        if(frameSelection.contains(frameIt.current()) > 0) continue; // ignore frames we selected.
        if(frameIt.current()->zOrder() >= refZOrder) {
            frameIt.current()->setZOrder( frameIt.current()->zOrder() + 1 );
        }
    }
}

// Make room for refZOrder, by lowering all z-orders below it by 1
void KWView::decreaseAllZOrdersUnder(int refZOrder, int pageNum, const Q3PtrList<KWFrame>& frameSelection) {
    Q3PtrList<KWFrame> framesInPage = m_doc->framesInPage( pageNum, false );
    for ( Q3PtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt ) {
        if(frameSelection.contains(frameIt.current()) > 0) continue; // ignore frames we selected.
        if(frameIt.current()->zOrder() <= refZOrder) {
            frameIt.current()->setZOrder( frameIt.current()->zOrder() - 1 );
        }
    }
}

int KWView::raiseFrame(const Q3PtrList<KWFrame>& frameSelection, const KWFrame *frame) {
    int newZOrder = 10000;
    Q3ValueList<int> zorders;
    Q3PtrList<KWFrame> framesInPage = m_doc->framesInPage( frame->pageNumber(), false );
    for ( Q3PtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt ) {
        if(frameSelection.contains(frameIt.current()) > 0) continue; // ignore other frames we selected.
        if(! frameIt.current()->intersects(*frame)) continue; // only frames that I intersect with.
        int z = frameIt.current()->zOrder();
        if(z > frame->zOrder()) {
            newZOrder=qMin(newZOrder, z + 1);
        }
        zorders.append( z );
    }
    if(newZOrder==10000) return frame->zOrder();
    // Ensure that newZOrder is "free"
    if ( zorders.find( newZOrder ) != zorders.end() )
        increaseAllZOrdersAbove( newZOrder, frame->pageNumber(), frameSelection );
    return newZOrder;
}

int KWView::lowerFrame(const Q3PtrList<KWFrame>& frameSelection, const KWFrame *frame) {
    int newZOrder = -10000;
    Q3ValueList<int> zorders;
    Q3PtrList<KWFrame> framesInPage = m_doc->framesInPage( frame->pageNumber(), false );
    for ( Q3PtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt ) {
        if(frameSelection.contains(frameIt.current()) > 0) continue; // ignore other frames we selected.
        if(frameIt.current()->frameSet()->isMainFrameset()) continue; // ignore main frameset.
        if(! frameIt.current()->intersects(*frame)) continue; // only frames that I intersect with.
        int z = frameIt.current()->zOrder();
        if(z < frame->zOrder()) {
            newZOrder=qMax(newZOrder, z -1);
        }
        zorders.append( z );
    }
    if(newZOrder==-10000) return frame->zOrder();
    // Ensure that newZOrder is "free"
    if ( zorders.find( newZOrder ) != zorders.end() )
        decreaseAllZOrdersUnder( newZOrder, frame->pageNumber(), frameSelection );
    return newZOrder;
}

int KWView::bringToFront(const Q3PtrList<KWFrame>& frameSelection, const KWFrame *frame) {
    int newZOrder = frame->zOrder();
    Q3PtrList<KWFrame> framesInPage = m_doc->framesInPage( frame->pageNumber(), false );
    for ( Q3PtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt ) {
        if(frameSelection.contains(frameIt.current()) > 0) continue; // ignore other frames we selected.
        if(! frameIt.current()->intersects(*frame)) continue; // only frames that I intersect with.
        newZOrder=qMax(newZOrder, frameIt.current()->zOrder()+1);
    }
    return newZOrder;
}

int KWView::sendToBack(const Q3PtrList<KWFrame>& frameSelection, const KWFrame *frame) {
    int newZOrder = frame->zOrder();
    Q3PtrList<KWFrame> framesInPage = m_doc->framesInPage( frame->pageNumber(), false );
    for ( Q3PtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt ) {
        if(frameSelection.contains(frameIt.current()) > 0) continue; // ignore other frames we selected.
        if(frameIt.current()->frameSet()->isMainFrameset()) continue; // ignore main frameset.
        if(! frameIt.current()->intersects(*frame)) continue; // only frames that I intersect with.
        newZOrder=qMin(newZOrder, frameIt.current()->zOrder()-1);
    }
    return newZOrder;
}

void KWView::editDeleteFrame()
{
    deleteFrame();
}

void KWView::deleteFrame( bool warning )
{
    if ( !m_doc->isReadWrite() )
        return;

    Q3ValueList<KWFrameView*> frames = frameViewManager()->selectedFrames();
    if( frames.count() < 1) {
        kWarning() << "KWView::deleteFrame: no frame selected" << endl;
        return;
    }
    if(frames.count()==1)
    {
        KWFrame *theFrame = frames[0]->frame();
        KWFrameSet *fs = theFrame->frameSet();

        Q_ASSERT( !fs->isAHeader() ); // the action is disabled for such cases
        Q_ASSERT( !fs->isAFooter() );
        if ( fs->isMainFrameset() || fs->isAFooter() || fs->isAHeader() || fs->isFootEndNote())
            return;

        // frame is part of a table?
        if ( fs->groupmanager() )
        {
            int result = KMessageBox::warningContinueCancel(
                this,
                i18n( "You are about to delete a table.\n"
                      "Doing so will delete all the text in the table.\n"
                      "Are you sure you want to do that?"),
                i18n("Delete Table"), KStdGuiItem::del(),
                "DeleteTableConfirmation",
                true );
            if (result != KMessageBox::Continue)
                return;
            m_doc->deleteTable( fs->groupmanager() );
            return;
        }

        if ( fs->frameCount() == 1 && fs->type() == FT_TEXT) {
            if ( fs->isMainFrameset())
                return; // if primairy FS, we can't delete it :)

            KWTextFrameSet * textfs = dynamic_cast<KWTextFrameSet *>(fs);
            Q_ASSERT(textfs);
            if ( !textfs )
                return;

            KoTextDocument * textdoc = textfs->textDocument();
            if ( textdoc->length() > 0 )
            {
                int result = KMessageBox::warningContinueCancel(
                    this,
                    i18n( "You are about to delete the last Frame of the "
                          "Frameset '%1'. "
                          "The contents of this Frameset will not appear "
                          "anymore!\n"
                          "Are you sure you want to do that?").arg(fs->name()),
                    i18n("Delete Frame"), KStdGuiItem::del());

                if (result != KMessageBox::Continue)
                    return;

                m_doc->deleteFrame( theFrame );
                return;
            }

        }

        if(warning)
        {
            int result = KMessageBox::warningContinueCancel(
                this,
                i18n("Do you want to delete this frame?"),
                i18n("Delete Frame"),
                KGuiItem(i18n("&Delete"),"editdelete"),
                "DeleteLastFrameConfirmation",
                true );
            if (result != KMessageBox::Continue)
                return;
        }
        m_doc->deleteFrame( theFrame );
    }
    else
    {
        //several frame
        if(warning)
        {
            int result = KMessageBox::warningContinueCancel(
                this,
                i18n("Do you want to delete this frame?"),
                i18n("Delete Frame"),
                KGuiItem(i18n("&Delete"),"editdelete"),
                "DeleteLastFrameConfirmation",
                true );
            if (result != KMessageBox::Continue)
                return;
        }

        deleteSelectedFrames();
    }
}

void KWView::createLinkedFrame()
{
    Q3ValueList<KWFrameView*> selectedFrames = frameViewManager()->selectedFrames();
    if (selectedFrames.count() != 1)
        return; // action is disabled in such a case
    KWFrame* frame = selectedFrames[0]->frame();
    KWFrame* newFrame = new KWFrame(0L, frame->x() + m_gui->getVertRuler()->minimumSizeHint().width(), frame->y() + m_gui->getHorzRuler()->minimumSizeHint().height(), frame->width(), frame->height() );
    newFrame->setZOrder( m_doc->maxZOrder( newFrame->pageNumber(m_doc) ) + 1 ); // make sure it's on top
    newFrame->setCopy(true);
    newFrame->setNewFrameBehavior( KWFrame::Copy );
    frame->frameSet()->addFrame( newFrame );

    frameViewManager()->view(newFrame)->setSelected(true);

    KWCreateFrameCommand *cmd = new KWCreateFrameCommand( i18n("Create Linked Copy"), newFrame );
    m_doc->addCommand( cmd );

    m_doc->frameChanged( newFrame );
}

void KWView::editCustomVariable()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if (edit)
    {
        KoCustomVariable *var = static_cast<KoCustomVariable *>(edit->variable());
        if (var)
        {
            QString oldvalue = var->value();
            KoCustomVarDialog dia( this, var );
            if ( dia.exec() )
            {
                m_doc->recalcVariables( VT_CUSTOM );
                if( var->value() != oldvalue )
                {
                    KWChangeCustomVariableValue *cmd=new KWChangeCustomVariableValue(i18n( "Change Custom Variable" ),m_doc, oldvalue, var->value(), var );
                    m_doc->addCommand(cmd);
                }
            }
        }
    }
}

void KWView::editCustomVars()
{
    KoCustomVariablesDia dia( this, m_doc->variableCollection()->getVariables() );
    QStringList listOldCustomValue;
    Q3PtrListIterator<KoVariable> oldIt( m_doc->variableCollection()->getVariables() );
    for ( ; oldIt.current() ; ++oldIt )
    {
        if(oldIt.current()->type()==VT_CUSTOM)
            listOldCustomValue.append(((KoCustomVariable*)oldIt.current())->value());
    }
    if(dia.exec())
    {
        m_doc->recalcVariables( VT_CUSTOM );
        //temporaly hack, for the moment we can't undo/redo change custom variables
        Q3PtrListIterator<KoVariable> it( m_doc->variableCollection()->getVariables() );
        KMacroCommand * macroCommand = 0L;
        int i=0;
        for ( ; it.current() ; ++it )
        {
            if(it.current()->type() == VT_CUSTOM )
            {
                if(((KoCustomVariable*)it.current())->value()!=*(listOldCustomValue.at(i)))
                {
                    if(!macroCommand)
                        macroCommand = new KMacroCommand( i18n( "Change Custom Variable" ) );
                    KWChangeCustomVariableValue *cmd=new KWChangeCustomVariableValue(i18n( "Change Custom Variable" ),m_doc,*(listOldCustomValue.at(i)), ((KoCustomVariable*)it.current())->value() ,((KoCustomVariable*)it.current()));
                    macroCommand->addCommand(cmd);
                }
                i++;
            }
        }
        if(macroCommand)
            m_doc->addCommand(macroCommand);
    }
}

void KWView::editMailMergeDataBase()
{
    m_doc->mailMergeDataBase()->showConfigDialog(this);
#if 0
    KWMailMergeEditor *dia = new KWMailMergeEditor( this, m_doc->mailMergeDataBase() );
    dia->exec();
    // Don't know if we really need this so it's commented out (SL)
    // m_gui->canvasWidget()->repaintAll( FALSE );
    delete dia;
#endif
}

void KWView::viewTextMode()
{
    if ( m_actionViewTextMode->isChecked() )
    {
        KWTextFrameSet* fs = KWViewModeText::determineTextFrameSet( m_doc );
        if ( fs ) { // TODO: disable the action when there is no text frameset available
            if ( dynamic_cast<KWViewModePreview *>(viewMode()) )
            {
                m_zoomViewModePreview.m_zoom = m_doc->zoom();
                m_zoomViewModePreview.m_zoomMode = m_doc->zoomMode();
            }
            else
            {
                m_zoomViewModeNormal.m_zoom = m_doc->zoom();
                m_zoomViewModeNormal.m_zoomMode = m_doc->zoomMode();
            }
            // we don't know ZOOM_PAGE in the new view. so
            // we use ZOOM_CONSTANT in that case
            switch(m_zoomViewModeNormal.m_zoomMode)
            {
                case KoZoomMode::ZOOM_WIDTH:
                    m_doc->setZoomMode(KoZoomMode::ZOOM_WIDTH);
                    QTimer::singleShot( 0, this, SLOT( updateZoom() ) );
                    break;
                case KoZoomMode::ZOOM_PAGE: // no break
                    m_zoomViewModeNormal.m_zoomMode = KoZoomMode::ZOOM_CONSTANT;
                case KoZoomMode::ZOOM_CONSTANT:
                    m_doc->setZoomMode(KoZoomMode::ZOOM_CONSTANT);
                    showZoom( m_zoomViewModeNormal.m_zoom );
                    setZoom( m_zoomViewModeNormal.m_zoom, false );
                    break;
            }
            m_doc->switchViewMode( "ModeText" );
        } else
            initGUIButton(); // ensure we show the current viewmode
    }
    else
        m_actionViewTextMode->setChecked( true ); // always one has to be checked !
}

void KWView::viewPageMode()
{
    if ( m_actionViewPageMode->isChecked() )
    {
        if ( dynamic_cast<KWViewModePreview *>(viewMode()) )
        {
            m_zoomViewModePreview.m_zoom = m_doc->zoom();
            m_zoomViewModePreview.m_zoomMode = m_doc->zoomMode();
        }
        else
        {
            m_zoomViewModeNormal.m_zoom = m_doc->zoom();
            m_zoomViewModeNormal.m_zoomMode = m_doc->zoomMode();
        }
        switch(m_zoomViewModeNormal.m_zoomMode)
        {
            case KoZoomMode::ZOOM_WIDTH:
                m_doc->setZoomMode(KoZoomMode::ZOOM_WIDTH);
                QTimer::singleShot( 0, this, SLOT( updateZoom() ) );
                break;
            case KoZoomMode::ZOOM_PAGE:
                m_doc->setZoomMode(KoZoomMode::ZOOM_PAGE);
                QTimer::singleShot( 0, this, SLOT( updateZoom() ) );
                break;
            case KoZoomMode::ZOOM_CONSTANT:
                m_doc->setZoomMode(KoZoomMode::ZOOM_CONSTANT);
                showZoom( m_zoomViewModeNormal.m_zoom );
                setZoom( m_zoomViewModeNormal.m_zoom, false );
                break;
        }
        m_doc->switchViewMode( "ModeNormal" );
    }
    else
        m_actionViewPageMode->setChecked( true ); // always one has to be checked !
}

void KWView::viewPreviewMode()
{
    if ( m_actionViewPreviewMode->isChecked() )
    {
        m_zoomViewModeNormal.m_zoom = m_doc->zoom();
        m_zoomViewModeNormal.m_zoomMode = m_doc->zoomMode();
        switch(m_zoomViewModePreview.m_zoomMode)
        {
            case KoZoomMode::ZOOM_WIDTH:
                m_doc->setZoomMode(KoZoomMode::ZOOM_WIDTH);
                QTimer::singleShot( 0, this, SLOT( updateZoom() ) );
                break;
            case KoZoomMode::ZOOM_PAGE:
                m_doc->setZoomMode(KoZoomMode::ZOOM_PAGE);
                QTimer::singleShot( 0, this, SLOT( updateZoom() ) );
                break;
            case KoZoomMode::ZOOM_CONSTANT:
                m_doc->setZoomMode(KoZoomMode::ZOOM_CONSTANT);
                showZoom( m_zoomViewModePreview.m_zoom );
                setZoom( m_zoomViewModePreview.m_zoom, false );
                break;
        }
        m_doc->switchViewMode( "ModePreview" );
    }
    else
        m_actionViewPreviewMode->setChecked( true ); // always one has to be checked !
}

void KWView::updateZoomControls()
{
    switch(m_doc->zoomMode())
    {
        case KoZoomMode::ZOOM_WIDTH:
        case KoZoomMode::ZOOM_PAGE:
            showZoom( KoZoomMode::toString(m_doc->zoomMode()) );
            break;
        case KoZoomMode::ZOOM_CONSTANT:
            changeZoomMenu( m_doc->zoom() );
            showZoom( m_doc->zoom() );
            break;
    }
}

void KWView::changeZoomMenu( int zoom )
{
    QString mode;
    if ( m_gui && m_gui->canvasWidget() && viewMode())
        mode = viewMode()->type();

    QStringList lst;
    lst << KoZoomMode::toString(KoZoomMode::ZOOM_WIDTH);
    if ( mode!="ModeText" )
        lst << KoZoomMode::toString(KoZoomMode::ZOOM_PAGE);

    if(zoom>0)
    {
        Q3ValueList<int> list;
        bool ok;
        const QStringList itemsList ( m_actionViewZoom->items() );
        QRegExp regexp("(\\d+)"); // "Captured" non-empty sequence of digits

        for (QStringList::ConstIterator it = itemsList.begin() ; it != itemsList.end() ; ++it)
        {
            regexp.search(*it);
            const int val=regexp.cap(1).toInt(&ok);
            //zoom : limit inferior=10
            if(ok && val>9 && list.contains(val)==0)
                list.append( val );
        }
        //necessary at the beginning when we read config
        //this value is not in combo list
        if(list.contains(zoom)==0)
            list.append( zoom );

        qHeapSort( list );

        for (Q3ValueList<int>::Iterator it = list.begin() ; it != list.end() ; ++it)
            lst.append( i18n("%1%").arg(*it) );
    }
    else
    {
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
    }
    m_actionViewZoom->setItems( lst );
}

void KWView::showZoom( int zoom )
{
    QStringList list = m_actionViewZoom->items();
    QString zoomStr( i18n("%1%").arg( zoom ) );
    m_actionViewZoom->setCurrentItem( list.findIndex(zoomStr)  );
}

void KWView::showZoom( const QString& zoom )
{
    QStringList list = m_actionViewZoom->items();
    m_actionViewZoom->setCurrentItem( list.findIndex( zoom )  );
}

void KWView::slotViewFormattingChars()
{
    m_doc->setViewFormattingChars(m_actionViewFormattingChars->isChecked());
    m_doc->layout(); // Due to the different formatting when this option is activated
    m_doc->repaintAllViews();
}

void KWView::slotViewFrameBorders()
{
    setViewFrameBorders(m_actionViewFrameBorders->isChecked());
    m_gui->canvasWidget()->repaintAll();
}

void KWView::viewHeader()
{
    bool state = m_actionViewHeader->isChecked();
    m_doc->setHeaderVisible( state );
    KWHideShowHeader *cmd=new KWHideShowHeader( state ? i18n("Enable Document Headers"):i18n("Disable Document Headers"), m_doc, state);
    m_doc->addCommand(cmd);
    updateHeader();
}

void KWView::updateHeader()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    bool state = m_actionViewHeader->isChecked();
    if(!state )
    {
        KWFrameSet *frameSet=0L;
        if(edit)
        {
            frameSet=edit->frameSet();
            if (frameSet->isAHeader())
                m_doc->terminateEditing( frameSet );
            else
            {
                KWTableFrameSet *table = frameSet->frame(0)->frameSet()->groupmanager();
                if (table)
                {
                    if (table->isFloating() && table->anchorFrameset()->isAHeader())
                        m_doc->terminateEditing( table );
                }
            }

        }
        else
        {
            KWFormulaFrameSetEdit * editFormula = dynamic_cast<KWFormulaFrameSetEdit *>(m_gui->canvasWidget()->currentFrameSetEdit());
            if(editFormula)
            {
                frameSet= editFormula->frameSet();
                if(frameSet->type()==FT_FORMULA && frameSet->isFloating())
                    m_doc->terminateEditing( frameSet );
            }

        }
    }
}


void KWView::viewFooter()
{
    bool state=m_actionViewFooter->isChecked();
    m_doc->setFooterVisible( state );
    KWHideShowFooter *cmd=new KWHideShowFooter( state ? i18n("Enable Document Footers"):i18n("Disable Document Footers"), m_doc, state);
    m_doc->addCommand(cmd);
    updateFooter();
}

void KWView::updateFooter()
{
    bool state=m_actionViewFooter->isChecked();
    KWTextFrameSetEdit * edit = currentTextEdit();
    if(!state )
    {
        KWFrameSet *frameSet=0L;
        if(edit)
        {
            frameSet=edit->frameSet();
            if (frameSet->isAFooter())
                m_doc->terminateEditing( frameSet );
            else
            {
                KWTableFrameSet *table = frameSet->frame(0)->frameSet()->groupmanager();
                if (table)
                {
                    if (table->isFloating() && table->anchorFrameset()->isAFooter())
                        m_doc->terminateEditing( table );
                }
            }
        }
        else
        {
            KWFormulaFrameSetEdit * editFormula = dynamic_cast<KWFormulaFrameSetEdit *>(m_gui->canvasWidget()->currentFrameSetEdit());
            if(editFormula)
            {
                frameSet= editFormula->frameSet();
                if(frameSet->type()==FT_FORMULA && frameSet->isFloating())
                    m_doc->terminateEditing( frameSet );

            }

        }
    }

}

void KWView::updateZoom( ) {
    viewZoom(m_actionViewZoom->currentText());
}

void KWView::viewZoom( const QString &s )
{
    bool ok=false;
    KWCanvas * canvas = m_gui->canvasWidget();
    int zoom = 0;

    if ( s == KoZoomMode::toString(KoZoomMode::ZOOM_WIDTH) )
    {
        m_doc->setZoomMode(KoZoomMode::ZOOM_WIDTH);
        zoom = qRound( static_cast<double>(canvas->visibleWidth() * 100 ) / (m_doc->resolutionX() * m_currentPage->width() ) ) - 1;

        if(zoom != m_doc->zoom() && !canvas->verticalScrollBar() ||
                !canvas->verticalScrollBar()->isVisible()) { // has no vertical scrollbar
            // we have to do this twice to take into account a possibly appearing vertical scrollbar
            QTimer::singleShot( 0, this, SLOT( updateZoom() ) );
        }
        ok = true;
    }
    else if ( s == KoZoomMode::toString(KoZoomMode::ZOOM_PAGE) )
    {
        m_doc->setZoomMode(KoZoomMode::ZOOM_PAGE);
        double height = m_doc->resolutionY() * m_currentPage->height();
        double width = m_doc->resolutionX() * m_currentPage->height();
        zoom = qMin( qRound( static_cast<double>(canvas->visibleHeight() * 100 ) / height ),
                     qRound( static_cast<double>(canvas->visibleWidth() * 100 ) / width ) ) - 1;

        ok = true;
    }
    else
    {
        m_doc->setZoomMode(KoZoomMode::ZOOM_CONSTANT);
        QRegExp regexp("(\\d+)"); // "Captured" non-empty sequence of digits
        regexp.search(s);
        zoom=regexp.cap(1).toInt(&ok);
    }

    if( !ok || zoom<10 ) //zoom should be valid and >10
        zoom = m_doc->zoom();
    if ( !KoZoomMode::isConstant(s) )
        showZoom( s ); //set current menu item
    else
    {
        changeZoomMenu( zoom ); //add current zoom value to the menu
        showZoom( zoom ); //set current menu item
    }

    //apply zoom if zoom!=m_doc->zoom()
    if( zoom != m_doc->zoom() )
    {
        setZoom( zoom, true );

        KWTextFrameSetEdit * edit = currentTextEdit();
        if ( edit )
            edit->ensureCursorVisible();
    }

    m_gui->canvasWidget()->setFocus();

}

void KWView::setZoom( int zoom, bool updateViews )
{
    m_doc->setZoomAndResolution( zoom, KoGlobal::dpiX(), KoGlobal::dpiY());
    m_doc->newZoomAndResolution( updateViews, false );
    m_doc->updateZoomRuler();

    if ( statusBar() )
        m_sbZoomLabel->setText( ' ' + QString::number( zoom ) + "% " );

    // Also set the zoom in KoView (for embedded views)
    kDebug() << "KWView::setZoom " << zoom << " setting koview zoom to " << m_doc->zoomedResolutionY() << endl;
    KoView::setZoom( m_doc->zoomedResolutionY() /* KoView only supports one zoom */ );
}

void KWView::insertPicture()
{
    if ( m_actionToolsCreatePix->isChecked() )
    {
        KWInsertPicDia dia( this,m_gui->canvasWidget()->pictureInline(),m_gui->canvasWidget()->pictureKeepRatio(),m_doc );
        if ( dia.exec() == QDialog::Accepted && !dia.picture().isNull() )
        {
            insertPicture( dia.picture(), dia.makeInline(), dia.keepRatio(), 0, 0 );
            m_gui->canvasWidget()->setPictureInline( dia.makeInline());
            m_gui->canvasWidget()->setPictureKeepRatio( dia.keepRatio() );
        }
        else
            m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_EDIT );
    }
    else
    {
        // clicked on the already active tool -> abort
        m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_EDIT );
    }
}


void KWView::slotEmbedImage( const QString &filename )
{
    KoPicture picture;
    KoPictureKey key;
    key.setKeyFromFile( filename );
    picture.setKey( key );
    picture.loadFromFile( filename );
    insertPicture( picture, false, true, 0, 0 );
}

void KWView::insertPicture( const KoPicture& picture, const bool makeInline, const bool keepRatio, int suggestedWidth, int suggestedHeight )
{
    QSize pixmapSize( picture.getOriginalSize() );
    if ( suggestedWidth > 0 && suggestedHeight > 0 )
        pixmapSize = QSize( suggestedWidth, suggestedHeight );

    if ( makeInline )
    {
        const double widthLimit = m_currentPage->width() - m_currentPage->leftMargin() -
            m_currentPage->rightMargin() - 10;
        const double heightLimit = m_currentPage->height() - m_currentPage->topMargin() -
            m_currentPage->bottomMargin() - 10;
        m_fsInline = 0;
        KWPictureFrameSet *frameset = new KWPictureFrameSet( m_doc, QString::null );

        frameset->insertPicture( picture );

        // This ensures 1-1 at 100% on screen, but allows zooming and printing with correct DPI values
        // ### TODO/FIXME: is the qRound really necessary?
        double width = m_doc->unzoomItX( qRound( (double)pixmapSize.width() * m_doc->zoomedResolutionX() / POINT_TO_INCH( KoGlobal::dpiX() ) ) );
        double height = m_doc->unzoomItY( qRound( (double)pixmapSize.height() * m_doc->zoomedResolutionY() / POINT_TO_INCH( KoGlobal::dpiY() ) ) );

        frameset->setKeepAspectRatio( keepRatio);


        if ( keepRatio && ((width > widthLimit) || (height > heightLimit)) )
        {
            // size too big => adjust the size and keep ratio
            const double ratioX = width / widthLimit;
            const double ratioY = height / heightLimit;
            const double ratioPicture = width / height;

            if ( ratioPicture == 0 )  // unlikely
            {
                width = widthLimit;
                height = heightLimit;
            }
            else
                if ( ratioX > ratioY )  // restrict width and calculate height
                {
                    width = widthLimit;
                    height = widthLimit/ratioPicture;
                }
                else   // restrict height and calculate width
                {
                    width = heightLimit*ratioPicture;
                    height = heightLimit;
                }
        }
        else
        {
            // Apply reasonable limits
            width = qMin( width, widthLimit );
            height = qMin( height, heightLimit );
        }

        m_fsInline = frameset;
        KWFrame *frame = new KWFrame ( m_fsInline, 0, 0, width, height );
        m_fsInline->addFrame( frame, false );
        m_gui->canvasWidget()->inlinePictureStarted();
        showMouseMode( KWCanvas::MM_EDIT );

        displayFrameInlineInfo();

#if 0
        edit->insertFloatingFrameSet( fs, i18n("Insert Picture Inline") );
        fs->finalize(); // done last since it triggers a redraw
        showMouseMode( KWCanvas::MM_EDIT );
        m_doc->refreshDocStructure(Pictures);
#endif
    }
    else
    {
        m_gui->canvasWidget()->insertPicture( picture, pixmapSize, keepRatio );
    }
}

bool KWView::insertInlinePicture()
{
    Q_ASSERT( m_fsInline );
    KWTextFrameSetEdit * edit = currentTextEdit();
    if(edit)
    {
        if ( edit->textFrameSet()->textObject()->protectContent() )
            return false;

        m_doc->addFrameSet( m_fsInline, false ); // done first since the frame number is stored in the undo/redo
#if 0
        KWFrame *frame = new KWFrame( m_fsInline, 0, 0, m_doc->unzoomItX( width ), m_doc->unzoomItY( height ) );
        m_fsInline->addFrame( frame, false );
#endif
        edit->insertFloatingFrameSet( m_fsInline, i18n("Insert Picture Inline") );
        m_fsInline->finalize(); // done last since it triggers a redraw
        showMouseMode( KWCanvas::MM_EDIT );
        m_doc->refreshDocStructure(Pictures);
        m_fsInline=0;
        updateFrameStatusBarItem();
    }
    else
    {
        delete m_fsInline;
        m_fsInline=0;
        updateFrameStatusBarItem();
    }
    return true;
}

void KWView::displayFrameInlineInfo()
{
    KMessageBox::information(this,
                             i18n("Set cursor where you want to insert inline frame."),
                             i18n("Insert Inline Frame"),
                             "SetCursorInsertInlineFrame",true);

    if ( statusBar() && m_sbFramesLabel )
        m_sbFramesLabel->setText( ' ' + i18n("Set cursor where you want to insert inline frame." ) );
}

void KWView::insertSpecialChar()
{
    KWTextFrameSetEdit *edit=currentTextEdit();
    if ( !edit )
        return;
    QString f = edit->textFontFamily();
    QChar c=' ';
    if (m_specialCharDlg==0)
    {
        m_specialCharDlg = new KoCharSelectDia( this, "insert special char", f, c, false );
        connect( m_specialCharDlg, SIGNAL(insertChar(QChar,const QString &)),
                 this, SLOT(slotSpecialChar(QChar,const QString &)));
        connect( m_specialCharDlg, SIGNAL( finished() ),
                 this, SLOT( slotSpecialCharDlgClosed() ) );
    }
    m_specialCharDlg->show();
}

void KWView::slotSpecialCharDlgClosed()
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

void KWView::slotSpecialChar(QChar c, const QString &font)
{
    KWTextFrameSetEdit *edit=currentTextEdit();
    if ( !edit )
        return;
    edit->insertSpecialChar(c, font);
}

void KWView::insertFrameBreak()
{
    KWTextFrameSetEdit *edit=currentTextEdit();
    if ( !edit )
        return;
    edit->insertFrameBreak();
}

void KWView::insertPage()
{
    if ( m_doc->processingType() == KWDocument::WP )
    {
        m_gui->canvasWidget()->editFrameSet( m_doc->frameSet(0) );
        KWTextFrameSetEdit *edit = currentTextEdit();
        Q_ASSERT(edit);
        if ( edit )
            edit->insertWPPage();
    } else {
        KWInsertPageDia dlg( this, "insertpage");
        if ( dlg.exec())
        {
            // If 'before', subtract 1 to the page number
            int page = m_currentPage->pageNumber();
            KCommand* cmd = new KWInsertRemovePageCommand( m_doc, KWInsertRemovePageCommand::Insert, dlg.insertPagePos()==KW_INSERTPAGEAFTER ?  page : (page -1));
            cmd->execute();
            m_doc->addCommand( cmd );
        }
    }
}

void KWView::deletePage()
{
    KCommand* cmd = new KWInsertRemovePageCommand( m_doc, KWInsertRemovePageCommand::Remove, m_currentPage->pageNumber() );
    cmd->execute();
    m_doc->addCommand( cmd );
    if(m_doc->lastPage() > m_currentPage->pageNumber())
        m_currentPage = m_doc->pageManager()->page( m_doc->lastPage() );
}

void KWView::insertLink()
{
    KWTextFrameSetEdit *edit=currentTextEdit();
    if ( !edit )
        return;
    QString link;
    QString ref;
    if ( edit->textFrameSet()->hasSelection() )
    {
        QString selectedText = edit->textFrameSet()->textObject()->selectedText();
        if ( edit->textFrameSet()->textObject()->selectionHasCustomItems() || selectedText.contains('\n') )
            return;
        if ( selectedText.startsWith( "mailto:/" ) ||
             selectedText.startsWith( "ftp:/" ) ||
             selectedText.startsWith( "http:/" ) )
        {
            link=selectedText;
            ref = selectedText;
        }
        else
        {
            //Just add text as link name and not url
            link = selectedText;
        }
    }

    if(KoInsertLinkDia::createLinkDia(link, ref, m_doc->listOfBookmarkName(0), true, this))
    {
        if(!link.isEmpty() && !ref.isEmpty())
            edit->insertLink(link, ref);
    }
}

void KWView::insertComment()
{
    KWTextFrameSetEdit *edit=currentTextEdit();
    if ( !edit )
        return;
    QString authorName;
    KoDocumentInfo * info = m_doc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
        kWarning() << "Author information not found in documentInfo !" << endl;
    else
        authorName = authorPage->fullName();

    KoCommentDia *commentDia = new KoCommentDia( this, QString::null,authorName );
    if( commentDia->exec() )
    {
        edit->insertComment(commentDia->commentText());
    }
    delete commentDia;
}


void KWView::insertVariable()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KAction * act = (KAction *)(sender());
        VariableDefMap::Iterator it = m_variableDefMap.find( act );
        if ( it == m_variableDefMap.end() )
            kWarning() << "Action not found in m_variableDefMap." << endl;
        else
        {
            if ( (*it).type == VT_FIELD )
                edit->insertVariable( (*it).type, KoFieldVariable::fieldSubType( (*it).subtype ) );
            else
                edit->insertVariable( (*it).type, (*it).subtype );
        }
    }
}

void KWView::insertFootNote()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    //Q_ASSERT( edit ); // the action should be disabled if we're not editing a textframeset...
    if ( edit ) // test for dcop call !
    {
        if ( edit->frameSet() != m_doc->frameSet(0) )
        {
            KMessageBox::sorry( this,
                                i18n( "You can only insert footnotes or "
                                      "endnotes into the first frameset."),
                                i18n("Insert Footnote"));
        } else {
            KWFootNoteDia dia( m_gui->canvasWidget()->footNoteType(), m_gui->canvasWidget()->numberingFootNoteType(), QString::null, this, m_doc, 0 );
            Q3PtrListIterator<KoTextCustomItem> it( edit->textDocument()->allCustomItems() );
            for ( ; it.current() ; ++it )
            {
                KWFootNoteVariable *fnv = dynamic_cast<KWFootNoteVariable *>( it.current() );
                if (fnv && !fnv->isDeleted() && fnv->frameSet() && !fnv->frameSet()->isDeleted() &&
                 fnv->numberingType()==KWFootNoteVariable::Manual )
                    dia.appendManualFootNote( fnv->text() );
            }
            if ( dia.exec() ) {
                edit->insertFootNote( dia.noteType(), dia.numberingType(), dia.manualString() );
                m_gui->canvasWidget()->setFootNoteType( dia.noteType() );
                m_gui->canvasWidget()->setNumberingFootNoteType( dia.numberingType() );
            }
        }
    }
}

void KWView::updateTocActionText(bool hasToc)
{
   KActionCollection * coll = actionCollection();
   QString name= hasToc ? i18n("Update Table of &Contents"):i18n("Table of &Contents");
   coll->action("insert_contents")->setText(name);
}

void KWView::insertContents()
{
    KWTextFrameSetEdit *edit = currentTextEdit();
    if (edit)
        edit->insertTOC();
}

void KWView::formatFont()
{
    KoTextFormatInterface* textIface = applicableTextInterfaces().first();

    if ( !textIface || !textIface->currentFormat() )
        return;

    delete m_fontDlg;
    m_fontDlg = new KoFontDia( *textIface->currentFormat()
                               , m_broker
                               , this, "" );

    connect( m_fontDlg, SIGNAL( applyFont() ),
                 this, SLOT( slotApplyFont() ) );

    m_fontDlg->exec();
    delete m_fontDlg;
    m_fontDlg=0L;

    //m_gui->canvasWidget()->setFocus();
}

void KWView::slotApplyFont()
{
    int flags = m_fontDlg->changedFlags();
    if ( flags )
    {
        KMacroCommand *globalCmd = new KMacroCommand(i18n("Change Font"));
        Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
        Q3PtrListIterator<KoTextFormatInterface> it( lst );
        for ( ; it.current() ; ++it )
        {
            KoTextFormat newFormat = m_fontDlg->newFormat();
            KCommand *cmd = it.current()->setFormatCommand( &newFormat, flags, true);
            if (cmd)
                globalCmd->addCommand(cmd);
        }
        m_doc->addCommand(globalCmd);
        m_gui->canvasWidget()->setFocus(); // the combo keeps focus...
    }

}

void KWView::formatParagraph()
{
    showParagraphDialog();
}

void KWView::showParagraphDialog( int initialPage, double initialTabPos )
{
    KWTextFrameSetEdit *edit = currentTextEdit();
    if (edit)
    {
        delete m_paragDlg;
        bool showFrameEndOptions = !edit->frameSet()->isHeaderOrFooter() &&
                                   !edit->frameSet()->groupmanager();
        m_paragDlg = new KoParagDia( this, "",
                                     KoParagDia::PD_SPACING | KoParagDia::PD_ALIGN |
                                     KoParagDia::PD_DECORATION | KoParagDia::PD_NUMBERING |
                                     KoParagDia::PD_TABS,
                                     m_doc->unit(),
                                     edit->textFrameSet()->frame(0)->width(),
                                     showFrameEndOptions,
                                     edit->frameSet()->isFootEndNote());
        m_paragDlg->setCaption( i18n( "Paragraph Settings" ) );

        // Initialize the dialog from the current paragraph's settings
        m_paragDlg->setParagLayout( edit->cursor()->parag()->paragLayout() );

        // Set initial page and initial tabpos if necessary
        if ( initialPage != -1 )
        {
            m_paragDlg->setCurrentPage( initialPage );
            if ( initialPage == KoParagDia::PD_TABS )
                m_paragDlg->tabulatorsWidget()->setCurrentTab( initialTabPos );
        }
        connect( m_paragDlg, SIGNAL( applyParagStyle() ), this, SLOT( slotApplyParag()));

        m_paragDlg->exec();
        delete m_paragDlg;
        m_paragDlg=0L;
    }

}

void KWView::slotApplyParag()
{
    KWTextFrameSetEdit *edit = currentTextEdit();
    if( !edit)
        return;
    KMacroCommand * macroCommand = 0L;
    KCommand *cmd=0L;
    if(m_paragDlg->isLeftMarginChanged())
    {
        cmd=edit->setMarginCommand( Q3StyleSheetItem::MarginLeft, m_paragDlg->leftIndent() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
        m_gui->getHorzRuler()->setLeftIndent( KoUnit::toUserValue( m_paragDlg->leftIndent(), m_doc->unit() ) );

    }

    if(m_paragDlg->isRightMarginChanged())
    {
        cmd=edit->setMarginCommand( Q3StyleSheetItem::MarginRight, m_paragDlg->rightIndent() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
        m_gui->getHorzRuler()->setRightIndent( KoUnit::toUserValue( m_paragDlg->rightIndent(), m_doc->unit() ) );
    }
    if(m_paragDlg->isSpaceBeforeChanged())
    {
        cmd=edit->setMarginCommand( Q3StyleSheetItem::MarginTop, m_paragDlg->spaceBeforeParag() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
    }
    if(m_paragDlg->isSpaceAfterChanged())
    {
        cmd=edit->setMarginCommand( Q3StyleSheetItem::MarginBottom, m_paragDlg->spaceAfterParag() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
    }
    if(m_paragDlg->isFirstLineChanged())
    {
        cmd=edit->setMarginCommand( Q3StyleSheetItem::MarginFirstLine, m_paragDlg->firstLineIndent());
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
        m_gui->getHorzRuler()->setFirstIndent(
            KoUnit::toUserValue( m_paragDlg->firstLineIndent(), m_doc->unit() ) );
    }

    if(m_paragDlg->isAlignChanged())
    {
        cmd=edit->setAlignCommand( m_paragDlg->align() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
    }
    if(m_paragDlg->isCounterChanged())
    {
        cmd=edit->setCounterCommand( m_paragDlg->counter() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
    }
    if(m_paragDlg->listTabulatorChanged())
    {
        cmd=edit->setTabListCommand( m_paragDlg->tabListTabulator() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );
            macroCommand->addCommand(cmd);
        }
    }

    if(m_paragDlg->isLineSpacingChanged())
    {
        cmd=edit->setLineSpacingCommand( m_paragDlg->lineSpacing(),m_paragDlg->lineSpacingType() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );

            macroCommand->addCommand(cmd);
        }
    }
    if(m_paragDlg->isBorderChanged())
    {
        cmd=edit->setBordersCommand( m_paragDlg->leftBorder(),
                                     m_paragDlg->rightBorder(),
                                     m_paragDlg->topBorder(),
                                     m_paragDlg->bottomBorder() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );

            macroCommand->addCommand(cmd);
        }
    }
    if( m_paragDlg->isJoinBorderChanged() )
    {
        cmd=edit->setJoinBordersCommand( m_paragDlg->joinBorder() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );

            macroCommand->addCommand(cmd);
        }
    }
    if ( m_paragDlg->isPageBreakingChanged() )
    {
        cmd=edit->setPageBreakingCommand( m_paragDlg->pageBreaking() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );

            macroCommand->addCommand(cmd);
        }
    }

    if ( m_paragDlg->isBackgroundColorChanged() )
    {
        cmd=edit->setBackgroundColorCommand( m_paragDlg->backgroundColor() );
        if(cmd)
        {
            if ( !macroCommand )
                macroCommand = new KMacroCommand( i18n( "Paragraph Settings" ) );

            macroCommand->addCommand(cmd);
        }
    }

    if(macroCommand)
        m_doc->addCommand(macroCommand);
    // Set "oldLayout" in KoParagDia from the current paragraph's settings
    // Otherwise "isBlahChanged" will return wrong things when doing A -> B -> A
    m_paragDlg->setParagLayout( edit->cursor()->parag()->paragLayout() );
}

// This handles Tabulators _only_
void KWView::slotHRulerDoubleClicked( double ptpos )
{
    showParagraphDialog( KoParagDia::PD_TABS, ptpos );
}

// This handles either:
// - Indents
// - Page Layout
//
// This does _not_ handle Tabulators!
void KWView::slotHRulerDoubleClicked()
{
    QString mode = viewMode()->type();
    bool state = (mode!="ModeText");
    if ( !state )
        return;

    KoRuler *ruler = m_gui->getHorzRuler ();
    if ( (ruler->flags() & KoRuler::F_INDENTS) && currentTextEdit() ) {
        if ( ruler->doubleClickedIndent () ) {
            formatParagraph();
            return;
        }
    }

    formatPage();
}

void KWView::formatPage()
{
    if( !m_doc->isReadWrite())
        return;
    QString mode = viewMode()->type();
    bool state = (mode!="ModeText");
    if ( !state )
        return;

    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter kwhf;
    m_doc->getPageLayout( pgLayout, cl, kwhf );

    KWPageLayoutStruct oldLayout( pgLayout, cl, kwhf );

    KoHeadFoot hf;
    int flags = FORMAT_AND_BORDERS | DISABLE_UNIT;
    if ( m_doc->processingType() == KWDocument::WP )
        flags |= KW_HEADER_AND_FOOTER | COLUMNS;
    else
        flags |= DISABLE_BORDERS;

    KoUnit::Unit unit = m_doc->unit();
    KoUnit::Unit oldUnit = unit;

    if ( KoPageLayoutDia::pageLayout( pgLayout, hf, cl, kwhf, flags, unit, this ) )
    {
        if( !(oldLayout._pgLayout==pgLayout) ||
            oldLayout._cl != cl ||
            oldLayout._hf != kwhf )
        {
            KWPageLayoutStruct newLayout( pgLayout, cl, kwhf );

            KWTextFrameSetEdit *edit = currentTextEdit();
            if (edit)
                edit->textFrameSet()->clearUndoRedoInfo();
            KCommand *cmd =new KWPageLayoutCommand( i18n("Change Layout"),
                                                    m_doc, oldLayout, newLayout );
            m_doc->addCommand(cmd);

            m_doc->setPageLayout( pgLayout, cl, kwhf );
        }
        if ( unit != oldUnit )
            m_doc->setUnit( unit ); // ##### needs undo/redo support
    }
}

void KWView::formatFrameSet()
{
    if ( frameViewManager()->selectedFrame() )
    {
        m_gui->canvasWidget()->editFrameProperties();
    }
    else // Should never happen, the action is disabled
        KMessageBox::sorry( this,
                            i18n("You must select a frame first."),
                            i18n("Format Frameset"));
}

void KWView::slotSpellCheck()
{
    if (m_spell.kospell) return; // Already in progress
    //m_doc->setReadWrite(false); // prevent editing text - not anymore
    m_spell.macroCmdSpellCheck = 0L;
    m_spell.replaceAll.clear();
    Q3ValueList<KoTextObject *> objects;
    KWTextFrameSetEdit * edit = currentTextEdit();
    if (!edit)
        return;
    int options = 0;
    if ( edit && edit->textFrameSet()->hasSelection() )
    {
        objects.append(edit->textFrameSet()->textObject());
        options = KFind::SelectedText;
    }
    else
    {
        objects = m_gui->canvasWidget()->kWordDocument()->visibleTextObjects(viewMode());
    }
    m_spell.textIterator = new KoTextIterator( objects, edit, options );
    kDebug()<<"Created iterator with "<< objects.count() <<endl;
    startKSpell();
}

void KWView::extraAutoFormat()
{
    m_doc->autoFormat()->readConfig();
    KoAutoFormatDia dia( this, 0, m_doc->autoFormat() );
    dia.exec();
    m_doc->startBackgroundSpellCheck(); // will do so if enabled
}

void KWView::extraFrameStylist()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->hideCursor();
    KWFrameStyleManager * frameStyleManager = new KWFrameStyleManager( this, m_doc, QString::null );
    frameStyleManager->exec();
    delete frameStyleManager;
    if ( edit )
        edit->showCursor();
}

void KWView::createFrameStyle()
{

    Q3ValueList<KWFrameView*> selectedFrames = frameViewManager()->selectedFrames();
    if (selectedFrames.count() != 1)
        return;

    KWFrame* frame = selectedFrames[0]->frame();

    KoCreateStyleDia *dia = new KoCreateStyleDia( m_doc->frameStyleCollection()->displayNameList(), this, 0 );
    if ( dia->exec() )
    {
        KWFrameStyle *style= new KWFrameStyle( dia->nameOfNewStyle(), frame );
        m_doc->frameStyleCollection()->addStyle( style );
        m_doc->updateAllFrameStyleLists();
    }
    delete dia;
}

void KWView::extraStylist()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    QString activeStyleName  = QString::null;
    if ( edit )
    {
        edit->hideCursor();
        if (edit->cursor() && edit->cursor()->parag() && edit->cursor()->parag()->style())
            activeStyleName = edit->cursor()->parag()->style()->displayName();
    }
    KWStyleManager * styleManager = new KWStyleManager( this, m_doc->unit(),m_doc, *m_doc->styleCollection(), activeStyleName );
    styleManager->exec();
    delete styleManager;
    if ( edit )
        edit->showCursor();
}

void KWView::extraCreateTemplate()
{
    int width = 60;
    int height = 60;
    QPixmap pix = m_doc->generatePreview(QSize(width, height));

    KTempFile tempFile( QString::null, ".odt" );
    tempFile.setAutoDelete(true);

    m_doc->saveNativeFormat( tempFile.name() );

    KoTemplateCreateDia::createTemplate( "kword_template", KWFactory::instance(),
                                         tempFile.name(), pix, this );

    KWFactory::instance()->dirs()->addResourceType("kword_template",
                                                    KStandardDirs::kde_default( "data" ) +
                                                    "kword/templates/");
}

void KWView::toolsCreateText()
{
    if ( m_actionToolsCreateText->isChecked() )
        m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_CREATE_TEXT );
    else
    {
        // clicked on the already active tool -> abort
        m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_EDIT );
    }
}

void KWView::insertTable()
{
    KWCanvas * canvas = m_gui->canvasWidget();
    canvas->setMouseMode( KWCanvas::MM_EDIT );
    KWTableDia *tableDia = new KWTableDia( this, 0, KWTableDia::NEW, canvas, m_doc,
                                           canvas->tableRows(),
                                           canvas->tableCols(),
                                           canvas->tableWidthMode(),
                                           canvas->tableHeightMode(),
                                           canvas->tableIsFloating(),
                                           canvas->tableTemplateName(),
                                           canvas->tableFormat());
    tableDia->setCaption( i18n( "Insert Table" ) );
    if ( tableDia->exec() == QDialog::Rejected )
        canvas->setMouseMode( KWCanvas::MM_EDIT );
    delete tableDia;
}

void KWView::insertFormula( QMimeSource* source )
{
    KWTextFrameSetEdit *edit = currentTextEdit();
    if (edit)
    {
        KWFormulaFrameSet *frameset = new KWFormulaFrameSet( m_doc, QString::null );
        m_doc->addFrameSet( frameset, false ); // done first since the frame number is stored in the undo/redo
        if ( source ) {
            QByteArray data = source->encodedData( KFormula::MimeSource::selectionMimeType() );
            QDomDocument formula;
            formula.setContent( data );
            QDomElement formulaElem = formula.namedItem("KFORMULA").toElement();
            frameset->paste( formulaElem );
        }
        KWFrame *frame = new KWFrame(frameset, 0, 0, 10, 10 );
        frame->setZOrder( m_doc->maxZOrder( frame->pageNumber(m_doc) ) + 1 ); // make sure it's on top
        frameset->addFrame( frame, false );
        edit->insertFloatingFrameSet( frameset, i18n("Insert Formula") );
        frameset->finalize(); // done last since it triggers a redraw
        m_doc->refreshDocStructure(FT_FORMULA);

        m_gui->canvasWidget()->editFrameSet( frameset );
        frameset->setChanged();
        m_gui->canvasWidget()->repaintChanged( frameset, true );
    }
}

void KWView::toolsPart()
{
    m_gui->canvasWidget()->insertPart( m_actionToolsCreatePart->documentEntry() );
}


int KWView::tableSelectCell(const QString &tableName, uint row, uint col)
{
    if(!m_doc || !m_gui)
        return -1;
    KWFrameSet *fs = m_doc->frameSetByName(tableName);
    if(!fs)
        return -1;
    KWTableFrameSet *table = dynamic_cast<KWTableFrameSet*>(fs);
    if(!table)
        return -1;
    if (row >= table->getRows() || col >= table->getColumns())
        return -1;

    KWTableFrameSet::Cell *cell = table->cell(row, col);

    KWCanvas *canvas = m_gui->canvasWidget();
    if(!canvas)
        return -1;
    canvas->tableSelectCell(table, cell);
    return 0;
}

int KWView::tableDeleteRow(const Q3ValueList<uint>& rows, KWTableFrameSet *table )
{
    if(!table)
        table = m_gui->canvasWidget()->getCurrentTable();

    if (!m_doc || !table)
        return -1;

    if(rows.count() >= table->getRows()) {
        m_doc->deleteTable(table);
        return 0;
    }

    KMacroCommand *macro = new KMacroCommand(i18n("Remove Rows"));

    for (int i = rows.count() - 1; i >= 0 ; i--) {
        KWRemoveRowCommand *cmd = new KWRemoveRowCommand( i18n("Remove Row"),
            table, rows[i] );
        macro->addCommand(cmd);
    }

    macro->execute();
    m_doc->addCommand(macro);
    return 0;
}

int KWView::tableDeleteCol(const Q3ValueList<uint>& cols, KWTableFrameSet *table)
{
    if(!table)
        table = m_gui->canvasWidget()->getCurrentTable();

    if (!m_doc || !table)
        return -1;

    if(cols.count() >= table->getColumns()) {
        m_doc->deleteTable(table);
        return 0;
    }

    KMacroCommand *macro = new KMacroCommand(i18n("Remove Columns"));

    for (int i = cols.count() - 1;  i >= 0; i--) {
        KWRemoveColumnCommand *cmd = new KWRemoveColumnCommand( i18n("Remove Column"),
            table, cols[i] );
        macro->addCommand(cmd);
    }

    macro->execute();
    m_doc->addCommand(macro);
    return 0;
}

void KWView::tableProperties()
{
    KWCanvas * canvas = m_gui->canvasWidget();
    KWTableFrameSet *table = canvas->getCurrentTable();
    if (table)
    {
        canvas->setMouseMode( KWCanvas::MM_EDIT );
        KWTableDia *tableDia = new KWTableDia( this, 0, KWTableDia::EDIT, canvas, m_doc,
                                               table->getRows(),
                                               table->getColumns(),
                                               canvas->tableWidthMode(),
                                               canvas->tableHeightMode(),
                                               canvas->tableIsFloating(),
                                               canvas->tableTemplateName(),
                                               canvas->tableFormat());
        tableDia->setCaption( i18n( "Adjust Table" ) );
        if ( tableDia->exec() == QDialog::Rejected )
            canvas->setMouseMode( KWCanvas::MM_EDIT );
        delete tableDia;
    }
}

void KWView::tableInsertRow()
{
    TableInfo ti(frameViewManager()->selectedFrames());
    KWTableFrameSet::Cell *cell = ti.firstSelectedCell();
    if(! cell) return;

    KWInsertDia dia( this, cell->groupmanager(), KWInsertDia::insertRow,  cell->firstRow());
    dia.exec();
}

void KWView::tableInsertRow(uint row, KWTableFrameSet *table)
{
    if(!table)
        table = m_gui->canvasWidget()->getCurrentTable();

    if (!m_doc || !table)
        return;

    if(row > table->getRows())
        return;

    KWInsertRowCommand *cmd = new KWInsertRowCommand( i18n("Insert Row"), table, row);
    cmd->execute();
    m_doc->addCommand(cmd);
}


void KWView::tableInsertCol()
{
    TableInfo ti(frameViewManager()->selectedFrames());
    KWTableFrameSet::Cell *cell = ti.firstSelectedCell();
    if(! cell) return;

    KWInsertDia dia( this, cell->groupmanager(), KWInsertDia::insertColumn,  cell->firstColumn());
    dia.exec();
}

void KWView::tableInsertCol(uint col,  KWTableFrameSet *table  )
{
    if(!table)
        table = m_gui->canvasWidget()->getCurrentTable();

    if (!m_doc || !table)
        return;

    if(col > table->getColumns())
        return;

    // we pass as last parameter the maximum offset that the table can use.
    // this offset is the max right offset of the containing frame in the case
    // of an inline (floating) table, the size of the page for other tables.
    double maxRightOffset;
    if (table->isFloating())    // inline table: max offset of containing frame
        maxRightOffset = table->anchorFrameset()->frame(0)->right();
    else {                      // non inline table: max offset of the page
        KWPage *page = m_doc->pageManager()->page( table->cell(0,0)->frame(0) );
        maxRightOffset = page->width() - page->rightMargin();
    }

    KWInsertColumnCommand *cmd = new KWInsertColumnCommand( i18n("Insert Column"),
        table, col,  maxRightOffset);
    cmd->execute();
    m_doc->addCommand(cmd);
}

void KWView::tableDeleteRow()
{
    TableInfo ti( frameViewManager()->selectedFrames() );
    if(ti.amountRowsSelected() == 0) return;

    KWDeleteDia dia( this, ti.firstSelectedCell()->groupmanager(),
            KWDeleteDia::deleteRow, ti.selectedRows() );
    dia.exec();
}

void KWView::tableDeleteCol()
{
    TableInfo ti( frameViewManager()->selectedFrames() );
    if(ti.amountColumnsSelected() == 0) return;

    KWDeleteDia dia( this, ti.firstSelectedCell()->groupmanager(),
            KWDeleteDia::deleteColumn, ti.selectedColumns() );
    dia.exec();
}

void KWView::tableResizeCol()
{
    TableInfo ti( frameViewManager()->selectedFrames() );
    KWTableFrameSet::Cell *cell = ti.firstSelectedCell();
    if(cell == 0)
        return;
    KWResizeTableDia dia(this, cell->groupmanager(), m_doc, cell->firstColumn());
    dia.exec();
}

void KWView::tableJoinCells()
{
    KWTableFrameSet *table = 0;
    Q3ValueList<KWFrameView*> selectedFrames = frameViewManager()->selectedFrames();
    Q3ValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
    unsigned int x1=10000, y1=10000, x2=0, y2=0;
    for(;framesIterator != selectedFrames.end(); ++framesIterator) {
        KWFrameView *view = *framesIterator;
        if(!view->selected()) continue;
        KWFrameSet *fs = view->frame()->frameSet();
        Q_ASSERT(fs);
        KWTableFrameSet::Cell *cell = dynamic_cast<KWTableFrameSet::Cell*>(fs);
        if(cell == 0) continue;
        if(!table)
            table = cell->groupmanager();
        else if(table != cell->groupmanager()) { // more then one table has selected cells
            KMessageBox::sorry( this,
                    i18n( "More then one table has selected cells, please make sure "
                        "the selected cells are in one table and are connecting"),
                    i18n( "Join Cells Failed" ) );
            return;
        }

        if(cell->firstRow() < y1)  y1 = cell->firstRow();
        if(cell->firstColumn() < x1)  x1 = cell->firstColumn();
        if(cell->lastRow() > y2) y2 = cell->lastRow();
        if(cell->lastColumn() > x2) x2 = cell->lastColumn();
    }

    Q_ASSERT(table);
    if (!table)
        return;
    KCommand * cmd=table->joinCells(x1, y1, x2, y2);
    if ( !cmd )
    {
        KMessageBox::sorry( this,
                            i18n( "You have to select some cells which are next to each other "
                                  "and are not already joined." ),
                            i18n( "Join Cells" ) );
        return;
    }
    m_doc->addCommand(cmd);
    m_doc->layout();
}

void KWView::tableSplitCells() {
    KWSplitCellDia *splitDia=new KWSplitCellDia( this,"split cell",
            m_tableSplit.columns, m_tableSplit.rows );
    if(splitDia->exec()) {
        m_tableSplit.rows = splitDia->rows();
        m_tableSplit.columns = splitDia->columns();
        tableSplitCells( m_tableSplit.columns, m_tableSplit.rows );
    }
    delete splitDia;
}

void KWView::tableSplitCells(int cols, int rows)
{
    TableInfo ti( frameViewManager()->selectedFrames() );
    if(! ti.oneCellSelected()) {
        KMessageBox::sorry( this,
                            i18n( "You have to put the cursor into a table "
                                  "before splitting cells." ),
                            i18n( "Split Cells" ) );
        return;
    }

    KWTableFrameSet::Cell *cell = ti.firstSelectedCell();
    KCommand *cmd=cell->groupmanager()->splitCell(rows, cols, cell->firstColumn(), cell->firstRow());
    if ( !cmd ) {
        KMessageBox::sorry( this,
                            i18n("There is not enough space to split the cell into that many parts, make it bigger first"),
                            i18n("Split Cells") );
        return;
    }
    m_doc->addCommand(cmd);
    m_doc->updateAllFrames();
    m_doc->layout();
    frameViewManager()->view(cell->frame(0))->setSelected(true);
}

void KWView::tableUngroupTable()
{
    m_gui->canvasWidget()->setMouseMode( KWCanvas::MM_EDIT );

    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    Q_ASSERT(table);
    if (!table)
        return;

    // Use a macro command because we may have to make the table non-floating first
    KMacroCommand * macroCmd = new KMacroCommand( i18n( "Ungroup Table" ) );

    if ( table->isFloating() )
    {
        KWFrameSetInlineCommand *cmd = new KWFrameSetInlineCommand( QString::null, table, false );
        macroCmd->addCommand(cmd);
    }

    KWUngroupTableCommand *cmd = new KWUngroupTableCommand( QString::null, table );
    macroCmd->addCommand( cmd );
    m_doc->addCommand( macroCmd );
    macroCmd->execute(); // do it all
}

void KWView::tableDelete()
{
    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    Q_ASSERT(table);
    if (!table)
        return;
    m_doc->deleteTable( table );
}

void KWView::tableStylist()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->hideCursor();
    KWTableStyleManager * tableStyleManager = new KWTableStyleManager( this, m_doc );
    tableStyleManager->exec();
    delete tableStyleManager;
    if ( edit )
        edit->showCursor();
}

void KWView::tableProtectCells(bool on)
{
    KMacroCommand *macro = 0;
    Q3ValueList<KWFrameView*> selectedFrames = frameViewManager()->selectedFrames();
    Q3ValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
    for(;framesIterator != selectedFrames.end(); ++framesIterator) {
        KWFrameView *view = *framesIterator;
        KWFrameSet *fs = view->frame()->frameSet();
        Q_ASSERT(fs);
        KWTableFrameSet::Cell *cell = dynamic_cast<KWTableFrameSet::Cell*>(fs);
        if(cell == 0) continue;
        if(cell->protectContent() != on) {
            KWProtectContentCommand *cmd = new KWProtectContentCommand( i18n("Protect Content"), cell , on);
            if ( !macro )
                macro = new KMacroCommand( i18n("Protect Content"));
            macro->addCommand( cmd );
        }
    }
    if(macro) {
        macro->execute();
        m_doc->addCommand( macro );
    }
}

void KWView::textStyleSelected( KoParagStyle *sty )
{
    if ( !sty )
        return;

    if ( m_gui->canvasWidget()->currentFrameSetEdit() )
    {
        KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(m_gui->canvasWidget()->currentFrameSetEdit()->currentTextEdit());
        if ( edit )
            edit->applyStyle( sty );
    }
    else
    { // it might be that a frame (or several frames) are selected
        Q3ValueList<KWFrameView*> selectedFrames = frameViewManager()->selectedFrames();
        if (selectedFrames.count() <= 0)
            return; // nope, no frames are selected.
        // yes, indeed frames are selected.

        Q3ValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
        KMacroCommand *globalCmd = 0L;
        while(framesIterator != selectedFrames.end()) {
            KWFrame *curFrame = (*framesIterator)->frame();
            KWFrameSet *curFrameSet = curFrame->frameSet();
            if (curFrameSet->type() == FT_TEXT)
            {
                KoTextObject *textObject = ((KWTextFrameSet*)curFrameSet)->textObject();
                textObject->textDocument()->selectAll( KoTextDocument::Temp );
                KCommand *cmd = textObject->applyStyleCommand( 0L, sty , KoTextDocument::Temp, KoParagLayout::All, KoTextFormat::Format, true, true );
                textObject->textDocument()->removeSelection( KoTextDocument::Temp );
                if (cmd)
                {
                    if ( !globalCmd )
                        globalCmd = new KMacroCommand( selectedFrames.count() == 1 ? i18n("Apply Style to Frame") : i18n("Apply Style to Frames"));
                    globalCmd->addCommand( cmd );
                }
            }
            ++framesIterator;
        }
        if ( globalCmd )
            m_doc->addCommand( globalCmd );
    }
    m_gui->canvasWidget()->setFocus(); // the combo keeps focus...*/

}

// Called by the above, and when selecting a style in the style combobox
void KWView::textStyleSelected( int index )
{
    textStyleSelected( m_doc->styleCollection()->styleAt( index ) );
}

// Slot is called when selecting a framestyle in the Frames / Framestyle menu
void KWView::slotFrameStyleSelected()
{
    QString actionName = QString::fromUtf8(sender()->name());
    const QString prefix = FRAMESTYLE_ACTION_PREFIX;
    if ( actionName.startsWith( prefix ) ) {
        actionName = actionName.mid( prefix.length() );
        frameStyleSelected( m_doc->frameStyleCollection()->findStyle( actionName ) );
    }
}

void KWView::frameStyleSelected( int index )
{
    frameStyleSelected( m_doc->frameStyleCollection()->frameStyleAt( index ) );
}


// Called by the above, and when selecting a style in the framestyle combobox
void KWView::frameStyleSelected( KWFrameStyle *sty )
{
    if ( !sty )
        return;

    if ( m_gui->canvasWidget()->currentFrameSetEdit() )
    {
        KWFrame * single = m_gui->canvasWidget()->currentFrameSetEdit()->currentFrame();
        if ( single ) {

            KCommand *cmd = new KWFrameStyleCommand( i18n("Apply Framestyle to Frame"), single, sty );
            if (cmd) {
                m_doc->addCommand( cmd );
                cmd->execute();
            }
        }
    }
    else
    { // it might be that a frame (or several frames) are selected
        Q3ValueList<KWFrameView*> selectedFrames = frameViewManager()->selectedFrames();
        if (selectedFrames.count() <= 0)
            return; // nope, no frames are selected.
        // yes, indeed frames are selected.

        KMacroCommand *globalCmd = new KMacroCommand( selectedFrames.count() == 1 ? i18n("Apply Framestyle to Frame") : i18n("Apply Framestyle to Frames"));

        Q3ValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
        while(framesIterator != selectedFrames.end()) {
            KWFrame *curFrame = (*framesIterator)->frame();
            KCommand *cmd = new KWFrameStyleCommand( i18n("Apply Framestyle"), curFrame, sty );
            if (cmd)
                globalCmd->addCommand( cmd );
            ++framesIterator;
        }
        m_doc->addCommand( globalCmd );
        globalCmd->execute();
    }

    m_gui->canvasWidget()->repaintAll();
    m_gui->canvasWidget()->setFocus(); // the combo keeps focus...*/

    // Adjust GUI
    const int pos = m_doc->frameStyleCollection()->indexOf( sty );
    Q_ASSERT( pos >= 0 );
    m_actionFrameStyle->setCurrentItem( pos );
    KToggleAction* act = dynamic_cast<KToggleAction *>(actionCollection()->action( sty->name().utf8() ));
    if ( act )
        act->setChecked( true );
}


// Called when selecting a tablestyle in the Table / Tablestyle menu
void KWView::slotTableStyleSelected()
{
    QString actionName = QString::fromUtf8(sender()->name());
    const QString prefix = TABLESTYLE_ACTION_PREFIX;
    if ( actionName.startsWith( prefix ) ) {
        actionName = actionName.mid( prefix.length() );
        tableStyleSelected( m_doc->tableStyleCollection()->findStyle( actionName ) );
    }
}

void KWView::tableStyleSelected( int index )
{
    tableStyleSelected( m_doc->tableStyleCollection()->tableStyleAt( index ) );
}

// Called by the above, and when selecting a style in the framestyle combobox
void KWView::tableStyleSelected( KWTableStyle *sty )
{
    if ( !sty )
        return;

    if ( m_gui->canvasWidget()->currentFrameSetEdit() )
    {
        KWFrame * single = m_gui->canvasWidget()->currentFrameSetEdit()->currentFrame();
        if ( (single) && ( single->frameSet()->type() == FT_TEXT ) )
        {
            KCommand *cmd = new KWTableStyleCommand( i18n("Apply Tablestyle to Frame"), single, sty );
            if (cmd) {
                m_doc->addCommand( cmd );
                cmd->execute();
            }
        }
    }
    else
    {
        Q3ValueList<KWFrameView*> selectedFrames = frameViewManager()->selectedFrames();
        if (selectedFrames.count() <= 0)
            return; // nope, no frames are selected.

        KMacroCommand *globalCmd = new KMacroCommand( selectedFrames.count() == 1 ? i18n("Apply Tablestyle to Frame") : i18n("Apply Tablestyle to Frames"));

        Q3ValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
        while(framesIterator != selectedFrames.end() ) {
            KWFrame *curFrame = (*framesIterator)->frame();
            if(dynamic_cast<KWTextFrameSet*>(curFrame->frameSet()))  {
                KCommand *cmd = new KWTableStyleCommand( i18n("Apply Tablestyle to Frame"),
                        curFrame, sty );
                if (cmd)
                    globalCmd->addCommand( cmd );
            }
            ++framesIterator;
        }
        m_doc->addCommand( globalCmd );
        globalCmd->execute();
    }

    m_gui->canvasWidget()->repaintAll();
    m_gui->canvasWidget()->setFocus(); // the combo keeps focus...*/

    // Adjust GUI
    int pos = m_doc->tableStyleCollection()->indexOf( sty );
    m_actionTableStyle->setCurrentItem( pos );
    KToggleAction* act = dynamic_cast<KToggleAction *>(actionCollection()->action( sty->name().utf8() ));
    if ( act )
        act->setChecked( true );
}

void KWView::increaseFontSize()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    KoTextFormat *format = edit->currentFormat();
    if ( edit )
        textSizeSelected( edit->textFrameSet()->textObject()->docFontSize( format ) + 1 );
}

void KWView::decreaseFontSize()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    KoTextFormat *format = edit->currentFormat();
    if ( edit )
        textSizeSelected( edit->textFrameSet()->textObject()->docFontSize( format ) - 1 );
}

void KWView::textSizeSelected( int size )
{
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand *globalCmd = new KMacroCommand(i18n("Change Text Size"));
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setPointSizeCommand( size );
        if (cmd)
            globalCmd->addCommand(cmd);
    }
    m_doc->addCommand(globalCmd);
    m_gui->canvasWidget()->setFocus(); // the combo keeps focus...
}

void KWView::textFontSelected( const QString & font )
{
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setFamilyCommand( font );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Change Text Font") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd )
        m_doc->addCommand( macroCmd );
    m_gui->canvasWidget()->setFocus(); // the combo keeps focus...
}

Q3PtrList<KoTextFormatInterface> KWView::applicableTextInterfaces() const
{
    Q3PtrList<KoTextFormatInterface> lst;
    if (currentTextEdit())
    {
        if ( !currentTextEdit()->textObject()->protectContent())
        {
            // simply return the current textEdit
            lst.append( currentTextEdit() );
            //kDebug() << "text frame name: " << currentTextEdit()->textFrameSet()->name() << endl;
            KWCollectFramesetsVisitor visitor;
            currentTextEdit()->textDocument()->visitSelection( KoTextDocument::Standard, &visitor ); //find all framesets in the selection
            const Q3ValueList<KWFrameSet *>& frameset = visitor.frameSets();
            for ( Q3ValueList<KWFrameSet *>::ConstIterator it = frameset.begin(); it != frameset.end(); ++it )
            {
                if ( (*it)->type() == FT_TABLE )
                {
                    KWTableFrameSet* kwtableframeset = static_cast<KWTableFrameSet *>( *it );
                    //kDebug() << "table found: " << kwtableframeset->frameCount() << endl;
                    int const rows  = kwtableframeset->getRows();
                    int const cols = kwtableframeset->getColumns();
                    //finding all cells and add them to the interface list
                    for (int r=0; r<rows; ++r)
                    {
                        for (int c=0; c<cols; ++c)
                        {
                            KWTableFrameSet::Cell *cell = kwtableframeset->cell(r,c);
                            if (cell)
                            {
                                kDebug() << "adding (" << r << "," << c << ")" << endl;
                                lst.append(cell);
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {   // it might be that a frame (or several frames) are selected
        // in that case, list the text framesets behind them
        Q3ValueList<KWFrameView*> selectedFrames = frameViewManager()->selectedFrames();
        Q3ValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
        while(framesIterator != selectedFrames.end()) {
            KWTextFrameSet* fs = dynamic_cast<KWTextFrameSet *>( (*framesIterator)->frame()->frameSet() );
            if ( fs && !lst.contains( fs )&& !fs->protectContent() )
                lst.append( fs );
            ++framesIterator;
        }
    }
    return lst;
}

void KWView::textBold()
{
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setBoldCommand( m_actionFormatBold->isChecked() );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Make Text Bold") );
            macroCmd->addCommand(cmd);
        }
    }
    if(macroCmd)
        m_doc->addCommand(macroCmd);

}

void KWView::textItalic()
{
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setItalicCommand( m_actionFormatItalic->isChecked() );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Make Text Italic") );
            macroCmd->addCommand( cmd );
        }
    }
    if( macroCmd)
        m_doc->addCommand( macroCmd );
}

void KWView::textUnderline()
{
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;

    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setUnderlineCommand( m_actionFormatUnderline->isChecked() );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Underline Text") );
            macroCmd->addCommand( cmd );
        }
    }
    if(macroCmd)
        m_doc->addCommand( macroCmd );
}

void KWView::textStrikeOut()
{
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setStrikeOutCommand( m_actionFormatStrikeOut->isChecked() );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Strike Out Text") );
            macroCmd->addCommand( cmd );
        }
    }
    if( macroCmd)
        m_doc->addCommand( macroCmd );
}

void KWView::textColor()
{
    /*        QColor color = edit->textColor();
              if ( KColorDialog::getColor( color ) ) {
              m_actionFormatColor->setColor( color );
              edit->setTextColor( color );
              }
    */
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setTextColorCommand( m_actionFormatColor->color() );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Set Text Color") );
            macroCmd->addCommand( cmd );
        }
    }
    if( macroCmd)
        m_doc->addCommand( macroCmd );
}

void KWView::textAlignLeft()
{
    if ( m_actionFormatAlignLeft->isChecked() )
    {
        Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
        Q3PtrListIterator<KoTextFormatInterface> it( lst );
        KMacroCommand* macroCmd = 0L;
        for ( ; it.current() ; ++it )
        {
            KCommand *cmd = it.current()->setAlignCommand( Qt::AlignLeft );
            if (cmd)
            {
                if ( !macroCmd)
                    macroCmd = new KMacroCommand( i18n("Left-Align Text") );
                macroCmd->addCommand( cmd );
            }
        }
        if( macroCmd )
            m_doc->addCommand( macroCmd );
    }
    else
        m_actionFormatAlignLeft->setChecked( true );
}

void KWView::textAlignCenter()
{
    if ( m_actionFormatAlignCenter->isChecked() )
    {
        Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
        Q3PtrListIterator<KoTextFormatInterface> it( lst );
        KMacroCommand* macroCmd = 0L;
        for ( ; it.current() ; ++it )
        {
            KCommand *cmd = it.current()->setAlignCommand( Qt::AlignHCenter );
            if (cmd)
            {
                if ( !macroCmd )
                    macroCmd = new KMacroCommand( i18n("Center Text") );
                macroCmd->addCommand( cmd );
            }
        }
        if( macroCmd )
            m_doc->addCommand( macroCmd );
    }
    else
        m_actionFormatAlignCenter->setChecked( true );
}

void KWView::textAlignRight()
{
    if ( m_actionFormatAlignRight->isChecked() )
    {
        Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
        Q3PtrListIterator<KoTextFormatInterface> it( lst );
        KMacroCommand* macroCmd = 0L;
        for ( ; it.current() ; ++it )
        {
            KCommand *cmd = it.current()->setAlignCommand( Qt::AlignRight );
            if ( cmd )
            {
                if (!macroCmd )
                    macroCmd = new KMacroCommand( i18n("Right-Align Text") );
                macroCmd->addCommand( cmd );
            }
        }
        if( macroCmd)
            m_doc->addCommand( macroCmd );
    }
    else
        m_actionFormatAlignRight->setChecked( true );
}

void KWView::textAlignBlock()
{
    if ( m_actionFormatAlignBlock->isChecked() )
    {
        Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
        Q3PtrListIterator<KoTextFormatInterface> it( lst );
        KMacroCommand* macroCmd = 0L;
        for ( ; it.current() ; ++it )
        {
            KCommand *cmd = it.current()->setAlignCommand( Qt::AlignJustify );
            if ( cmd )
            {
                if ( !macroCmd )
                    macroCmd = new KMacroCommand( i18n("Justify Text") );
                macroCmd->addCommand( cmd );
            }
        }
        if( macroCmd)
            m_doc->addCommand( macroCmd );
    }
    else
        m_actionFormatAlignBlock->setChecked( true );
}

void KWView::setSpacing( KoParagLayout::SpacingType spacing, const QString& commandName)
{
  Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
  if ( lst.isEmpty() ) return;
  Q3PtrListIterator<KoTextFormatInterface> it( lst );
  KMacroCommand* macroCmd = 0L;
  for ( ; it.current() ; ++it )
  {
    KoParagLayout layout( *it.current()->currentParagLayoutFormat() );
    layout.lineSpacingType = spacing;

    KCommand *cmd = it.current()->setParagLayoutFormatCommand( &layout, KoParagLayout::LineSpacing );
    if (cmd)
    {
      if ( !macroCmd )
        macroCmd = new KMacroCommand( commandName );
      macroCmd->addCommand(cmd);
    }
  }
  if( macroCmd)
    m_doc->addCommand(macroCmd);
}

void KWView::textSpacingSingle()
{
  if ( m_actionFormatSpacingSingle->isChecked() )
    setSpacing( KoParagLayout::LS_SINGLE, i18n("Set Single Line Spacing") );
  else
    m_actionFormatSpacingSingle->setChecked( true );
}

void KWView::textSpacingOneAndHalf()
{
  if ( m_actionFormatSpacingOneAndHalf->isChecked() )
    setSpacing( KoParagLayout::LS_ONEANDHALF, i18n("Set One and a Half Line Spacing") );
  else
    m_actionFormatSpacingOneAndHalf->setChecked( true );
}

void KWView::textSpacingDouble()
{
  if ( m_actionFormatSpacingDouble->isChecked() )
    setSpacing( KoParagLayout::LS_DOUBLE, i18n("Set Double Line Spacing") );
  else
    m_actionFormatSpacingDouble->setChecked( true );
}

void KWView::slotCounterStyleSelected()
{
    QString actionName = QString::fromLatin1(sender()->name());
    QString styleStr = actionName.mid(13);
    //kDebug() << "KWView::slotCounterStyleSelected styleStr=" << styleStr << endl;
    KoParagCounter::Style style = (KoParagCounter::Style)(styleStr.toInt());
    KoParagCounter c;
    if ( style == KoParagCounter::STYLE_NONE )
        c.setNumbering( KoParagCounter::NUM_NONE );
    else {
        c.setNumbering( KoParagCounter::NUM_LIST );
        c.setStyle( style );
        if ( c.isBullet() )
            c.setSuffix( QString::null );
        else
            c.setSuffix( "." );
        // TODO save this setting, to use the last one selected in the dialog?
        // (same for custom bullet char etc.)

        // 68927: restart numbering, by default, if last parag wasn't numbered
        // (and if we're not applying this to a selection)
        if ( currentTextEdit() && !currentTextEdit()->textFrameSet()->hasSelection() ) {
            KoTextParag* parag = currentTextEdit()->cursor()->parag();
            if ( parag->prev() && !parag->prev()->counter() )
                c.setRestartCounter(true);
        }
    }

    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setCounterCommand( c );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd=new KMacroCommand( i18n("Change List Type") );
            macroCmd->addCommand( cmd );
        }
    }
    if( macroCmd)
        m_doc->addCommand( macroCmd );
}

void KWView::textSuperScript()
{
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setTextSuperScriptCommand(m_actionFormatSuper->isChecked());
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Make Text Superscript") );
            macroCmd->addCommand(cmd);
        }
    }
    if( macroCmd)
        m_doc->addCommand(macroCmd);
    if (m_actionFormatSuper->isChecked() )
        m_actionFormatSub->setChecked( false );
}

void KWView::textSubScript()
{
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setTextSubScriptCommand(m_actionFormatSub->isChecked());
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Make Text Subscript") );
            macroCmd->addCommand(cmd);
        }
    }
    if( macroCmd )
        m_doc->addCommand(macroCmd);
    if (m_actionFormatSub->isChecked() )
        m_actionFormatSuper->setChecked( false );
}

void KWView::changeCaseOfText()
{
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KoChangeCaseDia *caseDia=new KoChangeCaseDia( this,"change case" );
    if(caseDia->exec())
    {
        KMacroCommand* macroCmd = 0L;
        for ( ; it.current() ; ++it )
        {
            KCommand *cmd = it.current()->setChangeCaseOfTextCommand(caseDia->getTypeOfCase());
            if (cmd)
            {
                if ( !macroCmd )
                    macroCmd = new KMacroCommand( i18n("Change Case of Text") );
                macroCmd->addCommand(cmd);
            }
        }
        if( macroCmd )
            m_doc->addCommand(macroCmd);
    }
    delete caseDia;
}

void KWView::editPersonalExpr()
{
   KWEditPersonnalExpression *personalDia=new KWEditPersonnalExpression( this );
   if(personalDia->exec())
       m_doc->refreshMenuExpression();
   delete personalDia;
}


void KWView::textIncreaseIndent()
{
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    double leftMargin=0.0;
    if(!lst.isEmpty())
        leftMargin=lst.first()->currentParagLayoutFormat()->margins[Q3StyleSheetItem::MarginLeft];
    double indent = m_doc->indentValue();
    double newVal = leftMargin + indent;
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setMarginCommand( Q3StyleSheetItem::MarginLeft, newVal );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Increase Paragraph Depth") );
            macroCmd->addCommand(cmd);
        }
    }
    if( macroCmd)
        m_doc->addCommand(macroCmd);
    if(!lst.isEmpty())
    {
        const KoParagLayout *layout=lst.first()->currentParagLayoutFormat();
        showRulerIndent( layout->margins[Q3StyleSheetItem::MarginLeft], layout->margins[Q3StyleSheetItem::MarginFirstLine], layout->margins[Q3StyleSheetItem::MarginRight], lst.first()->rtl());
    }
#if 0


    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        double leftMargin = edit->currentLeftMargin();
        double indent = m_doc->indentValue();
        double newVal = leftMargin + indent;
        // Test commented out. This breaks with the DTP case... The user can put
        // a frame anywhere, even closer to the edges than left/right border allows (DF).
        //if( newVal <= (m_doc->ptPaperWidth()-m_doc->ptRightBorder()-m_doc->ptLeftBorder()))
        {
            KCommand *cmd=edit->setMarginCommand( Q3StyleSheetItem::MarginLeft, newVal );
            if(cmd)
                m_doc->addCommand(cmd);
        }
    }
#endif
}

void KWView::textDecreaseIndent()
{
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    double leftMargin=0.0;
    if(!lst.isEmpty())
        leftMargin=lst.first()->currentParagLayoutFormat()->margins[Q3StyleSheetItem::MarginLeft];
    double indent = m_doc->indentValue();
    double newVal = leftMargin - indent;
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setMarginCommand( Q3StyleSheetItem::MarginLeft, qMax( newVal, 0 ) );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Decrease Paragraph Depth") );
            macroCmd->addCommand(cmd);
        }
    }
    if( macroCmd)
        m_doc->addCommand(macroCmd);
    if(!lst.isEmpty())
    {
        const KoParagLayout *layout=lst.first()->currentParagLayoutFormat();
        showRulerIndent( layout->margins[Q3StyleSheetItem::MarginLeft], layout->margins[Q3StyleSheetItem::MarginFirstLine], layout->margins[Q3StyleSheetItem::MarginRight], lst.first()->rtl());
    }


#if 0
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        double leftMargin = edit->currentLeftMargin();
        if ( leftMargin > 0 )
        {
            double indent = m_doc->indentValue();
            double newVal = leftMargin - indent;
            KCommand *cmd=edit->setMarginCommand( Q3StyleSheetItem::MarginLeft, qMax( newVal, 0 ) );
            if(cmd)
                m_doc->addCommand(cmd);
        }
    }
#endif
}


void KWView::textDefaultFormat()
{
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setDefaultFormatCommand();
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Default Format") );
            macroCmd->addCommand(cmd);
        }
    }
    if( macroCmd)
        m_doc->addCommand(macroCmd);
}


void KWView::borderOutline()
{
    bool on = m_actionBorderOutline->isChecked();
    if(m_actionBorderLeft->isChecked() != on) {
        m_actionBorderLeft->setChecked(on);
        borderLeft();
    }
    if(m_actionBorderRight->isChecked() != on) {
        m_actionBorderRight->setChecked(on);
        borderRight();
    }
    if(m_actionBorderTop->isChecked() != on) {
        m_actionBorderTop->setChecked(on);
        borderTop();
    }
    if(m_actionBorderBottom->isChecked() != on) {
        m_actionBorderBottom->setChecked(on);
        borderBottom();
    }
}

void KWView::borderLeft() {
    borderChanged(KoBorder::LeftBorder);
}

void KWView::borderRight() {
    borderChanged(KoBorder::RightBorder);
}

void KWView::borderTop() {
    borderChanged(KoBorder::TopBorder);
}

void KWView::borderBottom() {
    borderChanged(KoBorder::BottomBorder);
}

void KWView::borderChanged(KoBorder::BorderType type) {
    KoBorder border;
    border.setPenWidth( m_actionBorderWidth->currentText().toInt() );
    border.color = m_actionBorderColor->color();
    border.setStyle( KoBorder::getStyle( m_actionBorderStyle->currentText() ) );
    bool enabled = false;
    QString comment;
    switch(type) {
        case KoBorder::LeftBorder:
            enabled = m_actionBorderLeft->isChecked();
            comment = i18n("Change Left Frame Border");
            break;
        case KoBorder::RightBorder:
            enabled = m_actionBorderRight->isChecked();
            comment = i18n("Change Right Frame Border");
            break;
        case KoBorder::TopBorder:
            enabled = m_actionBorderTop->isChecked();
            comment = i18n("Change Top Frame Border");
            break;
        case KoBorder::BottomBorder:
        default:
            enabled = m_actionBorderBottom->isChecked();
            comment = i18n("Change Bottom Frame Border");
            break;
    }
    if(!enabled)
        border.setPenWidth(0);

    KCommand *cmd = 0;
    KWTextFrameSetEdit *edit = currentTextEdit();
    if ( edit ) {
        KoBorder left = edit->border(KoBorder::LeftBorder);
        KoBorder right = edit->border(KoBorder::RightBorder);
        KoBorder top = edit->border(KoBorder::TopBorder);
        KoBorder bottom = edit->border(KoBorder::BottomBorder);
        if(type == KoBorder::LeftBorder)
            left = border;
        else if(type == KoBorder::RightBorder)
            right = border;
        else if(type == KoBorder::TopBorder)
            top = border;
        else
            bottom = border;
        cmd=edit->setBordersCommand( left, right, top, bottom );
    }
    else {
        Q3PtrList<FrameIndex> indexes;
        Q3PtrList<KWFrameBorderCommand::FrameBorderTypeStruct> borders;
        Q3ValueList<KWFrameView*> selectedFrames = frameViewManager()->selectedFrames();
        Q3ValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
        for(;framesIterator != selectedFrames.end(); ++framesIterator) {
            if( !(*framesIterator)->selected() ) continue;
            KWFrame *frame = (*framesIterator)->frame();
            indexes.append(new FrameIndex( frame ));
            KWFrameBorderCommand::FrameBorderTypeStruct *bts =
                new KWFrameBorderCommand::FrameBorderTypeStruct;
            bts->m_EFrameType = type;
            if(type == KoBorder::LeftBorder)
                bts->m_OldBorder=frame->leftBorder();
            else if(type == KoBorder::RightBorder)
                bts->m_OldBorder=frame->rightBorder();
            else if(type == KoBorder::TopBorder)
                bts->m_OldBorder=frame->topBorder();
            else
                bts->m_OldBorder=frame->bottomBorder();
            borders.append(bts);
        }
        if(indexes.count() > 0)
            cmd=new KWFrameBorderCommand(comment, indexes, borders, border);
    }
    if(cmd) {
        cmd->execute();
        m_doc->addCommand( cmd );
    }
    // update actions.
    m_actionBorderOutline->setChecked(
            m_actionBorderLeft->isChecked() && m_actionBorderRight->isChecked() &&
            m_actionBorderTop->isChecked() && m_actionBorderBottom->isChecked());
}

void KWView::backgroundColor()
{
    QColor backColor = m_actionBackgroundColor->color();
    // ### TODO port to applicableTextInterfaces ? Hmm, careful with the "frame" case.
    KWTextFrameSetEdit *edit = currentTextEdit();
    if ( m_gui)
    {
        if(edit)
        {
            KCommand *cmd=edit->setTextBackgroundColorCommand(backColor);
            if( cmd)
                m_doc->addCommand( cmd );
        }
        else
            m_gui->canvasWidget()->setFrameBackgroundColor( backColor );
    }
}

void KWView::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    if ( m_gui )
    {
        m_gui->resize( width(), height() );
        QString s = m_actionViewZoom->currentText();
        if ( !KoZoomMode::isConstant(s) )
            viewZoom( s );
    }
}

void KWView::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
    if ( ev->activated() )
    {
        initGui();
        if (m_doc->isEmbedded() ) {
            // Get zoom level from KoView, i.e. from the parent view
            const int zoom = qRound( KoView::zoom() * 100 );
            setZoom( zoom, true );
            showZoom( zoom );
        }
    }
    KoView::guiActivateEvent( ev );
}

void KWView::tabListChanged( const KoTabulatorList & tabList )
{
    if(!m_doc->isReadWrite())
        return;
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setTabListCommand( tabList );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand(i18n("Change Tabulator") );
            macroCmd->addCommand(cmd);
        }
    }
    if(macroCmd)
        m_doc->addCommand(macroCmd);
}

void KWView::newPageLayout( const KoPageLayout &layout )
{
    QString mode = viewMode()->type();
    bool state = (mode!="ModeText");
    if ( !state )
        return;

    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter hf;
    m_doc->getPageLayout( pgLayout, cl, hf );

    if(layout==pgLayout)
        return;

    KWPageLayoutStruct oldLayout( pgLayout, cl, hf );

    m_doc->setPageLayout( layout, cl, hf );

    KWPageLayoutStruct newLayout( layout, cl, hf );

    KWTextFrameSetEdit *edit = currentTextEdit();
    if (edit)
        edit->textFrameSet()->clearUndoRedoInfo();
    KCommand *cmd = new KWPageLayoutCommand( i18n("Change Layout"), m_doc, oldLayout, newLayout );
    m_doc->addCommand(cmd);
}

void KWView::slotPageLayoutChanged( const KoPageLayout& layout )
{
    // This is connected to a signal of KWDocument, so that when the
    // above method, or any other way of changing the page layout happens,
    // the rulers are updated in all views.
    viewMode()->setPageLayout( m_gui->getHorzRuler(), m_gui->getVertRuler(), layout );
    m_gui->canvasWidget()->repaintAll();
}

void KWView::newFirstIndent( double firstIndent )
{
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setMarginCommand( Q3StyleSheetItem::MarginFirstLine, firstIndent );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand(i18n("Change First Line Indent"));
            macroCmd->addCommand(cmd);
        }
    }
    if(macroCmd)
        m_doc->addCommand(macroCmd);
}

void KWView::newLeftIndent( double leftIndent )
{
    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setMarginCommand( Q3StyleSheetItem::MarginLeft, leftIndent );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand(i18n("Change Indent") );
            macroCmd->addCommand(cmd);
        }
    }
    if(macroCmd)
        m_doc->addCommand(macroCmd);

}

void KWView::newRightIndent( double rightIndent)
{

    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    Q3PtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it )
    {
        KCommand *cmd = it.current()->setMarginCommand( Q3StyleSheetItem::MarginRight, rightIndent );
        if (cmd)
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand(i18n("Change Indent") );
            macroCmd->addCommand(cmd);
        }
    }
    if(macroCmd)
        m_doc->addCommand(macroCmd);
}

Q3PopupMenu * KWView::popupMenu( const QString& name )
{
    // factory() is 0 when right-clicking on the kword document while
    // an embedded object is active. KoPartManager lets the click through,
    // without activating us - so let's do that first.
    if ( !factory() )
        partManager()->setActivePart( m_doc, this );
    Q_ASSERT( factory() );
    if ( factory() )
        return ((Q3PopupMenu*)factory()->container( name, this ));
    return 0;
}

void KWView::startKSpell()
{
    if ( !m_spell.kospell )
        m_spell.kospell = new KoSpell( m_broker, this  );

    // Spell-check the next paragraph
    Q_ASSERT( m_spell.textIterator );

    m_spell.kospell->check( m_spell.textIterator, true );
    delete m_spell.dlg;
    m_spell.dlg = new KSpell2::Dialog( m_spell.kospell, this );
    m_spell.dlg->activeAutoCorrect( true );

    QObject::connect( m_spell.dlg, SIGNAL(misspelling(const QString&, int)),
                      this, SLOT(spellCheckerMisspelling(const QString&, int)) );
    QObject::connect( m_spell.dlg, SIGNAL(replace(const QString&, int, const QString&)),
                      this, SLOT(spellCheckerCorrected(const QString&, int, const QString&)) );
    QObject::connect( m_spell.dlg, SIGNAL(done(const QString&) ),
                      this, SLOT(spellCheckerDone(const QString&)) );
    QObject::connect( m_spell.dlg, SIGNAL(cancel() ),
                      this, SLOT( spellCheckerCancel() ) );
    QObject::connect( m_spell.dlg, SIGNAL(autoCorrect(const QString &, const QString & ) ),
                      this, SLOT( spellAddAutoCorrect (const QString &, const QString &) ) );

    m_spell.dlg->show();
    //clearSpellChecker();
}

void KWView::spellCheckerMisspelling( const QString &old, int pos )
{
    //kDebug(32001) << "KWView::spellCheckerMisspelling old=" << old << " pos=" << pos << endl;
    KoTextObject* textobj = m_spell.kospell->currentTextObject();
    KoTextParag* parag = m_spell.kospell->currentParag();
    Q_ASSERT( textobj );
    Q_ASSERT( parag );
    if ( !textobj || !parag ) return;
    KWTextDocument *textdoc=static_cast<KWTextDocument *>( textobj->textDocument() );
    Q_ASSERT( textdoc );
    if ( !textdoc ) return;
    pos += m_spell.kospell->currentStartIndex();
    kDebug(32001) << "KWView::spellCheckerMisspelling parag=" << parag->paragId() << " pos=" << pos << " length=" << old.length() << endl;
    textdoc->textFrameSet()->highlightPortion( parag, pos, old.length(), m_gui->canvasWidget() );
}

void KWView::spellCheckerCorrected( const QString &old, int pos , const QString &corr )
{
    //kDebug(32001) << "KWView::spellCheckerCorrected old=" << old << " corr=" << corr << " pos=" << pos << endl;
    KoTextObject* textobj = m_spell.kospell->currentTextObject();
    KoTextParag* parag = m_spell.kospell->currentParag();
    Q_ASSERT( textobj );
    Q_ASSERT( parag );
    if ( !textobj || !parag ) return;
    KWTextDocument *textdoc=static_cast<KWTextDocument *>( textobj->textDocument() );
    Q_ASSERT( textdoc );
    if ( !textdoc ) return;
    pos += m_spell.kospell->currentStartIndex();
    textdoc->textFrameSet()->highlightPortion( parag, pos, old.length(), m_gui->canvasWidget() );

    KoTextCursor cursor( textdoc );
    cursor.setParag( parag );
    cursor.setIndex( pos );
    if(!m_spell.macroCmdSpellCheck)
        m_spell.macroCmdSpellCheck=new KMacroCommand(i18n("Correct Misspelled Word"));
    m_spell.macroCmdSpellCheck->addCommand(textobj->replaceSelectionCommand(
        &cursor, corr, QString::null, KoTextDocument::HighlightSelection));
}

void KWView::spellCheckerDone( const QString & )
{
    //kDebug(32001) << "KWView::spellCheckerDone" << endl;
    KWTextDocument *textdoc=static_cast<KWTextDocument *>( m_spell.kospell->textDocument() );
    Q_ASSERT( textdoc );
    if ( textdoc )
        textdoc->textFrameSet()->removeHighlight();

    clearSpellChecker();
}

void KWView::clearSpellChecker(bool cancelSpellCheck)
{
    kDebug(32001) << "KWView::clearSpellChecker" << endl;

    delete m_spell.textIterator;
    m_spell.textIterator = 0L;
    delete m_spell.kospell;
    m_spell.kospell = 0;

    if ( m_spell.macroCmdSpellCheck )
    {
        if ( !cancelSpellCheck )
            m_doc->addCommand(m_spell.macroCmdSpellCheck);
        else
        {
            //reverte all changes
            m_spell.macroCmdSpellCheck->unexecute();
            delete m_spell.macroCmdSpellCheck;
        }
    }
    m_spell.macroCmdSpellCheck=0L;
    m_spell.replaceAll.clear();
    //m_doc->setReadWrite(true);
}

void KWView::spellCheckerCancel()
{
    kDebug()<<"void KWView::spellCheckerCancel() \n";
    spellCheckerRemoveHighlight();
    //we add command :( => don't add command and reverte changes
    clearSpellChecker(true);
}


void KWView::spellCheckerRemoveHighlight()
{
    KoTextObject* textobj = m_spell.kospell->currentTextObject();
    if ( textobj ) {
        KWTextDocument *textdoc=static_cast<KWTextDocument *>( textobj->textDocument() );
        if ( textdoc )
            textdoc->textFrameSet()->removeHighlight();
    }
    KWTextFrameSetEdit * edit = currentTextEdit();
    if (edit)
        edit->drawCursor( TRUE );
}

void KWView::spellAddAutoCorrect (const QString & originalword, const QString & newword)
{
    m_doc->autoFormat()->addAutoFormatEntry( originalword, newword );
}

void KWView::configure()
{
    KWConfig configDia( this );
    configDia.exec();
}

KWTextFrameSetEdit *KWView::currentTextEdit() const
{
    if (!m_gui)
        return 0L;
    KWFrameSetEdit * edit = m_gui->canvasWidget()->currentFrameSetEdit();
    if ( edit )
        return dynamic_cast<KWTextFrameSetEdit *>(edit->currentTextEdit());
    return 0L;
}

void KWView::slotFrameSetEditChanged()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    bool rw = koDocument()->isReadWrite();
    bool selectedFrames = frameViewManager()->selectedFrames().count() > 0;
    bool hasSelection = false;
    if ( edit ) {
        hasSelection = edit->textFrameSet()->hasSelection();
        if ( edit->textFrameSet()->textObject()->protectContent())
            rw = false;
    }
    else {
        KWFrameSetEdit * e = m_gui->canvasWidget()->currentFrameSetEdit();
        if ( e && e->frameSet()->type() == FT_FORMULA ) {
            hasSelection = true;
        }
    }
    m_actionEditCut->setEnabled( hasSelection && rw );

    m_actionEditCopy->setEnabled( hasSelection );
    m_actionEditReplace->setEnabled( /*edit &&*/ rw );
    clipboardDataChanged(); // for paste

    bool state = (edit != 0 | selectedFrames) && rw;
    m_actionEditSelectAll->setEnabled(state);
    m_actionEditSelectCurrentFrame->setEnabled(state);
    m_actionInsertComment->setEnabled( state );
    m_actionFormatDefault->setEnabled( state );
    m_actionFormatFont->setEnabled( state );
    m_actionFormatFontSize->setEnabled( state );
    m_actionFormatFontFamily->setEnabled( state );
    m_actionAddBookmark->setEnabled(state);
    m_actionBackgroundColor->setEnabled( state );
    m_actionFormatStyleMenu->setEnabled( state );
    m_actionFormatBold->setEnabled( state );
    m_actionFormatItalic->setEnabled( state );
    m_actionFormatUnderline->setEnabled( state );
    m_actionFormatStrikeOut->setEnabled( state );
    m_actionFormatColor->setEnabled( state );
    m_actionFormatAlignLeft->setEnabled( state );
    m_actionFormatAlignCenter->setEnabled( state );
    m_actionFormatAlignRight->setEnabled( state );
    m_actionFormatAlignBlock->setEnabled( state );

    m_actionBorderLeft->setEnabled( state );
    m_actionBorderRight->setEnabled( state );
    m_actionBorderTop->setEnabled( state );
    m_actionBorderBottom->setEnabled( state );
    m_actionBorderOutline->setEnabled( state );
    m_actionBorderColor->setEnabled( state );
    m_actionBorderWidth->setEnabled( state );
    m_actionBorderStyle->setEnabled( state );


    //m_actionFormatIncreaseIndent->setEnabled(state);
    m_actionInsertLink->setEnabled(state);
    m_actionCreateStyleFromSelection->setEnabled( state /*&& hasSelection*/);
    m_actionConvertToTextBox->setEnabled( state && hasSelection);
    m_actionAddPersonalExpression->setEnabled( state && hasSelection);
    m_actionSortText->setEnabled( state && hasSelection);
    bool goodleftMargin=false;
    if(state && edit)
        goodleftMargin=(edit->currentLeftMargin()>0);

    m_actionFormatDecreaseIndent->setEnabled(goodleftMargin);
    bool isFootNoteSelected = ((rw && edit && !edit->textFrameSet()->isFootEndNote())||(!edit&& rw));
    m_actionFormatBullet->setEnabled(isFootNoteSelected);
    m_actionFormatNumber->setEnabled(isFootNoteSelected);
    m_actionFormatStyle->setEnabled(isFootNoteSelected);
    m_actionFormatSpacingSingle->setEnabled(rw);
    m_actionFormatSpacingOneAndHalf->setEnabled(rw);
    m_actionFormatSpacingDouble->setEnabled(rw);
    m_actionFormatSuper->setEnabled(rw);
    m_actionFormatSub->setEnabled(rw);
    m_actionFormatParag->setEnabled(state);
    m_actionInsertSpecialChar->setEnabled(state);
    m_actionSpellCheck->setEnabled(state);

    m_actionChangeCase->setEnabled( (rw && !edit)|| (state && hasSelection) );

    if ( edit && edit->textFrameSet()->protectContent())
    {
        m_actionChangeCase->setEnabled( false );
        m_actionEditCut->setEnabled( false );
    }
    else
        m_actionChangeCase->setEnabled( true );

    updateTableActions( frameViewManager()->selectedFrames() ) ;

    m_actionInsertFormula->setEnabled(state && (viewMode()->type()!="ModeText"));
    actionInsertVariable->setEnabled(state);
    m_actionInsertExpression->setEnabled(state);

    changeFootEndNoteState();
    //frameset different of header/footer
    state= state && edit && edit->frameSet() && !edit->frameSet()->isHeaderOrFooter() && !edit->frameSet()->groupmanager() && !edit->frameSet()->isFootEndNote();
    m_actionInsertContents->setEnabled(state);
    m_actionInsertFrameBreak->setEnabled( state );
    updatePageInfo();
}

void KWView::changeFootEndNoteState()
{
    bool rw = koDocument()->isReadWrite();
    KWTextFrameSetEdit * edit = currentTextEdit();
    QString mode = viewMode()->type();

    bool isEditableFrameset = edit && edit->frameSet() && edit->frameSet()->isMainFrameset();
    bool ok = rw && isEditableFrameset && (mode!="ModeText");
    m_actionInsertFootEndNote->setEnabled( ok );
    m_actionEditFootEndNote->setEnabled( ok );
}

void KWView::changeFootNoteMenuItem( bool footnote)
{
    m_actionEditFootEndNote->setText( footnote? i18n("Edit Footnote"): i18n("Edit Endnote"));
    m_actionChangeFootNoteType->setText( footnote? i18n("Change Footnote Parameter"):i18n("Change Endnote Parameter"));
}

void KWView::slotUpdateRuler()
{
    KWCanvas* canvas = m_gui->canvasWidget();
    QRect r( canvas->viewMode()->rulerFrameRect() );
    if ( !r.isNull() )
    {
        m_gui->getHorzRuler()->setFrameStartEnd( r.left(), r.right() );
        m_gui->getVertRuler()->setFrameStartEnd( r.top(), r.bottom() );
    }
    canvas->updateRulerOffsets();
}

void KWView::frameSelectedChanged()
{
    bool rw = koDocument()->isReadWrite();
    Q3ValueList<KWFrameView*> selectedFrames = frameViewManager()->selectedFrames();

    m_actionFormatFrameSet->setEnabled( selectedFrames.count() >= 1 );
    if ( rw && selectedFrames.count() >= 1 )
    {
        bool okForDelete = true;
        bool okForCopy = true;
        bool okForLowerRaise = false;
        bool okForChangeParagStyle = true;
        bool okForChangeInline = true;
        bool containsCellFrame = false;
        bool containsMainFrame = false;

        Q3ValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
        while(framesIterator != selectedFrames.end() && (okForDelete || okForLowerRaise ||
                    okForChangeParagStyle || okForChangeInline) ) {
            // Check we selected no footer nor header
            bool isFootNote = (*framesIterator)->frame()->frameSet()->isFootEndNote();
            bool headerFooterFootNote = isFootNote ||
                (*framesIterator)->frame()->frameSet()->isHeaderOrFooter();
            bool isMainWPFrame = (*framesIterator)->frame()->frameSet()->isMainFrameset();
            okForChangeParagStyle &= !isFootNote;
            okForCopy &= !headerFooterFootNote;

            okForDelete &= !headerFooterFootNote;
            okForDelete &= !isMainWPFrame;

            // Check we selected a frame we can lower raise.
            // The header, footer, main frameset, footnotes and inline frames can't be raised.
            // As soon as we find one who we can lower/raise open the option.
            okForLowerRaise |= !(isMainWPFrame || headerFooterFootNote || (*framesIterator)->frame()->frameSet()->isFloating());
            okForChangeInline &= !(isMainWPFrame || headerFooterFootNote );

            if ( (*framesIterator)->frame()->frameSet()->groupmanager() )
                containsCellFrame = true;
            if ( isMainWPFrame )
                containsMainFrame = true;
            ++framesIterator;
        }
        m_actionEditDelFrame->setEnabled( okForDelete );
        m_actionEditCut->setEnabled( okForDelete && !containsCellFrame );
        m_actionEditCopy->setEnabled( selectedFrames.count() >= 1 && okForCopy && !containsMainFrame && !containsCellFrame);

        m_actionLowerFrame->setEnabled( okForLowerRaise );
        m_actionRaiseFrame->setEnabled( okForLowerRaise );
        m_actionSendBackward->setEnabled( okForLowerRaise );
        m_actionBringToFront->setEnabled( okForLowerRaise );
        m_actionFormatBullet->setEnabled( okForChangeParagStyle );
        m_actionFormatNumber->setEnabled( okForChangeParagStyle );
        m_actionFormatStyle->setEnabled( okForChangeParagStyle);
        m_actionInlineFrame->setEnabled( okForChangeInline);

        KWFrame *frame = selectedFrames[0]->frame();
        updateBorderButtons(frame->leftBorder(), frame->rightBorder(), frame->topBorder(),
                frame->bottomBorder());
    } else
    {   // readonly document, or no frame selected -> disable
        m_actionEditDelFrame->setEnabled( false );
        m_actionInlineFrame->setEnabled(false);
        m_actionEditCut->setEnabled( false );
        m_actionLowerFrame->setEnabled( false );
        m_actionRaiseFrame->setEnabled( false );
        m_actionSendBackward->setEnabled( false );
        m_actionBringToFront->setEnabled( false );

    }
    bool frameDifferentOfPart=false;
    if(selectedFrames.count() >= 1)
    {
        Q3ValueListIterator<KWFrameView*> framesIterator = selectedFrames.begin();
        while(framesIterator != selectedFrames.end()) {
            if ( (*framesIterator)->frame()->frameSet()->type()!=FT_PART &&
                    (*framesIterator)->frame()->frameSet()->type()!= FT_PICTURE) {
                frameDifferentOfPart=true;
                break;
            }
            ++framesIterator;
        }
    }

    m_actionBackgroundColor->setEnabled( (selectedFrames.count() >= 1) && frameDifferentOfPart);
    m_actionBackgroundColor->setText(i18n("Frame Background Color..."));

    if ( frameDifferentOfPart ) {
        KWFrame *frame = selectedFrames[0]->frame();
        QColor frameCol=frame->backgroundColor().color();
        //m_actionBackgroundColor->setText(i18n("Frame Background Color..."));
        m_actionBackgroundColor->setCurrentColor( frameCol.isValid()? frame->backgroundColor().color() :  QApplication::palette().color( QPalette::Active, QColorGroup::Base ));
    }

    m_actionCreateFrameStyle->setEnabled( selectedFrames.count()==1 );
    m_actionCreateLinkedFrame->setEnabled( selectedFrames.count()==1 );

    updateTableActions( selectedFrames );
    updatePageInfo(); // takes care of slotUpdateRuler()
    updateFrameStatusBarItem();

    Q3PtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( !lst.isEmpty() )
    {
        Q3PtrListIterator<KoTextFormatInterface> it( lst );
        KoTextFormat format=*(lst.first()->currentFormat());
        showFormat( format );

        const KoParagLayout * paragLayout=lst.first()->currentParagLayoutFormat();
        KoParagCounter counter;
        if(paragLayout->counter)
            counter = *(paragLayout->counter);
        showCounter( counter );
        int align = paragLayout->alignment;
        if ( align == Qt::AlignLeft )
            align = Qt::AlignLeft; // ## seems hard to detect RTL here
        showAlign( align );
        KoParagLayout::SpacingType spacing=paragLayout->lineSpacingType;
        showSpacing( spacing );
    }

    m_gui->canvasWidget()->repaintAll(false);
}


void KWView::updateTableActions( Q3ValueList<KWFrameView*> selectedFrames)
{
    TableInfo ti(selectedFrames);
    KWTableFrameSet *table = m_gui->canvasWidget()->getCurrentTable();
    m_actionTableJoinCells->setEnabled( ti.tableCellsSelected());
    m_actionConvertTableToText->setEnabled( table && table->isFloating() );

    m_actionTableSplitCells->setEnabled( ti.oneCellSelected() );

    m_actionTableInsertRow->setEnabled( ti.amountRowsSelected() );
    m_actionTableDelRow->setEnabled( ti.amountRowsSelected() );
    m_actionTableInsertCol->setEnabled( ti.amountColumnsSelected() );

    if(ti.amountColumnsSelected() == 1)
        m_actionTableDelCol->setText(i18n("D&elete Current Column..."));
    else
        m_actionTableDelCol->setText(i18n("D&elete Selected Columns..."));
    m_actionTableDelCol->setEnabled( ti.amountColumnsSelected() > 0 );
    if(ti.amountRowsSelected() == 1)
        m_actionTableDelRow->setText(i18n("&Delete Current Row..."));
    else
        m_actionTableDelRow->setText(i18n("&Delete Selected Rows..."));
    m_actionTableDelRow->setEnabled( ti.amountRowsSelected() > 0 );

    m_actionTableResizeCol->setEnabled( ti.tableCellsSelected() );
    m_actionTableDelete->setEnabled( ti.tableCellsSelected() );
    m_actionTablePropertiesMenu->setEnabled( ti.tableCellsSelected() );

    m_actionTableUngroup->setEnabled( ti.tableCellsSelected() );
    m_actionTableProtectCells->setEnabled( ti.tableCellsSelected() );
    m_actionTableProtectCells->setChecked( ti.protectContentEnabled() );
}

void KWView::docStructChanged(int type)
{
    KWDocStruct *m_pDocStruct=m_gui->getDocStruct();
    if(m_pDocStruct)
        m_pDocStruct->refreshTree(type);
    m_doc->recalcVariables(  VT_STATISTIC );
}

void KWView::documentModified( bool b )
{
    if ( !statusBar() )
        return;

    if ( b )
        m_sbModifiedLabel->setPixmap( KGlobal::iconLoader()->loadIcon( "action-modified", K3Icon::Small ) );
    else
        m_sbModifiedLabel->setText( "   " );
}

void KWView::changeOverwriteMode( bool b )
{
    if ( !statusBar() )
        return;

    if ( b )
        m_sbOverwriteLabel->setText( ' ' + i18n( "OVER" ) + ' ' );
    else
        m_sbOverwriteLabel->setText( ' ' + i18n( "INSRT" ) + ' ' );
}

void KWView::setViewFrameBorders(bool b)
{
    m_viewFrameBorders = b;
    // Store setting in doc, for further views and for saving
    m_doc->setViewFrameBorders( b );
}

bool KWView::doubleClickActivation() const
{
    return TRUE;
}

QWidget* KWView::canvas() const
{
    // used to be ->viewport(), but this makes no sense.
    // active embedded objects must be positioned relative to the real view,
    // to scroll around with it, instead of being children of the fixed viewport.
    return m_gui->canvasWidget();
}

int KWView::canvasXOffset() const
{
    return m_gui->canvasWidget()->contentsX();
}

int KWView::canvasYOffset() const
{
    return m_gui->canvasWidget()->contentsY();
}

void KWView::canvasAddChild( KoViewChild * child )
{
    connect( m_gui->canvasWidget(), SIGNAL( viewTransformationsChanged() ),
             child, SLOT( reposition() ) );
    // Not necessary anymore
    //m_gui->canvasWidget()->addChild( child->frame() );
}

void KWView::changePicture()
{
    KWFrameView *view = frameViewManager()->selectedFrame();
    KWFrame *frame = view == 0 ? 0 : view->frame();
    if( !frame )
        return;
    KWPictureFrameSet *frameset = static_cast<KWPictureFrameSet *>(frame->frameSet());
    KoPictureKey oldKey ( frameset->picture().getKey() );
    QString oldFile ( oldKey.filename() );
    KUrl url;
    url.setPath( oldFile );
    if (!QDir(url.directory()).exists())
        oldFile = url.fileName();

    KoPicture picture ( KWInsertPicDia::selectPictureDia( oldFile, this ) );
    if ( !picture.isNull() )
    {
        KWFrameChangePictureCommand *cmd= new KWFrameChangePictureCommand( i18n("Change Picture"), FrameIndex(frame), oldKey, picture.getKey() ) ;

        frameset->insertPicture( picture );
        m_doc->frameChanged( frame );
        m_doc->refreshDocStructure( FT_PICTURE );
        m_doc->addCommand(cmd);
    }
    else
        kDebug() << "KWView::changePicture cancelled" << endl;
}

void KWView::savePicture()
{
    KWFrameView *view = frameViewManager()->selectedFrame();
    KWFrame *frame = view == 0 ? 0 : view->frame();
    if ( frame )//test for dcop call
    {
        KWPictureFrameSet *frameset = static_cast<KWPictureFrameSet *>(frame->frameSet());
        QString oldFile = frameset->picture().getKey().filename();
        KUrl url;
        url.setPath( oldFile );
        if ( !QDir(url.directory()).exists() )
            oldFile = url.fileName();

        KoPicture picture( frameset->picture() );
        QString mimetype = picture.getMimeType();
        kDebug() << "Picture has mime type: " << mimetype << endl;
        QStringList mimetypes;
        mimetypes << mimetype;
        KFileDialog fd( oldFile, QString::null, this, 0, TRUE );
        fd.setMimeFilter( mimetypes );
        fd.setCaption(i18n("Save Picture"));
        fd.setOperationMode(KFileDialog::Saving);
        if ( fd.exec() == QDialog::Accepted )
        {
            url = fd.selectedURL();
            if ( url.isValid() )
            {
                if ( url.isLocalFile() )
                {
                    QFile file( url.path() );
                    if ( file.open( QIODevice::ReadWrite ) )
                    {
                        picture.save( &file );
                        file.close();
                    }
                    else
                    {
                        KMessageBox::error(this,
                                   i18n("Error during saving. Could not open '%1' for writing").arg ( url.path() ),
                                   i18n("Save Picture"));
                    }
                }
                else
                {
                    KTempFile tempFile;
                    tempFile.setAutoDelete( true );
                    if ( tempFile.status() == 0 )
                    {
                        QFile file( tempFile.name() );
                        if ( file.open( QIODevice::ReadWrite ) )
                        {
                            picture.save( &file );
                            file.close();
                            if ( !KIO::NetAccess::upload( tempFile.name(), url, this ) )
                            {
                              KMessageBox::sorry( this, i18n(
                                  "Unable to save the file to '%1'. %2.").arg( url.prettyURL() ).arg( KIO::NetAccess::lastErrorString() ),
                                  i18n("Save Failed") );
                            }
                        }
                        else
                            KMessageBox::error(this,
                                i18n("Error during saving. Could not open '%1' temporary file for writing").arg ( file.name() ),
                                i18n("Save Picture"));
                    }
                    else
                        KMessageBox::sorry( this, i18n(
                            "Error during saving. Could not create temporary file: %1.").arg( strerror( tempFile.status() ) ),
                            i18n("Save Picture") );
                }
            }
            else
                KMessageBox::sorry( this, i18n("URL %1 is invalid.").arg( url.prettyURL() ), i18n("Save Picture") );
        }
    }
}

void KWView::configureHeaderFooter()
{
    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter kwhf;
    m_doc->getPageLayout( pgLayout, cl, kwhf );

    KWPageLayoutStruct oldLayout( pgLayout, cl, kwhf );

    KoHeadFoot hf;
    int flags = KW_HEADER_AND_FOOTER;
    KoUnit::Unit unit = m_doc->unit();
    KoUnit::Unit oldUnit = unit;

    if ( KoPageLayoutDia::pageLayout( pgLayout, hf, cl, kwhf, flags, unit ) ) {
        if( oldLayout._hf != kwhf )
        {
            KWPageLayoutStruct newLayout( pgLayout, cl, kwhf );

            KCommand *cmd = new KWPageLayoutCommand( i18n("Change Layout"), m_doc,
                                                     oldLayout, newLayout );
            m_doc->addCommand(cmd);

            m_doc->setPageLayout( pgLayout, cl, kwhf );
        }
        if ( unit != oldUnit )
            m_doc->setUnit( unit ); // needs undo/redo support
    }

}

void KWView::inlineFrame()
{
    KWFrameView *view = frameViewManager()->selectedFrame();
    KWFrame *frame = view == 0 ? 0 : view->frame();
    if( !frame)
        return;
    KWFrameSet * fs = frame->frameSet();
    KWFrameSet * parentFs = fs->groupmanager() ? fs->groupmanager() : fs;

    if(m_actionInlineFrame->isChecked())
    {

        KMacroCommand* macroCmd = new KMacroCommand( i18n("Make Frameset Inline") );
        Q3ValueList<FrameIndex> frameindexList;
        Q3ValueList<FrameMoveStruct> frameindexMove;

        KoPoint initialPos = frame->topLeft();
        // turn non-floating frame into floating frame
        KWFrameSetInlineCommand *cmd = new KWFrameSetInlineCommand( i18n("Make Frameset Inline"), parentFs, true );
        cmd->execute();

        frameindexList.append( FrameIndex( frame ) );
        frameindexMove.append( FrameMoveStruct( initialPos, frame->topLeft() ) );

        KWFrameMoveCommand *cmdMoveFrame = new KWFrameMoveCommand( i18n("Move Frame"), frameindexList, frameindexMove );

        macroCmd->addCommand(cmdMoveFrame);
        macroCmd->addCommand(cmd);
        m_doc->addCommand(macroCmd);
    }
    else
    {
        KWFrameSetInlineCommand *cmd = new KWFrameSetInlineCommand( i18n("Make Frameset Non-Inline"), parentFs, false );
        m_doc->addCommand(cmd);
        cmd->execute();
    }
}

void KWView::openLink()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->openLink();
}

void KWView::changeLink()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KoLinkVariable * var=edit->linkVariable();
        if(var)
        {
            QString oldhref= var->url();
            QString oldLinkName=var->value();
            QString link=oldLinkName;
            QString ref=oldhref;
            if(KoInsertLinkDia::createLinkDia(link, ref, m_doc->listOfBookmarkName(0), true, this))
            {
                if(!link.isEmpty() && !ref.isEmpty())
                {
                    if( ref != oldhref || link!=oldLinkName)
                    {
                        KWChangeLinkVariable*cmd=new KWChangeLinkVariable( i18n("Change Link"), m_doc,oldhref, ref, oldLinkName,link, var);
                        cmd->execute();
                        m_doc->addCommand(cmd);
                    }
                }
            }
        }
    }
}

void KWView::copyLink()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->copyLink();
}

void KWView::removeLink()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->removeLink();
}

void KWView::addToBookmark()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KoLinkVariable * var=edit->linkVariable();
        if(var)
        {
            edit->addBookmarks(var->url());
        }
    }
}

void KWView::showDocStructure()
{
    m_doc->setShowDocStruct(m_actionShowDocStruct->isChecked());
    m_doc->reorganizeGUI();
    QTimer::singleShot( 0, this, SLOT( updateZoom() ) );
}

void KWView::showRuler()
{
    m_doc->setShowRuler( m_actionShowRuler->isChecked());
    m_doc->reorganizeGUI();
    QTimer::singleShot( 0, this, SLOT( updateZoom() ) );
}

void KWView::viewGrid()
{
    m_doc->setShowGrid( m_actionViewShowGrid->isChecked() );
    m_doc->setModified( true );
    m_doc->updateGridButton();
    m_doc->repaintAllViews(false);
}

void KWView::viewSnapToGrid()
{
    m_doc->setSnapToGrid( m_actionViewSnapToGrid->isChecked() );
    m_doc->setModified( true );
    m_doc->updateGridButton();
}

void KWView::slotSoftHyphen()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->insertSoftHyphen();
}

void KWView::slotNonbreakingSpace()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->insertNonbreakingSpace();
}

void KWView::slotNonbreakingHyphen()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->insertNonbreakingHyphen();
}

void KWView::slotLineBreak()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->insertLineBreak();
}

void KWView::slotIncreaseNumberingLevel()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->increaseNumberingLevel( m_doc->styleCollection() );
}

void KWView::slotDecreaseNumberingLevel()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->decreaseNumberingLevel( m_doc->styleCollection() );
}

void KWView::refreshAllVariable()
{
    m_doc->recalcVariables( VT_ALL );
}

void KWView::slotAllowAutoFormat()
{
    bool state = m_actionAllowAutoFormat->isChecked();
    m_doc->setAllowAutoFormat( state );
}

void KWView::slotCompletion()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
        edit->completion();
}

void KWView::updateHeaderFooterButton()
{
    m_actionViewHeader->setChecked(m_doc->isHeaderVisible());
    m_actionViewFooter->setChecked(m_doc->isFooterVisible());
}

void KWView::editComment()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KoVariable * tmpVar=edit->variable();
        KoNoteVariable * var = dynamic_cast<KoNoteVariable *>(tmpVar);
        if(var)
        {
            QString authorName;
            KoDocumentInfo * info = m_doc->documentInfo();
            KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
            if ( !authorPage )
                kWarning() << "Author information not found in documentInfo !" << endl;
            else
                authorName = authorPage->fullName();
            QString oldValue = var->note();
            QString createDate = var->createdNote();
            KoCommentDia *commentDia = new KoCommentDia( this, oldValue, authorName, createDate);
            if( commentDia->exec() )
            {
                if ( oldValue != commentDia->commentText())
                {
                    KWChangeVariableNoteText *cmd = new KWChangeVariableNoteText( i18n("Change Note Text"), m_doc, oldValue,commentDia->commentText(), var);
                    m_doc->addCommand( cmd );
                    cmd->execute();
                }
            }
            delete commentDia;
        }
    }
}

void KWView::fileStatistics()
{
    KWStatisticsDialog *statisticsDialog = new KWStatisticsDialog( this, m_doc );
    if ( !statisticsDialog->wasCanceled() )
        statisticsDialog->exec();
    delete statisticsDialog;
}

void KWView::removeComment()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        edit->removeComment();
    }
}

void KWView::copyTextOfComment()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        edit->copyTextOfComment();
    }
}

void KWView::configureCompletion()
{
    m_doc->autoFormat()->readConfig();
    KoCompletionDia dia( this, 0, m_doc->autoFormat() );
    dia.exec();
}

void KWView::applyAutoFormat()
{
    m_doc->autoFormat()->readConfig();
    KMacroCommand *macro = 0L;
    Q3ValueList<KoTextObject *> list(m_doc->visibleTextObjects(viewMode()));
    Q3ValueList<KoTextObject *>::Iterator fit = list.begin();
    for ( ; fit != list.end() ; ++fit )
    {
        KCommand *cmd = m_doc->autoFormat()->applyAutoFormat( *fit );
        if ( cmd )
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Apply Autoformat"));
            macro->addCommand( cmd );
        }
    }
    if ( macro )
        m_doc->addCommand( macro );
}

void KWView::createStyleFromSelection()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KoStyleCollection* coll = m_doc->styleCollection();
        KoCreateStyleDia *dia = new KoCreateStyleDia( QStringList(), this, 0 );
        if ( dia->exec() )
        {
            QString name = dia->nameOfNewStyle();
            KoParagStyle* style = coll->findStyleByDisplayName( name );
            if ( style ) // update existing style
            {
                // TODO confirmation message box
                edit->updateStyleFromSelection( style );
            }
            else // create new style
            {
                style = edit->createStyleFromSelection( name );
                m_doc->styleCollection()->addStyle( style );
                m_doc->updateAllStyleLists();
            }
            showStyle( name );
        }
        delete dia;
    }
}

// Initially called by initGUIButton
void KWView::switchModeView()
{
    KWCanvas* canvas = m_gui->canvasWidget();
    slotUpdateRuler();

    // Now update the actions appropriately
    QString mode = canvas->viewMode()->type();
    bool isTextMode = mode == "ModeText";
    bool state = !isTextMode;
    m_actionToolsCreateText->setEnabled(state);
    m_actionToolsCreatePix->setEnabled(state);
    m_actionToolsCreatePart->setEnabled(state);
    m_actionInsertFormula->setEnabled(state);
    m_actionInsertTable->setEnabled(state);
    changeFootEndNoteState();
    m_actionViewFooter->setEnabled( state && m_doc->processingType() == KWDocument::WP );
    m_actionViewHeader->setEnabled( state && m_doc->processingType() == KWDocument::WP );
    //m_actionViewTextMode->setEnabled(m_doc->processingType()==KWDocument::WP);
    m_actionShowDocStruct->setEnabled(state);
    m_actionFormatPage->setEnabled(state);
    m_actionInsertContents->setEnabled( state );
    m_actionFrameStyle->setEnabled( state );
    m_actionTableStyle->setEnabled ( state );
    m_actionViewShowGrid->setEnabled( state );
    m_actionViewSnapToGrid->setEnabled( mode == "ModeNormal" || mode == "ModeEmbedded" ); // TODO fix snapping in preview mode
    if ( m_gui->getHorzRuler())
    {
        m_gui->getHorzRuler()->setPageLayoutMenuItemEnabled( state );
        if ( !koDocument()->isReadWrite())
            m_gui->getHorzRuler()->changeFlags( KoRuler::F_NORESIZE );
        else
        {
            if ( state )
                m_gui->getHorzRuler()->changeFlags( m_gui->getHorzRuler()->flags() & ~(KoRuler::F_NORESIZE) );
            else
                m_gui->getHorzRuler()->changeFlags( m_gui->getHorzRuler()->flags() | KoRuler::F_NORESIZE );
        }
    }
    if ( m_gui->getVertRuler() )
    {
        m_gui->getVertRuler()->setPageLayoutMenuItemEnabled( state );
        if ( !koDocument()->isReadWrite())
            m_gui->getVertRuler()->changeFlags( KoRuler::F_NORESIZE );
        else
        {
            if ( state )
                m_gui->getVertRuler()->changeFlags( m_gui->getVertRuler()->flags() & ~(KoRuler::F_NORESIZE) );
            else
                m_gui->getVertRuler()->changeFlags( m_gui->getVertRuler()->flags() | KoRuler::F_NORESIZE );

        }
    }

    if ( isTextMode )
    {
        if ( m_doc->showdocStruct() )
        {
            m_doc->setShowDocStruct(false);
            m_doc->reorganizeGUI();
        }
    }
    else
    {
        m_doc->setShowDocStruct(m_actionShowDocStruct->isChecked());
        m_doc->reorganizeGUI();
    }
    //recalc pgnum variable when we swith viewmode
    //because in text mode view we display field code and not value
    //normal because we don't have real page in this mode
    m_doc->recalcVariables( VT_PGNUM );
    if ( isTextMode )
    {
        // Make sure we edit the same frameset as the one shown in the textview ;-)
        canvas->editFrameSet( static_cast<KWViewModeText* >(canvas->viewMode())->textFrameSet() );

    }
    //remove/add "zoom to page". Not necessary in text mode view.
    updateZoomControls();

    updatePageInfo();
    // set page layout in rulers
    canvas->viewMode()->setPageLayout( m_gui->getHorzRuler(), m_gui->getVertRuler(), m_doc->pageLayout() );
}

void KWView::configureFootEndNote()
{
    KWConfigFootNoteDia *dia = new KWConfigFootNoteDia( this, "configfootnote", m_doc );
    dia->exec();
    delete dia;
}

void KWView::editFootEndNote()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KWFootNoteVariable * var = dynamic_cast<KWFootNoteVariable *>( edit->variable() );
        if ( var && var->frameSet() )
        {
            m_gui->canvasWidget()->editFrameSet( var->frameSet() );
            KWTextFrameSetEdit* textedit = currentTextEdit();
            if ( textedit )
                textedit->ensureCursorVisible();
        }
    }
}

void KWView::changeFootNoteType()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        KoVariable * tmpVar=edit->variable();
        KWFootNoteVariable * var = dynamic_cast<KWFootNoteVariable *>(tmpVar);
        if(var && var->frameSet())
        {
            KWFootNoteDia dia( var->noteType(), var->numberingType(), (var->numberingType()==KWFootNoteVariable::Auto) ? QString::null : var->manualString(), this, m_doc, 0 );
            Q3PtrListIterator<KoTextCustomItem> it( edit->textDocument()->allCustomItems() );
            for ( ; it.current() ; ++it )
            {
                KWFootNoteVariable *fnv = dynamic_cast<KWFootNoteVariable *>( it.current() );
                if (fnv && !fnv->isDeleted() && fnv->frameSet() && !fnv->frameSet()->isDeleted() &&
                 fnv->numberingType()==KWFootNoteVariable::Manual && fnv != var)
                    dia.appendManualFootNote( fnv->text() );
            }
            if ( dia.exec() )
            {
                FootNoteParameter oldParam( var );
                FootNoteParameter newParam(dia.noteType(), dia.numberingType(), dia.manualString() );
                KWChangeFootNoteParametersCommand * cmd = new KWChangeFootNoteParametersCommand( i18n("Change Footnote Parameters"), var , oldParam, newParam, m_doc);
                cmd->execute();
                m_doc->addCommand ( cmd );
            }
        }
    }
}


void KWView::autoSpellCheck()
{
    autoSpellCheck( m_actionAllowBgSpellCheck->isChecked() );
}

void KWView::autoSpellCheck(bool b)
{
    m_doc->changeBgSpellCheckingState( b );
}

void KWView::goToFootEndNote()
{
    KWFrameView *view = frameViewManager()->selectedFrame();
    KWFrame *frame = view == 0 ? 0 : view->frame();
    if( !frame)
        return;
    KWFootNoteFrameSet *footNoteFrameSet = dynamic_cast<KWFootNoteFrameSet *>(frame->frameSet());
    if ( footNoteFrameSet )
    {
        KWFootNoteVariable* var=footNoteFrameSet->footNoteVariable();
        KoTextParag *parag = var->paragraph();
        int index = var->index();
        KWTextDocument *textDoc = static_cast<KWTextDocument *>(var->textDocument());
        KWTextFrameSet *frameSet =textDoc->textFrameSet();
        m_gui->canvasWidget()->editTextFrameSet( frameSet, parag, index );
    }
}

void KWView::openDocStructurePopupMenu( const QPoint &p, KWFrameSet *frameset, KWTextParag *parag)
{
    bool rw = koDocument()->isReadWrite();
    bool hasText = (frameset->type()==FT_TEXT || frameset->type()==FT_TABLE || frameset->type()==FT_FORMULA );

    m_actionDocStructEdit->setEnabled( rw && hasText );
    m_actionDocStructDelete->setEnabled( (rw && !parag && !frameset->isMainFrameset() &&
        !frameset->isFootEndNote() && !frameset->isHeaderOrFooter()) );

    Q3PopupMenu* popup = static_cast<Q3PopupMenu *>(factory()->container("docstruct_popup",this));
    if ( popup )
        popup->exec(p);
}

void KWView::docStructSelect()
{
    if ( m_gui->getDocStruct() )
    {
        m_gui->getDocStruct()->selectItem();
    }
}

void KWView::docStructEdit()
{
    if ( m_gui->getDocStruct() )
    {
        m_gui->getDocStruct()->editItem();
        //return focus to canvas.
        m_gui->canvasWidget()->setFocus();
    }
}

void KWView::docStructProperties()
{
    if ( m_gui->getDocStruct() )
    {
        m_gui->getDocStruct()->editProperties();
    }
}

void KWView::docStructDelete()
{
    if ( m_gui->getDocStruct() )
    {
        m_gui->getDocStruct()->deleteItem();
    }
}

void KWView::insertFile()
{
    KFileDialog fd( QString::null, QString::null, this, 0, TRUE );
    fd.setMimeFilter( "application/x-kword" );
    fd.setCaption(i18n("Insert File"));
    KUrl url;
    if ( fd.exec() != QDialog::Accepted )
        return;
    url = fd.selectedURL();
    if( url.isEmpty() )
    {
        KMessageBox::sorry( this,
                            i18n("File name is empty."),
                            i18n("Insert File"));
        return;
    }
    insertFile( url );
}

void KWView::insertFile(const KUrl& url)
{
    KMacroCommand* macroCmd = 0L;
    bool hasFixedFramesets = false;
    KoStore* store=KoStore::createStore( this, url, KoStore::Read );

    // TODO: this code only supports KWord-1.3 stores, it needs to support OASIS too.

    // ###### All of this should be ported to use the pasting code instead.

    QString maindoc = "maindoc.xml";
    if ( store )
    {
        // We need to load the pictures before we treat framesets
        // because KWDocument::pasteFrames() calls processPictureRequests().
        bool b = store->open(maindoc);
        if ( !b )
        {
            KMessageBox::sorry( this,
                                i18n("This file is not a KWord file!"),
                                i18n("Insert File"));
            delete store;
            return;
        }

        QDomDocument doc;
        doc.setContent( store->device() );
        QDomElement word = doc.documentElement();

        m_doc->loadPictureMap( word );
        store->close();
        m_doc->loadImagesFromStore( store );
    }
    if ( store )
    {
        bool b = store->open(maindoc);
        if ( !b )
        {
            KMessageBox::sorry( this,
                                i18n("File name is not a KWord file!"),
                                i18n("Insert File"));
            delete store;
            return;
        }
        QDomDocument doc;
        doc.setContent( store->device() );
        QDomElement word = doc.documentElement();

        QDomElement framesets = word.namedItem( "FRAMESETS" ).toElement();
        if ( !framesets.isNull() )
        {
            QDomElement framesetElem = framesets.firstChild().toElement();
            // just in case
            while ( !framesetElem.isNull() && framesetElem.tagName() != "FRAMESET" )
                framesetElem = framesetElem.nextSibling().toElement();

            if ( !framesetElem.isNull() )
            {
                KWTextFrameSet *textFrameSet = dynamic_cast<KWTextFrameSet *>( m_doc->frameSet(0) );
                KoTextCursor insertionCursor( textFrameSet->textDocument() );
                KWTextFrameSetEdit* edit = currentTextEdit();
                if ( edit && !edit->textFrameSet()->protectContent()) {
                    textFrameSet = edit->textFrameSet();
                    insertionCursor = *edit->cursor();
                }
                // Handle the main textframeset special - concatenate the text
                QDomDocument domDoc( "PARAGRAPHS" );
                QDomElement paragsElem = domDoc.createElement( "PARAGRAPHS" );
                domDoc.appendChild( paragsElem );

                // Need an intermediate list otherwise nextSibling doesn't work after moving the node
                // to the other DOM tree ;)
                Q3ValueList<QDomElement> paragList;
                Q3ValueList<QString> inlineFsNames;
                QDomElement fsElem;

                QDomNode n = framesetElem.firstChild().toElement();
                while( !n.isNull() )
                {
                    QDomElement e = n.toElement(); // try to convert the node to an element.
                    if  ( !e.isNull() && e.tagName() == "PARAGRAPH" )
                    {
                        paragList.append( e );
                        // Handle inline framesets
                        QDomElement formatsElem = e.namedItem( "FORMATS" ).toElement();
                        if ( !formatsElem.isNull() )
                        {
                            // Get references to inline framesets
                            QDomElement formatElem = formatsElem.firstChild().toElement();
                            for ( ; !formatElem.isNull() ; formatElem = formatElem.nextSibling().toElement() )
                            {
                                QDomElement anchorElem = formatElem.namedItem( "ANCHOR" ).toElement();
                                if ( !anchorElem.isNull() )
                                {
                                    QString type = anchorElem.attribute( "type" );
                                    if ( type == "grpMgr" /* old syntax */ || type == "frameset" )
                                    {
                                        QString iName = anchorElem.attribute( "instance" );
                                        inlineFsNames.append( iName );
                                        // inline framsets shall appear after the paragraph
                                        QString tableName;
                                        QDomElement table;
                                        fsElem = framesetElem.nextSibling().toElement();
                                        for ( ; !fsElem.isNull() ; fsElem = fsElem.nextSibling().toElement() )
                                        {
                                            if ( fsElem.tagName() == "FRAMESET" )
                                            {
                                                QString name = fsElem.attribute( "name" );
                                                QString grpMgr = fsElem.attribute( "grpMgr" );
                                                if ( name == iName )
                                                {
                                                    paragList.append( fsElem );
                                                    //kDebug()<<k_funcinfo<<" Inline frameset: "<<name<<" added"<<endl;
                                                }
                                                else if ( grpMgr == iName )
                                                {   // Table so we need to create table framset if it is new
                                                    if ( grpMgr != tableName )
                                                    {
                                                        tableName = grpMgr;
                                                        table = domDoc.createElement("FRAMESET");
                                                        table.setAttribute("frameType", FT_TABLE);
                                                        table.setAttribute("frameInfo", 0);
                                                        table.setAttribute("protectSize", fsElem.attribute("protectSize","0"));
                                                        table.setAttribute("name", tableName);
                                                        table.setAttribute("visible", fsElem.attribute("visible","1"));
                                                        paragList.append( table ); // same level as paragraphs, so it goes into the paragList
                                                        //kDebug()<<k_funcinfo<<" paragList Added new table: "<<grpMgr<<endl;
                                                    }

                                                    table.appendChild( fsElem.cloneNode() ); // add the cell as child to the table frameset
                                                    //kDebug()<<k_funcinfo<<" Inline table: "<<grpMgr<<" Added new cell: "<<name<<endl;
                                                }
                                                //else kDebug()<<k_funcinfo<<" Fixed frameset: "<<name<<endl;
                                            }
                                            //else kDebug()<<k_funcinfo<<" Not frameset: "<<fsElem.tagName()<<endl;
                                        }
                                        //kDebug()<<k_funcinfo<<" Treated "<<i<<" frameset elements"<<endl;
                                    }
                                }
                            }
                        }
                    }
                    n = n.nextSibling();
                }

                Q3ValueList<QDomElement>::Iterator it = paragList.begin();
                Q3ValueList<QDomElement>::Iterator end = paragList.end();
                for ( ; it!= end ; ++it )
                {
                    //kDebug()<<k_funcinfo<<" paragList tag: "<<(*it).tagName()<<" name: "<<(*it).attribute( "name" )<<" grpMgr: "<<(*it).attribute( "grpMgr" )<<endl;
                    paragsElem.appendChild( *it );
                }
                //kDebug() << k_funcinfo << "Paragraphs:\n" << domDoc.toCString() << endl;

                // The fixed framesets
                // doctype SELECTION is used for fixed framesets
                QDomDocument domDocFrames( "SELECTION" ); // see KWCanvas::copySelectedFrames
                QDomElement topElem = domDocFrames.createElement( "SELECTION" );
                domDocFrames.appendChild( topElem );
                QString tableName;
                QDomElement table;
                Q3ValueList<QString> fsInHeader;
                Q3ValueList<QDomElement> framesetsList;

                framesetElem = framesetElem.nextSibling().toElement();
                for ( ; !framesetElem.isNull() ; framesetElem = framesetElem.nextSibling().toElement() )
                {
                    if ( framesetElem.tagName() == "FRAMESET" )
                    {
                        FrameSetType frameSetType = static_cast<FrameSetType>( KWDocument::getAttribute( framesetElem, "frameType", FT_BASE ) );
                        KWFrameSet::Info info = static_cast<KWFrameSet::Info>( framesetElem.attribute("frameInfo").toInt() );
                        if ( frameSetType == FT_TEXT &&
                             (info == KWFrameSet::FI_FIRST_HEADER ||
                              info == KWFrameSet::FI_EVEN_HEADER   ||
                              info == KWFrameSet::FI_ODD_HEADER  ||
                              info == KWFrameSet::FI_FIRST_FOOTER ||
                              info == KWFrameSet::FI_EVEN_FOOTER   ||
                              info == KWFrameSet::FI_ODD_FOOTER)
                            )
                            fsInHeader += getInlineFramesets( framesetElem );

                        QString name = framesetElem.attribute( "name" );
                        QString grpMgr = framesetElem.attribute( "grpMgr" );
                        if ( !inlineFsNames.contains(name) && !inlineFsNames.contains(grpMgr) )
                        {   // fixed frameset
                            if ( !grpMgr.isEmpty() )
                            {   // Table cell
                                if ( grpMgr != tableName )
                                {   // New table (first cell)
                                    tableName = grpMgr;
                                    table = domDocFrames.createElement("FRAMESET");
                                    table.setAttribute("frameType", FT_TABLE);
                                    table.setAttribute("frameInfo", 0);
                                    table.setAttribute("protectSize", framesetElem.attribute("protectSize","0"));
                                    table.setAttribute("name", tableName);
                                    table.setAttribute("visible", framesetElem.attribute("visible","1"));
                                    framesetsList.append( table );
                                    //kDebug()<<k_funcinfo<<" framesetsList Added new table: "<<grpMgr<<endl;
                                }
                                table.appendChild( framesetElem.cloneNode() ); // add the cell as child to the table frameset
                                //kDebug()<<k_funcinfo<<" Fixed table '"<<grpMgr<<"': Added new cell: '"<<name<<"'"<<endl;
                            }
                            else // other frameset type
                            {
                                framesetsList.append( framesetElem );
                                //kDebug()<<k_funcinfo<<" Fixed frameset: '"<<name<<"' added"<<endl;
                            }
                        }
                        //else kDebug()<<k_funcinfo<<" Inline frameset, skipped: "<<name<<endl;
                    }
                    //else kDebug()<<k_funcinfo<<" Not frameset element, skipped: "<<framesetElem.tagName()<<endl;
                }
                it = framesetsList.begin();
                end = framesetsList.end();
                for ( ; it != end ; ++it )
                {
                    framesetElem = (*it);
                    FrameSetType frameSetType = static_cast<FrameSetType>( KWDocument::getAttribute( framesetElem, "frameType", FT_BASE ) );
                    KWFrameSet::Info info = static_cast<KWFrameSet::Info>( framesetElem.attribute("frameInfo").toInt() );
                    QString name = framesetElem.attribute("name");
                    QString grpMgr = framesetElem.attribute( "grpMgr" );
                    // We skip headers, footers and framsets/tables inside theese
                    if ( !fsInHeader.contains(name) && !fsInHeader.contains(grpMgr) &&
                         !( frameSetType == FT_TEXT &&
                            (info == KWFrameSet::FI_FIRST_HEADER ||
                             info == KWFrameSet::FI_EVEN_HEADER   ||
                             info == KWFrameSet::FI_ODD_HEADER  ||
                             info == KWFrameSet::FI_FIRST_FOOTER ||
                             info == KWFrameSet::FI_EVEN_FOOTER   ||
                             info == KWFrameSet::FI_ODD_FOOTER) )
                        )
                    {
                        hasFixedFramesets = true;
                        topElem.appendChild( framesetElem );
                    }
                }
                // Embedded documents
                QDomDocument embeddedDoc( "SELECTION" );
                QDomElement selElem = embeddedDoc.createElement( "SELECTION" );
                embeddedDoc.appendChild( selElem );

                Q3ValueList<QDomElement> embeddedList;
                QDomElement embeddedElem = word.namedItem( "EMBEDDED" ).toElement();
                for ( ; !embeddedElem.isNull() ; embeddedElem = embeddedElem.nextSibling().toElement() )
                {
                    if ( embeddedElem.tagName() == "EMBEDDED" )
                    {
                        embeddedList.append( embeddedElem );
                    }
                }
                it = embeddedList.begin();
                end = embeddedList.end();
                bool hasEmbedded = false;
                for ( ; it!= end ; ++it )
                {
                    selElem.appendChild( *it );
                    hasEmbedded = true;
                }
                store->close();


                // Now we do the insertion
                if ( !macroCmd )
                    macroCmd = new KMacroCommand( i18n("Insert File") );

                if ( hasFixedFramesets )
                {
                    // insert fixed framesets
                    //kDebug() << k_funcinfo << domDocFrames.toCString() << endl;
                    m_doc->pasteFrames( topElem, macroCmd, false, false, false /* don't select frames */ );
                }
                if ( hasEmbedded )
                {
                    //kDebug()<<k_funcinfo<<" Embedded: \n"<<embeddedDoc.toCString()<<endl;
                    if ( !macroCmd )
                        macroCmd = new KMacroCommand( i18n("Insert File") );
                    m_doc->insertEmbedded( store, selElem, macroCmd, 0 );
                }

                // insert paragraphs and inline framesets (we always have at least one paragraph)
                KCommand *cmd = textFrameSet->pasteOasis( &insertionCursor, domDoc.toCString(), true );

                if ( cmd ) {
                    macroCmd->addCommand( cmd );
                }
                m_doc->addCommand( macroCmd );

            }
        }

        m_doc->loadImagesFromStore( store );
        m_doc->completePasting();

    }
    delete store;
}

Q3ValueList<QString> KWView::getInlineFramesets( const QDomNode &framesetElem)
{
    //kDebug()<<k_funcinfo<<" Frameset: "<<framesetElem.toElement().attribute("name")<<endl;
    Q3ValueList<QString> list;
    QDomNode n = framesetElem.firstChild().toElement();
    for( ; !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if  ( !e.isNull() && e.tagName() == "PARAGRAPH" )
        {
            QDomElement formatsElem = e.namedItem( "FORMATS" ).toElement();
            if ( !formatsElem.isNull() )
            {
                // Get references to inline framesets
                QDomElement formatElem = formatsElem.firstChild().toElement();
                for ( ; !formatElem.isNull() ; formatElem = formatElem.nextSibling().toElement() )
                {
                    QDomElement anchorElem = formatElem.namedItem( "ANCHOR" ).toElement();
                    if ( !anchorElem.isNull() )
                    {
                        QString type = anchorElem.attribute( "type" );
                        if ( type == "grpMgr" /* old syntax */ || type == "frameset" )
                        {
                            QString iName = anchorElem.attribute( "instance" );
                            list.append( iName );
                            //kDebug()<<k_funcinfo<<" added: "<<iName<<endl;
                        }
                    }
                }
            }
        }
    }
    return list;
}

void KWView::addBookmark()
{
    KWTextFrameSetEdit * edit = currentTextEdit();
    if ( edit )
    {
        //don't create list against viewmode. We must list all bookmarks.
        KWCreateBookmarkDia dia( m_doc->listOfBookmarkName(0L), this, 0 );
        if ( dia.exec() ) {
            QString bookName = dia.bookmarkName();
            KoTextCursor start, end;
            if ( edit->textDocument()->hasSelection( KoTextDocument::Standard ) ) {
                start = edit->textDocument()->selectionStartCursor( KoTextDocument::Standard );
                end = edit->textDocument()->selectionEndCursor( KoTextDocument::Standard );
            } else {
                start = *edit->cursor();
                end = start;
            }
            int startSel = start.index();
            int endSel = end.index();
            m_doc->insertBookmark(bookName, start.parag(),end.parag(), startSel, endSel);
        }
    }
}

void KWView::selectBookmark()
{
    KWSelectBookmarkDia dia( m_doc->listOfBookmarkName(viewMode()), m_doc, this, 0 );
    if ( dia.exec() ) {
        QString bookName = dia.bookmarkSelected();
        const KoTextBookmark * book = m_doc->bookmarkByName( bookName );
        Q_ASSERT( book );
        if ( book )
        {
            Q_ASSERT( book->startParag() );
            Q_ASSERT( book->endParag() );
            if ( !book->startParag() || !book->endParag() )
                return;
            KWTextFrameSet* textfs = static_cast<KWTextDocument *>( book->textDocument() )->textFrameSet();
            m_gui->canvasWidget()->editTextFrameSet( textfs, book->startParag(), book->bookmarkStartIndex() );
            KWTextFrameSetEdit * edit = currentTextEdit();
            Q_ASSERT( edit );
            if ( edit )
            {
                edit->textDocument()->removeSelection( KoTextDocument::Standard );
                edit->textDocument()->setSelectionStart( KoTextDocument::Standard, edit->cursor() );
                edit->cursor()->setParag( book->endParag());
                edit->cursor()->setIndex(book->bookmarkEndIndex() );
                edit->textDocument()->setSelectionEnd( KoTextDocument::Standard, edit->cursor() );
                book->startParag()->setChanged( true );
                book->endParag()->setChanged( true );
                m_doc->slotRepaintChanged( edit->frameSet() );
            }
        }
    }
}

void KWView::importStyle()
{
    KWImportStyleDia dia( m_doc, m_doc->styleCollection(), this );
    if ( dia.exec() && !dia.importedStyles().isEmpty() ) {
        m_doc->styleCollection()->importStyles( dia.importedStyles() );
        m_doc->setModified( true );
        m_doc->updateAllStyleLists();
    }
}

void KWView::testAndCloseAllFrameSetProtectedContent()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    if ( edit && edit->textFrameSet()->protectContent()) {
        m_doc->terminateEditing( edit->frameSet());
    }
}

void KWView::updateRulerInProtectContentMode()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    KoRuler * hRuler = m_gui ? m_gui->getHorzRuler() : 0;

    if ( edit && hRuler) {
        if ( !edit->textFrameSet()->protectContent() )
            hRuler->changeFlags(KoRuler::F_INDENTS | KoRuler::F_TABS);
        else
            hRuler->changeFlags(0);
        hRuler->repaint();
    }
}

void KWView::deselectAllFrames()
{
    m_gui->canvasWidget()->selectAllFrames( false );
}

void KWView::insertDirectCursor()
{
#if 0
    insertDirectCursor( m_actionInsertDirectCursor->isChecked());
#endif
}

void KWView::insertDirectCursor(bool b)
{
    m_doc->setInsertDirectCursor(b);
}

void KWView::updateDirectCursorButton()
{
#if 0
    m_actionInsertDirectCursor->setChecked(m_doc->insertDirectCursor());
#endif
}

void KWView::convertTableToText()
{
    KWCanvas * canvas = m_gui->canvasWidget();
    KWTableFrameSet *table = canvas->getCurrentTable();
    if ( table && table->isFloating() )
    {
        const QByteArray arr = table->convertTableToText();
        KWAnchor * anchor = table->findAnchor( 0 );
        if ( anchor && arr.size() )
        {
            KWTextFrameSet *frameset = table->anchorFrameset();
            KoTextParag *parag = anchor->paragraph();
            int pos = anchor->index();
            KMacroCommand *macro = new KMacroCommand(i18n("Convert Table to Text"));
            KCommand *cmd = table->anchorFrameset()->deleteAnchoredFrame( anchor );
            if ( cmd )
                macro->addCommand( cmd);

            m_gui->canvasWidget()->editTextFrameSet( frameset, parag, pos );

            KWTextFrameSetEdit* edit = currentTextEdit();
            if ( edit && edit->textFrameSet())
            {
                cmd = edit->textFrameSet()->pasteOasis( edit->cursor(), arr, true );
                if ( cmd )
                    macro->addCommand( cmd );
            }
            m_doc->addCommand(macro);
        }
    }
}

void KWView::convertToTextBox()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    if ( !edit )
        return;

    KWTextFrameSet* textfs = edit->textFrameSet();
    if( textfs->protectContent() || !textfs->textObject()->hasSelection() )
        return;

    KWOasisSaver oasisSaver( m_doc );
    textfs->textDocument()->copySelection( oasisSaver.bodyWriter(), oasisSaver.savingContext(), KoTextDocument::Standard );
    if ( !oasisSaver.finish() )
        return;
    const QByteArray arr = oasisSaver.data();
    if ( !arr.size() )
        return;

    KCommand *cmd = textfs->textObject()->removeSelectedTextCommand( edit->textView()->cursor(), KoTextDocument::Standard );
    Q_ASSERT( cmd );
    KMacroCommand* macro = new KMacroCommand( i18n("Convert to Text Box"));
    macro->addCommand( cmd );
    // Where to place the resulting text box? Maybe it should be made inline?
    cmd = m_gui->canvasWidget()->createTextBox( KoRect(30,30,300,300) );
    Q_ASSERT( cmd );
    if ( cmd )
        macro->addCommand( cmd );

    edit = currentTextEdit();
    Q_ASSERT( edit ); // if it can really be 0, we need to undo the above...
    if ( edit )
    {
        cmd = edit->textFrameSet()->pasteOasis( edit->textView()->cursor(), arr, true );
        if ( cmd )
            macro->addCommand( cmd );
        // Auto-resize the frame from its contents
        edit->textFrameSet()->layout();
    }
    m_doc->addCommand( macro );
}

void KWView::slotAddIgnoreAllWord()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    if ( edit )
        m_doc->addSpellCheckIgnoreWord( edit->currentWordOrSelection() );
}

void KWView::sortText()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    if ( edit && edit->textFrameSet()->hasSelection() )
    {
        KWSortDia dlg( this, "sort dia" );
        if ( dlg.exec() )
        {
            const QByteArray arr = edit->textFrameSet()->sortText(dlg.getSortType());
            if ( arr.size() )
            {
                KCommand *cmd = edit->textFrameSet()->pasteOasis( edit->cursor(), arr, true );
                if ( cmd )
                    m_doc->addCommand(cmd);
            }
        }
    }
}

void KWView::addPersonalExpression()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    if ( !(edit && edit->textFrameSet()->hasSelection()))
        return;

    QString newExpression = edit->textFrameSet()->textObject()->selectedText();
    //load file !!!
    QString tmp=locateLocal("data","kword/expression/perso.xml");
    QFile file( tmp );
    if ( !file.open( QIODevice::ReadOnly ) )
        return;
    QDomDocument doc;
    doc.setContent( &file );
    file.close();

    QString group;
    QMap<QString, QStringList>lstOfPersonalExp;
    QStringList list;
    QDomNode n = doc.documentElement().firstChild();
    for( ; !n.isNull(); n = n.nextSibling() )
    {
        if ( n.isElement() )
        {
            QDomElement e = n.toElement();
            if ( e.tagName() == "Type" )
            {
                list.clear();
                group = i18n( e.namedItem( "TypeName" ).toElement().text().utf8() );

                QDomNode n2 = e.firstChild();
                for( ; !n2.isNull(); n2 = n2.nextSibling() )
                {

                    if ( n2.isElement() )
                    {
                        QDomElement e2 = n2.toElement();
                        if ( e2.tagName() == "Expression" )
                        {
                            QString text = i18n( e2.namedItem( "Text" ).toElement().text().utf8() );
                            list<<text;
                        }
                    }
                }
                lstOfPersonalExp.insert(group,list);
                group = "";
            }
        }
    }
    //save
    doc = QDomDocument( "KWordExpression" );
    QDomElement begin = doc.createElement( "KWordExpression" );
    doc.appendChild( begin );
    QMapIterator<QString, QStringList> itPersonalExp = lstOfPersonalExp.find(i18n("Normal"));
    if ( itPersonalExp != lstOfPersonalExp.end())
    {
        list = itPersonalExp.data();
        list<<newExpression;
        lstOfPersonalExp.replace( i18n("Normal"), list);
    }
    else
    {
        list.clear();
        list<<newExpression;
        lstOfPersonalExp.insert( i18n("Normal"), list);
    }


    itPersonalExp = lstOfPersonalExp.begin();
    for ( ; itPersonalExp != lstOfPersonalExp.end(); ++itPersonalExp )
    {
        QDomElement type = doc.createElement( "Type" );
        begin.appendChild( type );
        QDomElement typeName = doc.createElement( "TypeName" );
        type.appendChild( typeName );
        typeName.appendChild( doc.createTextNode(itPersonalExp.key()  ) );
        list=itPersonalExp.data();
        for( uint i=0;i<list.count();i++ )
        {
            QDomElement expr = doc.createElement( "Expression" );
            type.appendChild( expr );
            QDomElement text = doc.createElement( "Text" );
            expr.appendChild( text );
            text.appendChild( doc.createTextNode(list[i] ) );
        }
    }
    Q3CString s = doc.toCString();

    if ( !file.open( QIODevice::WriteOnly ) )
    {
        kDebug()<<"Error in addPersonalExpression()\n";
        return;
    }
    file.write(s,s.length());
    file.close();
    m_doc->refreshMenuExpression();
}

void KWView::addWordToDictionary()
{
    KWTextFrameSetEdit* edit = currentTextEdit();
    if ( edit )
    {
        QString word = edit->wordUnderCursor( *edit->cursor() );
        if ( !word.isEmpty() )
            m_doc->addWordToDictionary( word );
    }
}

void KWView::embeddedStoreInternal()
{
    kDebug(31001)<<k_funcinfo<<endl;
    KWFrameView *view = frameViewManager()->selectedFrame();
    KWFrame *frame = view == 0 ? 0 : view->frame();
    if( !frame)
        return;
    KWPartFrameSet *part = static_cast<KWPartFrameSet *>(frame->frameSet());
    part->storeInternal();
}

void KWView::goToDocumentStructure()
{
    KWDocStruct* docStruct = getGUI()->getDocStruct();
    if (docStruct) docStruct->setFocusHere();
}

void KWView::goToDocument()
{
    KWCanvas* canvas = getGUI()->canvasWidget();
    if (!canvas) return;
    canvas->setFocus();
}

void KWView::deleteFrameSet( KWFrameSet * frameset)
{
    if ( frameset  && frameset->frame(0))
    {
        frameViewManager()->view(frameset->frame(0))->setSelected( true );
        deleteFrame();
    }
}

Q3PtrList<KAction> KWView::listOfResultOfCheckWord( const QString &word )
{
    Q3PtrList<KAction> listAction;
    DefaultDictionary *dict = m_broker->defaultDictionary();
    const QStringList lst = dict->suggest( word );
    if ( !lst.contains( word ) )
    {
        QStringList::ConstIterator it = lst.begin();
        const QStringList::ConstIterator end = lst.end();
        for ( ; it != end ; ++it )
        {
            if ( !(*it).isEmpty() ) // in case of removed subtypes or placeholders
            {
                KAction * act = new KAction( *it );
                connect( act, SIGNAL(activated()), this, SLOT(slotCorrectWord()) );
                listAction.append( act );
            }
        }
    }
    return listAction;
}

void KWView::slotCorrectWord()
{
    KAction * act = (KAction *)(sender());
    KWTextFrameSetEdit* edit = currentTextEdit();
    if ( edit )
    {
        edit->selectWordUnderCursor( *(edit->cursor()) );
        m_doc->addCommand( edit->textObject()->replaceSelectionCommand( edit->cursor(), act->text(), i18n("Replace Word") ));
    }
}

void KWView::slotChildActivated( bool a )
{
  // Same hack as in KoView
  KoViewChild* ch = child( (KoView*)sender() );
  if ( !ch )
    return;
  KWDocumentChild* kwchild = static_cast<KWDocumentChild *>( ch->documentChild() );
  KWPartFrameSet* fs = kwchild->partFrameSet();
  //kDebug() << "KWView::slotChildActivated fs=" << fs << endl;
  Q_ASSERT( fs );
  if ( fs ) {
      if ( a )
          fs->startEditing();
      else
          fs->endEditing();
  }
  KoView::slotChildActivated( a );
}

Broker *KWView::broker() const
{
    return m_broker;
}

void KWView::slotUnitChanged( KoUnit::Unit unit )
{
    getGUI()->getHorzRuler()->setUnit( unit );
    getGUI()->getVertRuler()->setUnit( unit );
    if ( m_sbUnitLabel )
        m_sbUnitLabel->setText( ' ' + KoUnit::unitDescription( unit ) + ' ' );
}

KWFrameViewManager* KWView::frameViewManager() const {
    return getGUI()->canvasWidget()->frameViewManager();
}

void KWView::deleteSelectedFrames() {
    int nbCommand=0;

    int docItem=0; // bitmask for changed doc items.

    KMacroCommand * macroCmd = new KMacroCommand( i18n("Delete Frames") );
    KWFrameViewManager *fvMgr = frameViewManager();
    while (KWFrameView *frameView = fvMgr->selectedFrame()) {
        KWFrame *frame = frameView->frame();
        KWFrameSet *fs = frame->frameSet();
        if ( fs->isAFooter() || fs->isAHeader() )
            continue;
        KWTableFrameSet *table=fs->groupmanager();
        if ( table ) {
            docItem|=m_doc->typeItemDocStructure(table->type());

            if ( table->isFloating() ) {
                docItem|=m_doc->typeItemDocStructure(fs->type());

                KWAnchor * anchor = table->findAnchor( 0 );
                KCommand * cmd=table->anchorFrameset()->deleteAnchoredFrame( anchor );
                macroCmd->addCommand(cmd);
                nbCommand++;
            }
            else {
                KWDeleteTableCommand *cmd = new KWDeleteTableCommand( i18n("Delete Table"), table );
                cmd->execute();
                macroCmd->addCommand(cmd);
                nbCommand++;
            }
        }
        else { // a simple frame
            if ( fs->isMainFrameset() )
                continue;

            docItem|=m_doc->typeItemDocStructure(fs->type());

            if ( fs->isFloating() ) {
                KWAnchor * anchor = fs->findAnchor( 0 );
                KCommand *cmd=fs->anchorFrameset()->deleteAnchoredFrame( anchor );
                macroCmd->addCommand(cmd);
                nbCommand++;
            }
            else {
                KWDeleteFrameCommand *cmd = new KWDeleteFrameCommand( i18n("Delete Frame"), frame );
                cmd->execute();
                macroCmd->addCommand(cmd);
                nbCommand++;
            }
        }
    }
    if( nbCommand ) {
        m_doc->addCommand(macroCmd);
        m_doc->refreshDocStructure(docItem);
    }
    else
        delete macroCmd;
}


/******************************************************************/
/* Class: KWGUI                                                */
/******************************************************************/
KWGUI::KWGUI( const QString& viewMode, QWidget *parent, KWView *daView )
  : KHBox( parent, "" ),
    m_view ( daView )
{

    KWDocument * doc = m_view->kWordDocument();

    m_horRuler  = 0;
    m_vertRuler = 0;

    // The splitter
    m_panner = new QSplitter( Qt::Horizontal, this );

    // The left side
    m_docStruct = new KWDocStruct( m_panner, doc, this );
    m_docStruct->setMinimumWidth( 0 );

    // The right side
    m_right = new QWidget( m_panner );
    Q3GridLayout *gridLayout = new Q3GridLayout( m_right, 2, 2 );
    m_canvas = new KWCanvas( viewMode, m_right, doc, this );
    gridLayout->addWidget( m_canvas, 1, 1 );

    Q3ValueList<int> l;
    l << 10;
    l << 90;
    m_panner->setSizes( l );

    KoPageLayout layout = doc->pageLayout();

    m_tabChooser = new KoTabChooser( m_right, KoTabChooser::TAB_ALL );
    m_tabChooser->setReadWrite(doc->isReadWrite());
    gridLayout->addWidget( m_tabChooser, 0, 0 );

    m_horRuler = new KoRuler( m_right, m_canvas->viewport(), Qt::Horizontal, layout,
			      KoRuler::F_INDENTS | KoRuler::F_TABS,
			      doc->unit(), m_tabChooser );
    m_horRuler->setReadWrite(doc->isReadWrite());
    gridLayout->addWidget( m_horRuler, 0, 1 );

    m_vertRuler = new KoRuler( m_right, m_canvas->viewport(), Qt::Vertical, layout,
			       0, doc->unit() );
    m_vertRuler->setReadWrite(doc->isReadWrite());
    gridLayout->addWidget( m_vertRuler, 1, 0 );

    m_horRuler->setZoom( doc->zoomedResolutionX() );
    m_vertRuler->setZoom( doc->zoomedResolutionY() );

    m_horRuler->setGridSize(doc->gridX());

    connect( m_horRuler, SIGNAL( newPageLayout( const KoPageLayout & ) ), m_view, SLOT( newPageLayout( const KoPageLayout & ) ) );
    connect( m_horRuler, SIGNAL( newLeftIndent( double ) ), m_view, SLOT( newLeftIndent( double ) ) );
    connect( m_horRuler, SIGNAL( newFirstIndent( double ) ), m_view, SLOT( newFirstIndent( double ) ) );
    connect( m_horRuler, SIGNAL( newRightIndent( double ) ), m_view, SLOT( newRightIndent( double ) ) );

    connect( m_horRuler, SIGNAL( doubleClicked() ), m_view, SLOT( slotHRulerDoubleClicked() ) );
    connect( m_horRuler, SIGNAL( doubleClicked(double) ), m_view, SLOT( slotHRulerDoubleClicked(double) ) );
    connect( m_horRuler, SIGNAL( unitChanged( KoUnit::Unit ) ), this, SLOT( unitChanged( KoUnit::Unit ) ) );
    connect( m_vertRuler, SIGNAL( newPageLayout( const KoPageLayout & ) ), m_view, SLOT( newPageLayout( const KoPageLayout & ) ) );
    connect( m_vertRuler, SIGNAL( doubleClicked() ), m_view, SLOT( formatPage() ) );
    connect( m_vertRuler, SIGNAL( unitChanged( KoUnit::Unit ) ), this, SLOT( unitChanged( KoUnit::Unit ) ) );

    m_horRuler->hide();
    m_vertRuler->hide();

    m_canvas->show();

    reorganize();

    connect( m_horRuler, SIGNAL( tabListChanged( const KoTabulatorList & ) ), m_view,
             SLOT( tabListChanged( const KoTabulatorList & ) ) );

    setKeyCompression( TRUE );
    setAcceptDrops( TRUE );
    setFocusPolicy( Qt::NoFocus );
}

void KWGUI::showGUI()
{
    reorganize();
}

void KWGUI::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    reorganize();
}

void KWGUI::reorganize()
{
    int hSpace = m_vertRuler->minimumSizeHint().width();
    int vSpace = m_horRuler->minimumSizeHint().height();
    if(m_view->kWordDocument()->showRuler())
    {
        m_vertRuler->show();
        m_horRuler->show();
        m_tabChooser->show();
        m_tabChooser->setGeometry( 0, 0, hSpace, vSpace );
    }
    else
    {
        m_vertRuler->hide();
        m_horRuler->hide();
        m_tabChooser->hide();
        hSpace = 0;
        vSpace = 0;
    }

    if(m_view->kWordDocument()->showdocStruct()) {
        if(m_docStruct->isHidden()) {
            m_docStruct->show();
            if(m_panner->sizes()[0] < 50) {
                Q3ValueList<int> l;
                l << 100;
                l << width()-100;
                m_panner->setSizes( l );
            }
        }
    } else
        m_docStruct->hide();

    if( m_view->statusBar())
    {
        if(m_view->kWordDocument()->showStatusBar())
            m_view->statusBar()->show();
        else
            m_view->statusBar()->hide();
    }

    if ( m_view->kWordDocument()->showScrollBar())
    {
        m_canvas->setVScrollBarMode(Q3ScrollView::Auto);
        m_canvas->setHScrollBarMode(Q3ScrollView::Auto);
    }
    else
    {
        m_canvas->setVScrollBarMode(Q3ScrollView::AlwaysOff);
        m_canvas->setHScrollBarMode(Q3ScrollView::AlwaysOff);
    }
}

void KWGUI::unitChanged( KoUnit::Unit u )
{
    m_view->kWordDocument()->setUnit( u );
}

QPoint KWView::applyViewTransformations( const QPoint& p ) const
{
    return viewMode()->normalToView( m_doc->zoomPoint( KoPoint( p ) ) );
}

QPoint KWView::reverseViewTransformations( const QPoint& p ) const
{
    return m_doc->unzoomPoint( viewMode()->viewToNormal( p ) ).toQPoint();
}

int KWView::currentPage() const {
    return m_currentPage->pageNumber();
}

void KWView::slotDocumentLoadingCompleted()
{
    updateStyleList();
}

#include "KWView.moc"
