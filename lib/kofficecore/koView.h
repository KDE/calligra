#ifndef __ko_view_h__
#define __ko_view_h__

#include <koffice.h>
#include <opView.h>

class KoDocument;

class KoViewIf : virtual public OPViewIf,
		 virtual public KOffice::View_skel
{
public:
  KoViewIf( KoDocument* _doc );
  ~KoViewIf();
  
  // IDL
  virtual KOffice::View::Mode mode();
  virtual void setMode( KOffice::View::Mode _mode );

  // IDL
  virtual CORBA::Boolean isMarked();
  virtual void setMarked( CORBA::Boolean _marked );

  // IDL
  virtual CORBA::Boolean printDlg();
  virtual void setMainWindow( OpenParts::MainWindow_ptr _main );
  
  // IDL
  void KoViewIf::setFocus( CORBA::Boolean _focus );

  // IDL
  virtual CORBA::ULong leftGUISize() { return 0; }
  virtual CORBA::ULong rightGUISize() { return 0; }
  virtual CORBA::ULong topGUISize() { return 0; }
  virtual CORBA::ULong bottomGUISize() { return 0; }
  
protected:
  CORBA::Boolean m_bMarked;
  
  KOffice::View::Mode m_eMode;
  KOffice::MainWindow_var m_vKoMainWindow;
};

#endif
