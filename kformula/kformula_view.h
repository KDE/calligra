#ifndef __kformula_view_h__
#define __kformula_view_h__

class KFormulaView;
class KFormulaDoc;

#include <koFrame.h>
#include <koView.h>
#include <opMenu.h>
#include <opToolBar.h>
#include <koFrame.h>
#include <openparts_ui.h>

#include "kformula.h"


#include <qwidget.h>
#include <qlist.h>
#include <qkeycode.h>

class BasicElement;

class KFormulaView : public QWidget,
		     virtual public KoViewIf,
		     virtual public KFormula::View_skel
{
    Q_OBJECT
 public:
    // C++
    KFormulaView( QWidget *_parent, const char *_name, KFormulaDoc* _doc );
    virtual ~KFormulaView();

    // IDL
    virtual void newView();
    virtual CORBA::Boolean printDlg();   
public slots:
    // Document signals
    void modifyMatrix(QString str);
    void createMatrix(QString str);
    void slotModified();
    void slotTypeChanged( const BasicElement *elm);

protected:
    // C++
    virtual void init();
    // IDL
    virtual bool event( const char* _event, const CORBA::Any& _value );
    // C++
    virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
    virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );

    virtual void cleanUp();

    CORBA::Long addToolButton( OpenPartsUI::ToolBar_ptr toolbar,
			       const char* pictname,
			       const char* tooltip,
			       const char* func,
			       CORBA::Long id );

    void mousePressEvent(QMouseEvent *a);
    void paintEvent( QPaintEvent *_ev );
    void resizeEvent( QResizeEvent *_ev );
    void keyPressEvent( QKeyEvent *k );

    KFormulaDoc *m_pDoc;

    QPopupMenu *mn_indexList;

    OpenPartsUI::Menu_var m_vMenuView;
    CORBA::Long m_idMenuView_NewView;
    CORBA::Long m_idMenuView_FontToolbar;
    CORBA::Long m_idMenuView_TypeToolbar;
    CORBA::Long m_idMenuView_TextToolbar;
    
    OpenPartsUI::Menu_var m_vMenuElement;
    OpenPartsUI::Menu_var m_vMenuElement_AddIndex;    
    CORBA::Long m_idMenuElement_AddIndex_TL;
    CORBA::Long m_idMenuElement_AddIndex_BL;
    CORBA::Long m_idMenuElement_AddIndex_TR;
    CORBA::Long m_idMenuElement_AddIndex_BR;

    OpenPartsUI::Menu_var m_vMenuElement_AddElement;
    CORBA::Long m_idMenuElement_AddElement_T;  // text    
    CORBA::Long m_idMenuElement_AddElement_R;  // root
    CORBA::Long m_idMenuElement_AddElement_F;  // fraction 
    CORBA::Long m_idMenuElement_AddElement_V;  // vertSpace
    CORBA::Long m_idMenuElement_AddElement_B;  // bracket
    CORBA::Long m_idMenuElement_AddElement_I;  // integral
    CORBA::Long m_idMenuElement_AddElement_D;  // decoration
    CORBA::Long m_idMenuElement_AddElement_S;  // symbol
    CORBA::Long m_idMenuElement_AddElement_M;  // matrix
    OpenPartsUI::Menu_var m_vMenuElement_Text;
    CORBA::Long m_idMenuElement_Text_Font;
    CORBA::Long m_idMenuElement_Text_Split;
    OpenPartsUI::Menu_var m_vMenuElement_Root;
    CORBA::Long m_idMenuElement_Root_Pixmap;
    CORBA::Long m_idMenuElement_Root_Index;
    OpenPartsUI::Menu_var m_vMenuElement_Integral;
    CORBA::Long m_idMenuElement_Integral_Pixmap;
    CORBA::Long m_idMenuElement_Integral_Lower;
    CORBA::Long m_idMenuElement_Integral_Higher;
    OpenPartsUI::Menu_var m_vMenuElement_Bracket;
    CORBA::Long m_idMenuElement_Bracket_Type;
    OpenPartsUI::Menu_var m_vMenuElement_Matrix;
    CORBA::Long m_idMenuElement_Matrix_Set;
    CORBA::Long m_idMenuElement_Matrix_InsRow;
    CORBA::Long m_idMenuElement_Matrix_InsCol;
    CORBA::Long m_idMenuElement_Matrix_RemRow;
    CORBA::Long m_idMenuElement_Matrix_RemCol;
    OpenPartsUI::Menu_var m_vMenuElement_Fraction;
    CORBA::Long m_idMenuElement_Fraction_VA;  //Vert Allign
    CORBA::Long m_idMenuElement_Fraction_VA_U; 
    CORBA::Long m_idMenuElement_Fraction_VA_D; 
    CORBA::Long m_idMenuElement_Fraction_VA_M; 
    CORBA::Long m_idMenuElement_Fraction_HA;  //Horiz Allign
    CORBA::Long m_idMenuElement_Fraction_HA_C;
    CORBA::Long m_idMenuElement_Fraction_HA_L;
    CORBA::Long m_idMenuElement_Fraction_HA_R;
    CORBA::Long m_idMenuElement_Fraction_Dist;
    CORBA::Long m_idMenuElement_Fraction_MidLine;
    OpenPartsUI::Menu_var m_vMenuElement_Decoration;
    CORBA::Long m_idMenuElement_Decoration_Set;
    OpenPartsUI::Menu_var m_vMenuElement_Symbol;
    CORBA::Long m_idMenuElement_Symbol_Set;
    CORBA::Long m_idMenuElement_Color;
    CORBA::Long m_idMenuElement_Remove;
    OpenPartsUI::Menu_var m_vMenuFormula;
    CORBA::Long m_idMenuFormula_Color;    
    CORBA::Long m_idMenuFormula_Font;    

    OpenPartsUI::Menu_var m_vMenuHelp;
    CORBA::Long m_idMenuHelp_Using;
  
    OpenPartsUI::ToolBar_var m_vToolBarFormula;
    OpenPartsUI::ToolBar_var m_vToolBarFont;
    OpenPartsUI::ToolBar_var m_vToolBarType;
    CORBA::Long m_idButtonFormula_0;
    CORBA::Long m_idButtonFormula_1;
    CORBA::Long m_idButtonFormula_2;
    CORBA::Long m_idButtonFormula_3;
    CORBA::Long m_idButtonFormula_4;
    CORBA::Long m_idButtonFormula_5;
    CORBA::Long m_idButtonFormula_6;
    CORBA::Long m_idButtonFormula_7;
    CORBA::Long m_idButtonFont_Bold;
    CORBA::Long m_idButtonFont_Underl;
    CORBA::Long m_idButtonFont_Italic;
    CORBA::Long m_idButtonFont_0;
    CORBA::Long m_idButtonFont_1;
    CORBA::Long m_idButtonFont_2;
    CORBA::Long m_idButtonFont_3;
    CORBA::Long m_idButtonFont_4;
    CORBA::Long m_idButtonFont_5;
    CORBA::Long m_idComboFont_FontSize;
    CORBA::Long m_idComboFont_FontFamily;
    CORBA::Long m_idComboFont_ScaleMode;
    CORBA::Long m_idButtonType_Spl;
    CORBA::Long m_idButtonType_RIn;
    CORBA::Long m_idButtonType_Del;
    CORBA::Long m_idComboType_DelLeft;
    CORBA::Long m_idComboType_DelRight;
    CORBA::Long m_idButtonType_UAl;
    CORBA::Long m_idButtonType_MAl;
    CORBA::Long m_idButtonType_DAl;
    CORBA::Long m_idButtonType_LAl;
    CORBA::Long m_idButtonType_CAl;
    CORBA::Long m_idButtonType_RAl;
    CORBA::Long m_idButtonType_Mor;
    CORBA::Long m_idButtonType_Les;
    CORBA::Long m_idButtonType_Mid;
    CORBA::Long m_idButtonType_AddH;
    CORBA::Long m_idButtonType_AddL;
    CORBA::Long m_idButtonType_SetM;
    CORBA::Long m_idButtonType_InR;
    CORBA::Long m_idButtonType_InC;
    CORBA::Long m_idButtonType_ReR;
    CORBA::Long m_idButtonType_ReC;
    CORBA::Long m_idButtonType_Pix;
    // IDL
    virtual void addText();
    virtual void addRoot();
    virtual void addFraction();
    virtual void addVertSpace();
    virtual void addBracket();
    virtual void addMatrix();
    virtual void addIntegral();
    virtual void addSymbol();
    virtual void reduce();
    virtual void enlarge();
    virtual void fractionAlignU();
    virtual void fractionAlignD();
    virtual void fractionAlignM();
    virtual void fractionAlignL();
    virtual void fractionAlignR();
    virtual void fractionAlignC();
    virtual void fractionDist();
    virtual void fractionDistMore();
    virtual void fractionDistLess();
    virtual void fontSwitch();
    virtual void addTopLeftIndex();
    virtual void addBottomLeftIndex();
    virtual void addTopRightIndex();
    virtual void addBottomRightIndex();
    virtual void indexList();
    virtual void sizeSelected(const char*);
    virtual void fontSelected(const char*);
    virtual void modeSelected(const char*);
    virtual void textFont();
    virtual void textSplit();
    virtual void togglePixmap();
    virtual void integralLower();
    virtual void integralHigher();
    virtual void bracketType();
    virtual void matrixSet();
    virtual void matrixRemRow();
    virtual void matrixRemCol();
    virtual void matrixInsRow();
    virtual void matrixInsCol();
    virtual void fractionHAlign();
    virtual void toggleMidline();
    virtual void symbolType();
    virtual void DecorationType();
    virtual void remove();
    virtual void elementColor();
    virtual void generalColor();
    virtual void generalFont();
    virtual void delimiterLeft(const char*);
    virtual void delimiterRight(const char*);    
 protected slots:
    void insertIndex(int i);
};

#endif
