/*
	Copyright (C) 1998 Simon Hausmann
                       <tronical@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
//
// $Id$
//

#include "kohtml_doc.h"
#include "kohtml_doc.moc"
#include "kohtml_shell.h"

#include <unistd.h>

#include <koIMR.h>
#include <qmsgbox.h>
#include <qpaintdevice.h>

#include <khtmlsavedpage.h>

#include "htmwidget.h"
#include "kfileio.h"
#include <klocale.h>

KoHTMLChild::KoHTMLChild(KoHTMLDoc *doc, const QRect &rect, KOffice::Document_ptr koDoc)
:KoDocumentChild(rect, koDoc)
{
  m_parent = doc;
  m_rDoc = KOffice::Document::_duplicate(koDoc);
  m_geometry = rect;
  m__geometry = rect;
}

KoHTMLChild::KoHTMLChild(KoHTMLDoc *doc)
:KoDocumentChild()
{
  m_parent = doc;
}

KoHTMLChild::~KoHTMLChild()
{
  m_rDoc = 0L;
}

KoHTMLDoc::KoHTMLDoc()
{
  ADD_INTERFACE("IDL:KOffice/Print:1.0");

  SIGNAL_IMPL( "documentStarted" );
  SIGNAL_IMPL( "documentDone" );

  m_lstViews.setAutoDelete(false);
  m_lstChildren.setAutoDelete(true);
  m_lstHTMLViews.setAutoDelete(false);
  m_lstJobs.setAutoDelete(false);

  m_bModified = false;
  m_bLoadError = false;

  m_eSaveLoadMode = KoHTML::KoHTMLDocument::URLOnly;

  m_bRepaintDocument = true;
  m_pDocumentPixmap = 0L;
  m_lastWidth = -1;
  m_lastHeight = -1;
  m_lastScale = -1;
  m_lastXOffset = -1;
  m_lastYOffset = -1;

  m_strCurrentURL = "";

  QObject::connect( &m_timer, SIGNAL(timeout()),
                    this, SLOT(slotTimeout()));

  m_pInternalView = 0L;
//  m_pInternalView = new KMyHTMLView( this );

//  addHTMLView(m_pInternalView);
/*
  QObject::connect(this, SIGNAL(contentChanged()),
                         SLOT(slotUpdateInternalView()));
  QObject::connect(m_pInternalView, SIGNAL(documentDone(KHTMLView *)),
                   this, SLOT(slotDocumentDoneInternal(KHTMLView *)));
*/
}

KoHTMLDoc::~KoHTMLDoc()
{
  if ( m_pInternalView )
     {
       removeHTMLView(m_pInternalView);
       delete m_pInternalView;
     }

  if (m_pDocumentPixmap)
    delete m_pDocumentPixmap;

//  cleanUp();
}

void KoHTMLDoc::cleanUp()
{
  if (m_bIsClean) return;

  m_lstChildren.clear();

  KoDocument::cleanUp();
}

KOffice::MainWindow_ptr KoHTMLDoc::createMainWindow()
{
  KoHTMLShell* shell = new KoHTMLShell;
  shell->show();
  shell->setDocument( this );

  return KOffice::MainWindow::_duplicate( shell->koInterface() );
}

void KoHTMLDoc::removeView(KoHTMLView *view)
{
  m_lstViews.removeRef(view);
}

KoHTMLView *KoHTMLDoc::createKoHTMLView( QWidget* _parent )
{
  KoHTMLView *p = new KoHTMLView(_parent, 0L, this);
  p->QWidget::show();

  m_lstViews.append(p);

  return p;
}

OpenParts::View_ptr KoHTMLDoc::createView()
{
  return OpenParts::View::_duplicate( createKoHTMLView() );
}

void KoHTMLDoc::viewList(OpenParts::Document::ViewList *&list)
{
  (*list).length(m_lstViews.count());

  int i = 0;
  QListIterator<KoHTMLView> it(m_lstViews);
  for (; it.current(); ++it)
  {
    (*list)[i++] = OpenParts::View::_duplicate(it.current());
  }

}

int KoHTMLDoc::viewCount()
{
  return m_lstViews.count();
}

CORBA::Boolean KoHTMLDoc::initDoc()
{
  return true;
}

char *KoHTMLDoc::htmlURL()
{
  return CORBA::string_dup( m_strCurrentURL.data() );
}

void KoHTMLDoc::openURL( const char *_url, CORBA::Boolean reload )
{
  QString url = _url;
  url.stripWhiteSpace();

  KURL u( url );

  if ( u.isMalformed() )
    return;

  QString anchor = u.htmlRef();

  QString old = m_strCurrentURL;
  
  m_strCurrentURL = u.url();

  stopLoading();

  m_bLoadError = false;
  m_bDocumentDone = false;

  if ( (!((bool)reload)) && urlcmp( u.url(), old, true, true ) )
    {
      QListIterator<KMyHTMLView> it( m_lstHTMLViews );
      for (; it.current(); ++it)
        it.current()->gotoAnchor( anchor );
    }
  else
    {
      //update all views
      QListIterator<KMyHTMLView> it( m_lstHTMLViews );
      for (; it.current(); ++it)
        it.current()->openURL( u.url(), (bool)reload );

      documentStarted();
    }
}

void KoHTMLDoc::documentStarted()
{
  SIGNAL_CALL0( "documentStarted" );
  m_bDocumentDone = false;
}

void KoHTMLDoc::documentDone()
{
  SIGNAL_CALL0( "documentDone" );
  m_bDocumentDone = true;
}

CORBA::Boolean KoHTMLDoc::documentLoading()
{
  return (CORBA::Boolean)m_bDocumentDone;
}

void KoHTMLDoc::stopLoading()
{
  cerr << "killing old stuff" << endl;

  //"stop" all views
  QListIterator<KMyHTMLView> it2( m_lstHTMLViews );
  for (; it2.current(); ++it2)
    it2.current()->stop();

  cerr << "1" << endl;

  QListIterator<KoHTMLJob> it(m_lstJobs);
  for (; it.current(); ++it)
      {
        KoHTMLJob *job = it.current();
        cerr << "killing job (" << job->url() << ")" << endl;
        m_lstJobs.removeRef(job);
        job->kill();
      }

  cerr << "2" << endl;

  assert( m_lstJobs.count() == 0 );
  m_bRepaintDocument = true;
}

KoHTML::KoHTMLDocument::SaveLoadMode KoHTMLDoc::saveLoadMode()
{
  return m_eSaveLoadMode;
}

void KoHTMLDoc::setSaveLoadMode( KoHTML::KoHTMLDocument::SaveLoadMode mode )
{
  m_eSaveLoadMode = mode;
}

void KoHTMLDoc::slotHTMLLoadError(KoHTMLJob *job, const char *errMsg)
{
  job->view()->stop();

  m_lstJobs.removeRef( job );

  m_strErrorMsg = errMsg;

  m_timer.start( 0, true );
}

void KoHTMLDoc::slotHTMLRedirect(int id, const char *url)
{
  KURL u( url );
  m_strCurrentURL = u.url();
}

void KoHTMLDoc::slotTimeout()
{
  QMessageBox::critical(0L, i18n("KoHTML Error"), m_strErrorMsg, i18n("OK"));

  QListIterator<KoHTMLView> it( m_lstViews );
  for (; it.current(); ++it)
    it.current()->slotBack();
}

void KoHTMLDoc::draw(QPaintDevice *dev, CORBA::Long width, CORBA::Long height,
                     CORBA::Float _scale)
{
  if ( m_strCurrentURL.isEmpty() )
     {
       QPainter painter;
       painter.begin( dev );

       if (_scale != 1.0) painter.translate(_scale, _scale);

       const char *msg = i18n("KoHTML: No document loaded!");

       QRect r = painter.fontMetrics().boundingRect(msg);

       painter.drawText((width / 2) - (r.width() / 2), (height / 2) - (r.height() / 2) , msg);

       drawChildren(&painter, _scale);

       painter.end();

       return;
     }


  if ( m_lstHTMLViews.count() == 0 )
    return;

  KMyHTMLView *view = m_lstHTMLViews.first();

  if ((!m_pDocumentPixmap) || (m_bRepaintDocument) || (width != m_lastWidth) ||
      (height != m_lastHeight) || (_scale != m_lastScale) ||
      (view->xOffset() != m_lastXOffset) || (view->yOffset() != m_lastYOffset )
     )
     {
       if (m_pDocumentPixmap)
         m_pDocumentPixmap->resize( width, height );
       else
         m_pDocumentPixmap = new QPixmap( width, height );
	
       drawDocument( m_pDocumentPixmap, width, height, _scale );

       m_lastWidth = width;
       m_lastHeight = height;
       m_lastScale = _scale;	
       m_lastXOffset = view->xOffset();
       m_lastYOffset = view->yOffset();
       m_bRepaintDocument = false;
     }

  QPainter p;
  p.begin( dev );
  p.drawPixmap(0, 0, *m_pDocumentPixmap );
  p.end();
}

void KoHTMLDoc::drawDocument(QPaintDevice *dev, CORBA::Long width, CORBA::Long height,
		             CORBA::Float _scale )
{
  KMyHTMLView *view = m_pInternalView;

  QPainter painter;
  painter.begin(dev);

  if (_scale != 1.0) painter.translate(_scale, _scale);

  cerr << "view is #1 " << view << endl;

  if ( !view && m_lstHTMLViews.count() > 0 )
    view = m_lstHTMLViews.first();

  cerr << "view is #2 " << view << endl;

  if ( !view )
    {
      painter.end();
      return;
    }

  view->draw( &painter, width, height );

  drawChildren(&painter, _scale);

  painter.end();
}

void KoHTMLDoc::drawChildren( QPainter *painter, CORBA::Float _scale)
{			
  QListIterator<KoHTMLChild> it(m_lstChildren);

  for (; it.current(); ++it)
      {
        QRect geom = it.current()->geometry();
	QPixmap pix(geom.width(), geom.height());
	pix.fill();
	QPainter p2;
	p2.begin(&pix);
        it.current()->draw(_scale, true)->play(&p2);
	p2.end();
	painter->drawPixmap(geom.left(), geom.top(), pix);
      }
}

bool KoHTMLDoc::hasToWriteMultipart()
{
  QListIterator<KoHTMLChild> it(m_lstChildren);

  for (; it.current(); ++it)
      {
        if (!it.current()->isStoredExtern())
	   return true;
      }
  return false;
}

bool KoHTMLDoc::loadChildren(KOStore::Store_ptr store)
{

  /*
   ARGHL, if _anyone_ is able to tell me why the usage of QListIterator
   here celebrates a segfault I will send him a nice malt whisky.... ;-)
  */

  KoHTMLChild *c;
  for (c = m_lstChildren.first(); c; c = m_lstChildren.next())
      {
	if (!c->loadDocument(store, c->mimeType()))
	   return false;
      }

  return true;
}

bool KoHTMLDoc::loadXML(KOMLParser &parser, KOStore::Store_ptr store)
{

  string tag;
  vector<KOMLAttrib> lst;
  string name;

  if (!parser.open("DOC", tag))
     return false;

  KOMLParser::parseTag(tag.c_str(), name, lst);

  vector<KOMLAttrib>::const_iterator it = lst.begin();
  for (; it != lst.end(); it++)
      {
        if ((*it).m_strName == "mime")
	   {
	     if ((*it).m_strValue != MIME_TYPE)
     	        return false;
	   }
      }


  while (parser.open(0L, tag))
        {
	  KOMLParser::parseTag(tag.c_str(), name, lst);
	
	  if (name == "URL")
	     {
	
	       string tmp;
	
	       parser.readText(tmp);
		
	       openURL( tmp.c_str(), true );
	     }
	  else if (name == "OBJECT")
	     {
	       KoHTMLChild *ch = new KoHTMLChild(this);
	       ch->load(parser, lst);
	       insertChild(ch);
	     }
	
	  if (!parser.close(tag))
	     return false;
	}

  parser.close(tag);

  return true;
}

void KoHTMLDoc::makeChildListIntern(KOffice::Document_ptr root, const char *path)
{
  int i = 0;

  QListIterator<KoHTMLChild> it(m_lstChildren);
  for (; it.current(); ++it)
      {
        QString tmp;
	tmp.sprintf("/%i", i++);
	QString p(path);
	p += tmp;
	
	KOffice::Document_var doc = it.current()->document();
	doc->makeChildList(root, p);
      }
}

bool KoHTMLDoc::save(ostream &out, const char *format)
{
  out << "<?xml version=\"1.0\"?>" << endl;
  out << otag << "<DOC author=\"" << "Simon Hausmann"
              << "\" email=\"" << "<tronical@gmx.net>"
	      << "\" editor=\"" << "KoHTML"
	      << "\" mime=\"" << MIME_TYPE << "\" >" << endl;
	
  out << otag << "<URL>" << m_strCurrentURL.data() << "</URL>" << endl;

  /*
   * m_pInternalView->save( out ) ??????
   */

  QListIterator<KoHTMLChild> ch(m_lstChildren);
  for (; ch.current(); ++ch)
      ch.current()->save(out);

  out << etag << "</DOC>" << endl;

  setModified(false);

  return true;	
}

const char *KoHTMLDoc::copyright()
{
  return "KoHTML (C) Simon Hausmann, <tronical@gmx.net> 1998";
}

const char *KoHTMLDoc::comment()
{
  return "insert your comment here";
}

void KoHTMLDoc::insertObject(const QRect &rect, KoDocumentEntry &de)
{
  KOffice::Document_var doc = imr_createDoc(de);

  if (CORBA::is_nil(doc))
     return;

  if (!doc->initDoc())
     {
       QMessageBox::critical(0L, i18n("KoHTML Error"), i18n("Could not initialize child document"), i18n("OK"));
       return;
     }

  KoHTMLChild *c = new KoHTMLChild(this, rect, doc);

  insertChild(c);

  m_bModified = true;
  m_bRepaintDocument = true;
}

void KoHTMLDoc::insertChild(KoHTMLChild *child)
{
  m_lstChildren.append(child);

  emit sig_insertObject(child);

  m_bModified = true;
  m_bRepaintDocument = true;
}

void KoHTMLDoc::changeChildGeometry(KoHTMLChild *child, const QRect &rect)
{
  child->setGeometry(rect);
  child->setkGeometry(rect);

  emit sig_updateChildGeometry(child);

  m_bModified = true;
  m_bRepaintDocument = true;
}

QListIterator<KoHTMLChild> KoHTMLDoc::childIterator()
{
  return QListIterator<KoHTMLChild>(m_lstChildren);
}

void KoHTMLDoc::addHTMLView(KMyHTMLView *view)
{
  m_lstHTMLViews.append(view);
/*
  if ( m_pInternalView && view != m_pInternalView )
    {
      m_pInternalView->stop();

      QListIterator<KoHTMLJob> it( m_lstJobs );
      for (; it.current(); ++it)
        {
	  KoHTMLJob *job = it.current();
	  m_lstJobs.removeRef( job );
	  job->kill();
	}
*/
/*
      QListIterator<KoHTMLJob> it( m_lstJobs );
      for (; it.current(); ++it)
        {
          KoHTMLJob *job = it.current();

	  if ( job->view() == m_pInternalView ||
	       m_pInternalView->isChild( job->view() ) )
	     {
               cerr << "killing job (" << job->url() << ")" << endl;
               m_lstJobs.removeRef(job);
               job->kill();
             }

	}
*/
/*      m_lstHTMLViews.removeRef( m_pInternalView );
      delete m_pInternalView;
      m_pInternalView = 0L;
    }*/
}

void KoHTMLDoc::removeHTMLView(KMyHTMLView *view)
{
  view->stop();

  m_lstHTMLViews.removeRef(view);
}

void KoHTMLDoc::requestDocument( KMyHTMLView *view, const char *url, bool reload )
{
  KoHTMLJob *job = new KoHTMLJob( view, url, url, KoHTMLJob::HTML, reload );

  QObject::connect( job, SIGNAL(jobData( KoHTMLJob *, const char *, int, bool )),
                    this, SLOT(slotJobData( KoHTMLJob *, const char *, int, bool )));

  //is a top-level view?		
  if ( m_lstHTMLViews.findRef( view ) != -1 )
    QObject::connect( job, SIGNAL(sigRedirection(int, const char *)),
                      this, SLOT(slotHTMLRedirect(int, const char *)));

  QObject::connect( job, SIGNAL(jobError(KoHTMLJob *, const char *)),
                    this, SLOT(slotHTMLLoadError(KoHTMLJob *, const char *)));

  m_lstJobs.append( job );

  job->start();		
}

void KoHTMLDoc::cancelDocument( KMyHTMLView *view, const char *url )
{
  KoHTMLJob *job = findJob( view, url, KoHTMLJob::HTML );

  if (!job)
    return; //aarghl...

  m_lstJobs.removeRef( job );

  job->kill();
}

void KoHTMLDoc::viewFinished( KMyHTMLView *view )
{
  if (m_lstHTMLViews.findRef( view ) == -1)
    return; //aaiieeee

//  if ( m_lstHTMLViews.count() > 0 && view == m_pInternalView )
//    return;

  emit contentChanged();
  documentDone();
  m_bRepaintDocument = true;
}

void KoHTMLDoc::requestImage( KMyHTMLView *view, const char *url, bool reload )
{
  KoHTMLJob *job = new KoHTMLJob( view, url, url, KoHTMLJob::Image, reload );

  QObject::connect( job, SIGNAL(jobData( KoHTMLJob *, const char *, int, bool )),
                    this, SLOT(slotJobData( KoHTMLJob *, const char *, int, bool )));

// leave out error handling.......arghl

  m_lstJobs.append( job );

  job->start();		
}

void KoHTMLDoc::cancelImage( KMyHTMLView *view, const char *url )
{
  KoHTMLJob *job = findJob( view, url, KoHTMLJob::HTML );

  if (!job)
    return; //aarghl...

  m_lstJobs.removeRef( job );

  job->kill();
}

void KoHTMLDoc::slotJobData( KoHTMLJob *job, const char *data, int len, bool eof )
{
  if ( job->type() == KoHTMLJob::HTML )
    job->view()->feedDocumentData( data, eof );
  else
    job->view()->data( job->url(), data, len, eof );

  if ( eof )
    m_lstJobs.removeRef( job );
}

void KoHTMLDoc::slotDocumentDoneInternal(KHTMLView *view)
{
  m_bDocumentDone = true;
  m_bRepaintDocument = true;
}

KoHTMLJob *KoHTMLDoc::findJob( KMyHTMLView *view, const char *url, KoHTMLJob::JobType type )
{
  QListIterator<KoHTMLJob> it( m_lstJobs );

  for (; it.current(); ++it)
    if ( it.current()->view() == view &&
         it.current()->url() == url &&
	 it.current()->type() == type )
      return it.current();

  return 0L;
}
