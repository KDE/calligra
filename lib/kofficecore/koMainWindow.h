#ifndef __ko_main_window_h__
#define __ko_main_window_h__

#include <opMainWindow.h>
#include <opMainWindowIf.h>
#include <openparts.h>
#include <koffice.h>

class OPMenu;
class OPMenuBar;
class KoFrame;
class KoMainWindowIf;
class KoDocument;
class KoViewIf;

class KoMainWindow : public OPMainWindow
{
  Q_OBJECT
public:
  KoMainWindow( const char *_name = 0L );
  ~KoMainWindow();

  virtual OPMainWindowIf* interface();
  virtual KoMainWindowIf* koInterface();

  virtual void createFileMenu( OPMenuBar* );
  virtual void createHelpMenu( OPMenuBar* );

  virtual void setRootPart( unsigned long _part_id );
  
  virtual void cleanUp();

  virtual KoDocument* document() = 0L;
  virtual KoViewIf* view() = 0L;

protected slots:
  virtual void slotActivePartChanged( unsigned long _new_part_id, unsigned long _old_opart_id );  

  virtual void slotFileNew();
  virtual void slotFileOpen();
  virtual void slotFileSave();
  virtual void slotFileSaveAs();
  virtual void slotFilePrint();
  virtual void slotFileClose();
  virtual void slotFileQuit();
  virtual void slotHelpAbout();
  
protected:
  OPMenu* m_pFileMenu;
  OPMenu* m_pHelpMenu;
  
  int m_idMenuFile_New;
  int m_idMenuFile_Open;
  int m_idMenuFile_Save;
  int m_idMenuFile_SaveAs;
  int m_idMenuFile_Print;
  int m_idMenuFile_Close;
  int m_idMenuFile_Quit;
  int m_idMenuHelp_About;
  
  enum { TOOLBAR_NEW, TOOLBAR_OPEN, TOOLBAR_SAVE, TOOLBAR_PRINT }; 

  KoFrame* m_pFrame;
  KoMainWindowIf* m_pKoInterface;
};

class KoMainWindowIf : virtual public OPMainWindowIf,
		       virtual public KOffice::MainWindow_skel
{
public:
  KoMainWindowIf( KoMainWindow* _main );
  ~KoMainWindowIf();

  // IDL
  virtual void setMarkedPart( OpenParts::Id id );		
  virtual KOffice::Document_ptr document();
  virtual KOffice::View_ptr view();
  virtual CORBA::Boolean partClicked( OpenParts::Id _part_id, CORBA::Long _button );
  
protected:
  void unmarkPart();
  
  KoMainWindow* m_pKoMainWindow;

  OpenParts::Id m_iMarkedPart;
};

#endif
