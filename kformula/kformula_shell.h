#ifndef __kformula_shell_h__
#define __kformula_shell_h__

class KFormulaShell_impl;

#include <koMainWindow.h>

class KFormulaDoc;
class KFormulaView;

#include <qlist.h>
#include <qtimer.h>
#include <qstring.h>

class KFormulaShell : public KoMainWindow
{
  Q_OBJECT
public:
  // C++
  KFormulaShell();
  ~KFormulaShell();

  // C++
  virtual void cleanUp();
  void setDocument( KFormulaDoc *_doc );

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
  virtual KoDocument* document();
  virtual KoViewIf* view();

  virtual bool printDlg();
  virtual void helpAbout();
  virtual int documentCount();

  bool isModified();
  bool requestClose();

  void releaseDocument();
  
  KFormulaDoc* m_pDoc;
  KFormulaView* m_pView;

  static QList<KFormulaShell>* s_lstShells;
};

#endif
