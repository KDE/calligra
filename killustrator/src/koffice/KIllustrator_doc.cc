#include "KIllustrator_doc.h"
#include "KIllustrator_doc.moc"
#include "KIllustrator_view.h"

KIllustratorChild::KIllustratorChild (KIllustratorDocument* killu, 
				      const QRect& rect, 
				      OPParts::Document_ptr doc) {
  m_pKilluDoc = killu;
  m_rDoc = OPParts::Document::_duplicate (doc);
  m_geometry = rect;
}

KIllustratorChild::~KIllustratorChild () {
  m_rDoc = 0L;
}


KIllustratorDocument::KIllustratorDocument () {
  m_lstViews.setAutoDelete (true);
  m_lstChildren.setAutoDelete (true);
}

KIllustratorDocument::~KIllustratorDocument () {
  cleanUp ();
}

bool KIllustratorDocument::save (ostream& os) {
  return false;
}

void KIllustratorDocument::cleanUp () {
  if (m_bIsClean)
    return;
  
  m_lstChildren.clear ();
  
  Document_impl::cleanUp ();
}

bool KIllustratorDocument::loadChildren (OPParts::MimeMultipartDict_ptr dict) {
  return false;
}

bool KIllustratorDocument::hasToWriteMultipart () {
  return true;
}

CORBA::Boolean KIllustratorDocument::init () {
  return true;
}

OPParts::View_ptr KIllustratorDocument::createView () {
  cout << "KIllustratorDocument::createView ()" << endl;
  KIllustratorView *view = new KIllustratorView (0L, 0L, this);
  //  view->setGeometry (5000, 5000, 100, 100);
  view->QWidget::show ();
  view->setDocument (this);
  m_lstViews.append (view);
  return OPParts::View::_duplicate (view);
}

void KIllustratorDocument::removeView (KIllustratorView* view) {
  m_lstViews.removeRef (view);
}

void KIllustratorDocument::viewList (OPParts::Document::ViewList*& list_ptr) {
  list_ptr->length (m_lstViews.count ());
  int i = 0;
  QListIterator<KIllustratorView> it (m_lstViews);
  for (; it.current (); ++it)
    (*list_ptr)[i++] = OPParts::View::_duplicate (it.current ());
}

CORBA::Boolean KIllustratorDocument::isModified () {
  return false;
}

