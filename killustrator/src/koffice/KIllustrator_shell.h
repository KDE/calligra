#ifndef KIllustrator_shell_h_
#define KIllustrator_shell_h_

#include <default_shell_impl.h>
#include <document_impl.h>

#include "KIllustrator.h"
#include "KIllustrator_doc.h"

class KIllustratorShell : public DefaultShell_impl {
  Q_OBJECT
public:
  KIllustratorShell ();
  ~KIllustratorShell ();

  // set a document
  void setDocument (KIllustratorDocument* doc);

  // open a document
  bool openDocument (const char* filename);

  // save the document
  bool saveDocument (const char* filename, const char* fmt);

  void enableMenuBar ();

  void fileNew ();
  void fileOpen ();
  void fileSave ();
  void fileSaveAs ();
  void fileClose ();
  void fileQuit ();
  void helpAbout ();
  
  void cleanUp ();

protected:
  Document_ref m_rDoc;
  OPParts::View_var m_vView;

  MenuBar_ref m_rMenuBar;
  CORBA::Long m_idMenuFile, m_idMenuFile_New, m_idMenuFile_Open,
    m_idMenuFile_Save, m_idMenuFile_SaveAs, m_idMenuFile_Close,
    m_idMenuFile_Exit;
  CORBA::Long m_idMenuHelp_About;
};

#endif
