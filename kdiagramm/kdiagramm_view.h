#ifndef __kdiagramm_gui_h__
#define __kdiagramm_gui_h__

class KDiagrammView;
class KDiagrammDoc;
class KDiagrammShell;

#include <koFrame.h>
#include <koView.h>
#include <opMenu.h>
#include <opToolBar.h>
#include <koFrame.h>
#include <openparts_ui.h>

#include <qlist.h>
#include <qscrbar.h>
#include <qlabel.h> 
#include <qbutton.h>
#include <qpoint.h>

#include "kdiagramm.h"
#include <koDiagramm.h>

/**
 */
class KDiagrammView : public KoDiagrammView,
		    virtual public KoViewIf,
		    virtual public KDiagramm::View_skel
{
    Q_OBJECT
public:
    KDiagrammView( QWidget *_parent, const char *_name, KDiagrammDoc *_doc );
    ~KDiagrammView();

    KDiagrammDoc* doc() { return m_pDoc; }
  
    /**
     * ToolBar
     */
    void modeLines();
    /**
     * ToolBar
     */
    void modeAreas();
    /**
     * ToolBar
     */
    void modeBars();
    /**
     * ToolBar
     */
    void modeCakes();

    /**
     * MenuBar
     */
    void editData();
  
    /**
     * MenuBar
     */
    void pageLayout();

    virtual void cleanUp();

    CORBA::Boolean printDlg();
  
public slots:
    // Document signals
    void slotUpdateView();

protected:
    // C++
    virtual void init();
    // IDL
    virtual bool event( const char* _event, const CORBA::Any& _value );
    // C++
    virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
    virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );

    virtual void newView();
    virtual void helpUsing();
  
    OpenPartsUI::ToolBar_var m_vToolBarEdit;
    CORBA::Long m_idButtonEdit_Lines;
    CORBA::Long m_idButtonEdit_Areas;
    CORBA::Long m_idButtonEdit_Bars;
    CORBA::Long m_idButtonEdit_Cakes;

    OpenPartsUI::Menu_var m_vMenuEdit;
    CORBA::Long m_idMenuEdit_Lines;
    CORBA::Long m_idMenuEdit_Areas;
    CORBA::Long m_idMenuEdit_Bars;
    CORBA::Long m_idMenuEdit_Cakes;
    CORBA::Long m_idMenuEdit_Data;
    CORBA::Long m_idMenuEdit_Page;
    OpenPartsUI::Menu_var m_vMenuHelp;
    CORBA::Long m_idMenuHelp_About;
    CORBA::Long m_idMenuHelp_Using;
    
    KDiagrammDoc *m_pDoc;  
};

#endif


