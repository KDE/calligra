#ifndef __kchart_view_h__
#define __kchart_view_h__

class KChartView;
class KChartDocument;
class KChartChild;

#include <view_impl.h>
#include <document_impl.h>
#include <part_frame_impl.h>
#include <menu_impl.h>
#include <toolbar_impl.h>

#include "kchart.h"

#include <qwidget.h>
#include <qlist.h>

class KChartView : public QWidget,
		   virtual public View_impl,
		   virtual public KCharter::View_skel
{
  Q_OBJECT
public:
  // C++
  KChartView( QWidget *_parent = 0L, const char *_name = 0L );
  virtual ~KChartView();

  // IDL
  virtual void newView();
  
  // C++
  virtual void setDocument( KChartDocument *_doc );
  virtual void createGUI();

public slots:
  // Document signals
  void slotChartModified();
  
protected:
  virtual void cleanUp();
  
  void paintEvent( QPaintEvent *_ev );
  void resizeEvent( QResizeEvent *_ev );

  KChartDocument *m_pDoc;
  
  OPParts::MenuBarFactory_var m_vMenuBarFactory;
  MenuBar_ref m_rMenuBar;
  CORBA::Long m_idMenuView;
  CORBA::Long m_idMenuView_NewView;
  
  /* OPParts::ToolBarFactory_var m_vToolBarFactory;
  ToolBar_ref m_rToolBarEdit;
  CORBA::Long m_idButtonEdit_Darker; */
};

#endif
