// has to be included first
#include <qprinter.h>

#include <qpainter.h>

#include <kiconloader.h>
#include <kdebug.h>

#include "TextElement.h"
#include "matrixwidget.h"
#include "BracketElement.h"
#include "RootElement.h"
#include "TextElement.h"
#include "FractionElement.h"
#include "MatrixElement.h"
#include "PrefixedElement.h"

//#include <kglobal.h>

/**********************************************************
 *
 * KFormulaView
 *
 **********************************************************/
#include "kformula_view.h"
#include "kformula_factory.h"
#include "kformula_doc.h"

#include <qpainter.h>
#include <kaction.h>
#include <kstdaction.h>
#include <qpopupmenu.h>

KFormulaView::KFormulaView( KFormulaDoc* _doc, QWidget* _parent, const char* _name )
    : KoView( _doc, _parent, _name )
{
//    setWidget( this );
//    widget()->setFocusPolicy( QWidget::StrongFocus );

//    setBackgroundColor( white );

    m_pDoc = _doc;
    setInstance( KFormulaFactory::global());
    setXMLFile( "kformula.rc" );

    mn_indexList = new QPopupMenu();
    mn_indexList->insertItem(BarIcon("index0"),0);
    mn_indexList->insertSeparator();
    mn_indexList->insertItem(BarIcon("index1"),1);
    mn_indexList->insertSeparator();
    mn_indexList->insertItem(BarIcon("index2"),2);
    mn_indexList->insertSeparator();
    mn_indexList->insertItem(BarIcon("index3"),3);
    mn_indexList->setMouseTracking(true);
    mn_indexList->setCheckable(false);


    actionEditUndo = KStdAction::undo( this, SLOT( undo() ), actionCollection(), "edit_undo" );
    actionEditUndo->setText( i18n( "No Undo possible" ) );
    actionEditRedo = KStdAction::redo( this, SLOT( redo() ), actionCollection(), "edit_redo" );
    actionEditRedo->setText( i18n( "No Redo possible" ) );

    actionElement_AddElement_T = new KAction(i18n( "Add/change to simple text" ),
					"mini-xy",
					 CTRL + Key_1 ,
					this,SLOT(addText()),
					actionCollection(),"addtext");

    actionElement_AddElement_R = new KAction(i18n( "Add/change to root" ),
					"mini-root",
					 CTRL + Key_2 ,
					this,SLOT(addRoot()),
					actionCollection(),"addroot");

    actionElement_AddElement_F = new KAction(i18n( "Add/change to fraction" ),
					"mini-frac",
					 CTRL + Key_3 ,
					this,SLOT(addFraction()),
					actionCollection(),"addfrac");

    actionElement_AddElement_V = new KAction(i18n( "Add/change to vertical space" ),
					"mini-vspace",
					 CTRL + Key_4 ,
					this,SLOT(addVertSpace()),
					actionCollection(),"addvspace");

    actionElement_AddElement_B = new KAction(i18n( "Add/change to bracket" ),
					"mini-bra",
					 CTRL + Key_5 ,
					this,SLOT(addBracket()),
					actionCollection(),"addbra");

    actionElement_AddElement_I = new KAction(i18n( "Add/change to integral" ),
					"mini-integral",
					 CTRL + Key_6 ,
					this,SLOT(addIntegral()),
					actionCollection(),"addintegral");

    actionElement_AddElement_S = new KAction(i18n( "Add/change to symbol" ),
					"mini-symbol",
					 CTRL + Key_7 ,
					this,SLOT(addSymbol()),
					actionCollection(),"addsymbol");

    actionElement_AddElement_M = new KAction(i18n( "Add/change to matrix" ),
					"matrix",
					 CTRL + Key_8 ,
					this,SLOT(addMatrix()),
					actionCollection(),"addmatrix");

    actionElement_AddElement_L = new KAction(i18n( "Add index at pos..." ),
					"index",
					 CTRL + Key_9 ,
					this,SLOT(indexList()),
					actionCollection(),"addindex");

     actionElement_Text_Font = new KFontAction(i18n( "Font family" ),0,
					actionCollection(),"textfont");
     connect( actionElement_Text_Font, SIGNAL( activated( const QString& ) ), this, SLOT( fontSelected( const QString& ) ) );

     actionElement_Text_Size = new KFontSizeAction(i18n( "Size" ),0,
					actionCollection(),"textsize");

     connect( actionElement_Text_Size, SIGNAL( fontSizeChanged( int ) ), this, SLOT( sizeSelected( int ) ) );

     actionElement_Text_Bold = new KToggleAction(i18n( "Bold" ),
					"bold",
					 CTRL + Key_B ,
					actionCollection(),"textbold");
     connect( actionElement_Text_Bold, SIGNAL( toggled( bool ) ), this, SLOT( bold( bool ) ) );

    actionElement_Text_Italic = new KToggleAction(i18n( "Italic" ),
					"italic",
					 CTRL + Key_I ,
					actionCollection(),"textitalic");
    connect( actionElement_Text_Italic, SIGNAL( toggled( bool ) ), this, SLOT( italic( bool ) ) );

    actionElement_Text_Under = new KToggleAction(i18n( "Underlined" ),
					"underl",
					 CTRL + Key_U ,
					actionCollection(),"textunder");
    connect(actionElement_Text_Under, SIGNAL( toggled( bool ) ), this, SLOT( underline( bool ) ) );

    actionElement_Font_Element = new KToggleAction(i18n( "Resize element" ),
					"elementsw",
					 CTRL + ALT + Key_E ,
					this,SLOT(fontSwitch()),
					actionCollection(),"fontelement");

    //actionElement_Font_Element->setEnabled(false);

    actionElement_Font_Child = new KToggleAction(i18n( "Resize children" ),
					"childrensw",
					 CTRL + ALT + Key_C ,
					this,SLOT(fontSwitch()),
					actionCollection(),"fontchild");

    actionElement_Font_Index = new KToggleAction(i18n( "Resize index" ),
					"indexsw",
					 CTRL + ALT + Key_I ,
					this,SLOT(fontSwitch()),
					actionCollection(),"fontindex");

    actionElement_Font_Next = new KToggleAction(i18n( "Resize next" ),
					"nextsw",
					 CTRL + ALT + Key_N ,
					this,SLOT(fontSwitch()),
					actionCollection(),"fontnext");

    // ************************* Text *********************

    actionElement_Text_Split = new KAction(i18n( "Split text" ),
					"split",
					 CTRL + Key_0 ,
					this,SLOT(textSplit()),
					actionCollection(),"textsplit");

    // ************************** Root ********************

    actionElement_Root_Index = new KAction(i18n( "Root index" ),
					"rootindex",
					 CTRL + Key_0 ,
					this,SLOT(addTopLeftIndex()),
					actionCollection(),"rootindex");


    QStringList delimiter;
    delimiter.clear();
    delimiter.append(QString("("));
    delimiter.append(QString("["));
    delimiter.append(QString("<"));
    delimiter.append(QString("/"));
    delimiter.append(QString("\\"));
    delimiter.append(QString(")"));
    delimiter.append(QString("]"));
    delimiter.append(QString(">"));
    delimiter.append(QString("|"));
    //toolTip = Q2C( i18n("Left delimiter") );
    actionElement_Bracket_Type_Left = new KSelectAction(i18n("Left delimiter"),
					   0,this, SLOT(delimiterLeft()),
					   actionCollection(),"typeleft");
    ((KSelectAction *)actionElement_Bracket_Type_Left)->setItems(delimiter);

    delimiter.clear();
    delimiter.append(QString(")"));
    delimiter.append(QString("]"));
    delimiter.append(QString(">"));
    delimiter.append(QString("/"));
    delimiter.append(QString("\\"));
    delimiter.append(QString("("));
    delimiter.append(QString("["));
    delimiter.append(QString("<"));
    delimiter.append(QString("|"));
    actionElement_Bracket_Type_Right = new KSelectAction(i18n("Right delimiter"),
					   0,this, SLOT(delimiterRight()),
					   actionCollection(),"typeright");
    ((KSelectAction *)actionElement_Bracket_Type_Right)->setItems(delimiter);



    // ************************** Fraction ********************

    actionElement_Fraction_VA_M = new KToggleAction(i18n( "Middle align" ),
					"midalign",
					 0 ,
					this,SLOT(fractionAlignM()),
					actionCollection(),"fracmiddle");

    //((KToggleAction *)actionElement_Fraction_VA_M)->setExclusiveGroup("FractionVa");

    actionElement_Fraction_VA_U = new KToggleAction(i18n( "Numerator align" ),
					"upalign",
					 0 ,
					this,SLOT(fractionAlignU()),
					actionCollection(),"fracup");

  // ((KToggleAction *)actionElement_Fraction_VA_U)->setExclusiveGroup("FractionVa");

    actionElement_Fraction_VA_D = new KToggleAction(i18n( "Denominator align" ),
					"downalign",
					 0 ,
					this,SLOT(fractionAlignD()),
					actionCollection(),"fracdown");

  // ((KToggleAction *)actionElement_Fraction_VA_D)->setExclusiveGroup("FractionVa");

    actionElement_Fraction_HA_C = new KToggleAction(i18n( "Center align" ),
					"centralign",
					 0 ,
					this,SLOT(fractionAlignC()),
					actionCollection(),"fraccenter");

  // ((KToggleAction *)actionElement_Fraction_HA_C)->setExclusiveGroup("FractionHa");

    actionElement_Fraction_HA_L = new KToggleAction(i18n( "Left align" ),
					"leftalign",
					 0 ,
					this,SLOT(fractionAlignL()),
					actionCollection(),"fracleft");

    // ((KToggleAction *)actionElement_Fraction_HA_L)->setExclusiveGroup("FractionHa");
    
    actionElement_Fraction_HA_R = new KToggleAction(i18n( "Right align" ),
					"rightalign",
					 0 ,
					this,SLOT(fractionAlignR()),
					actionCollection(),"fracright");

    
    // ((KToggleAction *)actionElement_Fraction_HA_R)->setExclusiveGroup("FractionHa");

    actionElement_Fraction_MidLine = new KToggleAction(i18n( "Draw midline" ),
					"midline",
					 0 ,
					this,SLOT(toggleMidline()),
					actionCollection(),"fracmidline");

    actionElement_Fraction_Far = new KAction(i18n( "More distant" ),
					"far",
					 0 ,
					this,SLOT(fractionDistMore()),
					actionCollection(),"fracfar");

    actionElement_Fraction_Near = new KAction(i18n( "Less distant" ),
					"near",
					 0 ,
					this,SLOT(fractionDistLess()),
					actionCollection(),"fracnear");


    actionElement_Integral_Lower = new KAction(i18n( "Low limit" ),
					"Ilower",
					 0 ,
					this,SLOT(integralLower()),
					actionCollection(),"ilower");

    actionElement_Integral_Higher = new KAction(i18n( "Hi limit" ),
					"Ihigher",
					 0 ,
					this,SLOT(integralHigher()),
					actionCollection(),"ihigher");

    actionElement_Matrix_Set = new KAction(i18n( "Matrix setup" ),
					"matrix",
					 0 ,
					this,SLOT(matrixSet()),
					actionCollection(),"matrixset");

    actionElement_Matrix_Ins_Row = new KAction(i18n( "Insert row" ),
					"insrow",
					 0 ,
					this,SLOT(matrixInsRow()),
					actionCollection(),"matrixinsrow");
    actionElement_Matrix_Ins_Col = new KAction(i18n( "Insert col" ),
					"inscol",
					 0 ,
					this,SLOT(matrixInsCol()),
					actionCollection(),"matrixinscol");

    actionElement_Matrix_Rem_Row = new KAction(i18n( "Remove row" ),
					"remrow",
					 0 ,
					this,SLOT(matrixRemRow()),
					actionCollection(),"matrixremrow");
    actionElement_Matrix_Rem_Col = new KAction(i18n( "Remove col" ),
					"remcol",
					 0 ,
					this,SLOT(matrixRemCol()),
					actionCollection(),"matrixremcol");

    m_pDoc->setModified( true );
    QObject::connect(mn_indexList,SIGNAL(activated(int)),this,SLOT(insertIndex(int)));

    QObject::connect( m_pDoc, SIGNAL( sig_modified() ),
		      this, SLOT( slotModified() ) );

    QObject::connect( m_pDoc,
		      SIGNAL(sig_changeType(const BasicElement *)    ),
		      this, SLOT( slotTypeChanged(const BasicElement *)    ) );
}




void KFormulaView::init()
{
}

KFormulaView::~KFormulaView()
{
    cleanUp();
}

void KFormulaView::cleanUp()
{
}

void KFormulaView::updateReadWrite( bool readwrite )
{
#ifdef __GNUC__
#warning TODO
#endif
  QValueList<KAction*> actions = actionCollection()->actions();
  QValueList<KAction*>::ConstIterator aIt = actions.begin();
  QValueList<KAction*>::ConstIterator aEnd = actions.end();
  for (; aIt != aEnd; ++aIt )
    (*aIt)->setEnabled( readwrite );
}


void KFormulaView::paintEvent( QPaintEvent *_ev )
{
    m_pDoc->paintEvent(_ev, this);
/* void KFormulaView::paintEvent( QPaintEvent* ev )
{
    QPainter painter;
    painter.begin( this );

    // ### TODO: Scaling

    // Let the document do the drawing
    part()->paintEverything( painter, ev->rect(), FALSE, this );

    painter.end();
}

*/
}

void KFormulaView::mousePressEvent( QMouseEvent *_ev )
{
    m_pDoc->mousePressEvent(_ev,this);
}

void KFormulaView::resizeEvent( QResizeEvent * )
{
}

void KFormulaView::keyPressEvent( QKeyEvent *k )
{
    kdDebug(39001)<< "Key pressed, ascii "<< k->ascii() << endl;
    m_pDoc->keyPressEvent(k);
}



void KFormulaView::slotTypeChanged( const BasicElement *elm)
{

//    actionElement_Font_Element->setEnabled(false);

    bool isText, isBracket, isFraction, isPrefixed, isMatrix, isRoot;
    if (elm)
      {
	const type_info& type = typeid(*elm);
	isText = type == typeid(TextElement);
	isBracket = type == typeid(BracketElement);
	isFraction = type == typeid(FractionElement);
	isPrefixed = type == typeid(PrefixedElement);
	isMatrix = type == typeid(MatrixElement);
	isRoot = type == typeid(RootElement);
      }
    else
      {
	isRoot = isMatrix = isPrefixed =isFraction = isBracket = isText = false;
      }

    actionElement_Text_Split->setEnabled(isText);
    // It remains deactivated !!!
    //    m_rMenuBar->setItemEnabled(m_idMenuElement_Fraction,isFraction);
    if(elm->getIndex(0)==0) //in Root element you can't have more one element
        (actionElement_Root_Index)->setEnabled(isRoot);
    //kdDebug(39001) <<"Toggle\n";
    (actionElement_Bracket_Type_Left)->setEnabled(isBracket);
    (actionElement_Bracket_Type_Right)->setEnabled(isBracket);

    (actionElement_Fraction_VA_U)->setEnabled(isFraction);
    (actionElement_Fraction_VA_D)->setEnabled(isFraction);
    (actionElement_Fraction_VA_M)->setEnabled(isFraction);
    (actionElement_Fraction_HA_C)->setEnabled(isFraction);
    (actionElement_Fraction_HA_L)->setEnabled(isFraction);
    (actionElement_Fraction_HA_R)->setEnabled(isFraction);

    //kdDebug(39001) <<"End\n";
    (actionElement_Fraction_Near)->setEnabled(isFraction);
    (actionElement_Fraction_Far)->setEnabled(isFraction);
    (actionElement_Fraction_MidLine)->setEnabled(isFraction);

    (actionElement_Integral_Lower)->setEnabled(isPrefixed);
    (actionElement_Integral_Higher)->setEnabled(isPrefixed);
    (actionElement_Matrix_Set)->setEnabled(isMatrix);
    (actionElement_Matrix_Ins_Row)->setEnabled(isMatrix);
    (actionElement_Matrix_Ins_Col)->setEnabled(isMatrix);
    (actionElement_Matrix_Rem_Row)->setEnabled(isMatrix);
    (actionElement_Matrix_Rem_Col)->setEnabled(isMatrix);

    if (elm)
        {
	  ((KFontSizeAction *)actionElement_Text_Size)->setFontSize(elm->getNumericFont());
	  kdDebug(39001) <<"Size font :"<<elm->getNumericFont()<<endl;
	  BasicElement *el=m_pDoc->currentElement();
	  TextElement *te = dynamic_cast<TextElement*>(el);
	  if (te==0)
	    {
	      ((KToggleAction*)actionElement_Text_Bold)->setChecked(false);
	      ((KToggleAction*)actionElement_Text_Italic)->setChecked(false);
	      ((KToggleAction*)actionElement_Text_Under)->setChecked(false);
	    }
	  else
	    {
	      ((KToggleAction*)actionElement_Text_Bold)->setChecked(te->isBold());
	      ((KToggleAction*)actionElement_Text_Italic)->setChecked(te->isItalic());
	      ((KToggleAction*)actionElement_Text_Under)->setChecked(te->isUnderline());
	      ((KFontAction*)actionElement_Text_Font)->setFont(te->textFontFamily());
	    }
        }

      if (isFraction)
      {
	QString content=elm->getContent();
	((KToggleAction *)actionElement_Fraction_HA_C)->setChecked(content[2]=='C');
	((KToggleAction *)actionElement_Fraction_HA_L)->setChecked(content[2]=='L');
	((KToggleAction *)actionElement_Fraction_HA_R)->setChecked(content[2]=='R');
	((KToggleAction *)actionElement_Fraction_VA_M)->setChecked(content[1]=='M');
	((KToggleAction *)actionElement_Fraction_VA_U)->setChecked(content[1]=='U');
	((KToggleAction *)actionElement_Fraction_VA_D)->setChecked(content[1]=='D');
	((KToggleAction *)actionElement_Fraction_MidLine)->setChecked(content[0]=='F');
      }
    else if(isMatrix)
      {
	QString content=elm->getContent();
	((KToggleAction *)actionElement_Fraction_HA_C)->setChecked(content[2]=='C');
	((KToggleAction *)actionElement_Fraction_HA_L)->setChecked(content[2]=='L');
	((KToggleAction *)actionElement_Fraction_HA_R)->setChecked(content[2]=='R');
      }
    else
      {
	((KToggleAction *)actionElement_Fraction_HA_C)->setChecked(false);
	((KToggleAction *)actionElement_Fraction_HA_L)->setChecked(false);
	((KToggleAction *)actionElement_Fraction_HA_R)->setChecked(false);
        ((KToggleAction *)actionElement_Fraction_VA_M)->setChecked(false);
	((KToggleAction *)actionElement_Fraction_VA_U)->setChecked(false);
	((KToggleAction *)actionElement_Fraction_VA_D)->setChecked(false);
	((KToggleAction *)actionElement_Fraction_MidLine)->setChecked(false);
      }

//    actionElement_Font_Element->setEnabled(false);

}


void KFormulaView::slotModified()
{
    update();
}

void KFormulaView::undo()
{
    kdDebug(39001)<<"No Undo possible"<<endl;
}

void KFormulaView::redo()
{
    kdDebug(39001)<<"No Redo possible"<<endl;
}

void KFormulaView::addText()
{
    kdDebug(39001)<<"adding Text"<<endl;
    m_pDoc->addTextElement();
    //    m_vToolBarType->show();
}

void KFormulaView::addRoot()
{
    kdDebug(39001)<<"adding Root"<<endl;
    m_pDoc->addRootElement();
}

void KFormulaView::addFraction()
{
    kdDebug(39001)<<"adding Fraction"<<endl;
    m_pDoc->addFractionElement(DEFAULT_FRACTION);
}

void KFormulaView::addVertSpace()
{
    kdDebug(39001)<<"adding VerticalSpace"<<endl;
    m_pDoc->addFractionElement(DEFAULT_VSPACE);
}

void KFormulaView::addBracket()
{
    kdDebug(39001)<<"adding parentheses"<<endl;
    m_pDoc->addBracketElement(DEFAULT_DELIMITER);
    //    m_vToolBarType->hide();
}

void KFormulaView::addMatrix()
{
    kdDebug(39001)<<"adding Matrix"<<endl;
    MatrixSetupWidget *ms=new MatrixSetupWidget();
    ms->setString("MCC002002001006NNNNNN");
    QObject::connect(ms,SIGNAL(returnString(QString)),this,SLOT(createMatrix(QString)));
    ms->show();

}

void KFormulaView::createMatrix(QString str)
{
    if(str!="")
	m_pDoc->addMatrixElement(str);
    update();
}

void KFormulaView::modifyMatrix(QString str)
{
    int x,y,old;
    BasicElement *el=m_pDoc->currentElement();
    if (el==0)
	return;
    MatrixElement *elm = dynamic_cast<MatrixElement*>(el);
    if (elm==0)
	return;

    QString oldc=elm->getContent();
    x=oldc.mid(3,3).toInt();
    y=oldc.mid(6,3).toInt();
    old=x*y;
    x=str.mid(3,3).toInt();
    y=str.mid(6,3).toInt();
    elm->setChildrenNumber(x*y);
    for(int i=old;i<x*y;i++)
	elm->setChild(new BasicElement(m_pDoc,elm,i+4),i);
    for(int i=x*y;i<old;i++)
	delete    elm->getChild(i); //code to remove unused children...


    elm->setContent(str);
    update();
}

void KFormulaView::addIntegral()
{
    kdDebug(39001)<<"adding Integral"<<endl;
    m_pDoc->addPrefixedElement("IS000");
}

void KFormulaView::addSymbol()
{
    kdDebug(39001)<<"##adding Symbol"<<endl;
    m_pDoc->addPrefixedElement("IF06" );
}

void KFormulaView::reduce()
{
    if(m_pDoc->currentElement()==0) 
      return;
    int level;
    level= FN_REDUCE | FN_P43;
    if(((KToggleAction*)actionElement_Font_Element)->isChecked()) 
      level=level | FN_ELEMENT;
    if(((KToggleAction*)actionElement_Font_Index)->isChecked())  
      level=level | FN_INDEXES;
    if(((KToggleAction*)actionElement_Font_Child)->isChecked()) 
      level=level | FN_CHILDREN;
    if(((KToggleAction*)actionElement_Font_Next)->isChecked())  
      level=level | FN_NEXT;

    m_pDoc->currentElement()->scaleNumericFont(level);

    update();
}

void KFormulaView::enlarge()
{
    if(m_pDoc->currentElement()==0) 
      return;
    int level;
    level= FN_ENLARGE | FN_P43;

    //kdDebug(39001) <<m_vToolBarFont->comboItem(m_idComboFont_ScaleMode)<<endl;
    if(((KToggleAction*)actionElement_Font_Element)->isChecked()) 
      level=level | FN_ELEMENT;
    
    if(((KToggleAction*)actionElement_Font_Index)->isChecked())  
      level=level | FN_INDEXES;
    
    if(((KToggleAction*)actionElement_Font_Child)->isChecked()) 
      level=level | FN_CHILDREN;
    
    if(((KToggleAction*)actionElement_Font_Next)->isChecked())  
      level=level | FN_NEXT;

    m_pDoc->currentElement()->scaleNumericFont(level);
    update();
}


void KFormulaView::fractionAlignM()
{
  if(!((KToggleAction*)actionElement_Fraction_VA_M)->isChecked())
      if( (((KToggleAction*)actionElement_Fraction_VA_D)->isChecked())
	|| (((KToggleAction*)actionElement_Fraction_VA_U)->isChecked())
      ) return;

    kdDebug(39001) <<"M\n";
    QString content=m_pDoc->currentElement()->getContent();
    content[1]='M';
    m_pDoc->currentElement()->setContent(content);
    
    ((KToggleAction *)actionElement_Fraction_VA_M)->setChecked(true);
    ((KToggleAction *)actionElement_Fraction_VA_D)->setChecked(false);
    ((KToggleAction *)actionElement_Fraction_VA_U)->setChecked(false);
    update();
}
void KFormulaView::fractionAlignU()
{
    if(!((KToggleAction*)actionElement_Fraction_VA_U)->isChecked())
    if( (((KToggleAction*)actionElement_Fraction_VA_D)->isChecked())
      || (((KToggleAction*)actionElement_Fraction_VA_M)->isChecked())
      ) return;

    kdDebug(39001) <<"U\n";
    QString content=m_pDoc->currentElement()->getContent();
    content[1]='U';
    m_pDoc->currentElement()->setContent(content);
    ((KToggleAction *)actionElement_Fraction_VA_U)->setChecked(true);
    ((KToggleAction *)actionElement_Fraction_VA_D)->setChecked(false);
    ((KToggleAction *)actionElement_Fraction_VA_M)->setChecked(false);
    
    update();
}

void KFormulaView::fractionAlignD()
{
    if(!((KToggleAction*)actionElement_Fraction_VA_D)->isChecked())
    if( (((KToggleAction*)actionElement_Fraction_VA_M)->isChecked())
      || (((KToggleAction*)actionElement_Fraction_VA_U)->isChecked())
      ) return;

    kdDebug(39001) <<"D\n";
    QString content=m_pDoc->currentElement()->getContent();
    content[1]='D';
    m_pDoc->currentElement()->setContent(content);
    ((KToggleAction *)actionElement_Fraction_VA_D)->setChecked(true);
    ((KToggleAction *)actionElement_Fraction_VA_M)->setChecked(false);
    ((KToggleAction *)actionElement_Fraction_VA_U)->setChecked(false);
    update();
}
void KFormulaView::fractionAlignL()
{

    QString content=m_pDoc->currentElement()->getContent();
    content[2]='L';
    m_pDoc->currentElement()->setContent(content);
    ((KToggleAction *)actionElement_Fraction_HA_L)->setChecked(true);
    ((KToggleAction *)actionElement_Fraction_HA_C)->setChecked(false);
    ((KToggleAction *)actionElement_Fraction_HA_R)->setChecked(false);
    update();
}
void KFormulaView::fractionAlignR()
{

    QString content=m_pDoc->currentElement()->getContent();
    content[2]='R';
    m_pDoc->currentElement()->setContent(content);
    ((KToggleAction *)actionElement_Fraction_HA_L)->setChecked(false);
    ((KToggleAction *)actionElement_Fraction_HA_C)->setChecked(false);
    ((KToggleAction *)actionElement_Fraction_HA_R)->setChecked(true);
    update();
}
void KFormulaView::fractionAlignC()
{

    QString content=m_pDoc->currentElement()->getContent();
    content[2]='C';
    m_pDoc->currentElement()->setContent(content);
    ((KToggleAction *)actionElement_Fraction_HA_L)->setChecked(false);
    ((KToggleAction *)actionElement_Fraction_HA_C)->setChecked(true);
    ((KToggleAction *)actionElement_Fraction_HA_R)->setChecked(false);
    update();
}

void KFormulaView::fractionDist()
{

}

void KFormulaView::fractionDistMore()
{
    QString content=m_pDoc->currentElement()->getContent();
 
    QString tmpContent=content;
    content=tmpContent.left(3);
    content+=QString::number(tmpContent.right(tmpContent.length()-3).toInt()+1);
    
    kdDebug(39001) <<content<<endl;
    m_pDoc->currentElement()->setContent(content);
    update();
}
void KFormulaView::fractionDistLess()
{
    QString content=m_pDoc->currentElement()->getContent();
    int space=content.right(content.length()-3).toInt()-1;
    if (space<1)
        space =1;
   
    content=content.left(3);
    content+=QString::number(space);

    kdDebug(39001) <<content<<endl;
    m_pDoc->currentElement()->setContent(content);
    update();
}

void KFormulaView::fontSwitch()
{
//   if(m_vToolBarFont->isButtonOn(m_idButtonFont_2)) warning("element");
//    if(m_vToolBarFont->isButtonOn(m_idButtonFont_3)) warning("index");
//    if(m_vToolBarFont->isButtonOn(m_idButtonFont_4)) warning("children");
//    if(m_vToolBarFont->isButtonOn(m_idButtonFont_5)) warning("next");
}

void KFormulaView::bold(bool b)
{
    BasicElement *el=m_pDoc->currentElement();
    if (el==0)
	return;

    TextElement *te = dynamic_cast<TextElement*>(el);
    if (te==0)
	return;
    te->changePropertieFont(b);
    kdDebug(39001) <<"bold : "<<b<<endl;
    update();
}

void KFormulaView::italic(bool b)
{

    BasicElement *el=m_pDoc->currentElement();
    if (el==0)
	return;

    TextElement *te = dynamic_cast<TextElement*>(el);
    if (te==0)
	return;
    te->changePropertieFont(-1,-1,b);
    kdDebug(39001) <<"italic : "<<b<<endl;
    update();
}

void KFormulaView::underline(bool b)
{

   BasicElement *el=m_pDoc->currentElement();
    if (el==0)
	return;

    TextElement *te = dynamic_cast<TextElement*>(el);
    if (te==0)
	return;
    te->changePropertieFont(-1,b);
    kdDebug(39001) <<"underline : "<<b<<endl;
    update();
}


void KFormulaView::addTopLeftIndex()
{
     //kdDebug(39001)<<"add TopLeft index[0]\n";
    m_pDoc->addIndex(IN_TOPLEFT);

}

void KFormulaView::addBottomLeftIndex()
{
    //kdDebug(39001)<<"add BottomLeft index[1]\n";
    m_pDoc->addIndex(IN_BOTTOMLEFT);
}

void KFormulaView::addTopRightIndex()
{
    //kdDebug(39001)<<"add Top Right index[2]\n";
    m_pDoc->addIndex(IN_TOPRIGHT);
}

void KFormulaView::addBottomRightIndex()
{
    //kdDebug(39001)<<"add Top Right index[3]\n";
    m_pDoc->addIndex(IN_BOTTOMRIGHT);
}

void KFormulaView::indexList()
{
    //kdDebug(39001)<<"index\n";
    QPoint pnt(QCursor::pos());
    mn_indexList->popup(pnt);
}
void KFormulaView::insertIndex(int i)
{
    //kdDebug(39001)<<"index "<<i<<endl;
    m_pDoc->addIndex(i);
}

void KFormulaView::sizeSelected(int size)
{
    BasicElement *el=m_pDoc->currentElement();
    if (el==0)
        return;
    el->setNumericFont(size);
    kdDebug(39001) <<"size selected :"<<size<<endl;
    update();
}

void KFormulaView::fontSelected( const QString& font)
{

    BasicElement *el=m_pDoc->currentElement();
    if (el==0)
	return;

    TextElement *te = dynamic_cast<TextElement*>(el);

    if (te==0)
	return;
/*    if (font == "(default)")
	font="";*/
    te->changeFontFamily(font);
    kdDebug(39001) <<font<<endl;
    update();
}

void KFormulaView::modeSelected(char *)
{
    kdDebug(39001) <<"mode\n";
}
void KFormulaView::textFont()
{
    kdDebug(39001) <<"Slot textFont\n";
}
void KFormulaView::textSplit()
{
    kdDebug(39001) <<"Slot textSplit\n";

    BasicElement *el=m_pDoc->currentElement();

    if (el==0)
	return;

    TextElement *te = dynamic_cast<TextElement*>(el);

    if (te==0)
	return;

    te->split(-1);
    update();

}
void KFormulaView::togglePixmap()
{
    kdDebug(39001) <<"Slot togglePixmap\n";
}
void KFormulaView::integralLower()
{
    kdDebug(39001) <<"Slot integralLower\n";
    m_pDoc->addChild(2);
}
void KFormulaView::integralHigher()
{
    kdDebug(39001) <<"Slot integralHigher\n";
    m_pDoc->addChild(1);
}
void KFormulaView::bracketType()
{
    kdDebug(39001) <<"Slot bracketType\n";
}
void KFormulaView::matrixSet()
{
    MatrixSetupWidget *ms=new MatrixSetupWidget();
    ms->setString(m_pDoc->currentElement()->getContent());
    QObject::connect(ms,SIGNAL(returnString(QString)),this,SLOT(modifyMatrix(QString)));
    ms->show();

}
void KFormulaView::matrixRemRow()
{
    kdDebug(39001) <<"Slot matrixRemRow\n";
}
void KFormulaView::matrixRemCol()
{
    kdDebug(39001) <<"Slot matrixRemCol\n";
}
void KFormulaView::matrixInsRow()
{
    kdDebug(39001) <<"Slot matrixInsRow\n";
}
void KFormulaView::matrixInsCol()
{
    kdDebug(39001) <<"Slot matrixInsCol\n";
}
void KFormulaView::fractionHAlign()
{
    kdDebug(39001) <<"Slot fractionHAlign\n";
}
void KFormulaView::toggleMidline()
{
    kdDebug(39001) <<"Slot toggleMidline\n";
    QString content=m_pDoc->currentElement()->getContent();
    if (((KToggleAction *)actionElement_Fraction_MidLine)->isChecked())
     content[0]='F';
    else content[0]='V';
    m_pDoc->currentElement()->setContent(content);
    update();
}
void KFormulaView::symbolType()
{
    kdDebug(39001) <<"Slot symbolType\n";
}
void KFormulaView::DecorationType()
{
    kdDebug(39001) <<"Slot decorationType\n";
}
void KFormulaView::remove()
{
    kdDebug(39001) <<"Slot remove\n";
}
void KFormulaView::elementColor()
{
    kdDebug(39001) <<"Slot elementColor\n";
}
void KFormulaView::generalColor()
{
    kdDebug(39001) <<"Slot generalColor\n";
}
void KFormulaView::generalFont()
{
    kdDebug(39001) <<"Slot generalFont\n";
}
void KFormulaView::delimiterLeft()
{
    QString left =  ((KSelectAction *)actionElement_Bracket_Type_Left)->currentText();
    QString content=m_pDoc->currentElement()->getContent();
    kdDebug(39001) <<content<<endl;
    content[0]=QString(left)[0];
    kdDebug(39001) <<content<<endl;
    m_pDoc->currentElement()->setContent(content);
    update();
}

void KFormulaView::delimiterRight()
{
    QString right=  ((KSelectAction *)actionElement_Bracket_Type_Right)->currentText();
    QString content=m_pDoc->currentElement()->getContent();
    kdDebug(39001) <<content<<endl;
    content[1]=QString(right)[0];
    kdDebug(39001) <<content<<endl;
    m_pDoc->currentElement()->setContent(content);
    update();
}

bool KFormulaView::printDlg()
{

    QPrinter thePrt;
    //  thePrt.setMinMax(1,1);
    if (thePrt.setup(this))
	{
	    if(m_pDoc!=0L)
		m_pDoc->print(&thePrt);
	}
    return true;
}

#include "kformula_view.moc"
