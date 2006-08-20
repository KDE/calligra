/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <qdom.h>
#include <qtextstream.h>
#include <qbuffer.h>
#include <qtabwidget.h>
#include <qpaintdevicemetrics.h>

#include "kivio_doc.h"
#include "kivio_page.h"
#include "kivio_map.h"
#include "kivio_view.h"
#include "kivio_factory.h"
#include "export_page_dialog.h"
#include "kivio_common.h"
#include "kivio_group_stencil.h"
#include "kivio_icon_view.h"
#include "kivio_layer.h"
#include "kivio_painter.h"
#include "kivio_screen_painter.h"
#include "kivio_stencil.h"
#include "kivio_stencil_spawner_set.h"
#include "kivioglobal.h"
#include "kivio_config.h"
#include "polylineconnectorspawner.h"
#include "kivio_canvas.h"

#include "stencilbarbutton.h"

#include <unistd.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kprinter.h>
#include <kdebug.h>
#include <kurl.h>
#include <kapplication.h>
#include <assert.h>
#include <qdatetime.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <QDir>
#include <qfileinfo.h>
#include <QFile>
#include <qpixmap.h>

#include <qpainter.h>
#include <qpen.h>
#include <QFont>
#include <qvaluelist.h>


#include <kstandarddirs.h>
#include <kmenu.h>
#include <kmenubar.h>
#include <kiconloader.h>
#include <khelpmenu.h>
#include <kconfig.h>
#include <KoTemplateChooseDia.h>
#include <KoFilterManager.h>
#include <KoStoreDevice.h>
#include "KIvioDocIface.h"
#include <kcommand.h>
#include <KoZoomHandler.h>
#include <KoApplication.h>
#include <kglobal.h>
#include <KoCommandHistory.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoOasisSettings.h>
#include <KoDom.h>
#include <KoXmlNS.h>

//using namespace std;

/*****************************************************************************
 *
 * KivioDoc
 *
 *****************************************************************************/

QPtrList<KivioDoc>* KivioDoc::s_docs = 0;
int KivioDoc::s_docId = 0;

KivioDoc::KivioDoc( QWidget *parentWidget, const char* widgetName, QObject* parent, const char* name, bool singleViewMode )
: KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
  dcop = 0;
  if (!s_docs)
    s_docs = new QPtrList<KivioDoc>;

  s_docs->append(this);

  m_pLstSpawnerSets = new QPtrList<KivioStencilSpawnerSet>;
  m_pLstSpawnerSets->setAutoDelete(true);
  m_loadTimer = 0;
  m_currentFile = 0;

  setInstance( KivioFactory::global(), false );
  setTemplateType("kivio_template");

  if ( !name )
  {
    QString tmp( "Document%1" );	//lukas: FIXME
    tmp = tmp.arg( s_docId++ );
    setName( tmp.latin1() );
  }

  m_iPageId = 1;
  m_bLoading = false;
  m_pMap = new KivioMap( this, "Map" );

  // Load autoLoadStencils in internal StencilSpawnerSet
  m_pInternalSet = new KivioStencilSpawnerSet("Kivio_Internal");
  m_pInternalSet->setId("Kivio - Internal - Do Not Touch");
  QStringList list = instance()->dirs()->findAllResources("data",instance()->instanceName()+"/autoloadStencils/*",true,false);
  QStringList::ConstIterator pIt = list.begin();
  QStringList::ConstIterator pEnd = list.end();

  for (; pIt != pEnd; ++pIt )
  {
    m_pInternalSet->loadFile(*pIt);
  }

  // Add the polyline connector spawner to the internal stencil set.
  addInternalStencilSpawner(new Kivio::PolyLineConnectorSpawner(internalSpawnerSet()));

  initConfig();

  //laurent: Why don't use menu history for undo/redo command ? sync with other koffice application
  m_commandHistory = new KoCommandHistory( actionCollection(),  /*false*/true  ) ;
  connect( m_commandHistory, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
  connect( m_commandHistory, SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );


  if ( name )
    dcopObject();
}

DCOPObject* KivioDoc::dcopObject()
{
  if ( !dcop ) {
    dcop = new KIvioDocIface( this );
  }

  return dcop;
}

QPtrList<KivioDoc>& KivioDoc::documents()
{
  if ( s_docs == 0 ) {
    s_docs = new QPtrList<KivioDoc>;
  }

  return *s_docs;
}

bool KivioDoc::initDoc(InitDocFlags flags, QWidget* parentWidget)
{
  KivioPage *t = createPage();
  m_pMap->addPage( t );
  m_docOpened = false; // Used to for a hack that make kivio not crash if you cancel startup dialog.

  if(flags == KoDocument::InitDocEmpty) {
    setEmpty();
    m_docOpened = true; // Used to for a hack that make kivio not crash if you cancel startup dialog.
    return true;
  }

  QString f;
  KoTemplateChooseDia::ReturnType ret;
  KoTemplateChooseDia::DialogType dlgtype;

  if(flags != KoDocument::InitDocFileNew) {
    dlgtype = KoTemplateChooseDia::Everything;
    initConfig();
  } else {
    dlgtype = KoTemplateChooseDia::OnlyTemplates;
  }

  ret = KoTemplateChooseDia::choose( KivioFactory::global(), f,
                                     dlgtype, "kivio_template", parentWidget );

  if( ret == KoTemplateChooseDia::File ) {
    KUrl url(f);
    bool ok = openURL(url);
    m_docOpened = ok; // Used to for a hack that make kivio not crash if you cancel startup dialog.
    return ok;
  } else if ( ret == KoTemplateChooseDia::Template ) {
    QFileInfo fileInfo( f );
    QString fileName( fileInfo.dirPath(true) + "/" + fileInfo.baseName() + ".kft" );
    resetURL();
    bool ok = loadNativeFormat( fileName );
    if ( !ok )
        showLoadingErrorDialog();
    setEmpty();
    m_docOpened = ok; // Used to for a hack that make kivio not crash if you cancel startup dialog.
    return ok;
  } else if ( ret == KoTemplateChooseDia::Empty ) {
    setEmpty();
    m_docOpened = true; // Used to for a hack that make kivio not crash if you cancel startup dialog.
    return true;
  } else {
    return false;
  }
}

void KivioDoc::openExistingFile( const QString& file )
{
  KivioPage* t = createPage();
  m_pMap->addPage(t);

  KoDocument::openExistingFile(file);
}

void KivioDoc::openTemplate( const QString& file )
{
  KivioPage* t = createPage();
  m_pMap->addPage(t);

  KoDocument::openTemplate(file);
}

void KivioDoc::initEmpty()
{
  KivioPage* t = createPage();
  m_pMap->addPage(t);
  KoDocument::initEmpty();
}

KoView* KivioDoc::createViewInstance( QWidget* parent )
{
  return new KivioView( parent, this );
}

QDomDocument KivioDoc::saveXML()
{
  QDomDocument doc( "kiviodoc" );
  doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
  QDomElement kivio = doc.createElement( "kiviosheet" );
  kivio.setAttribute( "editor", "Kivio" );
  kivio.setAttribute( "mime", MIME_TYPE );

  kivio.setAttribute( "units", unitName() );
  gridData.save(kivio,"grid");

  QDomElement viewItemsElement = doc.createElement("ViewItems");
  kivio.appendChild(viewItemsElement);

  doc.appendChild(kivio);

  // Save the list of stencils spawners we have loaded.
  // We save these as the following:
  //
  // <KivioStencilSpawnerSet  id="whatever the id is">
  //   <KivioSMLStencilSpawner id="whatever the id is"/>
  //   <KivioSMLStencilSpawner id="whatever the id is"/>
  // </KivioStencilSpawnerSet>
  // ....
  //
  // This is so we can load them back in, and check that we actually
  // have all these spawners on disk.
  KivioStencilSpawnerSet *pSet = m_pLstSpawnerSets->first();
  while( pSet )
  {
    if(checkStencilsForSpawnerSet(pSet)) {
      kivio.appendChild(pSet->saveXML(doc));
    }

    pSet = m_pLstSpawnerSets->next();
  }


  QDomElement e = m_pMap->save(doc);
  kivio.appendChild(e);

  setModified(false);
  return doc;
}

bool KivioDoc::saveOasis(KoStore* store, KoXmlWriter* manifestWriter)
{
    KoStoreDevice storeDev(store);
    KoGenStyles styles;

    KoGenStyle pageLayout = Kivio::Config::defaultPageLayout().saveOasis();
    QString layoutName = styles.lookup(pageLayout, "PL");
    KoGenStyle masterPage(KoGenStyle::STYLE_MASTER);
    masterPage.addAttribute("style:page-layout-name", layoutName);
    styles.lookup(masterPage, "Standard", false);

    if(!store->open("content.xml")) {
        return false;
    }

    KoXmlWriter* docWriter = createOasisXmlWriter(&storeDev, "office:document-content");

    docWriter->startElement("office:body");
    docWriter->startElement("office:drawing");

    m_pMap->saveOasis(store, docWriter, &styles); // Save contents

    docWriter->endElement(); // office:drawing
    docWriter->endElement(); // office:body
    docWriter->endElement(); // Root element
    docWriter->endDocument();
    delete docWriter;

    if(!store->close()) {
        return false;
    }

    manifestWriter->addManifestEntry("content.xml", "text/xml");

    if(!store->open("styles.xml")) {
        return false;
    }

    KoXmlWriter* styleWriter = createOasisXmlWriter(&storeDev, "office:document-styles");

    styleWriter->startElement("office:automatic-styles");

    QValueList<KoGenStyles::NamedStyle> styleList = styles.styles(KoGenStyle::STYLE_PAGELAYOUT);
    QValueList<KoGenStyles::NamedStyle>::const_iterator it = styleList.begin();

    for ( ; it != styleList.end(); ++it) {
        (*it).style->writeStyle(styleWriter, styles, "style:page-layout", (*it).name, "style:page-layout-properties");
    }

    styleList = styles.styles(Kivio::STYLE_PAGE);
    it = styleList.begin();

    for ( ; it != styleList.end(); ++it) {
        (*it).style->writeStyle(styleWriter, styles, "style:style", (*it).name, "style:properties");
    }

    styleWriter->endElement(); // office:automatic-styles

    styleList = styles.styles(KoGenStyle::STYLE_MASTER);
    it = styleList.begin();
    styleWriter->startElement("office:master-styles");

    for ( ; it != styleList.end(); ++it) {
        (*it).style->writeStyle(styleWriter, styles, "style:master-page", (*it).name, "");
    }

    styleWriter->endElement(); // office:master-styles

    styleWriter->endElement(); // Root element
    styleWriter->endDocument();
    delete styleWriter;

    if(!store->close()) {
        return false;
    }

    manifestWriter->addManifestEntry("styles.xml", "text/xml");

    if(!store->open("settings.xml")) {
        return false;
    }


    KoXmlWriter* settingsWriter = createOasisXmlWriter(&storeDev, "office:document-settings");
    settingsWriter->startElement("office:settings");
    settingsWriter->startElement("config:config-item-set");
    settingsWriter->addAttribute("config:name", "view-settings");

    KoUnit::saveOasis( settingsWriter, unit() );
    saveOasisSettings( *settingsWriter );

    settingsWriter->endElement(); // config:config-item-set
    settingsWriter->endElement(); // office:settings
    settingsWriter->endDocument();

    delete settingsWriter;

    if(!store->close()) {
        return false;
    }

    manifestWriter->addManifestEntry("settings.xml", "text/xml");

    setModified(false);
    return true;
}

void KivioDoc::saveOasisSettings( KoXmlWriter &/*settingsWriter*/ )
{
    //todo
}

bool KivioDoc::loadOasis( const QDomDocument& doc, KoOasisStyles& oasisStyles, const QDomDocument& settings, KoStore* )
{
  kDebug(43000) << "Start loading OASIS document..." << endl;
  m_bLoading = true;

  QDomElement contents = doc.documentElement();
  QDomElement body(KoDom::namedItemNS( contents, KoXmlNS::office, "body"));

  if(body.isNull()) {
    kDebug(43000) << "No office:body found!" << endl;
    setErrorMessage(i18n("Invalid OASIS document. No office:body tag found."));
    m_bLoading = false;
    return false;
  }

  body = KoDom::namedItemNS( body, KoXmlNS::office, "drawing");

  if(body.isNull()) {
    kDebug(43000) << "No office:drawing found!" << endl;
    setErrorMessage(i18n("Invalid OASIS document. No office:drawing tag found."));
    m_bLoading = false;
    return false;
  }

  QDomNode node = body.firstChild();
  QString localName;
  m_pMap->clear();

  // TODO: port to forEachElement
  while(!node.isNull()) {
    localName = node.localName();

    if(localName == "page" /* && and namespace is KoXmlNS::draw*/) {
      KivioPage* p = createPage();
      addPage(p);

      if(!p->loadOasis(node.toElement(), oasisStyles)) {
        m_bLoading = false;
        return false;
      }
    }

    node = node.nextSibling();
  }

  loadOasisSettings( settings );

  emit loadingFinished();
  return true;
}


void KivioDoc::loadOasisSettings( const QDomDocument&settingsDoc )
{
    if ( settingsDoc.isNull() )
        return ; //not a error some file doesn't have settings.xml
    KoOasisSettings settings( settingsDoc );
    KoOasisSettings::Items viewSettings = settings.itemSet( "view-settings" );
    if ( !viewSettings.isNull() )
    {
        setUnit(KoUnit::unit(viewSettings.parseConfigItemString("unit")));
        //todo add other config here.
    }
}

bool KivioDoc::loadXML( QIODevice *, const QDomDocument& doc )
{
  m_bLoading = true;

  if ( doc.doctype().name() != "kiviodoc" ) {
    m_bLoading = false;
    return false;
  }

  QDomElement kivio = doc.documentElement();
  if ( kivio.attribute( "mime" ) != "application/x-kivio" &&
    kivio.attribute( "mime" ) != "application/vnd.kde.kivio" )
  {
    kDebug(43000) << "KivioDoc::loadXML() - Invalid mime type" << endl;
    m_bLoading = false;
    return false;
  }

  QDomNode node = kivio.firstChild();
  while( !node.isNull() )
  {
    QString name = node.nodeName();
    if( name == "KivioMap" )
    {
        if( !m_pMap->loadXML( node.toElement() ) )
        {
            m_bLoading = false;
            return false;
        }
    }
    else if( name == "KivioStencilSpawnerSet" )
    {
        QString id = XmlReadString( node.toElement(), "id", "" );
        bool hidden = (XmlReadString(node.toElement(), "hidden", "false") == "true");

        if( id.isEmpty() )
        {
            kDebug(43000) << "KivioDoc::loadXML() - Bad KivioStencilSpawnerSet found, it contains no id!" << endl;
        }
        else
        {
            loadStencilSpawnerSet(id, hidden);
        }
    }
    else if( name == "Options" )
    {
        // Not used anymore use users default page instead from Settings to create new pages...
    }
    else
    {
       kDebug(43000) << "KivioDoc::loadXML() - Unknown node " <<  name << endl;
    }

    node = node.nextSibling();
  }

  QString us = kivio.attribute("units", Kivio::Config::unit());
  bool isInt = false;
  int u = us.toInt(&isInt);

  if(!isInt) {
    setUnit(KoUnit::unit(us));
  } else {
    setUnit(Kivio::convToKoUnit(u));
  }

  if(kivio.hasAttribute("gridIsShow")) {
    gridData.load(kivio,"grid");
  }

  emit loadingFinished();
  return true;
}

bool KivioDoc::loadStencilSpawnerSet(const QString &id, bool hidden)
{
  KStandardDirs *dirs = KGlobal::dirs();
  QStringList dirList = dirs->findDirs("data", "kivio/stencils");
  QString rootDir;

  // Iterate through all data directories
  for( QStringList::Iterator it = dirList.begin(); it != dirList.end(); ++it )
  {
    rootDir = (*it);

    // Within each data directory, iterate through all directories looking
    // for a filename (dir) that matches the parameter
    QDir d(rootDir);
    d.setFilter( QDir::Dirs );
    d.setSorting( QDir::Name );

    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator listIT( *list );
    QFileInfo *fi;

    // Loop through the outer directories (like BasicFlowcharting)
    while( (fi=listIT.current()) )
    {
      if( fi->fileName() != "." &&
          fi->fileName() != ".." )
      {
        QDir innerD(fi->absoluteFilePath() );
        innerD.setFilter( QDir::Dirs );
        innerD.setSorting( QDir::Name );

        const QFileInfoList *innerList = innerD.entryInfoList();
        QFileInfoListIterator innerIT( *innerList );
        QFileInfo *innerFI;

        // Loop through the inner directories (like FlowChartingShapes1)
        while( (innerFI = innerIT.current()) )
        {
          if( innerFI->fileName() != ".." &&
              innerFI->fileName() != "." )
          {
            // Compare the descriptions
            QString foundId;

            // TODO: use ID system here for loading
            foundId = KivioStencilSpawnerSet::readId(innerFI->absoluteFilePath());
            if( foundId == id)
            {

              // Load the spawner set with  rootDir + "/" + fi.fileName()
              addSpawnerSetDuringLoad(innerFI->absoluteFilePath(), hidden);
              return true;
            }
          }

          ++innerIT;
        }
      }

      ++listIT;
    }
  }

  return false;
}

bool KivioDoc::completeLoading( KoStore* )
{
  m_bLoading = false;
  m_pMap->update();
  setModified( false );
  return true;
}

KivioPage* KivioDoc::createPage()
{
  QString s( i18n("Page%1") );
  s = s.arg( m_iPageId++ );

  KivioPage* t = new KivioPage(m_pMap, s);
  t->setPageName(s,true);

  return t;
}

void KivioDoc::addPage( KivioPage* page )
{
  m_pMap->addPage(page);
  setModified(true);
  emit sig_addPage(page);
}

void KivioDoc::insertPage( KivioPage * page )
{
    foreach ( KoView* view, views() )
        static_cast<KivioView*>(view)->insertPage( page );
}

void KivioDoc::takePage( KivioPage * page )
{
    foreach ( KoView* view, views() )
        static_cast<KivioView*>(view)->removePage( page );
}


void KivioDoc::paintContent( QPainter& painter, const QRect& rect, bool transparent, double /*zoomX*/, double /*zoomY*/ )
{
  KivioPage* page = m_pMap->firstPage();
  if ( !page )
    return;

  KoZoomHandler zoom;
  zoom.setZoomAndResolution(100, KoGlobal::dpiX(),
    KoGlobal::dpiY());
  KoRect r = page->getRectForAllStencils();

  float zw = (float) rect.width() / (float)zoom.zoomItX(r.width());
  float zh = (float) rect.height() / (float)zoom.zoomItY(r.height());
  float z = qMin(zw, zh);
  //kDebug(43000) << "paintContent: w = " << rect.width() << " h = " << rect.height() << endl;

  zoom.setZoomAndResolution(qRound(z * 100), KoGlobal::dpiX(),
    KoGlobal::dpiY());
  KivioScreenPainter ksp(&painter);
  ksp.painter()->translate( - zoom.zoomItX(r.x()), - zoom.zoomItY(r.y()) );
  paintContent(ksp,rect,transparent,page, QPoint(zoom.zoomItX(r.x()), zoom.zoomItY(r.y())), &zoom, false, false);
  ksp.setPainter(0L); // Important! Don't delete the QPainter!!!
}

void KivioDoc::paintContent( KivioPainter& painter, const QRect& rect, bool transparent, KivioPage* page, QPoint p0, KoZoomHandler* zoom, bool drawConnectorTargets, bool drawSelection )
{
  if ( isLoading() )
    return;

  page->paintContent(painter,rect,transparent,p0,zoom, drawConnectorTargets, drawSelection);
}

void KivioDoc::printContent( KPrinter &prn )
{
  KivioScreenPainter p;
  QValueList<int> pages = prn.pageList();
  KivioPage *pPage;

  // ### HACK: disable zooming-when-printing if embedded parts are used.
  // No koffice app supports zooming in paintContent currently.
  // Disable in ALL cases now
  bool doZoom = false;
  int dpiX = doZoom ? 300 : KoGlobal::dpiX();
  int dpiY = doZoom ? 300 : KoGlobal::dpiY();

  p.start(&prn);

  QPaintDeviceMetrics metrics( &prn );
  p.painter()->scale( (double)metrics.logicalDpiX() / (double)dpiX,
    (double)metrics.logicalDpiY() / (double)dpiY );

  QValueList<int>::iterator it;

  for(it = pages.begin(); it != pages.end(); ++it) {
    pPage = m_pMap->pageList().at((*it)-1);
    pPage->printContent(p, dpiX, dpiY);

    if( it != (--pages.end()) ) {
      prn.newPage();
    }
  }

  p.stop();
}

// TODO: Fix for id system
KivioStencilSpawnerSet* KivioDoc::findSpawnerSet(const QString& dirName, const QString& id)
{
    KivioStencilSpawnerSet *pSet = m_pLstSpawnerSets->first();
    while(pSet)
    {
        if( pSet->dir() == dirName || pSet->id() == id )
        {
            return pSet;
        }

        pSet = m_pLstSpawnerSets->next();
    }

    return 0;
}

void KivioDoc::addSpawnerSet( const QString &dirName )
{
  QString id = KivioStencilSpawnerSet::readId( dirName );
  KivioStencilSpawnerSet* set = findSpawnerSet(dirName, id);

  if(set)
  {
    // Unhide the set if it's only hidden
    if(set->hidden()) {
      set->setHidden(false);
      emit sig_addSpawnerSet(set);
    }

    return;
  }


  set = new KivioStencilSpawnerSet();

  if(!set->loadDir(dirName))
  {
    kDebug(43000) << "KivioDoc::addSpawnerSet() - Error loading dir set" << endl;
    delete set;
    return;
  }

  // Queue set for loading stencils
  m_stencilSetLoadQueue.append(set);

  if(!m_loadTimer) {
    m_loadTimer = new QTimer(this);
    connect(m_loadTimer, SIGNAL(timeout()), this, SLOT(loadStencil()));
  }

  if(!m_loadTimer->isActive()) {
    emit initProgress();
    m_loadTimer->start(0, false);
  }
}

void KivioDoc::addSpawnerSetDuringLoad(const QString& dirName, bool hidden)
{
  KivioStencilSpawnerSet *set;

  set = new KivioStencilSpawnerSet();
  set->setHidden(hidden);

  if( set->loadDir(dirName)==false )
  {
    kDebug(43000) << "KivioDoc::addSpawnerSetDuringLoad() - Error loading dir set" << endl;
    delete set;
    return;
  }

  QStringList::iterator it;
  QStringList files = set->files();

  for(it = files.begin(); it != files.end(); ++it) {
    QString fileName = set->dir() + "/" + (*it);
    set->loadFile(fileName);
  }

  m_pLstSpawnerSets->append(set);
}

KivioDoc::~KivioDoc()
{
  if(m_docOpened) {
    saveConfig();
  }

  // ***MUST*** Delete the pages first because they may
  // contain plugins which will be unloaded soon.  The stencils which are
  // spawned by plugins NEED the plugins still loaded when their destructor
  // is called or the program will slit it's throat.
  delete m_pMap;
  delete dcop;
  delete m_commandHistory;

  delete m_pLstSpawnerSets;
  m_pLstSpawnerSets = 0;

  s_docs->removeRef(this);
}

void KivioDoc::saveConfig()
{
  Kivio::Config::self()->writeConfig();
}

void KivioDoc::initConfig()
{
  setUnit( KoUnit::unit(Kivio::Config::unit()) );
  m_font = Kivio::Config::font();
  m_pageLayout = Kivio::Config::defaultPageLayout();
}

bool KivioDoc::removeSpawnerSet( KivioStencilSpawnerSet *pSet )
{
    return m_pLstSpawnerSets->removeRef( pSet );
}

/**
 * Iterates through all spawner objects in the stencil set checking if
 * they exist in any of the pages.
 */
void KivioDoc::slotDeleteStencilSet( DragBarButton *pBtn, QWidget *w, KivioStackBar *pBar )
{
  KivioIconView* pIconView = static_cast<KivioIconView*>(w);
  KivioStencilSpawnerSet* pSet = pIconView->spawnerSet();

  // Only actually remove the set if it isn't used...
  if(!checkStencilsForSpawnerSet(pSet)) {
    removeSpawnerSet(pSet);
  } else {
    pSet->setHidden(true);
  }

  // And emit the signal to kill the set (page & button)
  emit sig_deleteStencilSet(pBtn, w, pBar);
}

/**
 * Checks if any stencils in the document use this spawner
 */
bool KivioDoc::checkStencilsForSpawnerSet(KivioStencilSpawnerSet* spawnerSet)
{
  KivioPage *pPage = m_pMap->firstPage();
  KivioLayer *pLayer;
  KivioStencil *pStencil;

  // Iterate across all the pages
  while(pPage) {
    pLayer = pPage->layers()->first();

    while(pLayer) {
      pStencil = pLayer->stencilList()->first();

      while(pStencil) {
        // If this is a group stencil, then we must check all child stencils
        if(pStencil->groupList() && pStencil->groupList()->count() > 0) {
          if(checkGroupForSpawnerSet(pStencil, spawnerSet)) {
            return true;
          }
        } else if(pStencil->spawner()->set() == spawnerSet) {
          return true;
        }

        pStencil = pLayer->stencilList()->next();
      }

      pLayer = pPage->layers()->next();
    }

    pPage = m_pMap->nextPage();
  }

  return false;
}

bool KivioDoc::checkGroupForSpawnerSet(KivioStencil* pGroup, KivioStencilSpawnerSet* spawnerSet)
{
  KivioStencil *pStencil = pGroup->groupList()->first();

  while(pStencil) {
    if(pStencil->groupList() && pStencil->groupList()->count() > 0) {
      if(checkGroupForSpawnerSet(pStencil, spawnerSet))
        return true;
    } else if(pStencil->spawner()->set() == spawnerSet) {
      return true;
    }

    pStencil = pGroup->groupList()->next();
  }

  return false;
}

void KivioDoc::slotSelectionChanged()
{
    emit sig_selectionChanged();
}

KivioStencilSpawner* KivioDoc::findStencilSpawner( const QString& setId, const QString& stencilId )
{
    KivioStencilSpawnerSet *pSpawnerSet = m_pLstSpawnerSets->first();
    while( pSpawnerSet )
    {
        if( pSpawnerSet->id() == setId && pSpawnerSet->find(stencilId) )
        {
            return pSpawnerSet->find(stencilId);
	    // return pSpawnerSet->find(name)
        }
        pSpawnerSet = m_pLstSpawnerSets->next();
    }

    if( m_pInternalSet->id() == setId && m_pInternalSet->find(stencilId) )
    {
        return m_pInternalSet->find(stencilId);
    }

    return NULL;
}

KivioStencilSpawner* KivioDoc::findInternalStencilSpawner( const QString& stencilId )
{
    return m_pInternalSet->find(stencilId);
}

void KivioDoc::addInternalStencilSpawner(KivioStencilSpawner* spawner)
{
  m_pInternalSet->addSpawner(spawner);
}

void KivioDoc::updateView(KivioPage* page)
{
  emit sig_updateView(page);
}

void KivioDoc::updateButton()
{
    foreach ( KoView* view, views() )
        static_cast<KivioView*>(view)->updateButton();
}

void KivioDoc::resetLayerPanel()
{
    foreach ( KoView* view, views() )
        static_cast<KivioView*>(view)->resetLayerPanel();
}

void KivioDoc::addCommand( KCommand * cmd )
{
    kDebug(43000) << "KivioDoc::addCommand " << cmd->name() << endl;
    m_commandHistory->addCommand( cmd, false );
    setModified( true );
}

int KivioDoc::undoRedoLimit() const
{
    return m_commandHistory->undoLimit();
}

void KivioDoc::setUndoRedoLimit(int val)
{
    m_commandHistory->setUndoLimit(val);
    m_commandHistory->setRedoLimit(val);
}

void KivioDoc::slotDocumentRestored()
{
    setModified( false );
}

void KivioDoc::slotCommandExecuted()
{
    setModified( true );
}

void KivioDoc::updateProtectPanelCheckBox()
{
    foreach ( KoView* view, views() )
        static_cast<KivioView*>(view)->updateProtectPanelCheckBox();
}

void KivioDoc::loadStencil()
{
  KivioStencilSpawnerSet* set = m_stencilSetLoadQueue.first();
  QString fileName = set->dir() + "/" + set->files()[m_currentFile];
  set->loadFile(fileName);
  m_currentFile++;
  emit progress(qRound(((float)m_currentFile / (float)set->files().count()) * 100.0));

  if(m_currentFile >= set->files().count()) {
    m_pLstSpawnerSets->append(set);

    if(!m_bLoading) {
      setModified(true);
      emit sig_addSpawnerSet(set);
    }

    m_currentFile = 0;
    set = 0;
    m_stencilSetLoadQueue.pop_front();

    if(m_stencilSetLoadQueue.isEmpty()) {
      m_loadTimer->stop();
      emit endProgress();
    } else {
      emit initProgress();
    }
  }
}

void KivioDoc::updateGuideLines(KoView* sender)
{
  QValueList<double> hGuideLines;
  QValueList<double> vGuideLines;
  KivioView* view = static_cast<KivioView*>(sender);
  view->canvasWidget()->guideLines().getGuideLines(hGuideLines, vGuideLines);
  view->activePage()->setGuideLines(hGuideLines, vGuideLines);

  KivioView* itView = 0;

  foreach ( KoView* v, views() ) {
    itView = static_cast<KivioView*>(v);
    if(v != sender && (itView->activePage() == view->activePage())) {
      itView->canvasWidget()->guideLines().setGuideLines(hGuideLines, vGuideLines);
    }
  }
}

void KivioDoc::updateGuideLines(KivioPage* page)
{
  QValueList<double> hGuideLines = page->horizontalGuideLines();
  QValueList<double> vGuideLines = page->verticalGuideLines();
  KivioView* itView = 0;

  foreach ( KoView* view, views() ) {
    itView = static_cast<KivioView*>(view);
    if(itView->activePage() == page) {
      itView->canvasWidget()->guideLines().setGuideLines(hGuideLines, vGuideLines);
    }
  }
}

#include "kivio_doc.moc"
