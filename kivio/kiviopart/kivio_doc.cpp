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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <qdom.h>
#include <qtextstream.h>
#include <qbuffer.h>
#include <qtabwidget.h>

#include "tkunits.h"
#include "kivio_doc.h"
#include "kivio_page.h"
#include "kivio_map.h"
#include "kivio_view.h"
#include "kivio_factory.h"
#include "kivioabout.h"

#include "export_page_dialog.h"
#include "kivio_config.h"
#include "kivio_common.h"
#include "kivio_group_stencil.h"
#include "kivio_icon_view.h"
#include "kivio_layer.h"
#include "kivio_painter.h"
#include "kivio_screen_painter.h"
#include "kivio_stencil.h"
#include "kivio_stencil_spawner_set.h"
#include "kivio_viewmanager_panel.h"

#include "stencilbarbutton.h"

#include <unistd.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kurl.h>
#include <kapp.h>
#include <cassert>
#include <qdatetime.h>
#include <klocale.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qpixmap.h>

#include <qpainter.h>
#include <qpen.h>


#include <kstddirs.h>
#include <kpopupmenu.h>
#include <kmenubar.h>
#include <kiconloader.h>
#include <khelpmenu.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <koTemplateChooseDia.h>
#include <koFilterManager.h>
#include <koStoreDevice.h>

//using namespace std;

/*****************************************************************************
 *
 * KivioDoc
 *
 *****************************************************************************/

QList<KivioDoc>* KivioDoc::s_docs = 0;
int KivioDoc::s_docId = 0;

KivioDoc::KivioDoc( QWidget *parentWidget, const char* widgetName, QObject* parent, const char* name, bool singleViewMode )
: KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
  if (!s_docs)
    s_docs = new QList<KivioDoc>;

  s_docs->append(this);

  m_options = new KivioOptions();

  m_pLstSpawnerSets = new QList<KivioStencilSpawnerSet>;
  m_pLstSpawnerSets->setAutoDelete(true);

  setInstance( KivioFactory::global(), false );

  if ( !name )
  {
    QString tmp( "Document%1" );	//lukas: FIXME
    tmp = tmp.arg( s_docId++ );
    setName( tmp.latin1() );
  }

  m_pClipboard = NULL;

  m_iPageId = 1;
  m_pMap = 0L;
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

  m_units = (int)UnitPoint;

  viewItemList = new ViewItemList(this);
}

QList<KivioDoc>& KivioDoc::documents()
{
  if ( s_docs == 0 )
    s_docs = new QList<KivioDoc>;
  return *s_docs;
}

bool KivioDoc::initDoc()
{
  QString f;
  KoTemplateChooseDia::ReturnType ret;

  ret = KoTemplateChooseDia::choose(  KivioFactory::global(), f,
                                      "application/x-kivio", "*.flw", i18n("Kivio"),
                                      KoTemplateChooseDia::NoTemplates );

  if ( ret == KoTemplateChooseDia::File ) {
    KURL url;
    url.setPath(f);
    return openURL( url );
  }
  else if ( ret == KoTemplateChooseDia::Empty ) {
      KivioPage *t = createPage();
      m_pMap->addPage( t );
      resetURL();
      initConfig();
      return true;
  }
  else
    return false;
}

KoView* KivioDoc::createViewInstance( QWidget* parent, const char* name )
{
  if (!name)
    name = "View";

  return new KivioView( parent, name, this );
}

QDomDocument KivioDoc::saveXML()
{
  QDomDocument doc( "kiviodoc" );
  doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
  QDomElement kivio = doc.createElement( "kiviosheet" );
  kivio.setAttribute( "editor", "Kivio" );
  kivio.setAttribute( "mime", "application/x-kivio" );

  kivio.setAttribute( "units", m_units );
  gridData.save(kivio,"grid");

  QDomElement viewItemsElement = doc.createElement("ViewItems");
  kivio.appendChild(viewItemsElement);
  viewItemList->save(viewItemsElement);

  QDomElement optionsElement = doc.createElement("Options");
  kivio.appendChild(optionsElement);
  m_options->save(optionsElement);

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
    kivio.appendChild( pSet->saveXML( doc ) );

    pSet = m_pLstSpawnerSets->next();
  }


  QDomElement e = m_pMap->save(doc);
  kivio.appendChild(e);

  // Write it out to a tmp file
     QFile f("filedump.xml");
    if ( f.open(IO_WriteOnly) ) {    // file opened successfully
        QTextStream t( &f );        // use a text stream
        t << doc.toString();
        f.close();
    }

  setModified(false);
  return doc;
}

bool KivioDoc::loadXML( QIODevice *, const QDomDocument& doc )
{
  m_bLoading = true;

  if ( doc.doctype().name() != "kiviodoc" ) {
    m_bLoading = false;
    return false;
  }

  QDomElement kivio = doc.documentElement();
  if ( kivio.attribute( "mime" ) != "application/x-kivio" ) {
     kdDebug() << "KivioDoc::loadXML() - Invalid mime type" << endl;
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

        if( id == "" )
        {
	   kdDebug() << "KivioDoc::loadXML() - Bad KivioStencilSpawnerSet found, it contains no id!" << endl;
        }
        else
        {
            loadStencilSpawnerSet( id );
        }
    }
    else if( name == "ViewItems" )
    {
        viewItemList->load(node.toElement());
    }
    else if( name == "Options" )
    {
        m_options->load(node.toElement());
    }
    else
    {
       kdDebug() << "KivioDoc::loadXML() - Unknown node " <<  name << endl;
    }

    node = node.nextSibling();
  }

  // <map>
  //QDomElement mymap = kivio.namedItem( "map" ).toElement();
  //if ( !mymap.isNull() )
  //  if ( !m_pMap->loadXML(mymap) ) {
  //    m_bLoading = false;
  //    return false;
  //  }

  setUnits(kivio.attribute("units","0").toInt());

  gridData.load(kivio,"grid");
  return true;
}

bool KivioDoc::loadStencilSpawnerSet( const QString &id )
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
                QDir innerD(fi->absFilePath() );
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
                        foundId = KivioStencilSpawnerSet::readId(innerFI->absFilePath());
                        if( foundId == id)
                        {

                            // Load the spawner set with  rootDir + "/" + fi.fileName()
                            KivioStencilSpawnerSet *pSet = addSpawnerSetDuringLoad( innerFI->absFilePath() );
                            if( pSet )
                            {
			       ;
                            }
                            else
                            {
			       kdDebug() << "KivioDoc::loadStencilSpawnerSet() - Failed to load stencil:  "
					 << innerFI->absFilePath() << endl;
                            }
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

  KivioPage* t = new KivioPage(m_pMap,s.ascii());	//lukas: FIXME
  t->setPageName(s,true);

  return t;
}

void KivioDoc::addPage( KivioPage* page )
{
  m_pMap->addPage(page);
  setModified(true);
  emit sig_addPage(page);
}

void KivioDoc::paintContent( QPainter&, const QRect&, bool /*transparent*/, double /*zoomX*/, double /*zoomY*/ )
{
    // ## TODO - otherwise kivio isn't embeddable
//  KivioPage* page = m_pMap->activePage();
//  if ( !page )
//    return;

//  paintContent(painter,rect,transparent,page);
}

void KivioDoc::paintContent( KivioPainter& painter, const QRect& rect, bool transparent, KivioPage* page, QPoint p0, float zoom, bool drawHandles )
{
  if ( isLoading() )
    return;

  page->paintContent(painter,rect,transparent,p0,zoom, drawHandles);
}

void KivioDoc::printContent( KPrinter &prn )
{
    KivioScreenPainter p;
    int from = prn.fromPage();
    int to = prn.toPage();
    int i;

    KivioPage *pPage;

    kdDebug() << "KivioDoc::printContent() - Printing from " << from << " to " << to << endl;

    p.start(&prn);
    for( i=from; i<=to; i++ )
    {
        pPage = m_pMap->pageList().at(i-1);
        pPage->printContent(p);

        if( i<to )
            prn.newPage();
    }
    p.stop();
}

/* TODO:
 *
 * This entire function should probably be encapsulated in some sort of
 * object or other class.
 */
bool KivioDoc::exportPage(KivioPage *pPage,const QString &fileName, ExportPageDialog *dlg)
{
   QPixmap buffer( pPage->paperLayout().ptWidth() + dlg->border()*2,
		   pPage->paperLayout().ptHeight() + dlg->border()*2 );

   kdDebug() << "KivioDoc::exportCurPage() to " << fileName << "\n";

   KivioScreenPainter p;

   buffer.fill(Qt::white);

   p.start( &buffer );
   p.setTranslation( dlg->border(), dlg->border() );

   if( dlg->fullPage()==true )
   {
      pPage->printContent(p);
   }
   else
   {
      pPage->printSelected(p);
   }

   p.stop();


   QFileInfo finfo(fileName);

   return buffer.save( fileName, finfo.extension(false).upper().latin1(), dlg->quality());
}


// TODO: Fix for id system
bool KivioDoc::setIsAlreadyLoaded( QString dirName, QString id )
{
    KivioStencilSpawnerSet *pSet = m_pLstSpawnerSets->first();
    while(pSet)
    {
        if( pSet->dir() == dirName || pSet->id() == id )
        {
            return true;
        }

        pSet = m_pLstSpawnerSets->next();
    }

    return false;
}

KivioStencilSpawnerSet *KivioDoc::addSpawnerSet( QString dirName )
{
    KivioStencilSpawnerSet *set;

    QString id = KivioStencilSpawnerSet::readId( dirName );

    if( setIsAlreadyLoaded( dirName, id ) )
    {
       kdDebug() << "KivioDoc::addSpawnerSet() - Cannot load duplicate stencil sets" << endl;
        return NULL;
    }


    set = new KivioStencilSpawnerSet();

    if( set->loadDir(dirName)==false )
    {
       kdDebug() << "KivioDoc::addSpawnerSet() - Error loading dir set" << endl;
        delete set;
        return NULL;
    }


    m_pLstSpawnerSets->append( set );
    setModified(true);

    emit sig_addSpawnerSet( set );

    return set;
}

KivioStencilSpawnerSet *KivioDoc::addSpawnerSetDuringLoad( QString dirName )
{
    KivioStencilSpawnerSet *set;

    set = new KivioStencilSpawnerSet();
    if( set->loadDir(dirName)==false )
    {
       kdDebug() << "KivioDoc::addSpawnerSetDuringLoad() - Error loading dir set" << endl;
        delete set;
        return NULL;
    }

    m_pLstSpawnerSets->append( set );

    return set;
}

KivioDoc::~KivioDoc()
{
    saveConfig();
    // ***MUST*** Delete the pages first because they may
    // contain plugins which will be unloaded soon.  The stencils which are
    // spawned by plugins NEED the plugins still loaded when their destructor
    // is called or the program will slit it's throat.
    delete m_pMap;

    if( m_pClipboard )
    {
        delete m_pClipboard;
        m_pClipboard = NULL;
    }

    if( m_pLstSpawnerSets )
    {
        delete m_pLstSpawnerSets;
        m_pLstSpawnerSets = NULL;
    }

    s_docs->removeRef(this);

    delete m_options;
}

void KivioDoc::saveConfig()
{
    // Only save the config that is manipulated by the UI directly.
    // The config from the config dialog is saved by the dialog itself.
    KConfig *config = KivioFactory::global()->config();
    config->setGroup( "Interface" );
    config->writeEntry("ShowGrid", grid().isShow );
    config->writeEntry("SnapGrid",grid().isSnap);
}

void KivioDoc::initConfig()
{
    KConfig *config = KivioFactory::global()->config();
    if( config->hasGroup("Interface" ) )
    {
        config->setGroup( "Interface" );
        KivioGridData d = grid();
        d.isShow = config->readBoolEntry( "ShowGrid", true );
        d.isSnap = config->readBoolEntry( "SnapGrid", true);
        setGrid(d);
    }
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
    // Iterate through all spawners in the set checking if they exist in any of
    // the pages
    KivioIconView *pIconView = (KivioIconView *)w;
    KivioStencilSpawnerSet *pSet = pIconView->spawnerSet();

    // Start the iteration
    KivioStencilSpawner *pSpawner = pSet->spawners()->first();
    while( pSpawner )
    {
        // Check for a spawner.  If there is one, the set cannot be deleted
        if( checkStencilsForSpawner( pSpawner )==true )
        {
            KMessageBox::error(NULL, i18n("Cannot delete stencil set because there are still stencils in use."),
                i18n("Cannot Delete Stencil Set"));
            return;
        }

        // Now check the clipboard against this spawner
        if( m_pClipboard )
        {
            if( checkGroupForSpawner( m_pClipboard, pSpawner )==true )
            {
                if( KMessageBox::questionYesNo(NULL, i18n("The clipboard contains stencils which belong to the set you are trying to remove.\nWould you like to delete what is on the clipboard?\n(Saying no will cause this stencil set to not be removed.)"),
                    i18n("Clear the clipboard?"))==KMessageBox::Yes )
                {
                    delete m_pClipboard;
                    m_pClipboard = NULL;
                }
                else    // abort because the user aborted
                    return;
            }
        }


        pSpawner = pSet->spawners()->next();
    }



    // If we made it this far, it's ok to delete this stencil set, so do it
//    if( KMessageBox::questionYesNo(NULL, i18n("Are you sure you want to delete this stencil set?"),
//        i18n("Delete Stencil Set?"))==KMessageBox::Yes )
    {
        // Destroying the IconView does not destroy the spawner set, so we remove
        // it here
        removeSpawnerSet( pIconView->spawnerSet() );

        // And emit the signal to kill the set (page & button)
        emit sig_deleteStencilSet( pBtn, w, pBar );
    }
}

/**
 * Checks if any stencils in the document use this spawner
 */
bool KivioDoc::checkStencilsForSpawner( KivioStencilSpawner *pSpawner )
{
    KivioPage *pPage;
    KivioLayer *pLayer;
    KivioStencil *pStencil;

    // Iterate across all the pages
    pPage = m_pMap->firstPage();
    while( pPage )
    {
        pLayer = pPage->layers()->first();
        while( pLayer )
        {
            pStencil = pLayer->stencilList()->first();
            while( pStencil )
            {
                // If this is a group stencil, then we must check all child stencils
                if( pStencil->groupList() && pStencil->groupList()->count() > 0 )
                {
                    if( checkGroupForSpawner( pStencil, pSpawner )==true )
                        return true;
                }
                else if( pStencil->spawner() == pSpawner )
                    return true;

                pStencil = pLayer->stencilList()->next();
            }

            pLayer = pPage->layers()->next();
        }

        pPage = m_pMap->nextPage();
    }

    return false;
}

bool KivioDoc::checkGroupForSpawner( KivioStencil *pGroup, KivioStencilSpawner *pSpawner )
{
    KivioStencil *pStencil;

    pStencil = pGroup->groupList()->first();
    while( pStencil )
    {
        if( pStencil->groupList() && pStencil->groupList()->count() > 0 )
        {
            if( checkGroupForSpawner( pStencil, pSpawner )==true )
                return true;
        }
        else if( pStencil->spawner() == pSpawner )
        {
            return true;
        }

        pStencil = pGroup->groupList()->next();
    }

    return false;
}

void KivioDoc::setClipboard( KivioGroupStencil *p )
{
    if( m_pClipboard )
        delete m_pClipboard;

    m_pClipboard = p;
}

KivioGroupStencil *KivioDoc::clipboard()
{
    return m_pClipboard;
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

void KivioDoc::setUnits(int unit)
{
  if (m_units == unit)
    return;

  m_units = unit;
  emit unitsChanged(unit);
}

void KivioDoc::updateView(KivioPage* page, bool modified)
{
  emit sig_updateView(page);

  if (modified)
    setModified(true);
}

void KivioDoc::addShell(KoMainWindow *shell)
{
     kdDebug() << "addShell-----------------------------" << endl;
  KoDocument::addShell(shell);

  KPopupMenu* help = shell->customHelpMenu();
  help->disconnectItem(KHelpMenu::menuAboutApp, 0, 0);
  help->connectItem(KHelpMenu::menuAboutApp, this, SLOT(aboutKivio()));

  help->insertSeparator();
  help->insertItem(BarIcon("kivio"), i18n("Get Stencil Sets"), this, SLOT(aboutGetStencilSets()));

  shell->menuBar()->removeItemAt(shell->menuBar()->count() - 1);
  shell->menuBar()->insertItem(i18n("&Help"), help);
}

void KivioDoc::aboutKivio()
{
  KivioAbout *d = new KivioAbout(0, 0, true);
  d->exec();
  delete d;
}

void KivioDoc::aboutGetStencilSets()
{
  KivioAbout *d = new KivioAbout(0, 0, true);
  d->tabWidget->setCurrentPage(3);
  d->exec();
  delete d;
}

#include "kivio_doc.moc"
