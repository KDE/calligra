#ifndef __kspread_shell_h__
#define __kspread_shell_h__

class KSpreadShell_impl;

#include <koMainWindow.h>

class KSpreadDoc;
class KSpreadView;

#include <qlist.h>
#include <qtimer.h>
#include <qstring.h>

class KSpreadShell : public KoMainWindow
{
  Q_OBJECT
public:
  // C++
  KSpreadShell();
  ~KSpreadShell();

  // C++
  virtual void cleanUp();
  void setDocument( KSpreadDoc *_doc );

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
  
  KSpreadDoc* m_pDoc;
  KSpreadView* m_pView;

  static QList<KSpreadShell>* s_lstShells;
};

#endif
