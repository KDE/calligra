#ifndef __koshell_window_h__
#define __koshell_window_h__

#include <koMainWindow.h>
#include <qlist.h>
#include <qvaluelist.h>

class KoKoolBar;
class KoDocumentEntry;

class KoShellWindow : public KoMainWindow
{
  Q_OBJECT

public:

  // C++
  KoShellWindow();
  ~KoShellWindow();

  virtual void cleanUp();

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

  struct Page
  {
    KOffice::Document_var m_vDoc;
    KOffice::View_var m_vView;
    KoFrame* m_pFrame;
    int m_id;
  };

  virtual KOffice::Document_ptr document();
  virtual KOffice::View_ptr view();

  virtual bool printDlg();
  virtual void helpAbout();
  virtual int documentCount();

  bool isModified();
  bool requestClose();

  QValueList<Page> m_lstPages;
  QValueList<Page>::Iterator m_activePage;

  KoKoolBar* m_pKoolBar;

  int m_grpFile;
  int m_grpDocuments;
  
  QValueList<KoDocumentEntry> m_lstComponents;
  QMap<int,KoDocumentEntry*> m_mapComponents;

  OpenParts::Part_var m_vKfm;
  KoFrame* m_pKfmFrame;
  
  static QList<KoShellWindow>* s_lstShells;
};

#endif // __koshell_window_h__

