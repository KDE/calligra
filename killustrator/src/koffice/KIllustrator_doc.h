#ifndef KIllustrator_doc_h_
#define KIllustrator_doc_h_

#include <part_frame_impl.h>
#include <document_impl.h>
#include <koDocument.h>

#include "KIllustrator.h"
#include "GDocument.h"

#define MIME_TYPE "application/x-killustrator"
#define EDITOR "IDL:KIllustrator/Document:1.0"

class KIllustratorDocument;
class KIllustratorView;

class KIllustratorChild : public KoDocumentChild {
public:
  KIllustratorChild (KIllustratorDocument* killu, const QRect& rect, 
		     OPParts::Document_ptr doc);
  ~KIllustratorChild ();

  KIllustratorDocument* parent () { return m_pKilluDoc; }
protected:
  KIllustratorDocument *m_pKilluDoc; 
};

class KIllustratorDocument : public GDocument,
			virtual public KoDocument, 
			virtual public KIllustrator::Document_skel {
  Q_OBJECT
public:
  KIllustratorDocument ();
  ~KIllustratorDocument ();

  // --- C++ ---
  // Overloaded methods from KoDocument

  bool save (ostream& os);
  bool loadChildren (OPParts::MimeMultipartDict_ptr dict);
  bool hasToWriteMultipart ();

  void cleanUp ();

  // --- IDL ---
  // Overloaded methods from OPParts::Document
  //
  virtual CORBA::Boolean init ();

  // create a view
  virtual OPParts::View_ptr createView ();

  // get list of views
  virtual void viewList (OPParts::Document::ViewList*& list_ptr);

  void removeView (KIllustratorView* view);

  // get mime type
  virtual char* mimeType () { return CORBA::string_dup (MIME_TYPE); }

  // ask, if document is modified
  virtual CORBA::Boolean isModified ();

protected:
  QList<KIllustratorView> m_lstViews;
  QList<KIllustratorChild> m_lstChildren;
  KIllustratorView *viewPtr;
};

#endif
