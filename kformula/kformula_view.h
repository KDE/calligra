#ifndef __kformula_view_h__
#define __kformula_view_h__

class KFormulaView;
class KFormulaDoc;
/*
#include <koFrame.h>
#include <opMenu.h>
#include <opToolBar.h>
#include <koFrame.h>
#include <openparts_ui.h>
*/
#include <koView.h>

#include "kformula.h"
#include <kaction.h>
#include <qwidget.h>
#include <qlist.h>
#include <qkeycode.h>

class BasicElement;



//#include <container.h>

class KAction;
class QPaintEvent;


class KFormulaView : public KoView
{
    Q_OBJECT
//public:
//    KFormulaView( KFormulaDoc* part, QWidget* parent = 0, const char* name = 0 );

 public:
    // C++
    KFormulaView( KFormulaDoc * _doc,QWidget *_parent=0, const char *_name=0);
    virtual ~KFormulaView();

    // IDL
//    virtual void newView();
    virtual bool printDlg();
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
//    virtual bool event( const QCString & _event, const CORBA::Any& _value );
    // C++
//    virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
//    virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );

    virtual void cleanUp();

    /*long addToolButton( OpenPartsUI::ToolBar_ptr toolbar,
			       const char* pictname,
			       const QString &tooltip,
			       const char* func,
			       CORBA::Long id );
*/
    void mousePressEvent(QMouseEvent *a);
    void paintEvent( QPaintEvent *_ev );
    void resizeEvent( QResizeEvent *_ev );
    void keyPressEvent( QKeyEvent *k );
    virtual void updateReadWrite( bool /*readwrite*/ ) { };
    KFormulaDoc *m_pDoc;

    QPopupMenu *mn_indexList;


   //Actions!!

    KAction *actionEditUndo;
    KAction *m_idButtonFormula_0;
    KAction *actionView_NewView;
    KAction *actionView_FontToolbar;
    KAction *actionView_TypeToolbar;
    KAction *actionView_TextToolbar;

    KAction *actionElement_AddIndex_TL;
    KAction *actionElement_AddIndex_BL;
    KAction *actionElement_AddIndex_TR;
    KAction *actionElement_AddIndex_BR;
    KAction *actionElement_AddElement_T;  // text
    KAction *actionElement_AddElement_R;  // root
    KAction *actionElement_AddElement_F;  // fraction
    KAction *actionElement_AddElement_V;  // vertSpace
    KAction *actionElement_AddElement_B;  // bracket
    KAction *actionElement_AddElement_I;  // integral
    KAction *actionElement_AddElement_D;  // decoration
    KAction *actionElement_AddElement_S;  // symbol
    KAction *actionElement_AddElement_M;  // matrix
    KAction *actionElement_AddElement_L;  // index list

    KAction *actionElement_Text_Font;
    KAction *actionElement_Text_Size;
    KAction *actionElement_Text_Bold;
    KAction *actionElement_Text_Italic;
    KAction *actionElement_Text_Under;
    KAction *actionElement_Font_Child;
    KAction *actionElement_Font_Next;
    KAction *actionElement_Font_Element;
    KAction *actionElement_Font_Index;

    KAction *actionElement_Text_Split;
    KAction *actionElement_Root_Pixmap;
    KAction *actionElement_Root_Index;
    KAction *actionElement_Integral_Pixmap;
    KAction *actionElement_Integral_Lower;
    KAction *actionElement_Integral_Higher;
    KAction *actionElement_Bracket_Type_Left;
    KAction *actionElement_Bracket_Type_Right;
    KAction *actionElement_Matrix_Set;
    KAction *actionElement_Matrix_Ins_Row;
    KAction *actionElement_Matrix_Ins_Col;
    KAction *actionElement_Matrix_Rem_Row;
    KAction *actionElement_Matrix_Rem_Col;
    KAction *actionElement_Fraction_VA;  //Vert Allign
    KAction *actionElement_Fraction_VA_U;
    KAction *actionElement_Fraction_VA_D;
    KAction *actionElement_Fraction_VA_M;
    KAction *actionElement_Fraction_HA;  //Horiz Allign
    KAction *actionElement_Fraction_HA_C;
    KAction *actionElement_Fraction_HA_L;
    KAction *actionElement_Fraction_HA_R;
    KAction *actionElement_Fraction_Near;
    KAction *actionElement_Fraction_Far;
    KAction *actionElement_Fraction_Dist;
    KAction *actionElement_Fraction_MidLine;
    KAction *actionElement_Decoration_Set;
    KAction *actionElement_Symbol_Set;
    KAction *actionElement_Color;
    KAction *actionElement_Remove;
    KAction *actionFormula_Color;
    KAction *actionFormula_Font;
//    KAction *actionHelp_Using;

/*    CORBA::Long m_idButtonFormula_0;
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
*/
public slots:
     void addText();
     void addRoot();
     void addFraction();
     void addVertSpace();
     void addBracket();
     void addMatrix();
     void addIntegral();
     void addSymbol();
     void reduce();
     void enlarge();
     void fractionAlignU();
     void fractionAlignD();
     void fractionAlignM();
     void fractionAlignL();
     void fractionAlignR();
     void fractionAlignC();
     void fractionDist();
     void fractionDistMore();
     void fractionDistLess();
     void fontSwitch();
     void addTopLeftIndex();
     void addBottomLeftIndex();
     void addTopRightIndex();
     void addBottomRightIndex();
     void indexList();
     void sizeSelected();
     void fontSelected();
     void modeSelected(char*);
     void textFont();
     void textSplit();
     void togglePixmap();
     void integralLower();
     void integralHigher();
     void bracketType();
     void matrixSet();
     void matrixRemRow();
     void matrixRemCol();
     void matrixInsRow();
     void matrixInsCol();
     void fractionHAlign();
     void toggleMidline();
     void symbolType();
     void DecorationType();
     void remove();
     void elementColor();
     void generalColor();
     void generalFont();
     void delimiterLeft();
     void delimiterRight();
 protected slots:
    void insertIndex(int i);
};

#endif
