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

#include "khtmlwidget_patched.h"
#include "kfileio.h"

KoHTMLChild::KoHTMLChild(KoHTMLDoc *doc, const KRect &rect, KOffice::Document_ptr koDoc)
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

  KIOJob::initStatic();

  m_lstViews.setAutoDelete(false);
  m_lstChildren.setAutoDelete(true);
  m_lstHTMLViews.setAutoDelete(false);
  m_lstJobs.setAutoDelete(false);

  m_bModified = false;

  m_eSaveLoadMode = KoHTML::KoHTMLDocument::URLOnly;

  m_strHTMLData = "";  
  m_strCurrentURL = "";
  
  m_pInternalView = new KHTMLView_Patched;
  
  addHTMLView(m_pInternalView);

  QObject::connect(this, SIGNAL(contentChanged()),
                         SLOT(slotUpdateInternalView()));
  QObject::connect(m_pInternalView, SIGNAL(documentDone(KHTMLView *)),
                   this, SLOT(slotDocumentDoneInternal(KHTMLView *)));
  
}

KoHTMLDoc::~KoHTMLDoc()
{
  cerr << "removing internal view from list" << endl;
  removeHTMLView(m_pInternalView);
  cerr << "deleting internal view" << endl;
  delete m_pInternalView;
  cerr << "KoHTMLDoc is finished :)" << endl;
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

KoHTMLView *KoHTMLDoc::createKoHTMLView()
{
  KoHTMLView *p = new KoHTMLView(0L, 0L, this);
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

CORBA::Boolean KoHTMLDoc::init()
{
  return true;
}

char *KoHTMLDoc::htmlData()
{
  return CORBA::string_dup( m_strHTMLData.data() );
}

char *KoHTMLDoc::htmlURL()
{
  return CORBA::string_dup( m_strCurrentURL.data() );
}

void KoHTMLDoc::openURL(const char *_url)
{
  m_strCurrentURL = _url;
  m_strCurrentURL = m_strCurrentURL.stripWhiteSpace();
  
  KURL u(m_strCurrentURL);
  
  if (!u.isMalformed())
     {
       stopLoading();
       
       m_bLoadError = false;
       m_bDocumentDone = false;
     
       m_strHTMLData = "";
       
       KoHTMLJob *job = new KoHTMLJob(0L, 0L, u.url(), KoHTMLJob::HTML);
       
       QObject::connect(job, SIGNAL(jobDone(KoHTMLJob *, KHTMLView *, KHTMLView *, const char *, const char *, int)),
                        this, SLOT(slotHTMLCodeLoaded(KoHTMLJob *, KHTMLView *, KHTMLView *, const char *, const char *, int)));
       QObject::connect(job, SIGNAL(jobError(const char *)),
                        this, SLOT(slotHTMLLoadError(const char *)));
       QObject::connect(job, SIGNAL(sigRedirection(int, const char *)),
                        this, SLOT(slotHTMLRedirect(int, const char *)));
     
       m_lstJobs.append(job);
     
       job->start();
       documentStarted();
     }
}

void KoHTMLDoc::feedData(const char *url, const char *data)
{
  documentStarted();
  m_strCurrentURL = url;
  m_strHTMLData = data;
  m_bDocumentDone = true;
  documentDone();
  emit contentChanged();
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
  QListIterator<KoHTMLJob> it(m_lstJobs);
  for (; it.current(); ++it)
      {
        KoHTMLJob *job = it.current();
        m_lstJobs.removeRef(job);
        job->kill();
        cerr << "killed job" << endl;
      }     
      
  assert( m_lstJobs.count() == 0 );      
}

KoHTML::KoHTMLDocument::SaveLoadMode KoHTMLDoc::saveLoadMode()
{
  return m_eSaveLoadMode;
}

void KoHTMLDoc::setSaveLoadMode( KoHTML::KoHTMLDocument::SaveLoadMode mode )
{
  m_eSaveLoadMode = mode;
}

void KoHTMLDoc::slotHTMLCodeLoaded(KoHTMLJob *job, KHTMLView *topParent, KHTMLView *parent, const char *url, const char *data, int len)
{
  cerr << "void KoHTMLDoc::slotHTMLCodeLoaded(...)" <<endl;
  m_strHTMLData = QString(data, len);

  cerr << "removing job ref" << endl;  
  m_lstJobs.removeRef(job);
  
  cerr << "emitting contentChanged()" << endl;
  emit contentChanged();    
  cerr << "done... now our job should kill itself..." << endl;
}

void KoHTMLDoc::slotHTMLLoadError(const char *errMsg)
{
  m_bLoadError = true;
  m_strErrorMsg = errMsg;
}

void KoHTMLDoc::slotHTMLRedirect(int id, const char *url)
{
  m_strCurrentURL = url;
}

void KoHTMLDoc::draw(QPaintDevice *dev, CORBA::Long width, CORBA::Long height,
		     CORBA::Float _scale )
{ 
  QPainter painter;
  painter.begin(dev);

  if (_scale != 1.0) painter.translate(_scale, _scale);
  
  if (m_strCurrentURL.isEmpty())
     {
       const char *msg = i18n("KoHTML: No document loaded!");
       
       QRect r = painter.fontMetrics().boundingRect(msg);
       
       painter.drawText((width / 2) - (r.width() / 2), (height / 2) - (r.height() / 2) , msg);
       
       drawChildren(&painter, _scale);
       
       painter.end();
       
       return;
     }
     
//  if (!m_bDocumentDone) return;

  if (m_lstHTMLViews.count() > 1)
     {
       KHTMLView *v = m_lstHTMLViews.at(1);
       int x = v->xOffset();
       int y = v->yOffset();
       
       m_pInternalView->gotoXY(x, y);
     }

  cerr << "void KoHTMLDoc::draw(QPaintDevice *dev, CORBA::Long width, CORBA::Long height)" << endl;

  cerr << "saving ourselves ;)" << endl;
  SavedPage *p = m_pInternalView->saveYourself();

  cerr << "drawing" << endl;
  m_pInternalView->draw(p, &painter, width, height, _scale);
  
  delete p;
  
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
		   
	       openURL( tmp.c_str() );
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
	      
  KURL u(m_strCurrentURL);
  out << otag << "<URL>" << u.url() << "</URL>" << endl;
  
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

void KoHTMLDoc::insertObject(const KRect &rect, KoDocumentEntry &de)
{
  KOffice::Document_var doc = imr_createDoc(de);
  
  if (CORBA::is_nil(doc))
     return;
     
  if (!doc->init())
     {
       QMessageBox::critical(0L, i18n("KoHTML Error"), i18n("Could not initialize child document"), i18n("Ok"));
       return;
     }
    
  KoHTMLChild *c = new KoHTMLChild(this, rect, doc);
  
  insertChild(c);    
        
  m_bModified = true;     
}

void KoHTMLDoc::insertChild(KoHTMLChild *child)
{
  m_lstChildren.append(child);
  
  emit sig_insertObject(child);

  m_bModified = true;
}

void KoHTMLDoc::changeChildGeometry(KoHTMLChild *child, const KRect &rect)
{
  child->setGeometry(rect);
  child->setkGeometry(rect);
  
  emit sig_updateChildGeometry(child);
  
  m_bModified = true; 
}

QListIterator<KoHTMLChild> KoHTMLDoc::childIterator()
{
  return QListIterator<KoHTMLChild>(m_lstChildren);
}

void KoHTMLDoc::addHTMLView(KHTMLView *view)
{
  m_lstHTMLViews.append(view);  
  
  QObject::connect(view, SIGNAL(documentRequest(KHTMLView *, const char *)),
                   this, SLOT(slotDocumentRequest(KHTMLView *, const char *)));
  QObject::connect(view, SIGNAL(cancelDocumentRequest(KHTMLView *, const char *)),
                   this, SLOT(slotCancelDocumentRequest(KHTMLView *, const char *)));
  QObject::connect(view, SIGNAL(imageRequest(KHTMLView *, const char *)),
                   this, SLOT(slotImageRequest(KHTMLView *, const char *)));
  QObject::connect(view, SIGNAL(cancelImageRequest(KHTMLView *, const char *)),
                   this, SLOT(slotCancelImageRequest(KHTMLView *, const char *)));
  QObject::connect(view, SIGNAL(documentStarted(KHTMLView *)),
                   this, SLOT(slotDocumentStarted(KHTMLView *)));
  QObject::connect(view, SIGNAL(documentDone(KHTMLView *)),
                   this, SLOT(slotDocumentDone(KHTMLView *)));
		   
  QObject::connect(this, SIGNAL(imageLoaded(const char *, const char *)),
                   view, SLOT(slotImageLoaded(const char *, const char *)));
}

void KoHTMLDoc::removeHTMLView(KHTMLView *view)
{
  QListIterator<KoHTMLJob> it(m_lstJobs);
  
  cerr << "void KoHTMLDoc::removeHTMLView(KHTMLView *view)" << endl;
  
  cerr << "removing jobs" << endl;
  
  for (; it.current(); ++it)
      {
        KoHTMLJob *j = it.current();
	if ((j->topParent() == view))
	   {
	     m_lstJobs.removeRef(j);
	     delete j;
	     cerr << "removed job" << endl;
	   }     
      }

  cerr << "disconnecting" << endl;
  
  view->disconnect(this);
  
  cerr << "removing listref" << endl;
  m_lstHTMLViews.removeRef(view);
  cerr << "done" << endl;
}

KoHTMLJob *KoHTMLDoc::findJob(KHTMLView *view, const char *url, KoHTMLJob::JobType jType)
{
  QListIterator<KoHTMLJob> it(m_lstJobs);
  
  for (; it.current(); ++it)
      {
        KoHTMLJob *j = it.current();
	if ((j->type() == jType) &&
	    (strcmp(j->url(), url) == 0) &&
	    (j->parent() == view))
	   return j;
      }
      
  return 0L;      
}

void KoHTMLDoc::slotDocumentRequest(KHTMLView *view, const char *url)
{
  KHTMLView *topView = view->topView();
  
  if (m_lstHTMLViews.findRef(topView) == -1)
     {
       cerr << "AAAAAAIEEEEEE!!!" << endl;
       return;
     }
     
  if (view == topView)
     {
       openURL(url);
       return;
     }     
     
  KoHTMLJob *job = new KoHTMLJob(topView, view, url, KoHTMLJob::HTML);
  
  m_lstJobs.append(job);
  
  QObject::connect(job, SIGNAL(jobDone(KoHTMLJob *, KHTMLView *, KHTMLView *, const char *, const char *)),
                   this, SLOT(slotDocumentLoaded(KoHTMLJob *, KHTMLView *, KHTMLView *, const char *, const char *)));
  job->start();		   
}

void KoHTMLDoc::slotCancelDocumentRequest(KHTMLView *view, const char *url)
{
  KoHTMLJob *job = findJob(view, url, KoHTMLJob::HTML);
  
  if (!job)
     {
       cerr << "void KoHTMLDoc::slotCancelDocumentRequest(KHTMLView *view, const char *url)" << endl;
       cerr << "no job for url " << url << endl;
       return;
     }

  m_lstJobs.removeRef(job);
  job->kill();
}

void KoHTMLDoc::slotImageRequest(KHTMLView *view, const char *url)
{
  KHTMLView *topView = view->topView();
  
  if (m_lstHTMLViews.findRef(topView) == -1)
     {
       cerr << "AAAAAAIEEEEEE!!!" << endl;
       return;
     }
     
  KoHTMLJob *job = new KoHTMLJob(topView, view, url, KoHTMLJob::Image);
  
  m_lstJobs.append(job);
  
  QObject::connect(job, SIGNAL(jobDone(KoHTMLJob *, KHTMLView *, KHTMLView *, const char *, const char *)),
                   this, SLOT(slotImageLoaded(KoHTMLJob *, KHTMLView *, KHTMLView *, const char *, const char *)));
  job->start();		   
}

void KoHTMLDoc::slotCancelImageRequest(KHTMLView *view, const char *url)
{
  KoHTMLJob *job = findJob(view, url, KoHTMLJob::Image);
  
  if (!job)
     {
       cerr << "void KoHTMLDoc::slotCancelImageRequest(KHTMLView *view, const char *url)" << endl;
       cerr << "no job for url " << url << endl;
       return;
     }

  m_lstJobs.removeRef(job);
  job->kill();
}

void KoHTMLDoc::slotDocumentStarted(KHTMLView *view)
{
  KHTMLView *topView = view->topView();
  
  if (m_lstHTMLViews.findRef(topView) == -1)
     {
       cerr << "AAAAAAIEEEEEE!!!" << endl;
       return;
     }
     
//  if (view == topView) documentStarted();
}

void KoHTMLDoc::slotDocumentDone(KHTMLView *view)
{
  KHTMLView *topView = view->topView();
  
  if (m_lstHTMLViews.findRef(topView) == -1)
     {
       cerr << "AAAAAAIEEEEEE!!!" << endl;
       return;
     }
     
  if (view == topView) 
    {
      documentDone();
      
      // BadHack(tm) ... :-((
      if (m_bLoadError)
         {
           m_bLoadError = false;
           QMessageBox::critical(0L, i18n("KoHTML IO Error"), m_strErrorMsg, i18n("Abort"));
	 }  
    }  
}

void KoHTMLDoc::slotDocumentLoaded(KoHTMLJob *job, KHTMLView *topParent, KHTMLView *parent, const char *url, const char *filename)
{
  KHTMLView *topView = parent->topView();
  
  if (m_lstHTMLViews.findRef(topView) == -1)
     {
       cerr << "AAAAAAIEEEEEE!!!" << endl;
       return;
     }
  
  QString m_strHTMLData = kFileToString(filename);
  
  parent->begin(url);
  parent->parse();
  parent->write(m_strHTMLData);
  parent->end();
  
  m_lstJobs.removeRef(job);
}

void KoHTMLDoc::slotImageLoaded(KoHTMLJob *job, KHTMLView *topParent, KHTMLView *parent, const char *url, const char *filename)
{
  KHTMLView *topView = parent->topView();
  
  if (m_lstHTMLViews.findRef(topView) == -1)
     {
       cerr << "AAAAAAIEEEEEE!!!" << endl;
       return;
     }
     
  emit imageLoaded(url, filename);
  
  m_lstJobs.removeRef(job);
}

void KoHTMLDoc::slotUpdateInternalView()
{
  KURL u(m_strCurrentURL);
  
  m_bDocumentDone = false;
  
  m_pInternalView->begin(u.url());
  m_pInternalView->parse();
  m_pInternalView->write(m_strHTMLData);
  m_pInternalView->end();
}

void KoHTMLDoc::slotDocumentDoneInternal(KHTMLView *view)
{
  m_bDocumentDone = true;
}
