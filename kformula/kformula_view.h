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

class QAction;
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
    virtual void updateReadWrite( bool readwrite ) { };
    KFormulaDoc *m_pDoc;

    QPopupMenu *mn_indexList;


   //Actions!!
   
    QAction *actionEditUndo;
    QAction *m_idButtonFormula_0;
    QAction *actionView_NewView;
    QAction *actionView_FontToolbar;
    QAction *actionView_TypeToolbar;
    QAction *actionView_TextToolbar;

    QAction *actionElement_AddIndex_TL;
    QAction *actionElement_AddIndex_BL;
    QAction *actionElement_AddIndex_TR;
    QAction *actionElement_AddIndex_BR;
    QAction *actionElement_AddElement_T;  // text
    QAction *actionElement_AddElement_R;  // root
    QAction *actionElement_AddElement_F;  // fraction
    QAction *actionElement_AddElement_V;  // vertSpace
    QAction *actionElement_AddElement_B;  // bracket
    QAction *actionElement_AddElement_I;  // integral
    QAction *actionElement_AddElement_D;  // decoration
    QAction *actionElement_AddElement_S;  // symbol
    QAction *actionElement_AddElement_M;  // matrix
    QAction *actionElement_AddElement_L;  // index list

    QAction *actionElement_Text_Font;
    QAction *actionElement_Text_Size;
    QAction *actionElement_Text_Bold;
    QAction *actionElement_Text_Italic;
    QAction *actionElement_Text_Under;
    QAction *actionElement_Font_Child;
    QAction *actionElement_Font_Next;
    QAction *actionElement_Font_Element;
    QAction *actionElement_Font_Index;
    
    QAction *actionElement_Text_Split;
    QAction *actionElement_Root_Pixmap;
    QAction *actionElement_Root_Index;
    QAction *actionElement_Integral_Pixmap;
    QAction *actionElement_Integral_Lower;
    QAction *actionElement_Integral_Higher;
    QAction *actionElement_Bracket_Type_Left;
    QAction *actionElement_Bracket_Type_Right;
    QAction *actionElement_Matrix_Set;
    QAction *actionElement_Matrix_Ins_Row;
    QAction *actionElement_Matrix_Ins_Col;
    QAction *actionElement_Matrix_Rem_Row;
    QAction *actionElement_Matrix_Rem_Col;
    QAction *actionElement_Fraction_VA;  //Vert Allign
    QAction *actionElement_Fraction_VA_U;
    QAction *actionElement_Fraction_VA_D;
    QAction *actionElement_Fraction_VA_M;
    QAction *actionElement_Fraction_HA;  //Horiz Allign
    QAction *actionElement_Fraction_HA_C;
    QAction *actionElement_Fraction_HA_L;
    QAction *actionElement_Fraction_HA_R;
    QAction *actionElement_Fraction_Near;
    QAction *actionElement_Fraction_Far;
    QAction *actionElement_Fraction_Dist;
    QAction *actionElement_Fraction_MidLine;
    QAction *actionElement_Decoration_Set;
    QAction *actionElement_Symbol_Set;
    QAction *actionElement_Color;
    QAction *actionElement_Remove;
    QAction *actionFormula_Color;
    QAction *actionFormula_Font;
//    QAction *actionHelp_Using;

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
