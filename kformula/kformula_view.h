#ifndef __kformula_view_h__
#define __kformula_view_h__

class KFormulaView;
class KFormulaDocument;

#include <view_impl.h>
#include <document_impl.h>
#include <part_frame_impl.h>
#include <menu_impl.h>
#include <toolbar_impl.h>

#include "kformula.h"


#include <qwidget.h>
#include <qlist.h>
#include <qkeycode.h>

class KFormulaView : public QWidget,
		   virtual public View_impl,
		   virtual public KFormula::View_skel
{
    Q_OBJECT
public:
    // C++
    KFormulaView( QWidget *_parent = 0, const char *_name = 0 );
    virtual ~KFormulaView();

    // IDL
    virtual void newView();

    // C++
    virtual void setDocument( KFormulaDocument *_doc );
    virtual void createGUI();

public slots:
    // Document signals
    void slotModified();
    void slotTypeChanged(int type);

protected:
    virtual void cleanUp();

    CORBA::Long addToolButton (ToolBar_ref toolbar,
			       const char* pictname,
			       const char* tooltip,
			       const char* func);

    void mousePressEvent(QMouseEvent *a);
    void paintEvent( QPaintEvent *_ev );
    void resizeEvent( QResizeEvent *_ev );
    void keyPressEvent( QKeyEvent *k );

    KFormulaDocument *m_pDoc;

    OPParts::MenuBarFactory_var m_vMenuBarFactory;
    MenuBar_ref m_rMenuBar;
    CORBA::Long m_idMenuView;
    CORBA::Long m_idMenuView_NewView;

    OPParts::ToolBarFactory_var m_vToolBarFactory;
    ToolBar_ref m_rToolBarFormula;
    ToolBar_ref m_rToolBarFont;
    ToolBar_ref m_rToolBarType;
    CORBA::Long m_idButtonFormula_0;
    CORBA::Long m_idButtonFormula_1;
    CORBA::Long m_idButtonFormula_2;
    CORBA::Long m_idButtonFormula_3;
    CORBA::Long m_idButtonFormula_4;
    CORBA::Long m_idButtonFormula_5;
    CORBA::Long m_idButtonFormula_6;
    CORBA::Long m_idButtonFont_0;
    CORBA::Long m_idButtonFont_1;
    CORBA::Long m_idButtonFont_2;
    CORBA::Long m_idButtonFont_3;
    CORBA::Long m_idButtonFont_4;
    CORBA::Long m_idButtonFont_5;
    CORBA::Long m_idButtonType_0;
    CORBA::Long m_idButtonType_1;
    CORBA::Long m_idButtonType_2;
    CORBA::Long m_idButtonType_3;
    CORBA::Long m_idButtonType_4;
    CORBA::Long m_idButtonType_5;

    // IDL
    virtual void addB0();
    virtual void addB1();
    virtual void addB4();
    virtual void addB4bis();
    virtual void addB3();
    virtual void addB2();
    virtual void addB5();
    virtual void reduce();
    virtual void enlarge();
    virtual void reduceRecur();
    virtual void enlargeRecur();
    virtual void enlargeAll();
    virtual void setGreek();
    virtual void addCh1();
    virtual void addCh2();
    virtual void addCh3();
};

#endif
