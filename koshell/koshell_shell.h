#ifndef __koshell_window_h__
#define __koshell_window_h__

#include <koMainWindow.h>
#include <koQueryTypes.h>
#include <koKoolBar.h>

#include <qlist.h>
#include <qtimer.h>
#include <qstring.h>

#include <list>
#include <vector>
#include <map>

class KoShellWindow : public KoMainWindow
{
  Q_OBJECT
public:
  // C++
  KoShellWindow();
  ~KoShellWindow();

  // C++
  virtual void cleanUp();

  // C++
  virtual bool newPage( KoDocumentEntry& _e );
  virtual bool closeApplication();
  virtual bool saveAllPages();
  virtual void releasePages();

  virtual void attachView( KoFrame* _frame, unsigned long _part_id );
  
protected slots:
  void slotFileNew();
  void slotFileOpen();
  void slotFileSave();
  void slotFileSaveAs();
  void slotFilePrint();
  void slotFileClose();
  void slotFileQuit();

  void slotKoolBar( int _grp, int _item );
  
protected:
  // C++
  virtual KOffice::Document_ptr document();
  virtual KOffice::View_ptr view();

  virtual bool printDlg();
  virtual void helpAbout();
  virtual int documentCount();

  bool isModified();
  bool requestClose();

  struct Page
  {
    KOffice::Document_var m_vDoc;
    KOffice::View_var m_vView;
    KoFrame* m_pFrame;
    int m_id;
  };
  list<Page> m_lstPages;
  
  list<Page>::iterator m_activePage;

  KoKoolBar* m_pKoolBar;

  int m_grpFile;
  int m_grpDocuments;
  
  vector<KoDocumentEntry> m_lstComponents;
  map<int,KoDocumentEntry*> m_mapComponents;
  
  static QList<KoShellWindow>* s_lstShells;
};

#endif
