/* This file is part of the KDE project
   Copyright (C) 2005-2006 Inge Wallin <inge@lysator.liu.se>
             (C) 2004 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2000-2002 Laurent Montel <montel@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Phillip Mueller <philipp.mueller@gmx.de>
             (C) 2000 Werner Trobin <trobin@kde.org>
             (C) 1999-2000 Simon Hausmann <hausmann@kde.org>
             (C) 1999 David Faure <faure@kde.org>
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

#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>

#include <qapplication.h>
#include <qfileinfo.h>
#include <qfont.h>
#include <qpair.h>
//Added by qt3to4:
#include <Q3ValueList>

#include <kcompletion.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <ksconfig.h>
#include <ktempfile.h>

#include <KoApplication.h>
#include <KoDocumentInfo.h>
#include <KoDom.h>
#include <KoMainWindow.h>
#include <KoOasisSettings.h>
#include <KoOasisStyles.h>
#include <KoStoreDevice.h>
#include <KoTemplateChooseDia.h>
#include <KoVariable.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "kspread_canvas.h"
#include "kspread_locale.h"
#include "kspread_map.h"
#include "kspread_sheet.h"
#include "kspread_sheetprint.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "kspread_undo.h"
#include "kspread_util.h"
#include "kspread_view.h"

#include "commands.h"
#include "damages.h"
#include "formula.h"
#include "functions.h"
#include "ksploadinginfo.h"
#include "selection.h"
#include "valuecalc.h"
#include "valueconverter.h"
#include "valueformatter.h"
#include "valueparser.h"

#include "KSpreadDocIface.h"

#include "kspread_doc.h"

using namespace std;
using namespace KSpread;

static const int CURRENT_SYNTAX_VERSION = 1;
// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
static const char * CURRENT_DTD_VERSION = "1.2";

class Doc::Private
{
public:

  Map *map;
  KLocale *locale;
  StyleManager *styleManager;
  ValueParser *parser;
  ValueFormatter *formatter;
  ValueConverter *converter;
  ValueCalc *calc;

  Sheet *activeSheet;
    KSPLoadingInfo *m_loadingInfo;
  static QList<Doc*> s_docs;
  static int s_docId;

  DCOPObject* dcop;

  // URL of the this part. This variable is only set if the load() function
  // had been called with an URL as argument.
  QString fileURL;

  // for undo/redo
  int undoLocked;
  KCommandHistory* commandHistory;

  // true if loading is in process, otherwise false.
  // This flag is used to avoid updates etc. during loading.
  bool isLoading;

  QColor pageBorderColor;

  QList<Plugin*> plugins;

  QList<Reference> refs;
  KCompletion listCompletion;

  int numOperations;

  QList<Damage*> damages;

  // document properties
  int syntaxVersion;
  bool verticalScrollBar:1;
  bool horizontalScrollBar:1;
  bool columnHeader:1;
  bool rowHeader:1;
  QColor gridColor;
  double indentValue;
  bool showStatusBar:1;
  bool showTabBar:1;
  bool showFormulaBar:1;
  bool showError:1;
  KGlobalSettings::Completion completionMode;
  KSpread::MoveTo moveTo;
  MethodOfCalc calcMethod;
  bool delayCalculation:1;
  KSpellConfig *spellConfig;
  bool dontCheckUpperWord:1;
  bool dontCheckTitleCase:1;
    bool configLoadFromFile:1;
  QStringList spellListIgnoreAll;
  /// list of all objects
  QList<EmbeddedObject*> embeddedObjects;
  KoPictureCollection m_pictureCollection;
  Q3ValueList<KoPictureKey> usedPictures;
  bool m_savingWholeDocument;
};

/*****************************************************************************
 *
 * Doc
 *
 *****************************************************************************/

QList<Doc*> Doc::Private::s_docs;
int Doc::Private::s_docId = 0;

Doc::Doc( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
  : KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
  d = new Private;
  d->m_loadingInfo = 0;

  d->map = new Map( this, "Map" );
  d->locale = new Locale;
  d->styleManager = new StyleManager();

  d->parser = new ValueParser( d->locale );
  d->converter = new ValueConverter ( d->parser );
  d->calc = new ValueCalc( d->converter );
  d->calc->setDoc (this);
  d->formatter = new ValueFormatter( d->converter );

  d->activeSheet = 0;

  d->pageBorderColor = Qt::red;
  d->configLoadFromFile = false;

  QFont f( KoGlobal::defaultFont() );
  Format::setGlobalRowHeight( f.pointSizeF() + 3 );
  Format::setGlobalColWidth( ( f.pointSizeF() + 3 ) * 5 );

  d->delayCalculation = false;

  documents().append( this );

  setInstance( Factory::global(), false );
  setTemplateType( "kspread_template" );

  d->dcop = 0;
  d->isLoading = false;
  d->numOperations = 1; // don't start repainting before the GUI is done...

  d->undoLocked = 0;

  d->commandHistory = new KCommandHistory( actionCollection() );
  connect( d->commandHistory, SIGNAL( commandExecuted() ), SLOT( commandExecuted() ) );
  connect( d->commandHistory, SIGNAL( documentRestored() ), SLOT( documentRestored() ) );


  // Make us scripsheet if the document has a name
  // Set a name if there is no name specified
  if ( !name )
  {
      QString tmp( "Document%1" );
      tmp = tmp.arg( d->s_docId++ );
      setObjectName( tmp.toLocal8Bit());//tmp.toLatin1() );
      dcopObject();
  }
  else
      dcopObject();

  // default document properties
  d->syntaxVersion = CURRENT_SYNTAX_VERSION;
  d->verticalScrollBar = true;
  d->horizontalScrollBar = true;
  d->columnHeader = true;
  d->rowHeader = true;
  d->gridColor = Qt::lightGray;
  d->indentValue = 10.0;
  d->showStatusBar = true;
  d->showFormulaBar = true;
  d->showTabBar = true;
  d->showError = false;
  d->calcMethod = SumOfNumber;
  d->moveTo = Bottom;
  d->completionMode = KGlobalSettings::CompletionAuto;
  d->spellConfig = 0;
  d->dontCheckUpperWord = false;
  d->dontCheckTitleCase = false;
}

Doc::~Doc()
{
  //don't save config when kword is embedded into konqueror
  if(isReadWrite())
    saveConfig();

  delete d->dcop;
  d->s_docs.removeAll( this );

  kDebug(36001) << "alive 1" << endl;

  delete d->commandHistory;

  delete d->spellConfig;

  delete d->locale;
  delete d->map;
  delete d->styleManager;
  delete d->parser;
  delete d->formatter;
  delete d->converter;
  delete d->calc;

  delete d;
}

QList<Doc*> Doc::documents()
{
  return Private::s_docs;
}

bool Doc::initDoc(InitDocFlags flags, QWidget* parentWidget)
{
  //  ElapsedTime et( "      initDoc        " );

    QString f;

    if (flags==KoDocument::InitDocEmpty)
    {
        KConfig *config = Factory::global()->config();
        int _page=1;
        if( config->hasGroup("Parameters" ))
        {
                config->setGroup( "Parameters" );
                _page=config->readEntry( "NbPage",1 ) ;
        }

        for( int i=0; i<_page; i++ )
          map()->addNewSheet();

        resetURL();
        setEmpty();
        initConfig();
        styleManager()->createBuiltinStyles();

        return true;
    }

    KoTemplateChooseDia::ReturnType ret;
    KoTemplateChooseDia::DialogType dlgtype;
    if (flags != KoDocument::InitDocFileNew )
            dlgtype = KoTemplateChooseDia::Everything;
    else
            dlgtype = KoTemplateChooseDia::OnlyTemplates;

    ret = KoTemplateChooseDia::choose( Factory::global(), f,
                                       dlgtype, "kspread_template", parentWidget );

    if ( ret == KoTemplateChooseDia::File )
    {
  KUrl url( f );

  bool ok=openURL(url);

  while (KoDocument::isLoading())
    kapp->processEvents();

  return ok;


    }

    if ( ret == KoTemplateChooseDia::Empty )
    {
  KConfig *config = Factory::global()->config();
  int _page=1;
  if( config->hasGroup("Parameters" ))
  {
    config->setGroup( "Parameters" );
    _page=config->readEntry( "NbPage",1 ) ;
  }

  for( int i=0; i<_page; i++ )
    map()->addNewSheet ();

  resetURL();
  setEmpty();
  initConfig();
        styleManager()->createBuiltinStyles();
  return true;
    }

    if ( ret == KoTemplateChooseDia::Template )
    {
        resetURL();
        d->m_loadingInfo = new KSPLoadingInfo;
        d->m_loadingInfo->setLoadTemplate( true );
        bool ok = loadNativeFormat( f );
        if ( !ok )
        {
            showLoadingErrorDialog();
            deleteLoadingInfo();
        }
        setEmpty();
        initConfig();
        return ok;
    }

    return false;
}

void Doc::openTemplate (const QString& file)
{
    d->m_loadingInfo = new KSPLoadingInfo;
    d->m_loadingInfo->setLoadTemplate( true );
    KoDocument::openTemplate( file );
    deleteLoadingInfo();
    initConfig();
}

void Doc::initEmpty()
{
    KConfig *config = Factory::global()->config();
    int _page=1;
    if( config->hasGroup("Parameters" ))
    {
        config->setGroup( "Parameters" );
        _page=config->readEntry( "NbPage",1 ) ;
    }

    for( int i=0; i<_page; i++ )
        map()->addNewSheet();

    resetURL();
    initConfig();
    styleManager()->createBuiltinStyles();

    KoDocument::initEmpty();
}

KLocale *Doc::locale () const
{
  return d->locale;
}

Map *Doc::map () const
{
  return d->map;
}

StyleManager *Doc::styleManager () const
{
  return d->styleManager;
}

ValueParser *Doc::parser () const
{
  return d->parser;
}

ValueFormatter *Doc::formatter () const
{
  return d->formatter;
}

ValueConverter *Doc::converter () const
{
  return d->converter;
}

ValueCalc *Doc::calc () const
{
  return d->calc;
}

void Doc::saveConfig()
{
    if ( isEmbedded() ||!isReadWrite())
        return;
    KConfig *config = Factory::global()->config();
    config->setGroup( "Parameters" );
    config->writeEntry( "Zoom", m_zoom );

}

void Doc::initConfig()
{
    KConfig *config = Factory::global()->config();

    if( config->hasGroup("KSpread Page Layout" ))
    {
      config->setGroup( "KSpread Page Layout" );
      setUnit( (KoUnit::Unit)config->readEntry( "Default unit page" ,0));
    }
    if( config->hasGroup("Parameters" ))
    {
        config->setGroup( "Parameters" );
        m_zoom = config->readEntry( "Zoom", 100 );
    }
    else
      m_zoom = 100;

    int undo=30;
    if(config->hasGroup("Misc" ) )
    {
        config->setGroup( "Misc" );
        undo=config->readEntry("UndoRedo",-1);
    }
    if(undo!=-1)
        setUndoRedoLimit(undo);

    setZoomAndResolution( m_zoom, KoGlobal::dpiX(), KoGlobal::dpiY() );
}

int Doc::syntaxVersion() const
{
  return d->syntaxVersion;
}

bool Doc::isLoading() const
{
  return d->isLoading;
}

QColor Doc::pageBorderColor() const
{
  return d->pageBorderColor;
}

void Doc::changePageBorderColor( const QColor  & _color)
{
  d->pageBorderColor = _color;
}

const QList<Reference>  &Doc::listArea()
{
  return d->refs;
}

KCompletion& Doc::completion()
{
  return d->listCompletion;
}

KoView* Doc::createViewInstance( QWidget* parent, const char* name )
{
    if ( name == 0 )
        name = "View";
    return new View( parent, name, this );
}

bool Doc::saveChildren( KoStore* _store )
{
  return map()->saveChildren( _store );
}

int Doc::supportedSpecialFormats() const
{
    return KoDocument::supportedSpecialFormats();
}

bool Doc::completeSaving( KoStore* _store )
{
    d->m_pictureCollection.saveToStore( KoPictureCollection::CollectionPicture, _store, d->usedPictures );

    return true;
}


QDomDocument Doc::saveXML()
{
    /* don't pull focus away from the editor if this is just a background
       autosave */
    if (!isAutosaving())
    {
        foreach ( KoView* view, views() )
            static_cast<View *>( view )->deleteEditor( true );
    }

    QDomDocument doc = createDomDocument( "spreadsheet", CURRENT_DTD_VERSION );
    QDomElement spread = doc.documentElement();
    spread.setAttribute( "editor", "KSpread" );
    spread.setAttribute( "mime", "application/x-kspread" );
    spread.setAttribute( "syntaxVersion", CURRENT_SYNTAX_VERSION );

    QDomElement dlocale = ((Locale *)locale())->save( doc );
    spread.appendChild( dlocale );

    if (d->refs.count() != 0 )
    {
        QDomElement areaname = saveAreaName( doc );
        spread.appendChild( areaname );
    }

    if( !d->spellListIgnoreAll.isEmpty() )
    {
        QDomElement spellCheckIgnore = doc.createElement( "SPELLCHECKIGNORELIST" );
        spread.appendChild( spellCheckIgnore );
        for ( QStringList::Iterator it = d->spellListIgnoreAll.begin(); it != d->spellListIgnoreAll.end(); ++it )
        {
            QDomElement spellElem = doc.createElement( "SPELLCHECKIGNOREWORD" );
            spellCheckIgnore.appendChild( spellElem );
            spellElem.setAttribute( "word", *it );
        }
    }

    SavedDocParts::const_iterator iter = m_savedDocParts.begin();
    SavedDocParts::const_iterator end  = m_savedDocParts.end();
    while ( iter != end )
    {
      // save data we loaded in the beginning and which has no owner back to file
      spread.appendChild( iter.value() );
      ++iter;
    }

    QDomElement defaults = doc.createElement( "defaults" );
    defaults.setAttribute( "row-height", Format::globalRowHeight() );
    defaults.setAttribute( "col-width", Format::globalColWidth() );
    spread.appendChild( defaults );

    foreach ( Plugin* plugin, d->plugins )
    {
      QDomElement data( plugin->saveXML( doc ) );
      if ( !data.isNull() )
        spread.appendChild( data );
    }

    QDomElement s = styleManager()->save( doc );
    spread.appendChild( s );
    QDomElement e = map()->save( doc );
    spread.appendChild( e );

    setModified( false );

    return doc;
}

bool Doc::loadChildren( KoStore* _store )
{
    return map()->loadChildren( _store );
}

bool Doc::saveOasis( KoStore* store, KoXmlWriter* manifestWriter )
{
  emitBeginOperation(true);
  	bool result=saveOasisHelper( store, manifestWriter, SaveAll );
  emitEndOperation();

  return result;
}

bool Doc::saveOasisHelper( KoStore* store, KoXmlWriter* manifestWriter, SaveFlag saveFlag,
                            QString* /*plainText*/, KoPicture* /*picture*/ )
{
    d->m_pictureCollection.assignUniqueIds();
    d->m_savingWholeDocument = saveFlag == SaveAll ? true : false;

    /* don't pull focus away from the editor if this is just a background
       autosave */
    if (!isAutosaving())
    {
      foreach ( KoView* view, views() )
        static_cast<View *>( view )->deleteEditor( true );
    }
    if ( !store->open( "content.xml" ) )
        return false;

    KoStoreDevice dev( store );
    KoXmlWriter* contentWriter = createOasisXmlWriter( &dev, "office:document-content" );
    KoGenStyles mainStyles;//for compile

    KTempFile contentTmpFile;
    //Check that temp file was successfully created
    if (contentTmpFile.status() != 0)
    {
	    qWarning("Creation of temporary file to store document content failed.");
	    return false;
    }

    contentTmpFile.setAutoDelete( true );
    QFile* tmpFile = contentTmpFile.file();
    KoXmlWriter contentTmpWriter( tmpFile, 1 );



    //todo fixme just add a element for testing saving content.xml
    contentTmpWriter.startElement( "office:body" );
    contentTmpWriter.startElement( "office:spreadsheet" );

    int indexObj = 1;
    int partIndexObj = 0;

    styleManager()->saveOasis( mainStyles );
    map()->saveOasis( contentTmpWriter, mainStyles, store,  manifestWriter, indexObj, partIndexObj );

    saveOasisAreaName( contentTmpWriter );
    contentTmpWriter.endElement(); ////office:spreadsheet
  contentTmpWriter.endElement(); ////office:body

    // Done with writing out the contents to the tempfile, we can now write out the automatic styles
    contentWriter->startElement( "office:automatic-styles" );

    Q3ValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( KoGenStyle::STYLE_AUTO );
    Q3ValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( contentWriter, mainStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }

    styles = mainStyles.styles( STYLE_PAGE );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( contentWriter, mainStyles, "style:style", (*it).name, "style:table-properties" );
    }

    styles = mainStyles.styles( STYLE_COLUMN );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( contentWriter, mainStyles, "style:style", (*it).name, "style:table-column-properties" );
    }

    styles = mainStyles.styles( STYLE_ROW );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( contentWriter, mainStyles, "style:style", (*it).name, "style:table-row-properties" );
    }

    styles = mainStyles.styles( STYLE_CELL_AUTO );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( contentWriter, mainStyles, "style:style", (*it).name, "style:table-cell-properties" );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_NUMBER );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
      (*it).style->writeStyle( contentWriter, mainStyles, "number:number-style", (*it).name, 0 /*TODO ????*/  );
    }

    //TODO FIXME !!!!
    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_DATE );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( contentWriter, mainStyles, "number:date-style", (*it).name, 0 /*TODO ????*/  );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_TIME );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( contentWriter, mainStyles, "number:time-style", (*it).name, 0 /*TODO ????*/  );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_FRACTION );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( contentWriter, mainStyles, "number:number-style", (*it).name, 0 /*TODO ????*/  );
    }

    //TODO FIME !!!!!
    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_PERCENTAGE );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( contentWriter, mainStyles, "number:percentage-style", (*it).name, 0 /*TODO ????*/  );
    }

    //TODO FIME !!!!!
    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_CURRENCY );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( contentWriter, mainStyles, "number:currency-style", (*it).name, 0 /*TODO ????*/  );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_SCIENTIFIC );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( contentWriter, mainStyles, "number:number-style", (*it).name, 0 /*TODO ????*/  );
    }


    contentWriter->endElement(); // office:automatic-styles


   // And now we can copy over the contents from the tempfile to the real one
    tmpFile->close();
    contentWriter->addCompleteElement( tmpFile );
    contentTmpFile.close();


    contentWriter->endElement(); // root element
    contentWriter->endDocument();
    delete contentWriter;
    if ( !store->close() )
        return false;
    //add manifest line for content.xml
    manifestWriter->addManifestEntry( "content.xml",  "text/xml" );

    //todo add manifest line for style.xml
    if ( !store->open( "styles.xml" ) )
        return false;

    manifestWriter->addManifestEntry( "styles.xml",  "text/xml" );
    saveOasisDocumentStyles( store, mainStyles );

    if ( !store->close() ) // done with styles.xml
        return false;

    makeUsedPixmapList();
    d->m_pictureCollection.saveOasisToStore( store, d->usedPictures, manifestWriter);

    if(!store->open("settings.xml"))
        return false;

    KoXmlWriter* settingsWriter = createOasisXmlWriter(&dev, "office:document-settings");
    settingsWriter->startElement("office:settings");
    settingsWriter->startElement("config:config-item-set");
    settingsWriter->addAttribute("config:name", "view-settings");

    KoUnit::saveOasis(settingsWriter, unit());

    saveOasisSettings( *settingsWriter );

    settingsWriter->endElement(); // config:config-item-set

    settingsWriter->startElement("config:config-item-set");
    settingsWriter->addAttribute("config:name", "configuration-settings");
    settingsWriter->addConfigItem("SpellCheckerIgnoreList", d->spellListIgnoreAll.join( "," ) );
    settingsWriter->endElement(); // config:config-item-set
    settingsWriter->endElement(); // office:settings
    settingsWriter->endElement(); // Root:element
    settingsWriter->endDocument();
    delete settingsWriter;

    if(!store->close())
        return false;

    manifestWriter->addManifestEntry("settings.xml", "text/xml");


    if ( saveFlag == SaveSelected )
    {
      foreach ( EmbeddedObject* object, d->embeddedObjects )
      {
        if ( object->getType() != OBJECT_CHART  && object->getType() != OBJECT_KOFFICE_PART )
          continue;
        KoDocumentChild *embedded = dynamic_cast<EmbeddedKOfficeObject *>(object )->embeddedObject();
            //NOTE: If an application's .desktop file lies about opendocument support (ie. it indicates that it has
            //a native OASIS mime type, when it doesn't, this causes a crash when trying to reload and paint
            //the object, since it won't have an associated document.
          if ( !embedded->saveOasis( store, manifestWriter ) )
            continue;
      }
    }


    setModified( false );

    return true;
}

void Doc::loadOasisSettings( const QDomDocument&settingsDoc )
{
    KoOasisSettings settings( settingsDoc );
    KoOasisSettings::Items viewSettings = settings.itemSet( "view-settings" );
    if ( !viewSettings.isNull() )
    {
        setUnit(KoUnit::unit(viewSettings.parseConfigItemString("unit")));
    }
    map()->loadOasisSettings( settings );
    loadOasisIgnoreList( settings );
}

void Doc::saveOasisSettings( KoXmlWriter &settingsWriter )
{
    settingsWriter.startElement("config:config-item-map-indexed");
    settingsWriter.addAttribute("config:name", "Views");
    settingsWriter.startElement( "config:config-item-map-entry" );
    map()->saveOasisSettings( settingsWriter );
    settingsWriter.endElement();
    settingsWriter.endElement();
}


void Doc::loadOasisIgnoreList( const KoOasisSettings& settings )
{
    KoOasisSettings::Items configurationSettings = settings.itemSet( "configuration-settings" );
    if ( !configurationSettings.isNull() )
    {
        const QString ignorelist = configurationSettings.parseConfigItemString( "SpellCheckerIgnoreList" );
        //kDebug()<<" ignorelist :"<<ignorelist<<endl;
        d->spellListIgnoreAll = ignorelist.split( ',', QString::SkipEmptyParts );
    }
}


void Doc::saveOasisDocumentStyles( KoStore* store, KoGenStyles& mainStyles ) const
{
    KoStoreDevice stylesDev( store );
    KoXmlWriter* stylesWriter = createOasisXmlWriter( &stylesDev, "office:document-styles" );

    stylesWriter->startElement( "office:styles" );
    Q3ValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( KoGenStyle::STYLE_USER );
    Q3ValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }

    styles = mainStyles.styles( Doc::STYLE_CELL_USER );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        if ( (*it).style->isDefaultStyle() ) {
          (*it).style->writeStyle( stylesWriter, mainStyles, "style:default-style", (*it).name, "style:table-cell-properties" );
        }
        else {
          (*it).style->writeStyle( stylesWriter, mainStyles, "style:style", (*it).name, "style:table-cell-properties" );
        }
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_HATCH );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "draw:hatch", (*it).name, "style:graphic-properties" ,  true,  true /*add draw:name*/);
    }
    styles = mainStyles.styles( KoGenStyle::STYLE_GRAPHICAUTO );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "style:style", (*it).name , "style:graphic-properties"  );
    }

    stylesWriter->endElement(); // office:styles

    stylesWriter->startElement( "office:automatic-styles" );
    styles = mainStyles.styles( KoGenStyle::STYLE_PAGELAYOUT );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "style:page-layout", (*it).name, "style:page-layout-properties", false /*don't close*/ );
        stylesWriter->endElement();
    }

    stylesWriter->endElement(); // office:automatic-styles
    //code from kword
    stylesWriter->startElement( "office:master-styles" );

    styles = mainStyles.styles( Doc::STYLE_PAGEMASTER );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "style:master-page", (*it).name, "" );
    }

    stylesWriter->endElement(); // office:master-style


    stylesWriter->endElement(); // root element (office:document-styles)
    stylesWriter->endDocument();
    delete stylesWriter;;
}

bool Doc::loadOasis( const QDomDocument& doc, KoOasisStyles& oasisStyles, const QDomDocument& settings, KoStore* store)
{
    if ( !d->m_loadingInfo )
        d->m_loadingInfo = new KSPLoadingInfo;

    QTime dt;
    dt.start();

    emit sigProgress( 0 );
    d->isLoading = true;
    d->spellListIgnoreAll.clear();

    d->refs.clear();

    QDomElement content = doc.documentElement();
    QDomElement realBody ( KoDom::namedItemNS( content, KoXmlNS::office, "body" ) );
    if ( realBody.isNull() )
    {
        setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No office:body tag found." ));
        deleteLoadingInfo();
        return false;
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
        if ( localName.isEmpty() )
            setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No tag found inside office:body." ) );
        else
            setErrorMessage( i18n( "This document is not a spreadsheet, but %1. Please try opening it with the appropriate application." , KoDocument::tagNameToDocumentType( localName ) ) );
        deleteLoadingInfo();
        return false;
    }

    KoOasisLoadingContext context( this, oasisStyles, store );

    //load in first
    styleManager()->loadOasisStyleTemplate( oasisStyles );

    // TODO check versions and mimetypes etc.
    loadOasisAreaName( body );
    loadOasisCellValidation( body );

    // all <sheet:sheet> goes to workbook
    if ( !map()->loadOasis( body, context ) )
    {
        d->isLoading = false;
        deleteLoadingInfo();
        return false;
    }


    if ( !settings.isNull() )
    {
        loadOasisSettings( settings );
    }
    emit sigProgress( 90 );
    initConfig();
    emit sigProgress(-1);

    //display loading time
    kDebug(36001) << "Loading took " << (float)(dt.elapsed()) / 1000.0 << " seconds" << endl;
    return true;
}

bool Doc::loadXML( QIODevice *, const QDomDocument& doc )
{
  QTime dt;
  dt.start();

  emit sigProgress( 0 );
  d->isLoading = true;
  d->spellListIgnoreAll.clear();
  // <spreadsheet>
  QDomElement spread = doc.documentElement();

  if ( spread.attribute( "mime" ) != "application/x-kspread" && spread.attribute( "mime" ) != "application/vnd.kde.kspread" )
  {
    d->isLoading = false;
    setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-kspread or application/vnd.kde.kspread, got %1" , spread.attribute("mime") ) );
    return false;
  }

  d->syntaxVersion = Doc::getAttribute( spread, "syntaxVersion", 0 );
  if ( d->syntaxVersion > CURRENT_SYNTAX_VERSION )
  {
      int ret = KMessageBox::warningContinueCancel(
          0, i18n("This document was created with a newer version of KSpread (syntax version: %1)\n"
                  "When you open it with this version of KSpread, some information may be lost.",d->syntaxVersion),
          i18n("File Format Mismatch"), KStdGuiItem::cont() );
      if ( ret == KMessageBox::Cancel )
      {
          setErrorMessage( "USER_CANCELED" );
          return false;
      }
  }

  // <locale>
  QDomElement loc = spread.namedItem( "locale" ).toElement();
  if ( !loc.isNull() )
      ((Locale *) locale())->load( loc );

  emit sigProgress( 5 );

  QDomElement defaults = spread.namedItem( "defaults" ).toElement();
  if ( !defaults.isNull() )
  {
    bool ok = false;
    double d = defaults.attribute( "row-height" ).toDouble( &ok );
    if ( !ok )
      return false;
    Format::setGlobalRowHeight( d );

    d = defaults.attribute( "col-width" ).toDouble( &ok );

    if ( !ok )
      return false;

    Format::setGlobalColWidth( d );
  }

  d->refs.clear();
  //<areaname >
  QDomElement areaname = spread.namedItem( "areaname" ).toElement();
  if ( !areaname.isNull())
    loadAreaName(areaname);

  QDomElement ignoreAll = spread.namedItem( "SPELLCHECKIGNORELIST").toElement();
  if ( !ignoreAll.isNull())
  {
      QDomElement spellWord=spread.namedItem("SPELLCHECKIGNORELIST").toElement();

      spellWord=spellWord.firstChild().toElement();
      while ( !spellWord.isNull() )
      {
          if ( spellWord.tagName()=="SPELLCHECKIGNOREWORD" )
          {
              d->spellListIgnoreAll.append(spellWord.attribute("word"));
          }
          spellWord=spellWord.nextSibling().toElement();
      }
  }

  emit sigProgress( 40 );
  // In case of reload (e.g. from konqueror)
  map()->sheetList().clear(); // it's set to autoDelete

  QDomElement styles = spread.namedItem( "styles" ).toElement();
  if ( !styles.isNull() )
  {
    if ( !styleManager()->loadXML( styles ) )
    {
      setErrorMessage( i18n( "Styles cannot be loaded." ) );
      d->isLoading = false;
      return false;
    }
  }

  // <map>
  QDomElement mymap = spread.namedItem( "map" ).toElement();
  if ( mymap.isNull() )
  {
      setErrorMessage( i18n("Invalid document. No map tag.") );
      d->isLoading = false;
      return false;
  }
  if ( !map()->loadXML( mymap ) )
  {
      d->isLoading = false;
      return false;
  }

  //Backwards compatibility with older versions for paper layout
  if ( d->syntaxVersion < 1 )
  {
    QDomElement paper = spread.namedItem( "paper" ).toElement();
    if ( !paper.isNull() )
    {
      loadPaper( paper );
    }
  }

  emit sigProgress( 85 );

  QDomElement element( spread.firstChild().toElement() );
  while ( !element.isNull() )
  {
    QString tagName( element.tagName() );

    if ( tagName != "locale" && tagName != "map" && tagName != "styles"
         && tagName != "SPELLCHECKIGNORELIST" && tagName != "areaname"
         && tagName != "paper" )
    {
      // belongs to a plugin, load it and save it for later use
      m_savedDocParts[ tagName ] = element;
    }

    element = element.nextSibling().toElement();
  }

  emit sigProgress( 90 );
  initConfig();
  emit sigProgress(-1);

   kDebug(36001) << "Loading took " << (float)(dt.elapsed()) / 1000.0 << " seconds" << endl;

  emit sig_refreshView();

  return true;
}

void Doc::loadPaper( QDomElement const & paper )
{
  // <paper>
  QString format = paper.attribute( "format" );
  QString orientation = paper.attribute( "orientation" );

  // <borders>
  QDomElement borders = paper.namedItem( "borders" ).toElement();
  if ( !borders.isNull() )
  {
    float left = borders.attribute( "left" ).toFloat();
    float right = borders.attribute( "right" ).toFloat();
    float top = borders.attribute( "top" ).toFloat();
    float bottom = borders.attribute( "bottom" ).toFloat();

    //apply to all sheet
    foreach ( Sheet* sheet, map()->sheetList() )
    {
      sheet->print()->setPaperLayout( left, top, right, bottom,
                                      format, orientation );
    }
  }

  QString hleft, hright, hcenter;
  QString fleft, fright, fcenter;
  // <head>
  QDomElement head = paper.namedItem( "head" ).toElement();
  if ( !head.isNull() )
  {
    QDomElement left = head.namedItem( "left" ).toElement();
    if ( !left.isNull() )
      hleft = left.text();
    QDomElement center = head.namedItem( "center" ).toElement();
    if ( !center.isNull() )
      hcenter = center.text();
    QDomElement right = head.namedItem( "right" ).toElement();
    if ( !right.isNull() )
      hright = right.text();
  }
  // <foot>
  QDomElement foot = paper.namedItem( "foot" ).toElement();
  if ( !foot.isNull() )
  {
    QDomElement left = foot.namedItem( "left" ).toElement();
    if ( !left.isNull() )
      fleft = left.text();
    QDomElement center = foot.namedItem( "center" ).toElement();
    if ( !center.isNull() )
      fcenter = center.text();
    QDomElement right = foot.namedItem( "right" ).toElement();
    if ( !right.isNull() )
      fright = right.text();
  }
  //The macro "<sheet>" formerly was typed as "<table>"
  hleft   = hleft.replace(   "<table>", "<sheet>" );
  hcenter = hcenter.replace( "<table>", "<sheet>" );
  hright  = hright.replace(  "<table>", "<sheet>" );
  fleft   = fleft.replace(   "<table>", "<sheet>" );
  fcenter = fcenter.replace( "<table>", "<sheet>" );
  fright  = fright.replace(  "<table>", "<sheet>" );

  foreach ( Sheet* sheet, map()->sheetList() )
  {
    sheet->print()->setHeadFootLine( hleft, hcenter, hright,
                                     fleft, fcenter, fright);
  }
}

bool Doc::completeLoading( KoStore* /* _store */ )
{
  kDebug(36001) << "------------------------ COMPLETING --------------------" << endl;

  d->isLoading = false;

  //  map()->update();

  foreach ( KoView* view, views() )
    static_cast<View *>( view )->initialPosition();

  kDebug(36001) << "------------------------ COMPLETION DONE --------------------" << endl;

  setModified( false );
  return true;
}


void Doc::registerPlugin( Plugin * plugin )
{
  d->plugins.append( plugin );
}

void Doc::deregisterPlugin( Plugin * plugin )
{
  d->plugins.removeAll( plugin );
}

bool Doc::docData( QString const & xmlTag, QDomElement & data )
{
  SavedDocParts::iterator iter = m_savedDocParts.find( xmlTag );
  if ( iter == m_savedDocParts.end() )
    return false;

  data = iter.value();
  m_savedDocParts.erase( iter );

  return true;
}

void Doc::setShowVerticalScrollBar(bool _show)
{
  d->verticalScrollBar=_show;
}

bool Doc::showVerticalScrollBar()const
{
  return  d->verticalScrollBar;
}

void Doc::setShowHorizontalScrollBar(bool _show)
{
  d->horizontalScrollBar=_show;
}

bool Doc::showHorizontalScrollBar()const
{
  return  d->horizontalScrollBar;
}

KGlobalSettings::Completion Doc::completionMode( ) const
{
  return d->completionMode;
}

void Doc::setShowColumnHeader(bool _show)
{
  d->columnHeader=_show;
}

bool Doc::showColumnHeader() const
{
  return  d->columnHeader;
}

void Doc::setShowRowHeader(bool _show)
{
  d->rowHeader=_show;
}

bool Doc::showRowHeader() const
{
  return  d->rowHeader;
}

void Doc::setGridColor( const QColor& color )
{
  d->gridColor = color;
}

QColor Doc::gridColor() const
{
  return d->gridColor;
}

void Doc::setCompletionMode( KGlobalSettings::Completion complMode)
{
  d->completionMode= complMode;
}

double Doc::indentValue() const
{
  return d->indentValue;
}

void Doc::setIndentValue( double val )
{
  d->indentValue = val;
}

void Doc::setShowStatusBar(bool _statusBar)
{
  d->showStatusBar=_statusBar;
}

bool Doc::showStatusBar() const
{
  return  d->showStatusBar;
}

void Doc::setShowTabBar(bool _tabbar)
{
  d->showTabBar=_tabbar;
}

bool Doc::showTabBar()const
{
  return  d->showTabBar;
}

void Doc::setShowFormulaBar(bool _formulaBar)
{
  d->showFormulaBar=_formulaBar;
}

bool Doc::showFormulaBar() const
{
  return  d->showFormulaBar;
}

void Doc::setShowMessageError(bool _show)
{
  d->showError=_show;
}

bool Doc::showMessageError() const
{
  return  d->showError;
}

KSpread::MoveTo Doc::getMoveToValue() const
{
  return d->moveTo;
}

void Doc::setMoveToValue(KSpread::MoveTo _moveTo)
{
  d->moveTo = _moveTo;
}

void Doc::setTypeOfCalc( MethodOfCalc _calc)
{
  d->calcMethod=_calc;
}

MethodOfCalc Doc::getTypeOfCalc() const
{
  return d->calcMethod;
}

void Doc::setKSpellConfig(KSpellConfig _kspell)
{
  if (d->spellConfig == 0 )
    d->spellConfig = new KSpellConfig();

  d->spellConfig->setNoRootAffix(_kspell.noRootAffix ());
  d->spellConfig->setRunTogether(_kspell.runTogether ());
  d->spellConfig->setDictionary(_kspell.dictionary ());
  d->spellConfig->setDictFromList(_kspell.dictFromList());
  d->spellConfig->setEncoding(_kspell.encoding());
  d->spellConfig->setClient(_kspell.client());
}

KSpellConfig * Doc::getKSpellConfig()
{
    if (!d->spellConfig)
    {
    	KSpellConfig ksconfig;

    	KConfig *config = Factory::global()->config();
    	if( config->hasGroup("KSpell kspread" ) )
    	{
        	config->setGroup( "KSpell kspread" );
        	ksconfig.setNoRootAffix(config->readEntry ("KSpell_NoRootAffix", 0));
        	ksconfig.setRunTogether(config->readEntry ("KSpell_RunTogether", 0));
        	ksconfig.setDictionary(config->readEntry ("KSpell_Dictionary", ""));
        	ksconfig.setDictFromList(config->readEntry ("KSpell_DictFromList", false));
        	ksconfig.setEncoding(config->readEntry ("KSpell_Encoding", int(KS_E_ASCII)));
        	ksconfig.setClient(config->readEntry ("KSpell_Client", int(KS_CLIENT_ISPELL)));
        	setKSpellConfig(ksconfig);

        	setDontCheckUpperWord(config->readEntry("KSpell_IgnoreUppercaseWords", false));
        	setDontCheckTitleCase(config->readEntry("KSpell_IgnoreTitleCaseWords", false));
    	}
    }
  return d->spellConfig;
}

bool Doc::dontCheckUpperWord() const
{
  return d->dontCheckUpperWord;
}

void Doc::setDontCheckUpperWord( bool b )
{
  d->dontCheckUpperWord = b;
}

bool Doc::dontCheckTitleCase() const
{
  return  d->dontCheckTitleCase;
}

void Doc::setDontCheckTitleCase( bool b )
{
  d->dontCheckTitleCase = b;
}

QString Doc::unitName() const
{
  return KoUnit::unitName( unit() );
}

void Doc::increaseNumOperation()
{
  ++d->numOperations;
}

void Doc::decreaseNumOperation()
{
  --d->numOperations;
}

void Doc::addIgnoreWordAllList( const QStringList & _lst)
{
  d->spellListIgnoreAll = _lst;
}

QStringList Doc::spellListIgnoreAll() const
{
  return d->spellListIgnoreAll;
}

void Doc::setZoomAndResolution( int zoom, int dpiX, int dpiY )
{
    KoZoomHandler::setZoomAndResolution( zoom, dpiX, dpiY );
}

void Doc::newZoomAndResolution( bool updateViews, bool /*forPrint*/ )
{
/*    layout();
    updateAllFrames();*/
    if ( updateViews )
    {
        emit sig_refreshView();
    }
}

void Doc::addCommand( KCommand* command )
{
  if (undoLocked()) return;

  d->commandHistory->addCommand( command, false );
  setModified( true );
}

void Doc::addCommand( UndoAction* undo )
{
  if (undoLocked()) return;
  UndoWrapperCommand* command = new UndoWrapperCommand( undo );
  addCommand( command );
  setModified( true );
}

void Doc::undo()
{
  undoLock ();
  d->commandHistory->undo();
  undoUnlock ();
}

void Doc::redo()
{
  undoLock ();
  d->commandHistory->redo();
  undoUnlock ();
}

void Doc::commandExecuted()
{
  setModified( true );
}

void Doc::documentRestored()
{
  setModified( false );
}

void Doc::undoLock()
{
  d->undoLocked++;
}

void Doc::undoUnlock()
{
  d->undoLocked--;
}

bool Doc::undoLocked() const
{
  return (d->undoLocked > 0);
}

KCommandHistory* Doc::commandHistory()
{
  return d->commandHistory;
}

void Doc::enableUndo( bool _b )
{
  foreach ( KoView* view, views() )
    static_cast<View *>( view )->enableUndo( _b );
}

void Doc::enableRedo( bool _b )
{
  foreach ( KoView* view, views() )
    static_cast<View *>( view )->enableRedo( _b );
}

void Doc::paintContent( QPainter& painter, const QRect& rect,
                               bool transparent, double zoomX, double /*zoomY*/ )
{
    kDebug(36001) << "paintContent() called on " << rect << endl;

  //  ElapsedTime et( "Doc::paintContent1" );
    //kDebug(36001) << "Doc::paintContent m_zoom=" << m_zoom << " zoomX=" << zoomX << " zoomY=" << zoomY << " transparent=" << transparent << endl;

    // save current zoom
    int oldZoom = m_zoom;

    // choose sheet: the first or the active
    Sheet* sheet = 0;
    if ( !d->activeSheet )
        sheet = map()->sheet( 0 );
    else
        sheet = d->activeSheet;
    if ( !sheet )
        return;

    // only one zoom is supported
    double d_zoom = 1.0;
    setZoomAndResolution( 100, KoGlobal::dpiX(), KoGlobal::dpiY() );
    if ( m_zoomedResolutionX != zoomX )
        d_zoom *= ( zoomX / m_zoomedResolutionX );

    // KSpread support zoom, therefore no need to scale with worldMatrix
    QMatrix matrix = painter.matrix();
    matrix.setMatrix( 1, 0, 0, 1, matrix.dx(), matrix.dy() );
    QRect prect = rect;
    prect.setWidth( (int) (prect.width() * painter.matrix().m11()) );
    prect.setHeight( (int) (prect.height() * painter.matrix().m22()) );
    setZoomAndResolution( (int) ( d_zoom * 100 ), KoGlobal::dpiX(), KoGlobal::dpiY() );
    // paint the content, now zoom is correctly set
    kDebug(36001)<<"paintContent-------------------------------------\n";
    painter.save();
    painter.setMatrix( matrix );
    paintContent( painter, prect, transparent, sheet, false );
    painter.restore();

    // restore zoom
    m_zoom = oldZoom;
    setZoomAndResolution( oldZoom, KoGlobal::dpiX(), KoGlobal::dpiY() );
}

void Doc::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/, Sheet* sheet, bool drawCursor )
{
  Q_UNUSED( drawCursor );
    if ( isLoading() )
        return;
    //    ElapsedTime et( "Doc::paintContent2" );

    // if ( !transparent )
    // painter.eraseRect( rect );

    double xpos;
    double ypos;
    int left_col   = sheet->leftColumn( unzoomItX( rect.x() ), xpos );
    int right_col  = sheet->rightColumn( unzoomItX( rect.right() ) );
    int top_row    = sheet->topRow( unzoomItY( rect.y() ), ypos );
    int bottom_row = sheet->bottomRow( unzoomItY( rect.bottom() ) );
    QPen pen;
    pen.setWidth( 1 );
    painter.setPen( pen );

    /* Update the entire visible area. */
    Region region;
    region.add( QRect( left_col, top_row,
                       right_col - left_col + 1,
                       bottom_row - top_row + 1), sheet );

    paintCellRegions(painter, rect, 0, region);
}

void Doc::paintUpdates()
{
  foreach ( KoView* view, views() )
  {
    static_cast<View *>( view )->paintUpdates();
  }

  foreach ( Sheet* sheet, map()->sheetList() )
  {
    sheet->clearPaintDirtyData();
  }
}

void Doc::paintCellRegions(QPainter& painter, const QRect &viewRect,
			   View* view,
			   const Region& region)
{
  //
  // Clip away children
  //

  QRegion rgn = painter.clipRegion();
  if ( rgn.isEmpty() )
    rgn = QRegion( QRect( 0, 0, viewRect.width(), viewRect.height() ) );

  QMatrix matrix;
  if ( view ) {
    matrix.scale( zoomedResolutionX(),
                  zoomedResolutionY() );
    matrix.translate( - view->canvasWidget()->xOffset(),
                      - view->canvasWidget()->yOffset() );
  }
  else {
    matrix = painter.matrix();
  }

//   QPtrListIterator<KoDocumentChild> it( children() );
//   for( ; it.current(); ++it ) {
//     // if ( ((Child*)it.current())->sheet() == sheet &&
//     //    !m_pView->hasDocumentInWindow( it.current()->document() ) )
//     if ( ((Child*)it.current())->sheet() == sheet)
//       rgn -= it.current()->region( matrix );
//   }
  painter.setClipRegion( rgn );

  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );

  QRect cellRegion;
  KoRect unzoomedViewRect = unzoomRect( viewRect );

  Region::ConstIterator endOfList(region.constEnd());
  for (Region::ConstIterator it = region.constBegin(); it != endOfList; ++it)
  {
    cellRegion = (*it)->rect().normalized();

    PaintRegion(painter, unzoomedViewRect, view, cellRegion, (*it)->sheet());
  }
}


void Doc::PaintRegion(QPainter &painter, const KoRect &viewRegion,
		      View* view, const QRect &paintRegion,
		      const Sheet* sheet)
{
  // Paint region has cell coordinates (col,row) while viewRegion has
  // world coordinates.  paintRegion is the cells to update and
  // viewRegion is the area actually onscreen.

  if ( paintRegion.left() <= 0 || paintRegion.top() <= 0 )
    return;

  // Get the world coordinates of the upper left corner of the
  // paintRegion The view is 0, when paintRegion is called from
  // paintContent, which itself is only called, when we should paint
  // the output for INACTIVE embedded view.  If inactive embedded,
  // then there is no view and we alwas start at top/left, so the
  // offset is 0.
  //
  KoPoint  dblCorner;
  if ( view == 0 ) //Most propably we are embedded and inactive, so no offset
    dblCorner = KoPoint( sheet->dblColumnPos( paintRegion.left() ),
			 sheet->dblRowPos( paintRegion.top() ) );
  else
    dblCorner = KoPoint( sheet->dblColumnPos( paintRegion.left() )
			 - view->canvasWidget()->xOffset(),
			 sheet->dblRowPos( paintRegion.top() )
			 - view->canvasWidget()->yOffset() );
  KoPoint dblCurrentCellPos( dblCorner );

  int regionBottom = paintRegion.bottom();
  int regionRight  = paintRegion.right();
  int regionLeft   = paintRegion.left();
  int regionTop    = paintRegion.top();

  QLinkedList<QPoint>  mergedCellsPainted;
  for ( int y = regionTop;
        y <= regionBottom && dblCurrentCellPos.y() <= viewRegion.bottom();
        ++y )
  {
    const RowFormat * row_lay = sheet->rowFormat( y );
    dblCurrentCellPos.setX( dblCorner.x() );

    for ( int x = regionLeft;
          x <= regionRight && dblCurrentCellPos.x() <= viewRegion.right();
          ++x )
    {
      const ColumnFormat *col_lay = sheet->columnFormat( x );
      Cell* cell = sheet->cellAt( x, y );

      QPoint cellRef( x, y );

#if 0
      bool paintBordersBottom = false;
      bool paintBordersRight  = false;
      bool paintBordersLeft   = false;
      bool paintBordersTop    = false;
#endif
      Cell::Borders paintBorder = Cell::NoBorder;

      QPen rightPen( cell->effRightBorderPen( x, y ) );
      QPen leftPen( cell->effLeftBorderPen( x, y ) );
      QPen topPen( cell->effTopBorderPen( x, y ) );
      QPen bottomPen( cell->effBottomBorderPen( x, y ) );

      // Paint border if outermost cell or if the pen is more "worth"
      // than the border pen of the cell on the other side of the
      // border or if the cell on the other side is not painted. In
      // the latter case get the pen that is of more "worth"

      // right border:
      if ( x >= KS_colMax )
        //paintBordersRight = true;
        paintBorder |= Cell::RightBorder;
      else if ( x == regionRight ) {
	paintBorder |= Cell::RightBorder;
	if ( cell->effRightBorderValue( x, y )
	     < sheet->cellAt( x + 1, y )->effLeftBorderValue( x + 1, y ) )
	  rightPen = sheet->cellAt( x + 1, y )->effLeftBorderPen( x + 1, y );
      }
      else {
	paintBorder |= Cell::RightBorder;
	if ( cell->effRightBorderValue( x, y )
	     < sheet->cellAt( x + 1, y )->effLeftBorderValue( x + 1, y ) )
	  rightPen = sheet->cellAt( x + 1, y )->effLeftBorderPen( x + 1, y );
      }

      // Similiar for other borders...
      // bottom border:
      if ( y >= KS_rowMax )
        paintBorder |= Cell::BottomBorder;
      else if ( y == regionBottom ) {
	paintBorder |= Cell::BottomBorder;
	if ( cell->effBottomBorderValue( x, y )
	     < sheet->cellAt( x, y + 1 )->effTopBorderValue( x, y + 1) )
	  bottomPen = sheet->cellAt( x, y + 1 )->effTopBorderPen( x, y + 1 );
      }
      else {
        paintBorder |= Cell::BottomBorder;
        if ( cell->effBottomBorderValue( x, y )
	     < sheet->cellAt( x, y + 1 )->effTopBorderValue( x, y + 1) )
          bottomPen = sheet->cellAt( x, y + 1 )->effTopBorderPen( x, y + 1 );
      }

      // left border:
      if ( x == 1 )
        paintBorder |= Cell::LeftBorder;
      else if ( x == regionLeft ) {
	paintBorder |= Cell::LeftBorder;
	if ( cell->effLeftBorderValue( x, y )
	     < sheet->cellAt( x - 1, y )->effRightBorderValue( x - 1, y ) )
	  leftPen = sheet->cellAt( x - 1, y )->effRightBorderPen( x - 1, y );
      }
      else {
        paintBorder |= Cell::LeftBorder;
        if ( cell->effLeftBorderValue( x, y )
	     < sheet->cellAt( x - 1, y )->effRightBorderValue( x - 1, y ) )
          leftPen = sheet->cellAt( x - 1, y )->effRightBorderPen( x - 1, y );
      }

      // top border:
      if ( y == 1 )
        paintBorder |= Cell::TopBorder;
      else if ( y == regionTop ) {
	paintBorder |= Cell::TopBorder;
	if ( cell->effTopBorderValue( x, y )
	     < sheet->cellAt( x, y - 1 )->effBottomBorderValue( x, y - 1 ) )
	  topPen = sheet->cellAt( x, y - 1 )->effBottomBorderPen( x, y - 1 );
      }
      else {
        paintBorder |= Cell::TopBorder;
        if ( cell->effTopBorderValue( x, y )
	     < sheet->cellAt( x, y - 1 )->effBottomBorderValue( x, y - 1 ) )
          topPen = sheet->cellAt( x, y - 1 )->effBottomBorderPen( x, y - 1 );
      }

#if 0
      cell->paintCell( viewRegion, painter, view, dblCurrentCellPos, cellRef,
		       paintBordersRight, paintBordersBottom,
		       paintBordersLeft, paintBordersTop,
		       rightPen, bottomPen, leftPen, topPen,
		       mergedCellsPainted, false );

      Cell::BorderSides highlightBorder=Cell::NoBorder;
      QPen highlightPen;
#endif

      cell->paintCell( viewRegion, painter, view, dblCurrentCellPos, cellRef,
		       paintBorder,
		       rightPen, bottomPen, leftPen, topPen,
		       mergedCellsPainted );


      dblCurrentCellPos.setX( dblCurrentCellPos.x() + col_lay->dblWidth() );
    }
    dblCurrentCellPos.setY( dblCurrentCellPos.y() + row_lay->dblHeight() );
  }
}


DCOPObject* Doc::dcopObject()
{
    if ( !d->dcop )
        d->dcop = new DocIface( this );

    return d->dcop;
}

void Doc::addAreaName(const QRect &_rect,const QString & name,const QString & sheetName)
{
  setModified( true );
  Reference tmp;
  tmp.rect = _rect;
  tmp.sheet_name = sheetName;
  tmp.ref_name = name;
  d->refs.append( tmp);
  emit sig_addAreaName( name );
}

void Doc::removeArea( const QString & name)
{
    QList<Reference>::Iterator it2;
    for ( it2 = d->refs.begin(); it2 != d->refs.end(); ++it2 )
    {
        if((*it2).ref_name==name)
        {
            d->refs.erase(it2);
            emit sig_removeAreaName( name );
            return;
        }
    }
}

void Doc::changeAreaSheetName(const QString & oldName,const QString & sheetName)
{
  QList<Reference>::Iterator it2;
  for ( it2 = d->refs.begin(); it2 != d->refs.end(); ++it2 )
        {
        if((*it2).sheet_name==oldName)
                   (*it2).sheet_name=sheetName;
        }
}

QRect Doc::getRectArea(const QString  &_sheetName)
{
  QList<Reference>::Iterator it2;
  for ( it2 = d->refs.begin(); it2 != d->refs.end(); ++it2 )
        {
        if((*it2).ref_name==_sheetName)
                {
                return (*it2).rect;
                }
        }
  return QRect(-1,-1,-1,-1);
}

QDomElement Doc::saveAreaName( QDomDocument& doc )
{
   QDomElement element = doc.createElement( "areaname" );
   QList<Reference>::Iterator it2;
   for ( it2 = d->refs.begin(); it2 != d->refs.end(); ++it2 )
   {
        QDomElement e = doc.createElement("reference");
        QDomElement tabname = doc.createElement( "tabname" );
        tabname.appendChild( doc.createTextNode( (*it2).sheet_name ) );
        e.appendChild( tabname );

        QDomElement refname = doc.createElement( "refname" );
        refname.appendChild( doc.createTextNode( (*it2).ref_name ) );
        e.appendChild( refname );

        QDomElement rect = doc.createElement( "rect" );
        rect.setAttribute( "left-rect", ((*it2).rect).left() );
        rect.setAttribute( "right-rect",((*it2).rect).right() );
        rect.setAttribute( "top-rect", ((*it2).rect).top() );
        rect.setAttribute( "bottom-rect", ((*it2).rect).bottom() );
        e.appendChild( rect );
        element.appendChild(e);
   }
   return element;
}

void Doc::loadOasisCellValidation( const QDomElement&body )
{
    QDomNode validation = KoDom::namedItemNS( body, KoXmlNS::table, "content-validations" );
    kDebug()<<"void Doc::loadOasisCellValidation( const QDomElement&body ) \n";
    kDebug()<<"validation.isNull ? "<<validation.isNull()<<endl;
    if ( !validation.isNull() )
    {
        QDomNode n = validation.firstChild();
        for( ; !n.isNull(); n = n.nextSibling() )
        {
            if ( n.isElement() )
            {
                QDomElement element = n.toElement();
                //kDebug()<<" loadOasisCellValidation element.tagName() :"<<element.tagName()<<endl;
                if ( element.tagName() ==  "content-validation" && element.namespaceURI() == KoXmlNS::table ) {
                    d->m_loadingInfo->appendValidation(element.attributeNS( KoXmlNS::table, "name", QString::null ), element );
                    kDebug()<<" validation found :"<<element.attributeNS( KoXmlNS::table, "name", QString::null )<<endl;
                }
                else {
                    kDebug()<<" Tag not recognize :"<<element.tagName()<<endl;
                }
            }
        }
    }
}

void Doc::saveOasisAreaName( KoXmlWriter & xmlWriter )
{
    if ( listArea().count()>0 )
    {
        xmlWriter.startElement( "table:named-expressions" );
        QList<Reference>::Iterator it;
        for ( it = d->refs.begin(); it != d->refs.end(); ++it )
        {
            xmlWriter.startElement( "table:named-range" );

            xmlWriter.addAttribute( "table:name", ( *it ).ref_name );
            xmlWriter.addAttribute( "table:base-cell-address", convertRefToBase( ( *it ).sheet_name, ( *it ).rect ) );
            xmlWriter.addAttribute( "table:cell-range-address", convertRefToRange( ( *it ).sheet_name, ( *it ).rect ) );

            xmlWriter.endElement();
        }
        xmlWriter.endElement();
    }
}

void Doc::loadOasisAreaName( const QDomElement& body )
{
    kDebug()<<"void Doc::loadOasisAreaName( const QDomElement& body ) \n";
    QDomNode namedAreas = KoDom::namedItemNS( body, KoXmlNS::table, "named-expressions" );
    if ( !namedAreas.isNull() )
    {
        kDebug()<<" area name exist \n";
        QDomNode area = namedAreas.firstChild();
        while ( !area.isNull() )
        {
            QDomElement e = area.toElement();
            if ( e.isNull() || !e.hasAttributeNS( KoXmlNS::table, "name" ) || !e.hasAttributeNS( KoXmlNS::table, "cell-range-address" ) )
            {
                kDebug() << "Reading in named area failed" << endl;
                area = area.nextSibling();
                continue;
            }

            // TODO: what is: sheet:base-cell-address
            QString name  = e.attributeNS( KoXmlNS::table, "name", QString::null );
            QString range = e.attributeNS( KoXmlNS::table, "cell-range-address", QString::null );
            kDebug()<<"area name : "<<name<<" range :"<<range<<endl;
            d->m_loadingInfo->addWordInAreaList( name );
            kDebug() << "Reading in named area, name: " << name << ", area: " << range << endl;

            range = Oasis::decodeFormula( range );

            if ( range.indexOf( ':' ) == -1 )
            {
                Point p( range );

                int n = range.indexOf( '!' );
                if ( n > 0 )
                    range = range + ':' + range.right( range.length() - n - 1);

                kDebug() << "=> Area: " << range << endl;
            }

            Range p( range );

            addAreaName( p.range(), name, p.sheetName() );
            kDebug() << "Area range: " << p.sheetName() << endl;

            area = area.nextSibling();
        }
    }
}

void Doc::loadAreaName( const QDomElement& element )
{
  QDomElement tmp=element.firstChild().toElement();
  for( ; !tmp.isNull(); tmp=tmp.nextSibling().toElement()  )
  {
    if ( tmp.tagName() == "reference" )
    {
        QString tabname;
        QString refname;
        int left=0;
        int right=0;
        int top=0;
        int bottom=0;
        QDomElement sheetName = tmp.namedItem( "tabname" ).toElement();
        if ( !sheetName.isNull() )
        {
          tabname=sheetName.text();
        }
        QDomElement referenceName = tmp.namedItem( "refname" ).toElement();
        if ( !referenceName.isNull() )
        {
          refname=referenceName.text();
        }
        QDomElement rect =tmp.namedItem( "rect" ).toElement();
        if (!rect.isNull())
        {
          bool ok;
          if ( rect.hasAttribute( "left-rect" ) )
            left=rect.attribute("left-rect").toInt( &ok );
          if ( rect.hasAttribute( "right-rect" ) )
            right=rect.attribute("right-rect").toInt( &ok );
          if ( rect.hasAttribute( "top-rect" ) )
            top=rect.attribute("top-rect").toInt( &ok );
          if ( rect.hasAttribute( "bottom-rect" ) )
            bottom=rect.attribute("bottom-rect").toInt( &ok );
        }
        QRect _rect;
        _rect.setCoords(left,top,right,bottom);
        addAreaName(_rect,refname,tabname);
    }
  }
}

void Doc::addStringCompletion(const QString &stringCompletion)
{
  if ( d->listCompletion.items().contains(stringCompletion) == 0 )
    d->listCompletion.addItem( stringCompletion );
}

void Doc::refreshInterface()
{
  emit sig_refreshView();
}

void Doc::refreshLocale()
{
    emit sig_refreshLocale();
}


void Doc::emitBeginOperation(bool waitCursor)
{
    //If an emitBeginOperation occurs with waitCursor enabled, then the waiting cursor is set
    //until all operations have been completed.
    //
    //The reason being that any operations started before the first one with waitCursor set
    //are expected to be completed in a short time anyway.
    QCursor* activeOverride = QApplication::overrideCursor();

    if ( waitCursor &&
         ( !activeOverride || activeOverride->shape() != QCursor(Qt::WaitCursor).shape() ) )
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }

//    /* just duplicate the current cursor on the stack, then */
//  else if (QApplication::overrideCursor() != 0)
//    {
//        QApplication::setOverrideCursor(QApplication::overrideCursor()->shape());
//    }

    KoDocument::emitBeginOperation();
    d->delayCalculation = true;
    d->numOperations++;
}

void Doc::emitBeginOperation(void)
{
  emitBeginOperation(true);
}

void Doc::emitEndOperation()
{
  d->numOperations--;

  if ( d->numOperations > 0 )
  {
    KoDocument::emitEndOperation();
    return;
  }

  d->numOperations = 0;
  d->delayCalculation = false;

  KoDocument::emitEndOperation();

  QApplication::restoreOverrideCursor();

  // Do this after the parent class emitEndOperation,
  // because that allows updates on the view again.
  // Only if we have dirty cells, trigger a repainting.
  if (d->activeSheet && !d->activeSheet->paintDirtyData().isEmpty())
    paintUpdates();
}

void Doc::emitEndOperation( const Region& region )
{
  if (d->activeSheet)
    d->activeSheet->setRegionPaintDirty(region);
  Doc::emitEndOperation();
}

bool Doc::delayCalculation() const
{
   return d->delayCalculation;
}

void Doc::updateBorderButton()
{
  foreach ( KoView* view, views() )
    static_cast<View*>( view )->updateBorderButton();
}

void Doc::insertSheet( Sheet * sheet )
{
  foreach ( KoView* view, views() )
    static_cast<View*>( view )->insertSheet( sheet );
}

void Doc::takeSheet( Sheet * sheet )
{
  foreach ( KoView* view, views() )
    static_cast<View*>( view )->removeSheet( sheet );
}

void Doc::addIgnoreWordAll( const QString & word)
{
    if (d->spellListIgnoreAll.indexOf(word) == -1)
        d->spellListIgnoreAll.append( word );
}

void Doc::clearIgnoreWordAll( )
{
    d->spellListIgnoreAll.clear();
}

void Doc::setDisplaySheet(Sheet *_sheet )
{
    d->activeSheet = _sheet;
}

KSPLoadingInfo * Doc::loadingInfo() const
{
    return d->m_loadingInfo;
}

void Doc::deleteLoadingInfo()
{
    delete d->m_loadingInfo;
    d->m_loadingInfo = 0;
}

Sheet * Doc::displaySheet() const
{
    return d->activeSheet;
}

void Doc::addView( KoView *_view )
{
  KoDocument::addView( _view );
  foreach ( KoView* view, views() )
    static_cast<View*>( view )->closeEditor();
}

void Doc::addDamage( Damage* damage )
{
    d->damages.append( damage );

    if( d->damages.count() == 1 )
        QTimer::singleShot( 0, this, SLOT( flushDamages() ) );
}

void Doc::flushDamages()
{
    emit damagesFlushed( d->damages );
    QList<Damage*>::Iterator it;
    for( it = d->damages.begin(); it != d->damages.end(); ++it )
      delete *it;
    d->damages.clear();
}

void Doc::loadConfigFromFile()
{
    d->configLoadFromFile = true;
}

bool Doc::configLoadFromFile() const
{
    return d->configLoadFromFile;
}


void Doc::insertObject( EmbeddedObject * obj )
{
  switch ( obj->getType() )
  {
    case OBJECT_KOFFICE_PART: case OBJECT_CHART:
    {
      KoDocument::insertChild( dynamic_cast<EmbeddedKOfficeObject*>(obj)->embeddedObject() );
      break;
    }
    default:
      ;
  }
  d->embeddedObjects.append( obj );
}

QList<EmbeddedObject*>& Doc::embeddedObjects()
{
    return d->embeddedObjects;
}

KoPictureCollection *Doc::pictureCollection()
{
  return &d->m_pictureCollection;
}

void Doc::repaint( const QRect& rect )
{
  QRect r;
  foreach ( KoView* view, views() )
  {
    r = rect;
    Canvas* canvas = static_cast<View*>( view )->canvasWidget();
    r.moveTopLeft( QPoint( r.x() - (int) canvas->xOffset(),
                           r.y() - (int) canvas->yOffset() ) );
    canvas->update( r );
  }
}

void Doc::repaint( EmbeddedObject *obj )
{
  foreach ( KoView* view, views() )
  {
    Canvas* canvas = static_cast<View*>( view )->canvasWidget();
    if ( obj->sheet() == canvas->activeSheet() )
        canvas->repaintObject( obj );
  }
}

void Doc::repaint( const KoRect& rect )
{
  QRect r;
  foreach ( KoView* view, views() )
  {
    Canvas* canvas = static_cast<View*>( view )->canvasWidget();

    r = zoomRect( rect );
    r.translate( (int)( -canvas->xOffset()*zoomedResolutionX() ) ,
                        (int)( -canvas->yOffset() *zoomedResolutionY()) );
    canvas->update( r );
  }
}

void Doc::addShell( KoMainWindow *shell )
{
  connect( shell, SIGNAL( documentSaved() ), d->commandHistory, SLOT( documentSaved() ) );
  KoDocument::addShell( shell );
}

int Doc::undoRedoLimit() const
{
  return d->commandHistory->undoLimit();
}

void Doc::setUndoRedoLimit(int val)
{
  d->commandHistory->setUndoLimit(val);
  d->commandHistory->setRedoLimit(val);
}

void Doc::insertPixmapKey( KoPictureKey key )
{
    if ( !d->usedPictures.contains( key ) )
        d->usedPictures.append( key );
}

void Doc::makeUsedPixmapList()
{
    d->usedPictures.clear();
    foreach ( EmbeddedObject* object, d->embeddedObjects )
    {
        if( object->getType() == OBJECT_PICTURE && ( d->m_savingWholeDocument || object->isSelected() ) )
            insertPixmapKey( static_cast<EmbeddedPictureObject*>( object )->getKey() );
    }
}

bool Doc::savingWholeDocument()
{
    return d->m_savingWholeDocument;
}

#include "kspread_doc.moc"

