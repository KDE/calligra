/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2002-2006 David Faure <faure@kde.org>
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
#include "KoTextBookmark.h"
#include "KWCanvas.h"
#include "KWCommand.h"
#include "KWFormulaFrameSet.h"
#include "KWFrameLayout.h"
#include "KWPictureFrameSet.h"
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
#include "KWFrameView.h"
#include "KWFrameViewManager.h"
#include "KWStartupWidget.h"

#include <KoPictureCollection.h>
#include <KoTemplateChooseDia.h>
#include <KoMainWindow.h>
#include <KoDocumentInfo.h>
#include <KoGlobal.h>
#include <KoParagCounter.h>
#include <KoTextObject.h>
#include <KoAutoFormat.h>
#include <KoVariable.h>
#include <kformuladocument.h>
#include <KoApplication.h>
#include <KoOasisContext.h>
#include <kcommand.h>
#include <KoGenStyles.h>
#include <KoStore.h>
#include <KoStoreDrag.h>
#include <KoStoreDevice.h>
#include <KoXmlWriter.h>
#include <KoOasisStore.h>
#include <KoOasisStyles.h>
#include <KoXmlNS.h>
#include <KoDom.h>

#include <kcursor.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kspell.h>
#include <kstandarddirs.h>

#include <kspell2/settings.h>

#include <qfileinfo.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qbuffer.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3PtrList>
#include <QPixmap>

#include <unistd.h>
#include <math.h>

//#define DEBUG_PAGES
//#define DEBUG_SPEED

// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
static const char * CURRENT_DTD_VERSION = "1.2";

/******************************************************************/
/* Class: KWCommandHistory                                        */
/******************************************************************/
class KWCommandHistory : public KCommandHistory
{
public:
    KWCommandHistory( KWDocument * doc ) : KCommandHistory( doc->actionCollection(),  true ), m_pDoc( doc ) {}
public /*slots*/: // They are already slots in the parent. Running moc on the inherited class shouldn't be necessary AFAICS.
    virtual void undo();
    virtual void redo();
private:
    KWDocument * m_pDoc;
};

void KWCommandHistory::undo()
{
    m_pDoc->clearUndoRedoInfos();
    KCommandHistory::undo();
}

void KWCommandHistory::redo()
{
    m_pDoc->clearUndoRedoInfos();
    KCommandHistory::redo();
}

/******************************************************************/
/* Class: KWDocument                                              */
/******************************************************************/
void KWDocument::clearUndoRedoInfos()
{
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
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

const int KWDocument::CURRENT_SYNTAX_VERSION = 3;

KWDocument::KWDocument(QWidget *parentWidget, const char *widname, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widname, parent, name, singleViewMode ),
      m_urlIntern()
{
    KWStatisticVariable::setExtendedType(  true );
    dcop = 0;
    m_framesChangedHandler = 0;
    m_pageManager = new KWPageManager();
    m_pageManager->appendPage();
    m_loadingInfo = 0L;
    m_tabStop = MM_TO_POINT( 15.0 );
    m_processingType = WP;

//    varFormats.setAutoDelete(true);
    m_lstFrameSet.setAutoDelete( true );
    // m_textImageRequests does not create or delete the KWTextImage classes
    m_textImageRequests.setAutoDelete(false);

    m_styleColl = new KoStyleCollection();
    m_frameStyleColl = new KWFrameStyleCollection();
    m_tableStyleColl = new KWTableStyleCollection();
    m_tableTemplateColl = new KWTableTemplateCollection();
    m_pictureCollection = new KoPictureCollection();

    m_personalExpressionPath = KWFactory::instance()->dirs()->resourceDirs("expression");

    m_bShowGrid = false;
    m_bSnapToGrid = false;


    setInstance( KWFactory::instance(), false );
    setTemplateType( "kword_template" );

    m_gridX = m_gridY = MM_TO_POINT(5.0 );
    m_indent = MM_TO_POINT( 10.0 );

    m_iNbPagePerRow = 4;
    m_maxRecentFiles = 10;
    m_bShowRuler = true;

    m_footNoteSeparatorLinePos=SLP_LEFT;

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
    m_viewModeType="ModeNormal";
    m_layoutViewMode = 0;

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
    m_bookmarkList = new KoTextBookmarkList;
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

    //kDebug() << "Default font: requested family: " << m_defaultFont.family() << endl;
    //kDebug() << "Default font: real family: " << QFontInfo(m_defaultFont).family() << endl;

    if ( name )
        dcopObject();
}

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
    delete m_layoutViewMode;
    delete m_bufPixmap;
    delete m_pictureCollection;
    delete m_pageManager;
    delete m_bookmarkList;
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
      setGridY(qMax( config->readDoubleNumEntry("GridY",MM_TO_POINT(5.0) ), 0.1));
      setGridX(qMax( config->readDoubleNumEntry("GridX",MM_TO_POINT(5.0) ), 0.1));
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
      m_zoomMode = static_cast<KoZoomMode::Mode> (
              config->readNumEntry( "ZoomMode", KoZoomMode::ZOOM_CONSTANT )
      );

      m_bShowDocStruct = config->readBoolEntry( "showDocStruct", true );
      m_viewModeType = config->readEntry( "viewmode", "ModeNormal" );
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
  {
      m_zoom = 100;
      m_zoomMode = KoZoomMode::ZOOM_WIDTH;
  }
  int undo=30;
  if(config->hasGroup("Misc" ) )
  {
      config->setGroup( "Misc" );
      undo=config->readNumEntry("UndoRedo",-1);

      //load default unit setting - this is only used for new files (from templates) or empty files
      if ( config->hasKey( "Units" ) )
          setUnit( KoUnit::unit( config->readEntry("Units") ) );
      m_defaultColumnSpacing = config->readDoubleNumEntry( "ColumnSpacing", 3.0 );
  }

  if(undo!=-1)
      setUndoRedoLimit(undo);

  setZoomAndResolution( m_zoom, KoGlobal::dpiX(), KoGlobal::dpiY() );

  //text mode view is not a good default for a readonly document...
  if ( !isReadWrite() && m_viewModeType =="ModeText" )
      m_viewModeType= "ModeNormal";

  m_layoutViewMode = KWViewMode::create( m_viewModeType, this, 0 /*no canvas*/);

  if(config->hasGroup("Kword Path" ) )
  {
      config->setGroup( "Kword Path" );
      if ( config->hasKey( "expression path" ) )
          m_personalExpressionPath = config->readPathListEntry( "expression path" );
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
        config->writeEntry( "ZoomMode", (int)m_zoomMode );
        config->writeEntry( "showDocStruct", m_bShowDocStruct );
        config->writeEntry( "Rulers", m_bShowRuler );
        config->writeEntry( "viewmode", m_viewModeType) ;
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

KWTextFrameSet * KWDocument::textFrameSet ( unsigned int num ) const
{
    unsigned int i=0;
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        if(fit.current()->isDeleted()) continue;
        if(fit.current()->type()==FT_TEXT)
        {
            if(i==num)
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
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
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

    bool ok = false;

    if ( isEmbedded() )
    {
      QString fileName( locate( "kword_template", "Normal/.source/Embedded.kwt" , KWFactory::instance() ) );
      resetURL();
     ok = loadNativeFormat( fileName );
      if ( !ok )
        showLoadingErrorDialog();
      setEmpty();
      setModified( false );
      return ok;
    }
    else if (flags==KoDocument::InitDocEmpty)
    {
        QString fileName( locate( "kword_template", "Normal/.source/PlainText.kwt" , KWFactory::instance() ) );
        resetURL();
        ok = loadNativeFormat( fileName );
        if ( !ok )
            showLoadingErrorDialog();
        setEmpty();
        setModified( false );
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
        setEmpty();
    } else if ( ret == KoTemplateChooseDia::File ) {
        KUrl url( file );
        //kDebug() << "KWDocument::initDoc opening URL " << url.prettyURL() << endl;
        ok = openURL( url );
    } else if ( ret == KoTemplateChooseDia::Empty ) {
        QString fileName( locate( "kword_template", "Normal/.source/PlainText.kwt" , KWFactory::instance() ) );
        resetURL();
        ok = loadNativeFormat( fileName );
        if ( !ok )
            showLoadingErrorDialog();
        setEmpty();
    }
    setModified( false );
    return ok;
}

void KWDocument::openExistingFile( const QString& file )
{
  m_pageColumns.columns = 1;
  m_pageColumns.ptColumnSpacing = m_defaultColumnSpacing;

  m_pageHeaderFooter.header = HF_SAME;
  m_pageHeaderFooter.footer = HF_SAME;
  m_pageHeaderFooter.ptHeaderBodySpacing = 10;
  m_pageHeaderFooter.ptFooterBodySpacing = 10;
  m_pageHeaderFooter.ptFootNoteBodySpacing = 10;

  KoDocument::openExistingFile( file );
}

void KWDocument::openTemplate( const QString& file )
{
  m_pageColumns.columns = 1;
  m_pageColumns.ptColumnSpacing = m_defaultColumnSpacing;

  m_pageHeaderFooter.header = HF_SAME;
  m_pageHeaderFooter.footer = HF_SAME;
  m_pageHeaderFooter.ptHeaderBodySpacing = 10;
  m_pageHeaderFooter.ptFooterBodySpacing = 10;
  m_pageHeaderFooter.ptFootNoteBodySpacing = 10;

  KoDocument::openTemplate( file );
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
    setModified( false );
    setEmpty();
}

KoPageLayout KWDocument::pageLayout(int pageNumber /* = 0 */) const
{
    if( pageNumber < startPage()) // impossible page..
        pageNumber = startPage();
    return pageManager()->pageLayout(pageNumber);
}

void KWDocument::setPageLayout( const KoPageLayout& layout, const KoColumns& cl, const KoKWHeaderFooter& hf, bool updateViews )
{
    m_pageLayout = layout;
    if ( m_processingType == WP ) {
        m_pageColumns = cl;
    }
    if ( m_processingType == DTP || isEmbedded() ) {
        m_pageLayout.ptLeft = 0;
        m_pageLayout.ptRight = 0;
        m_pageLayout.ptTop = 0;
        m_pageLayout.ptBottom = 0;
    }
    pageManager()->setDefaultPage(m_pageLayout);
    m_pageHeaderFooter = hf;

    // pages have a different size -> update framesInPage
    // TODO: it would be better to move stuff so that text boxes remain in the same page...
    // (page-number preservation instead of Y preservation)
    updateAllFrames( KWFrameSet::UpdateFramesInPage );

    recalcFrames();

    updateAllFrames();

    if ( updateViews )
    {
        // Invalidate document layout, for proper repaint
        this->layout();
        emit pageLayoutChanged( m_pageLayout );
        updateContentsSize();
    }
}


double KWDocument::ptColumnWidth() const
{
    KWPage *page = pageManager()->page(pageManager()->startPage());
    return ( page->width() - page->leftMargin() - page->rightMargin() -
             ptColumnSpacing() * ( m_pageColumns.columns - 1 ) )
        / m_pageColumns.columns;
}

class KWFootNoteFrameSetList : public Q3PtrList<KWFootNoteFrameSet>
{
public:
    KWFootNoteFrameSetList( bool reversed ) : m_reversed( reversed ) {}
protected:
    // Compare the order of the associated variables
    virtual int compareItems(Q3PtrCollection::Item a, Q3PtrCollection::Item b)
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
    fromPage = qMax(pageManager()->startPage(), fromPage);
    if ( m_lstFrameSet.isEmpty() )
        return;
    //printDebug();
    kDebug(32002) << "KWDocument::recalcFrames from=" << fromPage << " to=" << toPage << endl;

    KWFrameSet *frameset = m_lstFrameSet.getFirst();

    if ( m_processingType == WP ) { // In WP mode the pages are created automatically. In DTP not...

        KWTextFrameSet *firstHeader = 0L, *evenHeader = 0L, *oddHeader = 0L;
        KWTextFrameSet *firstFooter = 0L, *evenFooter = 0L, *oddFooter = 0L;
        m_bHasEndNotes = false; // will be set to true if we find any endnote

        // Lookup the various header / footer framesets into the variables above
        // [Done in all cases, in order to hide unused framesets]

        KWFootNoteFrameSetList footnotesList( true ); // Reversed, we want footnotes from bottom to top
        KWFootNoteFrameSetList endnotesList( false ); // Endnotes are in top to bottom order
        Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
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
                break;
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
        Q3PtrList<KWFrameLayout::HeaderFooterFrameset> headerFooterList;
        headerFooterList.setAutoDelete( true );
        const int firstPageNum = startPage();

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
                                             oddHeader, firstPageNum, -1, m_pageHeaderFooter.ptHeaderBodySpacing ) );
                break;
            case HF_FIRST_EO_DIFF: // added for koffice-1.2-beta2
                firstHeader->setVisible( true );
                oddHeader->setVisible( true );
                evenHeader->setVisible( true );

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             firstHeader, firstPageNum, firstPageNum, m_pageHeaderFooter.ptHeaderBodySpacing ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddHeader, firstPageNum + 2, -1, m_pageHeaderFooter.ptHeaderBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Odd ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenHeader, firstPageNum + 1, -1, m_pageHeaderFooter.ptHeaderBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Even ) );
                break;
            case HF_FIRST_DIFF:
                oddHeader->setVisible( true );
                evenHeader->setVisible( false );
                evenHeader->deleteAllCopies();
                firstHeader->setVisible( true );

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             firstHeader, firstPageNum, firstPageNum, m_pageHeaderFooter.ptHeaderBodySpacing ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddHeader, firstPageNum + 1, -1, m_pageHeaderFooter.ptHeaderBodySpacing ) );
                break;
            case HF_EO_DIFF:
                oddHeader->setVisible( true );
                evenHeader->setVisible( true );
                firstHeader->setVisible( false );
                firstHeader->deleteAllCopies();

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddHeader, firstPageNum, -1, m_pageHeaderFooter.ptHeaderBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Odd ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenHeader, firstPageNum + 1, -1, m_pageHeaderFooter.ptHeaderBodySpacing,
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
                                             oddFooter, firstPageNum, -1, m_pageHeaderFooter.ptFooterBodySpacing ) );
                break;
            case HF_FIRST_EO_DIFF: // added for koffice-1.2-beta2
                firstFooter->setVisible( true );
                oddFooter->setVisible( true );
                evenFooter->setVisible( true );

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             firstFooter, firstPageNum, firstPageNum, m_pageHeaderFooter.ptFooterBodySpacing ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddFooter, firstPageNum + 2, -1, m_pageHeaderFooter.ptFooterBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Odd ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenFooter, firstPageNum + 1, -1, m_pageHeaderFooter.ptFooterBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Even ) );
                break;
            case HF_FIRST_DIFF:
                oddFooter->setVisible( true );
                evenFooter->setVisible( false );
                evenFooter->deleteAllCopies();
                firstFooter->setVisible( true );

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             firstFooter, firstPageNum, firstPageNum, m_pageHeaderFooter.ptFooterBodySpacing ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddFooter, firstPageNum + 1, -1, m_pageHeaderFooter.ptFooterBodySpacing ) );
                break;
            case HF_EO_DIFF:
                oddFooter->setVisible( true );
                evenFooter->setVisible( true );
                firstFooter->setVisible( false );
                firstFooter->deleteAllCopies();

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddFooter, firstPageNum, -1, m_pageHeaderFooter.ptFooterBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Odd ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenFooter, firstPageNum + 1, -1, m_pageHeaderFooter.ptFooterBodySpacing,
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
        Q3PtrList<KWFrameLayout::HeaderFooterFrameset> footnotesHFList;
        footnotesHFList.setAutoDelete( true );

        footnotesList.sort();
        Q3PtrListIterator<KWFootNoteFrameSet> fnfsIt( footnotesList );  // fnfs == "footnote frameset"
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
            for (Q3PtrListIterator<KWFrame> f = fnfs->frameIterator(); f.current() ; ++f )
                hff->m_height += f.current()->height();

            footnotesHFList.append( hff );
        }

        // Endnotes, however are laid out from top to bottom.
        Q3PtrList<KWFrameLayout::HeaderFooterFrameset> endnotesHFList;
        endnotesHFList.setAutoDelete( true );

        endnotesList.sort();
        Q3PtrListIterator<KWFootNoteFrameSet> enfsIt( endnotesList );  // enfs == "endnote frameset"
        for ( ; enfsIt.current() ; ++enfsIt )
        {
            KWFootNoteFrameSet* enfs = enfsIt.current();
            KWFrameLayout::HeaderFooterFrameset* hff = new KWFrameLayout::HeaderFooterFrameset(
                enfs, -42, -42, // determined by KWFrameLayout
                m_pageHeaderFooter.ptFootNoteBodySpacing,
                KWFrameLayout::HeaderFooterFrameset::All );

            // The height to pass to KWFrameLayout is the sum of the frame heights.
            hff->m_height = 0;
            for (Q3PtrListIterator<KWFrame> f = enfs->frameIterator(); f.current() ; ++f )
                hff->m_height += f.current()->height();

            endnotesHFList.append( hff );
        }

        // append pages as needed.
        double maxBottom = 0;
        for (Q3PtrListIterator<KWFrameSet> fsit = framesetsIterator(); fsit.current() ; ++fsit ) {
            KWFrameSet *fs = fsit.current();
            if ( !fs->isVisible() || fs->isAHeader() || !fs->isAFooter() ||
                    !fs->isFloating() || !fs->isFootEndNote() )
                continue;
            for (Q3PtrListIterator<KWFrame> fit = fs->frameIterator(); fit.current() ; ++fit )
                maxBottom = qMax(maxBottom, fit.current()->bottom());
        }
        KWPage *last = pageManager()->page(lastPage());
        double docHeight = last->offsetInDocument() + last->height();
        while(docHeight <= maxBottom) {
            last = pageManager()->appendPage();
            docHeight += last->height();
        }
        int oldPages = pageCount();

        if ( toPage == -1 )
            toPage = lastPage();
        if ( fromPage > toPage ) // this can happen with "endnotes only" pages :) // ### really?
            fromPage = toPage; // ie. start at the last real page
        KWFrameLayout frameLayout( this, headerFooterList, footnotesHFList, endnotesHFList );
        frameLayout.layout( frameset, m_pageColumns.columns, fromPage, toPage, flags );

        // If the number of pages changed, update views and variables etc.
        // (now that the frame layout has been done)
        if ( pageCount() != oldPages && !m_bGeneratingPreview )
        {
            // Very much like the end of appendPage, but we don't want to call recalcFrames ;)
            emit newContentsSize();
            emit numPagesChanged();
            recalcVariables( VT_PGNUM );
        }

    }
    else {
        // DTP mode: calculate the number of pages from the frames.
        double maxBottom=0;
        for (Q3PtrListIterator<KWFrameSet> fit = framesetsIterator(); fit.current() ; ++fit ) {
            if(fit.current()->isDeleted()) continue;
            if(fit.current()->frameSetInfo()==KWFrameSet::FI_BODY && !fit.current()->isFloating()) {
                KWFrameSet * fs = fit.current();
                for (Q3PtrListIterator<KWFrame> f = fs->frameIterator(); f.current() ; ++f )
                    maxBottom=qMax(maxBottom, f.current()->bottom());
            }
        }
        KWPage *last = pageManager()->page(lastPage());
        double docHeight = last->offsetInDocument() + last->height();
        while(docHeight <= maxBottom) {
            last = pageManager()->appendPage();
            docHeight += last->height();
        }
        if ( toPage == -1 )
            toPage = pageCount() - 1;
        KWFrameList::recalcFrames(this, fromPage, toPage);
    }
    kDebug(32002) << "            ~recalcFrames" << endl;
}

bool KWDocument::loadChildren( KoStore *store )
{
    //kDebug(32001) << "KWDocument::loadChildren" << endl;
    Q3PtrListIterator<KoDocumentChild> it( children() );
    for( ; it.current(); ++it ) {
        if ( !it.current()->loadDocument( store ) )
            return false;
    }

    return true;
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
    kDebug(32001) << "KWDocument::loadOasis" << endl;

    QDomElement content = doc.documentElement();
    QDomElement realBody ( KoDom::namedItemNS( content, KoXmlNS::office, "body" ) );
    if ( realBody.isNull() )
    {
        kError(32001) << "No office:body found!" << endl;
        setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No office:body tag found." ) );
        return false;
    }
    QDomElement body = KoDom::namedItemNS( realBody, KoXmlNS::office, "text" );
    if ( body.isNull() )
    {
        kError(32001) << "No office:text found!" << endl;
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

    KoOasisContext context( this, *m_varColl, oasisStyles, store );

    createLoadingInfo();

    // In theory the page format is the style:master-page-name of the first paragraph...
    // But, hmm, in a doc with only a table there was no reference to the master page at all...
    // So we load the standard page layout to start with, and in KWTextParag
    // we might overwrite it with another one.
    m_loadingInfo->m_currentMasterPage = "Standard";
    if ( !loadOasisPageLayout( m_loadingInfo->m_currentMasterPage, context ) )
        return false;

    KWOasisLoader oasisLoader( this );

    // <text:page-sequence> oasis extension for DTP (2003-10-27 post by Daniel)
    m_processingType = ( !KoDom::namedItemNS( body, KoXmlNS::text, "page-sequence" ).isNull() )
                       ? DTP : WP;

    m_hasTOC = false;
    m_tabStop = MM_TO_POINT(15);
    const QDomElement* defaultParagStyle = oasisStyles.defaultStyle( "paragraph" );
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

    // Load all styles before the corresponding paragraphs try to use them!
    m_styleColl->loadOasisStyles( context );
    if ( m_frameStyleColl->loadOasisStyles( context ) == 0 ) {
         // no styles loaded -> load default styles
        loadDefaultFrameStyleTemplates();
    }

    if ( m_tableStyleColl->loadOasisStyles( context, *m_styleColl, *m_frameStyleColl ) == 0 ) {
        // no styles loaded -> load default styles
        loadDefaultTableStyleTemplates();
    }

    static_cast<KWVariableSettings *>( m_varColl->variableSetting() )
        ->loadNoteConfiguration( oasisStyles.officeStyle() );

    loadDefaultTableTemplates();

    if ( m_processingType == WP ) {
        // Create main frameset
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Main Text Frameset" ) );
        m_lstFrameSet.append( fs ); // don't use addFrameSet here. We'll call finalize() once and for all in completeLoading
        fs->loadOasisContent( body, context );
        KWFrame* frame = new KWFrame( fs, 29, 42, 566-29, 798-42 );
        frame->setFrameBehavior( KWFrame::AutoCreateNewFrame );
        frame->setNewFrameBehavior( KWFrame::Reconnect );
        fs->addFrame( frame );

        // load padding, background and borders for the main frame
        const QDomElement* masterPage = context.oasisStyles().masterPages()[ m_loadingInfo->m_currentMasterPage ];
        const QDomElement *masterPageStyle = masterPage ? context.oasisStyles().findStyle(masterPage->attributeNS( KoXmlNS::style, "page-layout-name", QString::null ) ) : 0;
        if ( masterPageStyle )
        {
          KoStyleStack styleStack;
          styleStack.push(  *masterPageStyle );
          styleStack.setTypeProperties( "page-layout" );
          frame->loadBorderProperties( styleStack );
        }
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
                kDebug() << "DTP mode: found " << pages << "pages" << endl;
                //setPageCount ( pages );
            }
            else if ( localName == "frame" && tag.namespaceURI() == KoXmlNS::draw )
                oasisLoader.loadFrame( tag, context, KoPoint() );
            else
                kWarning(32001) << "Unsupported tag in DTP loading:" << tag.tagName() << endl;
        }
    }

    loadMasterPageStyle( m_loadingInfo->m_currentMasterPage, context );

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

    if ( !settings.isNull() )
    {
        oasisLoader.loadOasisSettings( settings );
    }

    kDebug(32001) << "Loading took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;
    endOfLoading();

    // This sets the columns and header/footer flags, and calls recalcFrames,
    // so it must be done last.
    setPageLayout( m_pageLayout, m_loadingInfo->columns, m_loadingInfo->hf, false );

    //printDebug();
    return true;
}

bool KWDocument::loadOasisPageLayout( const QString& masterPageName, KoOasisContext& context )
{
    KoColumns& columns = m_loadingInfo->columns;

    const KoOasisStyles& oasisStyles = context.oasisStyles();
    const QDomElement* masterPage = oasisStyles.masterPages()[ masterPageName ];
    Q_ASSERT( masterPage );
    const QDomElement *masterPageStyle = masterPage ? oasisStyles.findStyle( masterPage->attributeNS( KoXmlNS::style, "page-layout-name", QString::null ) ) : 0;
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
        pageManager()->setDefaultPage(m_pageLayout);

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
                kDebug() << "Unknown value for m_footNoteSeparatorLineType: " << style << endl;

            const QString pos = footnoteSep.attributeNS( KoXmlNS::style, "adjustment", QString::null );
            if ( pos == "centered" )
                m_footNoteSeparatorLinePos = SLP_CENTERED;
            else if ( pos == "right")
                m_footNoteSeparatorLinePos = SLP_RIGHT;
            else // if ( pos == "left" )
                m_footNoteSeparatorLinePos = SLP_LEFT;
        }

        const QDomElement columnsElem = KoDom::namedItemNS( properties, KoXmlNS::style, "columns" );
        if ( !columnsElem.isNull() )
        {
            columns.columns = columnsElem.attributeNS( KoXmlNS::fo, "column-count", QString::null ).toInt();
            if ( columns.columns == 0 )
                columns.columns = 1;
            // TODO OASIS OpenDocument supports columns of different sizes, using <style:column style:rel-width="...">
            // (with fo:start-indent/fo:end-indent for per-column spacing)
            // But well, it also allows us to specify a single gap.
            if ( columnsElem.hasAttributeNS( KoXmlNS::fo, "column-gap" ) )
                columns.ptColumnSpacing = KoUnit::parseValue( columnsElem.attributeNS( KoXmlNS::fo, "column-gap", QString::null ) );
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
        columns.columns = 1;
        columns.ptColumnSpacing = 2;
        m_headerVisible = false;
        m_footerVisible = false;
        m_pageLayout = KoPageLayout::standardLayout();
        pageManager()->setDefaultPage(m_pageLayout);
    }
    return true;
}

void KWDocument::loadMasterPageStyle( const QString& masterPageName, KoOasisContext& context )
{
    const KoOasisStyles& oasisStyles = context.oasisStyles();
    const QDomElement* masterPage = oasisStyles.masterPages()[ masterPageName ];
    Q_ASSERT( masterPage );
    const QDomElement *masterPageStyle = masterPage ? oasisStyles.findStyle( masterPage->attributeNS( KoXmlNS::style, "page-layout-name", QString::null ) ) : 0;
    Q_ASSERT( masterPageStyle );

    KoKWHeaderFooter& hf = m_loadingInfo->hf;

    bool hasEvenOddHeader = false;
    bool hasEvenOddFooter = false;
    if ( masterPageStyle )
    {
        KWOasisLoader oasisLoader( this );

        QDomElement headerStyle = KoDom::namedItemNS( *masterPageStyle, KoXmlNS::style, "header-style" );
        QDomElement footerStyle = KoDom::namedItemNS( *masterPageStyle, KoXmlNS::style, "footer-style" );
        QDomElement headerLeftElem = KoDom::namedItemNS( *masterPage, KoXmlNS::style, "header-left" );
        QDomElement headerFirstElem = KoDom::namedItemNS( *masterPage, KoXmlNS::style, "header-first" ); // hack, not oasis compliant
        const bool hasFirstHeader = !headerFirstElem.isNull();
        if ( !headerLeftElem.isNull() )
        {
            hasEvenOddHeader = true;
            hf.header = hasFirstHeader ? HF_FIRST_EO_DIFF : HF_EO_DIFF;
            oasisLoader.loadOasisHeaderFooter( headerLeftElem, hasEvenOddHeader, headerStyle, context );
        }
        else
        {
            hf.header = hasFirstHeader ? HF_FIRST_DIFF : HF_SAME;
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
            hf.footer = hasFirstFooter ? HF_FIRST_EO_DIFF : HF_EO_DIFF;
            oasisLoader.loadOasisHeaderFooter( footerLeftElem, hasEvenOddFooter, footerStyle, context );
        }
        else
        {
            hf.footer = hasFirstFooter ? HF_FIRST_DIFF : HF_SAME;
        }
        if ( hasFirstFooter )
        {
            oasisLoader.loadOasisHeaderFooter( footerFirstElem, hasEvenOddFooter, footerStyle, context );
        }
        QDomElement footerElem = KoDom::namedItemNS( *masterPage, KoXmlNS::style, "footer" );
        if ( !footerElem.isNull() )
        {
            oasisLoader.loadOasisHeaderFooter( footerElem, hasEvenOddFooter, footerStyle, context );
        }

        // The bottom margin of headers is what we call headerBodySpacing
        // (TODO support the 3 other margins)
        if ( !headerStyle.isNull() ) {
            context.styleStack().push( headerStyle );
            context.styleStack().setTypeProperties( "header-footer" );
            hf.ptHeaderBodySpacing = KoUnit::parseValue( context.styleStack().attributeNS( KoXmlNS::fo, "margin-bottom" ) );
            context.styleStack().pop();
        }
        // The top margin of footers is what we call footerBodySpacing
        // (TODO support the 3 other margins)
        if ( !footerStyle.isNull() ) {
            context.styleStack().push( footerStyle );
            context.styleStack().setTypeProperties( "header-footer" );
            hf.ptFooterBodySpacing = KoUnit::parseValue( context.styleStack().attributeNS( KoXmlNS::fo, "margin-top" ) );
            context.styleStack().pop();
        }
        // TODO ptFootNoteBodySpacing
    }
}

// Called before loading
// It's important to clear out anything that might be in the document already,
// for things like using DCOP to load multiple documents into the same KWDocument,
// or "reload" when kword is embedded into konqueror.
void KWDocument::clear()
{
    m_pictureMap.clear();
    m_textImageRequests.clear();
    m_pictureRequests.clear();
    m_anchorRequests.clear();
    m_footnoteVarRequests.clear();
    m_spellCheckIgnoreList.clear();

    m_pageHeaderFooter.header = HF_SAME;
    m_pageHeaderFooter.footer = HF_SAME;
    m_pageHeaderFooter.ptHeaderBodySpacing = 10;
    m_pageHeaderFooter.ptFooterBodySpacing = 10;
    m_pageHeaderFooter.ptFootNoteBodySpacing = 10;
    m_pageColumns.columns = 1;
    m_pageColumns.ptColumnSpacing = m_defaultColumnSpacing;
    m_bHasEndNotes = false;

    m_iFootNoteSeparatorLineLength = 20; // 20%, i.e. 1/5th
    m_footNoteSeparatorLineWidth = 0.5; // like in OOo
    m_footNoteSeparatorLineType = SLT_SOLID;

    m_lstFrameSet.clear();

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
    //kDebug() << "KWDocument::KWDocument creating standardStyle " << standardStyle << endl;
    standardStyle->format().setFont( m_defaultFont );
    m_styleColl->addStyle( standardStyle );

    // And let's do the same for framestyles
    KWFrameStyle * standardFrameStyle = new KWFrameStyle( "Plain" );
    standardFrameStyle->setBackgroundColor(Qt::white);
    standardFrameStyle->setTopBorder(KoBorder(Qt::black,KoBorder::SOLID,0));
    standardFrameStyle->setRightBorder(KoBorder(Qt::black,KoBorder::SOLID,0));
    standardFrameStyle->setLeftBorder(KoBorder(Qt::black,KoBorder::SOLID,0));
    standardFrameStyle->setBottomBorder(KoBorder(Qt::black,KoBorder::SOLID,0));
    m_frameStyleColl->addStyle( standardFrameStyle );

    // And let's do the same for tablestyles
    KWTableStyle *standardTableStyle = new KWTableStyle( "Plain", standardStyle, standardFrameStyle );
    m_tableStyleColl->addStyle( standardTableStyle );
}

bool KWDocument::loadXML( QIODevice *, const QDomDocument & doc )
{
    QTime dt;
    dt.start();
    emit sigProgress( 0 );
    kDebug(32001) << "KWDocument::loadXML" << endl;
    clear();

    KoPageLayout pgLayout;
    KoColumns columns;
    columns.columns = 1;
    columns.ptColumnSpacing = m_defaultColumnSpacing;
    KoKWHeaderFooter hf;
    hf.header = HF_SAME;
    hf.footer = HF_SAME;
    hf.ptHeaderBodySpacing = 10.0;
    hf.ptFooterBodySpacing = 10.0;
    hf.ptFootNoteBodySpacing = 10.0;

    QString value;
    QDomElement word = doc.documentElement();

    value = KWDocument::getAttribute( word, "mime", QString::null );
    if ( value.isEmpty() )
    {
        kError(32001) << "No mime type specified!" << endl;
        setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
        return false;
    }
    else if ( value != "application/x-kword" && value != "application/vnd.kde.kword" )
    {
        kError(32001) << "Unknown mime type " << value << endl;
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
        m_urlIntern = KUrl( value ).path();
    }

    emit sigProgress(5);

    // <PAPER>
    QDomElement paper = word.namedItem( "PAPER" ).toElement();
    if ( !paper.isNull() )
    {
        pgLayout.format = static_cast<KoFormat>( KWDocument::getAttribute( paper, "format", 0 ) );
        pgLayout.orientation = static_cast<KoOrientation>( KWDocument::getAttribute( paper, "orientation", 0 ) );
        pgLayout.ptWidth = getAttribute( paper, "width", 0.0 );
        pgLayout.ptHeight = getAttribute( paper, "height", 0.0 );
        kDebug() << " ptWidth=" << pgLayout.ptWidth << endl;
        kDebug() << " ptHeight=" << pgLayout.ptHeight << endl;
        if ( pgLayout.ptWidth <= 0 || pgLayout.ptHeight <= 0 )
        {
            // Old document?
            pgLayout.ptWidth = getAttribute( paper, "ptWidth", 0.0 );
            pgLayout.ptHeight = getAttribute( paper, "ptHeight", 0.0 );
            kDebug() << " ptWidth=" << pgLayout.ptWidth << endl;
            kDebug() << " ptHeight=" << pgLayout.ptHeight << endl;

            // Still wrong?
            if ( pgLayout.ptWidth <= 0 || pgLayout.ptHeight <= 0 )
            {
                setErrorMessage( i18n( "Invalid document. Paper size: %1x%2" )
                    .arg( pgLayout.ptWidth ).arg( pgLayout.ptHeight ) );
                return false;
            }
        }

        hf.header = static_cast<KoHFType>( KWDocument::getAttribute( paper, "hType", 0 ) );
        hf.footer = static_cast<KoHFType>( KWDocument::getAttribute( paper, "fType", 0 ) );
        hf.ptHeaderBodySpacing = getAttribute( paper, "spHeadBody", 0.0 );
        hf.ptFooterBodySpacing  = getAttribute( paper, "spFootBody", 0.0 );
        hf.ptFootNoteBodySpacing  = getAttribute( paper, "spFootNoteBody", 10.0 );
        m_iFootNoteSeparatorLineLength = getAttribute( paper, "slFootNoteLength", 20);
        if ( paper.hasAttribute( "slFootNoteWidth" ) )
            m_footNoteSeparatorLineWidth = paper.attribute( "slFootNoteWidth" ).toDouble();
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
        columns.columns = KWDocument::getAttribute( paper, "columns", 1 );
        columns.ptColumnSpacing = KWDocument::getAttribute( paper, "columnspacing", 0.0 );
        // Now part of the app config
        //m_zoom = KWDocument::getAttribute( paper, "zoom", 100 );
        //if(m_zoom!=100)
        //    setZoomAndResolution( m_zoom, KoGlobal::dpiX(), KoGlobal::dpiY(), false, false );


        // Support the undocumented syntax actually used by KDE 2.0 for some of the above (:-().
        // Do not add anything to this block!
        if ( pgLayout.ptWidth == 0.0 )
            pgLayout.ptWidth = getAttribute( paper, "ptWidth", 0.0 );
        if ( pgLayout.ptHeight == 0.0 )
            pgLayout.ptHeight = getAttribute( paper, "ptHeight", 0.0 );
        if ( hf.ptHeaderBodySpacing == 0.0 )
            hf.ptHeaderBodySpacing = getAttribute( paper, "ptHeadBody", 0.0 );
        if ( hf.ptFooterBodySpacing == 0.0 )
            hf.ptFooterBodySpacing = getAttribute( paper, "ptFootBody", 0.0 );
        if ( columns.ptColumnSpacing == 0.0 )
            columns.ptColumnSpacing = getAttribute( paper, "ptColumnspc", 0.0 );

        // <PAPERBORDERS>
        QDomElement paperborders = paper.namedItem( "PAPERBORDERS" ).toElement();
        if ( !paperborders.isNull() )
        {
            pgLayout.ptLeft = getAttribute( paperborders, "left", 0.0 );
            pgLayout.ptTop = getAttribute( paperborders, "top", 0.0 );
            pgLayout.ptRight = getAttribute( paperborders, "right", 0.0 );
            pgLayout.ptBottom = getAttribute( paperborders, "bottom", 0.0 );

            // Support the undocumented syntax actually used by KDE 2.0 for some of the above (:-().
            if ( pgLayout.ptLeft == 0.0 )
                pgLayout.ptLeft = getAttribute( paperborders, "ptLeft", 0.0 );
            if ( pgLayout.ptTop == 0.0 )
                pgLayout.ptTop = getAttribute( paperborders, "ptTop", 0.0 );
            if ( pgLayout.ptRight == 0.0 )
                pgLayout.ptRight = getAttribute( paperborders, "ptRight", 0.0 );
            if ( pgLayout.ptBottom == 0.0 )
                pgLayout.ptBottom = getAttribute( paperborders, "ptBottom", 0.0 );
        }
        else
            kWarning() << "No <PAPERBORDERS> tag!" << endl;
    }
    else
        kWarning() << "No <PAPER> tag! This is a mandatory tag! Expect weird page sizes..." << endl;

    // <ATTRIBUTES>
    QDomElement attributes = word.namedItem( "ATTRIBUTES" ).toElement();
    if ( !attributes.isNull() )
    {
        m_processingType = static_cast<ProcessingType>( KWDocument::getAttribute( attributes, "processing", 0 ) );
        //KWDocument::getAttribute( attributes, "standardpage", QString::null );
        m_headerVisible = static_cast<bool>( KWDocument::getAttribute( attributes, "hasHeader", 0 ) );
        m_footerVisible = static_cast<bool>( KWDocument::getAttribute( attributes, "hasFooter", 0 ) );
        if ( attributes.hasAttribute( "unit" ) )
            setUnit( KoUnit::unit( attributes.attribute( "unit" ) ) );
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

    setPageLayout( pgLayout, columns, hf, false );

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

    kDebug(32001) << "Loading took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;

    endOfLoading();
    return true;
}

void KWDocument::endOfLoading() // called by both oasis and oldxml
{
    // insert pages
    double maxBottom = 0;
    for (Q3PtrListIterator<KWFrameSet> fsit = framesetsIterator(); fsit.current() ; ++fsit ) {
        KWFrameSet *fs = fsit.current();
        for (Q3PtrListIterator<KWFrame> fit = fs->frameIterator(); fit.current() ; ++fit ) {
            KWFrame *frame = fit.current();
            maxBottom = qMax(maxBottom, frame->bottom());
        }
    }
    KWPage *last = pageManager()->page(lastPage());
    double docHeight = last->offsetInDocument() + last->height();
    while(docHeight <= maxBottom) {
        kDebug(32001) << "KWDocument::loadXML appends a page\n";
        last = pageManager()->appendPage();
        docHeight += last->height();
    }

    bool first_footer = false, even_footer = false, odd_footer = false;
    bool first_header = false, even_header = false, odd_header = false;

    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        switch( fit.current()->frameSetInfo() ) {
        case KWFrameSet::FI_FIRST_HEADER: first_header = true; break;
        case KWFrameSet::FI_ODD_HEADER: odd_header = true; break;
        case KWFrameSet::FI_EVEN_HEADER: even_header = true; break;
        case KWFrameSet::FI_FIRST_FOOTER: first_footer = true; break;
        case KWFrameSet::FI_ODD_FOOTER: odd_footer = true; break;
        case KWFrameSet::FI_EVEN_FOOTER: even_footer = true; break;
        case KWFrameSet::FI_FOOTNOTE: break;
        default: break;
        }
    }

    // Create defaults if they were not in the input file.

    // Where to insert the new frames: not at the end, since that breaks oasis-kword.sh
    uint newFramesetsIndex = m_lstFrameSet.isEmpty() ? 0 : 1;

    if ( !first_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "First Page Header" ) );
        //kDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameSetInfo( KWFrameSet::FI_FIRST_HEADER );
        KWPage *page = pageManager()->page(startPage());
        KWFrame *frame = new KWFrame(fs, page->leftMargin(), page->topMargin(),
                page->width() - page->leftMargin() - page->rightMargin(), 20 );
        //kDebug(32001) << "KWDocument::loadXML KWFrame created " << frame << endl;
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.insert( newFramesetsIndex++, fs );
    }

    if ( !odd_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Odd Pages Header" ) );
        //kDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameSetInfo( KWFrameSet::FI_ODD_HEADER );
        KWPage *page = pageManager()->page(qMin(startPage()+2, lastPage()));
        KWFrame *frame = new KWFrame(fs, page->leftMargin(), page->topMargin(),
                page->width() - page->leftMargin() - page->rightMargin(), 20 );
        //kDebug(32001) << "KWDocument::loadXML KWFrame created " << *frame << endl;
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.insert( newFramesetsIndex++, fs );
    }

    if ( !even_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Even Pages Header" ) );
        //kDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameSetInfo( KWFrameSet::FI_EVEN_HEADER );
        KWPage *page = pageManager()->page(qMin(startPage()+1, lastPage()));
        KWFrame *frame = new KWFrame(fs, page->leftMargin(), page->topMargin(), page->width() -
                page->leftMargin() - page->rightMargin(), 20 );
        //kDebug(32001) << "KWDocument::loadXML KWFrame created " << *frame << endl;
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.insert( newFramesetsIndex++, fs );
    }

    if ( !first_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "First Page Footer" ) );
        //kDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameSetInfo( KWFrameSet::FI_FIRST_FOOTER );
        KWPage *page = pageManager()->page(pageManager()->startPage());
        KWFrame *frame = new KWFrame(fs, page->leftMargin(), page->height() - page->topMargin()- 20,
                page->width() - page->leftMargin() - page->rightMargin(), 20 );
        //kDebug(32001) << "KWDocument::loadXML KWFrame created " << *frame << endl;
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.insert( newFramesetsIndex++, fs );
    }

    if ( !odd_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Odd Pages Footer" ) );
        //kDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameSetInfo( KWFrameSet::FI_ODD_FOOTER );
        KWPage *page = pageManager()->page(qMin(startPage()+2, lastPage()));
        KWFrame *frame = new KWFrame(fs, page->leftMargin(), page->height()- page->topMargin() - 20,
                page->width() - page->leftMargin() - page->rightMargin(), 20 );
        //kDebug(32001) << "KWDocument::loadXML KWFrame created " << *frame << endl;
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.insert( newFramesetsIndex++, fs );
    }

    if ( !even_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Even Pages Footer" ) );
        //kDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameSetInfo( KWFrameSet::FI_EVEN_FOOTER );
        KWPage *page = pageManager()->page(qMin(startPage()+1, lastPage()));
        KWFrame *frame = new KWFrame(fs, page->leftMargin(), page->height() - page->topMargin()- 20,
                page->width() - page->leftMargin() - page->rightMargin(), 20 );
        //kDebug(32001) << "KWDocument::loadXML KWFrame created " << *frame << endl;
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehavior( KWFrame::Copy );
        fs->addFrame( frame );
        m_lstFrameSet.insert( newFramesetsIndex++, fs );
    }

    // do some sanity checking on document.
    for (int i = frameSetCount()-1; i>-1; i--) {
        KWFrameSet *fs = frameSet(i);
        if(!fs) {
            kWarning() << "frameset " << i << " is NULL!!" << endl;
            m_lstFrameSet.remove(i);
            continue;
        }
        if( fs->type()==FT_TABLE) {
            static_cast<KWTableFrameSet *>( fs )->validate();
        } else if (fs->type() == FT_TEXT) {
            for (int f=fs->frameCount()-1; f>=0; f--) {
                KWFrame *frame = fs->frame(f);
                if(frame->left() < 0) {
                    kWarning() << fs->name() << " frame " << f << " pos.x is < 0, moving frame" << endl;
                    frame->moveBy( 0- frame->left(), 0);
                }
                if(frame->right() > m_pageLayout.ptWidth) {
                    kWarning() << fs->name() << " frame " << f << " rightborder outside page ("
                        << frame->right() << ">" << m_pageLayout.ptWidth << "), shrinking" << endl;
                    frame->setRight(m_pageLayout.ptWidth);
                }
                if(fs->isProtectSize())
                    continue; // don't make frames bigger of a protected frameset.
                if(frame->height() < s_minFrameHeight) {
                    kWarning() << fs->name() << " frame " << f << " height is so small no text will fit, adjusting (was: "
                                << frame->height() << " is: " << s_minFrameHeight << ")" << endl;
                    frame->setHeight(s_minFrameHeight);
                }
                if(frame->width() < s_minFrameWidth) {
                    kWarning() << fs->name() << " frame " << f << " width is so small no text will fit, adjusting (was: "
                                << frame->width() << " is: " << s_minFrameWidth  << ")" << endl;
                    frame->setWidth(s_minFrameWidth);
                }
            }
            if(fs->frameCount() == 0) {
                KWPage *page = pageManager()->page(startPage());
                KWFrame *frame = new KWFrame(fs, page->leftMargin(), page->topMargin(),
                        page->width() - page->leftMargin() - page->rightMargin(),
                        page->height() - page->topMargin() - page->bottomMargin());
                //kDebug(32001) << "KWDocument::loadXML main-KWFrame created " << *frame << endl;
                fs->addFrame( frame );
            }
        } else if(fs->frameCount() == 0) {
            kWarning () << "frameset " << i << " " << fs->name() << " has no frames" << endl;
            removeFrameSet(fs);
            if ( fs->type() == FT_PART )
                delete static_cast<KWPartFrameSet *>(fs)->getChild();
            delete fs;
            continue;
        }
        if(fs->frameCount() > 0) {
            KWFrame *frame = fs->frame(0);
            if(frame->isCopy()) {
                kWarning() << "First frame in a frameset[" << fs->name() << "] was set to be a copy; resetting\n";
                frame->setCopy(false);
            }
        }
    }

    // Renumber footnotes
    KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet *>( m_lstFrameSet.getFirst() );
    if ( frameset  )
        frameset->renumberFootNotes( false /*no repaint*/ );

    emit sigProgress(-1);

    //kDebug(32001) << "KWDocument::loadXML done" << endl;

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
        KWDocumentChild *ch = new KWDocumentChild( this );
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
            kDebug(32001) << "KWDocument::loadXML loading embedded object" << endl;
            fs->load( settings );
        }
        else
            kError(32001) << "No <SETTINGS> tag in EMBEDDED" << endl;

    } else
        kError(32001) << "No <OBJECT> tag in EMBEDDED" << endl;
}


void KWDocument::loadStyleTemplates( const QDomElement &stylesElem )
{
    Q3ValueList<QString> followingStyles;
    QDomNodeList listStyles = stylesElem.elementsByTagName( "STYLE" );
    if( listStyles.count() > 0) { // we are going to import at least one style.
        KoParagStyle *s = m_styleColl->findStyle("Standard");
        //kDebug(32001) << "KWDocument::loadStyleTemplates looking for Standard, to delete it. Found " << s << endl;
        if(s) // delete the standard style.
            m_styleColl->removeStyle(s);
    }
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KoParagStyle *sty = new KoParagStyle( QString::null );
        // Load the style from the <STYLE> element
        sty->loadStyle( styleElem, m_syntaxVersion );

        //kDebug(32001) << "KoParagStyle created name=" << sty->name() << endl;

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
            kWarning(32001) << "No FORMAT tag in <STYLE>" << endl; // This leads to problems in applyStyle().

        // Style created, now let's try to add it
        sty = m_styleColl->addStyle( sty );

        if(m_styleColl->styleList().count() > followingStyles.count() )
        {
            QString following = styleElem.namedItem("FOLLOWING").toElement().attribute("name");
            followingStyles.append( following );
        }
        else
            kWarning () << "Found duplicate style declaration, overwriting former " << sty->name() << endl;
    }

    Q_ASSERT( followingStyles.count() == m_styleColl->styleList().count() );

    unsigned int i=0;
    for( Q3ValueList<QString>::Iterator it = followingStyles.begin(); it != followingStyles.end(); ++it ) {
        KoParagStyle * style = m_styleColl->findStyle(*it);
        m_styleColl->styleAt(i++)->setFollowingStyle( style );
    }

}

void KWDocument::loadFrameStyleTemplates( const QDomElement &stylesElem )
{
    QDomNodeList listStyles = stylesElem.elementsByTagName( "FRAMESTYLE" );
    if( listStyles.count() > 0) { // we are going to import at least one style.
        KWFrameStyle *s = m_frameStyleColl->findStyle("Plain");
        if(s) // delete the standard style.
            m_frameStyleColl->removeStyle(s);
    }
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KWFrameStyle *sty = new KWFrameStyle( styleElem );
        m_frameStyleColl->addStyle( sty );
    }
}

void KWDocument::loadDefaultFrameStyleTemplates()
{
    const QString fsfileName( locate("data", "kword/framestyles.xml") );

    kDebug(30003) << "Data directory: " << KGlobal::dirs()->resourceDirs( "data" ) << endl;
    kDebug(30003) << "Directory searched: " << KGlobal::dirs()->resourceDirs( "data" ) << endl;
    kDebug(30003) << "File framestyles.xml searched at: " << fsfileName << endl;

    m_frameStyleColl->setDefault( true );

    if ( ! QFile::exists( fsfileName ) )
    {
        kWarning(30003) << "Cannot find any framestyles.xml" << endl;
        if (!m_frameStyleColl->findStyle("Plain")) {
            KWFrameStyle * standardFrameStyle = new KWFrameStyle( "Plain" );
            standardFrameStyle->setBackgroundColor(QColor("white"));
            standardFrameStyle->setTopBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            standardFrameStyle->setRightBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            standardFrameStyle->setLeftBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            standardFrameStyle->setBottomBorder(KoBorder(QColor("black"),KoBorder::SOLID,0));
            m_frameStyleColl->addStyle( standardFrameStyle );
        }
        return;
    }

    kDebug(30003) << "File framestyles.xml found!" << endl;

    // Open file and parse it
    QFile in( fsfileName );
    if ( !in.open( QIODevice::ReadOnly ) )
    {
        //i18n( "Couldn't open the file for reading (check read permissions)" );
        kWarning(30003) << "Couldn't open the file for reading (check read permissions)" << endl;
        return;
    }
    QString errorMsg;
    int errorLine;
    int errorColumn;
    QDomDocument doc;
    if ( ! doc.setContent( &in , &errorMsg, &errorLine, &errorColumn ) )
    {
        kError (30003) << "Parsing Error! Aborting! (in KWDocument::loadDefaultFrameStyleTemplates())" << endl
                        << "  Line: " << errorLine << " Column: " << errorColumn << endl
                        << "  Message: " << errorMsg << endl;
    }
    in.close();

    // Start adding framestyles
    QDomElement stylesElem = doc.documentElement();

    QDomNodeList listStyles = stylesElem.elementsByTagName( "FRAMESTYLE" );
    if( listStyles.count() > 0) { // we are going to import at least one style.
        KWFrameStyle *s = m_frameStyleColl->findStyle("Plain");
        if(s) // delete the standard style.
            m_frameStyleColl->removeStyle(s);
    }
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KWFrameStyle *sty = new KWFrameStyle( styleElem );
        m_frameStyleColl->addStyle( sty );
    }
}

void KWDocument::loadTableStyleTemplates( const QDomElement& stylesElem )
{
    QDomNodeList listStyles = stylesElem.elementsByTagName( "TABLESTYLE" );
    if( listStyles.count() > 0) { // we are going to import at least one style.
        KWTableStyle *s = m_tableStyleColl->findStyle("Plain");
        if(s) // delete the standard style.
            m_tableStyleColl->removeStyle(s);
    }
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KWTableStyle *sty = new KWTableStyle( styleElem, this );
        m_tableStyleColl->addStyle( sty );
    }
}

void KWDocument::loadDefaultTableStyleTemplates()
{
    KUrl fsfile;

    m_tableStyleColl->setDefault( true );

    if ( ! QFile::exists(locate("data", "kword/tablestyles.xml")) )
    {
        if (!m_tableStyleColl->findStyle("Plain")) {
            m_tableStyleColl->addStyle( new KWTableStyle( "Plain", m_styleColl->styleAt(0), m_frameStyleColl->frameStyleAt(0) ) );
        }
        return;
    }

    fsfile.setPath( locate("data", "kword/tablestyles.xml") );

    // Open file and parse it
    QFile in( fsfile.path() );
    if ( !in.open( QIODevice::ReadOnly ) )
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
        kError (30003) << "Parsing Error! Aborting! (in KWDocument::loadDefaultTableStyleTemplates())" << endl
                        << "  Line: " << errorLine << " Column: " << errorColumn << endl
                        << "  Message: " << errorMsg << endl;
    }
    in.close();

    // Start adding tablestyles
    QDomElement stylesElem = doc.documentElement();

    QDomNodeList listStyles = stylesElem.elementsByTagName( "TABLESTYLE" );
    if( listStyles.count() > 0) { // we are going to import at least one style.
        KWTableStyle *s = m_tableStyleColl->findStyle("Plain");
        if(s) // delete the standard style.
            m_tableStyleColl->removeStyle(s);
    }
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KWTableStyle *sty = new KWTableStyle( styleElem, this );
        m_tableStyleColl->addStyle( sty );
    }
}

void KWDocument::loadDefaultTableTemplates()
{
    KUrl fsfile;

    if ( ! QFile::exists(locate("data", "kword/tabletemplates.xml")) )
    {
        if (!m_tableTemplateColl->findTableTemplate("Plain")) {
            KWTableTemplate * standardTableTemplate = new KWTableTemplate( "Plain" );
            KWTableStyle* defaultTableStyle = tableStyleCollection()->findStyle("Plain");
            standardTableTemplate->setFirstRow( defaultTableStyle );
            standardTableTemplate->setLastRow( defaultTableStyle );
            standardTableTemplate->setFirstCol( defaultTableStyle );
            standardTableTemplate->setLastCol( defaultTableStyle );
            standardTableTemplate->setBodyCell( defaultTableStyle );
            standardTableTemplate->setTopLeftCorner( defaultTableStyle );
            standardTableTemplate->setTopRightCorner( defaultTableStyle );
            standardTableTemplate->setBottomLeftCorner( defaultTableStyle );
            standardTableTemplate->setBottomRightCorner( defaultTableStyle );
            m_tableTemplateColl->addTableTemplate( standardTableTemplate );
        }
        return;
    }

    fsfile.setPath( locate("data", "kword/tabletemplates.xml") );

    // Open file and parse it
    QFile in( fsfile.path() );
    if ( !in.open( QIODevice::ReadOnly ) )
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
        kError (30003) << "Parsing Error! Aborting! (in KWDocument::readTableTemplates())" << endl
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
        //kDebug(32001) << m_itemsLoaded << " items loaded. %=" << perc + 20 << endl;
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
    Q3ValueList<QDomElement> framesets;
    for ( ; !framesetElem.isNull() ; framesetElem = framesetElem.nextSibling().toElement() )
    {
        if ( framesetElem.tagName() == "FRAMESET" )
        {
            framesets.append( framesetElem );
            m_nrItemsToLoad += framesetElem.childNodes().count();
        }
    }

    m_itemsLoaded = 0;

    Q3ValueList<QDomElement>::Iterator it = framesets.begin();
    Q3ValueList<QDomElement>::Iterator end = framesets.end();
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
            Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
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
                addFrameSet(table, false);
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
                addFrameSet(fs, false);
                return fs;
            }
            else // Normal text frame
            {
                KWTextFrameSet *fs = new KWTextFrameSet( this, fsname );
                fs->load( framesetElem, loadFrames );
                addFrameSet(fs, false);

                // Old file format had autoCreateNewFrame as a frameset attribute
                if ( framesetElem.hasAttribute( "autoCreateNewFrame" ) )
                {
                    KWFrame::FrameBehavior behav = static_cast<KWFrame::FrameBehavior>( framesetElem.attribute( "autoCreateNewFrame" ).toInt() );
                    Q3PtrListIterator<KWFrame> frameIt( fs->frameIterator() );
                    for ( ; frameIt.current() ; ++frameIt ) // Apply it to all frames
                        frameIt.current()->setFrameBehavior( behav );
                }
                return fs;
            }
        }
    } break;
    case FT_CLIPART:
    {
        kError(32001) << "FT_CLIPART used! (in KWDocument::loadFrameSet)" << endl;
        // Do not break!
    }
    case FT_PICTURE:
    {
        KWPictureFrameSet *fs = new KWPictureFrameSet( this, fsname );
        fs->load( framesetElem, loadFrames );
        addFrameSet(fs, false);
        return fs;
    } break;
    case FT_FORMULA: {
        KWFormulaFrameSet *fs = new KWFormulaFrameSet( this, fsname );
        fs->load( framesetElem, loadFrames );
        addFrameSet(fs, false);
        return fs;
    } break;
    // Note that FT_PART cannot happen when loading from a file (part frames are saved into the SETTINGS tag)
    // and FT_TABLE can't happen either.
    case FT_PART:
        kWarning(32001) << "loadFrameSet: FT_PART: impossible case" << endl;
        break;
    case FT_TABLE:
        kWarning(32001) << "loadFrameSet: FT_TABLE: impossible case" << endl;
        break;
    case FT_BASE:
        kWarning(32001) << "loadFrameSet: FT_BASE !?!?" << endl;
        break;
    }
    return 0L;
}

void KWDocument::loadImagesFromStore( KoStore *store )
{
    if ( store && !m_pictureMap.isEmpty() ) {
        m_pictureCollection->readFromStore( store, m_pictureMap );
        m_pictureMap.clear(); // Release memory
    }
}

bool KWDocument::completeLoading( KoStore *store )
{
    kDebug() << k_funcinfo << endl;
    // Old-XML stuff. No-op when loading OASIS.
    loadImagesFromStore( store );
    processPictureRequests();
    processAnchorRequests();
    processFootNoteRequests();

    // Save memory
    m_urlIntern = QString::null;

    // The fields and dates just got loaded -> update vars
    recalcVariables( VT_FIELD );
    recalcVariables( VT_DATE );
    recalcVariables( VT_STATISTIC ); // e.g. number of words etc.

    // Finalize all the existing [non-inline] framesets
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->finalize();

    // This computes the number of pages (from the frames)
    // for the first time (and adds footers/headers/footnotes etc.)
    // ## Note: with OASIS the frame loading appends pages as necessary,
    // so maybe we don't need to calculate the pages from the frames anymore.
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
    m_loadingInfo->columns.ptColumnSpacing = m_defaultColumnSpacing;
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
    Q3PtrListIterator<KWTextImage> it2 ( m_textImageRequests );
    for ( ; it2.current() ; ++it2 )
    {
        it2.current()->setImage( *m_pictureCollection );
    }
    m_textImageRequests.clear();

    //kDebug(32001) << m_pictureRequests.count() << " picture requests." << endl;
    Q3PtrListIterator<KWPictureFrameSet> it3( m_pictureRequests );
    for ( ; it3.current() ; ++it3 )
        it3.current()->setPicture( m_pictureCollection->findPicture( it3.current()->key() ) );
    m_pictureRequests.clear();
}

void KWDocument::processAnchorRequests()
{
    QMapIterator<QString, KWAnchorPosition> itanch(m_anchorRequests);
    for(; itanch.hasNext(); itanch.next())
    {
        QString fsname = itanch.key();
        if ( m_pasteFramesetsMap && m_pasteFramesetsMap->contains( fsname ) )
            fsname = (*m_pasteFramesetsMap)[ fsname ];
        kDebug(32001) << "KWDocument::processAnchorRequests anchoring frameset " << fsname << endl;
        KWFrameSet * fs = frameSetByName( fsname );
        Q_ASSERT( fs );
        if ( fs )
            fs->setAnchored( itanch.value().textfs, itanch.value().paragId, itanch.value().index, true, false /*don't repaint yet*/ );
    }
    m_anchorRequests.clear();
}

bool KWDocument::processFootNoteRequests()
{
    bool ret = false;
    QMapIterator<QString, KWFootNoteVariable *> itvar(m_footnoteVarRequests);
    for ( ; itvar.hasNext(); itvar.next() )
    {
        QString fsname = itvar.key();
        if ( m_pasteFramesetsMap && m_pasteFramesetsMap->contains( fsname ) )
            fsname = (*m_pasteFramesetsMap)[ fsname ];
        //kDebug(32001) << "KWDocument::processFootNoteRequests binding footnote var " << itvar.value() << " and frameset " << fsname << endl;
        KWFrameSet * fs = frameSetByName( fsname );
        Q_ASSERT( fs );
        if ( !fs ) // #104431
            continue;
        Q_ASSERT( fs->type() == FT_TEXT );
        Q_ASSERT( fs->frameSetInfo() == KWFrameSet::FI_FOOTNOTE );
        KWFootNoteFrameSet* fnfs = dynamic_cast<KWFootNoteFrameSet *>(fs);
        if ( fnfs )
        {
            fnfs->setFootNoteVariable( itvar.value() );
            itvar.value()->setFrameSet( fnfs );
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
        //kDebug() << "pasteFrames: elem=" << elem.tagName() << endl;
        QDomElement frameElem;
        KWFrameSet * fs = 0L;
        if ( elem.tagName() == "FRAME" )
        {
            QString frameSetName = frameElem.attribute( "parentFrameset" );
            fs = frameSetByName( frameSetName );
            if ( !fs )
            {
                kWarning(32001) << "pasteFrames: Frameset '" << frameSetName << "' not found" << endl;
                continue;
            }
            frameElem = elem;
        }
        else if ( elem.tagName() == "FRAMESET" )
        {
            // Prepare a new name for the frameset
            QString oldName = elem.attribute( "name" );
            QString newName = uniqueFramesetName( oldName ); // make up a new name for the frameset

            m_pasteFramesetsMap->insert( oldName, newName ); // remember the name transformation
            if(oldName != newName)
                kDebug(32001) << "KWDocument::pasteFrames new frameset: " << oldName << "->" << newName << endl;
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
                    kDebug() << "KWDocument::pasteFrames created frameset: '" << newName << "'\n";
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
        //kWarning(32001) << "Unsupported toplevel-element in KWCanvas::pasteFrames : '" << elem.tagName() << "'" << endl;

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
                nb++;
                fs->addFrame( frame, false );
                if ( selectFrames ) {
                    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin();
                            it != m_lstViews.end(); ++it ) {
                        KWFrameView *fv = (*it)->frameViewManager()->view(frame);
                        if(fv)
                            fv->setSelected(true);
                    }
                }
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
                kError(32001) << "FT_CLIPART used! (in KWDocument::loadFrameSet)" << endl;
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
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->finalize();
    repaintAllViews();
    delete m_pasteFramesetsMap;
    m_pasteFramesetsMap = 0L;
}

void KWDocument::completeOasisPasting()
{
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
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
            kDebug()<<"KWDocument::insertEmbedded() Embedded object"<<endl;
            QDomElement object = elem.namedItem( "OBJECT" ).toElement();
            QDomElement settings = elem.namedItem( "SETTINGS" ).toElement();
            if ( object.isNull() || settings.isNull() )
            {
                kError() << "No <OBJECT> or <SETTINGS> tag" << endl;
            }
            else
            {
                KWDocumentChild *ch = new KWDocumentChild( this );
                kDebug()<<"KWDocument::insertEmbedded() loading document"<<endl;
                if ( ch->load( object, true ) )
                {
                    ch->loadDocument( store );
                    insertChild( ch );
                    QString oldName = settings.attribute( "name" );
                    QString newName = uniqueFramesetName( oldName );
                    m_pasteFramesetsMap->insert( oldName, newName ); // remember the name transformation
                    KWPartFrameSet *part = new KWPartFrameSet( this, ch, newName );
                    m_lstFrameSet.append( part );
                    kDebug() << "KWDocument::insertEmbedded loading embedded object" << endl;
                    part->load( settings );
                    if ( offset != 0 ) {
                        QRect r = ch->geometry();
                        r.moveBy( (int)offset, (int)offset );
                        ch->setGeometry( r );
                    }
                    part->setZOrder();
                    if ( macroCmd )
                    {
                        Q3PtrListIterator<KWFrame> frameIt( part->frameIterator() );
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
    Q3ValueList<KWFrameView*> noFrames;
    return saveOasisHelper( store, manifestWriter, SaveAll, noFrames);
}

// can't be const due to recalcVariables()
bool KWDocument::saveOasisHelper( KoStore* store, KoXmlWriter* manifestWriter, SaveFlag saveFlag, const Q3ValueList<KWFrameView*> &selectedFrames, QString* plainText, KoPicture* picture, KWTextFrameSet* fs) {
    m_pictureCollection->assignUniqueIds();
    fixZOrders();

    manifestWriter->addManifestEntry( "content.xml", "text/xml" );
    KoOasisStore oasisStore( store );

    KoXmlWriter* contentWriter = oasisStore.contentWriter();
    if ( !contentWriter )
        return false;

    Q3ValueList<KoPictureKey> pictureList;
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
    m_styleColl->saveOasis( mainStyles, KoGenStyle::STYLE_USER, savingContext );

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
        QBuffer buffer( &headerFooterContent );
        buffer.open( QIODevice::WriteOnly );
        KoXmlWriter headerFooterTmpWriter( &buffer );  // TODO pass indentation level
        Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
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
        Q3ValueList<KoGenStyles::NamedStyle> autoStyles = mainStyles.styles( KoGenStyle::STYLE_AUTO );
        for ( Q3ValueList<KoGenStyles::NamedStyle>::const_iterator it = autoStyles.begin();
              it != autoStyles.end(); ++it ) {
            mainStyles.markStyleForStylesXml( (*it).name );
        }
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
            const Q3ValueList<KWFrameSet *>& frameset = visitor.frameSets();
            kDebug(32001) << frameset.count() << " inline framesets" << endl;
            for ( Q3ValueList<KWFrameSet *>::ConstIterator it = frameset.begin(); it != frameset.end(); ++it )
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
        saveSelectedFrames( *bodyWriter, savingContext, pictureList,
                            selectedFrames, &newText ); // output vars
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

    //kDebug(32001) << "saveOasis: " << pictureList.count() << " pictures" << endl;
    m_pictureCollection->saveOasisToStore( store, pictureList, manifestWriter );

    if ( saveFlag == SaveSelected ) {
        // Save embedded objects - code inspired from KoDocument::saveChildrenOasis,
        // for the case where we're saving only some embedded objects, like with Ctrl+C.

        // IMPORTANT: This must be done *after* we're done with writing content.xml,
        // not while writing it (like in saveSelectedFrames).
        // We can't be writing to two files at the same time.

        Q3ValueList<KoDocumentChild*> embeddedObjects;
        Q3ValueListConstIterator<KWFrameView*> framesIterator = selectedFrames.begin();
        for(; framesIterator != selectedFrames.end(); ++framesIterator) {
            KWFrame *frame = (*framesIterator)->frame();
            KWFrameSet *fs = frame->frameSet();
            if ( fs->isVisible() && fs->type() == FT_PART) {
                embeddedObjects.append( static_cast<KWPartFrameSet *>(fs)->getChild() );
            }
        }

        Q3ValueList<KoDocumentChild *>::const_iterator chl = embeddedObjects.begin();
        for( ; chl != embeddedObjects.end(); ++chl ) {
            if ( !(*chl)->saveOasis( store, manifestWriter ) )
                return false;
        }
    }

    if ( saveFlag == SaveAll )
    {

        if(!store->open("settings.xml"))
            return false;

        KoStoreDevice contentDev( store );
        KoXmlWriter* settingsWriter = createOasisXmlWriter(&contentDev, "office:document-settings");
        saveOasisSettings( *settingsWriter );
        delete settingsWriter;

        if(!store->close())
            return false;

        manifestWriter->addManifestEntry("settings.xml", "text/xml");
    }
    return true;
}

// can't be const due to recalcVariables()
Q3DragObject* KWDocument::dragSelected( const Q3ValueList<KWFrameView*> &selectedFrames) {
    return dragSelectedPrivate(0, selectedFrames, 0);
}
// can't be const due to recalcVariables()
Q3DragObject* KWDocument::dragSelected( QWidget *parent, KWTextFrameSet* fs) {
    Q3ValueList<KWFrameView*> noFrames;
    return dragSelectedPrivate(parent, noFrames, fs);
}
// can't be const due to recalcVariables()
Q3DragObject* KWDocument::dragSelectedPrivate( QWidget *parent, const Q3ValueList<KWFrameView*> &selectedFrames, KWTextFrameSet* fs)
{
    // We'll create a store (ZIP format) in memory
    QBuffer buffer;
    QByteArray mimeType = KWOasisSaver::selectionMimeType();
    KoStore* store = KoStore::createStore( &buffer, KoStore::Write, mimeType );
    Q_ASSERT( store );
    Q_ASSERT( !store->bad() );
    KoOasisStore oasisStore( store );

    KoXmlWriter* manifestWriter = oasisStore.manifestWriter( mimeType );

    QString plainText;
    KoPicture picture;
    if ( !saveOasisHelper( store, manifestWriter, KWDocument::SaveSelected, selectedFrames, &plainText, &picture, fs )
         || !oasisStore.closeManifestWriter() )
    {
        delete store;
        return 0;
    }

    delete store;
/* removed because I have no idea what the replacement of KMultipleDrag is...
    KMultipleDrag* multiDrag = new KMultipleDrag( parent );
    if ( !plainText.isEmpty() )
        multiDrag->addDragObject( new Q3TextDrag( plainText, 0 ) );
    if ( !picture.isNull() )
        multiDrag->addDragObject( picture.dragObject( 0 ) );
    KoStoreDrag* storeDrag = new KoStoreDrag( KWOasisSaver::selectionMimeType(), 0 );
    kDebug() << k_funcinfo << "setting zip data: " << buffer.buffer().size() << " bytes." << endl;
    storeDrag->setEncodedData( buffer.buffer() );
    multiDrag->addDragObject( storeDrag );
    return multiDrag;
*/
return 0;
}

void KWDocument::saveSelectedFrames( KoXmlWriter& bodyWriter, KoSavingContext& savingContext, Q3ValueList<KoPictureKey>& pictureList, const Q3ValueList<KWFrameView*> &selectedFrames, QString* plainText ) const {
    Q3PtrList<KoDocumentChild> embeddedObjects;
    Q3ValueListConstIterator<KWFrameView*> framesIterator = selectedFrames.begin();
    for(; framesIterator != selectedFrames.end(); ++framesIterator) {
        KWFrame *frame = (*framesIterator)->frame();
        KWFrameSet *fs = frame->frameSet();
        if ( fs->isVisible() && fs->type() == FT_PART) {
            embeddedObjects.append( static_cast<KWPartFrameSet *>(fs)->getChild() );
        }
        bool isTable = fs->type() == FT_TABLE;

        // Two cases to be distinguished here
        // If it's the first frame of a frameset, then copy the frameset contents and the frame itself
        // Otherwise copy only the frame information
        if ( frame == fs->frame(0) || isTable ) {
            fs->saveOasis( bodyWriter, savingContext, false );
            if ( plainText )
                *plainText += fs->toPlainText();
        }
        else if ( !isTable ) {
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
            kDebug(32001) << "found non-inline picture framesets" << endl;

            const KoPictureKey key = static_cast<KWPictureFrameSet *>( fs )->key();
            if ( !pictureList.contains( key ) )
                pictureList.append( key );
        }
        if ( isTable ) // Copy tables only once, even if they have many cells selected
            break;
    }
}

void KWDocument::saveOasisSettings( KoXmlWriter& settingsWriter ) const
{
    settingsWriter.startElement("office:settings");
    settingsWriter.startElement("config:config-item-set");

    settingsWriter.addAttribute("config:name", "view-settings");

    KoUnit::saveOasis(&settingsWriter, unit());

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
    if ( saveFlag == SaveAll )
    {
        m_frameStyleColl->saveOasis( mainStyles, savingContext );
        m_tableStyleColl->saveOasis( mainStyles, savingContext );
    }

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

    Q3ValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( KoGenStyle::STYLE_USER );
    Q3ValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }
    styles = mainStyles.styles( KWDocument::STYLE_FRAME_USER );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "style:style", (*it).name , "style:graphic-properties"  );
    }
    styles = mainStyles.styles( KWDocument::STYLE_TABLE_CELL_USER );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "style:style", (*it).name , "style:table-cell-properties"  );
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

        if ( m_processingType == WP )
        {
            KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet *>( m_lstFrameSet.getFirst() );
            if ( frameset ) {
                frameset->frame(0)->saveBorderProperties( pageLayout );
            }
        }

        QBuffer buffer;
        buffer.open( QIODevice::WriteOnly );
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
        footnoteSepTmpWriter.addAttribute( "style:rel-width", QString::number( footNoteSeparatorLineLength() ) + '%' );
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
			QByteArray tmp;
            buffer.setBuffer( &tmp ); // clear data
            buffer.open( QIODevice::WriteOnly );
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
        Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
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
                hfStyle.addPropertyPt( "fo:min-height", frame->minimumFrameHeight() );
                frame->saveBorderProperties( hfStyle );
                frame->saveMarginProperties( hfStyle );
                ...
#endif
                stylesWriter->startElement( "style:header-footer-properties" );
                stylesWriter->addAttributePt( "fo:min-height", frame->minimumFrameHeight() );
                if ( fs->isAHeader() )
                    stylesWriter->addAttributePt( "fo:margin-bottom", m_pageHeaderFooter.ptHeaderBodySpacing );
                else
                    stylesWriter->addAttributePt( "fo:margin-top", m_pageHeaderFooter.ptFooterBodySpacing );
                // TODO frame->saveBorderAttributes( *stylesWriter );
                // Interesting idea, but we can't set margins (runaround) on
                //frame->saveMarginAttributes( *stylesWriter );
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
    Q3PtrListIterator<KoVariable> it( m_varColl->getVariables() );
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

        // Write out the non-inline framesets first; OOo wants it that way...
        Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
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

        // Write out the main text frameset's contents
        KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet *>( m_lstFrameSet.getFirst() );
        if ( frameset ) {
            frameset->saveOasisContent( writer, context );
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
        Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
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

    if( !m_bookmarkList->isEmpty() )
    {
        QDomElement bookmark = doc.createElement( "BOOKMARKS" );
        kwdoc.appendChild( bookmark );

        for ( KoTextBookmarkList::const_iterator it = m_bookmarkList->begin();
              it != m_bookmarkList->end() ; ++it )
        {
            const KoTextBookmark& book = *it;
            KWTextFrameSet* fs = static_cast<KWTextDocument*>(book.textDocument())->textFrameSet();
            if ( book.startParag() &&
                 book.endParag() &&
                 fs && !fs->isDeleted() )
            {
                QDomElement bookElem = doc.createElement( "BOOKMARKITEM" );
                bookmark.appendChild( bookElem );
                bookElem.setAttribute( "name", book.bookmarkName() );
                bookElem.setAttribute( "frameset", fs->name() );
                bookElem.setAttribute( "startparag", book.startParag()->paragId() );
                bookElem.setAttribute( "endparag", book.endParag()->paragId() );

                bookElem.setAttribute( "cursorIndexStart", book.bookmarkStartIndex() );
                bookElem.setAttribute( "cursorIndexEnd", book.bookmarkEndIndex() );
            }
        }
    }
    variableCollection()->variableSetting()->save(kwdoc );

    QDomElement framesets = doc.createElement( "FRAMESETS" );
    kwdoc.appendChild( framesets );

    m_textImageRequests.clear(); // for KWTextImage
    Q3ValueList<KoPictureKey> savePictures;

    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameSet = fit.current();
        // Save non-part framesets ( part are saved further down )
        if ( frameSet->type() != FT_PART )
            frameSet->save( framesets );

        // If picture frameset, make a note of the image it needs.
        if ( !frameSet->isDeleted() && ( frameSet->type() == FT_PICTURE ) )
        {
            KoPictureKey key = static_cast<KWPictureFrameSet *>( frameSet )->key();
            if ( !savePictures.contains( key ) )
                savePictures.append( key );
        }
    }

    // Process the data of the KWTextImage classes.
    Q3PtrListIterator<KWTextImage> textIt ( m_textImageRequests );
    for ( ; textIt.current() ; ++textIt )
    {
        KoPictureKey key = textIt.current()->getKey();
        kDebug(32001) << "KWDocument::saveXML registering text image " << key.toString() << endl;
        if ( !savePictures.contains( key ) )
            savePictures.append( key );
    }

    QDomElement styles = doc.createElement( "STYLES" );
    kwdoc.appendChild( styles );
    Q3ValueList<KoUserStyle *> styleList(m_styleColl->styleList());
    for ( Q3ValueList<KoUserStyle *>::const_iterator it = styleList.begin(), end = styleList.end();
          it != end ; ++it )
        saveStyle( static_cast<KoParagStyle *>( *it ), styles );

    QDomElement frameStyles = doc.createElement( "FRAMESTYLES" );
    kwdoc.appendChild( frameStyles );
    Q3ValueList<KoUserStyle *> frameStyleList(m_frameStyleColl->styleList());
    for ( Q3ValueList<KoUserStyle *>::const_iterator it = frameStyleList.begin(), end = frameStyleList.end();
          it != end ; ++it )
        saveFrameStyle( static_cast<KWFrameStyle *>(*it), frameStyles );

    QDomElement tableStyles = doc.createElement( "TABLESTYLES" );
    kwdoc.appendChild( tableStyles );
    Q3ValueList<KoUserStyle *> tableStyleList(m_tableStyleColl->styleList());
    for ( Q3ValueList<KoUserStyle *>::const_iterator it = tableStyleList.begin(), end = tableStyleList.end();
          it != end ; ++it )
        saveTableStyle( static_cast<KWTableStyle *>(*it), tableStyles );

    QDomElement pictures = m_pictureCollection->saveXML( KoPictureCollection::CollectionPicture, doc, savePictures );
    kwdoc.appendChild( pictures );

    // Not needed anymore
#if 0
    // Write out the list of parags (id) that form the table of contents, see KWContents::createContents
    if ( contents->hasContents() ) {
        QDomElement cParags = doc.createElement( "CPARAGS" );
        kwdoc.appendChild( cParags );
        Q3ValueList<int>::Iterator it = contents->begin();
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
void KWDocument::saveEmbeddedObjects( QDomElement& parentElem, const Q3PtrList<KoDocumentChild>& childList )
{
    // Write "OBJECT" tag for every child, appending "EMBEDDING" tags to the main XML
    Q3PtrListIterator<KoDocumentChild> chl( childList );
    QDomDocument doc = parentElem.ownerDocument();
    for( ; chl.current(); ++chl ) {
        KWDocumentChild* curr = static_cast<KWDocumentChild*>(chl.current());
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


Q3ValueList<KoPictureKey> KWDocument::savePictureList()
{
    Q3ValueList<KoPictureKey> savePictures;

    // At first, we must process the data of the KWTextImage classes.
    // Process the data of the KWTextImage classes.
    Q3PtrListIterator<KWTextImage> textIt ( m_textImageRequests );
    for ( ; textIt.current() ; ++textIt )
    {
        KoPictureKey key = textIt.current()->getKey();
        kDebug(32001) << "KWDocument::saveXML registering text image " << key.toString() << endl;
        if ( !savePictures.contains( key ) )
            savePictures.append( key );
    }
    m_textImageRequests.clear(); // Save some memory!

    // Now do the images/cliparts in frames.
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
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
bool KWDocument::completeSaving( KoStore *store )
{
    if ( !store )
        return true;

    QString u = KUrl( url() ).path();

    Q3ValueList<KoPictureKey> savePictures( savePictureList() );

    return m_pictureCollection->saveToStore( KoPictureCollection::CollectionPicture, store, savePictures );
}

int KWDocument::supportedSpecialFormats() const
{
    return KoDocument::supportedSpecialFormats();
}

void KWDocument::addView( KoView *view )
{
    m_lstViews.append( (KWView*)view );
    KoDocument::addView( view );
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it ) {
        (*it)->deselectAllFrames();
    }
}

void KWDocument::removeView( KoView *view )
{
    m_lstViews.remove( static_cast<KWView*>(view) );
    KoDocument::removeView( view );
}

void KWDocument::addShell( KoMainWindow *shell )
{
    connect( shell, SIGNAL( documentSaved() ), m_commandHistory, SLOT( documentSaved() ) );
    connect( shell, SIGNAL( saveDialogShown() ), this, SLOT( saveDialogShown() ) );
    KoDocument::addShell( shell );
}

KoView* KWDocument::createViewInstance( QWidget* parent, const char* name )
{
    if ( isEmbedded() )
        return new KWView( "ModeEmbedded", parent, name, this );
    else
        return new KWView( m_viewModeType, parent, name, this );
}

// Paint this document when it's embedded
// This is also used to paint the preview.png that goes into the ZIP file
void KWDocument::paintContent( QPainter& painter, const QRect& rectangle, bool transparent, double zoomX, double zoomY )
{
    //kDebug(32001) << "KWDocument::paintContent m_zoom=" << m_zoom << " zoomX=" << zoomX << " zoomY=" << zoomY << " transparent=" << transparent << " rectangle=" << rectangle << endl;
    Q_ASSERT( zoomX != 0 );
    Q_ASSERT( zoomY != 0 );

    setZoom( 100 );
    m_zoomMode = KoZoomMode::ZOOM_CONSTANT;

    // The caller doesn't care about DPI, that's our own internal zooming done on top of it:
    zoomX *= POINT_TO_INCH( static_cast<double>( KoGlobal::dpiX() ) );
    zoomY *= POINT_TO_INCH( static_cast<double>( KoGlobal::dpiY() ) );

    if ( m_zoomedResolutionX != zoomX || m_zoomedResolutionY != zoomY )
    {
        //kDebug(32001) << "m_zoomedResolutionX=" << m_zoomedResolutionX << " != " << zoomX << " -> calling setResolution(" << zoomX << ")" << endl;
        int zoomLevel = qRound( 100 * zoomY / m_zoomedResolutionY ); // ## ignores the case where the x and y scaling differs
        setZoom( zoomLevel );
        bool forPrint = painter.device() && painter.device()->devType() == QInternal::Printer;
        newZoomAndResolution( false, forPrint );
        if ( KFormula::Document* formulaDocument = m_formulaDocumentWrapper->document() )
            formulaDocument->setZoomAndResolution( zoomLevel, zoomX, zoomY, false, forPrint );
        // Note that this zoom and resolution are then used when activating the embedded object!
    }

    QRect rect( rectangle );
    painter.save();
    painter.translate( rect.x(), rect.y() );
    QRect clipRect( 0, 0, rect.width(), rect.height() );

    KWViewModeEmbedded * viewMode = new KWViewModeEmbedded( this, 0 /*no canvas*/ );
    viewMode->setDrawFrameBackground( !transparent );
    viewMode->setDrawSelections( false );

    QColorGroup cg = QApplication::palette().active();

    if (!transparent)
    {
        QRegion emptyRegion( rect );
        createEmptyRegion( rect, emptyRegion, viewMode );
        eraseEmptySpace( &painter, emptyRegion, cg.brush( QColorGroup::Base ) );
    }

    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameset = fit.current();
        if ( frameset->isVisible( viewMode ) && !frameset->isFloating() )
            frameset->drawContents( &painter, clipRect, cg,
                                    false /*onlyChanged*/, true /*resetChanged*/,
                                    0L, viewMode, 0 );
    }
    delete viewMode;

    painter.restore();
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
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it ) {
        (*it)->getGUI()->canvasWidget()->setUpdatesEnabled( false );
    }
    Q_ASSERT( !m_bGeneratingPreview );
    m_bGeneratingPreview = true;
    QPixmap pix = KoDocument::generatePreview(size);

    // Restore everything as it was before
    setResolution( oldResolutionX, oldResolutionY );
    setZoom( oldZoom );

    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it ) {
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
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
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
    painter->setClipRegion( emptySpaceRegion );
    painter->setPen( Qt::NoPen );

    //kDebug(32001) << "KWDocument::eraseEmptySpace emptySpaceRegion: " << emptySpaceRegion << endl;
    //kDebug(32001) << "                            boundingRect: " << DEBUGRECT( emptySpaceRegion.boundingRect() ) << endl;
    painter->fillRect( emptySpaceRegion.boundingRect(), brush );
    painter->restore();
}

KWDocumentChild* KWDocument::createChildDoc( const KoRect& rect, KoDocument* childDoc )
{
    KWDocumentChild* ch = new KWDocumentChild( this, rect.toQRect(), childDoc );
    insertChild( ch );
    return ch;
}

KWPartFrameSet* KWDocument::insertObject( const KoRect& rect, KoDocumentEntry& e, QWidget* parentWidget )
{
    QString errorMsg;
    KoDocument* doc = e.createDoc( &errorMsg, this );
    if ( !doc )
        return 0;
    if ( !doc->showEmbedInitDialog( parentWidget )  )
        return 0;

    KWDocumentChild* ch = createChildDoc( rect, doc );
    setModified( true );

    KWPartFrameSet *frameset = new KWPartFrameSet( this, ch, QString::null );
    KWFrame *frame = new KWFrame(frameset, rect.x(), rect.y(), rect.width(), rect.height() );
    frame->setZOrder( maxZOrder( frame->pageNumber(this) ) + 1 ); // make sure it's on top
    frameset->addFrame( frame );
    addFrameSet( frameset );

    KWCreateFrameCommand *cmd = new KWCreateFrameCommand( i18n("Create Part Frame"), frame);
    addCommand(cmd);

    frameChanged( frame ); // repaint etc.

    return frameset;
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
    //kDebug() << k_funcinfo << fromPage << endl;
    if ( m_recalcFramesPending == -1 || fromPage < m_recalcFramesPending )
    {
        m_recalcFramesPending = fromPage;
        QTimer::singleShot( 0, this, SLOT( slotRecalcFrames() ) );
    }
}

void KWDocument::slotRecalcFrames() {
    int from = m_recalcFramesPending;
    kDebug() << k_funcinfo << "from=" << from << endl;
    m_recalcFramesPending = -1;
    if ( from != -1 )
        recalcFrames( from );
}

void KWDocument::repaintAllViewsExcept( KWView *view, bool erase )
{
    //kDebug(32001) << "KWDocument::repaintAllViewsExcept" << endl;
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it ) {
        KWView* viewPtr = *it;
        if ( viewPtr != view /*&& viewPtr->getGUI() && viewPtr->getGUI()->canvasWidget()*/ ) {
            viewPtr->getGUI()->canvasWidget()->repaintAll( erase );
        }
    }
}

void KWDocument::updateAllStyleLists()
{
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->updateStyleList();
}

void KWDocument::updateStyleListOrder( const QStringList &list )
{
    styleCollection()->updateStyleListOrder( list );
}

void KWDocument::applyStyleChange( KoStyleChangeDefMap changed )
{
    Q3PtrList<KWTextFrameSet> textFramesets = allTextFramesets( true );

    KWTextFrameSet *frm;
    for ( frm=textFramesets.first(); frm != 0; frm=textFramesets.next() ){
        frm->applyStyleChange( changed );
    }
}

void KWDocument::updateAllFrameStyleLists()
{
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->updateFrameStyleList();
}

void KWDocument::updateAllTableStyleLists()
{
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->updateTableStyleList();
}

void KWDocument::repaintAllViews( bool erase )
{
    //kDebug(32001) << "KWDocument::repaintAllViews" << endl;
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->getGUI()->canvasWidget()->repaintAll( erase );
}

Q3PtrList<KWFrame> KWDocument::framesToCopyOnNewPage( int afterPageNum ) const {
    // afterPageNum can be -1 for 'before page 1'

    // Look at frames on pages afterPageNum and afterPageNum-1 (for sheetside stuff)
    Q3PtrList<KWFrame> framesToLookAt;
    if ( afterPageNum >= startPage() )
        framesToLookAt = framesInPage( afterPageNum, false );

    if ( afterPageNum >= startPage() + 1 )
    {
        Q3PtrList<KWFrame> framesToAlsoLookAt = framesInPage( afterPageNum-1, false ); // order doesn't matter

        // Merge into single list. Other alternative, two loops, code inside moved to another method.
        Q3PtrListIterator<KWFrame> frameAlsoIt( framesToAlsoLookAt );
        for ( ; frameAlsoIt.current(); ++frameAlsoIt )
            framesToLookAt.append( frameAlsoIt.current() );
    }

    Q3PtrList<KWFrame> framesToCopy; // the result

    Q3PtrListIterator<KWFrame> frameIt( framesToLookAt );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame * frame = frameIt.current();
        KWFrameSet* frameSet = frame->frameSet();

        // don't add tables! A table cell ( frameset ) _must_ not have cells auto-added to them!
        if ( frameSet->type() == FT_TABLE ) continue;

        // NewFrameBehavior == Copy is handled here except for headers/footers, which
        // are created in recalcFrames()
        if(frameSet->isAHeader() || frameSet->isAFooter()) continue;

#ifdef DEBUG_PAGES
        kDebug(32002) << "KWDocument::framesToCopyOnNewPage looking at frame " << frame << ", pageNum=" << frame->pageNumber() << " from " << frameSet->name() << endl;
        static const char * newFrameBh[] = { "Reconnect", "NoFollowup", "Copy" };
        kDebug(32002) << "   frame->newFrameBehavior()==" << newFrameBh[frame->newFrameBehavior()] << endl;
#endif
        const int frameIsOnPage = frame->pageNumber();
        if (frame->newFrameBehavior() == KWFrame::Copy &&
                (frameIsOnPage == afterPageNum && frame->sheetSide() == KWFrame::AnySide ||
                 frameIsOnPage == afterPageNum -1 && frame->sheetSide() != KWFrame::AnySide))
            framesToCopy.append( frame );
    }
    return framesToCopy;
}

KWPage* KWDocument::insertPage( int afterPageNum ) // can be -1 for 'before page 0'
{
#ifdef DEBUG_PAGES
    kDebug(32002) << "insertPage: afterPageNum=" << afterPageNum << endl;
#endif
    if ( processingType() == WP )
        Q_ASSERT( afterPageNum == lastPage() ); // WP mode: can only append.

    double pageHeight = pageManager()->page( afterPageNum )->height();
    // If not appending, move down everything after 'afterPageNum', to make room.
    for ( int pg = pageCount () -1 ; pg > afterPageNum ; --pg )
    {
        // pg is the 'src' page. Its contents must be moved to the page pg+1
        Q3PtrList<KWFrame> frames = framesInPage( pg, false );
#ifdef DEBUG_PAGES
        kDebug(32002) << "insertPage: moving " << frames.count() << " frames down, from page " << pg << endl;
#endif
        Q3PtrListIterator<KWFrame> frameIt( frames );
        for ( ; frameIt.current(); ++frameIt )
            frameIt.current()->moveBy( 0, pageHeight );
    }

    KWPage *page = pageManager()->insertPage(afterPageNum+1);

    // Fill in the new page
    Q3PtrList<KWFrame> framesToCopy = framesToCopyOnNewPage( afterPageNum );
    Q3PtrListIterator<KWFrame> frameIt( framesToCopy );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame * frame = frameIt.current();

        KWFrame *newFrame = frame->getCopy();
        newFrame->moveBy( 0, pageHeight );
        frame->frameSet()->addFrame( newFrame );

        if ( frame->newFrameBehavior()==KWFrame::Copy )
            newFrame->setCopy( true );
        //kDebug(32002) << "   => created frame " << newFrame << " " << *newFrame << endl;
    }
    return page;
}

KWPage* KWDocument::appendPage()
{
#ifdef DEBUG_PAGES
    kDebug(32002) << "KWDocument::appendPage pageCount()=" << pageCount() << " -> insertPage(" << lastPage() << ")" << endl;
#endif
    return insertPage( lastPage() );
}

void KWDocument::afterInsertPage( int pageNum )
{
#ifdef DEBUG_PAGES
    kDebug(32002) << "KWDocument::afterInsertPage " << pageNum << endl;
#endif
    if ( !m_bGeneratingPreview )
        emit newContentsSize();

    // Get headers and footers on the new page
    // This shouldn't delete the newly created page because it's still empty though
    recalcFrames( pageNum, -1, KWFrameLayout::DontRemovePages );
    // Take into account the frames on the new page, and run updateFramesOnTopOrBelow (#73819)
    updateAllFrames();

    recalcVariables( VT_PGNUM );
    emit numPagesChanged();
    if ( m_viewModeType == "ModePreview" )
        repaintAllViews();
}

bool KWDocument::canRemovePage( int num )
{
kDebug() << "KWDocument::canRemovePage " << num<< endl;
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameSet = fit.current();
        if ( frameSet->isHeaderOrFooter() ) // don't look at headers/footers, but look at footnotes/endnotes
            continue;
        if ( frameSet->isVisible() && !frameSet->canRemovePage( num ) )
            return false;
    }
#ifdef DEBUG_PAGES
    kDebug(32002) << "KWDocument::canRemovePage " << num << "-> TRUE" << endl;
#endif
    return true;
}

void KWDocument::removePage( int pageNum )
{
    if ( processingType() == WP )
        Q_ASSERT( pageNum == lastPage() ); // WP mode: can only remove last page.
    Q_ASSERT( pageCount() > 1 );
    if ( pageCount() == 1 )
        return;

    // ## This assumes that framesInPage is up-to-date.
    Q3PtrList<KWFrame> framesToDelete = framesInPage( pageNum, false );
#ifdef DEBUG_PAGES
    kDebug(32002) << "KWDocument::removePage " << pageNum << ", " << framesToDelete.count() << " frames to delete" << endl;
#endif
    Q3PtrListIterator<KWFrame> frameIt( framesToDelete );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame * frame = frameIt.current();
        KWFrameSet * frameSet = frame->frameSet();
        if ( frameSet->frameSetInfo() != KWFrameSet::FI_BODY )
            continue;
        frameSet->deleteFrame( frame, true );
    }

    // If not removing the last one, move up everything after the one we removed.
    for ( int pg = pageNum+1 ; pg < pageCount() ; ++pg )
    {
        // pg is the 'src' page. Its contents must be moved to the page pg-1
        Q3PtrList<KWFrame> frames = framesInPage( pg, false );
#ifdef DEBUG_PAGES
        kDebug(32002) << "removePage: moving " << frames.count() << " frames up, from page " << pg << endl;
#endif
        Q3PtrListIterator<KWFrame> frameIt( frames );
        for ( ; frameIt.current(); ++frameIt )
            frameIt.current()->moveBy( 0, pageManager()->page(0)->height() );
    }

    pageManager()->removePage(pageNum);
#ifdef DEBUG_PAGES
    kDebug(32002) << "KWDocument::removePage -- -> " << pageCount() << endl;
#endif
    // Emitting this one for each page being removed helps giving the user some feedback
    emit numPagesChanged();
}

void KWDocument::afterRemovePages()
{
    //### IMHO recalcFrames should take care of updateAllFrames (it already does it partially).
    recalcFrames();
    // Do this before recalcVariables (which repaints). The removed frames must be removed from the frame caches.
    // We don't call updateAllFrames() directly, because it still calls
    // updateFramesOnTopOrBelow, which is useless (and slow) here.
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->updateFrames();

    recalcVariables( VT_PGNUM );
    if ( !m_bGeneratingPreview )
        emit newContentsSize();
    if ( m_viewModeType == "ModePreview" )
        repaintAllViews();
}

bool KWDocument::tryRemovingPages()
{
    int last = lastPage();
    bool removed = false;
    // Last frame is empty -> try removing last page, and more if necessary
    while ( last > startPage() && canRemovePage( last ) )
    {
        removePage( last ); // this modifies pageCount
        if ( last <= lastPage() )
        {
            kWarning() << "Didn't manage to remove page " << last << " (still having " << pageCount() << " pages ). Aborting" << endl;
            break;
        }
        removed = true;
        last = lastPage();
    }
    // Don't call afterRemovePages or recalcFrames from here, since this method is
    // itself called from KWFrameLayout (#95047)
    return removed;
}


KWFrameSet * KWDocument::frameSetByName( const QString & name )
{
    // Note: this isn't recursive, so it won't find table cells.
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        if ( fit.current()->name() == name )
            return fit.current();
    return 0L;
}

//#define DEBUG_FRAMESELECT

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

void KWDocument::fixZOrders() {
    //KWFrame *frameFixed = 0;
    for (int pgnum = startPage() ; pgnum <= lastPage() ; pgnum++) {
        Q3PtrList<KWFrame> frames = framesInPage(pgnum);
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
            kDebug() << "fixing page " << pgnum << " z-orders " << endl;
            for (KWFrame *fr = frames.first();fr;fr=frames.next()) {
                // only consider non-inline framesets.
                if (fr->frameSet()->isFloating())
                    continue;
                current_zorder++;
                fr->setZOrder(current_zorder);
                //frameFixed = f;
            }
        }

        if ( m_processingType == KWDocument::WP )
        {
            // In all cases, ensure the main frames are below the rest.
            // (This could not be the case after e.g. an import filter does it wrong)
            lowerMainFrames( pgnum );
        }
    }
    //if ( frameFixed )
    //    frameFixed->frameStack()->recalcAllFrames();
    KWFrameList::recalcAllFrames(this);
}

void KWDocument::lowerMainFrames( int pageNum )
{
    Q3PtrList<KWFrame> framesInPage = this->framesInPage(pageNum);
    int lowestZOrder=10000;
    for ( Q3PtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt )
        lowestZOrder=qMin(lowestZOrder, frameIt.current()->zOrder());
    lowerMainFrames( pageNum, lowestZOrder );
}

// separated from the above one for KWView (which knows lowestZOrder already)
void KWDocument::lowerMainFrames( int pageNum, int lowestZOrder )
{
    // Get the main frameset and see if we have to lower its frame(s).
    Q3PtrList<KWFrame> framesInPage = this->framesInPage(pageNum);
    for ( Q3PtrListIterator<KWFrame> frameIt( framesInPage ); frameIt.current(); ++frameIt ) {
        if(frameIt.current()->frameSet()->isMainFrameset()) {
            if(lowestZOrder <= frameIt.current()->zOrder())
                frameIt.current()->setZOrder(lowestZOrder-1);
            // keep going, in case of multiple columns
        }
    }
}

Q3PtrList<KWFrame> KWDocument::framesInPage( int pageNum, bool sorted ) const {

    ZOrderedFrameList frames;
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameSet = fit.current();
        if ( !frameSet->isVisible() )
            continue;
        // Append all frames from frameSet in page pageNum
        Q3PtrListIterator<KWFrame> it( frameSet->framesInPage( pageNum ) );
        for ( ; it.current() ; ++it )
            frames.append( it.current() );
    }
    if (sorted) frames.sort();
    return frames;
}

void KWDocument::updateAllFrames( int flags )
{
#ifdef DEBUG_SPEED
    QTime dt;
    dt.start();
#endif
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->updateFrames( flags );

#ifdef DEBUG_SPEED
    kDebug(32001) << "updateAllFrames(" << flags << ") took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;
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
void KWDocument::frameChanged( KWFrame * frame )
{
    if(! m_framesChangedHandler) {
        m_framesChangedHandler = new FramesChangedHandler(this);
        QTimer::singleShot( 0, this, SLOT( updateFramesChanged() ) );
    }
    m_framesChangedHandler->addFrame(frame);
}

void KWDocument::framesChanged( const Q3PtrList<KWFrame> & frames, KWView * view )
{
    Q_UNUSED( view ); // DF: seems my idea of excluding one view got thrown away
    Q3PtrListIterator<KWFrame> it( frames );
    for ( ; it.current() ; ++it )
        frameChanged(it.current());
}

void KWDocument::updateFramesChanged() { // slot called from frameChanged()
    if(!m_framesChangedHandler) return;
    m_framesChangedHandler->execute();
    delete m_framesChangedHandler;
    m_framesChangedHandler = 0;
}

void KWDocument::framesChanged( const Q3ValueList<KWFrame*> &frames) {
    Q3ValueListConstIterator<KWFrame*> framesIterator = frames.begin();
    for(;framesIterator != frames.end(); ++framesIterator)
        frameChanged(*framesIterator);
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
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
    {
        (*it)->updateHeaderFooterButton();
        (*it)->updateHeader();
    }
}

void KWDocument::updateFooterButton()
{
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
    {
        (*it)->updateHeaderFooterButton();
        (*it)->updateFooter();
    }
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
    const Q3ValueList<KoVariable *> modifiedVariables = m_varColl->recalcVariables(type);
    if ( m_bGeneratingPreview )
        return;

    QMap<KoTextDocument *, bool> modifiedTextDocuments; // Qt4: QSet
    for ( Q3ValueList<KoVariable *>::const_iterator it = modifiedVariables.begin(), end = modifiedVariables.end() ; it != end ; ++it ) {
        KoTextDocument* textdoc = (*it)->textDocument();
        if ( modifiedTextDocuments.find( textdoc ) != modifiedTextDocuments.end() ) // Qt4: !contains
        {
            modifiedTextDocuments.insert( textdoc, true );
            KWTextFrameSet * textfs = static_cast<KWTextDocument *>(textdoc)->textFrameSet();
            slotRepaintChanged( textfs );
        }
    }
}

int KWDocument::mailMergeRecord() const
{
    return slRecordNum;
}

void KWDocument::setMailMergeRecord( int r )
{
    slRecordNum = r;
}

void KWDocument::getPageLayout( KoPageLayout& layout, KoColumns& cl, KoKWHeaderFooter& hf )
{
    layout = m_pageLayout;
    cl = m_pageColumns;
    hf = m_pageHeaderFooter;
}

void KWDocument::addFrameSet( KWFrameSet *f, bool finalize /*= true*/ )
{
    if(m_lstFrameSet.contains(f) > 0) {
        kWarning(32001) << "Frameset " << f << " " << f->name() << " already in list!" << endl;
        return;
    }
    m_lstFrameSet.append(f);

    KWFrameList::createFrameList(f, this);

    if ( finalize )
        f->finalize();
    setModified( true );
    emit sigFrameSetAdded(f);
}

void KWDocument::removeFrameSet( KWFrameSet *f )
{
    emit sig_terminateEditing( f );
    m_lstFrameSet.take( m_lstFrameSet.find(f) );
    setModified( true );
    emit sigFrameSetRemoved(f);
}

void KWDocument::addCommand( KCommand * cmd )
{
    Q_ASSERT( cmd );
    //kDebug(32001) << "KWDocument::addCommand " << cmd->name() << endl;
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
    kDebug() << "----------------------------------------"<<endl;
    m_styleColl->printDebug();
    kDebug() << m_frameStyleColl->count() << " frame styles" << endl;
    kDebug() << m_tableStyleColl->count() << " table-cell styles" << endl;
}

void KWDocument::printDebug()
{
    kDebug() << "----------------------------------------"<<endl;
    kDebug() << "                 Debug info"<<endl;
    kDebug() << "Document:" << this <<endl;
    kDebug() << "Type of document: (0=WP, 1=DTP) " << processingType() <<endl;
    kDebug() << "Header visible: " << isHeaderVisible() << endl;
    kDebug() << "Footer visible: " << isFooterVisible() << endl;
    kDebug() << "Units: " << unit() <<endl;
    kDebug() << "# Framesets: " << frameSetCount() <<endl;
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( unsigned int iFrameset = 0; fit.current() ; ++fit, iFrameset++ )
    {
        KWFrameSet * frameset = fit.current();
        kDebug() << "Frameset " << iFrameset << ": '" <<
            frameset->name() << "' (" << frameset << ")" << (frameset->isDeleted()?" Deleted":"")<<endl;
        if ( frameset->isVisible())
            frameset->printDebug();
        else
            kDebug() << "  [hidden] #" << frameset->frameCount() << " frames" << endl;
    }

    for ( uint pgNum = 0 ; pgNum < m_sectionTitles.size() ; ++pgNum ) {
        kDebug() << "Page " << pgNum << "  Section: '" << m_sectionTitles[ pgNum ] << "'"<< endl;
    }
    /*
    kDebug() << "# Images: " << getImageCollection()->iterator().count() <<endl;
    QDictIterator<KWImage> it( getImageCollection()->iterator() );
    while ( it.current() ) {
        kDebug() << " + " << it.current()->getFilename() << ": "<<it.current()->refCount() <<endl;
        ++it;
    }
    */

    kDebug() << "PageManager holds "<< pageCount() << " pages in the range: " << startPage() <<
        "-" << lastPage() << endl;
    for (int pgnum = startPage() ; pgnum <= lastPage() ; pgnum++) {
        KWPage *page = pageManager()->page(pgnum);
        kDebug() << "Page " << pgnum << " width=" << page->width() << " height=" << page->height() << endl;
    }
    kDebug() << "  The height of the doc (in pt) is: " << pageManager()->
        bottomOfPage(lastPage()) << endl;
}
#endif

void KWDocument::layout()
{
    Q3PtrListIterator<KWFrameSet> it = framesetsIterator();
    for (; it.current(); ++it )
        if ( it.current()->isVisible() )
            it.current()->layout();
}

void KWDocument::invalidate(const KWFrameSet *skipThisFrameSet)
{
    Q3PtrListIterator<KWFrameSet> it = framesetsIterator();
    for (; it.current(); ++it )
        if(it.current()!=skipThisFrameSet)
            it.current()->invalidate();
}

KFormula::Document* KWDocument::formulaDocument( bool init )
{
    KFormula::Document* formulaDocument = m_formulaDocumentWrapper->document();
    if (!formulaDocument) {
        kDebug() << k_funcinfo << endl;
        formulaDocument = new KFormula::Document;
        m_formulaDocumentWrapper->document( formulaDocument, init );
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
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it ) {
        (*it)->getGUI()->canvasWidget()->repaintChanged( frameset, it == m_lstViews.fromLast() );
    }
}

void KWDocument::deleteTable( KWTableFrameSet *table )
{
    if ( !table )
        return;
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
    kDebug(32002) << "KWDocument::deleteFrame frame=" << frame << " fs=" << fs << endl;
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
        kError(32001) << "FT_CLIPART used! (in KWDocument::deleteFrame)" << endl;
        break;
    case FT_PICTURE:
        cmdName=i18n("Delete Picture Frame");
        docItem=Pictures;
        break;
    case FT_PART:
        cmdName=i18n("Delete Object Frame");
        docItem=Embedded;
        break;
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

void KWDocument::reorganizeGUI()
{
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
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

int KWDocument::typeItemDocStructure(FrameSetType type)
{
    int typeItem;
    switch(type)
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

void KWDocument::refreshDocStructure(FrameSetType type)
{
    emit docStructureChanged(typeItemDocStructure(type));
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


void KWDocument::setTocPresent(bool hasToc)
{
    m_hasTOC=hasToc;
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->updateTocActionText(hasToc);
}

void KWDocument::refreshMenuExpression()
{
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->refreshMenuExpression();
}

void KWDocument::updateZoomRuler()
{
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it ) {
        (*it)->getGUI()->getHorzRuler()->setZoom( zoomedResolutionX() );
        (*it)->getGUI()->getVertRuler()->setZoom( zoomedResolutionY() );
        (*it)->slotUpdateRuler();
    }
}

void KWDocument::updateRulerFrameStartEnd()
{
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->slotUpdateRuler();
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

void KWDocument::setGridX(double gridx) {
    m_gridX = gridx;
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->getGUI()->getHorzRuler()->setGridSize(gridx);
}

Q3ValueList<KoTextObject *> KWDocument::visibleTextObjects(KWViewMode *viewmode) const
{
    Q3ValueList<KoTextObject *> lst;
    Q3PtrList<KWTextFrameSet> textFramesets = allTextFramesets( true );

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
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->initGUIButton();
}

void KWDocument::enableBackgroundSpellCheck( bool b )
{
    m_bgSpellCheck->setEnabled(b);
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->updateBgSpellCheckingState();
}

bool KWDocument::backgroundSpellCheckEnabled() const
{
    return m_bgSpellCheck->enabled();
}

void KWDocument::reactivateBgSpellChecking()
{
    Q3PtrList<KWTextFrameSet> textFramesets = allTextFramesets( true );

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
                kWarning() << "sectionTitle: was looking for pageNum " << pageNum << ", got frame " << frame << " page " << pgNum << endl;
        }
        kDebug(32001) << "KWDocument::sectionTitle for " << pageNum << ":" << txt << endl;
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
    //kDebug(32001) << "KWDocument::sectionTitle(pageNum=" << pageNum << ") m_sectionTitles.size()=" << m_sectionTitles.size() << endl;
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
    //kDebug(32001) << "KWDocument::sectionTitle " << pageNum
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
    Q3PtrList<KWFrame> frames = framesInPage( pageNum );
    Q3PtrListIterator<KWFrame> frameIt( frames );
    for ( ; frameIt.current(); ++frameIt ) {
        if ( first || frameIt.current()->zOrder() > maxZOrder ) {
            maxZOrder = frameIt.current()->zOrder();
            first = false;
        }
    }
    return maxZOrder;
}

Q3PtrList<KWTextFrameSet> KWDocument::allTextFramesets(bool onlyReadWrite) const
{
    Q3PtrList<KWTextFrameSet> textFramesets;
    Q3PtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit ) {
        if(fit.current()->isDeleted()) continue;
        fit.current()->addTextFrameSets(textFramesets, onlyReadWrite);
    }
    return textFramesets;
}

Q3ValueList<KoTextDocument *> KWDocument::allTextDocuments() const
{
    Q3ValueList<KoTextDocument *> lst;
    const Q3PtrList<KWTextFrameSet> textFramesets = allTextFramesets(false);
    Q3PtrListIterator<KWTextFrameSet> fit( textFramesets );
    for ( ; fit.current() ; ++fit ) {
        lst.append( fit.current()->textObject()->textDocument() );
    }
    return lst;
}

int KWDocument::numberOfTextFrameSet( KWFrameSet* fs, bool onlyReadWrite )
{
    Q3PtrList<KWTextFrameSet> textFramesets = allTextFramesets( onlyReadWrite );
    return textFramesets.findRef( static_cast<KWTextFrameSet*>(fs) );
}

KWFrameSet * KWDocument::textFrameSetFromIndex( unsigned int num, bool onlyReadWrite )
{
    return allTextFramesets( onlyReadWrite ).at( num );
}

void KWDocument::updateTextFrameSetEdit()
{
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->slotFrameSetEditChanged();

}

void KWDocument::displayFootNoteFieldCode()
{
    Q3PtrListIterator<KoVariable> it( m_varColl->getVariables() );
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
    QMap<KoTextDocument *, bool> modifiedTextDocuments; // Qt4: QSet
    Q3PtrListIterator<KoVariable> it( m_varColl->getVariables() );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->type() == VT_FOOTNOTE )
        {
            KWFootNoteVariable* footNoteVar = static_cast<KWFootNoteVariable *>(it.current());
            footNoteVar->formatedNote();
            footNoteVar->resize();
            footNoteVar->frameSet()->setCounterText( footNoteVar->text() );

            KoTextParag * parag = footNoteVar->paragraph();
            if ( parag )
            {
                parag->invalidate( 0 );
                parag->setChanged( true );
            }
            KoTextDocument* textdoc = parag->textDocument();
            if ( modifiedTextDocuments.find( textdoc ) != modifiedTextDocuments.end() ) // Qt4: !contains
                modifiedTextDocuments.insert( textdoc, true );
        }
    }
    for( QMap<KoTextDocument *,bool>::const_iterator it = modifiedTextDocuments.begin();
         it != modifiedTextDocuments.end(); ++it ) {
        KoTextDocument* textdoc = it.key();
        KWTextFrameSet * textfs = static_cast<KWTextDocument *>(textdoc)->textFrameSet();
        slotRepaintChanged( textfs );
    }
}


void KWDocument::setTabStopValue ( double tabStop )
{
    m_tabStop = tabStop;
    Q3PtrList<KWTextFrameSet> textFramesets = allTextFramesets( true );

    KWTextFrameSet *frm;
    for ( frm=textFramesets.first(); frm != 0; frm=textFramesets.next() ){
        frm->textDocument()->setTabStops( ptToLayoutUnitPixX( tabStop ));
        frm->layout();
    }
    repaintAllViews();
}

void KWDocument::setGlobalHyphenation( bool hyphen )
{
    m_bGlobalHyphenation = hyphen;
    // This is only used as a default setting for the default format in new documents;
    // In existing documents the hyphenation comes from the existing formats.
}

void KWDocument::setViewFrameBorders( bool b )
{
    m_viewFrameBorders = b;
    m_layoutViewMode->setDrawFrameBorders( b );
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->getGUI()->canvasWidget()->viewMode()->setDrawFrameBorders( b );
}

void KWDocument::switchViewMode( const QString& newViewModeType )
{
    // Don't compare m_viewModeType and newViewMode here, it would break
    // changing the number of pages per row for the preview mode, in kwconfig.
    m_viewModeType = newViewModeType;
    delete m_layoutViewMode;
    m_layoutViewMode = KWViewMode::create( m_viewModeType, this, 0 /*no canvas */ );

    //necessary to switchmode view in all canvas in first.
    //otherwise with more than one view kword crashes !
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->getGUI()->canvasWidget()->switchViewMode( m_viewModeType );

    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->switchModeView();
    emit newContentsSize();

    // Since the text layout depends on the view mode, we need to redo it
    // But after telling the canvas about the new viewmode, otherwise stuff like
    // slotNewContentsSize will crash.
    updateAllFrames();
    layout();

    repaintAllViews( true );
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
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
        for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
            (*it)->testAndCloseAllFrameSetProtectedContent();
    }
}

void KWDocument::updateRulerInProtectContentMode()
{
    for( Q3ValueList<KWView *>::const_iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->updateRulerInProtectContentMode();
}


void KWDocument::insertBookmark( const QString &name, KoTextParag *startparag, KoTextParag *endparag, int start, int end )
{
    m_bookmarkList->append( KoTextBookmark( name, startparag, endparag, start, end ) );
}

void KWDocument::deleteBookmark( const QString &name )
{
    if ( m_bookmarkList->removeByName( name ) )
        setModified(true);
}

void KWDocument::renameBookmark( const QString &oldName, const QString &newName )
{
    if ( oldName == newName )
        return;

    KoTextBookmarkList::iterator it = m_bookmarkList->findByName( oldName );
    if ( it != m_bookmarkList->end() )
    {
        (*it).setBookmarkName( newName );
        setModified(true);
    }
}

const KoTextBookmark * KWDocument::bookmarkByName( const QString & name ) const
{
    KoTextBookmarkList::const_iterator it = m_bookmarkList->findByName( name );
    if ( it != m_bookmarkList->end() )
        return &(*it);
    return 0;
}

QStringList KWDocument::listOfBookmarkName( KWViewMode * viewMode ) const
{
    QStringList list;
    KoTextBookmarkList::const_iterator it = m_bookmarkList->begin();
    const KoTextBookmarkList::const_iterator end = m_bookmarkList->end();
    for ( ; it != end ; ++it )
    {
        const KoTextBookmark& book = *it;
        KWFrameSet* fs = static_cast<KWTextDocument *>(book.textDocument())->textFrameSet();
        if ( fs->isVisible( viewMode ) && !fs->isDeleted() )
            list.append( book.bookmarkName() );
    }
    return list;
}

void KWDocument::paragraphModified(KoTextParag* /*parag*/, int /*KoTextParag::ParagModifyType*/ /*type*/, int /*start*/, int /*length*/)
{
    //kDebug()<<" parag :"<<parag<<" start :"<<start<<" length :"<<length<<endl;
    emit docStructureChanged( Tables | TextFrames );
}


void KWDocument::paragraphDeleted( KoTextParag *parag, KWFrameSet *frm )
{
    KWTextFrameSet* textfs = dynamic_cast<KWTextFrameSet *>( frm );
    if ( textfs )
    {
        // For speed KoTextBookmarkList should probably be a per-paragraph map.
        // The problem is that a bookmark is associated with TWO paragraphs...

        KoTextBookmarkList::iterator it = m_bookmarkList->begin();
        const KoTextBookmarkList::iterator end = m_bookmarkList->end();
        for ( ; it != end ; ++it )
        {
            KoTextBookmark& book = *it;

            // Adjust bookmark to point to a valid paragraph, below or above the deleted one.
            // The old implementation turned the bookmark into a useless one. OOo simply deletes the bookmark...
            if ( book.startParag() == parag )
                book.setStartParag( parag->next() ? parag->next() : parag->prev() );
            if ( book.endParag() == parag )
                book.setEndParag( parag->next() ? parag->next() : parag->prev() );
        }
    }
}

void KWDocument::initBookmarkList()
{
    Q_ASSERT( m_loadingInfo );
    if ( !m_loadingInfo )
        return;
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
                KoTextDocument* textdoc = frm->textDocument();
                KoTextParag* startparag = textdoc->paragAt( (*it).paragStartIndex );
                KoTextParag* endparag = textdoc->paragAt( (*it).paragEndIndex );
                if ( startparag && endparag )
                {
                    m_bookmarkList->append( KoTextBookmark( (*it).bookname,
                                                            startparag, endparag,
                                                            (*it).cursorStartIndex, (*it).cursorEndIndex ) );
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
            m_bufPixmap->resize( qMax( s.width(), m_bufPixmap->width() ),
                    qMax( s.height(), m_bufPixmap->height() ) );
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
    for( Q3ValueList<KWView *>::Iterator it = m_lstViews.begin(); it != m_lstViews.end(); ++it )
        (*it)->updateDirectCursorButton();
}

void KWDocument::setInsertDirectCursor(bool b)
{
    m_bInsertDirectCursor=b;
    KConfig *config = KWFactory::instance()->config();
    config->setGroup( "Interface" );
    config->writeEntry( "InsertDirectCursor", b );
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
    first_row = first_row.trimmed();
    kDebug() << "Suggested filename:" << first_row << endl;
    setURL(first_row);
}

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
    recalcVariables( VT_DATE ); // , VST_CREATION_DATE ...
    // If we then load a document, it will override that date.
}

void KWDocument::updateGridButton()
{
  Q3PtrListIterator<KoView> it( views() );
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

int KWDocument::startPage() const {
    return pageManager()->startPage();
}
int KWDocument::lastPage() const {
    return pageManager()->lastPageNumber();
}

QWidget* KWDocument::createCustomDocumentWidget(QWidget *parent) {
    KoColumns columns;
    columns.columns = 1;
    columns.ptColumnSpacing = m_defaultColumnSpacing;
    return new KWStartupWidget(parent, this, columns);
}

KWDocument::FramesChangedHandler::FramesChangedHandler(KWDocument *parent) {
    m_parent = parent;
    m_needLayout = false;
}

void KWDocument::FramesChangedHandler::addFrame(KWFrame *frame) {
    if(frame == 0) return;
    if(m_frameSets.contains(frame->frameSet())) return;
    m_frameSets.append(frame->frameSet());
    if( frame->runAround() != KWFrame::RA_NO )
        m_needLayout = true;
}

void KWDocument::FramesChangedHandler::addFrameSet(KWFrameSet *fs) {
    if(m_frameSets.contains(fs)) return;
    m_frameSets.append(fs);
    m_needLayout = true;
}

void KWDocument::FramesChangedHandler::execute() {
    if(m_frameSets.count() == 0)
        m_parent->updateAllFrames();
    else {
        Q3ValueListIterator<KWFrameSet*> iter = m_frameSets.begin();
        for(;iter != m_frameSets.end(); ++iter) {
            KWFrameSet *fs = *iter;
            fs->updateFrames();
            if(!m_needLayout)
                fs->layout();
        }

        KWFrameList::recalcAllFrames(m_parent);
    }

    // If frame with text flowing around it -> re-layout all frames
    if ( m_needLayout)
        m_parent->layout();
    //m_parent->repaintAllViewsExcept( 0 );
    m_parent->repaintAllViews();
    m_parent->updateRulerFrameStartEnd();
}

#include "KWDocument.moc"
