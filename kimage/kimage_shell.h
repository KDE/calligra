#ifndef __kimage_shell_h__
#define __kimage_shell_h__

class KImageShell_impl;

#include <koMainWindow.h>

class KImageDoc;
class KImageView;

#include <qlist.h>
#include <qtimer.h>
#include <qstring.h>

class KImageShell : public KoMainWindow
{
  Q_OBJECT
public:
  // C++
  KImageShell();
  ~KImageShell();

  // C++
  virtual void cleanUp();
  void setDocument( KImageDoc *_doc );

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
  
  KImageDoc* m_pDoc;
  KImageView* m_pView;

  static QList<KImageShell>* s_lstShells;
};

#endif
