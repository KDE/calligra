/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include <qfileinfo.h>
#include <qregexp.h>
#include <qtimer.h>

#include <kapplication.h> // for KDE_VERSION
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kspell.h>
#include <kcursor.h>

#include <koTemplateChooseDia.h>
#include <koMainWindow.h>
#include <koDocumentInfo.h>
#include <koGlobal.h>
#include <koparagcounter.h>
#include <kotextobject.h>
#include <koAutoFormat.h>
#include <koVariable.h>

#include <kformuladocument.h>
#include <unistd.h>
#include <math.h>

#include "kwtableframeset.h"
#include "kwdoc.h"
#include "kwcanvas.h"
#include "defs.h"
#include "mailmerge.h"
#include "kwview.h"
#include "kwviewmode.h"
#include "kwcommand.h"
#include "kwtextimage.h"
#include "kwbgspellcheck.h"
#include "KWordDocIface.h"
#include "kwvariable.h"
#include "kwframelayout.h"

//#define DEBUG_PAGES

// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
static const char * CURRENT_DTD_VERSION = "1.1";

/******************************************************************/
/* Class: KWChild                                              */
/******************************************************************/

KWChild::KWChild( KWDocument *_wdoc, const QRect& _rect, KoDocument *_doc )
    : KoDocumentChild( _wdoc, _doc, _rect )
{
}

KWChild::KWChild( KWDocument *_wdoc )
    : KoDocumentChild( _wdoc )
{
}

KWChild::~KWChild()
{
}

/******************************************************************/
/* Class: KWCommandHistory                                        */
/******************************************************************/
class KWCommandHistory : public KCommandHistory
{
public:
    KWCommandHistory( KWDocument * doc ) : KCommandHistory( doc->actionCollection(),  false ), m_pDoc( doc ) {}
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

/******************************************************************/
/* Class: KWDocument                                              */
/******************************************************************/
const int KWDocument::CURRENT_SYNTAX_VERSION = 2;
/*
const int KWDocument::U_FONT_FAMILY_SAME_SIZE = 1;
const int KWDocument::U_FONT_ALL_SAME_SIZE = 2;
const int KWDocument::U_COLOR = 4;
const int KWDocument::U_INDENT = 8;
const int KWDocument::U_BORDER = 16;
const int KWDocument::U_ALIGN = 32;
const int KWDocument::U_NUMBERING = 64;
const int KWDocument::U_FONT_FAMILY_ALL_SIZE = 128;
const int KWDocument::U_FONT_ALL_ALL_SIZE = 256;
const int KWDocument::U_TABS = 512;
const int KWDocument::U_SMART = 1024;
*/


KWDocument::KWDocument(QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode ),
      m_unit( KoUnit::U_MM ),
      m_urlIntern()
{
    dcop = 0;
    m_tabStop = MM_TO_POINT( 15.0 );
    bgFrameSpellChecked = 0L;

    m_lstViews.setAutoDelete( false );
    m_lstChildren.setAutoDelete( true );
//    varFormats.setAutoDelete(true);
    m_lstFrameSet.setAutoDelete( true );
    // m_textImageRequests does not create or delete the KWTextImage classes
    m_textImageRequests.setAutoDelete(false);

    m_styleColl=new KoStyleCollection();

    setInstance( KWFactory::global(), false );

    m_gridX = m_gridY = 10.0;
    m_indent = MM_TO_POINT( 10.0 );

    m_iNbPagePerRow = 4;
    m_maxRecentFiles = 10;
    m_defaultColumnSpacing=3;
    m_bShowRuler = true;

    m_viewFormattingChars = false;
    m_viewFrameBorders = true;
    m_repaintAllViewsPending = false;
    m_bShowDocStruct = true;
    m_bShowRuler = true;
    m_bDontCheckUpperWord = false;
    m_bDontCheckTitleCase = false;
    m_bShowStatusBar = true;
    m_bAllowAutoFormat = true;
    m_pgUpDownMovesCaret = false;
    m_bShowScrollBar = true;

    m_lastViewMode="ModeNormal";

    m_commandHistory = new KWCommandHistory( this );
    connect( m_commandHistory, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
    connect( m_commandHistory, SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );

    connect( documentInfo(), SIGNAL( sigDocumentInfoModifed()),this,SLOT(slotDocumentInfoModifed() ) );

    setEmpty();
    setModified(false);

    //styleMask = U_FONT_FAMILY_ALL_SIZE | U_COLOR | U_BORDER | U_INDENT |
    //                     U_NUMBERING | U_ALIGN | U_TABS | U_SMART;
    m_headerVisible = false;
    m_footerVisible = false;

    m_pixmapMap = 0L;
    m_clipartMap = 0L;
    m_pasteFramesetsMap = 0L;
    m_varFormatCollection = new KoVariableFormatCollection;
    m_varColl=new KWVariableCollection(new KWVariableSettings() );

    m_autoFormat = new KoAutoFormat(this,m_varColl,m_varFormatCollection );

    m_bgSpellCheck = new KWBgSpellCheck(this);

    m_formulaDocument = 0L; // created on demand

    m_slDataBase = new KWMailMergeDataBase( this );
    slRecordNum = -1;

    m_syntaxVersion = CURRENT_SYNTAX_VERSION;
    m_pKSpellConfig=0;
    m_hasTOC=false;

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
    //kdDebug() << "Default font: requested family: " << m_defaultFont.family() << endl;
    //kdDebug() << "Default font: real family: " << QFontInfo(m_defaultFont).family() << endl;

    int ptSize = m_defaultFont.pointSize();
    if ( ptSize == -1 ) // specified with a pixel size ?
        ptSize = QFontInfo(m_defaultFont).pointSize();

    // Zoom its size to layout units
    m_defaultFont.setPointSize( ptToLayoutUnitPt( ptSize ) );

    // Some simple import filters don't define any style,
    // so let's have a Standard style at least
    KWStyle * standardStyle = new KWStyle( "Standard" ); // This gets translated later on
    //kdDebug() << "KWDocument::KWDocument creating standardStyle " << standardStyle << endl;
    standardStyle->format().setFont( m_defaultFont );
    m_styleColl->addStyleTemplate( standardStyle );

    if ( name )
        dcopObject();
    connect(m_varColl,SIGNAL(repaintVariable()),this,SLOT(slotRepaintVariable()));

    // It's important to call this to have the kformula actions created.
    getFormulaDocument();
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
    // formula frames have to be deleted before m_formulaDocument
    m_lstFrameSet.clear();
    delete m_autoFormat;
    delete m_formulaDocument;
    delete m_commandHistory;
    delete m_varColl;
    delete m_varFormatCollection;
    delete m_slDataBase;
    delete dcop;
    delete m_bgSpellCheck;
    delete m_styleColl;
    delete m_pKSpellConfig;
}

void KWDocument::initConfig()
{
  KConfig *config = KWFactory::global()->config();
  KSpellConfig ksconfig;
  if( config->hasGroup("KSpell kword" ) )
  {
      config->setGroup( "KSpell kword" );
      ksconfig.setNoRootAffix(config->readNumEntry ("KSpell_NoRootAffix", 0));
      ksconfig.setRunTogether(config->readNumEntry ("KSpell_RunTogether", 0));
      ksconfig.setDictionary(config->readEntry ("KSpell_Dictionary", ""));
      ksconfig.setDictFromList(config->readNumEntry ("KSpell_DictFromList", FALSE));
      ksconfig.setEncoding(config->readNumEntry ("KSpell_Encoding", KS_E_ASCII));
      ksconfig.setClient(config->readNumEntry ("KSpell_Client", KS_CLIENT_ISPELL));
      setKSpellConfig(ksconfig);
      setDontCheckUpperWord(config->readBoolEntry("KSpell_dont_check_upper_word",false));
      setDontCheckTitleCase(config->readBoolEntry("KSpell_dont_check_title_case",false));
      // Default is false for spellcheck, but the spell-check config dialog
      // should write out "true" when the user configures spell checking.
      m_bgSpellCheck->enableBackgroundSpellCheck(config->readBoolEntry( "SpellCheck", false ));


  }

  if(config->hasGroup("Interface" ) )
  {
      config->setGroup( "Interface" );
      setGridY(config->readDoubleNumEntry("GridY",10.0));
      setGridX(config->readDoubleNumEntry("GridX",10.0));
      // Config-file value in mm, default 10 pt
      double indent = config->readDoubleNumEntry("Indent", MM_TO_POINT(10.0) ) ;
      setIndentValue(indent);
      setShowRuler(config->readBoolEntry("Rulers",true));
      int defaultAutoSave = KoDocument::defaultAutoSave()/60; // in minutes
      setAutoSave(config->readNumEntry("AutoSave",defaultAutoSave)*60); // read key in minutes, call setAutoSave(seconds)
      setNbPagePerRow(config->readNumEntry("nbPagePerRow",4));
      m_maxRecentFiles = config->readNumEntry( "NbRecentFile", 10 );

      m_viewFormattingChars = config->readBoolEntry( "ViewFormattingChars", false );
      m_viewFrameBorders = config->readBoolEntry( "ViewFrameBorders", true );

      m_zoom = config->readNumEntry( "Zoom", 100 );
      m_bShowDocStruct = config->readBoolEntry( "showDocStruct", true );
      m_lastViewMode = config->readEntry( "viewmode", "ModeNormal" );
      setShowStatusBar( config->readBoolEntry( "ShowStatusBar" , true ) );
      setAllowAutoFormat( config->readBoolEntry( "AllowAutoFormat" , true ) );
      setShowScrollBar( config->readBoolEntry( "ShowScrollBar", true ) );

  }
  else
      m_zoom = 100;

  if(config->hasGroup("Misc" ) )
  {
      config->setGroup( "Misc" );
      int undo=config->readNumEntry("UndoRedo",-1);
      if(undo!=-1)
          setUndoRedoLimit(undo);
  }

  setZoomAndResolution( m_zoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );
  newZoomAndResolution( false, false );

  m_viewMode = KWViewMode::create( m_lastViewMode, this );
}

void KWDocument::saveConfig()
{
    // Only save the config that is manipulated by the UI directly.
    // The config from the config dialog is saved by the dialog itself.
    KConfig *config = KWFactory::global()->config();
    config->setGroup( "Interface" );
    config->writeEntry( "ViewFormattingChars", m_viewFormattingChars );
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
    if ( m_formulaDocument )
        m_formulaDocument->setZoomAndResolution( zoom, dpiX, dpiY );
}

KWTextFrameSet * KWDocument::textFrameSet ( unsigned int _num ) const
{
    unsigned int i=0;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
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
    if ( m_formulaDocument )
        m_formulaDocument->newZoomAndResolution( updateViews,forPrint );
    // Update all fonts
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->zoom( forPrint );

    layout();
    updateAllFrames();
    if ( updateViews )
    {
        emit newContentsSize();
        repaintAllViews( true );
    }
}

bool KWDocument::initDoc()
{
    m_pages = 1;

    m_pageColumns.columns = 1;
    m_pageColumns.ptColumnSpacing = m_defaultColumnSpacing;

    m_pageHeaderFooter.header = HF_SAME;
    m_pageHeaderFooter.footer = HF_SAME;
    m_pageHeaderFooter.ptHeaderBodySpacing = 10;
    m_pageHeaderFooter.ptFooterBodySpacing = 10;

    QString _template;

    bool ok = FALSE;
    KoTemplateChooseDia::ReturnType ret = KoTemplateChooseDia::choose(
        KWFactory::global(), _template, "application/x-kword", "*.kwd", i18n("KWord"),
        KoTemplateChooseDia::Everything, "kword_template");
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
        ok = loadNativeFormat( fileName );
        initUnit();
        setEmpty();
    }
    setModified( FALSE );
    return ok;
}

void KWDocument::initUnit()
{
    //load unit config after we load file.
    //load it for new file or empty file
    KConfig *config = KWFactory::global()->config();
    if(config->hasGroup("Misc") )
    {
        config->setGroup( "Misc" );
        setUnit(KoUnit::unit( config->readEntry("Units",KoUnit::unitName(KoUnit::U_MM  ))));
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

    QString fileName( locate( "kword_template", "Normal/.source/PlainText.kwt" , KWFactory::global() ) );
    /*bool ok = */loadNativeFormat( fileName );
    resetURL();
    setModified( FALSE );
    setEmpty();
}


void KWDocument::setPageLayout( const KoPageLayout& _layout, const KoColumns& _cl, const KoKWHeaderFooter& _hf, bool updateViews )
{
    if ( m_processingType == WP ) {
        //kdDebug() << "KWDocument::setPageLayout WP" << endl;
        m_pageLayout = _layout;
        m_pageColumns = _cl;
        m_pageHeaderFooter = _hf;
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
protected:
    // Compare the order of the associated variables - and return a REVERSED result
    // (we want the footnotes from bottom to top)
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
            if (numa > numb) return -1; // > instead of <, to reverse the list.
            return 1;
        }
        return -1; // whatever
    }
};

/* append headers and footers if needed, and create enough pages for all the existing frames */
void KWDocument::recalcFrames( int fromPage, int toPage /*-1 for all*/ )
{
    //kdDebug(32002) << "KWDocument::recalcFrames" << endl;
    if ( m_lstFrameSet.isEmpty() )
        return;

    KWFrameSet *frameset = m_lstFrameSet.getFirst();

    KWTextFrameSet *firstHeader = 0L, *evenHeader = 0L, *oddHeader = 0L;
    KWTextFrameSet *firstFooter = 0L, *evenFooter = 0L, *oddFooter = 0L;

    // Lookup the various header / footer framesets into the variables above
    // [Done in all cases, in order to hide unused framesets]

    KWFootNoteFrameSetList footnotesList;
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
            case KWFrameSet::FI_EVEN_HEADER:
                if ( isHeaderVisible() ) {
                    evenHeader = dynamic_cast<KWTextFrameSet*>( fs );
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
                break;
            case KWFrameSet::FI_ODD_HEADER:
                if ( isHeaderVisible() ) {
                    oddHeader = dynamic_cast<KWTextFrameSet*>( fs );
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
                break;
            case KWFrameSet::FI_FIRST_FOOTER:
                if ( isFooterVisible() ) {
                    firstFooter = dynamic_cast<KWTextFrameSet*>( fs );
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
                break;
            case KWFrameSet::FI_EVEN_FOOTER:
                if ( isFooterVisible() ) {
                    evenFooter = dynamic_cast<KWTextFrameSet*>( fs );
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
                break;
            case KWFrameSet::FI_ODD_FOOTER:
                if ( isFooterVisible() ) {
                    oddFooter = dynamic_cast<KWTextFrameSet*>( fs );
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
            case KWFrameSet::FI_FOOTNOTE: {
                KWFootNoteFrameSet* fnfs = dynamic_cast<KWFootNoteFrameSet *>(fs);
                if ( fnfs && fnfs->isVisible() ) // not visible is when the footnote has been deleted
                    footnotesList.append( fnfs );
            }
            break;
            default: break;
        }
    }

    // This allocation each time might slow things down a bit.
    // TODO KWHeaderFooterFrameSet : public KWTextFrameSet, and store the HeaderFooterFrameset data into there.
    // ... hmm, but then KWFootNoteFrameSet needs to inherit KWHeaderFooterFrameSet?
    QPtrList<KWFrameLayout::HeaderFooterFrameset> headerFooterList;
    headerFooterList.setAutoDelete( true );

    // Now hide & forget the unused header/footer framesets (e.g. 'odd pages' if we are in 'all the same' mode etc.)
    if ( isHeaderVisible() ) {
        Q_ASSERT( firstHeader );
        Q_ASSERT( evenHeader );
        Q_ASSERT( oddHeader );
        switch ( getHeaderType() ) {
            case HF_SAME:
                evenHeader->setVisible( true );
                oddHeader->setVisible( false );
                oddHeader->deleteAllCopies();
                firstHeader->setVisible( false );
                firstHeader->deleteAllCopies();

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenHeader, 0, -1, m_pageHeaderFooter.ptHeaderBodySpacing ) );
                break;
            case HF_FIRST_EO_DIFF: // added for koffice-1.2-beta2
                firstHeader->setVisible( true );
                evenHeader->setVisible( true );
                oddHeader->setVisible( true );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             firstHeader, 0, 0, m_pageHeaderFooter.ptHeaderBodySpacing ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenHeader, 2, -1, m_pageHeaderFooter.ptHeaderBodySpacing, KWFrameLayout::HeaderFooterFrameset::Even ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddHeader, 1, -1, m_pageHeaderFooter.ptHeaderBodySpacing, KWFrameLayout::HeaderFooterFrameset::Odd ) );
                break;
            case HF_FIRST_DIFF:
                evenHeader->setVisible( true );
                oddHeader->setVisible( false );
                oddHeader->deleteAllCopies();
                firstHeader->setVisible( true );

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             firstHeader, 0, 0, m_pageHeaderFooter.ptHeaderBodySpacing ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenHeader, 1, -1, m_pageHeaderFooter.ptHeaderBodySpacing ) );
                break;
            case HF_EO_DIFF:
                evenHeader->setVisible( true );
                oddHeader->setVisible( true );
                firstHeader->setVisible( false );
                firstHeader->deleteAllCopies();

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenHeader, 0, -1, m_pageHeaderFooter.ptHeaderBodySpacing, KWFrameLayout::HeaderFooterFrameset::Even ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddHeader, 1, -1, m_pageHeaderFooter.ptHeaderBodySpacing, KWFrameLayout::HeaderFooterFrameset::Odd ) );
                break;
        }
    }
    if ( isFooterVisible() ) {
        Q_ASSERT( firstFooter );
        Q_ASSERT( evenFooter );
        Q_ASSERT( oddFooter );
        switch ( getFooterType() ) {
            case HF_SAME:
                evenFooter->setVisible( true );
                oddFooter->setVisible( false );
                oddFooter->deleteAllCopies();
                firstFooter->setVisible( false );
                firstFooter->deleteAllCopies();

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenFooter, 0, -1, m_pageHeaderFooter.ptFooterBodySpacing ) );
                break;
            case HF_FIRST_EO_DIFF: // added for koffice-1.2-beta2
                evenFooter->setVisible( true );
                oddFooter->setVisible( true );
                firstFooter->setVisible( true );

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             firstFooter, 1, -1, m_pageHeaderFooter.ptFooterBodySpacing ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenFooter, 2, -1, m_pageHeaderFooter.ptFooterBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Even ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddFooter, 1, -1, m_pageHeaderFooter.ptFooterBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Odd ) );
                break;
            case HF_FIRST_DIFF:
                evenFooter->setVisible( true );
                oddFooter->setVisible( false );
                oddFooter->deleteAllCopies();
                firstFooter->setVisible( true );

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenFooter, 0, 0, m_pageHeaderFooter.ptFooterBodySpacing ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             firstFooter, 1, -1, m_pageHeaderFooter.ptFooterBodySpacing ) );
                break;
            case HF_EO_DIFF:
                evenFooter->setVisible( true );
                oddFooter->setVisible( true );
                firstFooter->setVisible( false );
                firstFooter->deleteAllCopies();

                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             evenFooter, 0, -1, m_pageHeaderFooter.ptFooterBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Even ) );
                headerFooterList.append( new KWFrameLayout::HeaderFooterFrameset(
                                             oddFooter, 1, -1, m_pageHeaderFooter.ptFooterBodySpacing,
                                             KWFrameLayout::HeaderFooterFrameset::Odd ) );
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
                                     m_pageHeaderFooter.ptFooterBodySpacing, // do we need another var?
                                     KWFrameLayout::HeaderFooterFrameset::All );

        // With other kind of framesets, the height is simply frame->height.
        // But for footnotes, the height to pass to KWFrameLayout is the sum of the frame heights.
        hff->m_height = 0;
        for (QPtrListIterator<KWFrame> f = fnfs->frameIterator(); f.current() ; ++f )
            hff->m_height += f.current()->height();

        footnotesHFList.append( hff );
    }

    if ( m_processingType == WP ) { // In WP mode the pages are created automatically. In DTP not...

        int oldPages = m_pages;
        unsigned int frms = frameset->getNumFrames();

        // Determine number of pages - first from the text frames
        m_pages = static_cast<int>( ceil( static_cast<double>( frms ) / static_cast<double>( m_pageColumns.columns ) ) );
#ifdef DEBUG_PAGES
        kdDebug(32002) << "KWDocument::recalcFrames frms(" << frms << ") / columns(" << m_pageColumns.columns << ") = " << m_pages << endl;
#endif
        // Then from the other frames ( frameset-num > 0 )
        double maxBottom = 0;
        for (int m = getNumFrameSets() - 1; m > 0; m-- )
        {
            KWFrameSet *fs=frameSet(m);
            if ( fs->isVisible() && !fs->isAHeader() && !fs->isAFooter() && !fs->isFloating())
            {
                for (int n = fs->getNumFrames()-1; n >= 0 ; n--) {
                    //if ( n == fs->getNumFrames()-1 )
#ifdef DEBUG_PAGES
                    kdDebug(32002) << "KWDocument::recalcFrames frameset " << m << " " << fs->getName()
                                   << " frame " << n << " bottom=" << fs->frame(n)->bottom() << endl;
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
        if ( m_pages != oldPages )
        {
            emit pageNumChanged();
            recalcVariables( VT_PGNUM );
        }

    } else {
        // DTP mode: calculate the number of pages from the frames.
        double height=0;
        for (QPtrListIterator<KWFrameSet> fit = framesetsIterator(); fit.current() ; ++fit ) {
            if(fit.current()->frameSetInfo()==KWFrameSet::FI_BODY && !fit.current()->isFloating()) {
                KWFrameSet * fs = fit.current();
                for (QPtrListIterator<KWFrame> f = fs->frameIterator(); f.current() ; ++f ) {
                    height=QMAX(height, f.current()->bottom());
                }
            }
        }
        m_pages = static_cast<int>((height / ptPaperHeight()) + 0.5);
        if(m_pages < 1) m_pages=1;

        // Problem: in DTP mode ptLeftBorder() etc. seem to be 0,
        // so the headers and footers are at the edge of the page....
    }

    if ( toPage == -1 )
        toPage = m_pages - 1;
    KWFrameLayout frameLayout( this, headerFooterList, footnotesHFList );
    frameLayout.layout( frameset, m_pageColumns.columns, fromPage, toPage );
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

bool KWDocument::loadXML( QIODevice *, const QDomDocument & doc )
{
    QTime dt;
    dt.start();
    emit sigProgress( 0 );
    //kdDebug(32001) << "KWDocument::loadXML" << endl;
    delete m_pixmapMap;
    m_pixmapMap = 0L;
    delete m_clipartMap;
    m_clipartMap = 0L;
    m_textImageRequests.clear();
    m_imageRequests2.clear();
    m_anchorRequests.clear();
    m_clipartRequests.clear();
    m_footnoteVarRequests.clear();
    m_spellListIgnoreAll.clear();

    m_pageColumns.columns = 1;

    m_pageHeaderFooter.header = HF_SAME;
    m_pageHeaderFooter.footer = HF_SAME;
    m_pageHeaderFooter.ptHeaderBodySpacing = 10;
    m_pageHeaderFooter.ptFooterBodySpacing = 10;

    m_varFormatCollection->clear();

    m_pages = 1;

    KoPageLayout __pgLayout;
    KoColumns __columns;
    KoKWHeaderFooter __hf;
    __hf.header = HF_SAME;
    __hf.footer = HF_SAME;
    __hf.ptHeaderBodySpacing = 10.0;
    __hf.ptFooterBodySpacing = 10.0;

    QString value;
    QDomElement word = doc.documentElement();
    unsigned item;

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
            i18n("File format mismatch"), i18n("Continue") );
        if ( ret == KMessageBox::Cancel )
        {
            setErrorMessage( "USER_CANCELED" );
            return false;
        }
    }

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
        __hf.header = static_cast<KoHFType>( KWDocument::getAttribute( paper, "hType", 0 ) );
        __hf.footer = static_cast<KoHFType>( KWDocument::getAttribute( paper, "fType", 0 ) );
        __hf.ptHeaderBodySpacing = getAttribute( paper, "spHeadBody", 0.0 );
        __hf.ptFooterBodySpacing  = getAttribute( paper, "spFootBody", 0.0 );
        __columns.columns = KWDocument::getAttribute( paper, "columns", 1 );
        __columns.ptColumnSpacing = KWDocument::getAttribute( paper, "columnspacing", 0.0 );
        // Now part of the app config
        //m_zoom = KWDocument::getAttribute( paper, "zoom", 100 );
        //if(m_zoom!=100)
        //    setZoomAndResolution( m_zoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY(), false, false );
        // Support the undocumented syntax actually used by KDE 2.0 for some of the above (:-().
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
    }

    // <ATTRIBUTES>
    QDomElement attributes = word.namedItem( "ATTRIBUTES" ).toElement();
    QString unitName;
    if ( !attributes.isNull() )
    {
        m_processingType = static_cast<ProcessingType>( KWDocument::getAttribute( attributes, "processing", 0 ) );
        //KWDocument::getAttribute( attributes, "standardpage", QString::null );
        m_headerVisible = static_cast<bool>( KWDocument::getAttribute( attributes, "hasHeader", 0 ) );
        m_footerVisible = static_cast<bool>( KWDocument::getAttribute( attributes, "hasFooter", 0 ) );
        unitName = KWDocument::getAttribute( attributes, "unit", "pt" );
        m_hasTOC =  static_cast<bool>(KWDocument::getAttribute( attributes,"hasTOC", 0 ) );
        m_tabStop = KWDocument::getAttribute( attributes, "tabStopValue", MM_TO_POINT( 15) );
    } else {
        m_processingType = WP;
        m_headerVisible = false;
        m_footerVisible = false;
        unitName = "pt";
    }
    m_unit = KoUnit::unit( unitName );

    setPageLayout( __pgLayout, __columns, __hf, false );

    getVariableCollection()->variableSetting()->load(word );

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

    emit sigProgress(20);

    QDomElement spellCheckIgnore = word.namedItem( "SPELLCHECKIGNORELIST" ).toElement();
    if( !spellCheckIgnore.isNull() )
    {
        QDomElement spellWord=word.namedItem("SPELLCHECKIGNORELIST").toElement();
        spellWord=spellWord.firstChild().toElement();
        while ( !spellWord.isNull() )
        {
            if ( spellWord.tagName()=="SPELLCHECKIGNOREWORD" )
            {
                m_spellListIgnoreAll.append(spellWord.attribute("word"));
            }
            spellWord=spellWord.nextSibling().toElement();
        }
    }
    m_bgSpellCheck->addIgnoreWordAllList( m_spellListIgnoreAll );

    emit sigProgress(25);


    QDomElement framesets = word.namedItem( "FRAMESETS" ).toElement();
    if ( !framesets.isNull() )
        loadFrameSets( framesets );

    emit sigProgress(85);

    // <PIXMAPS>
    QDomElement pixmapsElem = word.namedItem( "PIXMAPS" ).toElement();
    if ( !pixmapsElem.isNull() )
    {
        m_pixmapMap = new QMap<KoPictureKey, QString>( m_imageCollection.readXML( pixmapsElem ) );
    }

    // <CLIPARTS>
    QDomElement clipartsElem = word.namedItem( "CLIPARTS" ).toElement();
    if ( !clipartsElem.isNull() )
    {
        m_clipartMap = new QMap<KoPictureKey, QString>( m_clipartCollection.readXML( clipartsElem ) );
    }

    emit sigProgress(90);

    // <EMBEDDED>
    QDomNodeList listEmbedded = word.elementsByTagName ( "EMBEDDED" );
    for (item = 0; item < listEmbedded.count(); item++)
    {
        QDomElement embedded = listEmbedded.item( item ).toElement();
        loadEmbedded( embedded );
    }

    emit sigProgress(100); // the rest is only processing, not loading

    bool _first_footer = FALSE, _even_footer = FALSE, _odd_footer = FALSE;
    bool _first_header = FALSE, _even_header = FALSE, _odd_header = FALSE;

    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        switch( fit.current()->frameSetInfo() ) {
        case KWFrameSet::FI_FIRST_HEADER: _first_header = TRUE; break;
        case KWFrameSet::FI_EVEN_HEADER: _even_header = TRUE; break;
        case KWFrameSet::FI_ODD_HEADER: _odd_header = TRUE; break;
        case KWFrameSet::FI_FIRST_FOOTER: _first_footer = TRUE; break;
        case KWFrameSet::FI_EVEN_FOOTER: _even_footer = TRUE; break;
        case KWFrameSet::FI_ODD_FOOTER: _odd_footer = TRUE; break;
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

    // do some sanity checking on document.
    for (int i = getNumFrameSets()-1; i>-1; i--) {
        KWFrameSet *fs = frameSet(i);
        if(!fs) {
            kdWarning() << "frameset " << i << " is NULL!!" << endl;
            m_lstFrameSet.remove(i);
        } else if( fs->type()==FT_TABLE) {
            static_cast<KWTableFrameSet *>( fs )->validate();
        } else if(!fs->getNumFrames()) {
            kdWarning () << "frameset " << i << " has no frames" << endl;
            removeFrameSet(fs);
            delete fs;
        } else if (fs->type() == FT_TEXT) {
            for (int f=fs->getNumFrames()-1; f>=0; f--) {
                KWFrame *frame = fs->frame(f);
                if(frame->height() < static_cast <int>(minFrameHeight)) {
                    kdWarning() << "frame height is so small no text will fit, adjusting (was: "
                      << frame->height() << " is: " << minFrameHeight << ")" << endl;
                    frame->setHeight(minFrameHeight);
                }
                if(frame->width() < static_cast <int>(minFrameWidth)) {
                    kdWarning() << "frame width is so small no text will fit, adjusting (was: "
                     << frame->width() << " is: " << minFrameWidth  << ")" << endl;
                    frame->setWidth(minFrameWidth);
                }
            }
        }
    }
    emit sigProgress(-1);

    //kdDebug(32001) << "KWDocument::loadXML done" << endl;

    setModified( false );

    // Connect to notifications from main text-frameset
    KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet *>( m_lstFrameSet.getFirst() );
    if ( frameset ) {
        connect( frameset->textObject(), SIGNAL( chapterParagraphFormatted( KoTextParag * ) ),
                 SLOT( slotChapterParagraphFormatted( KoTextParag * ) ) );
        connect( frameset, SIGNAL( mainTextHeightChanged() ),
                 SIGNAL( mainTextHeightChanged() ) );
    }

    kdDebug(32001) << "Loading took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;

    return true;
}

void KWDocument::startBackgroundSpellCheck()
{
    //don't start bg spell checking if
    if(backgroundSpellCheckEnabled() && isReadWrite())
    {
        m_bgSpellCheck->objectForSpell(textFrameSet(0));
        m_bgSpellCheck->startBackgroundSpellCheck();
    }

}

void KWDocument::loadEmbedded( QDomElement embedded )
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

void KWDocument::loadStyleTemplates( QDomElement stylesElem )
{
    QValueList<QString> followingStyles;
    QDomNodeList listStyles = stylesElem.elementsByTagName( "STYLE" );
    if( listStyles.count() > 0) { // we are going to import at least one style.
        KWStyle *s = m_styleColl->findStyle("Standard");
        //kdDebug(32001) << "KWDocument::loadStyleTemplates looking for Standard, to delete it. Found " << s << endl;
        if(s) // delete the standard style.
            m_styleColl->removeStyleTemplate(s);
    }
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KWStyle *sty = new KWStyle( QString::null );
        // Load the paraglayout from the <STYLE> element
        KoParagLayout lay = KoStyle::loadStyle( styleElem,syntaxVersion() );
        // This way, KWTextParag::setParagLayout also sets the style pointer, to this style
        lay.style = sty;
        sty->paragLayout() = lay;

        QDomElement nameElem = styleElem.namedItem("NAME").toElement();
        if ( !nameElem.isNull() )
        {
            sty->setName( nameElem.attribute("value") );
            //kdDebug(32001) << "KWStyle created  name=" << sty->name() << endl;
        } else
            kdWarning() << "No NAME tag in LAYOUT -> no name for this style!" << endl;

        // followingStyle is set by KWDocument::loadStyleTemplates after loading all the styles
        sty->setFollowingStyle( sty );

        QDomElement formatElem = styleElem.namedItem( "FORMAT" ).toElement();
        if ( !formatElem.isNull() )
            sty->format() = KWTextParag::loadFormat( formatElem, 0L, defaultFont() );
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
        KWStyle * style = m_styleColl->findStyle(*it);
        m_styleColl->styleAt(i++)->setFollowingStyle( style );
    }

}

void KWDocument::progressItemLoaded()
{
    m_itemsLoaded++;
    // We progress from 20 to 85 -> 65-wide range, 20 offset.
    unsigned int perc = 65 * m_itemsLoaded / m_nrItemsToLoad;
    if ( perc != 65 * (m_itemsLoaded-1) / m_nrItemsToLoad ) // only emit if different from previous call
    {
        //kdDebug(32001) << m_itemsLoaded << " items loaded. %=" << perc + 20 << endl;
        emit sigProgress( perc + 20 );
    }
}

void KWDocument::loadFrameSets( QDomElement framesetsElem )
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

KWFrameSet * KWDocument::loadFrameSet( QDomElement framesetElem, bool loadFrames )
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
    case FT_PICTURE: {
        KWPictureFrameSet *fs = new KWPictureFrameSet( this, fsname );
        fs->load( framesetElem, loadFrames );
        m_lstFrameSet.append( fs );
        return fs;
    } break;
    case FT_CLIPART: {
        KWClipartFrameSet *fs = new KWClipartFrameSet( this, fsname );
        fs->load( framesetElem, loadFrames );
        m_lstFrameSet.append( fs );
        return fs;
    } break;
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

bool KWDocument::completeLoading( KoStore *_store )
{
    if ( _store ) {
        if ( m_pixmapMap ) {
            m_imageCollection.readFromStore( _store, *m_pixmapMap );
            delete m_pixmapMap;
            m_pixmapMap = 0L;
        }
        if ( m_clipartMap ) {
            m_clipartCollection.readFromStore( _store, *m_clipartMap );
            delete m_clipartMap;
            m_clipartMap = 0L;
        }
    }

    processImageRequests();
    processAnchorRequests();
    processFootNoteRequests();

    // Save memory
    m_urlIntern = QString::null;

    // The fields from documentinfo.xml just got loaded -> update vars
    recalcVariables( VT_FIELD );

    // This computes the number of pages (from the frames)
    // for the first time (and adds footers/headers/footnotes etc.)
    // It is necessary to do so BEFORE calling finalize, since updateFrames
    // (in KWTextFrameSet) needs the number of pages.
    recalcFrames();

    // Finalize all the existing framesets
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->finalize();

    // Fix z orders on older documents
    fixZOrders();

    emit newContentsSize();
    repaintAllViews( true );     // in case any view exists already

    startBackgroundSpellCheck();

    return TRUE;
}

void KWDocument::processImageRequests()
{
    QPtrListIterator<KWTextImage> it2 ( m_textImageRequests );
    for ( ; it2.current() ; ++it2 )
    {
        it2.current()->setImage( m_imageCollection );
    }
    m_textImageRequests.clear();

    QPtrListIterator<KWPictureFrameSet> it3( m_imageRequests2 );
    for ( ; it3.current() ; ++it3 )
        it3.current()->setImage( m_imageCollection.findPicture( it3.current()->key() ) );
    m_imageRequests2.clear();

    QPtrListIterator<KWClipartFrameSet> it4( m_clipartRequests );
    for ( ; it4.current() ; ++it4 )
        it4.current()->setClipart( m_clipartCollection.findPicture( it4.current()->key() ) );
    m_clipartRequests.clear();
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
            fs->setAnchored( itanch.data().textfs, itanch.data().paragId, itanch.data().index, true );
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

void KWDocument::pasteFrames( QDomElement topElem, KMacroCommand * macroCmd )
{
    m_pasteFramesetsMap = new QMap<QString, QString>();
    QPtrList<KWFrameSet> frameSetsToFinalize;
    int ref=0;

    QDomElement elem = topElem.firstChild().toElement();
    for ( ; !elem.isNull() ; elem = elem.nextSibling().toElement() )
    {
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
            QString newName;

            // make up a new name for the frame, use Copy[digits]-[oldname] as template.
            // Fully translatable naturally :)
            int count=0;
            QString searchString ("^("+ i18n("Copy%1-%2").arg("\\d*").arg("){0,1}"));
            searchString=searchString.replace(QRegExp("\\-"), "\\-"); // escape the '-'

            QRegExp searcher(searchString);
            do {
                newName=oldName;
                newName.replace(searcher,i18n("Copy%1-%2").arg(count > 0? QString("%1").arg(count):"").arg(""));
                count++;
            } while ( frameSetByName( newName ) );

            m_pasteFramesetsMap->insert( oldName, newName ); // remember the name transformation
            kdDebug(32001) << "KWDocument::pasteFrames new frame : " << oldName << "->" << newName << endl;
            FrameSetType frameSetType = static_cast<FrameSetType>( KWDocument::getAttribute( elem, "frameType", FT_BASE ) );
            switch ( frameSetType ) {
            case FT_TABLE: {
                KWTableFrameSet *table = new KWTableFrameSet( this, newName );
                table->fromXML( elem, true, false /*don't apply names*/ );
                table->moveBy( 20.0, 20.0 );
                m_lstFrameSet.append( table );
                if ( macroCmd )
                    macroCmd->addCommand( new KWCreateTableCommand( QString::null, table ) );
                fs = table;
                break;
            }
            case FT_PART:
                kdWarning(32001) << "Copying part objects isn't implemented yet" << endl;
                break;
            default:
                fs = loadFrameSet( elem, false );
                kdDebug() << "KWDocument::pasteFrames created frame " << newName << endl;
                fs->setName( newName );
                frameElem = elem.namedItem( "FRAME" ).toElement();
            }
            //when we paste a header/footer we transforme it in a body frame
            if(fs->isHeaderOrFooter() || fs->isFootEndNote())
                fs->setFrameSetInfo(KWFrameSet::FI_BODY);
        }
        // Test commented out since the toplevel element can contain "PARAGRAPH" now
        //else
        //kdWarning(32001) << "Unsupported toplevel-element in KWCanvas::pasteFrames : '" << elem.tagName() << "'" << endl;

        if ( fs )
        {
            if ( frameSetsToFinalize.findRef( fs ) == -1 )
                frameSetsToFinalize.append( fs );

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
                frame->load( frameElem, fs->isHeaderOrFooter(), KWDocument::CURRENT_SYNTAX_VERSION );
                frame->setZOrder( maxZOrder( frame->pageNum(this) ) + 1 ); // make sure it's on top
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
    processImageRequests();
    processAnchorRequests();
    if ( processFootNoteRequests() )
    {
        // We pasted footnotes. Relayout frames.
        recalcFrames();
    }

    // Finalize afterwards - especially in case of inline frames, made them inline in processAnchorRequests
    for ( QPtrListIterator<KWFrameSet> fit( frameSetsToFinalize ); fit.current(); ++fit )
        fit.current()->finalize();

    repaintAllViews();
    refreshDocStructure(ref);
    delete m_pasteFramesetsMap;
    m_pasteFramesetsMap = 0L;
}

QDomDocument KWDocument::saveXML()
{
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

    getVariableCollection()->variableSetting()->save(kwdoc );

    QDomElement framesets = doc.createElement( "FRAMESETS" );
    kwdoc.appendChild( framesets );

    m_textImageRequests.clear(); // for KWTextImage
    QValueList<KoPictureKey> saveImages;
    QValueList<KoPictureKey> saveCliparts;

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
            static_cast<KWPartFrameSet *>(frameSet)->updateChildGeometry();
        }

        // If picture frameset, make a note of the image it needs.
        if ( !frameSet->isDeleted() && frameSet->type() == FT_PICTURE )
        {
            KoPictureKey key = static_cast<KWPictureFrameSet *>( frameSet )->key();
            if ( !saveImages.contains( key ) )
                saveImages.append( key );
        }
        if ( !frameSet->isDeleted() && frameSet->type() == FT_CLIPART )
        {
            KoPictureKey key = static_cast<KWClipartFrameSet *>( frameSet )->key();
            if ( !saveCliparts.contains( key ) )
                saveCliparts.append( key );
        }
    }

    // Process the data of the KWTextImage classes.
    QPtrListIterator<KWTextImage> textIt ( m_textImageRequests );
    for ( ; textIt.current() ; ++textIt )
    {
        KoPictureKey key = textIt.current()->getKey();
        kdDebug(32001) << "KWDocument::saveXML registering text image " << key.toString() << endl;
        if ( !saveImages.contains( key ) )
            saveImages.append( key );
    }

    QDomElement styles = doc.createElement( "STYLES" );
    kwdoc.appendChild( styles );
    QPtrList<KWStyle> m_styleList(m_styleColl->styleList());
    for ( KWStyle * p = m_styleList.first(); p != 0L; p = m_styleList.next() )
        saveStyle( p, styles );

    // Save the PIXMAPS list
    QDomElement pixmaps = m_imageCollection.saveXML( KoPictureCollection::CollectionImage, doc, saveImages );
    kwdoc.appendChild( pixmaps );
    QDomElement cliparts = m_clipartCollection.saveXML(KoPictureCollection::CollectionClipart, doc, saveCliparts );
    kwdoc.appendChild( cliparts );

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

    // Write "OBJECT" tag for every child
    QPtrListIterator<KoDocumentChild> chl( children() );
    for( ; chl.current(); ++chl ) {
        QDomElement embeddedElem = doc.createElement( "EMBEDDED" );
        kwdoc.appendChild( embeddedElem );

        KWChild* curr = static_cast<KWChild*>(chl.current());

        QDomElement objectElem = curr->save( doc, true );
        embeddedElem.appendChild( objectElem );

        QDomElement settingsElem = doc.createElement( "SETTINGS" );
        embeddedElem.appendChild( settingsElem );

        QPtrListIterator<KWFrameSet> fit = framesetsIterator();
        for ( ; fit.current() ; ++fit )
        {
            KWFrameSet * fs = fit.current();
            if ( fs->type() == FT_PART &&
                 dynamic_cast<KWPartFrameSet*>( fs )->getChild() == curr )
                fs->save( settingsElem );
        }
    }

    return doc;
}

void KWDocument::saveStyle( KWStyle *sty, QDomElement parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement styleElem = doc.createElement( "STYLE" );
    parentElem.appendChild( styleElem );

    sty->saveStyle( styleElem );

    QDomElement formatElem = KWTextParag::saveFormat( doc, &sty->format(), 0L, 0, 0 );
    styleElem.appendChild( formatElem );
}

bool KWDocument::completeSaving( KoStore *_store )
{
    if ( !_store )
        return TRUE;

    QString u = KURL( url() ).path();

    QValueList<KoPictureKey> saveImages;
    QValueList<KoPictureKey> saveCliparts;

    // At first, we must process the data of the KWTextImage classes.
    // Process the data of the KWTextImage classes.
    QPtrListIterator<KWTextImage> textIt ( m_textImageRequests );
    for ( ; textIt.current() ; ++textIt )
    {
        KoPictureKey key = textIt.current()->getKey();
        kdDebug(32001) << "KWDocument::saveXML registering text image " << key.toString() << endl;
        if ( !saveImages.contains( key ) )
            saveImages.append( key );
    }
    m_textImageRequests.clear(); // Save some memory!

    // Now do the images/cliparts in frames.
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameSet = fit.current();
        // If picture frameset, make a note of the image it needs.
        if ( !frameSet->isDeleted() && frameSet->type() == FT_PICTURE )
        {
            KoPictureKey key = static_cast<KWPictureFrameSet *>( frameSet )->key();
            if ( !saveImages.contains( key ) )
                saveImages.append( key );
        }
        if ( !frameSet->isDeleted() && frameSet->type() == FT_CLIPART )
        {
            KoPictureKey key = static_cast<KWClipartFrameSet *>( frameSet )->key();
            if ( !saveCliparts.contains( key ) )
                saveCliparts.append( key );
        }
    }
    m_imageCollection.saveToStore( KoPictureCollection::CollectionImage, _store, saveImages );
    m_clipartCollection.saveToStore( KoPictureCollection::CollectionClipart, _store, saveCliparts );
    return TRUE;
}

bool KWDocument::saveChildren( KoStore *_store )
{
    int i = 0;
    //kdDebug(32001) << "KWDocument::saveChildren: " << children().count() << " children" << endl;

    QPtrListIterator<KoDocumentChild> it( children() );
    for( ; it.current(); ++it ) {
        KoDocument* childDoc = it.current()->document();
        if (childDoc)
        {
            kdDebug(32001) << "KWDocument::saveChildren url:" << childDoc->url().url()
                      << " extern:" << childDoc->isStoredExtern() << endl;
            if ( childDoc->isStoredExtern() ) {
                if ( !childDoc->save() )
                    return FALSE;
            }
            else
                if ( !childDoc->saveToStore( _store, QString::number( i++ ) ) )
                    return FALSE;
        } else
            kdWarning() << "No document to save for child document " << it.current()->url().url() << endl;
    }
    return true;
}

void KWDocument::addView( KoView *_view )
{
    m_lstViews.append( (KWView*)_view );
    KoDocument::addView( _view );
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
    KoDocument::addShell( shell );
}

KoView* KWDocument::createViewInstance( QWidget* parent, const char* name )
{
    return new KWView( m_viewMode, parent, name, this );
}

void KWDocument::paintContent( QPainter& painter, const QRect& _rect, bool transparent, double zoomX, double zoomY )
{
    //kdDebug(32001) << "************************KWDocument::paintContent m_zoom=" << m_zoom << " zoomX=" << zoomX << " zoomY=" << zoomY << endl;
    m_zoom = 100;
    if ( m_zoomedResolutionX != zoomX || m_zoomedResolutionY != zoomY )
    {
        setResolution( zoomX, zoomY );
        bool forPrint = painter.device() && painter.device()->devType() == QInternal::Printer;
        newZoomAndResolution( false, forPrint );
        if ( m_formulaDocument )
            m_formulaDocument->setZoomAndResolution( m_zoom, zoomX, zoomY, false, forPrint );
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
                                    0L, viewMode, 0L );
    }
    delete viewMode;
}

QPixmap KWDocument::generatePreview( const QSize& size )
{
    int oldZoom = m_zoom;
    double oldZoomX = m_zoomedResolutionX;
    double oldZoomY = m_zoomedResolutionY;

    QPixmap pix = KoDocument::generatePreview(size);

    m_zoom = oldZoom;
    setResolution( oldZoomX, oldZoomY );
    newZoomAndResolution( false, false );

    if ( m_formulaDocument ) {
        m_formulaDocument->setZoomAndResolution( oldZoom, oldZoomX, oldZoomY );
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

    //kdDebug(32001) << "KWDocument::eraseEmptySpace emptySpaceRegion: " << DEBUGRECT( emptySpaceRegion.boundingRect() ) << endl;
    painter->fillRect( emptySpaceRegion.boundingRect(), brush );
    painter->restore();
}

void KWDocument::insertObject( const KoRect& rect, KoDocumentEntry& _e )
{

    KoDocument* doc = _e.createDoc( this );
    if ( !doc || !doc->initDoc() )
        return;

    KWChild* ch = new KWChild( this, rect.toQRect(), doc );

    insertChild( ch );
    setModified( TRUE );

    KWPartFrameSet *frameset = new KWPartFrameSet( this, ch, QString::null );
    KWFrame *frame = new KWFrame(frameset, rect.x(), rect.y(), rect.width(), rect.height() );
    frame->setZOrder( maxZOrder( frame->pageNum(this) ) + 1 ); // make sure it's on top
    frameset->addFrame( frame );
    addFrameSet( frameset );

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

void KWDocument::repaintAllViewsExcept( KWView *_view, bool erase )
{
    //kdDebug(32001) << "KWDocument::repaintAllViewsExcept" << endl;
    for ( KWView * viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
        if ( viewPtr->getGUI() && viewPtr->getGUI()->canvasWidget() ) {
            if ( viewPtr != _view )
                viewPtr->getGUI()->canvasWidget()->repaintAll( erase );
        }
    }
}

void KWDocument::setUnit( KoUnit::Unit _unit )
{
    m_unit = _unit;
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
        if ( viewPtr->getGUI() ) {
            viewPtr->getGUI()->getHorzRuler()->setUnit( KoUnit::unitName( m_unit ) );
            viewPtr->getGUI()->getVertRuler()->setUnit( KoUnit::unitName( m_unit ) );
        }
    }
}

void KWDocument::updateAllStyleLists()
{
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->updateStyleList();
}

void KWDocument::applyStyleChange( KWStyle * changedStyle, int paragLayoutChanged, int formatChanged )
{
    QPtrList<KWTextFrameSet> textFramesets;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit ) {
        fit.current()->addTextFrameSets(textFramesets);
    }

    KWTextFrameSet *frm;
    for ( frm=textFramesets.first(); frm != 0; frm=textFramesets.next() ){
        frm->applyStyleChange( changedStyle, paragLayoutChanged, formatChanged );
    }
}

void KWDocument::repaintAllViews( bool erase )
{
    //kdDebug(32001) << "KWDocument::repaintAllViews" << endl;
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->getGUI()->canvasWidget()->repaintAll( erase );
}

void KWDocument::appendPage( /*unsigned int _page*/ )
{
    int thisPageNum = m_pages-1;
#ifdef DEBUG_PAGES
    kdDebug(32002) << "KWDocument::appendPage m_pages=" << m_pages << " so thisPageNum=" << thisPageNum << endl;
#endif
    m_pages++;

    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameSet = fit.current();
        // don't add tables! A table cell ( frameset ) _must_ not have cells auto-added to them!
        if ( frameSet->type() == FT_TABLE ) continue;

#ifdef DEBUG_PAGES
        kdDebug(32002) << "KWDocument::appendPage looking at frameset " << frameSet->getName() << endl;
#endif
        // KWFrameSet::addFrame triggers a reshuffle in the frames list (KWTextFrameSet::updateFrames)
        // which destroys the iterators -> append the new frames at the end.
        QPtrList<KWFrame> newFrames;

        QPtrListIterator<KWFrame> frameIt( frameSet->frameIterator() );
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame * frame = frameIt.current();
            /* copy the frame if: - it is on this page or
                                  - it is on the former page and the frame is set to double sided.
                                  - AND the frame is set to be reconnected or copied
                                  -  */
#ifdef DEBUG_PAGES
            kdDebug(32002) << "   frame=" << frame << " frame->pageNum()=" << frame->pageNum() << endl;
            static const char * newFrameBh[] = { "Reconnect", "NoFollowup", "Copy" };
            kdDebug(32002) << "   frame->newFrameBehavior()==" << newFrameBh[frame->newFrameBehavior()] << endl;
#endif
            if ( (frame->pageNum() == thisPageNum ||
                  (frame->pageNum() == thisPageNum -1 && frame->sheetSide() != KWFrame::AnySide) )
                 &&
                 ( ( frame->newFrameBehavior()==KWFrame::Reconnect && frameSet->type() == FT_TEXT ) ||  // (*)
                   ( frame->newFrameBehavior()==KWFrame::Copy && !frameSet->isAHeader() && !frameSet->isAFooter() ) ) // (**)
                )
            {
                // (*) : Reconnect only makes sense for text frames
                // (**) : NewFrameBehavior == Copy is handled here except for headers/footers, which
                // are created in recalcFrames() anyway.

                KWFrame *frm = frame->getCopy();
                frm->moveBy( 0, ptPaperHeight() );
                //frm->setPageNum( frame->pageNum()+1 );
                newFrames.append( frm );

                if ( frame->newFrameBehavior()==KWFrame::Copy )
                    frm->setCopy( true );
                //kdDebug(32002) << "   => created frame " << frm << endl;
            }
        }
        QPtrListIterator<KWFrame> newFrameIt( newFrames );
        for ( ; newFrameIt.current() ; ++newFrameIt )
            frameSet->addFrame( newFrameIt.current() );
    }
    emit newContentsSize();

    if ( isHeaderVisible() || isFooterVisible() )
        recalcFrames();  // Get headers and footers on the new page
    // setModified(TRUE); This is called by formatMore, possibly on loading -> don't set modified

    recalcVariables( VT_PGNUM );
    emit pageNumChanged();
}

bool KWDocument::canRemovePage( int num )
{
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameSet = fit.current();
        if ( frameSet->frameSetInfo() != KWFrameSet::FI_BODY ) // if header/footer/footnote
            continue;
        if ( frameSet->isVisible() && !frameSet->canRemovePage( num ) )
            return false;
    }
#ifdef DEBUG_PAGES
    kdDebug(32002) << "KWDocument::canRemovePage " << num << "-> TRUE" << endl;
#endif
    return true;
}

void KWDocument::removePage( int num )
{
#ifdef DEBUG_PAGES
    kdDebug(32002) << "KWDocument::removePage " << num << endl;
#endif
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameSet = fit.current();
        if ( frameSet->frameSetInfo() != KWFrameSet::FI_BODY )
            continue;
        QPtrListIterator<KWFrame> frameIt( frameSet->frameIterator() );
        QPtrList<KWFrame> toDelete;
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame * frm = frameIt.current();
            if ( frm->pageNum() == num )
            {
#ifdef DEBUG_PAGES
                kdDebug(32002) << "KWDocument::removePage deleting frame " << frm << " (from frameset " << frameSet->getName() << ")" << endl;
#endif
                toDelete.append( frm ); // Can't remove the frame here, it screws up the iterator -> toDelete
            }
        }
        QPtrListIterator<KWFrame> delIt( toDelete );
        for ( ; delIt.current(); ++delIt )
            frameSet->delFrame( delIt.current(), true );
    }
    m_pages--;
#ifdef DEBUG_PAGES
    kdDebug(32002) << "KWDocument::removePage -- -> " << m_pages << endl;
#endif
    emit pageNumChanged();
    recalcVariables( VT_PGNUM );
    recalcFrames();
    emit newContentsSize();
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
        int page = QMIN(m_pages-1, static_cast<int>(docPoint.y() / ptPaperHeight()));
        QPtrList<KWFrame> frames = framesInPage(page);
        bool alreadyFoundArgumentFrame=false;

        for (KWFrame *f = frames.last();f;f=frames.prev()) { // z-order
            // only consider non-inline frames.
            if (f->frameSet()->isFloating())
                continue;

            if (alreadyFoundArgumentFrame) {
                if(f->frameAtPos(nPoint, true)) {
                    if ( border ) *border = true;
                    return f;
                }
                if(f->frameAtPos(nPoint))
                    return deepestInlineFrame(f,nPoint,border);
            } else if(f == frame)
                alreadyFoundArgumentFrame=true;
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
        return 0L;
#ifdef DEBUG_FRAMESELECT
    kdDebug(32001) << "KWDocument::frameUnderMouse nPoint=" << nPoint << " firstNonSelected=" << firstNonSelected << endl;
#endif
    KWFrame *candidate=topFrameUnderMouse(nPoint,border);
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


QCursor KWDocument::getMouseCursor( const QPoint &nPoint, bool controlPressed )
{
    bool border=true;

    if (positionToSelectRowcolTable(nPoint) != TABLE_POSITION_NONE)
        return KCursor::handCursor();

    KWFrame *frameundermouse = frameUnderMouse(nPoint, &border );

    if (frameundermouse) {
        QCursor cursor;
        KWFrameSet *frameSet = frameundermouse->frameSet();
        if ( frameSet->getMouseCursor(nPoint, controlPressed, cursor))
            return cursor;
    }
    return ibeamCursor;
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
    KWFrame *frameundermouse, *frameclosetomouseright, *frameclosetomouseunder;

    TableToSelectPosition result = TABLE_POSITION_NONE;

    // now simply check the actual frame under the mouse
    bool border=true;
    frameundermouse = frameUnderMouse(nPoint, &border );

    // now get a frame close to the mouse pointer
    // slightly on the right (could it be that it is a table?)
    QPoint pointTestTableSelect = nPoint;
    pointTestTableSelect.rx() += KWDocument::DISTANCE_TABLE_SELECT_ROWCOL;
    frameclosetomouseright = frameUnderMouse(pointTestTableSelect, &border);

    pointTestTableSelect = nPoint;
    pointTestTableSelect.ry() += KWDocument::DISTANCE_TABLE_SELECT_ROWCOL;
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
    for (int pgnum=0;pgnum<getPages();pgnum++) {
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

    }
    if ( fixed_something )
        updateAllFrames();
}


class KWFrameList: public QPtrList<KWFrame>
{
protected:
    virtual int compareItems(QPtrCollection::Item a, QPtrCollection::Item b)
    {
           int za = ((KWFrame *)a)->zOrder();
           int zb = ((KWFrame *)b)->zOrder();
           if (za == zb) return 0;
           if (za < zb) return -1;
           return 1;
    }
};

// TODO pass viewmode for isVisible? Depends on how framesInPage is being used...
QPtrList<KWFrame> KWDocument::framesInPage( int pageNum, bool sorted) const {
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
    //kdDebug(32002) << "KWDocument::updateAllFrames " << m_lstFrameSet.count() << " framesets." << endl;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->updateFrames();
}

// Tell this method when a frame is moved / resized / created / deleted
// and everything will be update / repainted accordingly
void KWDocument::frameChanged( KWFrame * frame, KWView * view )
{
    //kdDebug(32002) << "KWDocument::frameChanged" << endl;
    updateAllFrames();
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

void KWDocument::setFrameMargins( double l, double r, double t, double b )
{
    // todo, make this more OO, and update the tableheaders as well..
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        if ( frameSet( i )->hasSelectedFrame() ) {
            KWFrameSet *frameset = frameSet( i );
            for ( unsigned int j = 0; j < frameset->getNumFrames(); j++ ) {
                if ( frameset->frame( j )->isSelected() ) {
                    frameset->frame( j )->setBLeft( l );
                    frameset->frame( j )->setBRight( r );
                    frameset->frame( j )->setBTop( t );
                    frameset->frame( j )->setBBottom( b );
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

void KWDocument::addImageRequest( KWPictureFrameSet *fs )
{
    m_imageRequests2.append( fs );
}

void KWDocument::addClipartRequest( KWClipartFrameSet *fs )
{
    m_clipartRequests.append( fs );
}

void KWDocument::addAnchorRequest( const QString &framesetName, const KWAnchorPosition &anchorPos )
{
    m_anchorRequests.insert( framesetName, anchorPos );
}

void KWDocument::addFootNoteRequest( const QString &framesetName, KWFootNoteVariable* var )
{
    m_footnoteVarRequests.insert( framesetName, var );
}


void KWDocument::refreshMenuCustomVariable()
{
   emit sig_refreshMenuCustomVariable();
}


void KWDocument::recalcVariables( int type )
{
    m_varColl->recalcVariables(type);
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

void KWDocument::slotRepaintVariable()
{
    QPtrListIterator<KWFrameSet> it = framesetsIterator();
    for (; it.current(); ++it )
        if( it.current()->type()==FT_TEXT)
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
    m_lstFrameSet.append(f);
    if ( finalize )
        f->finalize();
    setModified( true );
}

void KWDocument::removeFrameSet( KWFrameSet *f )
{
    emit sig_terminateEditing( f );
    m_lstFrameSet.take( m_lstFrameSet.find(f) );
    if ( m_bgSpellCheck->currentCheckSpellingFrame() == f )
        // TODO nextTextFrameSet instead of:
        m_bgSpellCheck->objectForSpell( 0L);
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

void KWDocument::setKSpellConfig(KSpellConfig _kspell)
{
  if(m_pKSpellConfig==0)
    m_pKSpellConfig=new KSpellConfig();

  m_pKSpellConfig->setNoRootAffix(_kspell.noRootAffix ());
  m_pKSpellConfig->setRunTogether(_kspell.runTogether ());
  m_pKSpellConfig->setDictionary(_kspell.dictionary ());
  m_pKSpellConfig->setDictFromList(_kspell.dictFromList());
  m_pKSpellConfig->setEncoding(_kspell.encoding());
  m_pKSpellConfig->setClient(_kspell.client());

  m_bgSpellCheck->setKSpellConfig(_kspell);;
}

#ifndef NDEBUG
void KWDocument::printStyleDebug()
{
    kdDebug() << "----------------------------------------"<<endl;
    QPtrList<KWStyle> m_styleList(m_styleColl->styleList());
    for ( KWStyle * p = m_styleList.first(); p != 0L; p = m_styleList.next() )
    {
        kdDebug() << "Style " << p << "  " << p->name() <<endl;
        kdDebug() << "   format: " << p->format().key() <<endl;
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
    kdDebug() << "# Framesets: " << getNumFrameSets() <<endl;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( unsigned int iFrameset = 0; fit.current() ; ++fit, iFrameset++ )
    {
        KWFrameSet * frameset = fit.current();
        kdDebug() << "Frameset " << iFrameset << ": '" <<
            frameset->getName() << "' (" << frameset << ")" <<endl;
        if ( frameset->isVisible() )
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

// Currently unused, I think
void KWDocument::layout()
{
    QPtrListIterator<KWFrameSet> it = framesetsIterator();
    for (; it.current(); ++it )
        if ( it.current()->isVisible() )
            it.current()->layout();
}

void KWDocument::invalidate()
{
    QPtrListIterator<KWFrameSet> it = framesetsIterator();
    for (; it.current(); ++it )
        it.current()->invalidate();
}

KFormula::Document* KWDocument::getFormulaDocument()
{
    if (!m_formulaDocument) {
        /// ##### kapp->config()? Shouldn't that be instance()->config() instead?
        // Otherwise it depends on which app is embedding us (David).
        m_formulaDocument = new KFormula::Document( kapp->config(), actionCollection(), m_commandHistory );
        m_formulaDocument->setZoomAndResolution( m_zoom,
                                                 qRound(INCH_TO_POINT( m_resolutionX )), // re-calculate dpiX and dpiY
                                                 qRound(INCH_TO_POINT( m_resolutionY )) );
        m_formulaDocument->newZoomAndResolution(false,false);
    }
    return m_formulaDocument;
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
    case FT_PICTURE:
        cmdName=i18n("Delete Picture Frame");
        docItem=Pictures;
        break;
    case FT_CLIPART:
        cmdName=i18n("Delete Picture Frame"); // TODO "Delete clipart frame" after msg freeze
        docItem=Cliparts;
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

void KWDocument::deleteSeveralFrame()
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
                addCommand( cmd );
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
                addCommand( cmd );
                cmd->execute();
                macroCmd->addCommand(cmd);
                nbCommand++;
            }
        }
    }
    if( nbCommand)
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
        case FT_CLIPART:
            typeItem=(int)Cliparts;
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

int KWDocument::undoRedoLimit()
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

// TODO rename ; pass viewmode
QPtrList<KoTextObject> KWDocument::frameTextObject() const
{
    QPtrList<KoTextObject>lst;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWTextFrameSet * fs = dynamic_cast<KWTextFrameSet *> (fit.current());
        if ( fs && fs->isVisible() && !fs->textObject()->protectContent())
        {
            lst.append(fs->textObject());
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
    m_bgSpellCheck->enableBackgroundSpellCheck(b);
}

bool KWDocument::backgroundSpellCheckEnabled() const
{
    return m_bgSpellCheck->backgroundSpellCheckEnabled();
}

void KWDocument::reactivateBgSpellChecking()
{
    QPtrList<KWTextFrameSet> textFramesets;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit ) {
        fit.current()->addTextFrameSets(textFramesets);
    }

    KWTextFrameSet *frm;
    for ( frm=textFramesets.first(); frm != 0; frm=textFramesets.next() ){
        frm->textObject()->setNeedSpellCheck(true);
    }
    repaintAllViews();
    startBackgroundSpellCheck();
}

KWTextFrameSet* KWDocument::nextTextFrameSet(KWTextFrameSet *obj)
{
    int pos = -1;
    if ( bgFrameSpellChecked )
        pos=m_lstFrameSet.findNextRef(bgFrameSpellChecked);
    if(pos !=-1)
    {
        KWFrameSet *frm=0L;
        for ( frm=m_lstFrameSet.at(pos); frm != 0; frm=m_lstFrameSet.next() ){
            KWTextFrameSet *newFrm = frm->nextTextObject( obj );
            if(newFrm && newFrm->textObject()->needSpellCheck())
            {
                bgFrameSpellChecked = frm;
                return newFrm;
            }
        }
    }
    else
    {
        KWFrameSet *frm=0L;
        for ( frm=m_lstFrameSet.first(); frm != 0; frm=m_lstFrameSet.next() ){
            KWTextFrameSet *newFrm = frm->nextTextObject( obj );
            if(newFrm && newFrm->textObject()->needSpellCheck())
            {
                bgFrameSpellChecked = frm;
                return newFrm;
            }
        }
    }
    bgFrameSpellChecked = 0L;
    return 0L;
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

    for ( ; parag ; parag = parag->next() )
    {
        if ( parag->rect().bottom() < topLUpix ) // too early
            continue;
        if ( parag->rect().top() > bottomLUpix ) // done
            break;
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
            //kdDebug(32001) << "KWDocument::sectionTitle " << txt << endl;
#endif
            // Ensure array is big enough
            if ( pageNum > (int)m_sectionTitles.size()-1 )
                const_cast<KWDocument*>(this)->m_sectionTitles.resize( pageNum + 1 );
            const_cast<KWDocument*>(this)->m_sectionTitles[ pageNum ] = txt;
            return txt;
        }
    }

    // No heading found in page.
    // Try going back to previous pages to find one - recursively.
    if ( pageNum > 0 )
        return sectionTitle( pageNum - 1 );
    // First page, no heading found
    return QString::null;
}

void KWDocument::addIgnoreWordAll( const QString & word)
{
    if( m_spellListIgnoreAll.findIndex( word )==-1)
        m_spellListIgnoreAll.append( word );
    m_bgSpellCheck->addIgnoreWordAll( word );

}

void KWDocument::clearIgnoreWordAll( )
{
    m_spellListIgnoreAll.clear();
    m_bgSpellCheck->clearIgnoreWordAll();

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


int KWDocument::numberOfTextFrameSet( KWFrameSet* fs )
{
    QPtrList<KWTextFrameSet> textFramesets;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit ) {
        fit.current()->addTextFrameSets(textFramesets);
    }
    return textFramesets.findRef( static_cast<KWTextFrameSet*>(fs) );
}

KWFrameSet * KWDocument::textFrameSetFromIndex( unsigned int _num )
{
    QPtrList<KWTextFrameSet> textFramesets;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit ) {
        fit.current()->addTextFrameSets(textFramesets);
    }
    return textFramesets.at( _num );

}

void KWDocument::updateTextFrameSetEdit()
{
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->slotFrameSetEditChanged();

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

void KWDocument::setDontCheckUpperWord(bool _b)
{
    m_bDontCheckUpperWord=_b;
    m_bgSpellCheck->setIgnoreUpperWords( _b);
}

void KWDocument::setDontCheckTitleCase(bool _b)
{
    m_bDontCheckTitleCase=_b;
    m_bgSpellCheck->setIgnoreTitleCase( _b );
}


void KWDocument::setTabStopValue ( double _tabStop )
{
    m_tabStop = _tabStop;
    QPtrList<KWTextFrameSet> textFramesets;
    QPtrListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit ) {
        fit.current()->addTextFrameSets(textFramesets);
    }

    KWTextFrameSet *frm;
    for ( frm=textFramesets.first(); frm != 0; frm=textFramesets.next() ){
        frm->textDocument()->setTabStops( ptToLayoutUnitPixX( _tabStop ));
        frm->layout();
    }
    repaintAllViews();
}

void KWDocument::switchViewMode( KWViewMode * newViewMode )
{
    // Don't compare m_viewMode and newViewMode here, it would break
    // changing the number of pages per row for the preview mode, in kwconfig.
    delete m_viewMode;
    m_viewMode = newViewMode;
    m_lastViewMode = m_viewMode->type(); // remember for saving config

    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->switchModeView();

    emit newContentsSize();
    updateResizeHandles();

    // Since the text layout depends on the view mode, we need to redo it
    // But after telling the canvas about the new viewmode, otherwise stuff like
    // slotNewContentsSize will crash.
    layout();

    repaintAllViews( true );
}

#include "kwdoc.moc"
