#ifndef __kdiagramm_shell_h__
#define __kdiagramm_shell_h__

class KDiagrammShell_impl;

#include <koMainWindow.h>

class KDiagrammDoc;
class KDiagrammView;

#include <qlist.h>
#include <qtimer.h>
#include <qstring.h>

class KDiagrammShell : public KoMainWindow
{
  Q_OBJECT
public:
  // C++
  KDiagrammShell();
  ~KDiagrammShell();

  // C++
  virtual void cleanUp();
  void setDocument( KDiagrammDoc *_doc );

  // C++
  virtual bool newDocument();
  virtual bool openDocument( const char *_filename, const char* _format );
  virtual bool saveDocument( const char *_file, const char *_format );
  virtual bool closeDocument();
  virtual bool closeAllDocuments();

protected slots:
  void slotFileNew();
  void slotFileOpen();
  void slotFileSave();
  void slotFileSaveAs();
  void slotFilePrint();
  void slotFileClose();
  void slotFileQuit();

protected:
  // C++
  virtual KOffice::Document_ptr document();
  virtual KOffice::View_ptr view();

  virtual bool printDlg();
  virtual void helpAbout();
  virtual int documentCount();

  bool isModified();
  bool requestClose();

  void releaseDocument();
  
  KDiagrammDoc* m_pDoc;
  KDiagrammView* m_pView;

  static QList<KDiagrammShell>* s_lstShells;
};

#endif
