/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "KIllustrator_doc.h"
#include "KIllustrator_doc.moc"
#include "KIllustrator_view.h"
#include "KIllustrator_shell.h"

#include "GPart.h"
#include "GfxWrapper.h"
#include <opApplication.h>
#include <qmessagebox.h>
#include <klocale.h>

#include <koQueryTypes.h>
#include <koDocument.h>

KIllustratorChild::KIllustratorChild (KIllustratorDocument* killu,
				      const QRect& rect,
				      KOffice::Document_ptr doc)
  : KoDocumentChild (rect, doc) {
  m_pKilluDoc = killu;
  cout << "mime type = " << (const char *) m_strMimeType << endl;
  cout << "url = " << (const char *) m_strURL << " (" <<
    m_rDoc->url () << ")" << endl;
}

KIllustratorChild::~KIllustratorChild () {
  m_rDoc = 0L;
}

void KIllustratorChild::setURL (const char* url) {
  m_strURL = url;
}

void KIllustratorChild::setMimeType (const char *mime) {
  m_strMimeType = mime;
}

const char* KIllustratorChild::urlForSave () {
  return m_rDoc->url ();
}

KIllustratorDocument::KIllustratorDocument () {
  ADD_INTERFACE("IDL:KOffice/Print:1.0")
  cout << "create new KIllustratorDocument: ior = "
       << opapp_orb->object_to_string (this) << endl;

  GObject::registerPrototype ("object", new GPart ());

  m_lstViews.setAutoDelete (true);
  m_lstChildren.setAutoDelete (true);
  m_bEmpty = true;
}

KIllustratorDocument::~KIllustratorDocument () {
  cleanUp ();
}

bool KIllustratorDocument::save (ostream& os, const char* ) {
  cout << "save KIllu to stream !!!!!!!!!!!!!!!" << endl;
  return GDocument::saveToXml (os);
}

void KIllustratorDocument::cleanUp () {
  if (m_bIsClean)
    return;

  m_lstChildren.clear ();

  KoDocument::cleanUp ();
}


bool KIllustratorDocument::load (istream& in, KOStore::Store_ptr ) {
  cout << "load KIllu from stream !!!!!!!!!" << endl;
  if (GDocument::readFromXml (in)) {

    // now look for part objects in order to create the child list
    vector<GLayer*>::iterator i = layers.begin ();
    for (; i != layers.end (); i++) {
      GLayer* layer = *i;
      list<GObject*>& contents = layer->objects ();
      for (list<GObject*>::iterator oi = contents.begin ();
	   oi != contents.end (); oi++) {
	if ((*oi)->isA ("GPart")) {
	  GPart *part = (GPart *) *oi;
	  insertChild (part->getChild ());
	}
      }
    }
    return true;
  }
  return false;
}

bool KIllustratorDocument::loadChildren (KOStore::Store_ptr store) {
  QListIterator<KIllustratorChild> it (m_lstChildren);
  for (; it.current (); ++it) {
    if (! it.current ()->loadDocument (store))
      return false;
  }

  return true;
}

bool KIllustratorDocument::saveChildren (KOStore::Store_ptr _store, const char *_path) {
  cerr << "void KIllustratorDocument::saveChildren( KOStore::Store _store, const char *_path )" << endl;
  int i = 0;
  QListIterator<KIllustratorChild> it (m_lstChildren);
  for( ; it.current(); ++it )
    {
        // set the child document's url to an internal url (ex: "tar:/0/1")
        QString internURL = QString( "%1/%2" ).arg( _path ).arg( i++ );
        KOffice::Document_var doc = it.current()->document();
        if ( !doc->saveToStore( _store, 0L, internURL ) )
          return false;
    }
  return true;
}  

bool KIllustratorDocument::hasToWriteMultipart () {
  return (m_lstChildren.count () > 0);
}

void KIllustratorDocument::insertPart (const QRect& rect,
				       KoDocumentEntry& e) {
  KOffice::Document_var doc = e.createDoc();
  if (CORBA::is_nil (doc))
    return;

  if (! doc->initDoc ()) {
    QMessageBox::critical ((QWidget *) 0L, i18n ("KIllustrator Error"),
			   i18n ("Could not insert document"), i18n ("OK"));
    return;
  }

  KIllustratorChild *child =
    new KIllustratorChild (this, rect, doc);
  insertChild (child);

  GPart* part = new GPart (child);
  GDocument::insertObject (part);
  emit partInserted (child, part);
}


void KIllustratorDocument::insertChild (KIllustratorChild* child) {
  m_lstChildren.append (child);
  setModified (true);
}

void KIllustratorDocument::changeChildGeometry (KIllustratorChild* child,
						const QRect& r) {
  child->setGeometry (r);
  setModified (true);
  emit childGeometryChanged (child);
}

CORBA::Boolean KIllustratorDocument::initDoc () {
  return true;
}

KOffice::MainWindow_ptr KIllustratorDocument::createMainWindow()
{
  KIllustratorShell* shell = new KIllustratorShell;
  shell->show();
  shell->setDocument( this );

  return KOffice::MainWindow::_duplicate( shell->koInterface() );
}

KIllustratorView* KIllustratorDocument::createKIllustratorView ( QWidget* _parent ) {
  KIllustratorView *view = new KIllustratorView (_parent, 0L, this);
  //view->QWidget::show ();
  m_lstViews.append (view);
  return view;
}

OpenParts::View_ptr KIllustratorDocument::createView () {
  return OpenParts::View::_duplicate (createKIllustratorView ());
}

void KIllustratorDocument::removeView (KIllustratorView* view) {
  m_lstViews.setAutoDelete (false);
  m_lstViews.removeRef (view);
  m_lstViews.setAutoDelete (true);
}

int KIllustratorDocument::viewCount () {
  return m_lstViews.count ();
}

void KIllustratorDocument::viewList (OpenParts::Document::ViewList*& list_ptr) {
  list_ptr->length (m_lstViews.count ());
  int i = 0;
  QListIterator<KIllustratorView> it (m_lstViews);
  for (; it.current (); ++it)
    (*list_ptr)[i++] = OpenParts::View::_duplicate (it.current ());
}

CORBA::Boolean KIllustratorDocument::isModified () {
  return GDocument::isModified ();
}

void KIllustratorDocument::setModified (bool f) {
  GDocument::setModified (f);
  if (f)
    m_bEmpty = false;
}

void KIllustratorDocument::draw (QPaintDevice* dev,
				 CORBA::Long , CORBA::Long ,
				 CORBA::Float _scale ) {
  Painter painter;
  painter.begin (dev);

  if ( _scale != 1.0 )
    painter.scale( _scale, _scale );

  GDocument::drawContents (painter);
  painter.end ();
}

CORBA::Boolean KIllustratorDocument::checkForSelection () {
  cout << "check for selection..." << endl;
  return !selectionIsEmpty ();
}

KIllustrator::GfxObjectSeq* KIllustratorDocument::getSelection () {
  KIllustrator::GfxObjectSeq* seq = new KIllustrator::GfxObjectSeq;
  seq->length (selectionCount ());
  int n = 0;
  for (list<GObject*>::iterator i = selection.begin ();
       i != selection.end (); i++) {
    GfxWrapper* wobj = (GfxWrapper *) (*i)->getWrapper ();
    if (wobj == 0L)
      wobj = new GfxWrapper (this, *i);
    (*seq)[n++] = KIllustrator::GfxObject::_duplicate (wobj);
  }

  return seq;
}

void KIllustratorDocument::addToSelection (KIllustrator::GfxObject_ptr) {
}

void
KIllustratorDocument::removeFromSelection (KIllustrator::GfxObject_ptr) {
}

void KIllustratorDocument::groupSelection () {
}
