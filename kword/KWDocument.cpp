/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2002-2005 David Faure <faure@kde.org>
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
 * Boston, MA 02110-1301, USA.
*/

#include "KWDocument.h"

#include "KWordDocIface.h"
#include "KWBgSpellCheck.h"
#include "KWBookMark.h"
#include "KWCanvas.h"
#include "KWCommand.h"
#include "KWFormulaFrameSet.h"
#include "KWFrameLayout.h"
#include "KWPartFrameSet.h"
#include "KWTableFrameSet.h"
#include "KWTableStyle.h"
#include "KWTableTemplate.h"
#include "KWTextImage.h"
#include "KWVariable.h"
#include "KWView.h"
#include "KWViewMode.h"
#include "KWMailMergeDataBase.h"
#include "KWLoadingInfo.h"
#include "KWCollectFramesetsVisitor.h"
#include "KWOasisLoader.h"
#include "KWOasisSaver.h"
#include "KWFrameList.h"
#include "KWPageManager.h"
#include "KWPage.h"

#include <koPictureCollection.h>
#include <koTemplateChooseDia.h>
#include <koMainWindow.h>
#include <koDocumentInfo.h>
#include <koGlobal.h>
#include <koparagcounter.h>
#include <kotextobject.h>
#include <koAutoFormat.h>
#include <kovariable.h>
#include <kformuladocument.h>
#include <koApplication.h>
#include <kooasiscontext.h>
#include <kocommandhistory.h>
#include <koGenStyles.h>
#include <koStore.h>
#include <koStoreDrag.h>
#include <koStoreDevice.h>
#include <koxmlwriter.h>
#include <koOasisStore.h>
#include <koOasisStyles.h>
#include <koxmlns.h>
#include <kodom.h>

#include <kcursor.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <klibloader.h>
#include <kmultipledrag.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kspell.h>
#include <kstandarddirs.h>

#include <kspell2/settings.h>

#include <qfileinfo.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qbuffer.h>

#include <unistd.h>
#include <math.h>

//#define DEBUG_PAGES
//#define DEBUG_SPEED

// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
static const char * CURRENT_DTD_VERSION = "1.2";

/******************************************************************/
/* Class: KWCommandHistory                                        */
/******************************************************************/
class KWCommandHistory : public KoCommandHistory
{
public:
    KWCommandHistory( KWDocument * doc ) : KoCommandHistory( doc->actionCollection(),  true ), m_pDoc( doc ) {}
public /*slots*/: // They are already slots in the parent. Running moc on the inherited class shouldn't be necessary AFAICS.
    virtual void undo();
    virtual void redo();
private:
    KWDocument * m_pDoc;
};

void KWCommandHistory::undo()
{
    m_pDoc->clearUndoRedoInfos();
    KoCommandHistory::undo();
}

void KWCommandHistory::redo()
{
    m_pDoc->clearUndoRedoInfos();
    KoCommandHistory::redo();
}

void KWDocument::clearUndoRedoInfos()
{
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet *>( fit.current() );
        if ( fs )
            fs->clearUndoRedoInfo();
    }
}

/**
 * Temporary storage for the initial edition info
 * (activeFrameset, cursorParagraph and cursorIndex attributes of the XML)
 */
class KWDocument::InitialEditing {
public:
    QString m_initialFrameSet;
    int m_initialCursorParag;
    int m_initialCursorIndex;
};

/******************************************************************/
/* Class: KWDocument                                              */
/******************************************************************/
const int KWDocument::CURRENT_SYNTAX_VERSION = 3;

KWDocument::KWDocument(QWidget *parentWidget, const char *widname, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widname, parent, name, singleViewMode ),
      m_urlIntern()
{
    KWStatisticVariable::setExtendedType(  true );
    dcop = 0;
    if (KGlobal::locale()->measureSystem() == KLocale::Imperial) {
        m_unit = KoUnit::U_INCH;
    } else {
        m_unit = KoUnit::U_CM;
    }
    m_pageManager = new KWPageManager();
    m_pageManager->appendPage();
    m_loadingInfo = 0L;
    m_tabStop = MM_TO_POINT( 15.0 );
    m_processingType = WP;

//    varFormats.setAutoDelete(true);
    m_lstFrameSet.setAutoDelete( true );
    // m_textImageRequests does not create or delete the KWTextImage classes
    m_textImageRequests.setAutoDelete(false);
    m_bookmarkList.setAutoDelete( true );

    m_styleColl=new KoStyleCollection();
    m_frameStyleColl = new KWFrameStyleCollection();
    m_tableStyleColl = new KWTableStyleCollection();
    m_tableTemplateColl = new KWTableTemplateCollection();
    m_pictureCollection = new KoPictureCollection();

    m_personalExpressionPath = KWFactory::instance()->dirs()->resourceDirs("expression");
    m_picturePath= KGlobalSettings::documentPath();

    m_bShowGrid = false;
    m_bSnapToGrid = false;

#if 0 // KWORD_HORIZONTAL_LINE
    m_horizontalLinePath = KWFactory::instance()->dirs()->resourceDirs("horizontalLine");
#endif

    setInstance( KWFactory::instance(), false );

    m_gridX = m_gridY = MM_TO_POINT(5.0 );
    m_indent = MM_TO_POINT( 10.0 );

    m_iNbPagePerRow = 4;
    m_maxRecentFiles = 10;
    m_defaultColumnSpacing=3;
    m_bShowRuler = true;

    m_footNoteSeparatorLinePos=SLP_LEFT;

    m_iFootNoteSeparatorLineLength = 20; // 20%, i.e. 1/5th
    m_footNoteSeparatorLineWidth = 2.0;
    m_footNoteSeparatorLineType = SLT_SOLID;

    m_viewFormattingChars = false;

    m_viewFormattingEndParag = true;
    m_viewFormattingSpace = true;
    m_viewFormattingTabs = true;
    m_viewFormattingBreak = true;

    m_viewFrameBorders = true;
    m_repaintAllViewsPending = false;
    m_recalcFramesPending = -1;
    m_bShowDocStruct = true;
    m_bShowRuler = true;
    m_bShowStatusBar = true;
    m_bAllowAutoFormat = true;
    m_pgUpDownMovesCaret = true;
    m_bShowScrollBar = true;
    m_cursorInProtectectedArea = true;
    m_bHasEndNotes = false;

    m_bInsertDirectCursor=false;
    m_globalLanguage = KGlobal::locale()->language();
    m_bGlobalHyphenation = false;
    m_bGeneratingPreview = false;
    m_lastViewMode="ModeNormal";
    m_viewMode = 0;

    m_commandHistory = new KWCommandHistory( this );
    connect( m_commandHistory, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
    connect( m_commandHistory, SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );

    //styleMask = U_FONT_FAMILY_ALL_SIZE | U_COLOR | U_BORDER | U_INDENT |
    //                     U_NUMBERING | U_ALIGN | U_TABS | U_SMART;
    m_headerVisible = false;
    m_footerVisible = false;

    m_pasteFramesetsMap = 0L;
    m_initialEditing = 0L;
    m_bufPixmap = 0L;
    m_varFormatCollection = new KoVariableFormatCollection;
    m_varColl = new KWVariableCollection( new KWVariableSettings(), m_varFormatCollection );

    m_autoFormat = new KoAutoFormat(this,m_varColl,m_varFormatCollection );
    m_bgSpellCheck = new KWBgSpellCheck(this);
    m_slDataBase = new KWMailMergeDataBase( this );
    slRecordNum = -1;

    m_syntaxVersion = CURRENT_SYNTAX_VERSION;

    m_hasTOC=false;

    // It's important to call this to have the kformula actions
    // created. The real document is still to be created if needed.
    m_formulaDocumentWrapper =
        new KFormula::DocumentWrapper( instance()->config(),
                                       actionCollection(),
                                       m_commandHistory );

    setEmpty();
    setModified(false);

    initConfig();

    // Get default font from the KWord config file
    KConfig *config = KWFactory::instance()->config();
    config->setGroup("Document defaults" );
    QString defaultFontname=config->readEntry("DefaultFont");
    if ( !defaultFontname.isEmpty() )
        m_defaultFont.fromString( defaultFontname );
    // If not found, we automatically fallback to the application font (the one from KControl's font module)

    // Try to force a scalable font.
    m_defaultFont.setStyleStrategy( QFont::ForceOutline );

    int ptSize = m_defaultFont.pointSize();
    if ( ptSize == -1 ) // specified with a pixel size ?
        ptSize = QFontInfo(m_defaultFont).pointSize();

    //kdDebug() << "Default font: requested family: " << m_defaultFont.family() << endl;
    //kdDebug() << "Default font: real family: " << QFontInfo(m_defaultFont).family() << endl;

    if ( name )
        dcopObject();
    connect(m_varColl,SIGNAL(repaintVariable()),this,SLOT(slotRepaintVariable()));
}

/*==============================================================*/
DCOPObject* KWDocument::dcopObject()
{
    if ( !dcop )
        dcop = new KWordDocIface( this );
    return dcop;
}



KWDocument::~KWDocument()
{
    //don't save config when kword is embedded into konqueror
    if(isReadWrite())
        saveConfig();
    // formula frames have to be deleted before m_formulaDocumentWrapper
    m_lstFrameSet.clear();
    m_bookmarkList.clear();
    delete m_loadingInfo;
    delete m_autoFormat;
    delete m_formulaDocumentWrapper;
    delete m_commandHistory;
    delete m_varColl;
    delete m_varFormatCollection;
    delete m_slDataBase;
    delete dcop;
    delete m_bgSpellCheck;
    delete m_styleColl;
    delete m_frameStyleColl;
    delete m_tableStyleColl;
    delete m_tableTemplateColl;
    delete m_viewMode;
    delete m_bufPixmap;
    delete m_pictureCollection;
}

void KWDocument::initConfig()
{
  KConfig *config = KWFactory::instance()->config();
  if( config->hasGroup("KSpell kword" ) )
  {
      config->setGroup( "KSpell kword" );

      // Default is false for spellcheck, but the spell-check config dialog
      // should write out "true" when the user configures spell checking.
      if ( isReadWrite() )
          m_bgSpellCheck->setEnabled(config->readBoolEntry( "SpellCheck", false ));
      else
          m_bgSpellCheck->setEnabled( false );
  }

  if(config->hasGroup("Interface" ) )
  {
      config->setGroup( "Interface" );
      setGridY(QMAX( config->readDoubleNumEntry("GridY",MM_TO_POINT(5.0) ), 0.1));
      setGridX(QMAX( config->readDoubleNumEntry("GridX",MM_TO_POINT(5.0) ), 0.1));
      setCursorInProtectedArea( config->readBoolEntry( "cursorInProtectArea", true ));
      // Config-file value in mm, default 10 pt
      double indent = config->readDoubleNumEntry("Indent", MM_TO_POINT(10.0) ) ;
      setIndentValue(indent);
      setShowRuler(config->readBoolEntry("Rulers",true));
      int defaultAutoSave = KoDocument::defaultAutoSave()/60; // in minutes
      setAutoSave(config->readNumEntry("AutoSave",defaultAutoSave)*60); // read key in minutes, call setAutoSave(seconds)
      setBackupFile( config->readBoolEntry("BackupFile", true) );

      setNbPagePerRow(config->readNumEntry("nbPagePerRow",4));
      m_maxRecentFiles = config->readNumEntry( "NbRecentFile", 10 );

      m_viewFormattingChars = config->readBoolEntry( "ViewFormattingChars", false );
      m_viewFormattingBreak = config->readBoolEntry( "ViewFormattingBreaks", true );
      m_viewFormattingSpace = config->readBoolEntry( "ViewFormattingSpace", true );
      m_viewFormattingEndParag = config->readBoolEntry( "ViewFormattingEndParag", true );
      m_viewFormattingTabs = config->readBoolEntry( "ViewFormattingTabs", true );

      m_viewFrameBorders = config->readBoolEntry( "ViewFrameBorders", true );

      m_zoom = config->readNumEntry( "Zoom", 100 );
      m_bShowDocStruct = config->readBoolEntry( "showDocStruct", true );
      m_lastViewMode = config->readEntry( "viewmode", "ModeNormal" );
      setShowStatusBar( config->readBoolEntry( "ShowStatusBar" , true ) );
      setAllowAutoFormat( config->readBoolEntry( "AllowAutoFormat" , true ) );
      setShowScrollBar( config->readBoolEntry( "ShowScrollBar", true ) );
      if ( isEmbedded() )
          m_bShowDocStruct = false; // off by default for embedded docs, but still toggleable
      m_pgUpDownMovesCaret = config->readBoolEntry( "PgUpDownMovesCaret", true );
      m_bInsertDirectCursor= config->readBoolEntry( "InsertDirectCursor", false );
      m_globalLanguage=config->readEntry("language", KGlobal::locale()->language());
      m_bGlobalHyphenation=config->readBoolEntry("hyphenation", false);

      setShowGrid( config->readBoolEntry( "ShowGrid" , false ));
      setSnapToGrid( config->readBoolEntry( "SnapToGrid", false ));
      setGridX( config->readDoubleNumEntry( "ResolutionX", MM_TO_POINT( 5.0 ) ));
      setGridY( config->readDoubleNumEntry( "ResolutionY", MM_TO_POINT( 5.0 ) ));
  }
  else
      m_zoom = 100;
  int undo=30;
  if(config->hasGroup("Misc" ) )
  {
      config->setGroup( "Misc" );
      undo=config->readNumEntry("UndoRedo",-1);
  }
  if(undo!=-1)
      setUndoRedoLimit(undo);

  setZoomAndResolution( m_zoom, KoGlobal::dpiX(), KoGlobal::dpiY() );

  //text mode view is not a good default for a readonly document...
  if ( !isReadWrite() && m_lastViewMode =="ModeText" )
      m_lastViewMode= "ModeNormal";

  m_viewMode = KWViewMode::create( m_lastViewMode, this );

  if(config->hasGroup("Kword Path" ) )
  {
      config->setGroup( "Kword Path" );
      if ( config->hasKey( "expression path" ) )
          m_personalExpressionPath = config->readPathListEntry( "expression path" );
#if 0 // KWORD_HORIZONTAL_LINE
      if ( config->hasKey( "horizontal line path" ) )
          m_horizontalLinePath = config->readPathListEntry( "horizontal line path" );
#endif
      if ( config->hasKey( "picture path" ) )
          m_picturePath = config->readPathEntry( "picture path" );
      setBackupPath(config->readPathEntry( "backup path" ));
  }

  // Load personal dict
  KConfigGroup group( KoGlobal::kofficeConfig(), "Spelling" );
  m_spellCheckPersonalDict = group.readListEntry( "PersonalDict" );
}

void KWDocument::saveConfig()
{
    if ( !isReadWrite() )
        return;
    KConfigGroup group( KoGlobal::kofficeConfig(), "Spelling" );
    group.writeEntry( "PersonalDict", m_spellCheckPersonalDict );

    if ( !isEmbedded() )
    {
        // Only save the config that is manipulated by the UI directly.
        // The config from the config dialog is saved by the dialog itself.
        KConfig *config = KWFactory::instance()->config();
        config->setGroup( "Interface" );
        config->writeEntry( "ViewFormattingChars", m_viewFormattingChars );
        config->writeEntry( "ViewFormattingBreaks", m_viewFormattingBreak );
        config->writeEntry( "ViewFormattingEndParag", m_viewFormattingEndParag );
        config->writeEntry( "ViewFormattingTabs", m_viewFormattingTabs );
        config->writeEntry( "ViewFormattingSpace", m_viewFormattingSpace );
        config->writeEntry( "ViewFrameBorders", m_viewFrameBorders );
        config->writeEntry( "Zoom", m_zoom );
        config->writeEntry( "showDocStruct", m_bShowDocStruct);
        config->writeEntry( "Rulers", m_bShowRuler);
        config->writeEntry( "viewmode", m_lastViewMode);
        config->writeEntry( "AllowAutoFormat", m_bAllowAutoFormat );
        config->writeEntry( "ShowGrid" , m_bShowGrid );
        config->writeEntry( "SnapToGrid" , m_bSnapToGrid );
        config->writeEntry( "ResolutionX", m_gridX );
        config->writeEntry( "ResolutionY", m_gridY );
    }
}

void KWDocument::setZoomAndResolution( int zoom, int dpiX, int dpiY )
{
    KoTextZoomHandler::setZoomAndResolution( zoom, dpiX, dpiY );
    if ( KFormula::Document* formulaDocument = m_formulaDocumentWrapper->document() )
        formulaDocument->setZoomAndResolution( zoom, dpiX, dpiY );
}

KWTextFrameSet * KWDocument::textFrameSet ( unsigned int _num ) const
{
    unsigned int i=0;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        if(fit.current()->isDeleted()) continue;
        if(fit.current()->type()==FT_TEXT)
        {
            if(i==_num)
                return static_cast<KWTextFrameSet*>(fit.current());
            i++;
        }
    }
    return static_cast<KWTextFrameSet*>(m_lstFrameSet.getFirst());
}

void KWDocument::newZoomAndResolution( bool updateViews, bool forPrint )
{
    if ( KFormula::Document* formulaDocument = m_formulaDocumentWrapper->document() )
        formulaDocument->newZoomAndResolution( updateViews,forPrint );
#if 0
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->zoom( forPrint );
#endif

    // First recalc all frames (including the kotextdocument width)
    updateAllFrames();
    // Then relayout the text inside the frames
    layout();
    if ( updateViews )
    {
        emit newContentsSize();
        repaintAllViews( true );
    }
}

bool KWDocument::initDoc(InitDocFlags flags, QWidget* parentWidget)
{
    m_pageColumns.columns = 1;
    m_pageColumns.ptColumnSpacing = m_defaultColumnSpacing;

    m_pageHeaderFooter.header = HF_SAME;
    m_pageHeaderFooter.footer = HF_SAME;
    m_pageHeaderFooter.ptHeaderBodySpacing = 10;
    m_pageHeaderFooter.ptFooterBodySpacing = 10;
    m_pageHeaderFooter.ptFootNoteBodySpacing = 10;

    bool ok = FALSE;

    if ( isEmbedded() )
    {
      QString fileName( locate( "kword_template", "Normal/.source/Embedded.kwt" , KWFactory::instance() ) );
      resetURL();
      initUnit();
      ok = loadNativeFormat( fileName );
      if ( !ok )
        showLoadingErrorDialog();
      setEmpty();
      setModified( FALSE );
      return ok;
    }
    else if (flags==KoDocument::InitDocEmpty)
    {
        QString fileName( locate( "kword_template", "Normal/.source/PlainText.kwt" , KWFactory::instance() ) );
        resetURL();
        initUnit();
        ok = loadNativeFormat( fileName );
        if ( !ok )
            showLoadingErrorDialog();
        setEmpty();
        setModified( FALSE );
        return ok;
    }

    KoTemplateChooseDia::DialogType dlgtype;

    if (flags != KoDocument::InitDocFileNew)
        dlgtype = KoTemplateChooseDia::Everything;
    else
        dlgtype = KoTemplateChooseDia::OnlyTemplates;


    QString file;
    KoTemplateChooseDia::ReturnType ret = KoTemplateChooseDia::choose(
        KWFactory::instance(), file,
        dlgtype, "kword_template", parentWidget );
    if ( ret == KoTemplateChooseDia::Template ) {
        resetURL();
        ok = loadNativeFormat( file );
        if ( !ok )
            showLoadingErrorDialog();
        initUnit();
        setEmpty();
    } else if ( ret == KoTemplateChooseDia::File ) {
        KURL url( file );
        //kdDebug() << "KWDocument::initDoc opening URL " << url.prettyURL() << endl;
        ok = openURL( url );
    } else if ( ret == KoTemplateChooseDia::Empty ) {
        QString fileName( locate( "kword_template", "Normal/.source/PlainText.kwt" , KWFactory::instance() ) );
        resetURL();
        initUnit();
        ok = loadNativeFormat( fileName );
        if ( !ok )
            showLoadingErrorDialog();
        setEmpty();
    }
    setModified( FALSE );
    return ok;
}

void KWDocument::initUnit()
{
    //load default unit setting - this is only used for new files (from templates) or empty files
    KConfig *config = KWFactory::instance()->config();

    if (KGlobal::locale()->measureSystem() == KLocale::Imperial) {
        m_unit = KoUnit::U_INCH;
    } else {
        m_unit = KoUnit::U_CM;
    }

    if(config->hasGroup("Misc") )
    {
        config->setGroup( "Misc" );
        if ( config->hasKey( "Units" ) )
            setUnit( KoUnit::unit( config->readEntry("Units") ) );
        setDefaultColumnSpacing( config->readDoubleNumEntry("ColumnSpacing", 3.0) );
    }
    m_pageColumns.ptColumnSpacing = m_defaultColumnSpacing;
}

void KWDocument::initEmpty()
{
    m_pageColumns.columns = 1;
    m_pageColumns.ptColumnSpacing = m_defaultColumnSpacing;

    m_pageHeaderFooter.header = HF_SAME;
    m_pageHeaderFooter.footer = HF_SAME;
    m_pageHeaderFooter.ptHeaderBodySpacing = 10;
    m_pageHeaderFooter.ptFooterBodySpacing = 10;
    m_pageHeaderFooter.ptFootNoteBodySpacing = 10;

    QString fileName( locate( "kword_template", "Normal/.source/PlainText.kwt" , KWFactory::instance() ) );
    bool ok = loadNativeFormat( fileName );
    if ( !ok )
        showLoadingErrorDialog();
    resetURL();
    setModified( FALSE );
    setEmpty();
}

KoPageLayout KWDocument::pageLayout(int pageNumber /* = 0 */) const
{
    return pageManager()->pageLayout(pageNumber);
}

void KWDocument::setPageLayout( const KoPageLayout& _layout, const KoColumns& _cl, const KoKWHeaderFooter& _hf, bool updateViews )
{
    if ( m_processingType == WP ) {
        int numPages = pageCount();
        //kdDebug() << "KWDocument::setPageLayout WP" << endl;
        m_pageLayout = _layout;
        pageManager()->setDefaultPageSize(_layout.ptWidth, _layout.ptHeight);
        pageManager()->setDefaultPageMargins(_layout.ptTop, _layout.ptLeft, _layout.ptBottom, _layout.ptRight);
        m_pageColumns = _cl;
        m_pageHeaderFooter = _hf;

        if ( updateViews ) {
            // Fix things up, when we change the orientation we might accidentally change the number of pages
            // (and frames of the main textframeset might just remain un-moved...)
            KWFrameSet *frameset = m_lstFrameSet.getFirst();
            KWFrame* lastFrame = frameset->frame( frameset->frameCount() - 1 );
            if ( lastFrame && lastFrame->pageNumber() + 1 < numPages ) {
                kdDebug(32002) << "KWDocument::setPageLayout ensuring that recalcFrames will consider " << numPages << " pages." << endl;
                // All that matters is that it's on numPages so that all pages will be recalc-ed.
                // If the text layout then wants to remove some pages, no problem.
                KWPage *page =  pageManager()->page( numPages );
                lastFrame->setY( page->offsetInDocument() + page->topMargin() );
            }
        }

    } else {
        //kdDebug() << "KWDocument::setPageLayout NON-WP" << endl;
        pageManager()->setDefaultPageSize(_layout.ptWidth, _layout.ptHeight);
        pageManager()->setDefaultPageMargins(0, 0, 0, 0);
        m_pageLayout = _layout;
        m_pageLayout.ptLeft = 0;
        m_pageLayout.ptRight = 0;
        m_pageLayout.ptTop = 0;
        m_pageLayout.ptBottom = 0;
        m_pageHeaderFooter = _hf;
    }

    // pages have a different size -> update framesInPage
    // TODO: it would be better to move stuff so that text boxes remain in the same page...
    // (page-number preservation instead of Y preservation)
    updateAllFrames( KWFrameSet::UpdateFramesInPage );

    recalcFrames();

    updateAllFrames();

    if ( updateViews )
    {
        // Invalidate document layout, for proper repaint
        layout();
        emit pageLayoutChanged( m_pageLayout );
        updateResizeHandles();
        updateContentsSize();
    }
}


double KWDocument::ptColumnWidth() const
{
    KoPageLayout pl = pageManager()->pageLayout(0 /* real page nr? */);
    return ( pl.ptWidth - pl.ptLeft - pl.ptRight -
             ptColumnSpacing() * ( m_pageColumns.columns - 1 ) )
        / m_pageColumns.columns;
}

class KWFootNoteFrameSetList : public QPtrList<KWFootNoteFrameSet>
{
public:
    KWFootNoteFrameSetList( bool reversed ) : m_reversed( reversed ) {}
protected:
    // Compare the order of the associated variables
    virtual int compareItems(QPtrCollection::Item a, QPtrCollection::Item b)
    {
        KWFootNoteFrameSet* fsa = ((KWFootNoteFrameSet *)a);
        KWFootNoteFrameSet* fsb = ((KWFootNoteFrameSet *)b);
        Q_ASSERT( fsa->footNoteVariable() );
        Q_ASSERT( fsb->footNoteVariable() );
        if ( fsa->footNoteVariable() && fsb->footNoteVariable() )
        {
            int numa = fsa->footNoteVariable()->num();
            int numb = fsb->footNoteVariable()->num();
            if (numa == numb) return 0;
            if (numa > numb) return m_reversed ? -1 : 1;
            return m_reversed ? 1 : -1;
        }
        return -1; // whatever
    }
private:
    bool m_reversed;
};

/* append headers and footers if needed, and create enough pages for all the existing frames */
void KWDocument::recalcFrames( int fromPage, int toPage /*-1 for all*/, uint flags )
{
    if ( m_lstFrameSet.isEmpty() )
        return;
    //printDebug();
    kdDebug(32002) << "KWDocument::recalcFrames from=" << fromPage << " to=" << toPage << endl;

    KWFrameSet *frameset = m_lstFrameSet.getFirst();

    if ( m_processingType == WP ) { // In WP mode the pages are created automatically. In DTP not...

        KWTextFrameSet *firstHeader = 0L, *evenHeader = 0L, *oddHeader = 0L;
        KWTextFrameSet *firstFooter = 0L, *evenFooter = 0L, *oddFooter = 0L;
        m_bHasEndNotes = false; // will be set to true if we find any endnote

        // Lookup the various header / footer framesets into the variables above
        // [Done in all cases, in order to hide unused framesets]

        KWFootNoteFrameSetList footnotesList( true ); // Reversed, we want footnotes from bottom to top
        KWFootNoteFrameSetList endnotesList( false ); // Endnotes are in top to bottom order
        QPtrListIterator<KWFrameSet> fit = framesetsIterator();
        for ( ; fit.current() ; ++fit )
        {
            KWFrameSet * fs = fit.current();
            switch ( fs->frameSetInfo() ) {
            case KWFrameSet::FI_FIRST_HEADER:
                if ( isHeaderVisible() ) {
                    firstHeader = dynamic_cast<KWTextFrameSet*>( fs );
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
                break;
            case KWFrameSet::FI_ODD_HEADER:
                if ( isHeaderVisible() ) {
                    oddHeader = dynamic_cast<KWTextFrameSet*>( fs );
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
                break;
            case KWFrameSet::FI_EVEN_HEADER:
                if ( isHeaderVisible() ) {
                    evenHeader = dynamic_cast<KWTextFrameSet*>( fs );
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
                break;
            case KWFrameSet::FI_FIRST_FOOTER:
                if ( isFooterVisible() ) {
                    firstFooter = dynamic_cast<KWTextFrameSet*>( fs );
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
                break;
            case KWFrameSet::FI_ODD_FOOTER:
                if ( isFooterVisible() ) {
                    oddFooter = dynamic_cast<KWTextFrameSet*>( fs );
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
                break;
            case KWFrameSet::FI_EVEN_FOOTER:
                if ( isFooterVisible() ) {
                    evenFooter = dynamic_cast<KWTextFrameSet*>( fs );
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
            case KWFrameSet::FI_FOOTNOTE: {
                KWFootNoteFrameSet* fnfs = dynamic_cast<KWFootNoteFrameSet *>(fs);
                if ( fnfs && fnfs->isVisible() ) // not visible is when the footnote has been deleted
                {
                    if ( fnfs->isFootNote() )
                        footnotesList.append( fnfs );
                    else if ( fnfs->isEndNote() ) {
                        endnotesList.append( fnfs );
                        m_bHasEndNotes = true;
                    }
                }
            }
                break;
            default: break;
            }
        }

        // This allocation each time might slow things down a bit.
        // TODO KWHeaderFooterFrameSet : public KWTextFrameSet, and store the HeaderFooterFrameset data into there.
        // ... hmm, and then KWFootNoteFrameSet needs to inherit KWHeaderFooterFrameSet
        QPtrList<KWFrameLayout::HeaderFooterFrameset> headerFooterList;
        headerFooterList.setAutoDelete( true );

        // Now hide & forget the unused header/footer framesets (e.g. 'odd pages' if we are in 'all the same' mode etc.)
        if ( isHeaderVisible() ) {
            Q_ASSERT( firstHeader );
            Q_ASSERT( oddHeader );
            Q_ASSERT( evenHeader );
            switch ( headerType() ) {
            case HF_SAME:
                oddHeader->setVisible( true );
                evenHeader->setVisible( false );
                evenHeader->deleteAllCopies();
                firstHeader->setVisible( false );
                firstHeader->deleteAllCopies();

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddHeader, 0, -1, m_pageHeaderFooter.ptHeaderBodySpacing ) );
                break;
            case HF_FIRST_EO_DIFF: // added for koffice-1.2-beta2
                firstHeader->setVisible( true );
                oddHeader->setVisible( true );
                evenHeader->setVisible( true );

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             firstHeader, 0, 0, m_pageHeaderFooter.ptHeaderBodySpacing ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddHeader, 2, -1, m_pageHeaderFooter.ptHeaderBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Odd ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenHeader, 1, -1, m_pageHeaderFooter.ptHeaderBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Even ) );
                break;
            case HF_FIRST_DIFF:
                oddHeader->setVisible( true );
                evenHeader->setVisible( false );
                evenHeader->deleteAllCopies();
                firstHeader->setVisible( true );

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             firstHeader, 0, 0, m_pageHeaderFooter.ptHeaderBodySpacing ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddHeader, 1, -1, m_pageHeaderFooter.ptHeaderBodySpacing ) );
                break;
            case HF_EO_DIFF:
                oddHeader->setVisible( true );
                evenHeader->setVisible( true );
                firstHeader->setVisible( false );
                firstHeader->deleteAllCopies();

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddHeader, 0, -1, m_pageHeaderFooter.ptHeaderBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Odd ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenHeader, 1, -1, m_pageHeaderFooter.ptHeaderBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Even ) );
                break;
            }
        }
        if ( isFooterVisible() ) {
            Q_ASSERT( firstFooter );
            Q_ASSERT( oddFooter );
            Q_ASSERT( evenFooter );
            switch ( footerType() ) {
            case HF_SAME:
                oddFooter->setVisible( true );
                evenFooter->setVisible( false );
                evenFooter->deleteAllCopies();
                firstFooter->setVisible( false );
                firstFooter->deleteAllCopies();

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddFooter, 0, -1, m_pageHeaderFooter.ptFooterBodySpacing ) );
                break;
            case HF_FIRST_EO_DIFF: // added for koffice-1.2-beta2
                firstFooter->setVisible( true );
                oddFooter->setVisible( true );
                evenFooter->setVisible( true );

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             firstFooter, 0, 0, m_pageHeaderFooter.ptFooterBodySpacing ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddFooter, 2, -1, m_pageHeaderFooter.ptFooterBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Odd ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenFooter, 1, -1, m_pageHeaderFooter.ptFooterBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Even ) );
                break;
            case HF_FIRST_DIFF:
                oddFooter->setVisible( true );
                evenFooter->setVisible( false );
                evenFooter->deleteAllCopies();
                firstFooter->setVisible( true );

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             firstFooter, 0, 0, m_pageHeaderFooter.ptFooterBodySpacing ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddFooter, 1, -1, m_pageHeaderFooter.ptFooterBodySpacing ) );
                break;
            case HF_EO_DIFF:
                oddFooter->setVisible( true );
                evenFooter->setVisible( true );
                firstFooter->setVisible( false );
                firstFooter->deleteAllCopies();

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddFooter, 0, -1, m_pageHeaderFooter.ptFooterBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Odd ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenFooter, 1, -1, m_pageHeaderFooter.ptFooterBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Even ) );
                break;
            }
        }

        // The frameset order _on screen_ is:
        // Header
        // Main text frame (if WP)
        // Footnote_s_
        // Footer
        // In the list it will have to be from top and from bottom:
        // Header, Footer, Footnote from bottom to top
        QPtrList<KWFrameLayout::HeaderFooterFrameset> footnotesHFList;
        footnotesHFList.setAutoDelete( true );

        footnotesList.sort();
        QPtrListIterator<KWFootNoteFrameSet> fnfsIt( footnotesList );  // fnfs == "footnote frameset"
        for ( ; fnfsIt.current() ; ++fnfsIt )
        {
            KWFootNoteFrameSet* fnfs = fnfsIt.current();
            int pageNum = -42; //fnfs->footNoteVariable()->pageNumber(); // determined by KWFrameLayout
            KWFrameLayout::HeaderFooterFrameset* hff = new KWFrameLayout::HeaderFooterFrameset(
                fnfs, pageNum, pageNum,
                m_pageHeaderFooter.ptFootNoteBodySpacing,
                KWFrameLayout::HeaderFooterFrameset::All );

            // With other kind of framesets, the height is simply frame->height.
            // But for footnotes, the height to pass to KWFrameLayout is the sum of the frame heights.
            hff->m_height = 0;
            for (QPtrListIterator<KWFrame> f = fnfs->frameIterator(); f.current() ; ++f )
                hff->m_height += f.current()->height();

            footnotesHFList.append( hff );
        }

        // Endnotes, however are laid out from top to bottom.
        QPtrList<KWFrameLayout::HeaderFooterFrameset> endnotesHFList;
        endnotesHFList.setAutoDelete( true );

        endnotesList.sort();
        QPtrListIterator<KWFootNoteFrameSet> enfsIt( endnotesList );  // enfs == "endnote frameset"
        for ( ; enfsIt.current() ; ++enfsIt )
        {
            KWFootNoteFrameSet* enfs = enfsIt.current();
            KWFrameLayout::HeaderFooterFrameset* hff = new KWFrameLayout::HeaderFooterFrameset(
                enfs, -42, -42, // determined by KWFrameLayout
                m_pageHeaderFooter.ptFootNoteBodySpacing,
                KWFrameLayout::HeaderFooterFrameset::All );

            // The height to pass to KWFrameLayout is the sum of the frame heights.
            hff->m_height = 0;
            for (QPtrListIterator<KWFrame> f = enfs->frameIterator(); f.current() ; ++f )
                hff->m_height += f.current()->height();

            endnotesHFList.append( hff );
        }

        int oldPages = pageCount();

        // Determine number of pages - first from the text frames
        // - BUT NOT from the number of frames. Some people manage to end up with
        // multiple frames of textframeset1 on the same page(!)
        double maxBottom = frameset->frame( frameset->frameCount() -1 )->bottom();
        // Then from the other frames ( framesetNum > 0 )
        for (int m = numFrameSets() - 1; m > 0; m-- )
        {
            KWFrameSet *fs=frameSet(m);
            if ( fs->isVisible() && !fs->isAHeader() && !fs->isAFooter() && !fs->isFloating() && !fs->isFootEndNote() )
            {
                for (int n = fs->frameCount()-1; n >= 0 ; n--) {
                    //if ( n == fs->frameCount()-1 )
#ifdef DEBUG_PAGES
                    kdDebug(32002) << "KWDocument::recalcFrames frameset number " << m << " '" << fs->name()
                                   << "' frame " << n << " bottom=" << fs->frame(n)->bottom() << endl;
#endif
                    maxBottom = QMAX(maxBottom, fs->frame(n)->bottom());
                }
            }
        }
        KWPage *last = pageManager()->page(pageManager()->lastPageNumber());
        double docHeight = last->offsetInDocument() + last->height();
        while(docHeight <= maxBottom) {
            last = pageManager()->appendPage();
            docHeight += last->height();
        }

        if ( toPage == -1 )
            toPage = pageCount() - 1;
        if ( fromPage > toPage ) // this can happen with "endnotes only" pages :)
            fromPage = toPage; // ie. start at the last real page
        KWFrameLayout frameLayout( this, headerFooterList, footnotesHFList, endnotesHFList );
        frameLayout.layout( frameset, m_pageColumns.columns, fromPage, toPage, flags );

        // If the number of pages changed, update views and variables etc.
        // (now that the frame layout has been done)
        if ( pageCount() != oldPages && !m_bGeneratingPreview )
        {
            // Very much like the end of appendPage, but we don't want to call recalcFrames ;)
            emit newContentsSize();
            emit pageNumChanged();
            recalcVariables( VT_PGNUM );
        }

    }
    else {
        // DTP mode: calculate the number of pages from the frames.
        double maxBottom=0;
        for (QPtrListIterator<KWFrameSet> fit = framesetsIterator(); fit.current() ; ++fit ) {
            if(fit.current()->isDeleted()) continue;
            if(fit.current()->frameSetInfo()==KWFrameSet::FI_BODY && !fit.current()->isFloating()) {
                KWFrameSet * fs = fit.current();
                for (QPtrListIterator<KWFrame> f = fs->frameIterator(); f.current() ; ++f ) {
#ifdef DEBUG_PAGES
                    kdDebug(32002) << " fs=" << fs->name() << " bottom=" << f.current()->bottom() << endl;
#endif
                    maxBottom=QMAX(maxBottom, f.current()->bottom());
                }
            }
        }
        KWPage *last = pageManager()->page(pageManager()->lastPageNumber());
        double docHeight = last->offsetInDocument() + last->height();
        while(docHeight <= maxBottom) {
            last = pageManager()->appendPage();
            docHeight += last->height();
        }
        if ( toPage == -1 )
            toPage = pageCount() - 1;
        KWFrameList::recalcFrames(this, fromPage, toPage);
    }
}

bool KWDocument::loadChildren( KoStore *_store )
{
    //kdDebug(32001) << "KWDocument::loadChildren" << endl;
    QPtrListIterator<KoDocumentChild> it( children() );
    for( ; it.current(); ++it ) {
        if ( !it.current()->loadDocument( _store ) )
            return FALSE;
    }

    return TRUE;
}

void KWDocument::loadPictureMap ( QDomElement& domElement )
{
    m_pictureMap.clear();

    // <PICTURES>
    QDomElement picturesElem = domElement.namedItem( "PICTURES" ).toElement();
    if ( !picturesElem.isNull() )
    {
       m_pictureCollection->readXML( picturesElem, m_pictureMap );
    }

    // <PIXMAPS>
    QDomElement pixmapsElem = domElement.namedItem( "PIXMAPS" ).toElement();
    if ( !pixmapsElem.isNull() )
    {
       m_pictureCollection->readXML( pixmapsElem, m_pictureMap );
    }

    // <CLIPARTS>
    QDomElement clipartsElem = domElement.namedItem( "CLIPARTS" ).toElement();
    if ( !clipartsElem.isNull() )
    {
       m_pictureCollection->readXML( pixmapsElem, m_pictureMap );
    }
}


bool KWDocument::loadOasis( const QDomDocument& doc, KoOasisStyles& oasisStyles, const QDomDocument& settings, KoStore* store )
{
    QTime dt;
    dt.start();
    emit sigProgress( 0 );
    clear();
    kdDebug(32001) << "KWDocument::loadOasis" << endl;

    KoColumns __columns;
    __columns.columns = 1;
    __columns.ptColumnSpacing = m_defaultColumnSpacing;
    KoKWHeaderFooter __hf;
    __hf.header = HF_SAME;
    __hf.footer = HF_SAME;
    __hf.ptHeaderBodySpacing = 10.0;
    __hf.ptFooterBodySpacing = 10.0;
    __hf.ptFootNoteBodySpacing = 10.0;

    QDomElement content = doc.documentElement();
    QDomElement realBody ( KoDom::namedItemNS( content, KoXmlNS::office, "body" ) );
    if ( realBody.isNull() )
    {
        kdError(32001) << "No office:body found!" << endl;
        setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No office:body tag found." ) );
        return false;
    }
    QDomElement body = KoDom::namedItemNS( realBody, KoXmlNS::office, "text" );
    if ( body.isNull() )
    {
        kdError(32001) << "No office:text found!" << endl;
        QDomElement childElem;
        QString localName;
        forEachElement( childElem, realBody ) {
            localName = childElem.localName();
        }
        if ( localName.isEmpty() )
            setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No tag found inside office:body." ) );
        else
            setErrorMessage( i18n( "This is not a word processing document, but %1. Please try opening it with the appropriate application." ).arg( KoDocument::tagNameToDocumentType( localName ) ) );
        return false;
    }

    // TODO check versions and mimetypes etc.

    QString masterPageName = "Standard"; // use default layout as fallback
    // In theory the page format is the style:master-page-name of the first paragraph...
    // But, hmm, in a doc with only a table there was no reference to the master page at all...
    QDomElement* masterPage = oasisStyles.masterPages()[ masterPageName ];
    Q_ASSERT( masterPage );
    QDomElement *masterPageStyle = masterPage ? oasisStyles.styles()[masterPage->attributeNS( KoXmlNS::style, "page-layout-name", QString::null )] : 0;
    Q_ASSERT( masterPageStyle );
    if ( masterPageStyle )
    {
        m_pageLayout.loadOasis( *masterPageStyle );
        if ( m_pageLayout.ptWidth <= 1e-13 || m_pageLayout.ptHeight <= 1e-13 )
        {
            // Loading page layout failed, try to see why.
            QDomElement properties( KoDom::namedItemNS( *masterPageStyle, KoXmlNS::style, "page-layout-properties" ) );
            //if ( properties.isNull() )
            //    setErrorMessage( i18n( "Invalid document. No page layout properties were found. The application which produced this document isn't OASIS-compliant." ) );
            //else if ( properties.hasAttributeNS( KoXmlNS::fo, "page-width" ) )
            //    setErrorMessage( i18n( "Invalid document. Page layout has no page width. The application which produced this document isn't OASIS-compliant." ) );
            //else
            if ( properties.hasAttributeNS( "http://www.w3.org/1999/XSL/Format", "page-width" ) )
                setErrorMessage( i18n( "Invalid document. 'fo' has the wrong namespace. The application which produced this document is not OASIS-compliant." ) );
            else
                setErrorMessage( i18n( "Invalid document. Paper size: %1x%2" ).arg( m_pageLayout.ptWidth ).arg( m_pageLayout.ptHeight ) );
            return false;
        }
        pageManager()->setDefaultPageSize(m_pageLayout.ptWidth, m_pageLayout.ptHeight);
        pageManager()->setDefaultPageMargins(m_pageLayout.ptTop, m_pageLayout.ptLeft,
                m_pageLayout.ptBottom, m_pageLayout.ptRight);

        //__hf.ptHeaderBodySpacing = getAttribute( paper, "spHeadBody", 0.0 );
        //__hf.ptFooterBodySpacing  = getAttribute( paper, "spFootBody", 0.0 );
        //__hf.ptFootNoteBodySpacing  = getAttribute( paper, "spFootNoteBody", 10.0 );

        const QDomElement properties( KoDom::namedItemNS( *masterPageStyle, KoXmlNS::style, "page-layout-properties" ) );
        const QDomElement footnoteSep = KoDom::namedItemNS( properties, KoXmlNS::style, "footnote-sep" );
        if ( !footnoteSep.isNull() ) {
            // style:width="0.018cm" style:distance-before-sep="0.101cm"
            // style:distance-after-sep="0.101cm" style:adjustment="left"
            // style:rel-width="25%" style:color="#000000"
            const QString width = footnoteSep.attributeNS( KoXmlNS::style, "width", QString::null );
            if ( !width.isEmpty() ) {
                m_footNoteSeparatorLineWidth = KoUnit::parseValue( width );
            }

            QString pageWidth = footnoteSep.attributeNS( KoXmlNS::style, "rel-width", QString::null );
            if ( pageWidth.endsWith( "%" ) ) {
                pageWidth.truncate( pageWidth.length() - 1 ); // remove '%'
                m_iFootNoteSeparatorLineLength = qRound( pageWidth.toDouble() );
            }
            // Not in KWord: color, distance before and after separator

            const QString style = footnoteSep.attributeNS( KoXmlNS::style, "line-style", QString::null );
            if ( style == "solid" || style.isEmpty() )
                m_footNoteSeparatorLineType = SLT_SOLID;
            else if ( style == "dash" )
                m_footNoteSeparatorLineType = SLT_DASH;
            else if ( style == "dotted" )
                m_footNoteSeparatorLineType = SLT_DOT;
            else if ( style == "dot-dash" )
                m_footNoteSeparatorLineType = SLT_DASH_DOT;
            else if ( style == "dot-dot-dash" )
                m_footNoteSeparatorLineType = SLT_DASH_DOT_DOT;
            else
                kdDebug() << "Unknown value for m_footNoteSeparatorLineType: " << style << endl;

            const QString pos = footnoteSep.attributeNS( KoXmlNS::style, "adjustment", QString::null );
            if ( pos == "centered" )
                m_footNoteSeparatorLinePos = SLP_CENTERED;
            else if ( pos == "right")
                m_footNoteSeparatorLinePos = SLP_RIGHT;
            else // if ( pos == "left" )
                m_footNoteSeparatorLinePos = SLP_LEFT;
        }

        __columns.columns = 1;
        __columns.ptColumnSpacing = m_defaultColumnSpacing;

        const QDomElement columnsElem = KoDom::namedItemNS( properties, KoXmlNS::style, "columns" );
        if ( !columnsElem.isNull() )
        {
            __columns.columns = columnsElem.attributeNS( KoXmlNS::fo, "column-count", QString::null ).toInt();
            if ( __columns.columns == 0 )
                __columns.columns = 1;
            // TODO OASIS OpenDocument supports columns of different sizes, using <style:column style:rel-width="...">
            // (with fo:start-indent/fo:end-indent for per-column spacing)
            // But well, it also allows us to specify a single gap.
            if ( columnsElem.hasAttributeNS( KoXmlNS::fo, "column-gap" ) )
                __columns.ptColumnSpacing = KoUnit::parseValue( columnsElem.attributeNS( KoXmlNS::fo, "column-gap", QString::null ) );
            // It also supports drawing a vertical line as a separator...
        }

        m_headerVisible = false;
        m_footerVisible = false;

        // TODO spHeadBody (where is this in OOo?)
        // TODO spFootBody (where is this in OOo?)
        // Answer: margins of the <style:header-footer> element
    }
    else // this doesn't happen with normal documents, but it can happen if copying something,
         // pasting into konq as foo.odt, then opening that...
    {
        __columns.columns = 1;
        __columns.ptColumnSpacing = 2;
        m_headerVisible = false;
        m_footerVisible = false;
        m_pageLayout = KoPageLayout::standardLayout();
        pageManager()->setDefaultPageSize(m_pageLayout.ptWidth, m_pageLayout.ptHeight);
        pageManager()->setDefaultPageMargins(m_pageLayout.ptTop, m_pageLayout.ptLeft,
                m_pageLayout.ptBottom, m_pageLayout.ptRight);
    }

    createLoadingInfo();
    KWOasisLoader oasisLoader( this );

    // <text:page-sequence> oasis extension for DTP (2003-10-27 post by Daniel)
    m_processingType = ( !KoDom::namedItemNS( body, KoXmlNS::text, "page-sequence" ).isNull() )
                       ? DTP : WP;

    // TODO settings (m_unit, spellcheck settings)

    m_hasTOC = false;
    m_tabStop = MM_TO_POINT(15);
    QDomElement* defaultParagStyle = oasisStyles.defaultStyle( "paragraph" );
    if ( defaultParagStyle ) {
        KoStyleStack stack;
        stack.push( *defaultParagStyle );
        stack.setTypeProperties( "paragraph" );
        QString tabStopVal = stack.attributeNS( KoXmlNS::style, "tab-stop-distance" );
        if ( !tabStopVal.isEmpty() )
            m_tabStop = KoUnit::parseValue( tabStopVal );
    }
    m_initialEditing = 0;

    // TODO MAILMERGE

    // Variable settings
    // By default display real variable value
    if ( !isReadWrite())
        m_varColl->variableSetting()->setDisplayFieldCode(false);

    KoOasisContext context( this, *m_varColl, oasisStyles, store );
    Q_ASSERT( !oasisStyles.officeStyle().isNull() );

    // Load all styles before the corresponding paragraphs try to use them!
    m_styleColl->loadOasisStyleTemplates( context );
    static_cast<KWVariableSettings *>( m_varColl->variableSetting() )
        ->loadNoteConfiguration( oasisStyles.officeStyle() );

    // TODO framestyles and tablestyles

    loadDefaultTableTemplates();

    if ( m_processingType == WP ) {
        // Create main frameset
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Main Text Frameset" ) );
        m_lstFrameSet.append( fs ); // don't use addFrameSet here. We'll call finalize() once and for all in completeLoading
        fs->loadOasisContent( body, context );
        KWFrame* frame = new KWFrame( fs, 29, 42, 798-29, 566-42 );
        frame->setFrameBehavior( KWFrame::AutoCreateNewFrame );
        frame->setNewFrameBehavior( KWFrame::Reconnect );
        fs->addFrame( frame );

        fs->renumberFootNotes( false /*no repaint*/ );

    } else {
        // DTP mode: the items in the body are page-sequence and then frames
        QDomElement tag;
        forEachElement( tag, body )
        {
            context.styleStack().save();
            const QString localName = tag.localName();
            if ( localName == "page-sequence" && tag.namespaceURI() == KoXmlNS::text )
            {
                // We don't have support for changing the page layout yet, so just take the
                // number of pages
                int pages=1;
                QDomElement page;
                forEachElement( page, tag )
                    ++pages;
                kdDebug() << "DTP mode: found " << pages << "pages" << endl;
                //setPageCount ( pages );
            }
            else if ( localName == "frame" && tag.namespaceURI() == KoXmlNS::draw )
                oasisLoader.loadFrame( tag, context );
            else
                kdWarning(32001) << "Unsupported tag in DTP loading:" << tag.tagName() << endl;
        }
    }

    // Header/Footer
    // TODO support for first-page
    bool hasEvenOddHeader = false;
    bool hasEvenOddFooter = false;
    if ( masterPageStyle )
    {
        QDomElement headerStyle = KoDom::namedItemNS( *masterPageStyle, KoXmlNS::style, "header-style" );
        QDomElement footerStyle = KoDom::namedItemNS( *masterPageStyle, KoXmlNS::style, "footer-style" );
        QDomElement headerLeftElem = KoDom::namedItemNS( *masterPage, KoXmlNS::style, "header-left" );
        QDomElement headerFirstElem = KoDom::namedItemNS( *masterPage, KoXmlNS::style, "header-first" ); // hack, not oasis compliant
        const bool hasFirstHeader = !headerFirstElem.isNull();
        if ( !headerLeftElem.isNull() )
        {
            hasEvenOddHeader = true;
            __hf.header = hasFirstHeader ? HF_FIRST_EO_DIFF : HF_EO_DIFF;
            oasisLoader.loadOasisHeaderFooter( headerLeftElem, hasEvenOddHeader, headerStyle, context );
        }
        else
        {
            __hf.header = hasFirstHeader ? HF_FIRST_DIFF : HF_SAME;
        }
        if ( hasFirstHeader )
        {
            oasisLoader.loadOasisHeaderFooter( headerFirstElem, hasEvenOddHeader, headerStyle, context );
        }

        QDomElement headerElem = KoDom::namedItemNS( *masterPage, KoXmlNS::style, "header" );
        if ( !headerElem.isNull() )
        {
            oasisLoader.loadOasisHeaderFooter( headerElem, hasEvenOddHeader, headerStyle, context );
        }

        // -- and now footers

        QDomElement footerLeftElem = KoDom::namedItemNS( *masterPage, KoXmlNS::style, "footer-left" );
        QDomElement footerFirstElem = KoDom::namedItemNS( *masterPage, KoXmlNS::style, "footer-first" ); // hack, not oasis compliant
        const bool hasFirstFooter = !footerFirstElem.isNull();
        if ( !footerLeftElem.isNull() )
        {
            hasEvenOddFooter = true;
            __hf.footer = hasFirstFooter ? HF_FIRST_EO_DIFF : HF_EO_DIFF;
            oasisLoader.loadOasisHeaderFooter( footerLeftElem, hasEvenOddFooter, footerStyle, context );
        }
        else
        {
            __hf.footer = hasFirstFooter ? HF_FIRST_DIFF : HF_SAME;
        }
        if ( hasFirstFooter )
        {
            oasisLoader.loadOasisHeaderFooter( footerFirstElem, hasEvenOddHeader, headerStyle, context );
        }
        QDomElement footerElem = KoDom::namedItemNS( *masterPage, KoXmlNS::style, "footer" );
        if ( !footerElem.isNull() )
        {
            oasisLoader.loadOasisHeaderFooter( footerElem, hasEvenOddFooter, footerStyle, context );
        }
    }

    // TODO embedded objects

    if ( context.cursorTextParagraph() ) {
        // Maybe, once 1.3-support is dropped, we can get rid of InitialEditing and fetch the
        // values from KoOasisContext? But well, it lives a bit longer.
        // At least we could store a KWFrameSet* and a KoTextParag* instead of a name and an id.
        m_initialEditing = new InitialEditing();
        KWTextFrameSet* fs = static_cast<KWTextDocument *>( context.cursorTextParagraph()->textDocument() )->textFrameSet();
        m_initialEditing->m_initialFrameSet = fs->name();
        m_initialEditing->m_initialCursorParag = context.cursorTextParagraph()->paragId();
        m_initialEditing->m_initialCursorIndex = context.cursorTextIndex();
    }

    kdDebug(32001) << "Loading took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;
    endOfLoading();

    // This sets the columns and header/footer flags, and calls recalcFrames,
    // so it must be done last.
    setPageLayout( m_pageLayout, __columns, __hf, false );

    if ( !settings.isNull() )
    {
        oasisLoader.loadOasisSettings( settings );
    }
    //printDebug();

    return true;
}

// called before loading
void KWDocument::clear()
{
    m_pictureMap.clear();
    m_textImageRequests.clear();
    m_pictureRequests.clear();
    m_anchorRequests.clear();
    m_footnoteVarRequests.clear();
    m_spellCheckIgnoreList.clear();
    m_pageColumns.columns = 1;
    m_pageColumns.ptColumnSpacing = m_defaultColumnSpacing;

    m_pageHeaderFooter.header = HF_SAME;
    m_pageHeaderFooter.footer = HF_SAME;
    m_pageHeaderFooter.ptHeaderBodySpacing = 10;
    m_pageHeaderFooter.ptFooterBodySpacing = 10;
    m_pageHeaderFooter.ptFootNoteBodySpacing = 10;
    m_pageColumns.columns = 1;
    m_pageColumns.ptColumnSpacing = m_defaultColumnSpacing;
    m_bHasEndNotes = false;

    m_varColl->clear();
    m_pictureCollection->clear();
    m_varFormatCollection->clear();

    m_styleColl->clear();
    m_frameStyleColl->clear();
    m_tableStyleColl->clear();
    m_tableTemplateColl->clear();

    // Some simple import filters don't define any style,
    // so let's have a Standard style at least
    KoParagStyle * standardStyle = new KoParagStyle( "Standard" ); // This gets translated later on
    //kdDebug() << "KWDocument::KWDocument creating standardStyle " << standardStyle << endl;
    standardStyle->format().setFont( m_defaultFont );
    m_styleColl->addStyleTemplate( standardStyle );

    // And let's do the same for framestyles
    KWFrameStyle * standardFrameStyle = new KWFrameStyle( "Plain" );
    standardFrameStyle->setBackgroundColor(Qt::white);
    standardFrameStyle->setTopBorder(KoBorder(Qt::black,KoBorder::SOLID,0));
    standardFrameStyle->setRightBorder(KoBorder(Qt::black,KoBorder::SOLID,0));
    standardFrameStyle->setLeftBorder(KoBorder(Qt::black,KoBorder::SOLID,0));
    standardFrameStyle->setBottomBorder(KoBorder(Qt::black,KoBorder::SOLID,0));
    m_frameStyleColl->addFrameStyleTemplate( standardFrameStyle );

    // And let's do the same for tablestyles
    KWTableStyle *standardTableStyle = new KWTableStyle( "Plain", standardStyle, standardFrameStyle );
    m_tableStyleColl->addTableStyleTemplate( standardTableStyle );
}

bool KWDocument::loadXML( QIODevice *, const QDomDocument & doc )
{
    QTime dt;
    dt.start();
    emit sigProgress( 0 );
    kdDebug(32001) << "KWDocument::loadXML" << endl;
    clear();

    KoPageLayout __pgLayout;
    KoColumns __columns;
    __columns.columns = 1;
    __columns.ptColumnSpacing = m_defaultColumnSpacing;
    KoKWHeaderFooter __hf;
    __hf.header = HF_SAME;
    __hf.footer = HF_SAME;
    __hf.ptHeaderBodySpacing = 10.0;
    __hf.ptFooterBodySpacing = 10.0;
    __hf.ptFootNoteBodySpacing = 10.0;

    QString value;
    QDomElement word = doc.documentElement();

    value = KWDocument::getAttribute( word, "mime", QString::null );
    if ( value.isEmpty() )
    {
        kdError(32001) << "No mime type specified!" << endl;
        setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
        return false;
    }
    else if ( value != "application/x-kword" && value != "application/vnd.kde.kword" )
    {
        kdError(32001) << "Unknown mime type " << value << endl;
        setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-kword or application/vnd.kde.kword, got %1" ).arg( value ) );
        return false;
    }
    m_syntaxVersion = KWDocument::getAttribute( word, "syntaxVersion", 0 );
    if ( m_syntaxVersion > CURRENT_SYNTAX_VERSION )
    {
        int ret = KMessageBox::warningContinueCancel(
            0, i18n("This document was created with a newer version of KWord (syntax version: %1)\n"
                    "Opening it in this version of KWord will lose some information.").arg(m_syntaxVersion),
            i18n("File Format Mismatch"), KStdGuiItem::cont() );
        if ( ret == KMessageBox::Cancel )
        {
            setErrorMessage( "USER_CANCELED" );
            return false;
        }
    }

    createLoadingInfo();

    // Looks like support for the old way of naming images internally,
    // see completeLoading.
    value = KWDocument::getAttribute( word, "url", QString::null );
    if ( !value.isNull() )
    {
        m_urlIntern = KURL( value ).path();
    }

    emit sigProgress(5);

    // <PAPER>
    QDomElement paper = word.namedItem( "PAPER" ).toElement();
    if ( !paper.isNull() )
    {
        __pgLayout.format = static_cast<KoFormat>( KWDocument::getAttribute( paper, "format", 0 ) );
        __pgLayout.orientation = static_cast<KoOrientation>( KWDocument::getAttribute( paper, "orientation", 0 ) );
        __pgLayout.ptWidth = getAttribute( paper, "width", 0.0 );
        __pgLayout.ptHeight = getAttribute( paper, "height", 0.0 );
	kdDebug() << " ptWidth=" << __pgLayout.ptWidth << endl;
	kdDebug() << " ptHeight=" << __pgLayout.ptHeight << endl;
        if ( __pgLayout.ptWidth <= 0 || __pgLayout.ptHeight <= 0 )
        {
            // Old document?
            __pgLayout.ptWidth = getAttribute( paper, "ptWidth", 0.0 );
            __pgLayout.ptHeight = getAttribute( paper, "ptHeight", 0.0 );
		kdDebug() << " ptWidth=" << __pgLayout.ptWidth << endl;
		kdDebug() << " ptHeight=" << __pgLayout.ptHeight << endl;

            // Still wrong?
            if ( __pgLayout.ptWidth <= 0 || __pgLayout.ptHeight <= 0 )
            {
                setErrorMessage( i18n( "Invalid document. Paper size: %1x%2" )
                    .arg( __pgLayout.ptWidth ).arg( __pgLayout.ptHeight ) );
                return false;
            }
        }

        __hf.header = static_cast<KoHFType>( KWDocument::getAttribute( paper, "hType", 0 ) );
        __hf.footer = static_cast<KoHFType>( KWDocument::getAttribute( paper, "fType", 0 ) );
        __hf.ptHeaderBodySpacing = getAttribute( paper, "spHeadBody", 0.0 );
        __hf.ptFooterBodySpacing  = getAttribute( paper, "spFootBody", 0.0 );
        __hf.ptFootNoteBodySpacing  = getAttribute( paper, "spFootNoteBody", 10.0 );
        m_iFootNoteSeparatorLineLength = getAttribute( paper, "slFootNoteLength", 20);
        m_footNoteSeparatorLineWidth = getAttribute( paper, "slFootNoteWidth",2.0);
        m_footNoteSeparatorLineType = static_cast<SeparatorLineLineType>(getAttribute( paper, "slFootNoteType",0));

        if ( paper.hasAttribute("slFootNotePosition"))
        {
            QString tmp =paper.attribute("slFootNotePosition");
            if ( tmp =="centered" )
                m_footNoteSeparatorLinePos = SLP_CENTERED;
            else if ( tmp =="right")
                m_footNoteSeparatorLinePos = SLP_RIGHT;
            else if ( tmp =="left" )
                m_footNoteSeparatorLinePos = SLP_LEFT;
        }
        __columns.columns = KWDocument::getAttribute( paper, "columns", 1 );
        __columns.ptColumnSpacing = KWDocument::getAttribute( paper, "columnspacing", 0.0 );
        // Now part of the app config
        //m_zoom = KWDocument::getAttribute( paper, "zoom", 100 );
        //if(m_zoom!=100)
        //    setZoomAndResolution( m_zoom, KoGlobal::dpiX(), KoGlobal::dpiY(), false, false );


        // Support the undocumented syntax actually used by KDE 2.0 for some of the above (:-().
        // Do not add anything to this block!
        if ( __pgLayout.ptWidth == 0.0 )
            __pgLayout.ptWidth = getAttribute( paper, "ptWidth", 0.0 );
        if ( __pgLayout.ptHeight == 0.0 )
            __pgLayout.ptHeight = getAttribute( paper, "ptHeight", 0.0 );
        if ( __hf.ptHeaderBodySpacing == 0.0 )
            __hf.ptHeaderBodySpacing = getAttribute( paper, "ptHeadBody", 0.0 );
        if ( __hf.ptFooterBodySpacing == 0.0 )
            __hf.ptFooterBodySpacing = getAttribute( paper, "ptFootBody", 0.0 );
        if ( __columns.ptColumnSpacing == 0.0 )
            __columns.ptColumnSpacing = getAttribute( paper, "ptColumnspc", 0.0 );

        // <PAPERBORDERS>
        QDomElement paperborders = paper.namedItem( "PAPERBORDERS" ).toElement();
        if ( !paperborders.isNull() )
        {
            __pgLayout.ptLeft = getAttribute( paperborders, "left", 0.0 );
            __pgLayout.ptTop = getAttribute( paperborders, "top", 0.0 );
            __pgLayout.ptRight = getAttribute( paperborders, "right", 0.0 );
            __pgLayout.ptBottom = getAttribute( paperborders, "bottom", 0.0 );

            // Support the undocumented syntax actually used by KDE 2.0 for some of the above (:-().
            if ( __pgLayout.ptLeft == 0.0 )
                __pgLayout.ptLeft = getAttribute( paperborders, "ptLeft", 0.0 );
            if ( __pgLayout.ptTop == 0.0 )
                __pgLayout.ptTop = getAttribute( paperborders, "ptTop", 0.0 );
            if ( __pgLayout.ptRight == 0.0 )
                __pgLayout.ptRight = getAttribute( paperborders, "ptRight", 0.0 );
            if ( __pgLayout.ptBottom == 0.0 )
                __pgLayout.ptBottom = getAttribute( paperborders, "ptBottom", 0.0 );
        }
        else
            kdWarning() << "No <PAPERBORDERS> tag!" << endl;
    }
    else
        kdWarning() << "No <PAPER> tag! This is a mandatory tag! Expect weird page sizes..." << endl;

    // <ATTRIBUTES>
    m_unit = KoUnit::U_CM;
    QDomElement attributes = word.namedItem( "ATTRIBUTES" ).toElement();
    if ( !attributes.isNull() )
    {
        m_processingType = static_cast<ProcessingType>( KWDocument::getAttribute( attributes, "processing", 0 ) );
        //KWDocument::getAttribute( attributes, "standardpage", QString::null );
        m_headerVisible = static_cast<bool>( KWDocument::getAttribute( attributes, "hasHeader", 0 ) );
        m_footerVisible = static_cast<bool>( KWDocument::getAttribute( attributes, "hasFooter", 0 ) );
        if ( attributes.hasAttribute( "unit" ) )
            m_unit = KoUnit::unit( attributes.attribute( "unit" ) );
        m_hasTOC =  static_cast<bool>(KWDocument::getAttribute( attributes,"hasTOC", 0 ) );
        m_tabStop = KWDocument::getAttribute( attributes, "tabStopValue", MM_TO_POINT(15) );
        m_initialEditing = new InitialEditing();
        m_initialEditing->m_initialFrameSet = attributes.attribute( "activeFrameset" );
        m_initialEditing->m_initialCursorParag = attributes.attribute( "cursorParagraph" ).toInt();
        m_initialEditing->m_initialCursorIndex = attributes.attribute( "cursorIndex" ).toInt();
    } else {
        m_processingType = WP;
        m_headerVisible = false;
        m_footerVisible = false;
        m_hasTOC = false;
        m_tabStop = MM_TO_POINT(15);
        delete m_initialEditing;
        m_initialEditing = 0L;
    }

    setPageLayout( __pgLayout, __columns, __hf, false );

    variableCollection()->variableSetting()->load(word );
    //by default display real variable value
    if ( !isReadWrite())
        variableCollection()->variableSetting()->setDisplayFieldCode(false);

    emit sigProgress(10);

    QDomElement mailmerge = word.namedItem( "MAILMERGE" ).toElement();
    if (mailmerge!=QDomElement())
    {
        m_slDataBase->load(mailmerge);
    }

    emit sigProgress(15);

    // Load all styles before the corresponding paragraphs try to use them!
    QDomElement stylesElem = word.namedItem( "STYLES" ).toElement();
    if ( !stylesElem.isNull() )
        loadStyleTemplates( stylesElem );

    emit sigProgress(17);

    QDomElement frameStylesElem = word.namedItem( "FRAMESTYLES" ).toElement();
    if ( !frameStylesElem.isNull() )
        loadFrameStyleTemplates( frameStylesElem );
    else // load default styles
        loadDefaultFrameStyleTemplates();

    emit sigProgress(18);

    QDomElement tableStylesElem = word.namedItem( "TABLESTYLES" ).toElement();
    if ( !tableStylesElem.isNull() )
        loadTableStyleTemplates( tableStylesElem );
    else // load default styles
        loadDefaultTableStyleTemplates();

    emit sigProgress(19);

    loadDefaultTableTemplates();

    emit sigProgress(20);

    QDomElement bookmark = word.namedItem( "BOOKMARKS" ).toElement();
    if( !bookmark.isNull() )
    {
        QDomElement bookmarkitem = word.namedItem("BOOKMARKS").toElement();
        bookmarkitem = bookmarkitem.firstChild().toElement();

        while ( !bookmarkitem.isNull() )
        {
            if ( bookmarkitem.tagName() == "BOOKMARKITEM" )
            {
                KWLoadingInfo::BookMark bk;
                bk.bookname=bookmarkitem.attribute("name");
                bk.cursorStartIndex=bookmarkitem.attribute("cursorIndexStart").toInt();
                bk.frameSetName=bookmarkitem.attribute("frameset");
                bk.paragStartIndex = bookmarkitem.attribute("startparag").toInt();
                bk.paragEndIndex = bookmarkitem.attribute("endparag").toInt();
                bk.cursorEndIndex = bookmarkitem.attribute("cursorIndexEnd").toInt();
                Q_ASSERT( m_loadingInfo );
                m_loadingInfo->bookMarkList.append( bk );
            }
            bookmarkitem = bookmarkitem.nextSibling().toElement();
        }
    }

    QStringList lst;
    QDomElement spellCheckIgnore = word.namedItem( "SPELLCHECKIGNORELIST" ).toElement();
    if( !spellCheckIgnore.isNull() )
    {
        QDomElement spellWord=word.namedItem("SPELLCHECKIGNORELIST").toElement();
        spellWord=spellWord.firstChild().toElement();
        while ( !spellWord.isNull() )
        {
            if ( spellWord.tagName()=="SPELLCHECKIGNOREWORD" )
                lst.append(spellWord.attribute("word"));
            spellWord=spellWord.nextSibling().toElement();
        }
    }
    setSpellCheckIgnoreList( lst );

    emit sigProgress(25);


    QDomElement framesets = word.namedItem( "FRAMESETS" ).toElement();
    if ( !framesets.isNull() )
        loadFrameSets( framesets );

    emit sigProgress(85);

    loadPictureMap( word );

    emit sigProgress(90);

    // <EMBEDDED>
    loadEmbeddedObjects( word );

    emit sigProgress(100); // the rest is only processing, not loading

    kdDebug(32001) << "Loading took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;

    endOfLoading();
    return true;
}

void KWDocument::endOfLoading()
{
    // insert pages
    double maxBottom = 0;
    for (QPtrListIterator<KWFrameSet> fsit = framesetsIterator(); fsit.current() ; ++fsit ) {
        KWFrameSet *fs = fsit.current();
        for (QPtrListIterator<KWFrame> fit = fs->frameIterator(); fit.current() ; ++fit ) {
            KWFrame *frame = fit.current();
            maxBottom = QMAX(maxBottom, frame->bottom());
        }
    }
    KWPage *last = pageManager()->page(pageManager()->lastPageNumber());
    double docHeight = last->offsetInDocument() + last->height();
    while(docHeight <= maxBottom) {
        kdDebug(32001) << "KWDocument::loadXML appends a page\n";
        last = pageManager()->appendPage();
        docHeight += last->height();
    }

    bool _first_footer = false, _even_footer = false, _odd_footer = false;
    bool _first_header = false, _even_header = false, _odd_header = false;

    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        switch( fit.current()->frameSetInfo() ) {
        case KWFrameSet::FI_FIRST_HEADER: _first_header = true; break;
        case KWFrameSet::FI_ODD_HEADER: _odd_header = true; break;
        case KWFrameSet::FI_EVEN_HEADER: _even_header = true; break;
        case KWFrameSet::FI_FIRST_FOOTER: _first_footer = true; break;
        case KWFrameSet::FI_ODD_FOOTER: _odd_footer = true; break;
        case KWFrameSet::FI_EVEN_FOOTER: _even_footer = true; break;
        case KWFrameSet::FI_FOOTNOTE: break;
        default: break;
        }
    }

    // Create defaults if they were not in the input file.

    // Where to insert the new frames: not at the end, since that breaks oasis-kword.sh
    uint newFramesetsIndex = m_lstFrameSet.isEmpty() ? 0 : 1;

    if ( !_first_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "First Page Header" ) );
        //kdDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameSetInfo( KWFrameSet::FI_FIRST_HEADER );
        KoPageLayout pl = pageManager()->pageLayout(1);
        KWFrame *frame = new KWFrame(fs, pl.ptLeft, pl.ptTop, pl.ptWidth - pl.ptLeft -
                pl.ptRight, 20 );
        //kdDebug(32001) << "KWDocument::loadXML KWFrame created " << frame << endl;
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.insert( newFramesetsIndex++, fs );
    }

    if ( !_odd_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Odd Pages Header" ) );
        //kdDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameSetInfo( KWFrameSet::FI_ODD_HEADER );
        KoPageLayout pl = pageManager()->pageLayout(3);
        KWFrame *frame = new KWFrame(fs, pl.ptLeft, pl.ptTop, pl.ptWidth - pl.ptLeft -
                pl.ptRight, 20 );
        //kdDebug(32001) << "KWDocument::loadXML KWFrame created " << *frame << endl;
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.insert( newFramesetsIndex++, fs );
    }

    if ( !_even_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Even Pages Header" ) );
        //kdDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameSetInfo( KWFrameSet::FI_EVEN_HEADER );
        KoPageLayout pl = pageManager()->pageLayout(2);
        KWFrame *frame = new KWFrame(fs, pl.ptLeft, pl.ptTop, pl.ptWidth - pl.ptLeft -
                pl.ptRight, 20 );
        //kdDebug(32001) << "KWDocument::loadXML KWFrame created " << *frame << endl;
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.insert( newFramesetsIndex++, fs );
    }

    if ( !_first_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "First Page Footer" ) );
        //kdDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameSetInfo( KWFrameSet::FI_FIRST_FOOTER );
        KoPageLayout pl = pageManager()->pageLayout(1);
        KWFrame *frame = new KWFrame(fs, pl.ptLeft, pl.ptHeight - pl.ptTop - 20,
                pl.ptWidth - pl.ptLeft - pl.ptRight, 20 );
        //kdDebug(32001) << "KWDocument::loadXML KWFrame created " << *frame << endl;
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.insert( newFramesetsIndex++, fs );
    }

    if ( !_odd_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Odd Pages Footer" ) );
        //kdDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameSetInfo( KWFrameSet::FI_ODD_FOOTER );
        KoPageLayout pl = pageManager()->pageLayout(3);
        KWFrame *frame = new KWFrame(fs, pl.ptLeft, pl.ptHeight - pl.ptTop - 20,
                pl.ptWidth - pl.ptLeft - pl.ptRight, 20 );
        //kdDebug(32001) << "KWDocument::loadXML KWFrame created " << *frame << endl;
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.insert( newFramesetsIndex++, fs );
    }

    if ( !_even_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Even Pages Footer" ) );
        //kdDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameSetInfo( KWFrameSet::FI_EVEN_FOOTER );
        KoPageLayout pl = pageManager()->pageLayout(2);
        KWFrame *frame = new KWFrame(fs, pl.ptLeft, pl.ptHeight - pl.ptTop - 20,
                pl.ptWidth - pl.ptLeft - pl.ptRight, 20 );
        //kdDebug(32001) << "KWDocument::loadXML KWFrame created " << *frame << endl;
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.insert( newFramesetsIndex++, fs );
    }

    // do some sanity checking on document.
    for (int i = numFrameSets()-1; i>-1; i--) {
        KWFrameSet *fs = frameSet(i);
        if(!fs) {
            kdWarning() << "frameset " << i << " is NULL!!" << endl;
            m_lstFrameSet.remove(i);
        } else if( fs->type()==FT_TABLE) {
            static_cast<KWTableFrameSet *>( fs )->validate();
        } else if (fs->type() == FT_TEXT) {
            for (int f=fs->frameCount()-1; f>=0; f--) {
                KWFrame *frame = fs->frame(f);
                if(frame->height() < s_minFrameHeight) {
                    kdWarning() << fs->name() << " frame " << f << " height is so small no text will fit, adjusting (was: "
                                << frame->height() << " is: " << s_minFrameHeight << ")" << endl;
                    frame->setHeight(s_minFrameHeight);
                }
                if(frame->width() < s_minFrameWidth) {
                    kdWarning() << fs->name() << " frame " << f << " width is so small no text will fit, adjusting (was: "
                                << frame->width() << " is: " << s_minFrameWidth  << ")" << endl;
                    frame->setWidth(s_minFrameWidth);
                }
                if(frame->left() < 0) {
                    kdWarning() << fs->name() << " frame " << f << " pos.x is < 0, moving frame" << endl;
                    frame->moveBy( 0- frame->left(), 0);
                }
                if(frame->right() > m_pageLayout.ptWidth) {
                    kdWarning() << fs->name() << " frame " << f << " rightborder outside page ("
                        << frame->right() << ">" << m_pageLayout.ptWidth << "), shrinking" << endl;
                    frame->setRight( m_pageLayout.ptWidth);
                }
            }
            if(fs->frameCount() == 0) {
                KWFrame *frame = new KWFrame(fs, m_pageLayout.ptLeft, m_pageLayout.ptTop,
                        m_pageLayout.ptWidth - m_pageLayout.ptLeft - m_pageLayout.ptRight,
                        m_pageLayout.ptHeight - m_pageLayout.ptTop - m_pageLayout.ptBottom);
                //kdDebug(32001) << "KWDocument::loadXML main-KWFrame created " << *frame << endl;
                fs->addFrame( frame );
            }
        } else if(fs->frameCount() == 0) {
            kdWarning () << "frameset " << i << " " << fs->name() << " has no frames" << endl;
            removeFrameSet(fs);
            if ( fs->type() == FT_PART )
                delete static_cast<KWPartFrameSet *>(fs)->getChild();
            delete fs;
        }
    }

    // Renumber footnotes
    KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet *>( m_lstFrameSet.getFirst() );
    if ( frameset  )
        frameset->renumberFootNotes( false /*no repaint*/ );

    emit sigProgress(-1);

    //kdDebug(32001) << "KWDocument::loadXML done" << endl;

    // Connect to notifications from main text-frameset
    if ( frameset ) {
        connect( frameset->textObject(), SIGNAL( chapterParagraphFormatted( KoTextParag * ) ),
                 SLOT( slotChapterParagraphFormatted( KoTextParag * ) ) );
        connect( frameset, SIGNAL( mainTextHeightChanged() ),
                 SIGNAL( mainTextHeightChanged() ) );
    }

    // Note that more stuff will happen in completeLoading
}

void KWDocument::startBackgroundSpellCheck()
{
    if ( backgroundSpellCheckEnabled() && isReadWrite() )
    {
        m_bgSpellCheck->start();
    }
}

void KWDocument::loadEmbeddedObjects( QDomElement& word )
{
    QDomNodeList listEmbedded = word.elementsByTagName ( "EMBEDDED" );
    for (unsigned int item = 0; item < listEmbedded.count(); item++)
    {
        QDomElement embedded = listEmbedded.item( item ).toElement();
        loadEmbedded( embedded );
    }
}

void KWDocument::loadEmbedded( const QDomElement &embedded )
{
    QDomElement object = embedded.namedItem( "OBJECT" ).toElement();
    if ( !object.isNull() )
    {
        KWChild *ch = new KWChild( this );
        ch->load( object, true );
        insertChild( ch );
        QDomElement settings = embedded.namedItem( "SETTINGS" ).toElement();
        QString name;
        if ( !settings.isNull() )
            name = settings.attribute( "name" );
        KWPartFrameSet *fs = new KWPartFrameSet( this, ch, name );
        m_lstFrameSet.append( fs );
        if ( !settings.isNull() )
        {
            kdDebug(32001) << "KWDocument::loadXML loading embedded object" << endl;
            fs->load( settings );
        }
        else
            kdError(32001) << "No <SETTINGS> tag in EMBEDDED" << endl;

    } else
        kdError(32001) << "No <OBJECT> tag in EMBEDDED" << endl;
}


void KWDocument::loadStyleTemplates( const QDomElement &stylesElem )
{
    QValueList<QString> followingStyles;
    QDomNodeList listStyles = stylesElem.elementsByTagName( "STYLE" );
    if( listStyles.count() > 0) { // we are going to import at least one style.
        KoParagStyle *s = m_styleColl->findStyle("Standard");
        //kdDebug(32001) << "KWDocument::loadStyleTemplates looking for Standard, to delete it. Found " << s << endl;
        if(s) // delete the standard style.
            m_styleColl->removeStyleTemplate(s);
    }
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KoParagStyle *sty = new KoParagStyle( QString::null );
        // Load the style from the <STYLE> element
        sty->loadStyle( styleElem, m_syntaxVersion );

        //kdDebug(32001) << "KoParagStyle created name=" << sty->name() << endl;

        if ( m_syntaxVersion < 3 )
        {
            // Convert old style (up to 1.2.x included)
            // "include in TOC if chapter numbering" to the new attribute
            if ( sty->paragLayout().counter && sty->paragLayout().counter->numbering() == KoParagCounter::NUM_CHAPTER )
                sty->setOutline( true );
        }

        // the real value of followingStyle is set below after loading all styles
        sty->setFollowingStyle( sty );

        QDomElement formatElem = styleElem.namedItem( "FORMAT" ).toElement();
        if ( !formatElem.isNull() )
            sty->format() = KWTextParag::loadFormat( formatElem, 0L, defaultFont(), globalLanguage(), globalHyphenation() );
        else
            kdWarning(32001) << "No FORMAT tag in <STYLE>" << endl; // This leads to problems in applyStyle().

        // Style created, now let's try to add it
        sty = m_styleColl->addStyleTemplate( sty );

        if(m_styleColl->styleList().count() > followingStyles.count() )
        {
            QString following = styleElem.namedItem("FOLLOWING").toElement().attribute("name");
            followingStyles.append( following );
        }
        else
            kdWarning () << "Found duplicate style declaration, overwriting former " << sty->name() << endl;
    }

    Q_ASSERT( followingStyles.count() == m_styleColl->styleList().count() );

    unsigned int i=0;
    for( QValueList<QString>::Iterator it = followingStyles.begin(); it != followingStyles.end(); ++it ) {
        KoParagStyle * style = m_styleColl->findStyle(*it);
        m_styleColl->styleAt(i++)->setFollowingStyle( style );
    }

}

void KWDocument::loadFrameStyleTemplates( const QDomElement &stylesElem )
{
    QDomNodeList listStyles = stylesElem.elementsByTagName( "FRAMESTYLE" );
    if( listStyles.count() > 0) { // we are going to import at least one style.
        KWFrameStyle *s = m_frameStyleColl->findFrameStyle("Plain");
        if(s) // delete the standard style.
            m_frameStyleColl->removeFrameStyleTemplate(s);
    }
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KWFrameStyle *sty = new KWFrameStyle( styleElem );
        m_frameStyleColl->addFrameStyleTemplate( sty );
    }
}

void KWDocument::loadDefaultFrameStyleTemplates()
{
    // ### FIXME: why is this not working for $KDE_LANG != "en_US" (#61007)
    const QString fsfileName( locate("appdata", "framestyles.xml") );

    kdDebug(30003) << "Data directory: " << KGlobal::dirs()->resourceDirs( "data" ) << endl;
    kdDebug(30003) << "Directory searched: " << KGlobal::dirs()->resourceDirs( "appdata" ) << endl;
    kdDebug(30003) << "File framestyles.xml searched at: " << fsfileName << endl;

    if ( ! QFile::exists( fsfileName ) )
    {
        kdWarning(30003) << "Cannot find any framestyles.xml" << endl;
        if (!m_frameStyleColl->findFrameStyle("Plain")) {
            KWFrameStyle * standardFrameStyle = new KWFrameStyle( "Plain" );
            standardFrameStyle->setBackgroundColor(QColor("white"));
            standardFrameStyle->setTopBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            standardFrameStyle->setRightBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            standardFrameStyle->setLeftBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            standardFrameStyle->setBottomBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            m_frameStyleColl->addFrameStyleTemplate( standardFrameStyle );
        }
        return;
    }

    kdDebug(30003) << "File framestyles.xml found!" << endl;

    // Open file and parse it
    QFile in( fsfileName );
    if ( !in.open( IO_ReadOnly ) )
    {
        //i18n( "Couldn't open the file for reading (check read permissions)" );
        kdWarning(30003) << "Couldn't open the file for reading (check read permissions)" << endl;
        return;
    }
    QString errorMsg;
    int errorLine;
    int errorColumn;
    QDomDocument doc;
    if ( ! doc.setContent( &in , &errorMsg, &errorLine, &errorColumn ) )
    {
        kdError (30003) << "Parsing Error! Aborting! (in KWDocument::loadDefaultFrameStyleTemplates())" << endl
                        << "  Line: " << errorLine << " Column: " << errorColumn << endl
                        << "  Message: " << errorMsg << endl;
    }
    in.close();

    // Start adding framestyles
    QDomElement stylesElem = doc.documentElement();

    QDomNodeList listStyles = stylesElem.elementsByTagName( "FRAMESTYLE" );
    if( listStyles.count() > 0) { // we are going to import at least one style.
        KWFrameStyle *s = m_frameStyleColl->findFrameStyle("Plain");
        if(s) // delete the standard style.
            m_frameStyleColl->removeFrameStyleTemplate(s);
    }
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KWFrameStyle *sty = new KWFrameStyle( styleElem );
        m_frameStyleColl->addFrameStyleTemplate( sty );
    }
}

void KWDocument::loadTableStyleTemplates( const QDomElement& stylesElem )
{
    QDomNodeList listStyles = stylesElem.elementsByTagName( "TABLESTYLE" );
    if( listStyles.count() > 0) { // we are going to import at least one style.
        KWTableStyle *s = m_tableStyleColl->findTableStyle("Plain");
        if(s) // delete the standard style.
            m_tableStyleColl->removeTableStyleTemplate(s);
    }
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KWTableStyle *sty = new KWTableStyle( styleElem, this );
        m_tableStyleColl->addTableStyleTemplate( sty );
    }
}

void KWDocument::loadDefaultTableStyleTemplates()
{
    KURL fsfile;

    if ( ! QFile::exists(locate("appdata", "tablestyles.xml")) )
    {
        if (!m_tableStyleColl->findTableStyle("Plain")) {
            m_tableStyleColl->addTableStyleTemplate( new KWTableStyle( "Plain", m_styleColl->styleAt(0), m_frameStyleColl->frameStyleAt(0) ) );
        }
        return;
    }

    fsfile.setPath( locate("appdata", "tablestyles.xml") );

    // Open file and parse it
    QFile in( fsfile.path() );
    if ( !in.open( IO_ReadOnly ) )
    {
        //i18n( "Couldn't open the file for reading (check read permissions)" );
        return;
    }
    in.at(0);
    QString errorMsg;
    int errorLine;
    int errorColumn;
    QDomDocument doc;
    if ( doc.setContent( &in , &errorMsg, &errorLine, &errorColumn ) ) {
    }
    else
    {
        kdError (30003) << "Parsing Error! Aborting! (in KWDocument::loadDefaultTableStyleTemplates())" << endl
                        << "  Line: " << errorLine << " Column: " << errorColumn << endl
                        << "  Message: " << errorMsg << endl;
    }
    in.close();

    // Start adding tablestyles
    QDomElement stylesElem = doc.documentElement();

    QDomNodeList listStyles = stylesElem.elementsByTagName( "TABLESTYLE" );
    if( listStyles.count() > 0) { // we are going to import at least one style.
        KWTableStyle *s = m_tableStyleColl->findTableStyle("Plain");
        if(s) // delete the standard style.
            m_tableStyleColl->removeTableStyleTemplate(s);
    }
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KWTableStyle *sty = new KWTableStyle( styleElem, this );
        m_tableStyleColl->addTableStyleTemplate( sty );
    }
}

void KWDocument::loadDefaultTableTemplates()
{
    KURL fsfile;

    if ( ! QFile::exists(locate("appdata", "tabletemplates.xml")) )
    {
        if (!m_tableTemplateColl->findTableTemplate("Plain")) {
            KWTableTemplate * standardTableTemplate = new KWTableTemplate( "Plain" );
            standardTableTemplate->setFirstRow(tableStyleCollection()->findTableStyle("Plain"));
            standardTableTemplate->setLastRow(tableStyleCollection()->findTableStyle("Plain"));
            standardTableTemplate->setFirstCol(tableStyleCollection()->findTableStyle("Plain"));
            standardTableTemplate->setLastCol(tableStyleCollection()->findTableStyle("Plain"));
            standardTableTemplate->setBodyCell(tableStyleCollection()->findTableStyle("Plain"));
            standardTableTemplate->setTopLeftCorner(tableStyleCollection()->findTableStyle("Plain"));
            standardTableTemplate->setTopRightCorner(tableStyleCollection()->findTableStyle("Plain"));
            standardTableTemplate->setBottomLeftCorner(tableStyleCollection()->findTableStyle("Plain"));
            standardTableTemplate->setBottomRightCorner(tableStyleCollection()->findTableStyle("Plain"));
            m_tableTemplateColl->addTableTemplate( standardTableTemplate );
        }
        return;
    }

    fsfile.setPath( locate("appdata", "tabletemplates.xml") );

    // Open file and parse it
    QFile in( fsfile.path() );
    if ( !in.open( IO_ReadOnly ) )
    {
        //i18n( "Couldn't open the file for reading (check read permissions)" );
        return;
    }
    in.at(0);
    QString errorMsg;
    int errorLine;
    int errorColumn;
    QDomDocument doc;
    if ( doc.setContent( &in , &errorMsg, &errorLine, &errorColumn ) ) {
    }
    else
    {
        kdError (30003) << "Parsing Error! Aborting! (in KWDocument::readTableTemplates())" << endl
                        << "  Line: " << errorLine << " Column: " << errorColumn << endl
                        << "  Message: " << errorMsg << endl;
    }
    in.close();

    // Start adding framestyles
    QDomElement templatesElem = doc.documentElement();

    QDomNodeList listTemplates = templatesElem.elementsByTagName( "TABLETEMPLATE" );
    if( listTemplates.count() > 0) {
        KWTableTemplate *s = m_tableTemplateColl->findTableTemplate("Plain");
        if(s)
            m_tableTemplateColl->removeTableTemplate(s);
    }
    for (unsigned int item = 0; item < listTemplates.count(); item++) {
        QDomElement templateElem = listTemplates.item( item ).toElement();

        KWTableTemplate *temp = new KWTableTemplate( templateElem, this );
        m_tableTemplateColl->addTableTemplate( temp );
    }
}

void KWDocument::progressItemLoaded()
{
    if ( !m_nrItemsToLoad ) // happens when pasting
        return;
    m_itemsLoaded++;
    // We progress from 20 to 85 -> 65-wide range, 20 offset.
    unsigned int perc = 65 * m_itemsLoaded / m_nrItemsToLoad;
    if ( perc != 65 * (m_itemsLoaded-1) / m_nrItemsToLoad ) // only emit if different from previous call
    {
        //kdDebug(32001) << m_itemsLoaded << " items loaded. %=" << perc + 20 << endl;
        emit sigProgress( perc + 20 );
    }
}

void KWDocument::loadFrameSets( const QDomElement &framesetsElem )
{
    // <FRAMESET>
    // First prepare progress info
    m_nrItemsToLoad = 0; // total count of items (mostly paragraph and frames)
    QDomElement framesetElem = framesetsElem.firstChild().toElement();
    // Workaround the slowness of QDom's elementsByTagName
    QValueList<QDomElement> framesets;
    for ( ; !framesetElem.isNull() ; framesetElem = framesetElem.nextSibling().toElement() )
    {
        if ( framesetElem.tagName() == "FRAMESET" )
        {
            framesets.append( framesetElem );
            m_nrItemsToLoad += framesetElem.childNodes().count();
        }
    }

    m_itemsLoaded = 0;

    QValueList<QDomElement>::Iterator it = framesets.begin();
    QValueList<QDomElement>::Iterator end = framesets.end();
    for ( ; it != end ; ++it )
    {
        (void) loadFrameSet( *it );
    }
}

KWFrameSet * KWDocument::loadFrameSet( QDomElement framesetElem, bool loadFrames, bool loadFootnote )
{
    FrameSetType frameSetType = static_cast<FrameSetType>( KWDocument::getAttribute( framesetElem, "frameType", FT_BASE ) );
    QString fsname = KWDocument::getAttribute( framesetElem, "name", "" );

    switch ( frameSetType ) {
    case FT_TEXT: {
        QString tableName = KWDocument::getAttribute( framesetElem, "grpMgr", "" );
        if ( !tableName.isEmpty() ) {
            // Text frameset belongs to a table -> find table by name
            KWTableFrameSet *table = 0L;
            QPtrListIterator<KWFrameSet> fit = framesetsIterator();
            for ( ; fit.current() ; ++fit ) {
                KWFrameSet *f = fit.current();
                if( f->type() == FT_TABLE &&
                    f->isVisible() &&
                    f->name() == tableName ) {
                    table = static_cast<KWTableFrameSet *> (f);
                    break;
                }
            }
            // No such table yet -> create
            if ( !table ) {
                table = new KWTableFrameSet( this, tableName );
                m_lstFrameSet.append( table );
            }
            // Load the cell
            return table->loadCell( framesetElem );
        }
        else
        {
            KWFrameSet::Info info = static_cast<KWFrameSet::Info>( framesetElem.attribute("frameInfo").toInt() );
            if ( info == KWFrameSet::FI_FOOTNOTE )
            {
                if ( !loadFootnote )
                    return 0L;
                // Footnote -> create a KWFootNoteFrameSet
                KWFootNoteFrameSet *fs = new KWFootNoteFrameSet( this, fsname );
                fs->load( framesetElem, loadFrames );
                m_lstFrameSet.append( fs );
                return fs;
            }
            else // Normal text frame
            {
                KWTextFrameSet *fs = new KWTextFrameSet( this, fsname );
                fs->load( framesetElem, loadFrames );
                m_lstFrameSet.append( fs ); // don't use addFrameSet here. We'll call finalize() once and for all in completeLoading

                // Old file format had autoCreateNewFrame as a frameset attribute
                if ( framesetElem.hasAttribute( "autoCreateNewFrame" ) )
                {
                    KWFrame::FrameBehavior behav = static_cast<KWFrame::FrameBehavior>( framesetElem.attribute( "autoCreateNewFrame" ).toInt() );
                    QPtrListIterator<KWFrame> frameIt( fs->frameIterator() );
                    for ( ; frameIt.current() ; ++frameIt ) // Apply it to all frames
                        frameIt.current()->setFrameBehavior( behav );
                }
                return fs;
            }
        }
    } break;
    case FT_CLIPART:
    {
        kdError(32001) << "FT_CLIPART used! (in KWDocument::loadFrameSet)" << endl;
        // Do not break!
    }
    case FT_PICTURE:
    {
        KWPictureFrameSet *fs = new KWPictureFrameSet( this, fsname );
        fs->load( framesetElem, loadFrames );
        m_lstFrameSet.append( fs );
        return fs;
    } break;
#if 0 // KWORD_HORIZONTAL_LINE
    case FT_HORZLINE:
    {
        KWHorzLineFrameSet *fs = new KWHorzLineFrameSet( this, fsname );
        fs->load( framesetElem, loadFrames );
        m_lstFrameSet.append( fs );
        return fs;
    } break;
#endif
    case FT_FORMULA: {
        KWFormulaFrameSet *fs = new KWFormulaFrameSet( this, fsname );
        fs->load( framesetElem, loadFrames );
        m_lstFrameSet.append( fs );
        return fs;
    } break;
    // Note that FT_PART cannot happen when loading from a file (part frames are saved into the SETTINGS tag)
    // and FT_TABLE can't happen either.
    case FT_PART:
        kdWarning(32001) << "loadFrameSet: FT_PART: impossible case" << endl;
        break;
    case FT_TABLE:
        kdWarning(32001) << "loadFrameSet: FT_TABLE: impossible case" << endl;
        break;
    case FT_BASE:
        kdWarning(32001) << "loadFrameSet: FT_BASE !?!?" << endl;
        break;
    }
    return 0L;
}

void KWDocument::loadImagesFromStore( KoStore *_store )
{
    if ( _store && !m_pictureMap.isEmpty() ) {
        m_pictureCollection->readFromStore( _store, m_pictureMap );
        m_pictureMap.clear(); // Release memory
    }
}

bool KWDocument::completeLoading( KoStore *_store )
{
    kdDebug() << k_funcinfo << endl;
    // Old-XML stuff. No-op when loading OASIS.
    loadImagesFromStore( _store );
    processPictureRequests();
    processAnchorRequests();
    processFootNoteRequests();

    // Save memory
    m_urlIntern = QString::null;

    // The fields from documentinfo.xml just got loaded -> update vars
    recalcVariables( VT_FIELD );

    // Finalize all the existing [non-inline] framesets
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->finalize();

    // This computes the number of pages (from the frames)
    // for the first time (and adds footers/headers/footnotes etc.)
    recalcFrames();

    // Fix z orders on older documents
    fixZOrders();

    emit newContentsSize();
    repaintAllViews( true );     // in case any view exists already
    reactivateBgSpellChecking();
    connect( documentInfo(), SIGNAL( sigDocumentInfoModifed()),this,SLOT(slotDocumentInfoModifed() ) );

    //desactivate bgspellchecking
    //attributes isReadWrite is not placed at the beginning !
    if ( !isReadWrite())
        enableBackgroundSpellCheck( false );

    // Load bookmarks
    initBookmarkList();

    deleteLoadingInfo();

    setModified( false );

    return true;
}

KWLoadingInfo* KWDocument::createLoadingInfo()
{
    Q_ASSERT( !m_loadingInfo );
    m_loadingInfo = new KWLoadingInfo();
    return m_loadingInfo;
}

void KWDocument::deleteLoadingInfo()
{
    Q_ASSERT( m_loadingInfo );
    delete m_loadingInfo;
    m_loadingInfo = 0;
}

void KWDocument::processPictureRequests()
{
    QPtrListIterator<KWTextImage> it2 ( m_textImageRequests );
    for ( ; it2.current() ; ++it2 )
    {
        it2.current()->setImage( *m_pictureCollection );
    }
    m_textImageRequests.clear();

    //kdDebug(32001) << m_pictureRequests.count() << " picture requests." << endl;
    QPtrListIterator<KWPictureFrameSet> it3( m_pictureRequests );
    for ( ; it3.current() ; ++it3 )
        it3.current()->setPicture( m_pictureCollection->findPicture( it3.current()->key() ) );
    m_pictureRequests.clear();
}

void KWDocument::processAnchorRequests()
{
    QMapIterator<QString, KWAnchorPosition> itanch = m_anchorRequests.begin();
    for ( ; itanch != m_anchorRequests.end(); ++itanch )
    {
        QString fsname = itanch.key();
        if ( m_pasteFramesetsMap && m_pasteFramesetsMap->contains( fsname ) )
            fsname = (*m_pasteFramesetsMap)[ fsname ];
        kdDebug(32001) << "KWDocument::processAnchorRequests anchoring frameset " << fsname << endl;
        KWFrameSet * fs = frameSetByName( fsname );
        Q_ASSERT( fs );
        if ( fs )
            fs->setAnchored( itanch.data().textfs, itanch.data().paragId, itanch.data().index, true, false /*don't repaint yet*/ );
    }
    m_anchorRequests.clear();
}

bool KWDocument::processFootNoteRequests()
{
    bool ret = false;
    QMapIterator<QString, KWFootNoteVariable *> itvar = m_footnoteVarRequests.begin();
    for ( ; itvar != m_footnoteVarRequests.end(); ++itvar )
    {
        QString fsname = itvar.key();
        if ( m_pasteFramesetsMap && m_pasteFramesetsMap->contains( fsname ) )
            fsname = (*m_pasteFramesetsMap)[ fsname ];
        //kdDebug(32001) << "KWDocument::processFootNoteRequests binding footnote var " << itvar.data() << " and frameset " << fsname << endl;
        KWFrameSet * fs = frameSetByName( fsname );
        Q_ASSERT( fs );
        if ( !fs ) // #104431
            continue;
        Q_ASSERT( fs->type() == FT_TEXT );
        Q_ASSERT( fs->frameSetInfo() == KWFrameSet::FI_FOOTNOTE );
        KWFootNoteFrameSet* fnfs = dynamic_cast<KWFootNoteFrameSet *>(fs);
        if ( fnfs )
        {
            fnfs->setFootNoteVariable( itvar.data() );
            itvar.data()->setFrameSet( fnfs );
            ret = true;
        }
    }
    m_footnoteVarRequests.clear();
    // Renumber footnotes
    if ( ret ) {
        KWFrameSet *frameset = m_lstFrameSet.getFirst();
        if ( frameset && frameset->type() == FT_TEXT )
            static_cast<KWTextFrameSet *>(frameset)->renumberFootNotes( false /*no repaint*/ );
    }
    return ret;
}

QString KWDocument::uniqueFramesetName( const QString& oldName )
{
    QString newName = oldName;
    if (frameSetByName( oldName ))//rename it if name frameset exists
    {
        // make up a new name for the frameset, use Copy[digits]-[oldname] as template.
        // Fully translatable naturally :)
        QString searchString( "^(" + i18n("Copy%1-%2").arg("\\d*").arg("){0,1}") );
        searchString = searchString.replace(QRegExp("\\-"), "\\-"); // escape the '-'
        QRegExp searcher(searchString);
        int count=0;
        do {
            newName=oldName;
            newName.replace(searcher,i18n("Copy%1-%2").arg(count > 0? QString("%1").arg(count):"").arg(""));
            count++;
        } while ( frameSetByName( newName ) );
    }
    return newName;
}

void KWDocument::pasteFrames( QDomElement topElem, KMacroCommand * macroCmd, bool copyFootNote, bool loadFootNote, bool selectFrames )
{
    m_pasteFramesetsMap = new QMap<QString, QString>();
    //QPtrList<KWFrameSet> frameSetsToFinalize;
    int ref=0;
    int nb = 0;
    QDomElement elem = topElem.firstChild().toElement();
    for ( ; !elem.isNull() ; elem = elem.nextSibling().toElement() )
    {
        //kdDebug() << "pasteFrames: elem=" << elem.tagName() << endl;
        QDomElement frameElem;
        KWFrameSet * fs = 0L;
        if ( elem.tagName() == "FRAME" )
        {
            QString frameSetName = frameElem.attribute( "parentFrameset" );
            fs = frameSetByName( frameSetName );
            if ( !fs )
            {
                kdWarning(32001) << "pasteFrames: Frameset '" << frameSetName << "' not found" << endl;
                continue;
            }
            frameElem = elem;
        }
        else if ( elem.tagName() == "FRAMESET" )
        {
            // Prepare a new name for the frameset
            QString oldName = elem.attribute( "name" );
            QString newName = uniqueFramesetName( oldName ); // make up a new name for the frame

            m_pasteFramesetsMap->insert( oldName, newName ); // remember the name transformation
            kdDebug(32001) << "KWDocument::pasteFrames new frame : " << oldName << "->" << newName << endl;
            FrameSetType frameSetType = static_cast<FrameSetType>( KWDocument::getAttribute( elem, "frameType", FT_BASE ) );
            switch ( frameSetType ) {
            case FT_TABLE: {
                KWTableFrameSet *table = new KWTableFrameSet( this, newName );
                table->fromXML( elem, true, false /*don't apply names*/ );
                table->moveBy( 20.0, 20.0 );
                m_lstFrameSet.append( table );
                table->setZOrder();
                if ( macroCmd )
                    macroCmd->addCommand( new KWCreateTableCommand( QString::null, table ) );
                fs = table;
                break;
            }
            case FT_PART:
            {
                ref |= Embedded;
#if 0
                KWPartFrameSet *part = new KWPartFrameSet( this, newName );
                part->fromXML( elem, true, false /*don't apply names*/ );
                part->moveBy( 20.0, 20.0 );
                m_lstFrameSet.append( part );
                part->setZOrder();
                fs = part;
#endif
                break;
            }
            default:
                fs = loadFrameSet( elem, false, loadFootNote );
                if ( fs )
                {
                    kdDebug() << "KWDocument::pasteFrames created frame " << newName << endl;
                    fs->setName( newName );
                    frameElem = elem.namedItem( "FRAME" ).toElement();
                }
            }
            //when we paste a header/footer we transforme it in a body frame
            if(fs && (fs->isHeaderOrFooter() || ( !copyFootNote && fs->isFootEndNote())))
                fs->setFrameSetInfo(KWFrameSet::FI_BODY);
        }
        // Test commented out since the toplevel element can contain "PARAGRAPH" now
        //else
        //kdWarning(32001) << "Unsupported toplevel-element in KWCanvas::pasteFrames : '" << elem.tagName() << "'" << endl;

        if ( fs )
        {
            //if ( frameSetsToFinalize.findRef( fs ) == -1 )
            //    frameSetsToFinalize.append( fs );

            // Load the frame
            if ( !frameElem.isNull() )
            {
                double offs = 20.0;
                KoRect rect;
                rect.setLeft( KWDocument::getAttribute( frameElem, "left", 0.0 ) + offs );
                rect.setTop( KWDocument::getAttribute( frameElem, "top", 0.0 ) + offs );
                rect.setRight( KWDocument::getAttribute( frameElem, "right", 0.0 ) + offs );
                rect.setBottom( KWDocument::getAttribute( frameElem, "bottom", 0.0 ) + offs );
                KWFrame * frame = new KWFrame( fs, rect.x(), rect.y(), rect.width(), rect.height() );
                frame->load( frameElem, fs, KWDocument::CURRENT_SYNTAX_VERSION );
                frame->setZOrder( maxZOrder( frame->pageNumber(this) ) + 1 +nb ); // make sure it's on top
                if ( selectFrames )
                    frame->setSelected(TRUE);
                nb++;
                fs->addFrame( frame, false );
                if ( macroCmd )
                {
                    KWCreateFrameCommand *cmd = new KWCreateFrameCommand( QString::null, frame );
                    macroCmd->addCommand(cmd);
                }
            }
            int type=0;
            // Please move this to some common method somewhere (e.g. in KWDocument) (David)
            switch(fs->type())
            {
            case FT_TEXT:
                type=(int)TextFrames;
                break;
            case FT_CLIPART:
            {
                kdError(32001) << "FT_CLIPART used! (in KWDocument::loadFrameSet)" << endl;
                // Do not break!
            }
            case FT_PICTURE:
                type=(int)Pictures;
                break;
            case FT_PART:
                type=(int)Embedded;
                break;
            case FT_FORMULA:
                type=(int)FormulaFrames;
                break;
            case FT_TABLE:
                type=(int)Tables;
                break;
            default:
                type=(int)TextFrames;
            }
            ref|=type;
        }
    }
    refreshDocStructure(ref);
}

void KWDocument::completePasting()
{
    processPictureRequests();
    processAnchorRequests();
    if ( processFootNoteRequests() )
    {
        // We pasted footnotes. Relayout frames.
        recalcFrames();
    }

    // Finalize afterwards - especially in case of inline frames, made them inline in processAnchorRequests
    //for ( QPtrListIterator<KWFrameSet> fit( frameSetsToFinalize ); fit.current(); ++fit )

    // Do it on all of them (we'd need to store frameSetsToFinalize as member var if this is really slow)
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->finalize();
    repaintAllViews();
    delete m_pasteFramesetsMap;
    m_pasteFramesetsMap = 0L;
}

void KWDocument::completeOasisPasting()
{
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->finalize();
    repaintAllViews();
}

void KWDocument::insertEmbedded( KoStore *store, QDomElement topElem, KMacroCommand * macroCmd, double offset )
{
    if ( !m_pasteFramesetsMap ) // may have been created by pasteFrames
        m_pasteFramesetsMap = new QMap<QString, QString>();

    QDomElement elem = topElem.firstChild().toElement();
    for ( ; !elem.isNull() ; elem = elem.nextSibling().toElement() )
    {
        if ( elem.tagName() == "EMBEDDED" )
        {
            kdDebug()<<"KWDocument::insertEmbedded() Embedded object"<<endl;
            QDomElement object = elem.namedItem( "OBJECT" ).toElement();
            QDomElement settings = elem.namedItem( "SETTINGS" ).toElement();
            if ( object.isNull() || settings.isNull() )
            {
                kdError() << "No <OBJECT> or <SETTINGS> tag" << endl;
            }
            else
            {
                KWChild *ch = new KWChild( this );
                kdDebug()<<"KWDocument::insertEmbedded() loading document"<<endl;
                if ( ch->load( object, true ) )
                {
                    ch->loadDocument( store );
                    insertChild( ch );
                    QString oldName = settings.attribute( "name" );
                    QString newName = uniqueFramesetName( oldName );
                    m_pasteFramesetsMap->insert( oldName, newName ); // remember the name transformation
                    KWPartFrameSet *part = new KWPartFrameSet( this, ch, newName );
                    m_lstFrameSet.append( part );
                    kdDebug() << "KWDocument::insertEmbedded loading embedded object" << endl;
                    part->load( settings );
                    if ( offset != 0 ) {
                        QRect r = ch->geometry();
                        r.moveBy( (int)offset, (int)offset );
                        ch->setGeometry( r );
                    }
                    part->setZOrder();
                    if ( macroCmd )
                    {
                        QPtrListIterator<KWFrame> frameIt( part->frameIterator() );
                        for ( ; frameIt.current(); ++frameIt )
                        {
                            macroCmd->addCommand( new KWCreateFrameCommand( QString::null, frameIt.current() ) );
                        }
                    }
                }
            }
        }
    }
    refreshDocStructure( (int)Embedded );
}

bool KWDocument::saveOasis( KoStore* store, KoXmlWriter* manifestWriter )
{
    return saveOasisHelper( store, manifestWriter, SaveAll );
}

// can't be const due to recalcVariables()
bool KWDocument::saveOasisHelper( KoStore* store, KoXmlWriter* manifestWriter, SaveFlag saveFlag,
                            QString* plainText, KoPicture* picture, KWTextFrameSet* fs )
{
    m_pictureCollection->assignUniqueIds();
    fixZOrders();

    manifestWriter->addManifestEntry( "content.xml", "text/xml" );
    KoOasisStore oasisStore( store );

    KoXmlWriter* contentWriter = oasisStore.contentWriter();
    if ( !contentWriter )
        return false;

    QValueList<KoPictureKey> pictureList;
    if ( saveFlag == SaveAll )
        pictureList = savePictureList();

    m_varColl->variableSetting()->setModificationDate(QDateTime::currentDateTime());
    recalcVariables( VT_DATE );
    recalcVariables( VT_TIME ); // for "current time"
    recalcVariables( VT_STATISTIC );
    m_syntaxVersion = CURRENT_SYNTAX_VERSION; // ### clean this up once we remove the old format

    KoGenStyles mainStyles;
    KoSavingContext savingContext( mainStyles, m_varColl->variableSetting(), m_pageColumns.columns > 1, KoSavingContext::Store );

    // Save user styles as KoGenStyle objects
    KoSavingContext::StyleNameMap map = m_styleColl->saveOasis( mainStyles, KoGenStyle::STYLE_USER, savingContext );
    savingContext.setStyleNameMap( map );

    QByteArray headerFooterContent;
    if ( saveFlag == SaveAll )
    {
        // Save visual info for the first view, such as the active frameset and cursor position
        // It looks like a hack, but reopening a document creates only one view anyway (David)
        KWView * view = static_cast<KWView*>(views().getFirst());
        if ( view ) // no view if embedded document
        {
            KWFrameSetEdit* edit = view->getGUI()->canvasWidget()->currentFrameSetEdit();
            if ( edit )
            {
                KWTextFrameSetEdit* textedit = dynamic_cast<KWTextFrameSetEdit *>(edit);
                if ( textedit && textedit->cursor() ) {
                    KoTextCursor* cursor = textedit->cursor();
                    savingContext.setCursorPosition( cursor->parag(),
                                                     cursor->index() );
                }
            }
        }

        // Header and footers save their content into master-styles/master-page, and their
        // styles into the page-layout automatic-style.
        // However the paragraph styles used by header/footers need to be known before
        // hand, to promote them to styles.xml. So we collect them first, which means
        // storing the content into a buffer.
        QBuffer buffer( headerFooterContent );
        buffer.open( IO_WriteOnly );
        KoXmlWriter headerFooterTmpWriter( &buffer );  // TODO pass indentation level
        QPtrListIterator<KWFrameSet> fit = framesetsIterator();
        // ## This loop is duplicated in saveOasisDocumentStyles
        for ( ; fit.current() ; ++fit ) {
            const KWFrameSet* fs = fit.current();
            if ( fs->isVisible() && // HACK to avoid saving [hidden] headers/footers framesets for now
                 !fs->isFloating() &&
                 !fs->isDeleted() &&
                 fs->type() == FT_TEXT &&
                 fs->isHeaderOrFooter() )
            {
                // Save content
                headerFooterTmpWriter.startElement( fs->headerFooterTag() ); // e.g. style:header
                static_cast<const KWTextFrameSet *>(fs)->saveOasisContent( headerFooterTmpWriter, savingContext );
                headerFooterTmpWriter.endElement();
            }
        }
        // Add trailing '0'  (Qt4: remove)
        headerFooterContent.resize( headerFooterContent.size() + 1 );
        headerFooterContent[headerFooterContent.size()-1] = '\0';

        // Now mark all autostyles as "for styles.xml" since headers/footers need them
        QValueList<KoGenStyles::NamedStyle> autoStyles = mainStyles.styles( KoGenStyle::STYLE_AUTO, true );
        for ( QValueList<KoGenStyles::NamedStyle>::const_iterator it = autoStyles.begin();
              it != autoStyles.end(); ++it )
            mainStyles.markStyleForStylesXml( (*it).name );
    }

    KoXmlWriter* bodyWriter = oasisStore.bodyWriter();
    bodyWriter->startElement( "office:body" );
    bodyWriter->startElement( "office:text" );

    if ( saveFlag == SaveAll )
    {
        // save the body into bodyWriter
        saveOasisBody( *bodyWriter, savingContext );
    }
    else // SaveSelected
    {
        // In theory we should pass a view to this method, in order to
        // copy what is currently selected in that view only. But selection
        // is currently part of the KoTextParag data, so it's shared between views.
        if ( fs ) {
            *plainText = fs->textDocument()->copySelection( *bodyWriter, savingContext, KoTextDocument::Standard );
            // Collect inline framesets for e.g. pictures
            KWCollectFramesetsVisitor visitor;
            fs->textDocument()->visitSelection( KoTextDocument::Standard, &visitor );
            const QValueList<KWFrameSet *>& frameset = visitor.frameSets();
            kdDebug(32001) << frameset.count() << " inline framesets" << endl;
            for ( QValueList<KWFrameSet *>::ConstIterator it = frameset.begin(); it != frameset.end(); ++it )
            {
                switch ( (*it)->type() ) {
                case FT_PICTURE:
                {
                    const KoPictureKey key = static_cast<KWPictureFrameSet *>( *it )->key();
                    if ( !pictureList.contains( key ) )
                        pictureList.append( key );
                }
                break;
                case FT_PART:
                    // TODO
                default:
                    break;
                }
            }
        }

        // write selected (non-inline) frames
        QString newText;
        saveSelectedFrames( *bodyWriter, store, manifestWriter, savingContext, pictureList,
                            &newText ); // output vars
        *plainText += newText;
        // Single image -> return it
        if ( picture && pictureList.count() == 1 )
        {
            *picture = m_pictureCollection->findPicture( pictureList.first() );
        }
    }

    bodyWriter->endElement(); // office:text
    bodyWriter->endElement(); // office:body

    savingContext.writeFontFaces( *contentWriter );
    contentWriter->startElement( "office:automatic-styles" );
    KWOasisSaver::writeAutomaticStyles( *contentWriter, mainStyles, false );
    contentWriter->endElement(); // office:automatic-styles

    oasisStore.closeContentWriter();

    // Done with content.xml

    if ( !store->open( "styles.xml" ) )
        return false;
    manifestWriter->addManifestEntry( "styles.xml", "text/xml" );
    saveOasisDocumentStyles( store, mainStyles, savingContext, saveFlag, headerFooterContent );
    if ( !store->close() ) // done with styles.xml
        return false;

    //kdDebug(32001) << "saveOasis: " << pictureList.count() << " pictures" << endl;
    m_pictureCollection->saveOasisToStore( store, pictureList, manifestWriter );

    if ( saveFlag == SaveAll )
    {

        if(!store->open("settings.xml"))
            return false;

        KoStoreDevice contentDev( store );
        KoXmlWriter& settingsWriter = *createOasisXmlWriter(&contentDev, "office:document-settings");

        saveOasisSettings( settingsWriter );

        delete &settingsWriter;

        if(!store->close())
            return false;

        manifestWriter->addManifestEntry("settings.xml", "text/xml");
    }
    return true;
}

// can't be const due to recalcVariables()
QDragObject* KWDocument::dragSelected( QWidget* parent, KWTextFrameSet* fs )
{
    // We'll create a store (ZIP format) in memory
    QBuffer buffer;
    QCString mimeType = KWOasisSaver::selectionMimeType();
    KoStore* store = KoStore::createStore( &buffer, KoStore::Write, mimeType );
    Q_ASSERT( store );
    Q_ASSERT( !store->bad() );
    KoOasisStore oasisStore( store );

    KoXmlWriter* manifestWriter = oasisStore.manifestWriter( mimeType );

    QString plainText;
    KoPicture picture;
    if ( !saveOasisHelper( store, manifestWriter, KWDocument::SaveSelected, &plainText, &picture, fs )
         || !oasisStore.closeManifestWriter() )
    {
        delete store;
        return 0;
    }

    delete store;

    KMultipleDrag* multiDrag = new KMultipleDrag( parent );
    if ( !plainText.isEmpty() )
        multiDrag->addDragObject( new QTextDrag( plainText, 0 ) );
    if ( !picture.isNull() )
        multiDrag->addDragObject( picture.dragObject( 0 ) );
    KoStoreDrag* storeDrag = new KoStoreDrag( KWOasisSaver::selectionMimeType(), 0 );
    kdDebug() << k_funcinfo << "setting zip data: " << buffer.buffer().size() << " bytes." << endl;
    storeDrag->setEncodedData( buffer.buffer() );
    multiDrag->addDragObject( storeDrag );
    return multiDrag;
}

void KWDocument::saveSelectedFrames( KoXmlWriter& bodyWriter, KoStore* store, KoXmlWriter* manifestWriter,
                                     KoSavingContext& savingContext, QValueList<KoPictureKey>& pictureList,
                                     QString* plainText ) const
{
    QPtrList<KoDocumentChild> embeddedObjects;

    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit ) {
        KWFrameSet * fs = fit.current();
        if ( fs->isVisible() && fs->type() == FT_PART
             && fs->frameIterator().getFirst()->isSelected() ) {
            embeddedObjects.append( static_cast<KWPartFrameSet *>(fs)->getChild() );
        }
    }

    fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * fs = fit.current();
        if ( fs->isVisible() )
        {
            bool isTable = ( fs->type() == FT_TABLE );
            if ( fs->type() == FT_PART )
                continue;
            QPtrListIterator<KWFrame> frameIt = fs->frameIterator();
            KWFrame * firstFrame = frameIt.current();
            for ( ; frameIt.current(); ++frameIt )
            {
                KWFrame * frame = frameIt.current();
                if ( frame->isSelected() )
                {
                    kdDebug(32001) << "Selected frame " << frame << " from " << fs->name() << endl;
                    // Two cases to be distinguished here
                    // If it's the first frame of a frameset, then copy the frameset contents and the frame itself
                    // Otherwise copy only the frame information
                    if ( frame == firstFrame || isTable )
                    {
                        fs->saveOasis( bodyWriter, savingContext, /*TODO: check this: isTable ? true :*/ false );
                        if ( plainText )
                            *plainText += fs->toPlainText();
                    }
                    else if ( !isTable )
                    {
#if 0
                        // Save the frame information
                        QDomElement frameElem = parentElem.ownerDocument().createElement( "FRAME" );
                        parentElem.appendChild( frameElem );
                        frame->save( frameElem );
                        if ( frame != firstFrame )
                        {
                            // Frame saved alone -> remember which frameset it's part of
                            frameElem.setAttribute( "parentFrameset", fs->name() );
                        }
#endif
                    }
                    if ( fs->type() == FT_PICTURE ) {
                        kdDebug(32001) << "found non-inline picture framesets" << endl;

                        const KoPictureKey key = static_cast<KWPictureFrameSet *>( fs )->key();
                        if ( !pictureList.contains( key ) )
                            pictureList.append( key );
                    }
                    if ( isTable ) // Copy tables only once, even if they have many cells selected
                        break;
                }
            } // for each frame
        }
    }

    // Save embedded objects - code inspired from KoDocument::saveChildrenOasis
    QPtrListIterator<KoDocumentChild> chl( embeddedObjects );
    for( ; chl.current(); ++chl ) {
        KoDocument* childDoc = chl.current()->document();
        QString path;
        if ( childDoc ) {
            if ( !childDoc->isStoredExtern() ) {
                if ( !chl.current()->saveOasisToStore( store, manifestWriter ) )
                    return;

                //assert( childDoc->url().protocol() == INTERNAL_PROTOCOL );
                path = store->currentDirectory();
                if ( !path.isEmpty() )
                    path += '/';
                path += childDoc->url().path();
            } else {
                path = childDoc->url().url();
            }
            manifestWriter->addManifestEntry( path, childDoc->nativeOasisMimeType() );
        }
    }
}

void KWDocument::saveOasisSettings( KoXmlWriter& settingsWriter ) const
{
    settingsWriter.startElement("office:settings");
    settingsWriter.startElement("config:config-item-set");

    settingsWriter.addAttribute("config:name", "view-settings");

    KoUnit::saveOasis(&settingsWriter, m_unit);

    settingsWriter.endElement(); // config:config-item-set

    settingsWriter.startElement("config:config-item-set");
    settingsWriter.addAttribute("config:name", "configuration-settings");
    settingsWriter.addConfigItem("SpellCheckerIgnoreList", m_spellCheckIgnoreList.join( "," ) );
    settingsWriter.endElement(); // config:config-item-set

    m_varColl->variableSetting()->saveOasis( settingsWriter );

    settingsWriter.endElement(); // office:settings
    settingsWriter.endElement(); // Root element
    settingsWriter.endDocument();
}

void KWDocument::saveOasisDocumentStyles( KoStore* store, KoGenStyles& mainStyles, KoSavingContext& savingContext, SaveFlag saveFlag, const QByteArray& headerFooterContent ) const
{
    Q_UNUSED( savingContext ); // maybe we need it later again :)

    KoStoreDevice stylesDev( store );
    KoXmlWriter* stylesWriter = createOasisXmlWriter( &stylesDev, "office:document-styles" );

    stylesWriter->startElement( "office:styles" );

    if ( saveFlag == SaveAll )
    {
        stylesWriter->startElement( "style:default-style" );
        stylesWriter->addAttribute( "style:family", "paragraph" );
        stylesWriter->startElement( "style:paragraph-properties" );
        stylesWriter->addAttributePt( "style:tab-stop-distance", m_tabStop );
        stylesWriter->endElement(); // paragraph-properties
        stylesWriter->endElement(); // default-style
    }

    QValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( KoGenStyle::STYLE_USER );
    QValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }
    styles = mainStyles.styles( KoGenStyle::STYLE_LIST );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "text:list-style", (*it).name, 0 );
    }
    m_styleColl->saveOasisOutlineStyles( *stylesWriter );
    if ( saveFlag == SaveAll )
        static_cast<KWVariableSettings *>( m_varColl->variableSetting() )->saveNoteConfiguration( *stylesWriter );
    stylesWriter->endElement(); // office:styles

    QString pageLayoutName;
    if ( saveFlag == SaveAll )
    {
        stylesWriter->startElement( "office:automatic-styles" );

        KoGenStyle pageLayout = m_pageLayout.saveOasis();
        pageLayout.addAttribute( "style:page-usage", "all" ); // needed?
        // This is for e.g. spreadsheets, not for word-processors.
        //pageLayout.addProperty( "style:first-page-number", m_varColl->variableSetting()->startingPage() );

        QBuffer buffer;
        buffer.open( IO_WriteOnly );
        KoXmlWriter footnoteSepTmpWriter( &buffer );  // TODO pass indentation level
        footnoteSepTmpWriter.startElement( "style:footnote-sep" );
        QString tmp;
        switch( m_footNoteSeparatorLinePos )
        {
        case SLP_CENTERED:
            tmp = "centered";
            break;
        case SLP_RIGHT:
            tmp = "right";
            break;
        case SLP_LEFT:
            tmp = "left";
            break;
        }

        footnoteSepTmpWriter.addAttribute( "style:adjustment", tmp );
        footnoteSepTmpWriter.addAttributePt( "style:width", m_footNoteSeparatorLineWidth );
        footnoteSepTmpWriter.addAttribute( "style:rel-width", QString::number( footNoteSeparatorLineLength() ) + "%" );
        switch( m_footNoteSeparatorLineType )
        {
        case SLT_SOLID:
            tmp = "solid";
            break;
        case SLT_DASH:
            tmp = "dash";
            break;
        case SLT_DOT:
            tmp = "dotted";
            break;
        case SLT_DASH_DOT:
            tmp = "dot-dash";
            break;
        case SLT_DASH_DOT_DOT:
            tmp = "dot-dot-dash";
            break;
        }

        footnoteSepTmpWriter.addAttribute( "style:line-style", tmp );

        footnoteSepTmpWriter.endElement();
        const QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
        pageLayout.addChildElement( "separator", elementContents );
        buffer.close();

        if ( m_pageColumns.columns > 1 ) {
            buffer.setBuffer( QByteArray() ); // clear data
            buffer.open( IO_WriteOnly );
            KoXmlWriter columnsTmpWriter( &buffer );  // TODO pass indentation level
            columnsTmpWriter.startElement( "style:columns" );
            columnsTmpWriter.addAttribute( "fo:column-count", m_pageColumns.columns );
            columnsTmpWriter.addAttributePt( "fo:column-gap", m_pageColumns.ptColumnSpacing );
            columnsTmpWriter.endElement(); // style:columns
            buffer.close();
            const QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
            pageLayout.addChildElement( "columns", elementContents );
        }

        // This is a bit of a hack, which only works as long as we have only one page master
        // if there's more than one pagemaster we need to rethink all this

        pageLayoutName = mainStyles.lookup( pageLayout, "pm" );
        pageLayout.writeStyle( stylesWriter, mainStyles, "style:page-layout", pageLayoutName,
                               "style:page-layout-properties", false /*don't close*/ );

        // Ouch another problem: there is only one header style in oasis
        // ##### can't have different borders for even/odd headers...
        bool headerStyleSaved = false;
        bool footerStyleSaved = false;
        // ## This loop is duplicated in saveOasis
        QPtrListIterator<KWFrameSet> fit = framesetsIterator();
        for ( ; fit.current() ; ++fit ) {
            const KWFrameSet* fs = fit.current();
            if ( fs->isVisible() && // HACK to avoid saving [hidden] headers/footers framesets for now
                 !fs->isFloating() &&
                 !fs->isDeleted() &&
                 fs->type() == FT_TEXT &&
                 fs->isHeaderOrFooter() )
            {
                // Save header/footer style
                KWFrame* frame = fs->frame(0);
                if ( fs->isAHeader() ) {
                    if ( headerStyleSaved )
                        continue;
                    headerStyleSaved = true;
                    stylesWriter->startElement( "style:header-style" );
                } else {
                    if ( footerStyleSaved )
                        continue;
                    footerStyleSaved = true;
                    stylesWriter->startElement( "style:footer-style" );
                }
#if 0 // more code reuse, but harder to integrate
                KoGenStyle hfStyle;
                hfStyle.addPropertyPt( "fo:min-height", frame->minFrameHeight() );
                frame->saveBorderProperties( hfStyle );
                frame->saveMarginProperties( hfStyle );
                ...
#endif
                stylesWriter->startElement( "style:header-footer-properties" );
                stylesWriter->addAttributePt( "fo:min-height", frame->minFrameHeight() );
                // TODO frame->saveBorderAttributes( *stylesWriter );
                frame->saveMarginAttributes( *stylesWriter );
                stylesWriter->endElement(); // header-footer-properties
                stylesWriter->endElement(); // header-style
            }
        }
        stylesWriter->endElement(); // style:page-layout

        // Headers and footers might have created new automatic parag/text styles -> save those
        KWOasisSaver::writeAutomaticStyles( *stylesWriter, mainStyles, true );

        stylesWriter->endElement(); // office:automatic-styles
    }


    stylesWriter->startElement( "office:master-styles" );
    stylesWriter->startElement( "style:master-page" );
    stylesWriter->addAttribute( "style:name", "Standard" );
    stylesWriter->addAttribute( "style:page-layout-name", pageLayoutName );

    if ( isHeaderVisible() || isFooterVisible() ) { // ### TODO save them even when hidden (and not empty)?
        stylesWriter->addCompleteElement( headerFooterContent.data() );
    }

    stylesWriter->endElement();
    stylesWriter->endElement(); // office:master-styles

    stylesWriter->endElement(); // root element (office:document-styles)
    stylesWriter->endDocument();
    delete stylesWriter;
}

void KWDocument::saveOasisCustomFied( KoXmlWriter &writer )const
{
    bool customVariableFound = false;
    QPtrListIterator<KoVariable> it( m_varColl->getVariables() );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->type() == VT_CUSTOM )
        {
            if ( !customVariableFound )
            {
                writer.startElement( "text:user-field-decls" );
                customVariableFound = true;
            }
            //<text:user-field-decl office:value-type="string" office:string-value="dfddd" text:name="cvbcbcbx"/>
            writer.startElement( "text:user-field-decl" );
            writer.addAttribute( "office:value-type", "string" );
            writer.addAttribute( "office:string-value", static_cast<KoCustomVariable *>( it.current() )->value() );
            writer.addAttribute( "text:name", static_cast<KoCustomVariable*>( it.current() )->name() );
            writer.endElement();
        }
    }
    if ( customVariableFound )
        writer.endElement();
}

void KWDocument::saveOasisBody( KoXmlWriter& writer, KoSavingContext& context ) const
{
    saveOasisCustomFied( writer );
    if ( m_processingType == WP ) {

        // Write out the main text frameset's contents
        KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet *>( m_lstFrameSet.getFirst() );
        if ( frameset ) {
            frameset->saveOasisContent( writer, context );
        }
        // Write out the other (non-inline) framesets
        QPtrListIterator<KWFrameSet> fit = framesetsIterator();
        ++fit; // skip main text frameset
        for ( ; fit.current() ; ++fit ) {
            KWFrameSet* fs = fit.current();
            if ( !fs->isFloating() &&
                 !fs->isDeleted() &&
                // footnotes already saved inline, header/footers elsewhere
                 fs->frameSetInfo() == KWFrameSet::FI_BODY )
            {
                fs->saveOasis( writer, context, true );
            }
        }

    } else { // DTP mode: all framesets are equal
        // write text:page-sequence, one item per page.
        writer.startElement( "text:page-sequence" );
        for ( int page = 0; page < pageCount(); ++page )
        {
            writer.startElement( "text:page" );
            // "pm" is a hack, see mainStyles.lookup( pageLayout, "pm" ) in saveOasis
            // [which currently happens afterwards...]
            writer.addAttribute( "text:master-page-name", "pm" );
            writer.endElement(); // text:page
        }
        writer.endElement() ; // "text:page-sequence";
        // Now write the framesets
        QPtrListIterator<KWFrameSet> fit = framesetsIterator();
        for ( ; fit.current() ; ++fit ) {
            KWFrameSet* fs = fit.current();
            if ( !fs->isFloating() &&
                 !fs->isDeleted() &&
                 fs->frameSetInfo() == KWFrameSet::FI_BODY )
            {
                fs->saveOasis( writer, context, true );
            }
        }
     }
}

QDomDocument KWDocument::saveXML()
{
    m_varColl->variableSetting()->setModificationDate(QDateTime::currentDateTime());
    recalcVariables( VT_DATE );
    recalcVariables( VT_TIME ); // for "current time"
    recalcVariables( VT_STATISTIC );
    QDomDocument doc = createDomDocument( "DOC", CURRENT_DTD_VERSION );
    QDomElement kwdoc = doc.documentElement();
    kwdoc.setAttribute( "editor", "KWord" );
    kwdoc.setAttribute( "mime", "application/x-kword" );
    m_syntaxVersion = CURRENT_SYNTAX_VERSION;
    kwdoc.setAttribute( "syntaxVersion", m_syntaxVersion );

    QDomElement paper = doc.createElement( "PAPER" );
    kwdoc.appendChild( paper );
    paper.setAttribute( "format", static_cast<int>( m_pageLayout.format ) );
    paper.setAttribute( "pages", pageCount() );
    paper.setAttribute( "width", m_pageLayout.ptWidth );
    paper.setAttribute( "height", m_pageLayout.ptHeight );
    paper.setAttribute( "orientation", static_cast<int>( m_pageLayout.orientation ) );
    paper.setAttribute( "columns", m_pageColumns.columns );
    paper.setAttribute( "columnspacing", m_pageColumns.ptColumnSpacing );
    paper.setAttribute( "hType", static_cast<int>( m_pageHeaderFooter.header ) );
    paper.setAttribute( "fType", static_cast<int>( m_pageHeaderFooter.footer ) );
    paper.setAttribute( "spHeadBody", m_pageHeaderFooter.ptHeaderBodySpacing );
    paper.setAttribute( "spFootBody", m_pageHeaderFooter.ptFooterBodySpacing );
    paper.setAttribute( "spFootNoteBody", m_pageHeaderFooter.ptFootNoteBodySpacing );
    if ( m_footNoteSeparatorLinePos!=SLP_LEFT )
    {
        if (m_footNoteSeparatorLinePos==SLP_CENTERED )
            paper.setAttribute( "slFootNotePosition", "centered" );
        else if ( m_footNoteSeparatorLinePos==SLP_RIGHT )
            paper.setAttribute( "slFootNotePosition", "right" );
        else if ( m_footNoteSeparatorLinePos==SLP_LEFT ) //never !
            paper.setAttribute( "slFootNotePosition", "left" );
    }
    if ( m_footNoteSeparatorLineType != SLT_SOLID )
        paper.setAttribute( "slFootNoteType", static_cast<int>(m_footNoteSeparatorLineType) );


    paper.setAttribute("slFootNoteLength", m_iFootNoteSeparatorLineLength);
    paper.setAttribute("slFootNoteWidth", m_footNoteSeparatorLineWidth);

    // Now part of the app config
    //paper.setAttribute( "zoom",m_zoom );

    QDomElement borders = doc.createElement( "PAPERBORDERS" );
    paper.appendChild( borders );
    borders.setAttribute( "left", m_pageLayout.ptLeft );
    borders.setAttribute( "top", m_pageLayout.ptTop );
    borders.setAttribute( "right", m_pageLayout.ptRight );
    borders.setAttribute( "bottom", m_pageLayout.ptBottom );

    QDomElement docattrs = doc.createElement( "ATTRIBUTES" );
    kwdoc.appendChild( docattrs );
    docattrs.setAttribute( "processing", static_cast<int>( m_processingType ) );
    docattrs.setAttribute( "standardpage", 1 );
    docattrs.setAttribute( "hasHeader", static_cast<int>(isHeaderVisible()) );
    docattrs.setAttribute( "hasFooter", static_cast<int>(isFooterVisible()) );
    docattrs.setAttribute( "unit", KoUnit::unitName(unit()) );
    docattrs.setAttribute( "hasTOC", static_cast<int>(m_hasTOC));
    docattrs.setAttribute( "tabStopValue", m_tabStop );

    // Save visual info for the first view, such as the active frameset and cursor position
    // It looks like a hack, but reopening a document creates only one view anyway (David)
    KWView * view = static_cast<KWView*>(views().getFirst());
    if ( view ) // no view if embedded document
    {
        KWFrameSetEdit* edit = view->getGUI()->canvasWidget()->currentFrameSetEdit();
        if ( edit )
        {
            docattrs.setAttribute( "activeFrameset", edit->frameSet()->name() );
            KWTextFrameSetEdit* textedit = dynamic_cast<KWTextFrameSetEdit *>(edit);
            if ( textedit && textedit->cursor() ) {
                KoTextCursor* cursor = textedit->cursor();
                docattrs.setAttribute( "cursorParagraph", cursor->parag()->paragId() );
                docattrs.setAttribute( "cursorIndex", cursor->index() );
            }
        }
    }

    if( !m_bookmarkList.isEmpty() )
    {
        QDomElement bookmark = doc.createElement( "BOOKMARKS" );
        kwdoc.appendChild( bookmark );

        QPtrListIterator<KWBookMark> book(m_bookmarkList);
        for ( ; book.current() ; ++book )
        {
            if ( book.current()->startParag()!=0 &&
                 book.current()->endParag()!=0 &&
                 !book.current()->frameSet()->isDeleted())
            {
                QDomElement bookElem = doc.createElement( "BOOKMARKITEM" );
                bookmark.appendChild( bookElem );
                bookElem.setAttribute( "name", book.current()->bookMarkName());
                bookElem.setAttribute( "frameset", book.current()->frameSet()->name());
                bookElem.setAttribute( "startparag", book.current()->startParag()->paragId());
                bookElem.setAttribute( "endparag", book.current()->endParag()->paragId());

                bookElem.setAttribute( "cursorIndexStart", book.current()->bookmarkStartIndex());
                bookElem.setAttribute( "cursorIndexEnd", book.current()->bookmarkEndIndex());

            }
        }
    }
    variableCollection()->variableSetting()->save(kwdoc );

    QDomElement framesets = doc.createElement( "FRAMESETS" );
    kwdoc.appendChild( framesets );

    m_textImageRequests.clear(); // for KWTextImage
    QValueList<KoPictureKey> savePictures;

    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameSet = fit.current();
        // Save non-part framesets ( part are saved further down )
        if ( frameSet->type() != FT_PART )
            frameSet->save( framesets );
        else
        {
            // Set the child geometry from the frame geometry, with no viewmode applied
            // to prepare saving below with the correct geometry
            static_cast<KWPartFrameSet *>(frameSet)->updateChildGeometry( 0L );
        }

        // If picture frameset, make a note of the image it needs.
        if ( !frameSet->isDeleted() && ( frameSet->type() == FT_PICTURE ) )
        {
            KoPictureKey key = static_cast<KWPictureFrameSet *>( frameSet )->key();
            if ( !savePictures.contains( key ) )
                savePictures.append( key );
        }
    }

    // Process the data of the KWTextImage classes.
    QPtrListIterator<KWTextImage> textIt ( m_textImageRequests );
    for ( ; textIt.current() ; ++textIt )
    {
        KoPictureKey key = textIt.current()->getKey();
        kdDebug(32001) << "KWDocument::saveXML registering text image " << key.toString() << endl;
        if ( !savePictures.contains( key ) )
            savePictures.append( key );
    }

    QDomElement styles = doc.createElement( "STYLES" );
    kwdoc.appendChild( styles );
    QPtrList<KoParagStyle> styleList(m_styleColl->styleList());
    for ( KoParagStyle * p = styleList.first(); p != 0L; p = styleList.next() )
        saveStyle( p, styles );

    QDomElement frameStyles = doc.createElement( "FRAMESTYLES" );
    kwdoc.appendChild( frameStyles );
    QPtrList<KWFrameStyle> m_frameStyleList(m_frameStyleColl->frameStyleList());
    for ( KWFrameStyle * p = m_frameStyleList.first(); p != 0L; p = m_frameStyleList.next() )
        saveFrameStyle( p, frameStyles );

    QDomElement tableStyles = doc.createElement( "TABLESTYLES" );
    kwdoc.appendChild( tableStyles );
    QPtrList<KWTableStyle> m_tableStyleList(m_tableStyleColl->tableStyleList());
    for ( KWTableStyle * p = m_tableStyleList.first(); p != 0L; p = m_tableStyleList.next() )
        saveTableStyle( p, tableStyles );

    if (specialOutputFlag()==SaveAsKOffice1dot1)
    {
        m_pictureCollection->saveXMLAsKOffice1Dot1( doc, kwdoc, savePictures );
    }
    else
    {
        QDomElement pictures = m_pictureCollection->saveXML( KoPictureCollection::CollectionPicture, doc, savePictures );
        kwdoc.appendChild( pictures );
    }
    // Not needed anymore
#if 0
    // Write out the list of parags (id) that form the table of contents, see KWContents::createContents
    if ( contents->hasContents() ) {
        QDomElement cParags = doc.createElement( "CPARAGS" );
        kwdoc.appendChild( cParags );
        QValueList<int>::Iterator it = contents->begin();
        for ( ; it != contents->end(); ++it )
        {
            QDomElement paragElem = doc.createElement( "PARAG" );
            cParags.appendChild( paragElem );
            paragElem.setAttribute( "name", QString::number( *it ) ); // write parag id
        }
    }
#endif

    QDomElement mailMerge=m_slDataBase->save(doc);
    kwdoc.appendChild(mailMerge);

    if( !m_spellCheckIgnoreList.isEmpty() )
    {
        QDomElement spellCheckIgnore = doc.createElement( "SPELLCHECKIGNORELIST" );
        kwdoc.appendChild( spellCheckIgnore );
        for ( QStringList::ConstIterator it = m_spellCheckIgnoreList.begin(); it != m_spellCheckIgnoreList.end(); ++it )
        {
            QDomElement spellElem = doc.createElement( "SPELLCHECKIGNOREWORD" );
            spellCheckIgnore.appendChild( spellElem );
            spellElem.setAttribute( "word", *it );
        }
    }

    // Save embedded objects
    saveEmbeddedObjects( kwdoc, children() );
    return doc;
}

// KWord-1.3 format
void KWDocument::saveEmbeddedObjects( QDomElement& parentElem, const QPtrList<KoDocumentChild>& childList )
{
    // Write "OBJECT" tag for every child, appending "EMBEDDING" tags to the main XML
    QPtrListIterator<KoDocumentChild> chl( childList );
    QDomDocument doc = parentElem.ownerDocument();
    for( ; chl.current(); ++chl ) {
        KWChild* curr = static_cast<KWChild*>(chl.current());
        if ( !curr->isDeleted() )
        {
            QDomElement embeddedElem = doc.createElement( "EMBEDDED" );
            parentElem.appendChild( embeddedElem );

            QDomElement objectElem = curr->save( doc, true );
            embeddedElem.appendChild( objectElem );

            QDomElement settingsElem = doc.createElement( "SETTINGS" );
            embeddedElem.appendChild( settingsElem );

            curr->partFrameSet()->save( settingsElem );
        }
    }
}

// KWord-1.3 format
void KWDocument::saveStyle( KoParagStyle *sty, QDomElement parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement styleElem = doc.createElement( "STYLE" );
    parentElem.appendChild( styleElem );

    sty->saveStyle( styleElem );

    QDomElement formatElem = KWTextParag::saveFormat( doc, &sty->format(), 0L, 0, 0 );
    styleElem.appendChild( formatElem );
}

// KWord-1.3 format
void KWDocument::saveFrameStyle( KWFrameStyle *sty, QDomElement parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement frameStyleElem = doc.createElement( "FRAMESTYLE" );
    parentElem.appendChild( frameStyleElem );

    sty->saveFrameStyle( frameStyleElem );
}

// KWord-1.3 format
void KWDocument::saveTableStyle( KWTableStyle *sty, QDomElement parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement tableStyleElem = doc.createElement( "TABLESTYLE" );
    parentElem.appendChild( tableStyleElem );

    sty->saveTableStyle( tableStyleElem );
}


QValueList<KoPictureKey> KWDocument::savePictureList()
{
    QValueList<KoPictureKey> savePictures;

    // At first, we must process the data of the KWTextImage classes.
    // Process the data of the KWTextImage classes.
    QPtrListIterator<KWTextImage> textIt ( m_textImageRequests );
    for ( ; textIt.current() ; ++textIt )
    {
        KoPictureKey key = textIt.current()->getKey();
        kdDebug(32001) << "KWDocument::saveXML registering text image " << key.toString() << endl;
        if ( !savePictures.contains( key ) )
            savePictures.append( key );
    }
    m_textImageRequests.clear(); // Save some memory!

    // Now do the images/cliparts in frames.
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameSet = fit.current();
        // If picture frameset, make a note of the image it needs.
        if ( !frameSet->isDeleted() && ( frameSet->type() == FT_PICTURE ) )
        {
            KoPictureKey key = static_cast<KWPictureFrameSet *>( frameSet )->key();
            if ( !savePictures.contains( key ) )
                savePictures.append( key );
        }
    }
    return savePictures;
}

// KWord-1.3 format
bool KWDocument::completeSaving( KoStore *_store )
{
    if ( !_store )
        return TRUE;

    QString u = KURL( url() ).path();

    QValueList<KoPictureKey> savePictures( savePictureList() );

    if (specialOutputFlag()==SaveAsKOffice1dot1)
    {
        return m_pictureCollection->saveToStoreAsKOffice1Dot1( KoPictureCollection::CollectionImage, _store, savePictures );
    }
    else
    {
        return m_pictureCollection->saveToStore( KoPictureCollection::CollectionPicture, _store, savePictures );
    }
}

int KWDocument::supportedSpecialFormats() const
{
    return SaveAsKOffice1dot1 | KoDocument::supportedSpecialFormats();
}

void KWDocument::addView( KoView *_view )
{
    m_lstViews.append( (KWView*)_view );
    KoDocument::addView( _view );
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it ) {
        (*it)->deselectAllFrames();
    }
}

void KWDocument::removeView( KoView *_view )
{
    m_lstViews.remove( static_cast<KWView*>(_view) );
    KoDocument::removeView( _view );
}

void KWDocument::addShell( KoMainWindow *shell )
{
    connect( shell, SIGNAL( documentSaved() ), m_commandHistory, SLOT( documentSaved() ) );
    connect( shell, SIGNAL( saveDialogShown() ), this, SLOT( saveDialogShown() ) );
    KoDocument::addShell( shell );
}

KoView* KWDocument::createViewInstance( QWidget* parent, const char* name )
{
    return new KWView( m_viewMode, parent, name, this );
}

// Paint this document when it's embedded
// This is also used to paint the preview.png that goes into the ZIP file
void KWDocument::paintContent( QPainter& painter, const QRect& _rect, bool transparent, double zoomX, double zoomY )
{
    //kdDebug(32001) << "KWDocument::paintContent m_zoom=" << m_zoom << " zoomX=" << zoomX << " zoomY=" << zoomY << " transparent=" << transparent << endl;

    setZoom( 100 );
    if ( m_zoomedResolutionX != zoomX || m_zoomedResolutionY != zoomY )
    {
        setResolution( zoomX, zoomY );
        bool forPrint = painter.device() && painter.device()->devType() == QInternal::Printer;
        newZoomAndResolution( false, forPrint );
        if ( KFormula::Document* formulaDocument = m_formulaDocumentWrapper->document() )
            formulaDocument->setZoomAndResolution( m_zoom, zoomX, zoomY, false, forPrint );
    }

    QRect rect( _rect );
    // Translate the painter to avoid the margin
    /*painter.translate( -leftBorder(), -topBorder() );
    rect.moveBy( leftBorder(), topBorder() );*/

    KWViewModeEmbedded * viewMode = new KWViewModeEmbedded( this );

    QColorGroup cg = QApplication::palette().active();

    if (!transparent)
    {
        QRegion emptyRegion( rect );
        createEmptyRegion( rect, emptyRegion, viewMode );
        eraseEmptySpace( &painter, emptyRegion, cg.brush( QColorGroup::Base ) );
    }

    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameset = fit.current();
        if ( frameset->isVisible( viewMode ) && !frameset->isFloating() )
            frameset->drawContents( &painter, rect, cg, false /*onlyChanged*/, true /*resetChanged*/,
                                    0L, viewMode );
    }
    delete viewMode;
}

QPixmap KWDocument::generatePreview( const QSize& size )
{
    int oldZoom = m_zoom;
    double oldResolutionX = resolutionX();
    double oldResolutionY = resolutionY();
    double oldZoomX = zoomedResolutionX();
    double oldZoomY = zoomedResolutionY();

    // Sometimes (due to the different resolution?) the layout creates a new page
    // while saving the preview. If this happens, we don't want to repaint the real views
    // (due to KWCanvas::slotNewContentsSize)
    // ##### One day when we have real doc/view separation in kotextparag, we shouldn't mess with
    // the real view's resolution, we should instead create a fake view for the preview itself.
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it ) {
        (*it)->getGUI()->canvasWidget()->setUpdatesEnabled( false );
    }
    Q_ASSERT( !m_bGeneratingPreview );
    m_bGeneratingPreview = true;
    QPixmap pix = KoDocument::generatePreview(size);

    // Restore everything as it was before
    setResolution( oldResolutionX, oldResolutionY );
    setZoom( oldZoom );

    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it ) {
        (*it)->getGUI()->canvasWidget()->setUpdatesEnabled( true );
    }
    newZoomAndResolution( true /*set contents size again*/, false );
    m_bGeneratingPreview = false;
    if ( KFormula::Document* formulaDocument = m_formulaDocumentWrapper->document() ) {
        formulaDocument->setZoomAndResolution( oldZoom, oldZoomX, oldZoomY );
    }
    return pix;
}

void KWDocument::createEmptyRegion( const QRect & crect, QRegion & emptyRegion, KWViewMode * viewMode )
{
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameset = fit.current();
        if ( frameset->isVisible( viewMode ) )
            frameset->createEmptyRegion( crect, emptyRegion, viewMode );
    }
}

void KWDocument::eraseEmptySpace( QPainter * painter, const QRegion & emptySpaceRegion, const QBrush & brush )
{
    painter->save();
    painter->setClipRegion( emptySpaceRegion, QPainter::CoordPainter );
    painter->setPen( Qt::NoPen );

    //kdDebug(32001) << "KWDocument::eraseEmptySpace emptySpaceRegion: " << emptySpaceRegion << endl;
    //kdDebug(32001) << "                            boundingRect: " << DEBUGRECT( emptySpaceRegion.boundingRect() ) << endl;
    painter->fillRect( emptySpaceRegion.boundingRect(), brush );
    painter->restore();
}

KWChild* KWDocument::createChildDoc( const KoRect& rect, KoDocument* childDoc )
{
    KWChild* ch = new KWChild( this, rect.toQRect(), childDoc );
    insertChild( ch );
    return ch;
}

void KWDocument::insertObject( const KoRect& rect, KoDocumentEntry& _e )
{
    KoDocument* doc = _e.createDoc( this );
    if ( !doc )
        return;
    if ( !doc->initDoc(KoDocument::InitDocEmbedded) )
        return;

    KWChild* ch = createChildDoc( rect, doc );
    setModified( TRUE );

    KWPartFrameSet *frameset = new KWPartFrameSet( this, ch, QString::null );
    KWFrame *frame = new KWFrame(frameset, rect.x(), rect.y(), rect.width(), rect.height() );
    frame->setZOrder( maxZOrder( frame->pageNumber(this) ) + 1 ); // make sure it's on top
    frameset->addFrame( frame );
    addFrameSet( frameset );
    frameset->updateChildGeometry( viewMode() ); // set initial coordinates of 'ch' correctly

    KWCreateFrameCommand *cmd = new KWCreateFrameCommand( i18n("Create Part Frame"), frame);
    addCommand(cmd);

    frameChanged( frame ); // repaint etc.
}


void KWDocument::delayedRepaintAllViews() {
    if (!m_repaintAllViewsPending) {
        QTimer::singleShot( 0, this, SLOT( slotRepaintAllViews() ) );
        m_repaintAllViewsPending=true;
    }
}

void KWDocument::slotRepaintAllViews() {
    m_repaintAllViewsPending=false;
    repaintAllViews( false );
}

void KWDocument::delayedRecalcFrames( int fromPage ) {
    //kdDebug() << k_funcinfo << fromPage << endl;
    if ( m_recalcFramesPending == -1 || fromPage < m_recalcFramesPending )
    {
        m_recalcFramesPending = fromPage;
        QTimer::singleShot( 0, this, SLOT( slotRecalcFrames() ) );
    }
}

void KWDocument::slotRecalcFrames() {
    int from = m_recalcFramesPending;
    kdDebug() << k_funcinfo << "from=" << from << endl;
    m_recalcFramesPending = -1;
    if ( from != -1 )
        recalcFrames( from );
}

void KWDocument::repaintAllViewsExcept( KWView *_view, bool erase )
{
    //kdDebug(32001) << "KWDocument::repaintAllViewsExcept" << endl;
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it ) {
        KWView* viewPtr = *it;
        if ( viewPtr != _view /*&& viewPtr->getGUI() && viewPtr->getGUI()->canvasWidget()*/ ) {
            viewPtr->getGUI()->canvasWidget()->repaintAll( erase );
        }
    }
}

void KWDocument::setUnit( KoUnit::Unit _unit )
{
    m_unit = _unit;
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it ) {
        KWView* viewPtr = *it;
        if ( viewPtr->getGUI() ) {
            viewPtr->getGUI()->getHorzRuler()->setUnit( m_unit );
            viewPtr->getGUI()->getVertRuler()->setUnit( m_unit );
        }
    }
}

void KWDocument::updateAllStyleLists()
{
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->updateStyleList();
}

void KWDocument::updateStyleListOrder( const QStringList &list )
{
    styleCollection()->updateStyleListOrder( list );
}

void KWDocument::applyStyleChange( KoStyleChangeDefMap changed )
{
    QPtrList<KWTextFrameSet> textFramesets = allTextFramesets( true );

    KWTextFrameSet *frm;
    for ( frm=textFramesets.first(); frm != 0; frm=textFramesets.next() ){
        frm->applyStyleChange( changed );
    }
}

void KWDocument::updateAllFrameStyleLists()
{
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->updateFrameStyleList();
}

void KWDocument::updateAllTableStyleLists()
{
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->updateTableStyleList();
}

void KWDocument::repaintAllViews( bool erase )
{
    //kdDebug(32001) << "KWDocument::repaintAllViews" << endl;
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->getGUI()->canvasWidget()->repaintAll( erase );
}

QPtrList<KWFrame> KWDocument::framesToCopyOnNewPage( int afterPageNum ) const // can be -1 for 'before page 0'
{
    // Look at frames on pages afterPageNum and afterPageNum-1 (for sheetside stuff)
    QPtrList<KWFrame> framesToLookAt;
    if ( afterPageNum >= 0 )
        framesToLookAt = framesInPage( afterPageNum, false );

    if ( afterPageNum >= 1 )
    {
        QPtrList<KWFrame> framesToAlsoLookAt = framesInPage( afterPageNum-1, false ); // order doesn't matter

        // Merge into single list. Other alternative, two loops, code inside moved to another method.
        QPtrListIterator<KWFrame> frameAlsoIt( framesToAlsoLookAt );
        for ( ; frameAlsoIt.current(); ++frameAlsoIt )
            framesToLookAt.append( frameAlsoIt.current() );
    }

    QPtrList<KWFrame> framesToCopy; // the result

    QPtrListIterator<KWFrame> frameIt( framesToLookAt );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame * frame = frameIt.current();
        KWFrameSet* frameSet = frame->frameSet();

        // don't add tables! A table cell ( frameset ) _must_ not have cells auto-added to them!
        if ( frameSet->type() == FT_TABLE ) continue;

        /* copy the frame if: - it is on this page or
           - it is on the former page and the frame is set to double sided.
           - AND the frame is set to be reconnected or copied
           -  */
#ifdef DEBUG_PAGES
        kdDebug(32002) << "KWDocument::framesToCopyOnNewPage looking at frame " << frame << ", pageNum=" << frame->pageNumber() << " from " << frameSet->name() << endl;
        static const char * newFrameBh[] = { "Reconnect", "NoFollowup", "Copy" };
        kdDebug(32002) << "   frame->newFrameBehavior()==" << newFrameBh[frame->newFrameBehavior()] << endl;
#endif
        if ( (frame->pageNumber() == afterPageNum ||
              (frame->pageNumber() == afterPageNum -1 && frame->sheetSide() != KWFrame::AnySide) )
             &&
             ( ( frame->newFrameBehavior()==KWFrame::Reconnect && frameSet->type() == FT_TEXT ) ||  // (*)
               ( frame->newFrameBehavior()==KWFrame::Copy && !frameSet->isAHeader() && !frameSet->isAFooter() ) ) // (**)
            )
        {
            // (*) : Reconnect only makes sense for text frames
            // (**) : NewFrameBehavior == Copy is handled here except for headers/footers, which
            // are created in recalcFrames() anyway.
            framesToCopy.append( frame );
        }
    }
    return framesToCopy;
}

KWPage* KWDocument::insertPage( int afterPageNum ) // can be -1 for 'before page 0'
{
#ifdef DEBUG_PAGES
    kdDebug(32002) << "insertPage: afterPageNum=" << afterPageNum << endl;
#endif
    if ( processingType() == WP )
        Q_ASSERT( afterPageNum == pageCount() -1 ); // WP mode: can only append.

    double pageHeight = pageManager()->page( afterPageNum )->height();
    // If not appending, move down everything after 'afterPageNum', to make room.
    for ( int pg = pageCount () -1 ; pg > afterPageNum ; --pg )
    {
        // pg is the 'src' page. Its contents must be moved to the page pg+1
        QPtrList<KWFrame> frames = framesInPage( pg, false );
#ifdef DEBUG_PAGES
        kdDebug(32002) << "insertPage: moving " << frames.count() << " frames down, from page " << pg << endl;
#endif
        QPtrListIterator<KWFrame> frameIt( frames );
        for ( ; frameIt.current(); ++frameIt )
            frameIt.current()->moveBy( 0, pageHeight );
    }

    KWPage *page = pageManager()->insertPage(afterPageNum+1);

    // Fill in the new page
    QPtrList<KWFrame> framesToCopy = framesToCopyOnNewPage( afterPageNum );
    QPtrListIterator<KWFrame> frameIt( framesToCopy );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame * frame = frameIt.current();

        KWFrame *newFrame = frame->getCopy();
        newFrame->moveBy( 0, pageHeight );
        //newFrame->setPageNum( frame->pageNumber()+1 );
        frame->frameSet()->addFrame( newFrame );

        if ( frame->newFrameBehavior()==KWFrame::Copy )
            newFrame->setCopy( true );
        //kdDebug(32002) << "   => created frame " << newFrame << " " << *newFrame << endl;
    }
    return page;
}

KWPage* KWDocument::appendPage()
{
#ifdef DEBUG_PAGES
    kdDebug(32002) << "KWDocument::appendPage pageCount()=" << pageCount() << " -> insertPage(" << pageCount()-1 << ")" << endl;
#endif
    return insertPage( pageCount() - 1 );
}

void KWDocument::afterAppendPage( int pageNum )
{
#ifdef DEBUG_PAGES
    kdDebug(32002) << "KWDocument::afterAppendPage " << pageNum << endl;
#endif
    if ( !m_bGeneratingPreview )
        emit newContentsSize();

    if ( isHeaderVisible() || isFooterVisible() || m_bHasEndNotes )
    {
#ifdef DEBUG_PAGES
        kdDebug(32002) << "KWDocument::afterAppendPage calling recalcFrames" << endl;
#endif
        // Get headers and footers on the new page
        // This shouldn't delete the newly created page because it's still empty though
        recalcFrames( pageNum, -1, KWFrameLayout::DontRemovePages );
#ifdef DEBUG_PAGES
        kdDebug(32002) << "KWDocument::afterAppendPage recalcFrames done" << endl;
#endif
    }
    else
    {
        // Take into account the frames on the new page, and run updateFramesOnTopOrBelow (#73819)
        updateAllFrames();
    }

    recalcVariables( VT_PGNUM );
    emit pageNumChanged();
    if ( m_viewMode->type() == "ModePreview" )
        repaintAllViews();
}

bool KWDocument::canRemovePage( int num )
{
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameSet = fit.current();
        if ( frameSet->isHeaderOrFooter() ) // don't look at headers/footers, but look at footnotes/endnotes
            continue;
        if ( frameSet->isVisible() && !frameSet->canRemovePage( num ) )
            return false;
    }
#ifdef DEBUG_PAGES
    kdDebug(32002) << "KWDocument::canRemovePage " << num << "-> TRUE" << endl;
#endif
    return true;
}

void KWDocument::removePage( int pageNum )
{
    if ( processingType() == WP )
        Q_ASSERT( pageNum == pageCount() -1 ); // WP mode: can only remove last page.
    Q_ASSERT( pageCount() > 1 );
    if ( pageCount() == 1 )
        return;

    // ## This assumes that framesInPage is up-to-date.
    QPtrList<KWFrame> framesToDelete = framesInPage( pageNum, false );
#ifdef DEBUG_PAGES
    kdDebug(32002) << "KWDocument::removePage " << pageNum << ", " << framesToDelete.count() << " frames to delete" << endl;
#endif
    QPtrListIterator<KWFrame> frameIt( framesToDelete );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame * frame = frameIt.current();
        KWFrameSet * frameSet = frame->frameSet();
        if ( frameSet->frameSetInfo() != KWFrameSet::FI_BODY )
            continue;
        frameSet->delFrame( frame, true );
    }

    // If not removing the last one, move up everything after the one we removed.
    for ( int pg = pageNum+1 ; pg < pageCount() ; ++pg )
    {
        // pg is the 'src' page. Its contents must be moved to the page pg-1
        QPtrList<KWFrame> frames = framesInPage( pg, false );
#ifdef DEBUG_PAGES
        kdDebug(32002) << "removePage: moving " << frames.count() << " frames up, from page " << pg << endl;
#endif
        QPtrListIterator<KWFrame> frameIt( frames );
        for ( ; frameIt.current(); ++frameIt )
            frameIt.current()->moveBy( 0, pageManager()->page(0)->height() );
    }

    pageManager()->removePage(pageCount()-1);
#ifdef DEBUG_PAGES
    kdDebug(32002) << "KWDocument::removePage -- -> " << pageCount() << endl;
#endif
    // Emitting this one for each page being removed helps giving the user some feedback
    emit pageNumChanged();
}

void KWDocument::afterRemovePages()
{
    //### IMHO recalcFrames should take care of updateAllFrames (it already does it partially).
    recalcFrames();
    // Do this before recalcVariables (which repaints). The removed frames must be removed from the frame caches.
    // We don't call updateAllFrames() directly, because it still calls
    // updateFramesOnTopOrBelow, which is useless (and slow) here.
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->updateFrames();

    recalcVariables( VT_PGNUM );
    if ( !m_bGeneratingPreview )
        emit newContentsSize();
    if ( m_viewMode->type() == "ModePreview" )
        repaintAllViews();
}

bool KWDocument::tryRemovingPages()
{
    int lastPage = numPages() - 1;
    bool removed = false;
    // Last frame is empty -> try removing last page, and more if necessary
    while ( lastPage > 0 && canRemovePage( lastPage ) )
    {
        removePage( lastPage ); // this modifies pageCount
        if ( lastPage <= pageCount() - 1 )
        {
            kdWarning() << "Didn't manage to remove page " << lastPage << " (still having " << pageCount() << " pages ). Aborting" << endl;
            break;
        }
        removed = true;
        lastPage = pageCount() - 1;
    }
    // Don't call afterRemovePages or recalcFrames from here, since this method is
    // itself called from KWFrameLayout (#95047)
    return removed;
}


KWFrameSet * KWDocument::frameSetByName( const QString & name )
{
    // Note: this isn't recursive, so it won't find table cells.
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        if ( fit.current()->name() == name )
            return fit.current();
    return 0L;
}

//#define DEBUG_FRAMESELECT


KWFrame * KWDocument::deepestInlineFrame(KWFrame *parent, const QPoint& nPoint, bool *border) {
#ifdef DEBUG_FRAMESELECT
    kdDebug(32001) << "KWDocument::deepestInlineFrame parent=" << parent << " nPoint=" << nPoint << endl;
#endif
    KWFrameSet *hostFrameSet=parent->frameSet();
    KoPoint docPoint( unzoomPoint( nPoint ) );
    int page = pageManager()->pageNumber(docPoint);
    QPtrList<KWFrame> frames = framesInPage(page);

    for (KWFrame *f = frames.last();f;f=frames.prev()) { // z-order
        // only consider inline frames.
        if (! f->frameSet()->isFloating())
            continue;

        // only use the frames that are embedded in the parent
        if (hostFrameSet != f->frameSet()->anchorFrameset())
            continue;

        if(f->frameAtPos(nPoint, true)) {
            if ( border ) *border = true;
            return f;
        }
        if(f->frameAtPos(nPoint)) {
            return deepestInlineFrame(f,nPoint,border);
        }
    }
    if (border != 0) *border=false;
    return parent;
}

KWFrame * KWDocument::frameBelowFrame(const QPoint& nPoint, KWFrame *frame, bool *border) {

#ifdef DEBUG_FRAMESELECT
    kdDebug(32001) << "KWDocument::frameBelowFrame frame=" << frame << " nPoint=" << nPoint << endl;
#endif

    KWFrameSet *fs = frame->frameSet();
    KoPoint docPoint( unzoomPoint( nPoint ) );
    if (fs->isFloating()) {
        // now lets be smart here; we know that a frame that is floating is embedded
        // inside its hostFrameSet frame. This basically means that the frame directly
        // below is the hostFrameSet frame :)
        // since we know nPoint is already in frame, we don't have to check for anchorFrame here.
        KWFrameSet *hostFrameSet = fs->anchorFrameset();
        KWFrame *f = hostFrameSet->frameByBorder(nPoint);
        if (f) {
            if (border) *border=true;
            return f;
        }
        f = hostFrameSet->frameAtPos(docPoint.x(),docPoint.y());
        if (f) {
            if (border) *border=false;
            return f;
        }
    }
    else {
        Q_ASSERT (frame->frameStack());
        QValueList<KWFrame*> frames = frame->frameStack()->framesBelow();
        for(int index = frames.count()-1; index >= 0; index--) {
            KWFrame *f = *(frames.at(index));
            if (f->frameAtPos(nPoint,true)) {
                if(border) *border=true;
                return f;
            }
            if (f->frameAtPos(nPoint)) {
                return deepestInlineFrame(f,nPoint,border);
            }
        }
    }
    if (border != 0) *border=false;
    return 0L;
}

KWFrame * KWDocument::topFrameUnderMouse( const QPoint& nPoint, bool* border) {
#ifdef DEBUG_FRAMESELECT
    kdDebug(32001) << "KWDocument::topFrameUnderMouse nPoint=" << nPoint << endl;
#endif
    KoPoint docPoint( unzoomPoint( nPoint ) );
    int page = pageManager()->pageNumber(docPoint);
    QPtrList<KWFrame> frames = framesInPage(page);

#ifdef DEBUG_FRAMESELECT
    for (KWFrame *f = frames.last();f;f=frames.prev()) { // z-order
        kdDebug(32001) << "KWDocument::topFrameUnderMouse frame " << f << " (from " << f->frameSet()->name() << ") zOrder=" << f->zOrder() << endl;
    }
#endif

    for (KWFrame *f = frames.last();f;f=frames.prev()) { // z-order
        // only consider non-inline frames.
        if (f->frameSet()->isFloating())
            continue;

        if(f->frameAtPos(nPoint, true)) {
#ifdef DEBUG_FRAMESELECT
            kdDebug(32001) << "KWDocument::topFrameUnderMouse found frame " << f << " by border" << endl;
#endif
            if ( border ) *border = true;
            return f;
        }
        if(f->frameAtPos(nPoint)) {
#ifdef DEBUG_FRAMESELECT
            kdDebug(32001) << "KWDocument::topFrameUnderMouse found frame " << f << ", will dig into it." << endl;
#endif
            return deepestInlineFrame(f,nPoint,border);
        }
    }
    if (border != 0) *border=false;
    return 0L;
}


KWFrame * KWDocument::frameUnderMouse( const QPoint& nPoint, bool* border, bool firstNonSelected )
{
    if ( !m_viewMode->hasFrames() )
    {
        KWViewModeText* vmt = dynamic_cast<KWViewModeText *>( m_viewMode );
        return vmt ? vmt->textFrameSet()->frame(0) : 0L;
    }
#ifdef DEBUG_FRAMESELECT
    kdDebug(32001) << "KWDocument::frameUnderMouse nPoint=" << nPoint << " firstNonSelected=" << firstNonSelected << endl;
#endif
    KWFrame *candidate = topFrameUnderMouse(nPoint, border);
    if (!firstNonSelected)
        return candidate;
    KWFrame *goDeeper=candidate;
    bool foundselected=false;
    while (goDeeper) {
        while (goDeeper && goDeeper->isSelected())
        {
            goDeeper=frameBelowFrame(nPoint, goDeeper, border);
            foundselected=true;
        }
        if (foundselected) {
            if (goDeeper)
                return goDeeper;
            else
                return candidate;
        } else
            goDeeper=frameBelowFrame(nPoint, goDeeper, border);

    }
    return candidate;
}

MouseMeaning KWDocument::getMouseMeaning( const QPoint &nPoint, int keyState, KWFrame** pFrame )
{
    if ( pFrame )
        *pFrame = 0L;
    if (m_viewMode->hasFrames() &&
        positionToSelectRowcolTable(nPoint) != TABLE_POSITION_NONE)
        return MEANING_MOUSE_SELECT;

    bool border=true;
    KWFrame *frameundermouse = frameUnderMouse(nPoint, &border);
    if (frameundermouse) {
        KWFrameSet *frameSet = frameundermouse->frameSet();
        if ( pFrame )
            *pFrame = frameundermouse;
        if ( m_viewMode->hasFrames() )
            return frameSet->getMouseMeaning(nPoint, keyState);
        else // text view mode
            return MEANING_MOUSE_INSIDE_TEXT;
    }
    // Allow to select paragraphs by clicking on the left
    // TODO - introduce real mouse-meaning value, to change the cursor.
    // TODO: test if other word-processors allow doing it with text-boxes
    // when clicking on the left of the text box means clicking inside another frame.
    // Also note that one can currently select text in the very first parag by clicking
    // above the frame; this would need a normal "inside text" cursor, not a left one.
    if ( m_viewMode->hasFrames() )
    {
        return MEANING_MOUSE_INSIDE_TEXT;
    }
    return MEANING_NONE;
}

QCursor KWDocument::getMouseCursor( const QPoint &nPoint, int keyState )
{
    KWFrame* frame = 0L;
    MouseMeaning meaning = getMouseMeaning( nPoint, keyState, &frame );
    KWFrameSet* frameSet = frame ? frame->frameSet() : 0L;
    switch ( meaning ) {
    case MEANING_NONE:
        return Qt::ibeamCursor; // default cursor in margins
    case MEANING_MOUSE_INSIDE:
        return QCursor(); // default cursor !?!?
    case MEANING_MOUSE_INSIDE_TEXT:
        return Qt::ibeamCursor;
    case MEANING_MOUSE_OVER_LINK:
        return Qt::PointingHandCursor;
    case MEANING_MOUSE_OVER_FOOTNOTE:
        return Qt::PointingHandCursor;
    case MEANING_MOUSE_MOVE:
        if ( frameSet && frameSet->isProtectSize() )
            return Qt::forbiddenCursor;
        else
            return Qt::sizeAllCursor;
    case MEANING_MOUSE_SELECT:
        return KCursor::handCursor();
    case MEANING_ACTIVATE_PART:
        return KCursor::handCursor();
    case MEANING_TOPLEFT:
    case MEANING_BOTTOMRIGHT:
        if ( frameSet->isProtectSize() || frameSet->isMainFrameset())
            return Qt::forbiddenCursor;
        return Qt::sizeFDiagCursor;
    case MEANING_LEFT:
    case MEANING_RIGHT:
        if ( frameSet->isProtectSize() || frameSet->isMainFrameset())
            return Qt::forbiddenCursor;
        return Qt::sizeHorCursor;
    case MEANING_BOTTOMLEFT:
    case MEANING_TOPRIGHT:
        if ( frameSet->isProtectSize() || frameSet->isMainFrameset())
            return Qt::forbiddenCursor;
        return Qt::sizeBDiagCursor;
    case MEANING_TOP:
    case MEANING_BOTTOM:
        if ( frameSet->isProtectSize() || frameSet->isMainFrameset())
            return Qt::forbiddenCursor;
        return Qt::sizeVerCursor;
    case MEANING_RESIZE_COLUMN:
        // Bug in Qt up to Qt-3.1.1 : Qt::splitVCursor and Qt::splitHCursor are swapped!
#if QT_VERSION <= 0x030101
        return Qt::splitVCursor;
#else
        return Qt::splitHCursor;
#endif
    case MEANING_RESIZE_ROW:
#if QT_VERSION <= 0x030101
        return Qt::splitHCursor;
#else
        return Qt::splitVCursor;
#endif
    }
    return QCursor(); // default cursor !?!?
}

QString KWDocument::generateFramesetName( const QString & templateName )
{
    QString name;
    int num = 1;
    bool exists;
    do {
        name = templateName.arg( num );
        exists = frameSetByName( name );
        ++num;
    } while ( exists );
    return name;
}

/** if we are close on the left or the top of a table,
 * the user can select rows/cols */
KWDocument::TableToSelectPosition KWDocument::positionToSelectRowcolTable(const QPoint& nPoint, KWTableFrameSet **ppTable /*=0L*/) {

    static const int DISTANCE_TABLE_SELECT_ROWCOL = 5;

    KWFrame *frameundermouse, *frameclosetomouseright, *frameclosetomouseunder;

    TableToSelectPosition result = TABLE_POSITION_NONE;

    // now simply check the actual frame under the mouse
    bool border=true;
    frameundermouse = frameUnderMouse(nPoint, &border );

    // now get a frame close to the mouse pointer
    // slightly on the right (could it be that it is a table?)
    QPoint pointTestTableSelect = nPoint;
    pointTestTableSelect.rx() += DISTANCE_TABLE_SELECT_ROWCOL;
    frameclosetomouseright = frameUnderMouse(pointTestTableSelect, &border);

    pointTestTableSelect = nPoint;
    pointTestTableSelect.ry() += DISTANCE_TABLE_SELECT_ROWCOL;
    frameclosetomouseunder = frameUnderMouse(pointTestTableSelect, &border);

    KWFrame *frameclosetomouse; // the frame that we are going to test to know whether it is a table

    if ( frameclosetomouseright && frameclosetomouseright->frameSet()->groupmanager() ) {
        // ok, we can test the right frame
        frameclosetomouse = frameclosetomouseright;
        result = TABLE_POSITION_RIGHT;
    }
    else {
        // right frame is not good. maybe the one under?
        frameclosetomouse = frameclosetomouseunder;
        result = TABLE_POSITION_BOTTOM;
    }

    // is there a frame close to the cursor?
    if (frameclosetomouse) {
        if ( frameclosetomouse->frameSet()->groupmanager() && (!frameundermouse || !frameundermouse->frameSet()->groupmanager()) ) {
            // there is a frame, it is a table, and the cursor is NOT on a table ATM
            if (ppTable)
                *ppTable =frameclosetomouse->frameSet()->groupmanager();
            // place the cursor to say that we can select row/columns
            return result;
        }
    }
    return TABLE_POSITION_NONE;
}


// TODO pass viewmode for isVisible
// TODO use QValueList
QPtrList<KWFrame> KWDocument::getSelectedFrames() const {
    QPtrList<KWFrame> frames;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameSet = fit.current();
        if ( !frameSet->isVisible() || frameSet->isRemoveableHeader() )
            continue;
        QPtrListIterator<KWFrame> frameIt = frameSet->frameIterator();
        for ( ; frameIt.current(); ++frameIt )
            if ( frameIt.current()->isSelected() )
                frames.append( frameIt.current() );
    }

    return frames;
}


void KWDocument::fixZOrders() {
    KWFrame *frameFixed = 0;
    for (int pgnum = 0 ; pgnum < pageCount() ; pgnum++) {
        QPtrList<KWFrame> frames = framesInPage(pgnum);
        // scan this page to see if we need to fixup:
        // fix up if two frames have the same zOrder,
        // or if a zOrder is negative (not allowed by OASIS)
        bool need_fixup = false;
        KWFrame *f = frames.last();
        if ( !f )
            continue;
        int lastZOrder = f->zOrder();
        f = frames.prev();
        for ( ; f ; f=frames.prev() ) {
            if ( !f->frameSet()->isFloating() &&
                 ( f->zOrder() == lastZOrder || f->zOrder() < 0 ) ) {
                need_fixup = true;
                break;
            }
            lastZOrder = f->zOrder();
        }
        if ( need_fixup ) {
            int current_zorder=0;
            kdDebug() << "fixing page " << pgnum << " z-orders " << endl;
            for (KWFrame *fr = frames.first();fr;fr=frames.next()) {
                // only consider non-inline framesets.
                if (fr->frameSet()->isFloating())
                    continue;
                current_zorder++;
                fr->setZOrder(current_zorder);
                frameFixed = f;
            }
        }

        if ( m_processingType == KWDocument::WP )
        {
            // In all cases, ensure the main frames are below the rest.
            // (This could not be the case after e.g. an import filter does it wrong)
            lowerMainFrames( pgnum );
        }
    }
    if ( frameFixed )
        frameFixed->frameStack()->recalcAllFrames();
}

void KWDocument::lowerMainFrames( int pageNum )
{
    QPtrList<KWFrame> framesInPage = this->framesInPage(pageNum);
    int lowestZOrder=10000;
    for ( QPtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt )
        lowestZOrder=QMIN(lowestZOrder, frameIt.current()->zOrder());
    lowerMainFrames( pageNum, lowestZOrder );
}

// separated from the above one for KWView (which knows lowestZOrder already)
void KWDocument::lowerMainFrames( int pageNum, int lowestZOrder )
{
    // Get the main frameset and see if we have to lower its frame(s).
    QPtrList<KWFrame> framesInPage = this->framesInPage(pageNum);
    for ( QPtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt ) {
        if(frameIt.current()->frameSet()->isMainFrameset()) {
            if(lowestZOrder <= frameIt.current()->zOrder())
                frameIt.current()->setZOrder(lowestZOrder-1);
            // keep going, in case of multiple columns
        }
    }
}

// TODO pass viewmode for isVisible? Depends on how framesInPage is being used...
// -- answer to above question; kwview uses it for alerin z-order and kwdoc uses it a lot. But thats it.
QPtrList<KWFrame> KWDocument::framesInPage( int pageNum, bool sorted ) const {

    ZOrderedFrameList frames;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameSet = fit.current();
        if ( !frameSet->isVisible() || frameSet->isRemoveableHeader() )
            continue;
        // Append all frames from frameSet in page pageNum
        QPtrListIterator<KWFrame> it( frameSet->framesInPage( pageNum ) );
        for ( ; it.current() ; ++it )
            frames.append( it.current() );
    }
    if (sorted) frames.sort();
    return frames;
}


KWFrame *KWDocument::getFirstSelectedFrame() const
{
    // ### This should be done much more efficiently (caching?). It's called all the time.
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameSet = fit.current();
        for ( unsigned int j = 0; j < frameSet->frameCount(); j++ ) {
            if ( !frameSet->isVisible() || frameSet->isRemoveableHeader() )
                continue;
            if ( frameSet->frame( j )->isSelected() )
                return frameSet->frame( j );
        }
    }
    return 0L;
}

void KWDocument::updateAllFrames( int flags )
{
#ifdef DEBUG_SPEED
    QTime dt;
    dt.start();
#endif
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->updateFrames( flags );

#ifdef DEBUG_SPEED
    kdDebug(32001) << "updateAllFrames(" << flags << ") took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;
#endif

    // TODO: check all calls to updateAllFrames, and fix them.
    // E.g., if one frame moved, updateAllFrames isn't necessary,
    // only fs->updateFrames() and doc->updateFramesOnTopOrBelow() are necessary.

    // Update frames ontop and below _afterwards_,
    // it needs the 'frames in page' array (in other framesets)
    KWFrameList::recalcAllFrames(this);
}

// Tell this method when a frame is moved / resized / created / deleted
// and everything will be update / repainted accordingly
void KWDocument::frameChanged( KWFrame * frame, KWView * view )
{
    if ( !frame ) // TODO call another method for 'deleted frame', which passes the frameset
        updateAllFrames(); // ... in order to get rid of that call, and use the 'else' case instead
    else {
        frame->frameSet()->updateFrames();
        KWFrameList::recalcAllFrames(this);
    }

    //kdDebug(32002) << "KWDocument::frameChanged" << endl;
    // If frame with text flowing around it -> re-layout all frames
    if ( !frame || frame->runAround() != KWFrame::RA_NO )
    {
        layout();
    }
    else
    {
        frame->frameSet()->layout();
    }
    repaintAllViewsExcept( view );
    updateRulerFrameStartEnd();
    if ( frame && frame->isSelected() )
        updateFrameStatusBarItem();
}

void KWDocument::framesChanged( const QPtrList<KWFrame> & frames, KWView * view )
{
    //kdDebug(32002) << "KWDocument::framesChanged" << endl;
    // TODO replace with 'make unique list of framesets', call updateFrames on those,
    // then call updateFramesOnTopOrBelow.
    updateAllFrames();
    // Is there at least one frame with a text runaround set ?
    QPtrListIterator<KWFrame> it( frames );
    for ( ; it.current() ; ++it )
        if ( it.current()->runAround() != KWFrame::RA_NO )
        {
            //kdDebug(32002) << "KWDocument::framesChanged ->layout" << endl;
            layout();
            //kdDebug(32002) << "KWDocument::framesChanged ->repaintAllViewsExcept" << endl;
            repaintAllViewsExcept( view );
            break;
        }
    updateRulerFrameStartEnd();
    // Is at least one frame selected ?
    QPtrListIterator<KWFrame> it2( frames );
    for ( ; it2.current() ; ++it2 )
        if ( it2.current()->isSelected() ) {
            updateFrameStatusBarItem();
            break;
        }
}

void KWDocument::setHeaderVisible( bool h )
{
    m_headerVisible = h;
    recalcFrames();
    updateAllFrames();
    layout();
    setModified(true);
    repaintAllViews( true );
}

void KWDocument::setFooterVisible( bool f )
{
    m_footerVisible = f;
    recalcFrames();
    updateAllFrames();
    layout();
    setModified(true);
    repaintAllViews( true );
}

bool KWDocument::hasEndNotes() const
{
    return m_bHasEndNotes;
}

void KWDocument::updateHeaderButton()
{
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
    {
        (*it)->updateHeaderFooterButton();
        (*it)->updateHeader();
    }
}

void KWDocument::updateFooterButton()
{
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
    {
        (*it)->updateHeaderFooterButton();
        (*it)->updateFooter();
    }
}


bool KWDocument::isOnlyOneFrameSelected() const {
    return getSelectedFrames().count() == 1;
}

void KWDocument::setFramePadding( double l, double r, double t, double b )
{
    // todo, make this more OO, and update the tableheaders as well..
    for ( unsigned int i = 0; i < numFrameSets(); i++ ) {
        if ( frameSet( i )->hasSelectedFrame() ) {
            KWFrameSet *frameset = frameSet( i );
            for ( unsigned int j = 0; j < frameset->frameCount(); j++ ) {
                if ( frameset->frame( j )->isSelected() ) {
                    frameset->frame( j )->setPaddingLeft( l );
                    frameset->frame( j )->setPaddingRight( r );
                    frameset->frame( j )->setPaddingTop( t );
                    frameset->frame( j )->setPaddingBottom( b );
                }
            }
        }
    }

    setModified(TRUE);
}

void KWDocument::addTextImageRequest( KWTextImage *img )
{
    m_textImageRequests.append( img );
}

void KWDocument::addPictureRequest( KWPictureFrameSet *fs )
{
    m_pictureRequests.append( fs );
}

void KWDocument::addAnchorRequest( const QString &framesetName, const KWAnchorPosition &anchorPos )
{
    m_anchorRequests.insert( framesetName, anchorPos );
}

void KWDocument::addFootNoteRequest( const QString &framesetName, KWFootNoteVariable* var )
{
    if ( var->noteType() == EndNote )
        m_bHasEndNotes = true;
    m_footnoteVarRequests.insert( framesetName, var );
}


void KWDocument::refreshMenuCustomVariable()
{
   emit sig_refreshMenuCustomVariable();
}


void KWDocument::recalcVariables( int type )
{
    m_varColl->recalcVariables(type);
    if ( !m_bGeneratingPreview )
        slotRepaintVariable();

#if 0
    bool update = false;
    QPtrListIterator<KWVariable> it( variables );
    QPtrList<KWTextFrameSet> toRepaint;
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->type() == type )
        {
            update = true;
            it.current()->recalc();
            KoTextParag * parag = it.current()->paragraph();
            if ( parag )
            {
                kdDebug(32002) << "KWDoc::recalcVariables -> invalidating parag " << parag->paragId() << endl;
                parag->invalidate( 0 );
                parag->setChanged( true );
                KWTextFrameSet * textfs = static_cast<KWTextDocument *>(it.current()->textDocument())->textFrameSet();
                if ( toRepaint.findRef( textfs ) == -1 )
                    toRepaint.append( textfs );
            }
        }
    }
    for ( KWTextFrameSet * fs = toRepaint.first() ; fs ; fs = toRepaint.next() )
        slotRepaintChanged( fs );
#endif
}

// TODO pass list of textdocuments as argument
void KWDocument::slotRepaintVariable()
{
    QPtrListIterator<KWFrameSet> it = framesetsIterator();
    for (; it.current(); ++it )
        if( it.current()->type()==FT_TEXT && it.current()->isVisible() )
            slotRepaintChanged( (*it) );
#if 0
    KWTextFrameSet * textfs = static_cast<KWTextDocument *>(it.current()->textDocument())->textFrameSet();
    if ( toRepaint.findRef( textfs ) == -1 )
        toRepaint.append( textfs );


    QPtrListIteratorQPtrList<KWTextFrameSet> toRepaint;
        for ( KWTextFrameSet * fs = toRepaint.first() ; fs ; fs = toRepaint.next() )
        slotRepaintChanged( fs );
#endif
}

int KWDocument::mailMergeRecord() const
{
    return slRecordNum;
}

void KWDocument::setMailMergeRecord( int r )
{
    slRecordNum = r;
}

void KWDocument::getPageLayout( KoPageLayout& _layout, KoColumns& _cl, KoKWHeaderFooter& _hf )
{
    _layout = m_pageLayout;
    _cl = m_pageColumns;
    _hf = m_pageHeaderFooter;
}

void KWDocument::addFrameSet( KWFrameSet *f, bool finalize /*= true*/ )
{
    if(m_lstFrameSet.contains(f) > 0) {
        kdWarning(32001) << "Frameset " << f << " " << f->name() << " already in list!" << endl;
        return;
    }
    m_lstFrameSet.append(f);

    KWFrameList::createFrameList(f, this);

    if ( finalize )
        f->finalize();
    setModified( true );
}

void KWDocument::removeFrameSet( KWFrameSet *f )
{
    emit sig_terminateEditing( f );
    m_lstFrameSet.take( m_lstFrameSet.find(f) );
    setModified( true );
}

void KWDocument::addCommand( KCommand * cmd )
{
    Q_ASSERT( cmd );
    //kdDebug(32001) << "KWDocument::addCommand " << cmd->name() << endl;
    m_commandHistory->addCommand( cmd, false );
    setModified( true );
}

void KWDocument::slotDocumentRestored()
{
    setModified( false );
}

void KWDocument::slotCommandExecuted()
{
    setModified( true );
}

#ifndef NDEBUG
void KWDocument::printStyleDebug()
{
    kdDebug() << "----------------------------------------"<<endl;
    QPtrList<KoParagStyle> styleList(m_styleColl->styleList());
    for ( KoParagStyle * p = styleList.first(); p != 0L; p = styleList.next() )
    {
        kdDebug() << "Style " << p << "  " << p->name() <<endl;
        kdDebug() << "   format: " << p->format().key() <<endl;
        static const char * const s_align[] = { "Auto", "Left", "Right", "ERROR", "HCenter", "ERR", "ERR", "ERR", "Justify", };
        kdDebug() << "  align: " << s_align[(Qt::AlignmentFlags)p->paragLayout().alignment] << endl;

        kdDebug() << "   following style: " << p->followingStyle() << " "
                  << ( p->followingStyle() ? p->followingStyle()->name() : QString::null ) << endl;
    }
}

void KWDocument::printDebug()
{
    kdDebug() << "----------------------------------------"<<endl;
    kdDebug() << "                 Debug info"<<endl;
    kdDebug() << "Document:" << this <<endl;
    kdDebug() << "Type of document: (0=WP, 1=DTP) " << processingType() <<endl;
    //kdDebug() << "size: x:" << ptLeftBorder()<< ", y:"<<ptTopBorder() << ", w:"<< ptPaperWidth() << ", h:"<<ptPaperHeight()<<endl;
    kdDebug() << "Header visible: " << isHeaderVisible() << endl;
    kdDebug() << "Footer visible: " << isFooterVisible() << endl;
    kdDebug() << "Units: " << unit() <<endl;
    kdDebug() << "# Framesets: " << numFrameSets() <<endl;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( unsigned int iFrameset = 0; fit.current() ; ++fit, iFrameset++ )
    {
        KWFrameSet * frameset = fit.current();
        kdDebug() << "Frameset " << iFrameset << ": '" <<
            frameset->name() << "' (" << frameset << ")" << (frameset->isDeleted()?" Deleted":"")<<endl;
        if ( frameset->isVisible())
            frameset->printDebug();
        else
            kdDebug() << "  [hidden] #" << frameset->frameCount() << " frames" << endl;
    }

    for ( uint pgNum = 0 ; pgNum < m_sectionTitles.size() ; ++pgNum )
        kdDebug() << "Page " << pgNum << "  Section: '" << m_sectionTitles[ pgNum ] << "'"<< endl;
    /*
    kdDebug() << "# Images: " << getImageCollection()->iterator().count() <<endl;
    QDictIterator<KWImage> it( getImageCollection()->iterator() );
    while ( it.current() ) {
        kdDebug() << " + " << it.current()->getFilename() << ": "<<it.current()->refCount() <<endl;
        ++it;
    }
    */
}
#endif

void KWDocument::layout()
{
    QPtrListIterator<KWFrameSet> it = framesetsIterator();
    for (; it.current(); ++it )
        if ( it.current()->isVisible() )
            it.current()->layout();
}

void KWDocument::invalidate(const KWFrameSet *skipThisFrameSet)
{
    QPtrListIterator<KWFrameSet> it = framesetsIterator();
    for (; it.current(); ++it )
        if(it.current()!=skipThisFrameSet)
            it.current()->invalidate();
}

KFormula::Document* KWDocument::formulaDocument()
{
    KFormula::Document* formulaDocument = m_formulaDocumentWrapper->document();
    if (!formulaDocument) {
        kdDebug() << k_funcinfo << endl;
        formulaDocument = new KFormula::Document;
        m_formulaDocumentWrapper->document( formulaDocument );
        if ( formulaDocument != 0 ) {
            // re-calculate dpiX and dpiY
            formulaDocument->setZoomAndResolution( m_zoom,
                                                   qRound(INCH_TO_POINT( m_resolutionX )),
                                                   qRound(INCH_TO_POINT( m_resolutionY )) );
            formulaDocument->newZoomAndResolution(false,false);
        }
    }
    return formulaDocument;
}


void KWDocument::slotRepaintChanged( KWFrameSet * frameset )
{
    // This has to be a loop instead of a signal, so that we can
    // send "true" for the last view (see KWFrameSet::drawContents)
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it ) {
        (*it)->getGUI()->canvasWidget()->repaintChanged( frameset, it == m_lstViews.fromLast() );
    }
}

void KWDocument::refreshFrameBorderButton()
{

    KWFrame *frame= getFirstSelectedFrame();
    if (frame)
    {
        frame = KWFrameSet::settingsFrame(frame);
        for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it ) {
            (*it)->showFrameBorders( frame->leftBorder(), frame->rightBorder(), frame->topBorder(), frame->bottomBorder() );
        }
    }
}

void KWDocument::repaintResizeHandles()
{
   QPtrList<KWFrame> selectedFrames = getSelectedFrames();
   KWFrame *frame=0L;
   for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
   {
       frame->repaintResizeHandles();
   }
}

void KWDocument::updateResizeHandles( )
{
   QPtrList<KWFrame> selectedFrames = getSelectedFrames();
   KWFrame *frame=0L;
   for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
   {
       frame->updateResizeHandles();
   }
   updateRulerFrameStartEnd();
}

void KWDocument::updateCursorType( )
{
   QPtrList<KWFrame> selectedFrames = getSelectedFrames();
   KWFrame *frame=0L;
   for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
   {
       frame->updateCursorType();
   }
}

void KWDocument::deleteTable( KWTableFrameSet *table )
{
    if ( !table )
        return;
    table->deselectAll();
    if ( table->isFloating() )
    {
        emit sig_terminateEditing( table ); // to unselect its cells, especially
        KWAnchor * anchor = table->findAnchor( 0 );
        addCommand( table->anchorFrameset()->deleteAnchoredFrame( anchor ) );
    }
    else
    {
        KWDeleteTableCommand *cmd = new KWDeleteTableCommand( i18n("Delete Table"), table );
        addCommand( cmd );
        cmd->execute();
    }
}

void KWDocument::deleteFrame( KWFrame * frame )
{
    KWFrameSet * fs = frame->frameSet();
    kdDebug(32002) << "KWDocument::deleteFrame frame=" << frame << " fs=" << fs << endl;
    frame->setSelected(false);
    QString cmdName;
    TypeStructDocItem docItem = (TypeStructDocItem) 0;
    switch (fs->type() ) {
    case FT_TEXT:
        cmdName=i18n("Delete Text Frame");
        docItem=TextFrames;
        break;
    case FT_FORMULA:
        cmdName=i18n("Delete Formula Frame");
        docItem=FormulaFrames;
        break;
    case FT_CLIPART:
    {
        kdError(32001) << "FT_CLIPART used! (in KWDocument::loadFrameSet)" << endl;
        // Do not break!
    }
    case FT_PICTURE:
        cmdName=i18n("Delete Picture Frame");
        docItem=Pictures;
        break;
    case FT_PART:
        cmdName=i18n("Delete Object Frame");
        docItem=Embedded;
        break;
#if 0 // KWORD_HORIZONTAL_LINE
    case FT_HORZLINE:
        cmdName=i18n("Delete Horizontal Line");
        break;
#endif
    case FT_TABLE:
    case FT_BASE:
        Q_ASSERT( 0 );
        break;
    }
    if ( fs->isFloating() )
    {
        KWAnchor * anchor = fs->findAnchor( 0 );
        addCommand( fs->anchorFrameset()->deleteAnchoredFrame( anchor ) );
    }
    else
    {
        KWDeleteFrameCommand *cmd = new KWDeleteFrameCommand( cmdName, frame );
        addCommand( cmd );
        cmd->execute();
    }
    emit docStructureChanged(docItem);
}

void KWDocument::deleteSelectedFrames()
{
    QPtrList<KWFrame> frames=getSelectedFrames();
    int nbCommand=0;
    KWFrame *tmp=0;

    int docItem=0;

    KMacroCommand * macroCmd = new KMacroCommand( i18n("Delete Frames") );
    for ( tmp=frames.first(); tmp != 0; tmp=frames.next() )
    {
        KWFrameSet *fs = tmp->frameSet();
        if ( fs->isAFooter() || fs->isAHeader() )
            continue;
        //a table
        if ( fs->groupmanager() )
        {
            KWTableFrameSet *table=fs->groupmanager();
            Q_ASSERT(table);
            docItem|=typeItemDocStructure(table->type());

            if ( table->isFloating() )
            {
                emit sig_terminateEditing( table ); // to unselect its cells, especially
                docItem|=typeItemDocStructure(fs->type());

                KWAnchor * anchor = table->findAnchor( 0 );
                KCommand * cmd=table->anchorFrameset()->deleteAnchoredFrame( anchor );
                macroCmd->addCommand(cmd);
                nbCommand++;
            }
            else
            {
                KWDeleteTableCommand *cmd = new KWDeleteTableCommand( i18n("Delete Table"), table );
                cmd->execute();
                macroCmd->addCommand(cmd);
                nbCommand++;
            }
        }
        else
        {// a simple frame
            if ( fs->type() == FT_TEXT)
            {
                if ( processingType() == KWDocument::WP && frameSetNum( fs ) == 0 )
                    continue;
            }

            docItem|=typeItemDocStructure(fs->type());

            if ( fs->isFloating() )
            {
                tmp->setSelected( false );
                KWAnchor * anchor = fs->findAnchor( 0 );
                KCommand *cmd=fs->anchorFrameset()->deleteAnchoredFrame( anchor );
                macroCmd->addCommand(cmd);
                nbCommand++;
            }
            else
            {
                KWDeleteFrameCommand *cmd = new KWDeleteFrameCommand( i18n("Delete Frame"), tmp );
                cmd->execute();
                macroCmd->addCommand(cmd);
                nbCommand++;
            }
        }
    }
    if( nbCommand )
    {
        addCommand(macroCmd);
        emit refreshDocStructure(docItem);
    }
    else
        delete macroCmd;

}

void KWDocument::reorganizeGUI()
{
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->getGUI()->reorganize();
}

void KWDocument::slotDocumentInfoModifed()
{
    if (!variableCollection()->variableSetting()->displayFieldCode())
        recalcVariables( VT_FIELD );
}

void KWDocument::refreshDocStructure(int type)
{
     emit docStructureChanged(type);
}

int KWDocument::typeItemDocStructure(FrameSetType _type)
{
    int typeItem;
    switch(_type)
    {
        case FT_TEXT:
            typeItem=(int)TextFrames;
            break;
        case FT_PICTURE:
            typeItem=(int)Pictures;
            break;
        case FT_PART:
            typeItem=(int)Embedded;
            break;
        case FT_FORMULA:
            typeItem=(int)FormulaFrames;
            break;
        case FT_TABLE:
            typeItem=(int)Tables;
            break;
        default:
            typeItem=(int)TextFrames;
    }
    return typeItem;
}

void KWDocument::refreshDocStructure(FrameSetType _type)
{
    emit docStructureChanged(typeItemDocStructure(_type));
}

QBrush KWDocument::resolveBgBrush( const QBrush & brush, QPainter * painter )
{
    if ( brush.color().isValid() )
        return brush;
    QBrush ret( brush );
    ret.setColor( defaultBgColor( painter ) );
    return ret;
}

QColor KWDocument::resolveBgColor( const QColor & col, QPainter * painter )
{
    if (col.isValid())
        return col;

    return defaultBgColor( painter );
}

QColor KWDocument::defaultBgColor( QPainter * painter )
{
    if ( painter && painter->device()->devType() == QInternal::Printer )
        return Qt::white;
    return QApplication::palette().color( QPalette::Active, QColorGroup::Base );
}


void KWDocument::renameButtonTOC(bool b)
{
    m_hasTOC=b;
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->renameButtonTOC(b);
}

void KWDocument::refreshMenuExpression()
{
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->refreshMenuExpression();
}

void KWDocument::frameSelectedChanged()
{
    emit sig_frameSelectedChanged();
}

void KWDocument::updateZoomRuler()
{
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it ) {
        (*it)->getGUI()->getHorzRuler()->setZoom( zoomedResolutionX() );
        (*it)->getGUI()->getVertRuler()->setZoom( zoomedResolutionY() );
        (*it)->slotUpdateRuler();
    }
}

void KWDocument::updateRulerFrameStartEnd()
{
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->slotUpdateRuler();
}

void KWDocument::updateFrameStatusBarItem()
{
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->updateFrameStatusBarItem();
}

int KWDocument::undoRedoLimit() const
{
    return m_commandHistory->undoLimit();
}

void KWDocument::setUndoRedoLimit(int val)
{
    m_commandHistory->setUndoLimit(val);
    m_commandHistory->setRedoLimit(val);
}

void KWDocument::setGridX(double _gridx) {
    m_gridX = _gridx;
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->getGUI()->getHorzRuler()->setGridSize(_gridx);
}

QValueList<KoTextObject *> KWDocument::visibleTextObjects(KWViewMode *viewmode) const
{
    QValueList<KoTextObject *> lst;
    QPtrList<KWTextFrameSet> textFramesets = allTextFramesets( true );

    KWTextFrameSet *frm;
    for ( frm=textFramesets.first(); frm != 0; frm=textFramesets.next() ) {
        if ( frm && frm->isVisible(viewmode) && !frm->textObject()->protectContent() )
        {
            lst.append( frm->textObject() );
        }
    }

    return lst;
}

void KWDocument::refreshGUIButton()
{
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->initGUIButton();
}

void KWDocument::enableBackgroundSpellCheck( bool b )
{
    m_bgSpellCheck->setEnabled(b);
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->updateBgSpellCheckingState();
}

bool KWDocument::backgroundSpellCheckEnabled() const
{
    return m_bgSpellCheck->enabled();
}

void KWDocument::reactivateBgSpellChecking()
{
    QPtrList<KWTextFrameSet> textFramesets = allTextFramesets( true );

    KWTextFrameSet *frm;
    for ( frm=textFramesets.first(); frm != 0; frm=textFramesets.next() ){
        frm->textObject()->setNeedSpellCheck(true);
    }
    repaintAllViews();
    startBackgroundSpellCheck();
}

void KWDocument::slotChapterParagraphFormatted( KoTextParag* /*parag*/ )
{
    // Attempt at invalidating from the parag's page only
    // But that's not good enough - if a header gets moved down,
    // we also need to invalidate the previous page, from where the paragraph disappeared.
    /*
      KoPoint p;
    KWFrame* frame = internalToDocument( parag->rect().topLeft(), p );
    Q_ASSERT( frame );
    if ( frame )
        // Remove any information from this page and further pages.
        m_sectionTitles.resize( frame->pageNumber() );
    */

    m_sectionTitles.resize( 0 ); // clear up the entire cache

    // Don't store info from parag into m_sectionTitles here.
    // It breaks when having two headings in the same page
    // (or if it keeps existing info then it can't update properly)
}

QString KWDocument::checkSectionTitleInParag( KoTextParag* parag, KWTextFrameSet* frameset, int pageNum ) const
{
    if ( parag->counter() && parag->counter()->numbering() == KoParagCounter::NUM_CHAPTER
         && parag->counter()->depth() == 0 )
    {
        QString txt = parag->string()->toString();
        txt = txt.left( txt.length() - 1 ); // remove trailing space
#ifndef NDEBUG // not needed, just checking
        KoPoint p;
        KWFrame* frame = frameset->internalToDocument( parag->rect().topLeft(), p );
        Q_ASSERT( frame );
        if ( frame ) {
            int pgNum = frame->pageNumber();
            if( pgNum != pageNum )
                kdWarning() << "sectionTitle: was looking for pageNum " << pageNum << ", got frame " << frame << " page " << pgNum << endl;
        }
        kdDebug(32001) << "KWDocument::sectionTitle for " << pageNum << ":" << txt << endl;
#endif
        // Ensure array is big enough
        if ( pageNum > (int)m_sectionTitles.size()-1 )
            const_cast<KWDocument*>(this)->m_sectionTitles.resize( pageNum + 1 );
        const_cast<KWDocument*>(this)->m_sectionTitles[ pageNum ] = txt;
        return txt;
    }
    return QString::null;
}

QString KWDocument::sectionTitle( int pageNum ) const
{
    //kdDebug(32001) << "KWDocument::sectionTitle(pageNum=" << pageNum << ") m_sectionTitles.size()=" << m_sectionTitles.size() << endl;
    // First look in the cache. If info is present, it's uptodate (see slotChapterParagraphFormatted)
    if ( (int)m_sectionTitles.size() > pageNum )
    {
        // Look whether this page has a section title, and if not, go back pages, one by one
        for ( int i = pageNum; i >= 0 ; --i )
        {
            const QString& s = m_sectionTitles[i];
            if ( !s.isEmpty() )
            {
                // Update cache, to make this faster next time
                if ( pageNum > (int)m_sectionTitles.size()-1 )
                    const_cast<KWDocument*>(this)->m_sectionTitles.resize( pageNum + 1 );
                const_cast<KWDocument*>(this)->m_sectionTitles[ pageNum ] = s;
                return s;
            }
        }
    }

    // If not in the cache, determine from the paragraphs in the page.

    if ( m_lstFrameSet.isEmpty() )
        return QString::null;
    // We use the "main" frameset to determine section titles.
    KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet *>( m_lstFrameSet.getFirst() );
    if ( !frameset )
        return QString::null;

    int topLUpix, bottomLUpix;
    if ( !frameset->minMaxInternalOnPage( pageNum, topLUpix, bottomLUpix ) )
        return QString::null;

    KoTextParag* parag = frameset->textDocument()->firstParag();
    //kdDebug(32001) << "KWDocument::sectionTitle " << pageNum
    //          << " topLUpix=" << topLUpix << " bottomLUpix=" << bottomLUpix << endl;

    KoTextParag* lastParagOfPageAbove = parag;
    for ( ; parag ; parag = parag->next() )
    {
        if ( parag->rect().bottom() < topLUpix ) // too early
        {
            lastParagOfPageAbove = parag;
            continue;
        }
        if ( parag->rect().top() > bottomLUpix ) // done
            break;
        QString txt = checkSectionTitleInParag( parag, frameset, pageNum );
        if ( !txt.isEmpty() )
            return txt;
    }

    // No heading found in page.
    // Go back up until the first section parag
    parag = lastParagOfPageAbove;
    for (  ; parag ; parag = parag->prev() )
    {
        QString txt = checkSectionTitleInParag( parag, frameset, pageNum );
        if ( !txt.isEmpty() )
            return txt;
    }

    // First page, no heading found
    return QString::null;
}


void KWDocument::setSpellCheckIgnoreList( const QStringList& lst )
{
    m_spellCheckIgnoreList = lst;
    m_bgSpellCheck->settings()->setCurrentIgnoreList( m_spellCheckIgnoreList + m_spellCheckPersonalDict );
    setModified( true );
}

void KWDocument::addSpellCheckIgnoreWord( const QString & word )
{
    // ### missing: undo/redo support
    if( m_spellCheckIgnoreList.findIndex( word ) == -1 )
        m_spellCheckIgnoreList.append( word );
    setSpellCheckIgnoreList( m_spellCheckIgnoreList );
    if ( backgroundSpellCheckEnabled() )
        // Re-check everything to make this word normal again
        reactivateBgSpellChecking();
}

int KWDocument::maxZOrder( int pageNum) const
{
    bool first = true;
    int maxZOrder = 0; //this value is only used if there's no frame on the page
    QPtrList<KWFrame> frames = framesInPage( pageNum );
    QPtrListIterator<KWFrame> frameIt( frames );
    for ( ; frameIt.current(); ++frameIt ) {
        if ( first || frameIt.current()->zOrder() > maxZOrder ) {
            maxZOrder = frameIt.current()->zOrder();
            first = false;
        }
    }
    return maxZOrder;
}

QPtrList<KWTextFrameSet> KWDocument::allTextFramesets(bool onlyReadWrite) const
{
    QPtrList<KWTextFrameSet> textFramesets;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit ) {
        if(fit.current()->isDeleted()) continue;
        fit.current()->addTextFrameSets(textFramesets, onlyReadWrite);
    }
    return textFramesets;
}

QValueList<KoTextDocument *> KWDocument::allTextDocuments() const
{
    QValueList<KoTextDocument *> lst;
    const QPtrList<KWTextFrameSet> textFramesets = allTextFramesets(false);
    QPtrListIterator<KWTextFrameSet> fit( textFramesets );
    for ( ; fit.current() ; ++fit ) {
        lst.append( fit.current()->textObject()->textDocument() );
    }
    return lst;
}

int KWDocument::numberOfTextFrameSet( KWFrameSet* fs, bool onlyReadWrite )
{
    QPtrList<KWTextFrameSet> textFramesets = allTextFramesets( onlyReadWrite );
    return textFramesets.findRef( static_cast<KWTextFrameSet*>(fs) );
}

KWFrameSet * KWDocument::textFrameSetFromIndex( unsigned int _num, bool onlyReadWrite )
{
    return allTextFramesets( onlyReadWrite ).at( _num );
}

void KWDocument::updateTextFrameSetEdit()
{
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->slotFrameSetEditChanged();

}

void KWDocument::displayFootNoteFieldCode()
{
    QPtrListIterator<KoVariable> it( m_varColl->getVariables() );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->type() == VT_FOOTNOTE )
        {
            static_cast<KWFootNoteVariable *>(it.current())->resize();
            static_cast<KWFootNoteVariable *>(it.current())->frameSet()->setCounterText( static_cast<KWFootNoteVariable *>(it.current())->text() );

            KoTextParag * parag = it.current()->paragraph();
            if ( parag )
            {
                parag->invalidate( 0 );
                parag->setChanged( true );
            }
        }
    }
}

void KWDocument::changeFootNoteConfig()
{
    QPtrListIterator<KoVariable> it( m_varColl->getVariables() );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->type() == VT_FOOTNOTE )
        {
            static_cast<KWFootNoteVariable *>(it.current())->formatedNote();
            static_cast<KWFootNoteVariable *>(it.current())->resize();
            static_cast<KWFootNoteVariable *>(it.current())->frameSet()->setCounterText( static_cast<KWFootNoteVariable *>(it.current())->text() );

            KoTextParag * parag = it.current()->paragraph();
            if ( parag )
            {
                parag->invalidate( 0 );
                parag->setChanged( true );
            }
        }
    }
    slotRepaintVariable();
}


void KWDocument::setTabStopValue ( double _tabStop )
{
    m_tabStop = _tabStop;
    QPtrList<KWTextFrameSet> textFramesets = allTextFramesets( true );

    KWTextFrameSet *frm;
    for ( frm=textFramesets.first(); frm != 0; frm=textFramesets.next() ){
        frm->textDocument()->setTabStops( ptToLayoutUnitPixX( _tabStop ));
        frm->layout();
    }
    repaintAllViews();
}

void KWDocument::setGlobalHyphenation( bool _hyphen )
{
    m_bGlobalHyphenation = _hyphen;
    // This is only used as a default setting for the default format in new documents;
    // In existing documents the hyphenation comes from the existing formats.
}

void KWDocument::switchViewMode( KWViewMode * newViewMode )
{
    // Don't compare m_viewMode and newViewMode here, it would break
    // changing the number of pages per row for the preview mode, in kwconfig.
    delete m_viewMode;
    m_viewMode = newViewMode;
    m_lastViewMode = m_viewMode->type(); // remember for saving config

    //necessary to switchmode view in all canvas in first.
    //otherwise in multiview kword crash !
    //perhaps it's not a good idea to store m_modeView into kwcanvas.
    //but it's necessary for the future when kword will support
    //different view mode in different view.
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->getGUI()->canvasWidget()->switchViewMode( m_viewMode );

    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->switchModeView();
    emit newContentsSize();
    updateResizeHandles();

    // Since the text layout depends on the view mode, we need to redo it
    // But after telling the canvas about the new viewmode, otherwise stuff like
    // slotNewContentsSize will crash.
    updateAllFrames();
    layout();

    repaintAllViews( true );
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->getGUI()->canvasWidget()->ensureCursorVisible();
}

void KWDocument::changeBgSpellCheckingState( bool b )
{
    enableBackgroundSpellCheck( b );
    reactivateBgSpellChecking();
    KConfig *config = KWFactory::instance()->config();
    config->setGroup("KSpell kword" );
    config->writeEntry( "SpellCheck", (int)b );
}

QString KWDocument::initialFrameSet() const
{
    return m_initialEditing ? m_initialEditing->m_initialFrameSet : QString::null;
}

int KWDocument::initialCursorParag() const
{
    return m_initialEditing ? m_initialEditing->m_initialCursorParag : 0;
}

int KWDocument::initialCursorIndex() const
{
    return m_initialEditing ? m_initialEditing->m_initialCursorIndex : 0;
}

void KWDocument::deleteInitialEditingInfo()
{
    delete m_initialEditing;
    m_initialEditing = 0L;
}

bool KWDocument::cursorInProtectedArea()const
{
    return m_cursorInProtectectedArea;
}

void KWDocument::setCursorInProtectedArea( bool b )
{
    m_cursorInProtectectedArea=b;
    testAndCloseAllFrameSetProtectedContent();
}


void KWDocument::testAndCloseAllFrameSetProtectedContent()
{
    if ( !m_cursorInProtectectedArea )
    {
        for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
            (*it)->testAndCloseAllFrameSetProtectedContent();
    }
}

void KWDocument::updateRulerInProtectContentMode()
{
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->updateRulerInProtectContentMode();
}


void KWDocument::insertBookMark(const QString &_name, KWTextParag *_startparag,KWTextParag *_endparag, KWFrameSet *_frameSet, int _start, int _end)
{
    KWBookMark *book = new KWBookMark( _name, _startparag, _endparag, _frameSet, _start, _end );
    m_bookmarkList.append( book );
}

void KWDocument::deleteBookMark(const QString &_name)
{
    QPtrListIterator<KWBookMark> book(m_bookmarkList);
    for ( ; book.current() ; ++book )
    {
        if ( book.current()->bookMarkName()==_name)
        {
            m_bookmarkList.remove(book.current());
            setModified(true);
            break;
        }
    }
}

void KWDocument::renameBookMark(const QString &_oldName, const QString &_newName)
{
    if ( _oldName==_newName)
        return;
    QPtrListIterator<KWBookMark> book(m_bookmarkList);
    for ( ; book.current() ; ++book )
    {
        if ( book.current()->bookMarkName()==_oldName)
        {
            book.current()->setBookMarkName(_newName );
            setModified(true);
            break;
        }
    }
}

KWBookMark * KWDocument::bookMarkByName( const QString & name )
{
    QPtrListIterator<KWBookMark> book(m_bookmarkList);
    for ( ; book.current() ; ++book )
    {
        if ( book.current()->bookMarkName()==name)
            return book.current();
    }
    return 0L;
}

QStringList KWDocument::listOfBookmarkName(KWViewMode * viewMode)const
{
    QStringList list;
    if ( viewMode && viewMode->type()!="ModeText")
    {
        QPtrListIterator<KWBookMark> book(m_bookmarkList);
        for ( ; book.current() ; ++book )
        {
            if ( !book.current()->frameSet()->isDeleted())
                list.append( book.current()->bookMarkName());
        }
    }
    else
    {
        QPtrListIterator<KWBookMark> book(m_bookmarkList);
        for ( ; book.current() ; ++book )
        {
            if ( book.current()->frameSet()->isVisible( viewMode )&& !book.current()->frameSet()->isDeleted())
                list.append( book.current()->bookMarkName());
        }
    }
    return list;
}

void KWDocument::paragraphModified(KoTextParag* /*_parag*/, int /*KoTextParag::ParagModifyType*/ /*_type*/, int /*start*/, int /*lenght*/)
{
    //kdDebug()<<" _parag :"<<_parag<<" start :"<<start<<" lenght :"<<lenght<<endl;
}


void KWDocument::paragraphDeleted( KoTextParag *_parag, KWFrameSet *frm )
{
    if ( m_bookmarkList.isEmpty() )
        return;
    QPtrListIterator<KWBookMark> book(m_bookmarkList);
    for ( ; book.current() ; ++book ) {
        KWBookMark* bk = book.current();
        if ( bk->frameSet()==frm ) {
            // Adjust bookmark to point to a valid paragraph, below or above the deleted one.
            // The old implementation turned the bookmark into a useless one. OOo simply deletes the bookmark...
            if ( bk->startParag() == _parag )
                bk->setStartParag( _parag->next() ? _parag->next() : _parag->prev() );
            if ( bk->endParag() == _parag )
                bk->setEndParag( _parag->next() ? _parag->next() : _parag->prev() );
        }
    }
}

void KWDocument::initBookmarkList()
{
    Q_ASSERT( m_loadingInfo );
    if ( !m_loadingInfo )
        return;
    KWLoadingInfo::BookMarkList bookmarks();
    KWLoadingInfo::BookMarkList::Iterator it = m_loadingInfo->bookMarkList.begin();
    KWLoadingInfo::BookMarkList::Iterator end = m_loadingInfo->bookMarkList.end();
    for( ; it != end; ++it )
    {
        KWFrameSet * fs = 0L;
        QString fsName = (*it).frameSetName;
        if ( !fsName.isEmpty() )
            fs = frameSetByName( fsName );
        if ( fs )
        {
            KWTextFrameSet *frm = dynamic_cast<KWTextFrameSet *>(fs);
            if ( frm )
            {
                KoTextParag* startparag = frm->textDocument()->paragAt( (*it).paragStartIndex );
                KoTextParag* endparag = frm->textDocument()->paragAt( (*it).paragEndIndex );
                if ( startparag && endparag )
                {
                    KWBookMark *bk = new KWBookMark( (*it).bookname );
                    bk->setFrameSet( frm );
                    bk->setStartParag( startparag );
                    bk->setEndParag( endparag );
                    bk->setBookmarkStartIndex( (*it).cursorStartIndex );
                    bk->setBookmarkEndIndex( (*it).cursorEndIndex );
                    m_bookmarkList.append( bk );
                }
            }
        }
    }
}

QPixmap* KWDocument::doubleBufferPixmap( const QSize& s )
{
    if ( !m_bufPixmap ) {
	int w = QABS( s.width() );
	int h = QABS( s.height() );
	m_bufPixmap = new QPixmap( w, h );
    } else {
	if ( m_bufPixmap->width() < s.width() ||
	     m_bufPixmap->height() < s.height() ) {
	    m_bufPixmap->resize( QMAX( s.width(), m_bufPixmap->width() ),
				QMAX( s.height(), m_bufPixmap->height() ) );
	}
    }

    return m_bufPixmap;
}

void KWDocument::maybeDeleteDoubleBufferPixmap()
{
    if ( m_bufPixmap && m_bufPixmap->height() * m_bufPixmap->width() > 400*400 )
    {
        delete m_bufPixmap;
        m_bufPixmap = 0L;
    }
}

void KWDocument::setPersonalExpressionPath( const QStringList & lst)
{
    m_personalExpressionPath = lst;
    refreshMenuExpression();
}

void KWDocument::updateDirectCursorButton()
{
    for( QValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->updateDirectCursorButton();
}

void KWDocument::setInsertDirectCursor(bool _b)
{
    m_bInsertDirectCursor=_b;
    KConfig *config = KWFactory::instance()->config();
    config->setGroup( "Interface" );
    config->writeEntry( "InsertDirectCursor", _b );
    updateDirectCursorButton();
}

void KWDocument::saveDialogShown()
{
    if ( !textFrameSet(0) )
        return;
    // Grab first 50 chars from the main frameset's document
    // ### This is a somewhat slow method, if the document is huge - better iterate
    // over the first few parags until 50 chars have been collected.
    QString first_row = textFrameSet(0)->textDocument()->plainText().left(50);
    bool truncate = false;
    QChar ch;
    for (int i=0; i < (int)first_row.length(); i++)
    {
        ch = first_row.at(i);
        if (!truncate)
            if (ch.isPunct() || ch.isSpace() || ch == '.' )
            {
                first_row.remove(i,1);
                --i;
            }
            else
                truncate = true;
        else if ( truncate && (ch.isPunct() || ch == '.' || ch == '\n' ) )
        {
            first_row.truncate(i);
            break;
        }
    }
    first_row = first_row.stripWhiteSpace();
    kdDebug() << "Suggested filename:" << first_row << endl;
    setURL(first_row);
}

#if 0 // KWORD_HORIZONTAL_LINE
void KWDocument::setHorizontalLinePath( const QStringList & lst)
{
    m_horizontalLinePath = lst;
}
#endif

void KWDocument::addWordToDictionary( const QString& word )
{
    if ( m_bgSpellCheck )
    {
        if( m_spellCheckPersonalDict.findIndex( word ) == -1 )
            m_spellCheckPersonalDict.append( word );
        m_bgSpellCheck->settings()->setCurrentIgnoreList( m_spellCheckIgnoreList + m_spellCheckPersonalDict );
        if ( backgroundSpellCheckEnabled() )
            // Re-check everything to make this word normal again
            reactivateBgSpellChecking();
    }
}

void KWDocument::setEmpty()
{
    KoDocument::setEmpty();
    // Whether loaded from template or from empty doc: this is a new one -> set creation date
    m_varColl->variableSetting()->setCreationDate(QDateTime::currentDateTime());
}

void KWDocument::updateGridButton()
{
  QPtrListIterator<KoView> it( views() );
  for (; it.current(); ++it )
    ((KWView*)it.current())->updateGridButton();

}

unsigned int KWDocument::paperHeight(int pageNum) const {
    return static_cast<unsigned int>(zoomItY( pageManager()->pageLayout(pageNum).ptHeight ));
}

unsigned int KWDocument::paperWidth(int pageNum) const {
    return static_cast<unsigned int>(zoomItX( pageManager()->pageLayout(pageNum).ptWidth ));
}

unsigned int KWDocument::pageTop( int pgNum ) const {
    return zoomItY( pageManager()->topOfPage( pgNum ) );
}

int KWDocument::pageCount() const {
    return pageManager()->pageCount();
}

#include "KWDocument.moc"
