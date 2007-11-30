/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2000-2002 Laurent Montel <montel@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Phillip Mueller <philipp.mueller@gmx.de>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1999-2000 Simon Hausmann <hausmann@kde.org>
   Copyright 1999 David Faure <faure@kde.org>
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
#include "Doc.h"

#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>

#include <QApplication>
#include <QFileInfo>
#include <QFont>
#include <QPair>
#include <QTimer>
//Added by qt3to4:
#include <Q3ValueList>
#include <QPainter>

#include <kcompletion.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <k3sconfig.h>
#include <ktemporaryfile.h>

#include <KoGlobal.h>
#include <KoApplication.h>
#include <KoDocumentInfo.h>
#include <KoDom.h>
#include <KoMainWindow.h>
#include <KoOasisSettings.h>
#include <KoOdfStylesReader.h>
#include <KoOdfReadStore.h>
#include <KoOdfWriteStore.h>
#include <KoShapeConfigFactory.h>
#include <KoShapeFactory.h>
#include <KoShapeManager.h>
#include <KoShapeRegistry.h>
#include <KoStoreDevice.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoZoomHandler.h>

#include "BindingManager.h"
#include "Canvas.h"
#include "Damages.h"
#include "DependencyManager.h"
#include "Formula.h"
#include "Functions.h"
#include "LoadingInfo.h"
#include "Localization.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "Object.h"
#include "RecalcManager.h"
#include "RowColumnFormat.h"
#include "Selection.h"
#include "Sheet.h"
#include "SheetPrint.h"
#include "SheetShapeContainer.h"
#include "SheetView.h"
#include "StyleManager.h"
#include "Util.h"
#include "ValueCalc.h"
#include "ValueConverter.h"
#include "ValueFormatter.h"
#include "ValueParser.h"
#include "View.h"

// commands
#include "commands/UndoWrapperCommand.h"

// database
#include "database/DatabaseManager.h"

// chart shape
#include "kchart/shape/ChartShape.h"
#include "chart/ChartDialog.h"

using namespace std;
using namespace KSpread;

static const int CURRENT_SYNTAX_VERSION = 1;
// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
static const char * CURRENT_DTD_VERSION = "1.2";

typedef QMap<QString, QDomElement> SavedDocParts;

class Doc::Private
{
public:

  Map *map;
  KLocale *locale;
  BindingManager* bindingManager;
  DatabaseManager* databaseManager;
  DependencyManager* dependencyManager;
  NamedAreaManager* namedAreaManager;
  RecalcManager* recalcManager;
  StyleManager *styleManager;
  ValueParser *parser;
  ValueFormatter *formatter;
  ValueConverter *converter;
  ValueCalc *calc;

    // default objects
    ColumnFormat* defaultColumnFormat;
    RowFormat* defaultRowFormat;

  LoadingInfo *loadingInfo;
  static QList<Doc*> s_docs;
  static int s_docId;

//   DCOPObject* dcop;

  // URL of the this part. This variable is only set if the load() function
  // had been called with an URL as argument.
  QString fileURL;

  // for undo/redo
  int undoLocked;

  // true if loading is in process, otherwise false.
  // This flag is used to avoid updates etc. during loading.
  bool isLoading;

  QColor pageBorderColor;

  KCompletion listCompletion;

  int numOperations;

  QList<Damage*> damages;

  // document properties
  int syntaxVersion;
  bool verticalScrollBar        : 1;
  bool horizontalScrollBar      : 1;
  bool columnHeader             : 1;
  bool rowHeader                : 1;
  QColor gridColor;
  double indentValue;
  bool showStatusBar            : 1;
  bool showTabBar               : 1;
  bool showFormulaBar           : 1;
  bool showError                : 1;
  KGlobalSettings::Completion completionMode;
  KSpread::MoveTo moveTo;
  MethodOfCalc calcMethod;
  K3SpellConfig *spellConfig;
  bool dontCheckUpperWord       : 1;
  bool dontCheckTitleCase       : 1;
  bool configLoadFromFile       : 1;
  bool captureAllArrowKeys      : 1;
  QStringList spellListIgnoreAll;
  // list of all objects
  QList<EmbeddedObject*> embeddedObjects;
  KoPictureCollection m_pictureCollection;
  Q3ValueList<KoPictureKey> usedPictures;
  bool m_savingWholeDocument;
  SavedDocParts savedDocParts;

  // calculation settings
  bool caseSensitiveComparisons : 1;
  bool precisionAsShown         : 1;
  bool wholeCellSearchCriteria  : 1;
  bool automaticFindLabels      : 1;
  bool useRegularExpressions    : 1;
  int refYear; // the reference year two-digit years are relative to
  QDate refDate; // the reference date all dates are relative to
  // The precision used for decimal numbers, if the default cell style's
  // precision is set to arbitrary.
  int precision;
};

/*****************************************************************************
 *
 * Doc
 *
 *****************************************************************************/

QList<Doc*> Doc::Private::s_docs;
int Doc::Private::s_docId = 0;

Doc::Doc( QWidget *parentWidget, QObject* parent, bool singleViewMode )
    : KoDocument( parentWidget, parent, singleViewMode )
    , d( new Private )
{
  d->loadingInfo = 0;

  d->map = new Map( this, "Map" );
  d->locale = new Localization;
  d->bindingManager = new BindingManager(d->map);
  d->databaseManager = new DatabaseManager(d->map);
  d->dependencyManager = new DependencyManager( d->map );
  d->namedAreaManager = new NamedAreaManager(this);
  d->recalcManager = new RecalcManager( d->map );
  d->styleManager = new StyleManager();

  d->parser = new ValueParser( this );
  d->converter = new ValueConverter ( d->parser );
  d->calc = new ValueCalc( d->converter );
  d->calc->setDoc (this);
  d->formatter = new ValueFormatter( d->converter );

  d->defaultColumnFormat = new ColumnFormat();
  d->defaultRowFormat = new RowFormat();

  d->pageBorderColor = Qt::red;
  d->configLoadFromFile = false;
  d->captureAllArrowKeys = true;

  QFont font( KoGlobal::defaultFont() );
  d->defaultRowFormat->setHeight( font.pointSizeF() + 3 );
  d->defaultColumnFormat->setWidth( ( font.pointSizeF() + 3 ) * 5 );

  documents().append( this );

  setComponentData( Factory::global(), false );
  setTemplateType( "kspread_template" );

  d->isLoading = false;
  d->numOperations = 1; // don't start repainting before the GUI is done...

  d->undoLocked = 0;

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
  // calculation settings
  d->caseSensitiveComparisons = true;
  d->precisionAsShown         = false;
  d->wholeCellSearchCriteria  = true;
  d->automaticFindLabels      = true;
  d->useRegularExpressions    = true;
  d->refYear = 1930;
  d->refDate = QDate( 1899, 12, 30 );
  d->precision = 8;

    // Init chart shape factory with KSpread's specific configuration panels.
    QList<KoShapeConfigFactory*> panels = ChartDialog::panels( this );
    foreach (QString id, KoShapeRegistry::instance()->keys())
    {
        if ( id == ChartShapeId )
            KoShapeRegistry::instance()->value(id)->setOptionPanels(panels);
    }

    connect(d->namedAreaManager, SIGNAL(namedAreaModified(const QString&)),
            d->dependencyManager, SLOT(namedAreaModified(const QString&)));
    connect(this, SIGNAL(damagesFlushed(const QList<Damage*>&)),
            this, SLOT(handleDamages(const QList<Damage*>&)));
}

Doc::~Doc()
{
  //don't save config when kword is embedded into konqueror
  if(isReadWrite())
    saveConfig();

  delete d->spellConfig;

  delete d->defaultColumnFormat;
  delete d->defaultRowFormat;

  delete d->locale;
  delete d->map;
  delete d->bindingManager;
  delete d->databaseManager;
  delete d->dependencyManager;
  delete d->namedAreaManager;
  delete d->recalcManager;
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

void Doc::openTemplate (const KUrl& url)
{
    d->loadingInfo = new LoadingInfo;
    d->loadingInfo->setLoadTemplate( true );
    KoDocument::openTemplate( url );
    deleteLoadingInfo();
    initConfig();
}

void Doc::initEmpty()
{
    KSharedConfigPtr config = Factory::global().config();
    const int page = config->group( "Parameters" ).readEntry( "NbPage", 1 );

    for ( int i = 0; i < page; ++i )
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

BindingManager* Doc::bindingManager() const
{
    return d->bindingManager;
}

DatabaseManager* Doc::databaseManager() const
{
    return d->databaseManager;
}

DependencyManager* Doc::dependencyManager() const
{
    return d->dependencyManager;
}

NamedAreaManager* Doc::namedAreaManager() const
{
    return d->namedAreaManager;
}

RecalcManager* Doc::recalcManager() const
{
    return d->recalcManager;
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

const ColumnFormat* Doc::defaultColumnFormat() const
{
    return d->defaultColumnFormat;
}

const RowFormat* Doc::defaultRowFormat() const
{
    return d->defaultRowFormat;
}

void Doc::setDefaultColumnWidth( double width )
{
    d->defaultColumnFormat->setWidth( width );
}

void Doc::setDefaultRowHeight( double height )
{
    d->defaultRowFormat->setHeight( height );
}

void Doc::addShape( KoShape* shape )
{
    if ( !shape )
        return;
    KoShape* parent = shape;
    SheetShapeContainer* shapeContainer = 0;
    while ( !shapeContainer && ( parent = parent->parent() ) )
    {
        shapeContainer = dynamic_cast<SheetShapeContainer*>( parent );
    }
    Q_ASSERT( shapeContainer );

    foreach ( KoView* view, views() )
    {
        Canvas* canvas = static_cast<View*>( view )->canvasWidget();
        if ( canvas->activeSheet()->shapeContainer() == shapeContainer )
            canvas->shapeManager()->add( shape );
    }
}

void Doc::removeShape( KoShape* shape )
{
    if ( !shape )
        return;
    KoShape* parent = shape;
    SheetShapeContainer* shapeContainer = 0;
    while ( !shapeContainer && ( parent = parent->parent() ) )
    {
        shapeContainer = dynamic_cast<SheetShapeContainer*>( parent );
    }
    Q_ASSERT( shapeContainer );

    foreach ( KoView* view, views() )
    {
        Canvas* canvas = static_cast<View*>( view )->canvasWidget();
        if ( canvas->activeSheet()->shapeContainer() == shapeContainer )
            canvas->shapeManager()->remove( shape );
    }
}

void Doc::saveConfig()
{
    if ( isEmbedded() ||!isReadWrite())
        return;
    KSharedConfigPtr config = Factory::global().config();
#ifdef KSPREAD_DOC_ZOOM
    config->group( "Parameters" ).writeEntry( "Zoom", zoomInPercent() );
#endif // KSPREAD_DOC_ZOOM
}

void Doc::initConfig()
{
    KSharedConfigPtr config = Factory::global().config();

    const int page = config->group( "KSpread Page Layout" ).readEntry( "Default unit page", 0 );
    setUnit( KoUnit( (KoUnit::Unit) page ) );

#if 0 // UNDOREDOLIMIT
    const int undo = config->group( "Misc" ).readEntry( "UndoRedo", 30 );
    setUndoRedoLimit( undo );
#endif

    const int zoom = config->group( "Parameters" ).readEntry( "Zoom", 100 );
#ifdef KSPREAD_DOC_ZOOM
    setZoomAndResolution( zoom, KoGlobal::dpiX(), KoGlobal::dpiY() );
#endif // KSPREAD_DOC_ZOOM
}

int Doc::syntaxVersion() const
{
  return d->syntaxVersion;
}

bool Doc::isLoading() const
{
    // The KoDocument state is necessary to avoid damages while importing a file (through a filter).
    return d->isLoading || KoDocument::isLoading();
}

QColor Doc::pageBorderColor() const
{
  return d->pageBorderColor;
}

void Doc::changePageBorderColor( const QColor  & _color)
{
  d->pageBorderColor = _color;
}

KCompletion& Doc::completion()
{
  return d->listCompletion;
}

KoView* Doc::createViewInstance( QWidget* parent)
{
    return new View( parent, this );
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

    QDomDocument doc = KoDocument::createDomDocument( "kspread", "spreadsheet", CURRENT_DTD_VERSION );
    QDomElement spread = doc.documentElement();
    spread.setAttribute( "editor", "KSpread" );
    spread.setAttribute( "mime", "application/x-kspread" );
    spread.setAttribute( "syntaxVersion", CURRENT_SYNTAX_VERSION );

    QDomElement dlocale = ((Localization *)locale())->save( doc );
    spread.appendChild( dlocale );

    QDomElement areaname = d->namedAreaManager->saveXML(doc);
    spread.appendChild(areaname);

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

    SavedDocParts::const_iterator iter = d->savedDocParts.begin();
    SavedDocParts::const_iterator end  = d->savedDocParts.end();
    while ( iter != end )
    {
      // save data we loaded in the beginning and which has no owner back to file
      spread.appendChild( iter.value() );
      ++iter;
    }

    QDomElement defaults = doc.createElement( "defaults" );
    defaults.setAttribute( "row-height", d->defaultRowFormat->height() );
    defaults.setAttribute( "col-width", d->defaultColumnFormat->width() );
    spread.appendChild( defaults );

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
    ElapsedTime et("OpenDocument Saving", ElapsedTime::PrintOnlyTime);

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
    KoGenStyles mainStyles;//for compile

    KTemporaryFile contentTmpFile;
    //Check that temp file was successfully created
    if (!contentTmpFile.open())
    {
      qWarning("Creation of temporary file to store document content failed.");
      return false;
    }

    KoXmlWriter* contentWriter = KoOdfWriteStore::createOasisXmlWriter( &dev, "office:document-content" );
    KoXmlWriter contentTmpWriter( &contentTmpFile, 1 );



    //todo fixme just add a element for testing saving content.xml
    contentTmpWriter.startElement( "office:body" );
    contentTmpWriter.startElement( "office:spreadsheet" );

    int indexObj = 1;
    int partIndexObj = 0;

    // Saving the custom cell styles including the default cell style.
    styleManager()->saveOasis( mainStyles );

    // Saving the default column style
    KoGenStyle defaultColumnStyle( KoGenStyle::StyleTableColumn, "table-column" );
    defaultColumnStyle.addPropertyPt( "style:column-width", d->defaultColumnFormat->width() );
    defaultColumnStyle.setDefaultStyle( true );
    mainStyles.lookup( defaultColumnStyle, "Default", KoGenStyles::DontForceNumbering );

    // Saving the default row style
    KoGenStyle defaultRowStyle( KoGenStyle::StyleTableRow, "table-row" );
    defaultRowStyle.addPropertyPt( "style:row-height", d->defaultRowFormat->height() );
    defaultRowStyle.setDefaultStyle( true );
    mainStyles.lookup( defaultRowStyle, "Default", KoGenStyles::DontForceNumbering );

    // Saving the map.
    map()->saveOasis( contentTmpWriter, mainStyles, store,  manifestWriter, indexObj, partIndexObj );

    d->namedAreaManager->saveOdf(contentTmpWriter);
    d->databaseManager->saveOdf(contentTmpWriter);
    contentTmpWriter.endElement(); ////office:spreadsheet
    contentTmpWriter.endElement(); ////office:body

    // Done with writing out the contents to the tempfile, we can now write out the automatic styles
    mainStyles.saveOdfAutomaticStyles( contentWriter, false );

    // And now we can copy over the contents from the tempfile to the real one
    contentTmpFile.close();
    contentWriter->addCompleteElement( &contentTmpFile );


    contentWriter->endElement(); // root element
    contentWriter->endDocument();
    delete contentWriter;
    if ( !store->close() )
        return false;
    //add manifest line for content.xml
    manifestWriter->addManifestEntry( "content.xml",  "text/xml" );

    mainStyles.saveOdfStylesDotXml( store, manifestWriter );

#if 0 // KSPREAD_KOPART_EMBEDDING
    makeUsedPixmapList();
    d->m_pictureCollection.saveOasisToStore( store, d->usedPictures, manifestWriter);
#endif // KSPREAD_KOPART_EMBEDDING

    if(!store->open("settings.xml"))
        return false;

    KoXmlWriter* settingsWriter = KoOdfWriteStore::createOasisXmlWriter(&dev, "office:document-settings");
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
#if 0 // KSPREAD_KOPART_EMBEDDING
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
#endif // KSPREAD_KOPART_EMBEDDING
    }


    setModified( false );

    return true;
}

void Doc::loadOasisSettings( const KoXmlDocument&settingsDoc )
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
        //kDebug()<<" ignorelist :"<<ignorelist;
        d->spellListIgnoreAll = ignorelist.split( ',', QString::SkipEmptyParts );
    }
}


bool Doc::loadOdf( KoOdfReadStore & odfStore )
{
    if ( !d->loadingInfo )
        d->loadingInfo = new LoadingInfo;

    QTime dt;
    dt.start();

    emit sigProgress( 0 );
    d->isLoading = true;
    connect(this, SIGNAL(completed()), this, SLOT(finishLoading()));
    d->spellListIgnoreAll.clear();

    KoXmlElement content = odfStore.contentDoc().documentElement();
    KoXmlElement realBody ( KoDom::namedItemNS( content, KoXmlNS::office, "body" ) );
    if ( realBody.isNull() )
    {
        setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No office:body tag found." ));
        deleteLoadingInfo();
        return false;
    }
    KoXmlElement body = KoDom::namedItemNS( realBody, KoXmlNS::office, "spreadsheet" );

    if ( body.isNull() )
    {
        kError(32001) << "No office:spreadsheet found!" << endl;
        KoXmlElement childElem;
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

    KoOasisLoadingContext context( this, odfStore.styles(), odfStore.store() );

    //load in first
    styleManager()->loadOasisStyleTemplate( odfStore.styles(), this );

    // load default column style
    const KoXmlElement* defaultColumnStyle = odfStore.styles().defaultStyle( "table-column" );
    if ( defaultColumnStyle )
    {
//       kDebug() <<"style:default-style style:family=\"table-column\"";
      KoStyleStack styleStack;
      styleStack.push( *defaultColumnStyle );
      styleStack.setTypeProperties( "table-column" );
      if ( styleStack.hasProperty( KoXmlNS::style, "column-width" ) )
      {
        const double width = KoUnit::parseValue( styleStack.property( KoXmlNS::style, "column-width" ), -1.0 );
        if ( width != -1.0 )
        {
//           kDebug() <<"\tstyle:column-width:" << width;
          d->defaultColumnFormat->setWidth( width );
        }
      }
    }

    // load default row style
    const KoXmlElement* defaultRowStyle = odfStore.styles().defaultStyle( "table-row" );
    if ( defaultRowStyle )
    {
//       kDebug() <<"style:default-style style:family=\"table-row\"";
      KoStyleStack styleStack;
      styleStack.push( *defaultRowStyle );
      styleStack.setTypeProperties( "table-row" );
      if ( styleStack.hasProperty( KoXmlNS::style, "row-height" ) )
      {
        const double height = KoUnit::parseValue( styleStack.property( KoXmlNS::style, "row-height" ), -1.0 );
        if ( height != -1.0 )
        {
//           kDebug() <<"\tstyle:row-height:" << height;
          d->defaultRowFormat->setHeight( height );
        }
      }
    }

    // TODO check versions and mimetypes etc.
    loadOasisCellValidation( body ); // table:content-validations
    loadOasisCalculationSettings( body ); // table::calculation-settings

    // all <sheet:sheet> goes to workbook
    if ( !map()->loadOasis( body, context ) )
    {
        d->isLoading = false;
        deleteLoadingInfo();
        return false;
    }

    // Load databases. This needs the sheets to be loaded.
    d->databaseManager->loadOdf(body); // table:database-ranges
    d->namedAreaManager->loadOdf(body); // table:named-expressions

    if ( !odfStore.settingsDoc().isNull() )
    {
        loadOasisSettings( odfStore.settingsDoc() );
    }
    initConfig();
    emit sigProgress(-1);

    //display loading time
    kDebug(36001) <<"Loading took" << (float)(dt.elapsed()) / 1000.0 <<" seconds";

    return true;
}

bool Doc::loadXML( QIODevice *, const KoXmlDocument& doc )
{
  QTime dt;
  dt.start();

  emit sigProgress( 0 );
  d->isLoading = true;
  connect(this, SIGNAL(completed()), this, SLOT(finishLoading()));
  d->spellListIgnoreAll.clear();
  // <spreadsheet>
  KoXmlElement spread = doc.documentElement();

  if ( spread.attribute( "mime" ) != "application/x-kspread" && spread.attribute( "mime" ) != "application/vnd.kde.kspread" )
  {
    d->isLoading = false;
    setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-kspread or application/vnd.kde.kspread, got %1" , spread.attribute("mime") ) );
    return false;
  }

    bool ok = false;
    int version = spread.attribute( "syntaxVersion" ).toInt( &ok );
    d->syntaxVersion = ok ? version : 0;
  if ( d->syntaxVersion > CURRENT_SYNTAX_VERSION )
  {
      int ret = KMessageBox::warningContinueCancel(
          0, i18n("This document was created with a newer version of KSpread (syntax version: %1)\n"
                  "When you open it with this version of KSpread, some information may be lost.",d->syntaxVersion),
          i18n("File Format Mismatch"), KStandardGuiItem::cont() );
      if ( ret == KMessageBox::Cancel )
      {
          setErrorMessage( "USER_CANCELED" );
          return false;
      }
  }

  // <locale>
  KoXmlElement loc = spread.namedItem( "locale" ).toElement();
  if ( !loc.isNull() )
      ((Localization *) locale())->load( loc );

  emit sigProgress( 5 );

  KoXmlElement defaults = spread.namedItem( "defaults" ).toElement();
  if ( !defaults.isNull() )
  {
    double dim = defaults.attribute( "row-height" ).toDouble( &ok );
    if ( !ok )
      return false;
    d->defaultRowFormat->setHeight( dim );

    dim = defaults.attribute( "col-width" ).toDouble( &ok );

    if ( !ok )
      return false;

    d->defaultColumnFormat->setWidth( dim );
  }

  KoXmlElement ignoreAll = spread.namedItem( "SPELLCHECKIGNORELIST").toElement();
  if ( !ignoreAll.isNull())
  {
      KoXmlElement spellWord=spread.namedItem("SPELLCHECKIGNORELIST").toElement();

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
  qDeleteAll(map()->sheetList());
  map()->sheetList().clear();

  KoXmlElement styles = spread.namedItem( "styles" ).toElement();
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
  KoXmlElement mymap = spread.namedItem( "map" ).toElement();
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

    // named areas
    const KoXmlElement areaname = spread.namedItem( "areaname" ).toElement();
    if (!areaname.isNull())
        d->namedAreaManager->loadXML(areaname);

  //Backwards compatibility with older versions for paper layout
  if ( d->syntaxVersion < 1 )
  {
    KoXmlElement paper = spread.namedItem( "paper" ).toElement();
    if ( !paper.isNull() )
    {
      loadPaper( paper );
    }
  }

  emit sigProgress( 85 );

  KoXmlElement element( spread.firstChild().toElement() );
  while ( !element.isNull() )
  {
    QString tagName( element.tagName() );

    if ( tagName != "locale" && tagName != "map" && tagName != "styles"
         && tagName != "SPELLCHECKIGNORELIST" && tagName != "areaname"
         && tagName != "paper" )
    {
      // belongs to a plugin, load it and save it for later use
      d->savedDocParts[ tagName ] = KoXml::asQDomElement(QDomDocument(), element);
    }

    element = element.nextSibling().toElement();
  }

  emit sigProgress( 90 );
  initConfig();
  emit sigProgress(-1);

   kDebug(36001) <<"Loading took" << (float)(dt.elapsed()) / 1000.0 <<" seconds";

  emit sig_refreshView();

  return true;
}

void Doc::loadPaper( KoXmlElement const & paper )
{
  // <paper>
  QString format = paper.attribute( "format" );
  QString orientation = paper.attribute( "orientation" );

  // <borders>
  KoXmlElement borders = paper.namedItem( "borders" ).toElement();
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
  KoXmlElement head = paper.namedItem( "head" ).toElement();
  if ( !head.isNull() )
  {
    KoXmlElement left = head.namedItem( "left" ).toElement();
    if ( !left.isNull() )
      hleft = left.text();
    KoXmlElement center = head.namedItem( "center" ).toElement();
    if ( !center.isNull() )
      hcenter = center.text();
    KoXmlElement right = head.namedItem( "right" ).toElement();
    if ( !right.isNull() )
      hright = right.text();
  }
  // <foot>
  KoXmlElement foot = paper.namedItem( "foot" ).toElement();
  if ( !foot.isNull() )
  {
    KoXmlElement left = foot.namedItem( "left" ).toElement();
    if ( !left.isNull() )
      fleft = left.text();
    KoXmlElement center = foot.namedItem( "center" ).toElement();
    if ( !center.isNull() )
      fcenter = center.text();
    KoXmlElement right = foot.namedItem( "right" ).toElement();
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

void Doc::finishLoading()
{
    // update all dependencies and recalc all cells
    addDamage(new WorkbookDamage(map(), WorkbookDamage::Formula | WorkbookDamage::Value));
    disconnect(this, SIGNAL(completed()), this, SLOT(finishLoading()));
}

bool Doc::completeLoading( KoStore* /* _store */ )
{
  kDebug(36001) <<"------------------------ COMPLETING --------------------";

  d->isLoading = false;

  foreach ( KoView* view, views() )
    static_cast<View *>( view )->initialPosition();

  kDebug(36001) <<"------------------------ COMPLETION DONE --------------------";

  setModified( false );
  return true;
}


bool Doc::docData( QString const & xmlTag, QDomElement & data )
{
  SavedDocParts::iterator iter = d->savedDocParts.find( xmlTag );
  if ( iter == d->savedDocParts.end() )
    return false;
  data = iter.value();
  d->savedDocParts.erase( iter );
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

KSpread::MoveTo Doc::moveToValue() const
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

void Doc::setKSpellConfig(K3SpellConfig _kspell)
{
    Q_UNUSED( _kspell );
#ifdef __GNUC__
#warning TODO KDE4 port to sonnet
#endif
#if 0
  if (d->spellConfig == 0 )
    d->spellConfig = new K3SpellConfig();

  d->spellConfig->setNoRootAffix(_kspell.noRootAffix ());
  d->spellConfig->setRunTogether(_kspell.runTogether ());
  d->spellConfig->setDictionary(_kspell.dictionary ());
  d->spellConfig->setDictFromList(_kspell.dictFromList());
  d->spellConfig->setEncoding(_kspell.encoding());
  d->spellConfig->setClient(_kspell.client());
#endif
}

K3SpellConfig * Doc::getKSpellConfig()
{
#ifdef __GNUC__
#warning TODO KDE4 port to sonnet
#endif
#if 0
    if (!d->spellConfig)
    {
        K3SpellConfig ksconfig;

        KSharedConfigPtr config = Factory::global().config();
        const KConfigGroup spellGroup = config->group( "KSpell kspread" );

        ksconfig.setNoRootAffix(spellGroup.readEntry("KSpell_NoRootAffix", 0));
        ksconfig.setRunTogether(spellGroup.readEntry("KSpell_RunTogether", 0));
        ksconfig.setDictionary(spellGroup.readEntry("KSpell_Dictionary", ""));
        ksconfig.setDictFromList(spellGroup.readEntry("KSpell_DictFromList", false));
        ksconfig.setEncoding(spellGroup.readEntry("KSpell_Encoding", int(KS_E_ASCII)));
        ksconfig.setClient(spellGroup.readEntry("KSpell_Client", int(KS_CLIENT_ISPELL)));
        setKSpellConfig(ksconfig);

        setDontCheckUpperWord(spellGroup.readEntry("KSpell_IgnoreUppercaseWords", false));
        setDontCheckTitleCase(spellGroup.readEntry("KSpell_IgnoreTitleCaseWords", false));
    }
#endif
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

void Doc::newZoomAndResolution( bool updateViews, bool /*forPrint*/ )
{
/*    layout();
    updateAllFrames();*/
    if ( updateViews )
    {
        emit sig_refreshView();
    }
}

void Doc::addCommand( QUndoCommand* command )
{
    if (undoLocked()) return;
    KoDocument::addCommand( command );
}

void Doc::addCommand( UndoAction* undo )
{
  if (undoLocked()) return;
  UndoWrapperCommand* command = new UndoWrapperCommand( undo );
  addCommand( command );
  setModified( true );
}

void Doc::setUndoLocked( bool lock )
{
  lock ? d->undoLocked++ : d->undoLocked--;
}

bool Doc::undoLocked() const
{
  return (d->undoLocked > 0);
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

void Doc::paintContent( QPainter& painter, const QRect& rect)
{
#ifdef KSPREAD_DOC_ZOOM
//     kDebug(36001) <<"paintContent() called on" << rect;

//     ElapsedTime et( "Doc::paintContent1" );

    // choose sheet: the first or the active
    Sheet* sheet = 0;
    if ( !d->activeSheet )
        sheet = map()->sheet( 0 );
    else
        sheet = d->activeSheet;
    if ( !sheet )
        return;

    // save current zoom
    double oldZoom = m_zoom;
    // set the resolution once
    setZoom( 1.0 / m_zoomedResolutionX);

    // KSpread support zoom, therefore no need to scale with worldMatrix
    // Save the translation though.
    QMatrix matrix = painter.matrix();
    matrix.setMatrix( 1, 0, 0, 1, matrix.dx(), matrix.dy() );

    // Unscale the rectangle.
    QRect prect = rect;
    prect.setWidth( (int) (prect.width() * painter.matrix().m11()) );
    prect.setHeight( (int) (prect.height() * painter.matrix().m22()) );

    // paint the content, now zoom is correctly set
    kDebug(36001)<<"paintContent-------------------------------------";
    painter.save();
    painter.setMatrix( matrix );
    paintContent( painter, prect, sheet, false );
    painter.restore();

    // restore zoom
    setZoom( oldZoom );
#endif // KSPREAD_DOC_ZOOM
}

void Doc::paintContent( QPainter& painter, const QRect& rect, Sheet* sheet, bool drawCursor )
{
#ifdef KSPREAD_DOC_ZOOM
    Q_UNUSED( drawCursor );

    if ( isLoading() )
        return;
    //    ElapsedTime et( "Doc::paintContent2" );

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
#endif // KSPREAD_DOC_ZOOM
}

void Doc::paintUpdates()
{
  foreach ( KoView* view, views() )
  {
    static_cast<View *>( view )->paintUpdates();
  }
#if 0 // KSPREAD_KOPART_EMBEDDING
  foreach ( Sheet* sheet, map()->sheetList() )
  {
    sheet->clearPaintDirtyData();
  }
#endif // KSPREAD_KOPART_EMBEDDING
}

void Doc::paintCellRegions( QPainter& painter, const QRect &viewRect,
                            View* view, const Region& region )
{
#ifdef KSPREAD_DOC_ZOOM
    //
    // Clip away children
    //
    QRegion rgn = painter.clipRegion();
    if ( rgn.isEmpty() )
        rgn = QRegion( QRect( 0, 0, viewRect.width(), viewRect.height() ) );

//   QMatrix matrix;
//   if ( view ) {
//     matrix.scale( zoomedResolutionX(),
//                   zoomedResolutionY() );
//     matrix.translate( - view->canvasWidget()->xOffset(),
//                       - view->canvasWidget()->yOffset() );
//   }
//   else {
//     matrix = painter.matrix();
//   }
//
//   QPtrListIterator<KoDocumentChild> it( children() );
//   for( ; it.current(); ++it ) {
//     // if ( ((Child*)it.current())->sheet() == sheet &&
//     //    !m_pView->hasDocumentInWindow( it.current()->document() ) )
//     if ( ((Child*)it.current())->sheet() == sheet)
//       rgn -= it.current()->region( matrix );
//   }
    painter.setClipRegion( rgn );

    Region::ConstIterator endOfList(region.constEnd());
    for (Region::ConstIterator it = region.constBegin(); it != endOfList; ++it)
    {
        paintRegion(painter, viewToDocument( viewRect ), view,(*it)->rect(), (*it)->sheet());
    }
#endif // KSPREAD_DOC_ZOOM
}

void Doc::paintRegion( QPainter &painter, const QRectF &viewRegion,
                       View* view, const QRect &cellRegion, const Sheet* sheet )
{
    // cellRegion has cell coordinates (col,row) while viewRegion has
    // world coordinates.  cellRegion is the cells to update and
    // viewRegion is the area actually onscreen.

    if ( cellRegion.left() <= 0 || cellRegion.top() <= 0 )
        return;

    const QRectF viewRegionF( viewRegion.left(), viewRegion.right(), viewRegion.width(), viewRegion.height() );

    // Get the world coordinates of the upper left corner of the
    // cellRegion The view is 0, when cellRegion is called from
    // paintContent, which itself is only called, when we should paint
    // the output for INACTIVE embedded view.  If inactive embedded,
    // then there is no view and we alwas start at top/left, so the
    // offset is 0.
    //
    QPointF topLeft;
    if ( view == 0 ) //Most propably we are embedded and inactive, so no offset
        topLeft = QPointF( sheet->columnPosition( cellRegion.left() ),
                           sheet->rowPosition( cellRegion.top() ) );
    else
        topLeft = QPointF( sheet->columnPosition( cellRegion.left() ) - view->canvasWidget()->xOffset(),
                           sheet->rowPosition( cellRegion.top() ) - view->canvasWidget()->yOffset() );

    SheetView sheetView( sheet ); // FIXME Stefan: make member, otherwise cache lost
    if ( view )
    {
        sheetView.setPaintDevice( view->canvasWidget() );
        sheetView.setViewConverter( view->zoomHandler() );
    }
    sheetView.setPaintCellRange( cellRegion );
    sheetView.paintCells( view ? view->canvasWidget() : 0, painter, viewRegionF, topLeft );
}

void Doc::loadOasisCellValidation( const KoXmlElement&body )
{
    KoXmlNode validation = KoDom::namedItemNS( body, KoXmlNS::table, "content-validations" );
    kDebug()<<"void Doc::loadOasisCellValidation( const KoXmlElement&body )";
    kDebug()<<"validation.isNull ?"<<validation.isNull();
    if ( !validation.isNull() )
    {
        KoXmlNode n = validation.firstChild();
        for( ; !n.isNull(); n = n.nextSibling() )
        {
            if ( n.isElement() )
            {
                KoXmlElement element = n.toElement();
                //kDebug()<<" loadOasisCellValidation element.tagName() :"<<element.tagName();
                if ( element.tagName() ==  "content-validation" && element.namespaceURI() == KoXmlNS::table ) {
                    d->loadingInfo->appendValidation(element.attributeNS( KoXmlNS::table, "name", QString() ), element );
                    kDebug()<<" validation found :"<<element.attributeNS( KoXmlNS::table,"name", QString() );
                }
                else {
                    kDebug()<<" Tag not recognize :"<<element.tagName();
                }
            }
        }
    }
}

void Doc::loadOasisCalculationSettings( const KoXmlElement& body )
{
    KoXmlNode settings = KoDom::namedItemNS( body, KoXmlNS::table, "calculation-settings" );
    kDebug() <<"Calculation settings found?"<< !settings.isNull();
    if ( !settings.isNull() )
    {
        KoXmlElement element = settings.toElement();
        if ( element.hasAttributeNS( KoXmlNS::table,  "case-sensitive" ) )
        {
            d->caseSensitiveComparisons = true;
            QString value = element.attributeNS( KoXmlNS::table, "case-sensitive", "true" );
            if ( value == "false" )
                d->caseSensitiveComparisons = false;
        }
        else if ( element.hasAttributeNS( KoXmlNS::table, "precision-as-shown" ) )
        {
            d->precisionAsShown = false;
            QString value = element.attributeNS( KoXmlNS::table, "precision-as-shown", "false" );
            if ( value == "true" )
                d->precisionAsShown = true;
        }
        else if ( element.hasAttributeNS( KoXmlNS::table, "search-criteria-must-apply-to-whole-cell" ) )
        {
            d->wholeCellSearchCriteria = true;
            QString value = element.attributeNS( KoXmlNS::table, "search-criteria-must-apply-to-whole-cell", "true" );
            if ( value == "false" )
                d->wholeCellSearchCriteria = false;
        }
        else if ( element.hasAttributeNS( KoXmlNS::table, "automatic-find-labels" ) )
        {
            d->automaticFindLabels = true;
            QString value = element.attributeNS( KoXmlNS::table, "automatic-find-labels", "true" );
            if ( value == "false" )
                d->automaticFindLabels = false;
        }
        else if ( element.hasAttributeNS( KoXmlNS::table, "use-regular-expressions" ) )
        {
            d->useRegularExpressions = true;
            QString value = element.attributeNS( KoXmlNS::table, "use-regular-expressions", "true" );
            if ( value == "false" )
                d->useRegularExpressions = false;
        }
        else if ( element.hasAttributeNS( KoXmlNS::table, "null-year" ) )
        {
            d->refYear = 1930;
            QString value = element.attributeNS( KoXmlNS::table, "null-year", "1930" );
            if ( value == "false" )
                d->refYear = false;
        }

        forEachElement( element, settings )
        {
            if ( element.namespaceURI() != KoXmlNS::table )
                continue;
            else if ( element.tagName() ==  "null-date" )
            {
                d->refDate = QDate( 1899, 12, 30 );
                QString valueType = element.attributeNS( KoXmlNS::table, "value-type", "date" );
                if( valueType == "date" )
                {
                    QString value = element.attributeNS( KoXmlNS::table, "date-value", "1899-12-30" );
                    QDate date = QDate::fromString( value, Qt::ISODate );
                    if ( date.isValid() )
                        d->refDate = date;
                }
                else
                {
                    kDebug() <<"Doc: Error on loading null date."
                             << "Value type """ << valueType << """ not handled"
                             << ", falling back to default." << endl;
                    // NOTE Stefan: I don't know why different types are possible here!
                }
            }
            else if ( element.tagName() ==  "iteration" )
            {
                // TODO
            }
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

  KoDocument::emitEndOperation();

  QApplication::restoreOverrideCursor();

    // Do this after the parent class emitEndOperation,
    // because that allows updates on the view again.
    paintUpdates();
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

    d->namedAreaManager->remove(sheet);
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

LoadingInfo * Doc::loadingInfo() const
{
    return d->loadingInfo;
}

void Doc::deleteLoadingInfo()
{
    delete d->loadingInfo;
    d->loadingInfo = 0;
}

void Doc::addView( KoView *_view )
{
  KoDocument::addView( _view );
  foreach ( KoView* view, views() )
    static_cast<View*>( view )->closeEditor();
}

void Doc::addDamage( Damage* damage )
{
    // Do not create a new Damage, if we are in loading process. Check for it before
    // calling this function. This prevents unnecessary memory allocations (new).
    Q_ASSERT( !isLoading() );
    Q_CHECK_PTR( damage );

    if ( damage->type() == Damage::Cell )
        kDebug(36007) <<"Adding\t" << *static_cast<CellDamage*>(damage);
    else if ( damage->type() == Damage::Sheet )
        kDebug(36007) <<"Adding\t" << *static_cast<SheetDamage*>(damage);
    else if ( damage->type() == Damage::Selection )
        kDebug(36007) <<"Adding\t" << *static_cast<SelectionDamage*>(damage);
    else
        kDebug(36007) <<"Adding\t" << *damage;

    d->damages.append( damage );

    if( d->damages.count() == 1 )
        QTimer::singleShot( 0, this, SLOT( flushDamages() ) );
}

void Doc::flushDamages()
{
    // Copy the damages to process. This allows new damages while processing.
    QList<Damage*> damages = d->damages;
    d->damages.clear();
    emit damagesFlushed( damages );
    qDeleteAll( damages );
    damages.clear();
}

void Doc::handleDamages( const QList<Damage*>& damages )
{
    Region bindingChangedRegion;
    Region formulaChangedRegion;
    Region namedAreaChangedRegion;
    Region valueChangedRegion;
    WorkbookDamage::Changes workbookChanges = WorkbookDamage::None;

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

            if ((cellDamage->changes() & CellDamage::Binding) &&
                 !workbookChanges.testFlag(WorkbookDamage::Value))
            {
                bindingChangedRegion.add(region, damagedSheet);
            }
            if ( ( cellDamage->changes() & CellDamage::Formula ) &&
                 !workbookChanges.testFlag( WorkbookDamage::Formula ) )
            {
                formulaChangedRegion.add( region, damagedSheet );
            }
            if ( ( cellDamage->changes() & CellDamage::NamedArea ) &&
                 !workbookChanges.testFlag( WorkbookDamage::Formula ) )
            {
                namedAreaChangedRegion.add( region, damagedSheet );
            }
            if ( ( cellDamage->changes() & CellDamage::Value ) &&
                 !workbookChanges.testFlag( WorkbookDamage::Value ) )
            {
                valueChangedRegion.add( region, damagedSheet );
            }
            continue;
        }

        if( damage->type() == Damage::Sheet )
        {
            SheetDamage* sheetDamage = static_cast<SheetDamage*>( damage );
            kDebug(36007) <<"Processing\t" << *sheetDamage;
//             Sheet* damagedSheet = sheetDamage->sheet();

            if ( sheetDamage->changes() & SheetDamage::PropertiesChanged )
            {
            }
            continue;
        }

        if( damage->type() == Damage::Workbook )
        {
            WorkbookDamage* workbookDamage = static_cast<WorkbookDamage*>( damage );
            kDebug(36007) <<"Processing\t" << *damage;

            workbookChanges |= workbookDamage->changes();
            if ( workbookDamage->changes() & WorkbookDamage::Formula )
                formulaChangedRegion.clear();
            if ( workbookDamage->changes() & WorkbookDamage::Value )
                valueChangedRegion.clear();
            continue;
        }

        kDebug(36007) <<"Unhandled\t" << *damage;
    }

    // Update the named areas.
    if (!namedAreaChangedRegion.isEmpty())
        namedAreaManager()->regionChanged(namedAreaChangedRegion);
    // First, update the dependencies.
    if ( !formulaChangedRegion.isEmpty() )
        dependencyManager()->regionChanged( formulaChangedRegion );
    // Tell the RecalcManager which cells have had a value change.
    if ( !valueChangedRegion.isEmpty() )
        recalcManager()->regionChanged( valueChangedRegion );
    if ( workbookChanges.testFlag( WorkbookDamage::Formula ) )
    {
        namedAreaManager()->updateAllNamedAreas();
        dependencyManager()->updateAllDependencies( map() );
    }
    if ( workbookChanges.testFlag( WorkbookDamage::Value ) )
    {
        recalcManager()->recalcMap();
        bindingManager()->updateAllBindings();
    }
    // Update the bindings
    if (!bindingChangedRegion.isEmpty())
        bindingManager()->regionChanged(bindingChangedRegion);
}

void Doc::loadConfigFromFile()
{
    d->configLoadFromFile = true;
}

bool Doc::configLoadFromFile() const
{
    return d->configLoadFromFile;
}

void Doc::setCaptureAllArrowKeys( bool capture )
{
    d->captureAllArrowKeys = capture;
}

bool Doc::captureAllArrowKeys() const
{
    return d->captureAllArrowKeys;
}


#if 0 // KSPREAD_KOPART_EMBEDDING
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

void Doc::repaint( EmbeddedObject *obj )
{
  foreach ( KoView* view, views() )
  {
    Canvas* canvas = static_cast<View*>( view )->canvasWidget();
    if ( obj->sheet() == canvas->activeSheet() )
        canvas->repaintObject( obj );
  }
}
#endif // KSPREAD_KOPART_EMBEDDING

void Doc::repaint( const QRectF& rect )
{
    QRectF r;
    foreach ( KoView* koview, views() )
    {
        const View* view = static_cast<View*>( koview );
        Canvas* canvas = view->canvasWidget();

        r = view->zoomHandler()->documentToView( rect );
        r.translate( -canvas->xOffset() * view->zoomHandler()->zoomedResolutionX(),
                     -canvas->yOffset() * view->zoomHandler()->zoomedResolutionY() );
        canvas->update( r.toRect() );
    }
}


#if 0 // UNDOREDOLIMIT
int Doc::undoRedoLimit() const
{
  return d->commandHistory->undoLimit();
}

void Doc::setUndoRedoLimit(int val)
{
  d->commandHistory->setUndoLimit(val);
  d->commandHistory->setRedoLimit(val);
}
#endif

void Doc::setReferenceYear( int year )
{
    if ( year < 100)
       d->refYear = 1900 + year;
    else
       d->refYear = year;
}

int Doc::referenceYear() const
{
    return d->refYear;
}

void Doc::setReferenceDate( const QDate& date )
{
    if ( !date.isValid() ) return;
    d->refDate.setDate( date.year(), date.month(), date.day() );
}

QDate Doc::referenceDate() const
{
    return d->refDate;
}

void Doc::setDefaultDecimalPrecision( int precision )
{
    d->precision = ( precision < 0 ) ? 8 : precision;
}

int Doc::defaultDecimalPrecision() const
{
    return d->precision;
}

#if 0 // KSPREAD_KOPART_EMBEDDING
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
#endif // KSPREAD_KOPART_EMBEDDING

bool Doc::savingWholeDocument()
{
    return d->m_savingWholeDocument;
}

#include "Doc.moc"

