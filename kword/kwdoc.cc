/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2002-2004 David Faure <faure@kde.org>

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

#include "kwdoc.h"

#include "KWordDocIface.h"
#include "kwbgspellcheck.h"
#include "kwbookmark.h"
#include "kwcanvas.h"
#include "kwcommand.h"
#include "kwformulaframe.h"
#include "kwframelayout.h"
#include "kwpartframeset.h"
#include "kwtableframeset.h"
#include "kwtablestyle.h"
#include "kwtabletemplate.h"
#include "kwtextimage.h"
#include "kwvariable.h"
#include "kwview.h"
#include "kwviewmode.h"
#include "mailmerge.h"
#include "kwloadinginfo.h"

#include <qfileinfo.h>
#include <qregexp.h>
#include <qtimer.h>

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
#include <koOasisStyles.h>
#include <koApplication.h>
#include <kooasiscontext.h>
#include <kocommandhistory.h>
#include <koGenStyles.h>
#include <koStore.h>
#include <koStoreDevice.h>
#include <koxmlwriter.h>
#include <koOasisSettings.h>

#include <kcursor.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kspell.h>
#include <kstandarddirs.h>

#include <unistd.h>
#include <math.h>
#include <ktempfile.h>

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

KWDocument::KWDocument(QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode ),
      m_urlIntern()
{
    dcop = 0;
    if (KGlobal::locale()->measureSystem() == KLocale::Imperial) {
        m_unit = KoUnit::U_INCH;
    } else {
        m_unit = KoUnit::U_CM;
    }
    m_pages = 1;
    m_loadingInfo = 0L;
    m_tabStop = MM_TO_POINT( 15.0 );
    m_processingType = WP;

    m_lstViews.setAutoDelete( false );
    m_lstChildren.setAutoDelete( true );
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

    m_personalExpressionPath = KWFactory::global()->dirs()->resourceDirs("expression");
    m_picturePath= KGlobalSettings::documentPath();

#if 0 // KWORD_HORIZONTAL_LINE
    m_horizontalLinePath = KWFactory::global()->dirs()->resourceDirs("horizontalLine");
#endif

    setInstance( KWFactory::global(), false );

    m_gridX = m_gridY = 10.0;
    m_indent = MM_TO_POINT( 10.0 );

    m_iNbPagePerRow = 4;
    m_maxRecentFiles = 10;
    m_defaultColumnSpacing=3;
    m_bShowRuler = true;

    m_footNoteSeparatorLinePos=SLP_LEFT;

    //by default it's 1/5
    m_iFootNoteSeparatorLineLength = 20;
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
    m_pgUpDownMovesCaret = false;
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
#ifdef HAVE_LIBKSPELL2

    m_bgSpellCheck = new KWBgSpellCheck(this);
#endif
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
    KConfig *config = KWFactory::global()->config();
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
#ifdef HAVE_LIBKSPELL2
    delete m_bgSpellCheck;
#endif
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
  KConfig *config = KWFactory::global()->config();
  if( config->hasGroup("KSpell kword" ) )
  {
      config->setGroup( "KSpell kword" );

      // Default is false for spellcheck, but the spell-check config dialog
      // should write out "true" when the user configures spell checking.
#ifdef HAVE_LIBKSPELL2
      if ( isReadWrite() )
          m_bgSpellCheck->setEnabled(config->readBoolEntry( "SpellCheck", false ));
      else
          m_bgSpellCheck->setEnabled( false );
#endif
  }

  if(config->hasGroup("Interface" ) )
  {
      config->setGroup( "Interface" );
      setGridY(QMAX( config->readDoubleNumEntry("GridY",10.0), 0.1));
      setGridX(QMAX( config->readDoubleNumEntry("GridX",10.0), 0.1));
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
      m_pgUpDownMovesCaret = config->readBoolEntry( "PgUpDownMovesCaret", false );
      m_bInsertDirectCursor= config->readBoolEntry( "InsertDirectCursor", false );
      m_globalLanguage=config->readEntry("language", KGlobal::locale()->language());
      m_bGlobalHyphenation=config->readBoolEntry("hyphenation", false);
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
}

void KWDocument::saveConfig()
{
    if ( isEmbedded() || !isReadWrite() )
        return;
    // Only save the config that is manipulated by the UI directly.
    // The config from the config dialog is saved by the dialog itself.
    KConfig *config = KWFactory::global()->config();
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
}

void KWDocument::setZoomAndResolution( int zoom, int dpiX, int dpiY )
{
    KoZoomHandler::setZoomAndResolution( zoom, dpiX, dpiY );
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
    m_pages = 1;

    m_pageColumns.columns = 1;
    m_pageColumns.ptColumnSpacing = m_defaultColumnSpacing;

    m_pageHeaderFooter.header = HF_SAME;
    m_pageHeaderFooter.footer = HF_SAME;
    m_pageHeaderFooter.ptHeaderBodySpacing = 10;
    m_pageHeaderFooter.ptFooterBodySpacing = 10;
    m_pageHeaderFooter.ptFootNoteBodySpacing = 10;

    QString _template;

    bool ok = FALSE;

    if (flags==KoDocument::InitDocEmpty)
    {
        QString fileName( locate( "kword_template", "Normal/.source/PlainText.kwt" , KWFactory::global() ) );
        resetURL();
        initUnit();
        ok = loadNativeFormat( fileName );
        setEmpty();
        setModified( FALSE );
        return ok;
    }

    KoTemplateChooseDia::DialogType dlgtype;

    if (flags != KoDocument::InitDocFileNew)
	    dlgtype = KoTemplateChooseDia::Everything;
    else
	    dlgtype = KoTemplateChooseDia::OnlyTemplates;


    KoTemplateChooseDia::ReturnType ret = KoTemplateChooseDia::choose(
        KWFactory::global(), _template, "application/x-kword", "*.kwd", i18n("KWord"),
        dlgtype, "kword_template");
    if ( ret == KoTemplateChooseDia::Template ) {
        QFileInfo fileInfo( _template );
        QString fileName( fileInfo.dirPath( TRUE ) + "/" + fileInfo.baseName() + ".kwt" );
        resetURL();
        ok = loadNativeFormat( fileName );
        initUnit();
        setEmpty();
    } else if ( ret == KoTemplateChooseDia::File ) {
        KURL url( _template);
        //kdDebug() << "KWDocument::initDoc opening URL " << url.prettyURL() << endl;
        ok = openURL( url );
    } else if ( ret == KoTemplateChooseDia::Empty ) {
        QString fileName( locate( "kword_template", "Normal/.source/PlainText.kwt" , KWFactory::global() ) );
        resetURL();
        initUnit();
        ok = loadNativeFormat( fileName );
        setEmpty();
    }
    setModified( FALSE );
    return ok;
}

void KWDocument::initUnit()
{
    //load default unit setting - this is only used for new files (from templates) or empty files
    KConfig *config = KWFactory::global()->config();

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
    m_pages = 1;

    m_pageColumns.columns = 1;
    m_pageColumns.ptColumnSpacing = m_defaultColumnSpacing;

    m_pageHeaderFooter.header = HF_SAME;
    m_pageHeaderFooter.footer = HF_SAME;
    m_pageHeaderFooter.ptHeaderBodySpacing = 10;
    m_pageHeaderFooter.ptFooterBodySpacing = 10;
    m_pageHeaderFooter.ptFootNoteBodySpacing = 10;

    QString fileName( locate( "kword_template", "Normal/.source/PlainText.kwt" , KWFactory::global() ) );
    /*bool ok = */loadNativeFormat( fileName );
    resetURL();
    setModified( FALSE );
    setEmpty();
}

KoPageLayout KWDocument::pageLayout() const
{
    return m_pageLayout;
}

void KWDocument::setPageLayout( const KoPageLayout& _layout, const KoColumns& _cl, const KoKWHeaderFooter& _hf, bool updateViews )
{
    if ( m_processingType == WP ) {
        int numPages = m_pages;
        //kdDebug() << "KWDocument::setPageLayout WP" << endl;
        m_pageLayout = _layout;
        m_pageColumns = _cl;
        m_pageHeaderFooter = _hf;

        if ( updateViews ) {
            // Fix things up, when we change the orientation we might accidentally change the number of pages
            // (and frames of the main textframeset might just remain un-moved...)
            KWFrameSet *frameset = m_lstFrameSet.getFirst();
            KWFrame* lastFrame = frameset->frame( frameset->getNumFrames() - 1 );
            if ( lastFrame && lastFrame->pageNum() + 1 < numPages ) {
                kdDebug(32002) << "KWDocument::setPageLayout ensuring that recalcFrames will consider " << numPages << " pages." << endl;
                // All that matters is that it's on numPages so that all pages will be recalc-ed.
                // If the text layout then wants to remove some pages, no problem.
                lastFrame->setY( numPages * ptPaperHeight() + ptTopBorder() );
            }
        }

    } else {
        //kdDebug() << "KWDocument::setPageLayout NON-WP" << endl;
        m_pageLayout = _layout;
        m_pageLayout.ptLeft = 0;
        m_pageLayout.ptRight = 0;
        m_pageLayout.ptTop = 0;
        m_pageLayout.ptBottom = 0;
        m_pageHeaderFooter = _hf;
    }

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
    return ( ptPaperWidth() - ptLeftBorder() - ptRightBorder() -
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
            switch ( getHeaderType() ) {
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
            switch ( getFooterType() ) {
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
            int pageNum = -42; //fnfs->footNoteVariable()->pageNum(); // determined by KWFrameLayout
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

        int oldPages = m_pages;
        unsigned int frms = frameset->getNumFrames();

        // Determine number of pages - first from the text frames
        // - BUT NOT from the number of frames. Some people manage to end up with
        // multiple frames of textframeset1 on the same page(!)
        //m_pages = static_cast<int>( ceil( static_cast<double>( frms ) / static_cast<double>( m_pageColumns.columns ) ) );
#ifdef DEBUG_PAGES
        //kdDebug(32002) << "KWDocument::recalcFrames frms(" << frms << ") / columns(" << m_pageColumns.columns << ") = " << m_pages << endl;
#endif
        m_pages = frameset->frame( frms - 1 )->pageNum() + 1;

        // Then from the other frames ( framesetNum > 0 )
        double maxBottom = 0;
        for (int m = numFrameSets() - 1; m > 0; m-- )
        {
            KWFrameSet *fs=frameSet(m);
            if ( fs->isVisible() && !fs->isAHeader() && !fs->isAFooter() && !fs->isFloating() && !fs->isFootEndNote() )
            {
                for (int n = fs->getNumFrames()-1; n >= 0 ; n--) {
                    //if ( n == fs->getNumFrames()-1 )
#ifdef DEBUG_PAGES
                    kdDebug(32002) << "KWDocument::recalcFrames frameset number " << m << " '" << fs->getName()
                                   << "' frame " << n << " bottom=" << fs->frame(n)->bottom() << endl;
#endif
                    maxBottom = QMAX(maxBottom, fs->frame(n)->bottom());
                }
            }
        }
        int pages2 = static_cast<int>( ceil( maxBottom / ptPaperHeight() ) );
#ifdef DEBUG_PAGES
        kdDebug(32002) << "KWDocument::recalcFrames, WP, m_pages=" << m_pages << " pages2=" << pages2
                       << " (coming from maxBottom=" << maxBottom << " and ptPaperHeight=" << ptPaperHeight() << ")"
                       << endl;
#endif

        m_pages = QMAX( pages2, m_pages );

        if ( toPage == -1 )
            toPage = m_pages - 1;
        if ( fromPage > toPage ) // this can happen with "endnotes only" pages :)
            fromPage = toPage; // ie. start at the last real page
        KWFrameLayout frameLayout( this, headerFooterList, footnotesHFList, endnotesHFList );
        frameLayout.layout( frameset, m_pageColumns.columns, fromPage, toPage, flags );

        // If the number of pages changed, update views and variables etc.
        // (now that the frame layout has been done)
        if ( m_pages != oldPages && !m_bGeneratingPreview )
        {
            // Very much like the end of appendPage, but we don't want to call recalcFrames ;)
            emit newContentsSize();
            emit pageNumChanged();
            recalcVariables( VT_PGNUM );
        }

    } else {
        // DTP mode: calculate the number of pages from the frames.
        double maxBottom=0;
        for (QPtrListIterator<KWFrameSet> fit = framesetsIterator(); fit.current() ; ++fit ) {
            if(fit.current()->isDeleted()) continue;
            if(fit.current()->frameSetInfo()==KWFrameSet::FI_BODY && !fit.current()->isFloating()) {
                KWFrameSet * fs = fit.current();
                for (QPtrListIterator<KWFrame> f = fs->frameIterator(); f.current() ; ++f ) {
#ifdef DEBUG_PAGES
                    kdDebug(32002) << " fs=" << fs->getName() << " bottom=" << f.current()->bottom() << endl;
#endif
                    maxBottom=QMAX(maxBottom, f.current()->bottom());
                }
            }
        }
        m_pages = static_cast<int>( ceil( maxBottom / ptPaperHeight() ) );
#ifdef DEBUG_PAGES
	kdDebug(32002) << "DTP mode: pages = maxBottom("<<maxBottom<<") / ptPaperHeight=" << ptPaperHeight() << " = " << m_pages << endl;
#endif
        if(m_pages < 1) m_pages=1;
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
    kdDebug(32001) << "KWDocument::loadOasis" << endl;
    m_pictureMap.clear();
    m_textImageRequests.clear();
    m_pictureRequests.clear();
    m_anchorRequests.clear();
    m_footnoteVarRequests.clear();
    m_spellListIgnoreAll.clear();

    m_pageColumns.columns = 1;
    m_pageColumns.ptColumnSpacing = m_defaultColumnSpacing;

    m_pageHeaderFooter.header = HF_SAME;
    m_pageHeaderFooter.footer = HF_SAME;
    m_pageHeaderFooter.ptHeaderBodySpacing = 10;
    m_pageHeaderFooter.ptFooterBodySpacing = 10;
    m_pageHeaderFooter.ptFootNoteBodySpacing = 10;
    m_varFormatCollection->clear();

    m_pages = 1;
    m_bHasEndNotes = false;

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
    QDomElement body ( content.namedItem( "office:body" ).toElement() );
    if ( body.isNull() )
    {
        kdError(30518) << "No office:body found!" << endl;
        setErrorMessage( i18n( "Invalid OASIS document. No office:body tag found." ) );
        return false;
    }
    body = body.namedItem( "office:text" ).toElement();
    if ( body.isNull() )
    {
        kdError(30518) << "No office:text found!" << endl;
        // ## TODO: print the actual tag that was found, it might help finding the right app to use :)
        setErrorMessage( i18n( "Invalid KWord document. No office:text tag found." ) );
        return false;
    }

    // TODO check versions and mimetypes etc.

    QString masterPageName = "Standard"; // use default layout as fallback
    // In theory the page format is the style:master-page-name of the first paragraph...
    // But, hmm, in a doc with only a table there was no reference to the master page at all...
    QDomElement* masterPage = oasisStyles.masterPages()[ masterPageName ];
    Q_ASSERT( masterPage );
    QDomElement *masterPageStyle = masterPage ? oasisStyles.styles()[masterPage->attribute( "style:page-layout-name" )] : 0;
    Q_ASSERT( masterPageStyle );
    if ( masterPageStyle )
    {
        m_pageLayout.loadOasis( *masterPageStyle );
        if ( m_pageLayout.ptWidth <= 1e-13 || m_pageLayout.ptHeight <= 1e-13 )
        {
            setErrorMessage( i18n( "Invalid document. Paper size: %1x%2" ).arg( m_pageLayout.ptWidth ).arg( m_pageLayout.ptHeight ) );
            return false;
        }

        //__hf.ptHeaderBodySpacing = getAttribute( paper, "spHeadBody", 0.0 );
        //__hf.ptFooterBodySpacing  = getAttribute( paper, "spFootBody", 0.0 );
        //__hf.ptFootNoteBodySpacing  = getAttribute( paper, "spFootNoteBody", 10.0 );

        QDomElement properties( masterPageStyle->namedItem( "style:page-layout-properties" ).toElement() );
        QDomElement footnoteSep = properties.namedItem( "style:footnote-sep" ).toElement();
        if ( !footnoteSep.isNull() ) {
            // style:width="0.018cm" style:distance-before-sep="0.101cm"
            // style:distance-after-sep="0.101cm" style:adjustment="left"
            // style:rel-width="25%" style:color="#000000"
            QString width = footnoteSep.attribute( "style:width" );

            m_footNoteSeparatorLineWidth = KoUnit::parseValue( width );
            QString pageWidth = footnoteSep.attribute( "style:rel-width" );
            if ( pageWidth.endsWith( "%" ) ) {
                pageWidth.truncate( pageWidth.length() - 1 ); // remove '%'
                m_iFootNoteSeparatorLineLength = qRound( pageWidth.toDouble() );
            }
            // Not in KWord: color, distance before and after separator
            // Not in OOo, but in OASIS now: line type of separator (solid, dot, dash etc.)
            // m_footNoteSeparatorLineType = ...  // TODO style:line-style, added in OASIS

            QString pos = footnoteSep.attribute( "style:adjustment" );
            if ( pos =="centered" )
                m_footNoteSeparatorLinePos = SLP_CENTERED;
            else if ( pos =="right")
                m_footNoteSeparatorLinePos = SLP_RIGHT;
            else if ( pos =="left" )
                m_footNoteSeparatorLinePos = SLP_LEFT;
        }

        // TODO columns  (style:columns, attribute fo:column-count)
        __columns.columns = 1; // TODO
        // TODO columnspacing (style:column-sep ?)
        __columns.ptColumnSpacing = 2; // TODO

        m_headerVisible = false;
        m_footerVisible = false;

        // TODO spHeadBody (where is this in OOo?)
        // TODO spFootBody (where is this in OOo?)
        // Answer: margins of the <style:header-footer> element
    }

    m_loadingInfo = new KWLoadingInfo;

    // <text:page-sequence> oasis extension for DTP (2003-10-27 post by Daniel)
    m_processingType = ( body.firstChild().toElement().tagName() == "text:page-sequence" )
                       ? DTP : WP;

    // TODO settings (m_unit, spellcheck settings)
    m_hasTOC = false;
    m_tabStop = MM_TO_POINT(15); // TODO
    // TODO m_initialEditing

    // TODO variable settings
    // By default display real variable value
    if ( !isReadWrite())
        getVariableCollection()->variableSetting()->setDisplayFieldCode(false);

    // TODO MAILMERGE

    KoOasisContext context( this, *m_varColl, oasisStyles, store );
    Q_ASSERT( !oasisStyles.officeStyle().isNull() );

    // Load all styles before the corresponding paragraphs try to use them!
    m_styleColl->loadOasisStyleTemplates( context );

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

#if 0 // this is wrong - the contents of body is loaded above already
        QDomNode frameElement( body );
        QDomElement fr;
        for ( frameElement = frameElement.firstChild(); !frameElement.isNull(); frameElement = frameElement.nextSibling() )
        {
            fr = frameElement.toElement();
            if ( fr.tagName()== "draw:frame"  )
            {
                QDomNode tmp = fr.namedItem( "draw:text-box" );
                if ( !tmp.isNull() )
                {
                    kdDebug()<<" text object  :"<<endl;
                    KWTextFrameSet *fs = new KWTextFrameSet( this, fr.attribute( "draw:name" ) );
                    m_lstFrameSet.append( fs ); // don't use addFrameSet here. We'll call finalize() once and for all in completeLoading

                    kdDebug()<<"fr.attribute( svg:x )"<<fr.attribute( "svg:x" )<<" fr.attribute( svg:y ) "<<fr.attribute( "svg:y" )<<" fr.attribute( svg:width ) :"<<fr.attribute( "svg:width" )<<" fr.attribute( svg:height ) :"<< fr.attribute( "svg:height" )<<endl;
//kdDebug()<<" fr.attribute( svg:x ) :"<<fr.attribute( "svg:x" )<<" fr.attribute( svg:y ) :"<<fr.attribute( "svg:y" )<<" fr.attribute( svg:width ) :"<<" fr.attribute( "svg:width" )<<endl;
                    frame = new KWFrame( fs, KoUnit::parseValue(fr.attribute( "svg:x" ) ), KoUnit::parseValue(fr.attribute( "svg:y" ) ), KoUnit::parseValue(fr.attribute( "svg:width" ) ), KoUnit::parseValue(fr.attribute( "svg:height" ) ) );
                    frame->loadCommonOasisProperties( context, fs );
                    frame->setFrameBehavior( KWFrame::AutoExtendFrame );
                    frame->setNewFrameBehavior( KWFrame::Copy );
                    fs->addFrame( frame );
                    kdDebug()<<" tmp.firstChild().isNull() :"<<tmp.firstChild().isNull()<<endl;
                    fs->loadOasisContent( tmp.firstChild().toElement(), context );

                }
                tmp = fr.namedItem( "draw:image" );
                if ( !tmp.isNull() )
                {
                    kdDebug()<<" image object  :"<<endl;
                }
            }
        }
#endif
    }

    // Header/Footer
    // TODO support for first-page
    bool hasEvenOddHeader = false;
    bool hasEvenOddFooter = false;
    QDomElement headerStyle = masterPageStyle->namedItem( "style:header-style" ).toElement();
    QDomElement footerStyle = masterPageStyle->namedItem( "style:footer-style" ).toElement();
    QDomElement headerLeftElem = masterPage->namedItem( "style:header-left" ).toElement();
    if ( !headerLeftElem.isNull() ) {
        kdDebug() << "Found header-left" << endl;
        hasEvenOddHeader = true;
        __hf.header = HF_EO_DIFF; // ###
        loadOasisHeaderFooter( headerLeftElem, hasEvenOddHeader, headerStyle, context );
    }
    QDomElement headerElem = masterPage->namedItem( "style:header" ).toElement();
    if ( !headerElem.isNull() ) {
        kdDebug() << "Found header" << endl;
        loadOasisHeaderFooter( headerElem, hasEvenOddHeader, headerStyle, context );
    }
    QDomElement footerLeftElem = masterPage->namedItem( "style:footer-left" ).toElement();
    if ( !footerLeftElem.isNull() ) {
        kdDebug() << "Found footer-left" << endl;
        hasEvenOddFooter = true;
        __hf.footer = HF_EO_DIFF; // ###
        loadOasisHeaderFooter( footerLeftElem, hasEvenOddFooter, footerStyle, context );
    }
    QDomElement footerElem = masterPage->namedItem( "style:footer" ).toElement();
    if ( !footerElem.isNull() ) {
        kdDebug() << "Found footer" << endl;
        loadOasisHeaderFooter( footerElem, hasEvenOddFooter, footerStyle, context );
    }

    // TODO embedded objects

    kdDebug(32001) << "Loading took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;
    endOfLoading();

    // This sets the columns and header/footer flags, and calls recalcFrames,
    // so it must be done last.
    setPageLayout( m_pageLayout, __columns, __hf, false );

    loadOasisSettings( settings );

    //printDebug();

    return true;
}

void KWDocument::loadOasisSettings( const QDomDocument&settingsDoc )
{
    if ( settingsDoc.isNull() )
        return ; //not a error some file doesn't have settings.xml
    KoOasisSettings settings( settingsDoc );
    bool tmp = settings.selectItemSet( "view-settings" );
    //kdDebug()<<" settings : view-settings :"<<tmp<<endl;

    if ( tmp )
    {
        tmp = settings.selectItemMap( "Views" );
        setUnit(KoUnit::unit(settings.parseConfigItemString("unit")));
        //put other loading settings here
    }
}


static QString headerTypeToFramesetName( const QString& tagName, bool hasEvenOdd )
{
    if ( tagName == "style:header" )
        return hasEvenOdd ? i18n("Odd Pages Header") : i18n( "Header" );
    if ( tagName == "style:header-left" )
        return i18n("Even Pages Header");
    if ( tagName == "style:footer" )
        return hasEvenOdd ? i18n("Odd Pages Footer") : i18n( "Footer" );
    if ( tagName == "style:footer-left" )
        return i18n("Even Pages Footer");
    kdWarning(30518) << "Unknown tag in headerTypeToFramesetName: " << tagName << endl;
    // ######
    //return i18n("First Page Header");
    //return i18n("First Page Footer");
    return QString::null;
}

static KWFrameSet::Info headerTypeToFrameInfo( const QString& tagName, bool /*hasEvenOdd*/ )
{
    if ( tagName == "style:header" )
        return KWFrameSet::FI_ODD_HEADER;
    if ( tagName == "style:header-left" )
        return KWFrameSet::FI_EVEN_HEADER;
    if ( tagName == "style:footer" )
        return KWFrameSet::FI_ODD_FOOTER;
    if ( tagName == "style:footer-left" )
        return KWFrameSet::FI_EVEN_FOOTER;

    // ### return KWFrameSet::FI_FIRST_HEADER; TODO
    // ### return KWFrameSet::FI_FIRST_FOOTER; TODO
    return KWFrameSet::FI_BODY;
}

void KWDocument::loadOasisHeaderFooter( const QDomElement& headerFooter, bool hasEvenOdd, QDomElement& style, KoOasisContext& context )
{
    const QString tagName = headerFooter.tagName();
    bool isHeader = tagName.startsWith( "style:header" );

    KWTextFrameSet *fs = new KWTextFrameSet( this, headerTypeToFramesetName( tagName, hasEvenOdd ) );
    fs->setFrameSetInfo( headerTypeToFrameInfo( tagName, hasEvenOdd ) );
    m_lstFrameSet.append( fs ); // don't use addFrameSet here. We'll call finalize() once and for all in completeLoading

    if ( !style.isNull() )
        context.styleStack().push( style );
    KWFrame* frame = new KWFrame( fs, 29, isHeader?0:567, 798-29, 41 );
    frame->loadCommonOasisProperties( context, fs );
    frame->setFrameBehavior( KWFrame::AutoExtendFrame );
    frame->setNewFrameBehavior( KWFrame::Copy );
    fs->addFrame( frame );
    if ( !style.isNull() )
        context.styleStack().pop(); // don't let it be active when parsing the text

    fs->loadOasisContent( headerFooter, context );

    if ( isHeader )
        m_headerVisible = true;
    else
        m_footerVisible = true;
}

bool KWDocument::loadXML( QIODevice *, const QDomDocument & doc )
{
    QTime dt;
    dt.start();
    emit sigProgress( 0 );
    kdDebug(32001) << "KWDocument::loadXML" << endl;
    m_pictureMap.clear();
    m_textImageRequests.clear();
    m_pictureRequests.clear();
    m_anchorRequests.clear();
    m_footnoteVarRequests.clear();
    m_spellListIgnoreAll.clear();

    m_pageColumns.columns = 1;
    m_pageColumns.ptColumnSpacing = m_defaultColumnSpacing;

    m_pageHeaderFooter.header = HF_SAME;
    m_pageHeaderFooter.footer = HF_SAME;
    m_pageHeaderFooter.ptHeaderBodySpacing = 10;
    m_pageHeaderFooter.ptFooterBodySpacing = 10;
    m_pageHeaderFooter.ptFootNoteBodySpacing = 10;
    m_varFormatCollection->clear();

    m_pages = 1;
    m_bHasEndNotes = false;

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

    m_loadingInfo = new KWLoadingInfo;

    // Looks like support for the old way of naming images internally,
    // see completeLoading.
    value = KWDocument::getAttribute( word, "url", QString::null );
    if ( value != QString::null )
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

    getVariableCollection()->variableSetting()->load(word );
    //by default display real variable value
    if ( !isReadWrite())
        getVariableCollection()->variableSetting()->setDisplayFieldCode(false);

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

    QDomElement spellCheckIgnore = word.namedItem( "SPELLCHECKIGNORELIST" ).toElement();
    if( !spellCheckIgnore.isNull() )
    {
        QDomElement spellWord=word.namedItem("SPELLCHECKIGNORELIST").toElement();
        spellWord=spellWord.firstChild().toElement();
        while ( !spellWord.isNull() )
        {
            if ( spellWord.tagName()=="SPELLCHECKIGNOREWORD" )
                m_spellListIgnoreAll.append(spellWord.attribute("word"));
            spellWord=spellWord.nextSibling().toElement();
        }
    }

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

    // create defaults if they were not in the input file.

    if ( !_first_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "First Page Header" ) );
        //kdDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameSetInfo( KWFrameSet::FI_FIRST_HEADER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptTopBorder(),
                                     ptPaperWidth() - ptLeftBorder() - ptRightBorder(), 20 );
        //kdDebug(32001) << "KWDocument::loadXML KWFrame created " << frame << endl;
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.append( fs );
    }

    if ( !_odd_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Odd Pages Header" ) );
        fs->setFrameSetInfo( KWFrameSet::FI_ODD_HEADER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptTopBorder(),
                                     ptPaperWidth() - ptLeftBorder() - ptRightBorder(), 20 );
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.append( fs );
    }

    if ( !_even_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Even Pages Header" ) );
        fs->setFrameSetInfo( KWFrameSet::FI_EVEN_HEADER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptTopBorder(),
                                     ptPaperWidth() - ptLeftBorder() - ptRightBorder(), 20 );
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.append( fs );
    }

    if ( !_first_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "First Page Footer" ) );
        fs->setFrameSetInfo( KWFrameSet::FI_FIRST_FOOTER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptPaperHeight() -
                                     ptTopBorder() - 20, ptPaperWidth() - ptLeftBorder() -
                                     ptRightBorder(), 20 );
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.append( fs );
    }

    if ( !_odd_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Odd Pages Footer" ) );
        fs->setFrameSetInfo( KWFrameSet::FI_ODD_FOOTER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptPaperHeight() -
                                     ptTopBorder() - 20, ptPaperWidth() - ptLeftBorder() -
                                     ptRightBorder(), 20 );
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.append( fs );
    }

    if ( !_even_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Even Pages Footer" ) );
        fs->setFrameSetInfo( KWFrameSet::FI_EVEN_FOOTER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptPaperHeight() -
                                     ptTopBorder() - 20, ptPaperWidth() - ptLeftBorder() -
                                     ptRightBorder(), 20 );
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.append( fs );
    }

    // do some sanity checking on document.
    for (int i = numFrameSets()-1; i>-1; i--) {
        KWFrameSet *fs = frameSet(i);
        if(!fs) {
            kdWarning() << "frameset " << i << " is NULL!!" << endl;
            m_lstFrameSet.remove(i);
        } else if( fs->type()==FT_TABLE) {
            static_cast<KWTableFrameSet *>( fs )->validate();
        } else if(!fs->getNumFrames()) {
            kdWarning () << "frameset " << i << " " << fs->getName() << " has no frames" << endl;
            removeFrameSet(fs);
            if ( fs->type() == FT_PART )
                delete static_cast<KWPartFrameSet *>(fs)->getChild();
            delete fs;
        } else if (fs->type() == FT_TEXT) {
            for (int f=fs->getNumFrames()-1; f>=0; f--) {
                KWFrame *frame = fs->frame(f);
                if(frame->height() < s_minFrameHeight) {
                    kdWarning() << fs->getName() << " frame " << f << " height is so small no text will fit, adjusting (was: "
                                << frame->height() << " is: " << s_minFrameHeight << ")" << endl;
                    frame->setHeight(s_minFrameHeight);
                }
                if(frame->width() < s_minFrameWidth) {
                    kdWarning() << fs->getName() << " frame " << f << " width is so small no text will fit, adjusting (was: "
                                << frame->width() << " is: " << s_minFrameWidth  << ")" << endl;
                    frame->setWidth(s_minFrameWidth);
                }
            }
        }
    }

    // Renumber footnotes
    KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet *>( m_lstFrameSet.getFirst() );
    if ( frameset  )
        frameset->renumberFootNotes( false /*no repaint*/ );

    emit sigProgress(-1);

    //kdDebug(32001) << "KWDocument::loadXML done" << endl;

    setModified( false );

    // Connect to notifications from main text-frameset
    if ( frameset ) {
        connect( frameset->textObject(), SIGNAL( chapterParagraphFormatted( KoTextParag * ) ),
                 SLOT( slotChapterParagraphFormatted( KoTextParag * ) ) );
        connect( frameset, SIGNAL( mainTextHeightChanged() ),
                 SIGNAL( mainTextHeightChanged() ) );
    }

}

void KWDocument::startBackgroundSpellCheck()
{
    //don't start bg spell checking if
    if(backgroundSpellCheckEnabled() && isReadWrite())
    {
#ifdef HAVE_LIBKSPELL2
        m_bgSpellCheck->start();
#endif
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
        emit sig_insertObject( ch, fs );
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
                    f->getName() == tableName ) {
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
    if ( _store ) {
        m_pictureCollection->readFromStore( _store, m_pictureMap );
        m_pictureMap.clear(); // Release memory
    }
}

bool KWDocument::completeLoading( KoStore *_store )
{
    loadImagesFromStore( _store );

    processPictureRequests();
    processAnchorRequests();
    processFootNoteRequests();

    // Save memory
    m_urlIntern = QString::null;

    // The fields from documentinfo.xml just got loaded -> update vars
    recalcVariables( VT_FIELD );

    // Finalize all the existing framesets
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

    // Delete loading info
    delete m_loadingInfo;
    m_loadingInfo = 0;

    return true;
}

void KWDocument::processPictureRequests()
{
    QPtrListIterator<KWTextImage> it2 ( m_textImageRequests );
    for ( ; it2.current() ; ++it2 )
    {
        it2.current()->setImage( *m_pictureCollection );
    }
    m_textImageRequests.clear();

    kdDebug() << k_funcinfo << m_pictureRequests.count() << " picture requests." << endl;
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
                frame->setZOrder( maxZOrder( frame->pageNum(this) ) + 1 +nb ); // make sure it's on top
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
    if ( !store->open( "content.xml" ) )
        return false;

    m_pictureCollection->assignUniqueIds();

    manifestWriter->addManifestEntry( "content.xml", "text/xml" );
    KoStoreDevice contentDev( store );
    KoXmlWriter contentWriter( &contentDev, "office:document-content" );

    m_varColl->variableSetting()->setModificationDate(QDateTime::currentDateTime());
    recalcVariables( VT_DATE );
    recalcVariables( VT_TIME ); // for "current time"
    m_syntaxVersion = CURRENT_SYNTAX_VERSION; // todo clean this up

    KoGenStyles mainStyles;
    KoSavingContext savingContext( mainStyles );

    // Save user styles as KoGenStyle objects
    KoSavingContext::StyleNameMap map = m_styleColl->saveOasis( mainStyles, KoGenStyle::STYLE_USER );
    savingContext.setStyleNameMap( map );

    KTempFile contentTmpFile;
    contentTmpFile.setAutoDelete( true );
    QFile* tmpFile = contentTmpFile.file();
    KoXmlWriter contentTmpWriter( tmpFile, 1 );
    contentTmpWriter.startElement( "office:body" );
    contentTmpWriter.startElement( "office:text" );

    saveOasisBody( contentTmpWriter, savingContext );
    // TODO save the content into contentTmpWriter!
    // TODO save embedded objects

    contentTmpWriter.endElement(); // office:text
    contentTmpWriter.endElement(); // office:body

    // Done with writing out the contents to the tempfile, we can now write out the automatic styles
    contentWriter.startElement( "office:automatic-styles" );
    QValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( KoGenStyle::STYLE_AUTO );
    QValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }
    styles = mainStyles.styles( KoGenStyle::STYLE_LIST );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "text:list-style", (*it).name, 0 );
    }

    styles = mainStyles.styles( KWDocument::STYLE_FRAME );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name , "style:graphic-properties"  );
    }

    contentWriter.endElement(); // office:automatic-styles


    // And now we can copy over the contents from the tempfile to the real one
    tmpFile->close();
    contentWriter.addCompleteElement( tmpFile );
    contentTmpFile.close();
    contentWriter.endElement(); // document-content
    contentWriter.endDocument();

    if ( !store->close() ) // done with content.xml
        return false;

    KoGenStyle pageLayout = m_pageLayout.saveOasis();
    pageLayout.addAttribute( "style:page-usage", "all" ); // needed?
    // TODO (from variablesettings I guess) pageLayout.addAttribute( "style:first-page-number", ... );
    /*QString pageLayoutStyleName =*/ mainStyles.lookup( pageLayout, "pm" );

    if ( !store->open( "styles.xml" ) )
        return false;
    manifestWriter->addManifestEntry( "styles.xml", "text/xml" );
    saveOasisDocumentStyles( store, mainStyles );
    if ( !store->close() ) // done with styles.xml
        return false;

    m_pictureCollection->saveOasisToStore( store, savePictureList(), manifestWriter);

    if(!store->open("settings.xml"))
        return false;

    KoXmlWriter settingsWriter(&contentDev, "office:document-settings");
    settingsWriter.startElement("office:settings");
    settingsWriter.startElement("config:config-item-set");

    settingsWriter.addAttribute("config:name", "view-settings");

    settingsWriter.startElement("config:config-item-map-entry" );
    KoUnit::saveOasis(&settingsWriter, m_unit);

    saveOasisSettings( settingsWriter );

    settingsWriter.endElement();

    settingsWriter.endElement(); // config:config-item-set
    settingsWriter.endElement(); // office:settings
    settingsWriter.endElement(); // Root element
    settingsWriter.endDocument();

    if(!store->close())
        return false;

    manifestWriter->addManifestEntry("settings.xml", "text/xml");

    return true;
}

void KWDocument::saveOasisSettings( KoXmlWriter &/*settingsWriter*/ )
{
    //todo implement it.
}

void KWDocument::saveOasisDocumentStyles( KoStore* store, KoGenStyles& mainStyles ) const
{
    QString pageLayoutName;
    KoStoreDevice stylesDev( store );
    KoXmlWriter stylesWriter( &stylesDev, "office:document-styles" );

    stylesWriter.startElement( "office:styles" );
    QValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( KoGenStyle::STYLE_USER );
    QValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &stylesWriter, mainStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }
    m_styleColl->saveOasisOutlineStyles( stylesWriter );
    stylesWriter.endElement(); // office:styles

    stylesWriter.startElement( "office:automatic-styles" );

    styles = mainStyles.styles( KoGenStyle::STYLE_PAGELAYOUT );
    Q_ASSERT( styles.count() == 1 );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &stylesWriter, mainStyles, "style:page-layout", (*it).name, "style:page-layout-properties", false /*don't close*/ );
        //if ( m_pageLayout.columns > 1 ) TODO add columns element. This is a bit of a hack,
        // which only works as long as we have only one page master
        stylesWriter.endElement();
        Q_ASSERT( pageLayoutName.isEmpty() ); // if there's more than one pagemaster we need to rethink all this
        pageLayoutName = (*it).name;
    }



    stylesWriter.endElement(); // office:automatic-styles

    stylesWriter.startElement( "office:master-styles" );
    stylesWriter.startElement( "style:master-page" );
    stylesWriter.addAttribute( "style:name", "Standard" );
    stylesWriter.addAttribute( "style:page-layout-name", pageLayoutName );
    stylesWriter.endElement();
    stylesWriter.endElement(); // office:master-styles

    stylesWriter.endElement(); // root element (office:document-styles)
    stylesWriter.endDocument();
}

void KWDocument::saveOasisBody( KoXmlWriter& writer, KoSavingContext& context ) const
{
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
            if ( fit.current()->isVisible() && // HACK to avoid saving headers/footers for now
                 !fit.current()->isFloating() )
                fit.current()->saveOasis( writer, context );
        }

    } else {
        // TODO write the contents page-by-page
    }
}

QDomDocument KWDocument::saveXML()
{
    m_varColl->variableSetting()->setModificationDate(QDateTime::currentDateTime());
    recalcVariables( VT_DATE );
    recalcVariables( VT_TIME ); // for "current time"
    QDomDocument doc = createDomDocument( "DOC", CURRENT_DTD_VERSION );
    QDomElement kwdoc = doc.documentElement();
    kwdoc.setAttribute( "editor", "KWord" );
    kwdoc.setAttribute( "mime", "application/x-kword" );
    m_syntaxVersion = CURRENT_SYNTAX_VERSION;
    kwdoc.setAttribute( "syntaxVersion", m_syntaxVersion );

    QDomElement paper = doc.createElement( "PAPER" );
    kwdoc.appendChild( paper );
    paper.setAttribute( "format", static_cast<int>( m_pageLayout.format ) );
    paper.setAttribute( "pages", m_pages );
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
    docattrs.setAttribute( "unit", KoUnit::unitName(getUnit()) );
    docattrs.setAttribute( "hasTOC", static_cast<int>(m_hasTOC));
    docattrs.setAttribute( "tabStopValue", m_tabStop );

    // Save visual info for the first view, such as active table and active cell
    // It looks like a hack, but reopening a document creates only one view anyway (David)
    KWView * view = static_cast<KWView*>(views().getFirst());
    if ( view ) // no view if embedded document
    {
        KWFrameSetEdit* edit = view->getGUI()->canvasWidget()->currentFrameSetEdit();
        if ( edit )
        {
            docattrs.setAttribute( "activeFrameset", edit->frameSet()->getName() );
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
                bookElem.setAttribute( "frameset", book.current()->frameSet()->getName());
                bookElem.setAttribute( "startparag", book.current()->startParag()->paragId());
                bookElem.setAttribute( "endparag", book.current()->endParag()->paragId());

                bookElem.setAttribute( "cursorIndexStart", book.current()->bookmarkStartIndex());
                bookElem.setAttribute( "cursorIndexEnd", book.current()->bookmarkEndIndex());

            }
        }
    }
    getVariableCollection()->variableSetting()->save(kwdoc );

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

    if( !m_spellListIgnoreAll.isEmpty() )
    {
        QDomElement spellCheckIgnore = doc.createElement( "SPELLCHECKIGNORELIST" );
        kwdoc.appendChild( spellCheckIgnore );
        for ( QStringList::Iterator it = m_spellListIgnoreAll.begin(); it != m_spellListIgnoreAll.end(); ++it )
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

void KWDocument::addView( KoView *_view )
{
    m_lstViews.append( (KWView*)_view );
    KoDocument::addView( _view );
    QPtrListIterator<KWView> it( m_lstViews );
    for ( ; it.current() ; ++it )
        it.current()->deselectAllFrames();
}

void KWDocument::removeView( KoView *_view )
{
    m_lstViews.setAutoDelete( FALSE );
    m_lstViews.removeRef( static_cast<KWView*>(_view) );
    m_lstViews.setAutoDelete( TRUE );
    KoDocument::removeView( _view );
}

void KWDocument::addShell( KoMainWindow *shell )
{
    connect( shell, SIGNAL( documentSaved() ), m_commandHistory, SLOT( documentSaved() ) );
    connect( shell, SIGNAL( saveDialogShown(bool) ), this, SLOT( saveDialogShown(bool) ) );
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
    for ( KWView * viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
        viewPtr->getGUI()->canvasWidget()->setUpdatesEnabled( false );
    }
    Q_ASSERT( !m_bGeneratingPreview );
    m_bGeneratingPreview = true;
    QPixmap pix = KoDocument::generatePreview(size);

    // Restore everything as it was before
    setResolution( oldResolutionX, oldResolutionY );
    setZoom( oldZoom );
    newZoomAndResolution( false, false );

    for ( KWView * viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
        viewPtr->getGUI()->canvasWidget()->setUpdatesEnabled( true );
    }
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

void KWDocument::insertObject( const KoRect& rect, KoDocumentEntry& _e )
{
    KoDocument* doc = _e.createDoc( this );
    if ( !doc )
        return;
    if ( !doc->initDoc(KoDocument::InitDocEmbedded) )
        return;

    KWChild* ch = new KWChild( this, rect.toQRect(), doc );

    insertChild( ch );
    setModified( TRUE );

    KWPartFrameSet *frameset = new KWPartFrameSet( this, ch, QString::null );
    KWFrame *frame = new KWFrame(frameset, rect.x(), rect.y(), rect.width(), rect.height() );
    frame->setZOrder( maxZOrder( frame->pageNum(this) ) + 1 ); // make sure it's on top
    frameset->addFrame( frame );
    addFrameSet( frameset );
    frameset->updateChildGeometry( viewMode() ); // set initial coordinates of 'ch' correctly

    KWCreateFrameCommand *cmd = new KWCreateFrameCommand( i18n("Create Part Frame"), frame);
    addCommand(cmd);

    emit sig_insertObject( ch, frameset );

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
    for ( KWView * viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
        if ( viewPtr != _view /*&& viewPtr->getGUI() && viewPtr->getGUI()->canvasWidget()*/ ) {
            viewPtr->getGUI()->canvasWidget()->repaintAll( erase );
        }
    }
}

void KWDocument::setUnit( KoUnit::Unit _unit )
{
    m_unit = _unit;
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
        if ( viewPtr->getGUI() ) {
            viewPtr->getGUI()->getHorzRuler()->setUnit( m_unit );
            viewPtr->getGUI()->getVertRuler()->setUnit( m_unit );
        }
    }
}

void KWDocument::updateAllStyleLists()
{
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->updateStyleList();
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
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->updateFrameStyleList();
}

void KWDocument::updateAllTableStyleLists()
{
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->updateTableStyleList();
}

void KWDocument::repaintAllViews( bool erase )
{
    //kdDebug(32001) << "KWDocument::repaintAllViews" << endl;
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->getGUI()->canvasWidget()->repaintAll( erase );
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
        kdDebug(32002) << "KWDocument::framesToCopyOnNewPage looking at frame " << frame << ", pageNum=" << frame->pageNum() << " from " << frameSet->getName() << endl;
        static const char * newFrameBh[] = { "Reconnect", "NoFollowup", "Copy" };
        kdDebug(32002) << "   frame->newFrameBehavior()==" << newFrameBh[frame->newFrameBehavior()] << endl;
#endif
        if ( (frame->pageNum() == afterPageNum ||
              (frame->pageNum() == afterPageNum -1 && frame->sheetSide() != KWFrame::AnySide) )
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

void KWDocument::insertPage( int afterPageNum ) // can be -1 for 'before page 0'
{
#ifdef DEBUG_PAGES
    kdDebug(32002) << "insertPage: afterPageNum=" << afterPageNum << endl;
#endif
    if ( processingType() == WP )
        Q_ASSERT( afterPageNum == m_pages-1 ); // WP mode: can only append.

    // If not appending, move down everything after 'afterPageNum', to make room.
    for ( int pg = m_pages-1 ; pg > afterPageNum ; --pg )
    {
        // pg is the 'src' page. Its contents must be moved to the page pg+1
        QPtrList<KWFrame> frames = framesInPage( pg, false );
#ifdef DEBUG_PAGES
        kdDebug(32002) << "insertPage: moving " << frames.count() << " frames down, from page " << pg << endl;
#endif
        QPtrListIterator<KWFrame> frameIt( frames );
        for ( ; frameIt.current(); ++frameIt )
            frameIt.current()->moveBy( 0, ptPaperHeight() );
    }

    m_pages++;

    // Fill in the new page
    QPtrList<KWFrame> framesToCopy = framesToCopyOnNewPage( afterPageNum );
    QPtrListIterator<KWFrame> frameIt( framesToCopy );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame * frame = frameIt.current();

        KWFrame *newFrame = frame->getCopy();
        newFrame->moveBy( 0, ptPaperHeight() );
        //newFrame->setPageNum( frame->pageNum()+1 );
        frame->frameSet()->addFrame( newFrame );

        if ( frame->newFrameBehavior()==KWFrame::Copy )
            newFrame->setCopy( true );
        //kdDebug(32002) << "   => created frame " << newFrame << " " << *newFrame << endl;
    }
}

int KWDocument::appendPage()
{
#ifdef DEBUG_PAGES
    kdDebug(32002) << "KWDocument::appendPage m_pages=" << m_pages << " -> insertPage(" << m_pages-1 << ")" << endl;
#endif
    insertPage( m_pages - 1 );
    return m_pages - 1; // Note that insertPage changes m_pages!
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
        Q_ASSERT( pageNum == m_pages-1 ); // WP mode: can only remove last page.
    Q_ASSERT( m_pages > 1 );
    if ( m_pages == 1 )
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
    for ( int pg = pageNum+1 ; pg < m_pages ; ++pg )
    {
        // pg is the 'src' page. Its contents must be moved to the page pg-1
        QPtrList<KWFrame> frames = framesInPage( pg, false );
#ifdef DEBUG_PAGES
        kdDebug(32002) << "removePage: moving " << frames.count() << " frames up, from page " << pg << endl;
#endif
        QPtrListIterator<KWFrame> frameIt( frames );
        for ( ; frameIt.current(); ++frameIt )
            frameIt.current()->moveBy( 0, -ptPaperHeight() );
    }

    m_pages--;
#ifdef DEBUG_PAGES
    kdDebug(32002) << "KWDocument::removePage -- -> " << m_pages << endl;
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
}

void KWDocument::tryRemovingPages()
{
    int lastPage = numPages() - 1;
    bool removed = false;
    // Last frame is empty -> try removing last page, and more if necessary
    while ( lastPage > 0 && canRemovePage( lastPage ) )
    {
        removePage( lastPage ); // this modifies m_pages
        if ( lastPage <= m_pages - 1 )
        {
            kdWarning() << "Didn't manage to remove page " << lastPage << " (still having " << m_pages << " pages ). Aborting" << endl;
            break;
        }
        removed = true;
        lastPage = m_pages - 1;
    }
    // Do all the recalc in one go. Speeds up deleting many pages.
    if ( removed )
        afterRemovePages();
}


KWFrameSet * KWDocument::frameSetByName( const QString & name )
{
    // Note: this isn't recursive, so it won't find table cells.
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        if ( fit.current()->getName() == name )
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
    int page = QMIN(m_pages-1, static_cast<int>(docPoint.y() / ptPaperHeight()));
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
    } else {
	QPtrList<KWFrame> frames = frame->framesBelow();
	for (KWFrame *f = frames.last(); f;f=frames.prev()) {
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
    int page = QMIN(m_pages-1, static_cast<int>(docPoint.y() / ptPaperHeight()));
    QPtrList<KWFrame> frames = framesInPage(page);


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
    case MEANING_MOUSE_MOVE:
        return Qt::sizeAllCursor;
    case MEANING_MOUSE_SELECT:
        return KCursor::handCursor();
    case MEANING_ACTIVATE_PART:
        return KCursor::handCursor();
    case MEANING_TOPLEFT:
    case MEANING_BOTTOMRIGHT:
        if ( frameSet->isProtectSize() )
            return Qt::forbiddenCursor;
        return Qt::sizeFDiagCursor;
    case MEANING_LEFT:
    case MEANING_RIGHT:
        if ( frameSet->isProtectSize() )
            return Qt::forbiddenCursor;
        return Qt::sizeHorCursor;
    case MEANING_BOTTOMLEFT:
    case MEANING_TOPRIGHT:
        if ( frameSet->isProtectSize() )
            return Qt::forbiddenCursor;
        return Qt::sizeBDiagCursor;
    case MEANING_TOP:
    case MEANING_BOTTOM:
        if ( frameSet->isProtectSize() )
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

    if ( frameclosetomouseright && frameclosetomouseright->frameSet()->getGroupManager() ) {
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
        if ( frameclosetomouse->frameSet()->getGroupManager() && (!frameundermouse || !frameundermouse->frameSet()->getGroupManager()) ) {
            // there is a frame, it is a table, and the cursor is NOT on a table ATM
            if (ppTable)
                *ppTable =frameclosetomouse->frameSet()->getGroupManager();
            // place the cursor to say that we can select row/columns
            return result;
        }
    }
    return TABLE_POSITION_NONE;
}


// TODO pass viewmode for isVisible
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
    bool fixed_something = false;
    for (int pgnum = 0 ; pgnum < m_pages ; pgnum++) {
        QPtrList<KWFrame> frames= framesInPage(pgnum,false);
        // scan this page to see if we need to fixup.
        bool need_fixup=true;
        for (KWFrame *f = frames.last();f;f=frames.prev()) {
            if (f->zOrder() != 0) { // assumption: old documents come with no zorder=>initialised to 0
                need_fixup=false;
                break;
            }
        }
        if (need_fixup) {
            int current_zorder=0;
            kdDebug() << "fixing page " << pgnum << " z-orders " << endl;
            for (KWFrame *fr = frames.first();fr;fr=frames.next()) {
                // only consider non-inline framesets.
                if (fr->frameSet()->isFloating())
                    continue;
                current_zorder++;
                fr->setZOrder(current_zorder);
                fixed_something = true;
            }
        }

        if ( m_processingType == KWDocument::WP )
        {
            // In all cases, ensure the main frames are below the rest.
            // (This could not be the case after e.g. an import filter does it wrong)
            lowerMainFrames( pgnum );
        }
    }
    if ( fixed_something )
        updateFramesOnTopOrBelow();
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
QPtrList<KWFrame> KWDocument::framesInPage( int pageNum, bool sorted ) const {
    KWFrameList frames;
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
        for ( unsigned int j = 0; j < frameSet->getNumFrames(); j++ ) {
            if ( !frameSet->isVisible() || frameSet->isRemoveableHeader() )
                continue;
            if ( frameSet->frame( j )->isSelected() )
                return frameSet->frame( j );
        }
    }
    return 0L;
}

void KWDocument::updateAllFrames()
{
#ifdef DEBUG_SPEED
    QTime dt;
    dt.start();
#endif
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->updateFrames();

#ifdef DEBUG_SPEED
    kdDebug(32001) << "updateAllFrames took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;
#endif

    // TODO: check all calls to updateAllFrames, and fix them.
    // E.g., if one frame moved, updateAllFrames isn't necessary,
    // only fs->updateFrames() and doc->updateFramesOnTopOrBelow() are necessary.

    // Update frames ontop and below _afterwards_,
    // it needs the 'frames in page' array (in other framesets)
    updateFramesOnTopOrBelow();
}

void KWDocument::updateFramesOnTopOrBelow( int _pageNum /* -1 == all */ )
{
    if ( viewMode() && !viewMode()->hasFrames() )
        return;

#ifdef DEBUG_SPEED
    kdDebug() << "KWDocument::updateFramesOnTopOrBelow  pageNum=" << _pageNum << endl;
    QTime dt;
    dt.start();
    int numberAdded = 0;
#endif

    // Look at all pages if _pageNum == -1, otherwise look at _pageNum only.
    int fromPage = _pageNum == -1 ? 0 : _pageNum;
    int toPage = _pageNum == -1 ? m_pages - 1 : _pageNum;
    for ( int pageNum = fromPage ; pageNum <= toPage ; ++pageNum )
    {
        // For all frames in that page: clear ontop/below lists.
        // TODO we need to fix the case of multipage frames... somehow.
        QPtrList<KWFrame> framesInThisPage = framesInPage( pageNum );
        QPtrListIterator<KWFrame> frameIt( framesInThisPage );
        for ( ; frameIt.current(); ++frameIt )
        {
            frameIt.current()->clearFramesOnTop();
            frameIt.current()->clearFramesBelow();
        }

        frameIt.toFirst();
        for ( ; frameIt.current(); ++frameIt )
        {
            // currentFrame is the frame we're taking care of now
            // (the one whose ontop/below caches we're modifying)
            KWFrame* currentFrame = frameIt.current();
            KWFrameSet* currentFrameSet = currentFrame->frameSet();
            KWTableFrameSet* table = currentFrameSet->getGroupManager();
            bool isInline = currentFrameSet->isFloating();

            // Frank's code for going up to the right frame/frameset, if currentFrame is
            // floating, in order to use the right z order.
            // ### Maybe this logic could be in KWFrame::zOrder() ?
            // ### or at least we could have a 'first non-floating parent frame' method
            KWFrame *parentFrame = currentFrame;
            KWFrameSet *parentFrameset = currentFrameSet;
            while (parentFrameset->isFloating()) {
                parentFrameset=parentFrameset->anchorFrameset();
                KWFrame *oldParentFrame = parentFrame;
                parentFrame=parentFrameset->frameAtPos(parentFrame->x(), parentFrame->y());
                if(!parentFrame)
                    parentFrame = oldParentFrame;
            }

            // We now look at all other frames (in the same page)
            // to check for intersections. This is o(n^2), but with n small.
            QPtrListIterator<KWFrame> it( framesInThisPage );
            for ( ; it.current() ; ++it )
            {
                KWFrame* frameMaybeOnTop = it.current();
                if ( currentFrame == frameMaybeOnTop ) // Skip identity case ;)
                    continue;
                KWFrameSet* frameSet = frameMaybeOnTop->frameSet();

                // Skip all cells from 'currentFrameSet' if 'currentFrameSet' is a table.
                // We trust that KWTableFrameSet will not make cells overlap ;)
                if ( table && frameSet->getGroupManager() == table )
                    continue;
                // Skip all frames from the parent frameset, if 'currentFrameSet' is inline
                // ## might need a for loop for the case of inline-inside-inline,
                // or maybe calling isPaintedBy instead [depending on what should happen for tables]
                if ( isInline && frameSet == parentFrameset )
                    continue;
                // Floating frames are not "on top", they are "inside".
                // They are not "below" anything either - the parent frameset is.
                if ( frameSet->isFloating() )
                    continue;

                //kdDebug(32001) << "        comparing our frame " << parentFrame << " (z:" << parentFrame->zOrder() << ") with frame " << frameMaybeOnTop << " (z:" << frameMaybeOnTop->zOrder() << ") from frameset " << frameSet << endl;
                KoRect intersect = currentFrame->intersect( frameMaybeOnTop->outerKoRect() );
                if( !intersect.isEmpty() )
                {
                    bool added = false;
                    if ( parentFrame->zOrder() < frameMaybeOnTop->zOrder() )
                    {
                        // Floating frames are not "on top", they are "inside".
                        if ( !frameSet->isPaintedBy( currentFrameSet ) ) {
                            added = true;
                            currentFrame->addFrameOnTop( frameMaybeOnTop );
                        }
                    } else
                    {
                        // Don't treat a frameset as 'below' its inline framesets.
                        // Same problem with table cells. In general we want to forbid that, if
                        // painting A leads to painting B, A is stored as 'below B'.
                        // This is where the infinite loop comes from, if B is transparent.
                        // (Note: we only forbid this for 'below', not for 'on top', to get
                        // proper clipping).
                        if ( !currentFrameSet->isPaintedBy( frameSet ) && parentFrame->zOrder() > frameMaybeOnTop->zOrder() )
                        {
                            added = true;
                            currentFrame->addFrameBelow( frameMaybeOnTop );
                        }
                    }
#ifdef DEBUG_SPEED
                    if ( added )
                        numberAdded++;
#endif

#if 0
                    if ( added )
                    {
                        kdDebug(32002)
                            << "          adding frame "
                            << frameMaybeOnTop << "("<<frameSet->getName()<<")"
                            << " (zorder: " << frameMaybeOnTop->zOrder() << ")"
                            << (  parentFrame->zOrder() < frameMaybeOnTop->zOrder() ? " on top of" : " below" )
                            << " frame " << currentFrame << "("<<currentFrameSet->getName()<<")"
                            << " parentFrame " << parentFrame << " (zorder: " << parentFrame->zOrder() << ")" << endl;
                            //kdDebug(32002) << "   intersect: " << intersect
                            //<< " (zoomed: " << zoomRect( intersect ) << ")" << endl;
                    }
#endif
                }
            } // 'it' for loop
        } // 'frameIt' for loop

        frameIt.toFirst();
        for ( ; frameIt.current(); ++frameIt )
            frameIt.current()->sortFramesBelow();

    } // for (pages)

#ifdef DEBUG_SPEED
    kdDebug(32001) << "updateFramesOnTopOrBelow("<<_pageNum<<") took " << (float)(dt.elapsed()) / 1000 << " seconds, added " << numberAdded << " frames" << endl;
#endif
}

// Tell this method when a frame is moved / resized / created / deleted
// and everything will be update / repainted accordingly
void KWDocument::frameChanged( KWFrame * frame, KWView * view )
{
    if ( !frame ) // TODO call another method for 'deleted frame', which passes the frameset
        updateAllFrames(); // ... in order to get rid of that call, and use the 'else' case instead
    else {
        frame->frameSet()->updateFrames();
        updateFramesOnTopOrBelow();
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
    QPtrListIterator<KWView> it( m_lstViews );
    for ( ; it.current() ; ++it )
    {
        it.current()->updateHeaderFooterButton();
        it.current()->updateHeader();
    }
}

void KWDocument::updateFooterButton()
{
    QPtrListIterator<KWView> it( m_lstViews );
    for ( ; it.current() ; ++it )
    {
        it.current()->updateHeaderFooterButton();
        it.current()->updateFooter();
    }
}


bool KWDocument::isOnlyOneFrameSelected() {
    return getSelectedFrames().count()==1;
}

void KWDocument::setFramePadding( double l, double r, double t, double b )
{
    // todo, make this more OO, and update the tableheaders as well..
    for ( unsigned int i = 0; i < numFrameSets(); i++ ) {
        if ( frameSet( i )->hasSelectedFrame() ) {
            KWFrameSet *frameset = frameSet( i );
            for ( unsigned int j = 0; j < frameset->getNumFrames(); j++ ) {
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

int KWDocument::getMailMergeRecord() const
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
        kdWarning(32001) << "Frameset " << f << " " << f->getName() << " already in list!" << endl;
        return;
    }
    m_lstFrameSet.append(f);
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

int KWDocument::getPageOfRect( KoRect & _rect ) const
{
    int page = static_cast<int>(_rect.y() / ptPaperHeight());
    return QMIN( page, m_pages-1 );
}

// Return true if @p r is out of the page @p page
bool KWDocument::isOutOfPage( KoRect & r, int page ) const
{
    return r.x() < 0 ||
        r.right() > ptPaperWidth() ||
        r.y() < page * ptPaperHeight() ||
        r.bottom() > ( page + 1 ) * ptPaperHeight();
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
    kdDebug() << "size: x:" << ptLeftBorder()<< ", y:"<<ptTopBorder() << ", w:"<< ptPaperWidth() << ", h:"<<ptPaperHeight()<<endl;
    kdDebug() << "Header visible: " << isHeaderVisible() << endl;
    kdDebug() << "Footer visible: " << isFooterVisible() << endl;
    kdDebug() << "Units: " << getUnit() <<endl;
    kdDebug() << "# Framesets: " << numFrameSets() <<endl;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( unsigned int iFrameset = 0; fit.current() ; ++fit, iFrameset++ )
    {
        KWFrameSet * frameset = fit.current();
        kdDebug() << "Frameset " << iFrameset << ": '" <<
            frameset->getName() << "' (" << frameset << ")" << (frameset->isDeleted()?" Deleted":"")<<endl;
        if ( frameset->isVisible())
            frameset->printDebug();
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

KFormula::Document* KWDocument::getFormulaDocument()
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
    QPtrListIterator<KWView> it( m_lstViews );
    for ( ; it.current() ; ++it )
        it.current()->getGUI()->canvasWidget()->repaintChanged( frameset, it.atLast() );
}

void KWDocument::refreshFrameBorderButton()
{

    KWFrame *frame= getFirstSelectedFrame();
    if (frame)
    {
        QPtrListIterator<KWView> it( m_lstViews );
        frame = KWFrameSet::settingsFrame(frame);
        for ( ; it.current() ; ++it )
        {
            it.current()->showFrameBorders( frame->leftBorder(), frame->rightBorder(), frame->topBorder(), frame->bottomBorder() );
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
        if ( fs->getGroupManager() )
        {
            KWTableFrameSet *table=fs->getGroupManager();
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
   QPtrListIterator<KWView> it( m_lstViews );
   for ( ; it.current() ; ++it )
       it.current()->getGUI()->reorganize();
}

void KWDocument::slotDocumentInfoModifed()
{
    if (!getVariableCollection()->variableSetting()->displayFieldCode())
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

QColor KWDocument::resolveBgColor( const QColor & col, QPainter * painter )
{
    if (col.isValid())
        return col;

    return defaultBgColor( painter );
}

QColor KWDocument::defaultBgColor( QPainter * painter )
{
    if ( painter->device()->devType() == QInternal::Printer )
        return Qt::white;
    return QApplication::palette().color( QPalette::Active, QColorGroup::Base );
}


void KWDocument::renameButtonTOC(bool b)
{
    m_hasTOC=b;
    QPtrListIterator<KWView> it( m_lstViews );
    for ( ; it.current() ; ++it )
    {
        it.current()->renameButtonTOC(b);
    }
}

void KWDocument::refreshMenuExpression()
{
    QPtrListIterator<KWView> it( m_lstViews );
    for ( ; it.current() ; ++it )
        it.current()->refreshMenuExpression();
}

void KWDocument::frameSelectedChanged()
{
    emit sig_frameSelectedChanged();
}

void KWDocument::updateZoomRuler()
{
    QPtrListIterator<KWView> it( m_lstViews );
    for ( ; it.current() ; ++it )
    {
        it.current()->getGUI()->getHorzRuler()->setZoom( zoomedResolutionX() );
        it.current()->getGUI()->getVertRuler()->setZoom( zoomedResolutionY() );
        it.current()->slotUpdateRuler();
    }
}

void KWDocument::updateRulerFrameStartEnd()
{
    QPtrListIterator<KWView> it( m_lstViews );
    for ( ; it.current() ; ++it )
        it.current()->slotUpdateRuler();
}

void KWDocument::updateFrameStatusBarItem()
{
    QPtrListIterator<KWView> it( m_lstViews );
    for ( ; it.current() ; ++it )
        it.current()->updateFrameStatusBarItem();
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
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->getGUI()->getHorzRuler()->setGridSize(_gridx);
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
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->initGUIButton();
}

void KWDocument::enableBackgroundSpellCheck( bool b )
{
#ifdef HAVE_LIBKSPELL2
    m_bgSpellCheck->setEnabled(b);
#endif
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->updateBgSpellCheckingState();
}

bool KWDocument::backgroundSpellCheckEnabled() const
{
#ifdef HAVE_LIBKSPELL2
    return m_bgSpellCheck->enabled();
#else
    return false;
#endif
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
        m_sectionTitles.resize( frame->pageNum() );
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
            int pgNum = frame->pageNum();
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

void KWDocument::addIgnoreWordAll( const QString & word)
{
    if( m_spellListIgnoreAll.findIndex( word )==-1)
        m_spellListIgnoreAll.append( word );
    //m_bgSpellCheck->addIgnoreWordAll( word );

}

void KWDocument::clearIgnoreWordAll( )
{
    m_spellListIgnoreAll.clear();
    //m_bgSpellCheck->clearIgnoreWordAll();
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
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->slotFrameSetEditChanged();

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
    //but it's necessary for the futur when kword will support
    //different view mode in different view.
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->getGUI()->canvasWidget()->switchViewMode( m_viewMode );

    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->switchModeView();
    emit newContentsSize();
    updateResizeHandles();

    // Since the text layout depends on the view mode, we need to redo it
    // But after telling the canvas about the new viewmode, otherwise stuff like
    // slotNewContentsSize will crash.
    updateAllFrames();
    layout();

    repaintAllViews( true );
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->getGUI()->canvasWidget()->ensureCursorVisible();
}

void KWDocument::changeBgSpellCheckingState( bool b )
{
    enableBackgroundSpellCheck( b );
    reactivateBgSpellChecking();
    KConfig *config = KWFactory::global()->config();
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
        for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
            viewPtr->testAndCloseAllFrameSetProtectedContent();
    }
}

void KWDocument::updateRulerInProtectContentMode()
{
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->updateRulerInProtectContentMode();
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


void KWDocument::spellCheckParagraphDeleted( KoTextParag *_parag,  KWTextFrameSet *frm)
{
    //FIXME: do we need that?
    //m_bgSpellCheck->paragraphDeleted( _parag, frm->textObject());
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
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->updateDirectCursorButton();
}

void KWDocument::setInsertDirectCursor(bool _b)
{
    m_bInsertDirectCursor=_b;
    KConfig *config = KWFactory::global()->config();
    config->setGroup( "Interface" );
    config->writeEntry( "InsertDirectCursor", _b );
    updateDirectCursorButton();
}

void KWDocument::saveDialogShown(bool reset)
{
        if (reset)
        {
            resetURL();
            return;
        }
        else if (textFrameSet(0) )
        {
                QString first_row = textFrameSet(0)->textDocument()->text(0).left(50);
                bool truncate = false;
                QChar ch;
                for (int i=0;i<(signed int)first_row.length();i++)
                {
                        ch =  first_row.at(i);
                        if (!truncate)
                                if (ch.isPunct() || ch.isSpace() || ch == '.' )
                                {
                                        first_row.remove(i,1);
                                        --i;
                                }
                                else
                                        truncate = true;
                        else if ( truncate && (ch.isPunct() || ch == '.' ) )
                        {
                                first_row.truncate(i);
                                break;
                        }
                }
                first_row = first_row.stripWhiteSpace();
                setURL(first_row);
        }
}

#if 0 // KWORD_HORIZONTAL_LINE
void KWDocument::setHorizontalLinePath( const QStringList & lst)
{
    m_horizontalLinePath = lst;
}
#endif

void KWDocument::addWordToDictionary( const QString & /*word*/ )
{
#ifdef HAVE_LIBKSPELL2
    if ( m_bgSpellCheck )
    {
        //m_bgSpellCheck->addPersonalDictonary( word );
    }
#endif
}

void KWDocument::setEmpty()
{
    KoDocument::setEmpty();
    // Whether loaded from template or from empty doc: this is a new one -> set creation date
    m_varColl->variableSetting()->setCreationDate(QDateTime::currentDateTime());
}

#include "kwdoc.moc"
