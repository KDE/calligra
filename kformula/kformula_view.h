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

class BasicElement;

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
    void slotTypeChanged(  BasicElement *elm);

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
    QPopupMenu *mn_indexList;
    OPParts::MenuBarFactory_var m_vMenuBarFactory;
    MenuBar_ref m_rMenuBar;
    CORBA::Long m_idMenuView;
    CORBA::Long m_idMenuView_NewView;
    CORBA::Long m_idMenuView_FontToolbar;
    CORBA::Long m_idMenuView_TypeToolbar;
    CORBA::Long m_idMenuView_TextToolbar;
    
    CORBA::Long m_idMenuElement;
    CORBA::Long m_idMenuElement_AddIndex;    
    CORBA::Long m_idMenuElement_AddIndex_TL;
    CORBA::Long m_idMenuElement_AddIndex_BL;
    CORBA::Long m_idMenuElement_AddIndex_TR;
    CORBA::Long m_idMenuElement_AddIndex_BR;

    CORBA::Long m_idMenuElement_AddElement;    
    CORBA::Long m_idMenuElement_AddElement_T;  // text    
    CORBA::Long m_idMenuElement_AddElement_R;  // root
    CORBA::Long m_idMenuElement_AddElement_F;  // fraction 
    CORBA::Long m_idMenuElement_AddElement_V;  // vertSpace
    CORBA::Long m_idMenuElement_AddElement_B;  // bracket
    CORBA::Long m_idMenuElement_AddElement_I;  // integral
    CORBA::Long m_idMenuElement_AddElement_D;  // decoration
    CORBA::Long m_idMenuElement_AddElement_S;  // symbol
    CORBA::Long m_idMenuElement_AddElement_M;  // matrix
    CORBA::Long m_idMenuElement_Text;
    CORBA::Long m_idMenuElement_Text_Font;
    CORBA::Long m_idMenuElement_Text_Split;
    CORBA::Long m_idMenuElement_Root;
    CORBA::Long m_idMenuElement_Root_Pixmap;
    CORBA::Long m_idMenuElement_Root_Index;
    CORBA::Long m_idMenuElement_Integral;
    CORBA::Long m_idMenuElement_Integral_Pixmap;
    CORBA::Long m_idMenuElement_Integral_Lower;
    CORBA::Long m_idMenuElement_Integral_Higher;
    CORBA::Long m_idMenuElement_Bracket;
    CORBA::Long m_idMenuElement_Bracket_Type;
    CORBA::Long m_idMenuElement_Matrix;
    CORBA::Long m_idMenuElement_Matrix_Set;
    CORBA::Long m_idMenuElement_Matrix_InsRow;
    CORBA::Long m_idMenuElement_Matrix_InsCol;
    CORBA::Long m_idMenuElement_Matrix_RemRow;
    CORBA::Long m_idMenuElement_Matrix_RemCol;
    CORBA::Long m_idMenuElement_Fraction;
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
    CORBA::Long m_idMenuElement_Decoration;
    CORBA::Long m_idMenuElement_Decoration_Set;
    CORBA::Long m_idMenuElement_Symbol;        
    CORBA::Long m_idMenuElement_Symbol_Set;
    CORBA::Long m_idMenuElement_Color;
    CORBA::Long m_idMenuElement_Remove;    
    CORBA::Long m_idMenuFormula;
    CORBA::Long m_idMenuFormula_Color;    
    CORBA::Long m_idMenuFormula_Font;    

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
