
/**
 * KFormula - Formula Editor
 *
 * This code is free. Do with it what you want.
 *
 * (c) KDE Project
 * Andrea Rizzi
 * bilibao@ouverture.it
 */

 #include <klocale.h>
#include <qbitmap.h>
  #include <qcolor.h>
#define red QColor(255,0,0)        // Problems in koffice1 with QColor
#define blue QColor(0,0,255)
#define black QColor(0,0,0)
#define white QColor(255,255,255)
 #define FIXEDSPACE 1             //Space between 2 blocks
#define klocale klo->translate     
#define Inherited QDialog
#include "widget.h"
#include <kiconloader.h>
#include <kmsgbox.h>
#include <qkeycode.h>
#include <qbttngrp.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbt.h>
/*
Proprieties Dialog
*/

ProData::ProData
(       FormulaBlock* edbl,
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, TRUE )
{
	EdBl=edbl;
	TextLE = new KLined( this, "LineEdit_1" );
	TextLE->setGeometry( 110, 10, 120, 30 );
	TextLE->setText(EdBl->getcont());
	cb[0] = new QRadioButton( this, "RadioButton_1" );
	cb[0]->setGeometry( 250, 25, 120, 30 );
	cb[0]->setText( "Simple Text" );
	cb[1] = new QRadioButton( this, "RadioButton_2" );
	cb[1]->setGeometry( 250, 50, 120, 30 );
	cb[1]->setText( "Root" );
	cb[2] = new QRadioButton( this, "RadioButton_3" );
	cb[2]->setGeometry( 250, 75, 120, 30 );
	cb[2]->setText( "Integral" );
	cb[3] = new QRadioButton( this, "RadioButton_4" );
	cb[3]->setGeometry( 250, 100, 120, 30 );
	cb[3]->setText( "Bracket" );
	cb[4] = new QRadioButton( this, "RadioButton_5" );
	cb[4]->setGeometry( 250, 125, 120, 30 );
	cb[4]->setText( "Fraction" );
	cb[5] = new QRadioButton( this, "RadioButton_5" );
	cb[5]->setGeometry( 250, 150, 120, 30 );
	cb[5]->setText( "Symbols" );
        if(EdBl->gettype()!=-1) cb[EdBl->gettype()]->setChecked(TRUE); 
	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( this, "Label_1" );
	tmpQLabel->setGeometry( 10, 10, 90, 30 );
	tmpQLabel->setText( "Text" );
	tmpQLabel->setAlignment( 289 );
	tmpQLabel->setMargin( -1 );
	fcb[1] = new QCheckBox( this, "CheckBox_2" );
	fcb[1]->setGeometry( 30, 210, 100, 30 );
	connect( fcb[1], SIGNAL(toggled(bool)), SLOT(getValue1(bool)) );
	fcb[1]->setText( "Length" );
	fcb[1]->setAutoRepeat( FALSE );
	fcb[1]->setAutoResize( FALSE );
	fcb[3] = new QCheckBox( this, "CheckBox_3" );
	fcb[3]->setGeometry( 30, 240, 100, 30 );
	connect( fcb[3], SIGNAL(toggled(bool)), SLOT(getValue3(bool)) );
	fcb[3]->setText( "Vert-Space" );
	fcb[3]->setAutoRepeat( FALSE );
	fcb[3]->setAutoResize( FALSE );
	fcb[2] = new QCheckBox( this, "CheckBox_4" );
	fcb[2]->setGeometry( 200, 210, 90, 30 );
	connect( fcb[2], SIGNAL(toggled(bool)), SLOT(getValue2(bool)) );
	fcb[2]->setText( "Height" );
	fcb[2]->setAutoRepeat( FALSE );
	fcb[2]->setAutoResize( FALSE );
	fcb[4] = new QCheckBox( this, "CheckBox_5" );
	fcb[4]->setGeometry( 200, 240, 90, 30 );
	connect( fcb[4], SIGNAL(toggled(bool)), SLOT(getValue4(bool)) );
	fcb[4]->setText( "Mid-point" );
	fcb[4]->setAutoRepeat( FALSE );
	fcb[4]->setAutoResize( FALSE );
	QPushButton* tmpQPushButton;
	tmpQPushButton = new QPushButton( this, "PushButton_1" );
	tmpQPushButton->setGeometry( 130, 130, 100, 30 );
	connect( tmpQPushButton, SIGNAL(clicked()), SLOT(selectFont()) );
	tmpQPushButton->setText( "Select Font" );
	tmpQPushButton->setAutoRepeat( FALSE );
	tmpQPushButton->setAutoResize( FALSE );
	tmpQPushButton = new QPushButton( this, "PushButton_2" );
	tmpQPushButton->setGeometry( 20, 290, 100, 30 );
	connect( tmpQPushButton, SIGNAL(clicked()), SLOT(OkPressed()) );
	tmpQPushButton->setText( "OK" );
	tmpQPushButton->setAutoRepeat( FALSE );
	tmpQPushButton->setAutoResize( FALSE );
	tmpQPushButton->setAutoDefault( TRUE );
	tmpQPushButton = new QPushButton( this, "PushButton_3" );
	tmpQPushButton->setGeometry( 270, 290, 100, 30 );
	connect( tmpQPushButton, SIGNAL(clicked()), SLOT(CancelPressed()) );
	tmpQPushButton->setText( "Cancel" );
	tmpQPushButton->setAutoRepeat( FALSE );
	tmpQPushButton->setAutoResize( FALSE );
	spb[1] = new KNumericSpinBox( this, "LineEdit_3" );
	spb[1]->setGeometry( 120, 210, 60, 30 );
	spb[3] = new KNumericSpinBox( this, "LineEdit_4" );
	spb[3]->setGeometry( 120, 240, 60, 30 );
	spb[2] = new KNumericSpinBox( this, "LineEdit_5" );
	spb[2]->setGeometry( 290, 210, 60, 30 );
	spb[4] = new KNumericSpinBox( this, "LineEdit_6" );
	spb[4]->setGeometry( 290, 240, 60, 30 );
	QFrame* tmpQFrame;
	tmpQFrame = new QFrame( this, "Frame_2" );
	tmpQFrame->setGeometry( 10, 50, 220, 10 );
	tmpQFrame->setFrameStyle( 52 );
	tmpQLabel = new QLabel( this, "Label_5" );
	tmpQLabel->setGeometry( 70, 60, 100, 20 );
	tmpQLabel->setText( "Font:" );
	tmpQLabel->setAlignment( 292 );
	tmpQLabel->setMargin( -1 );
	tmpQLabel = new QLabel( this, "Label_7" );
	tmpQLabel->setGeometry( 10, 80, 100, 30 );
	tmpQLabel->setText( "Family:" );
	tmpQLabel->setAlignment( 289 );
	tmpQLabel->setMargin( -1 );
	tmpQLabel = new QLabel( this, "Label_8" );
	tmpQLabel->setGeometry( 10, 110, 100, 30 );
	tmpQLabel->setText( "Size:" );
	tmpQLabel->setAlignment( 289 );
	tmpQLabel->setMargin( -1 );
	tmpQLabel = new QLabel( this, "Label_9" );
	tmpQLabel->setGeometry( 10, 140, 100, 30 );
	tmpQLabel->setText( "Weight:" );
	tmpQLabel->setAlignment( 289 );
	tmpQLabel->setMargin( -1 );
	QButtonGroup* tmpQButtonGroup;
	tmpQButtonGroup = new QButtonGroup( this, "ButtonGroup_2" );
	tmpQButtonGroup->setGeometry( 10, 190, 370, 90 );
	tmpQButtonGroup->setFrameStyle( 49 );
	tmpQButtonGroup->setTitle( "Fixed Values" );
	tmpQButtonGroup->setAlignment( 1 );
	tmpQButtonGroup->insert( fcb[1] );
	tmpQButtonGroup->insert( fcb[3] );
	tmpQButtonGroup->insert( fcb[2] );
	tmpQButtonGroup->insert( fcb[4] );
	tmpQButtonGroup->lower();
	tmpQButtonGroup = new QButtonGroup( this, "ButtonGroup_1" );
	tmpQButtonGroup->setGeometry( 240, 10, 140, 180 );
	tmpQButtonGroup->setFrameStyle( 49 );
	tmpQButtonGroup->setTitle( "Type:" );
	tmpQButtonGroup->setAlignment( 1 );
	tmpQButtonGroup->insert( cb[0] );
	tmpQButtonGroup->insert( cb[1] );
	tmpQButtonGroup->insert( cb[2] );
	tmpQButtonGroup->insert( cb[3] );
	tmpQButtonGroup->insert( cb[4] );
	tmpQButtonGroup->insert( cb[5] );
	tmpQButtonGroup->lower();
	tmpQButtonGroup->setExclusive( TRUE );

	resize( 400, 340 );
show();
	
}


ProData::~ProData()
{
}


void ProData::getValue1(bool)
{
}
void ProData::getValue3(bool)
{
}
void ProData::getValue2(bool)
{
}
void ProData::getValue4(bool)
{
}

void ProData::selectFont()
{
KFontDialog *fontd = new KFontDialog(this);
fontd->setFont(EdBl->fontc);
fontd->getFont(EdBl->fontc);
}
void ProData::OkPressed()
{
int i;
EdBl->setcont(TextLE->text());
for (i=0;i<=5;i++) if(cb[i]->isChecked()) EdBl->settype(i);
close();
}
void ProData::CancelPressed()
{
close();
delete this;
}


#include <qmsgbox.h>


Kformulaw::Kformulaw( FormulaEditor *_part ) : QWidget( _part )
{   
    klo = new KLocale("kde");
    /************************************************************************
     *
     * Create your GUI here, but NO menubars. These are constructed in
     * KPart::showGUI
     *
     ************************************************************************/

    // At the beginning, we have not scrolled.
    xOffset = 0;
    yOffset = 0;
    
    // We assume that we are not active =>
    // We dont show scrollbars & rulers and stuff
    bShowGUI = FALSE;
    
    // Toolbar
        QString filename = getenv( "KDEDIR" );
    if ( !filename.isNull() )
	filename += "/share/";
    else
    {
	QMessageBox::message( "KDE ERROR", "KDEDIR not set" );
	exit(1);
    }

    
    toolbar1 = _part->newToolBar( "Main" );
    QString t;
    toolbar1->setFullWidth(FALSE);
    t = filename.data();
    t += "toolbar/fileopen.xpm";
    QPixmap pixmap;
    pixmap.load( t.data() );
    int pb = toolbar1->insertButton( pixmap, 1, SIGNAL( clicked() ), _part, SLOT( slotOpen() ), TRUE, "Open");
    t = filename.data();
    t += "toolbar/filefloppy.xpm";
    pixmap.load( t.data() );
    pb = toolbar1->insertButton( pixmap, 2, SIGNAL( clicked() ), _part, SLOT( slotSave() ), TRUE, "Save");
    t = filename.data();
    t += "toolbar/fileprint.xpm";
    pixmap.load( t.data() );
    pb = toolbar1->insertButton( pixmap, 6, SIGNAL( clicked() ), _part, SLOT( slotPrint() ), TRUE, "Print");

    filename = getenv( "KDEDIR" );
    if ( !filename.isNull() )
	filename += "/share/apps/";
    else
    {
	QMessageBox::message( "KDE ERROR", "KDEDIR not set" );
	exit(1);
    }

    /***********************************************************************
     *
     * Put code for your toolbars here.
     *
     ***********************************************************************/
    
    // Vert. Scroll Bar
    vertScrollBar = new QScrollBar( this, "ScrollBar_2" );
    connect( vertScrollBar, SIGNAL( valueChanged(int) ), this, SLOT( slotScrollV(int) ) );
    vertScrollBar->setRange( 0, 1024 );

    // Horz. Scroll Bar
    horzScrollBar = new QScrollBar( this, "ScrollBar_1" );
    connect( horzScrollBar, SIGNAL( valueChanged(int) ), this, SLOT( slotScrollH(int) ) );
    horzScrollBar->setRange( 0, 1024 );
    horzScrollBar->setOrientation( QScrollBar::Horizontal );

// The widget on which we display our stuff
   paperWidget = new FormulaDisplay( this );
    connect(paperWidget,SIGNAL(ChText(const char *)),this,SLOT(chtext(const char*)));
    connect(paperWidget,SIGNAL(ChType(int)),this,SLOT(chtype(int)));
   
   QPixmap pm;
   QString dir= filename.data();// KApplication::getKApplication()->kdedir()+ QString("/share/toolbar/");
    dir +="kformula/pics/"; 

  //  statusbar = new KStatusBar(this);
  //  statusbar->insertItem(klocale("KOffice - Formula Editor - by Andrea Rizzi"),1);
  //  statusbar->insertItem(klocale("Block Type "),2);
    

toolbar = _part->newToolBar( "Formula" );    
     toolbar->setFullWidth(FALSE);
     pm.load(dir+"mini-xy.xpm");
    toolbar->insertButton(pm,1,SIGNAL(clicked()),paperWidget ,SLOT(addB0()),1,
klocale("Add/change to simple text"),-1);	
    pm.load(dir+"mini-root.xpm");
    toolbar->insertButton(pm,2,SIGNAL(clicked()),paperWidget ,SLOT(addB1()),1,
klocale("Add/change to Root"),-1);	
    pm.load(dir+"mini-frac.xpm");
    toolbar->insertButton(pm,3,SIGNAL(clicked()),paperWidget ,SLOT(addB4()),1,
klocale("Add/change fract-line"),-1);	
    pm.load(dir+"mini-vspace.xpm");
    toolbar->insertButton(pm,4,SIGNAL(clicked()),paperWidget ,SLOT(addB4bis()),1,
klocale("Add/change Vertical Space"),-1);	
    pm.load(dir+"mini-bra.xpm");
    toolbar->insertButton(pm,5,SIGNAL(clicked()),paperWidget ,SLOT(addB3()),1,
klocale("Add/change a Bracket Block"),-1);	
    pm.load(dir+"mini-integral.xpm");
    toolbar->insertButton(pm,6,SIGNAL(clicked()),paperWidget ,SLOT(addB2()),1,
klocale("Add/change an integral"),-1);	
    pm.load(dir+"mini-symbols.xpm");
    toolbar->insertButton(pm,7,SIGNAL(clicked()),paperWidget ,SLOT(addB5()),1,
klocale("Add/change a block with symbols"),-1);	

    
    fontbar =  _part->newToolBar( "Font" );    ;
    fontbar->setFullWidth(FALSE);
    pm.load(dir+"reduce.xpm");
    fontbar->insertButton(pm,1,SIGNAL(clicked()),paperWidget ,SLOT(reduce()),1,
klocale("Reduce the font of active block"),-1);	

    pm.load(dir+"enlarge.xpm");
    fontbar->insertButton(pm,2,SIGNAL(clicked()),paperWidget ,SLOT(enlarge()),1,
klocale("Enlarge the font of active block"),-1);	

    pm.load(dir+"reduceall.xpm");
    fontbar->insertButton(pm,3,SIGNAL(clicked()),paperWidget ,SLOT(reduceRecur()),1,
		klocale("Reduce the font of active block & children"),-1);	
    pm.load(dir+"enlargeall.xpm");
    fontbar->insertButton(pm,4,SIGNAL(clicked()),paperWidget ,SLOT(enlargeRecur()),1,
		klocale("Enlarge the font of active block & children"),-1);			
    pm.load(dir+"enlargenext.xpm");
    fontbar->insertButton(pm,5,SIGNAL(clicked()),paperWidget ,SLOT(enlargeAll()),1,
		klocale("Enlarge the font of active block, children & next blocks"),-1);	
    pm.load(dir+"greek.xpm");
    fontbar->insertButton(pm,6,SIGNAL(toggled(bool)),paperWidget ,SLOT(setGreek(bool)),1,
		klocale("Sets greek font"),-1);	
    fontbar->setToggle(6);
		
    thistypebar = _part->newToolBar( "thisType" );    
     pm.load(dir+"kformula1-0.xpm");
     thistypebar->insertButton(pm,1,SIGNAL(clicked()),paperWidget ,SLOT(addCh1()),0,
     klocale("Add Exponent"),-1);	
     pm.load(dir+"kformula2-0.xpm");
     thistypebar->insertButton(pm,2,SIGNAL(clicked()),paperWidget ,SLOT(addCh2()),0,
     klocale("Add index"),-1);	
     pm.load(dir+"kformula2-1.xpm");
     thistypebar->insertButton(pm,3,SIGNAL(clicked()),paperWidget ,SLOT(addCh2()),0,
     klocale("Add root index"),-1);	
     pm.load(dir+"kformula2-2.xpm");
     thistypebar->insertButton(pm,4,SIGNAL(clicked()),paperWidget ,SLOT(addCh2()),0,
     klocale("Add high limit"),-1);	
     pm.load(dir+"kformula3-2.xpm");
     thistypebar->insertButton(pm,5,SIGNAL(clicked()),paperWidget ,SLOT(addCh3()),0,
     klocale("Add low limit"),-1);	
     pm.load(dir+"kformula2-3.xpm");
     thistypebar->insertButton(pm,6,SIGNAL(clicked()),paperWidget ,SLOT(addCh2()),0,
     klocale("Add exponent"),-1);	
     pm.load(dir+"frac-vline.xpm");
     thistypebar->insertFrame(7,90);
     lined = new KLined(thistypebar->getFrame(7),"Cont lined"); 
       lined->resize(90,24);
       lined->setText(dir);
      lined->show();
      connect(lined,SIGNAL(textChanged(const char *)),paperWidget,SLOT(change(const char *)));
        paperWidget->setBackgroundColor( white );
    paperWidget->update();
    paperWidget->setFocusPolicy( QWidget::StrongFocus );

}
void Kformulaw::chtext(const char * text)
{
QString str;
lined->setText(text);
str.sprintf("%s",paperWidget->Blocks[paperWidget->getCurrent()]->fontc.family());
fontbar->setButton(4,str.contains("Symbol",FALSE)); 
}

void Kformulaw::chtype(int type)
{   
int enType[7];
int i;
enType[0]=0;
enType[1]=0;
enType[2]=1;
enType[3]=2;
enType[4]=2;
enType[5]=3;
for(i=1;i<=6;i++)
    thistypebar->setItemEnabled(i,(enType[i-1]==type));
     update();
}

void Kformulaw::resizeEvent( QResizeEvent * )
{
    /***********************************************************************
     *
     * Put code that manages the geometry of additional rulers 
     * and stuff here.
     *
     ***********************************************************************/

    if ( bShowGUI )
    {
	// We display the whole GUI
	horzScrollBar->setGeometry( 0, height() - 20, width() - 20, 20 );
	vertScrollBar->setGeometry( width() - 20, 0 , 20, height() - 20 );
	paperWidget->setGeometry( 0, 0, width() - 20, height() - 20 );
    }
    else
	// We display the content only, no GUI.
	paperWidget->setGeometry( 0, 0, width(), height() );
}

void Kformulaw::showGUI( bool _status )
{
    /**************************************************************************
     *
     * Here is the place to hide/show additional toplevel windows.
     * For example non modal dialogs and stuff like that.
     * The code below hides/shows the scrollbars.
     *
     *************************************************************************/

    if ( _status == bShowGUI )
	return;
    
    bShowGUI = _status;
    resizeEvent( 0L );
}

void Kformulaw::slotScrollH( int _value )
{
//    paperWidget->scroll( xOffset - _value, 0 );
    xOffset = _value;
    paperWidget->xOff =  -xOffset;
    paperWidget->repaint();
}

void Kformulaw::slotScrollV( int _value )
{
//    paperWidget->scroll( 0, yOffset - _value );
    yOffset = _value;
    paperWidget->yOff =  -yOffset;
    paperWidget->repaint();
}

Kformulaw::~Kformulaw()
{
}

FormulaDisplay::FormulaDisplay( QWidget *parent=0, const char *name=0 )
 : QWidget(parent,name)
{
    klo = new KLocale("kde");
 painter = new QPainter(this);
 for(firstfree=0;firstfree<2000;firstfree++)
 isFree[firstfree]=TRUE;
  // Blocks[firstfree]=NULL;
 isFree[0]=FALSE;
 firstfree=1;
 current=0;
 pos=0;
 name= "KDE Formula Display";    
 cursorTimer= new QTimer(); 
 connect(cursorTimer,SIGNAL(timeout()),this,SLOT(toggleCursor()));
 cursorTimer->start(500); 
 Blocks[0] = new FormulaBlock(this,-1,0,-1,-1,"");
}

void FormulaDisplay::keyPressEvent( QKeyEvent *k )
{ 

 int c,prev,next,c1,c2,c3,len,type;
 c=getCurrent(); 
 prev=Blocks[c]->getprev();
 next=Blocks[c]->getnext();
 c1=Blocks[c]->getchild1();
 c2=Blocks[c]->getchild2();
 c3=Blocks[c]->getchild3();
 type=Blocks[c]->gettype();
 len=Blocks[c]->getcont().length();
 if(k->key()==Key_Left) {
		       if((type==0)&&(pos>0)) pos--;
		    else  
                       if(prev!=-1)setCurrent(prev);
				     }
 if(k->key()==Key_Right) { 
                        if((type==0)&&(pos<len)) pos++;
			else
			 if(next!=-1)  setCurrent(next);
			else
			 if(c1!=-1)    setCurrent(c1);
			else
			 if(c2!=-1)    setCurrent(c2);
			  }			
if(k->key()==Key_Up) {
		      if(c1!=-1)    setCurrent(c1);                        
	   	    else
		      if(prev!=-1)  setCurrent(prev);						 
    		     }  

if(k->key()==Key_Down) {
		      if(c2!=-1)    setCurrent(c2);                        
	   	    else
		      if(prev!=-1)  setCurrent(prev);						 
    		     }  

if(k->key()==Key_Backspace) {
	                 if(pos>0) { pos--;
		                     Blocks[c]->getcont().remove(pos,1);
				   }
			    }
 else 
 if(k->key()==Key_Delete) {
    if(type==0) Blocks[c]->getcont().remove(pos,1); else deleteIt(Blocks[c]);  
    }    else 
 
 if((k->ascii()>32)&&(k->ascii()<127))
  { 
c1=c;
c2=c1;
if(Blocks[c1]->gettype()>0) {    
    next=Blocks[c1]->getnext();
    c2=addBlock(0,-1,next,c1,"");
    Blocks[c1]->setnext(c2); }
       else Blocks[c1]->settype(0);
    setCurrent(c2);	   
    Blocks[getCurrent()]->getcont().insert(pos,k->ascii());
    pos++; 
  } 
ChText(Blocks[getCurrent()]->getcont());
ChType(Blocks[getCurrent()]->gettype());
update();
}

void FormulaDisplay::paintEvent( QPaintEvent * )
{

    Blocks[0]->getDimensions(); // Get dimension of Block0 (all formula dimension)
    painter->setPen( black );   
    Blocks[0]->PaintIt(painter,1+xOff,1+yOff+Blocks[0]->getLYU(),1);
    isBlack=FALSE;
}

int FormulaDisplay::getPos()
{
 return pos;
}

void FormulaDisplay::setCurrent(int cur)
{
  if(current!=cur) pos=Blocks[cur]->getcont().length();   
  Blocks[current]->setactive(0);
  Blocks[cur]->setactive(1); 
  current=cur; 
  ChText(Blocks[cur]->getcont());  //Signals
  ChType(Blocks[cur]->gettype());
}

int FormulaDisplay::getCurrent()
{
  return (current);
}

void FormulaDisplay::mousePressEvent( QMouseEvent *a)
{
   setCurrent(Blocks[0]->getClick(a->pos()));
   repaint();
if(a->button()==RightButton){
   QPopupMenu *mousepopup = new QPopupMenu;
   QPopupMenu *convert = new QPopupMenu;
   QPopupMenu *fontpopup = new QPopupMenu;    
       fontpopup->insertItem(klocale("Font +"), this, SLOT(enlarge()), ALT+Key_K);
    fontpopup->insertItem(klocale("Font -"), this, SLOT(reduce()), ALT+Key_K);
    fontpopup->insertItem(klocale("Font + (also Children)"), this, SLOT(enlargeRecur()), ALT+Key_K);
    fontpopup->insertItem(klocale("Font - (also Children)"), this, SLOT(reduceRecur()), ALT+Key_K);
    fontpopup->insertItem(klocale("All Font +"), this, SLOT(enlargeAll()), ALT+Key_K);
    fontpopup->insertItem(klocale("All Font -"), this, SLOT(reduceAll()), ALT+Key_K);
//    convert->insertItem(klocale("Simple Text"), parent->_part, SLOT(slotQuit()), ALT+Key_E);
//    convert->insertItem("AAAARGGHHH", _part, SLOT(slotQuit()), ALT+Key_E);
    mousepopup->insertItem(klocale("Proprieties"), this,SLOT(pro()), ALT+Key_E);
    mousepopup->insertItem(klocale("Delete"), this,SLOT(dele()), ALT+Key_E);
    mousepopup->insertSeparator();
    mousepopup->insertItem(klocale("Change font.."),fontpopup);    
    mousepopup->insertItem(klocale("Convert to.."),convert);
//    mousepopup->insertSeparator();
//    mousepopup->insertItem(klocale("Quit"), _part, SLOT(slotQuit()), ALT+Key_Q);
    mousepopup->popup(mapToGlobal(a->pos()));
 }
}
void FormulaDisplay::setCursor(QRect re)
{
cursorPos = re;
}


/*
			    FormulaDisplay SLOTS

*/

void FormulaDisplay::setGreek(bool on)
{
if (on) 
    Blocks[getCurrent()]->fontc.setFamily("Symbol");	
 else 
    Blocks[getCurrent()]->fontc.setFamily("utopia");
 repaint();
}
void FormulaDisplay::change(const char * newcont)
{
if (Blocks[getCurrent()]->gettype()==-1) Blocks[getCurrent()]->settype(0);
Blocks[getCurrent()]->setcont(newcont);
repaint();
}

void FormulaDisplay::toggleCursor()
{   
drawCursor();
}

void FormulaDisplay::drawCursor()
{   
//QPainter paint;
showIt=hasFocus();
if((showIt)||(isBlack&&(!showIt))) {
//painter->begin();
painter->drawWinFocusRect(cursorPos);
//painter->end(); 
isBlack=!isBlack;
}
}

void FormulaDisplay::reduce()
{   
   Blocks[getCurrent()]->reduceFont(0);
   repaint();
}
void FormulaDisplay::reduceRecur()
{   
   Blocks[getCurrent()]->reduceFont(1);
   repaint();
}
void FormulaDisplay::reduceAll()
{   
   Blocks[getCurrent()]->reduceFont(2);
   repaint();
}

void FormulaDisplay::enlarge()
{   
   Blocks[getCurrent()]->enlargeFont(0);
   repaint();
}
void FormulaDisplay::enlargeRecur()
{   
   Blocks[getCurrent()]->enlargeFont(1);
   repaint();
}
void FormulaDisplay::enlargeAll()
{   
   Blocks[getCurrent()]->enlargeFont(2);
   repaint();
}

void FormulaDisplay::addCh3()
{   
    int nextold=-1; 
    int Id;
    nextold=Blocks[getCurrent()]->getchild3();
    Id=addBlock(-1,-1,nextold,getCurrent(),"");
    Blocks[getCurrent()]->setchild3(Id);
    if(Blocks[getCurrent()]->gettype()==2) Blocks[Id]->reduceFont(1);    
    setCurrent(Id);	
   repaint();
}

void FormulaDisplay::line(int i)
{   
    if(i==1) Blocks[getCurrent()]->setcont("line");
    else        
    Blocks[getCurrent()]->setcont("vertical space");
   repaint();
}

void FormulaDisplay::addCh2()
{   
    int nextold=-1; 
    int Id;
    nextold=Blocks[getCurrent()]->getchild2();
    Id=addBlock(-1,-1,nextold,getCurrent(),"");
    Blocks[getCurrent()]->setchild2(Id); 
    if(Blocks[getCurrent()]->gettype()<4) Blocks[Id]->reduceFont(1);    
    setCurrent(Id);	
   repaint();
}
void FormulaDisplay::addCh1()
{   
    int nextold=-1; 
    int Id;
    nextold=Blocks[getCurrent()]->getchild1();
    Id=addBlock(-1,-1,nextold,getCurrent(),"");
    Blocks[getCurrent()]->setchild1(Id); 	
    if(Blocks[getCurrent()]->gettype()==0) Blocks[Id]->reduceFont(1);    
    setCurrent(Id);
   repaint();
}

void FormulaDisplay::pro()
{   
 new ProData(Blocks[getCurrent()],this);  //proprieties
 checkAndCreate(Blocks[getCurrent()]);
 ChText(Blocks[getCurrent()]->getcont());
 ChType(Blocks[getCurrent()]->gettype());
 setFocus();
 update();          
}

void FormulaDisplay::dele()
{   
deleteIt(Blocks[getCurrent()]);
}

void FormulaDisplay::addB1()
{   
    int nextold=-1; 
    int Id,cur;
cur=getCurrent();
Id=cur;
if(Blocks[cur]->gettype()!=-1) {    nextold=Blocks[cur]->getnext();
    Id=addBlock(1,-1,nextold,cur,"c");
    Blocks[cur]->setnext(Id);  }
       else {
	     Blocks[cur]->settype(1);

	checkAndCreate(Blocks[cur]);
            }
    setCurrent(Blocks[Id]->getchild1());	
   repaint();
}
void FormulaDisplay::addB2()
{   
    int nextold=-1; 
    int Id,cur;
cur=getCurrent();
Id=cur;
if(Blocks[cur]->gettype()!=-1) {    nextold=Blocks[cur]->getnext();
    Id=addBlock(2,-1,nextold,cur,"c");
    Blocks[cur]->setnext(Id);  }
       else {
	     Blocks[cur]->settype(2);
	checkAndCreate(Blocks[cur]);
            }
    setCurrent(Blocks[Id]->getchild1());	
   repaint();
}

void FormulaDisplay::addB0()
{   
    int nextold=-1; 
    int Id,cur;
cur=getCurrent();
Id=cur;
    nextold=Blocks[cur]->getnext();
    Id=addBlock(-1,-1,nextold,cur,"");
    Blocks[cur]->setnext(Id); 

    setCurrent(Id);	
   repaint();
}
void FormulaDisplay::addB5()
{   
    int nextold=-1; 
    int Id,cur;
cur=getCurrent();
Id=cur;
if(Blocks[cur]->gettype()!=-1) { 
    nextold=Blocks[cur]->getnext();
    Id=addBlock(5,-1,nextold,cur,"");
    Blocks[cur]->setnext(Id); }
    else {    
     Blocks[cur]->settype(5);
     Blocks[cur]->setcont(">");
     checkAndCreate(Blocks[cur]);
     } 
    setCurrent(Blocks[Id]->getchild1());	
   repaint();
}

void FormulaDisplay::addB4()
{   
    int nextold=-1; 
    int Id,cur;
cur=getCurrent();
Id=cur;
if(Blocks[cur]->gettype()!=-1) {    nextold=Blocks[cur]->getnext();
    Id=addBlock(4,-1,nextold,cur,"line");
    Blocks[cur]->setnext(Id); }
       else {
	     Blocks[cur]->settype(4);
	       Blocks[cur]->setcont("line");
	checkAndCreate(Blocks[cur]);
	     }
    setCurrent(Blocks[Id]->getchild1());	
   repaint();
}
void FormulaDisplay::addB4bis()
{   
    int nextold=-1; 
    int Id,cur;
cur=getCurrent();
Id=cur;
if(Blocks[cur]->gettype()!=-1) {    nextold=Blocks[cur]->getnext();
    Id=addBlock(4,-1,nextold,cur,"vertical space");
    Blocks[cur]->setnext(Id);  }
       else {
	     Blocks[cur]->settype(4);
	       Blocks[cur]->setcont("vertical space");
	checkAndCreate(Blocks[cur]);    
	     }
    setCurrent(Blocks[Id]->getchild1());	
   repaint();
}

void FormulaDisplay::addB3()
{   
    int nextold=-1; 
    int Id,cur;
cur=getCurrent();
Id=cur;
if(Blocks[cur]->gettype()!=-1) {    nextold=Blocks[cur]->getnext();
    Id=addBlock(3,-1,nextold,cur,"()");
    Blocks[cur]->setnext(Id);  }
       else {
	     Blocks[cur]->settype(3);
	       Blocks[cur]->setcont("()");
	checkAndCreate(Blocks[cur]);   
	     }
    setCurrent(Blocks[Id]->getchild1());	
   repaint();
}


int FormulaDisplay::addBlock(int Type = -1,int ID = -1,int nextID = -1,int prevID = -1,
  QString Cont = "", int Child1 = -1,int Child2=-1, int Child3=-1)
{

  if (ID==-1) ID=firstfree;
  isFree[ID]=FALSE;  
  for(firstfree=1;!isFree[firstfree];firstfree++)
     ;
//QString s;  
//s.sprintf("First free block: %i",firstfree);    
//    warning(s);   
    
  Blocks[ID]= new FormulaBlock(this,Type,ID,nextID,prevID,Cont,Child1,Child2,Child3);
    checkAndCreate(Blocks[ID]);
      return (ID);
}

void FormulaDisplay::deleteIt(FormulaBlock *bl)
{
int id=bl->getID();
if(id!=0) {
 int next=bl->getnext();
 int prev=bl->getprev();
 int child1=bl->getchild1();
 int child2=bl->getchild2();
 int child3=bl->getchild3(); 
  if(next!=-1) Blocks[next]->setprev(prev);
  if(prev!=-1) {
		if(Blocks[prev]->getnext()==id) Blocks[prev]->setnext(next);
		if(Blocks[prev]->getchild1()==id) Blocks[prev]->setchild1(next);
		if(Blocks[prev]->getchild2()==id) Blocks[prev]->setchild2(next);
		if((Blocks[prev]->getchild3()==id)/*&&(Blocks[prev]->gettype()!=4)*/) Blocks[prev]->setchild3(next);
	     }
  if(child1!=-1) deleteIt(Blocks[child1]);
  if(child2!=-1) deleteIt(Blocks[child2]);
  if((child3!=-1)/*&&(bl->gettype()!=4)*/) deleteIt(Blocks[child3]);
  checkAndCreate(Blocks[prev]);
  if(prev!=-1) setCurrent(prev);
  isFree[id]=TRUE; 
  delete bl;
  update();
}
else KMsgBox::message(this,klocale("Error"),
klocale("Can not delete First Block !!"));

}
void FormulaDisplay::checkAndCreate(FormulaBlock *bl)
{
int ID;
ID=bl->getID();
switch (bl->gettype())
      {
      case 1:
       if (bl->getchild1()==-1) bl->setchild1(addBlock(-1,-1,-1,ID,""));
      break;
      case 2:
       if (bl->getchild1()==-1) bl->setchild1(addBlock(-1,-1,-1,ID,""));
      break;
      case 3:
      if(bl->getcont()=="") bl->setcont("()");      
      if (bl->getchild1()==-1)  bl->setchild1(addBlock(-1,-1,-1,ID,""));
      break;
      case 4:
      if (bl->getcont()=="") bl->setcont("line");
      if (bl->getchild1()==-1)  bl->setchild1(addBlock(-1,-1,-1,ID,""));
      if (bl->getchild2()==-1)  bl->setchild2(addBlock(-1,-1,-1,ID,"")); 
      if (bl->getsp()==-1)  bl->setsp(10);  
      break;
      case 5:
        if (bl->getcont()=="") bl->setcont(">");
      if (bl->getchild1()==-1)  bl->setchild1(addBlock(-1,-1,-1,ID,""));
      break;
    }
}


/*
CLASS FormulaBlock

a component of AFormula

Structure of a Block:


Prev(BL)---BL-------Next(Bl)
        /  |  \
      /    |     \
 child1   child2  child3       

Prev(child1)=prev(child2)=prev(child3)=BL
more info about child1,child2,child3 on widget.h
*/

FormulaBlock::FormulaBlock(FormulaDisplay *Mother,int Type = -1,int ID = -1,int nextID = -1,int prevID = -1,
  QString Cont = "", int Child1 = -1,int Child2=-1, int Child3=-1)
{

  active=0;
  getdime=1;
  sp=Child3;
  if (Type==4) Child3=-1;
  id=ID;
  mother=Mother;
  next=nextID;
  prev=prevID;
  type=Type;
  cont=Cont;
  child1=Child1;
  child2=Child2;
  child3=Child3;
  if((prev!=-1)&&(!mother->isFree[prev])) {
                  fontc=mother->Blocks[prev]->fontc;
	       }
	else {
		fontc.setFamily("utopia");
		fontc.setPointSize(32);
		fontc.setWeight(QFont::Normal);
		fontc.setItalic(FALSE);
	     }
}


void FormulaBlock::reduceFont(int recur)
{
int i=fontc.pointSize();
i*=3;
i/=4;
if (i < 1) i = 1;
fontc.setPointSize(i);
if (recur>0) {
		if(child1!=-1) mother->Blocks[child1]->reduceFont(2);
		if(child2!=-1) mother->Blocks[child2]->reduceFont(2);
		if(child3!=-1) mother->Blocks[child3]->reduceFont(2);
		if((recur==2)&&(next!=-1)) mother->Blocks[next]->reduceFont(2);
	      }
}

void FormulaBlock::enlargeFont(int recur)
{
int i;
i=fontc.pointSize();
fontc.setPointSize((i*4)/3);
if (recur>0) {
		if(child1!=-1) mother->Blocks[child1]->enlargeFont(2);
		if(child2!=-1) mother->Blocks[child2]->enlargeFont(2);
		if(child3!=-1) mother->Blocks[child3]->enlargeFont(2);
		if((recur==2)&&(next!=-1)) mother->Blocks[next]->enlargeFont(2);
	      }
}

/*
To Stephan:
Object are drawn as Pixmap or with lines
I'd like to put an option to use Pixmap or DrawFunctions of QPainters
Please do not modify this part of code. Only try to use KiconLoder
to load pixmaps.
There Also is a problem with drawArc.I'll explain it to you later.

Andrea 
*/

void FormulaBlock::PaintIt(QPainter *here,int x,int y,int corr)
{
int sp1=0,X1=0,Y1=0,Y2=0,X2=0,X=0;
QString str;
QString filename = getenv( "KDEDIR" );
    if ( !filename.isNull() )
	filename += "/share/apps/kformula/pics/";

    //QBitmap *mask;
 QWMatrix m;
 KIconLoader il;
       QBitmap pm;
//    QBitmap bm;    
       QPixmap newpm;    
QFontMetrics fm = QFontMetrics(fontc);
where.setRect(x,y-lyu-1,tx,lyu+lyd+2);
//if (type==0) where.setRect(x,y-lyu,tx+1,lyu+lyd);
if(active && (type!=0))   mother->setCursor(QRect(x-1,y-oyu-1,ox+2,oyu+oyd+2));
here->setBackgroundColor(white);
switch (type)
      {
      case (-1) :
       if(active) here->setPen(red);
       here->drawRect(x,y-5,10,10);
       if(active) here->setPen(black);
       if(next!=-1) mother->Blocks[next]->PaintIt(here,x+ox+FIXEDSPACE,y,corr);
      break;
      case 0:
      if(active) here->setPen(red);
      if (cont=="") { here->drawRect(x,y-5,10,10); type=-1;}
      if(child1!=-1){   Y1=mother->Blocks[child1]->getTY();
			X1=mother->Blocks[child1]->getTX();
			}
      if(child2!=-1){	X2=mother->Blocks[child2]->getTX();
 		        Y2=mother->Blocks[child2]->getTY();
	}
      if (X1>=X2) X2=X1;  //X2 = x in piu'
      here->setFont(fontc);
    
      here->drawText(x,y+sp,cont);  // add a -co to Y  
      if(active)
       mother->setCursor(QRect(x+fm.width(cont,mother->getPos()),y-oyu,2,oyu+oyd));    
#ifdef RECT
      here->drawRect(where);	
#endif
      if(active) here->setPen(blue);
      if(child1!=-1) mother->Blocks[child1]->PaintIt(here,x+ox-X2,y-oyu+ mother->Blocks[child1]->getLYU(),corr);
      if(active) here->setPen(blue);
      if(child2!=-1) mother->Blocks[child2]->PaintIt(here,x+ox-X2,y+oyd- mother->Blocks[child2]->getLYD(),corr);
      if(active) here->setPen(black);
      if(next!=-1) mother->Blocks[next]->PaintIt(here,x+ox+FIXEDSPACE,y,corr);
      break;


//////////////////////////////////////////////////////////////////////////
      case 1:
      if(active) here->setPen(red);
		
			Y1=mother->Blocks[child1]->getTY();
			X1=mother->Blocks[child1]->getTX();
 		        Y2=mother->Blocks[child1]->getLYU();
      if(child2!=-1){	X2=mother->Blocks[child2]->getTX();
	}
      X=X2;
      X2-=Y1/4; 	
      if (X2 < 0) X2=0;	
      here->drawLine(x+X2,y-Y2+Y1/4,x+X2+Y1/4,y+2+mother->Blocks[child1]->getLYD());	
      here->drawLine(x+X2,y-Y2+Y1/4-1,x+X2+Y1/4,y+1+mother->Blocks[child1]->getLYD());	
      here->drawLine(x+X2+Y1/4,y-mother->Blocks[child1]->getLYU()-4,x+X2+Y1/4,
				y+2+mother->Blocks[child1]->getLYD());	

      here->drawLine(x+X2+Y1/4,y-mother->Blocks[child1]->getLYU()-4,x+ox,y-mother->Blocks[child1]->getLYU()-4);	
#ifdef RECT  
    here->drawRect(where);	
#endif
      if(active) here->setPen(blue);
                    mother->Blocks[child1]->PaintIt(here,x+3+Y1/4+X2,y,corr);
      if(active) here->setPen(blue);
      if(child2!=-1)mother->Blocks[child2]->PaintIt(here,x+1,y-oyu+
                                  mother->Blocks[child2]->getLYU(),corr);
      if(active) here->setPen(black);
      if(next!=-1) mother->Blocks[next]->PaintIt(here,x+ox+FIXEDSPACE,y,corr);
      break;
///////////////////////////////////////////////////////////////////////////   
      case 2:
      if(active) here->setPen(red);
		
 		        Y2=mother->Blocks[child1]->getLYU();
	X=0;
      if(child2!=-1){	X=mother->Blocks[child2]->getTX();	
			X1=mother->Blocks[child2]->getLYU();	
	}
      if(child3!=-1){	X2=mother->Blocks[child3]->getTX();	
			if (X2>X) X=X2;
			X2=mother->Blocks[child3]->getLYD();		
	}
	Y1=(oyu+oyd)/8;
if(Y1<3) Y1=3;	
     here->drawLine(x+Y1,y+oyd-Y1/2,x+Y1,y-oyu+Y1/2);
     here->drawArc(x+Y1,y-oyu,Y1,Y1,0,2880);
     here->drawArc(x+corr,y+oyd-Y1,Y1,Y1,2880,2880);     
#ifdef RECT  
    here->drawRect(where);	
#endif
      if(active) here->setPen(blue);
                    mother->Blocks[child1]->PaintIt(here,x+2*Y1+X,y,corr);
      if(active) here->setPen(blue);
      if(child2!=-1)mother->Blocks[child2]->PaintIt(here,x+Y1+2,y-oyu+X1+Y1,corr);
      if(child3!=-1)mother->Blocks[child3]->PaintIt(here,x+Y1+2,y+oyd-X2-Y1,corr);
      if(active) here->setPen(black);
      if(next!=-1) mother->Blocks[next]->PaintIt(here,x+ox+FIXEDSPACE,y,corr);

      break;
///////////////////////////////////////////////////////////////////////////   
      case 3:
      
        if(active) here->setPen(red);  
      Y1=mother->Blocks[child1]->getTY();
      Y2=mother->Blocks[child1]->getLYU();      
      Y2-=Y1/2;
      Y1=y-Y2;
      Y2=0;
      if (child2!=-1) Y2=mother->Blocks[child2]->getTX();
      X1=ox-sp/4-Y2;
      corr=0;
#ifdef RECT
     here->drawRect(where);	
#endif     
     for (X2=0;X2<=1;X2++) {
X=X1*X2;
m.reset();
switch(cont[X2])
{

     case '{' :
     pm.load(filename+"par3.xbm"); 
     m.scale(1.0*sp/200,1.0*sp/200);
     newpm = pm.xForm(m);     
     here->drawPixmap(x+X,Y1-sp/2,newpm);
     m.reset();
     m.scale(1.0,-1.0);
     pm = newpm.xForm(m);
     here->drawPixmap(x+X,Y1,pm);
 
     break;
     case '}' :
     pm.load(filename+"par3.xbm"); 
     m.scale(1.0*sp/200,1.0*sp/200);
     newpm = pm.xForm(m);     
     m.reset();
     m.scale(-1.00,1.00);
     pm = newpm.xForm(m);
     m.reset();
     m.scale(1.00,-1.00);
     newpm = pm.xForm(m);
     here->drawPixmap(x+X,Y1-sp/2,pm);
     here->drawPixmap(x+X,Y1,newpm);
     
     break;

     case '[' :
     pm.load(filename+"par2.xbm"); 
     m.scale(1.0*sp/200,1.0*sp/200);
     newpm = pm.xForm(m);     
     here->drawPixmap(x+X,Y1-sp/2,newpm);
     m.reset();
     m.scale(1.0,-1.0);
     pm = newpm.xForm(m);
     here->drawPixmap(x+X,Y1,pm);
 
     break;
     case ']' :
     pm.load(filename+"par2.xbm"); 
     m.scale(1.0*sp/200,1.0*sp/200);
     newpm = pm.xForm(m);     
     m.reset();
     m.scale(-1.00,1.00);
     pm = newpm.xForm(m);
     m.reset();
     m.scale(1.00,-1.00);
     newpm = pm.xForm(m);
     here->drawPixmap(x+X,Y1-sp/2,pm);
     here->drawPixmap(x+X,Y1,newpm);
     
     break;

    case '(' :
     pm.load(filename+"par1.xbm"); 
     m.scale(1.0*sp/200,1.0*sp/200);
     newpm = pm.xForm(m);     

     here->drawPixmap(x+X,Y1-sp/2,newpm);
     m.reset();
     m.scale(1.0,-1.0);
     pm = newpm.xForm(m);
     here->drawPixmap(x+X,Y1,pm);
 
     break;
     case ')' :
     pm.load(filename+"par1.xbm"); 
     m.scale(1.0*sp/200,1.0*sp/200);
     newpm = pm.xForm(m);     
     m.reset();
     m.scale(-1.00,1.00);
     pm = newpm.xForm(m);
     m.reset();
     m.scale(1.00,-1.00);
     newpm = pm.xForm(m);
    // mask = new QBitmap();
//      mask& = pm;
     pm.setMask( pm.createHeuristicMask() );
     here->drawPixmap(x+X,Y1-sp/2,pm);
     here->drawPixmap(x+X,Y1,newpm);
     
     break;
     
    case 'A' :
      sp1=sp/8;
     here->drawLine(x+sp1+X,Y1+sp1*3,x+sp1+X,Y1+sp1);
     here->drawArc(x+sp1+X,Y1      ,sp1*2,sp1*2,1440,1440);
     here->drawArc(x-sp1+X,Y1+sp1*2,sp1*2,sp1*2,-1440,1439);     
     here->drawArc(x-sp1+X,Y1-sp1*4,sp1*2,sp1*2,0,1440);
     here->drawArc(x+sp1+X,Y1-sp1*2,sp1*2,sp1*2,2880,1440);     
     here->drawLine(x+sp1+X,Y1-sp1*3,x+sp1+X,Y1-sp1); 
      
      break;
    case 'a' :
     here->drawLine(x+sp/8+X,Y1-sp/8*3,x+sp/8+X,Y1-sp/8);
     here->drawArc(x+sp/8+X,Y1-sp/2,sp/4,sp/4,1440,1440);
     here->drawArc(x+X-sp/8,Y1-sp/4,sp/4,sp/4,4320,1440);     
     here->drawLine(x+sp/8+X,Y1+sp/8,x+sp/8+X,Y1+sp/8*3);
     here->drawArc(x-sp/8+X,Y1,sp/4,sp/4,0,1440);
     here->drawArc(x+X+sp/8,Y1+sp/4,sp/4,sp/4,2880,1440);     
     //here->drawArc(100,100,sp/4,sp/4,0,5760);      
    break;
    case 'b' :
	here->drawLine(x+X,Y1-sp/2,x+X,Y1+sp/2);
	here->drawLine(x+X,Y1-sp/2,x+X+sp/4,Y1-sp/2);
	here->drawLine(x+X,Y1+sp/2,x+X+sp/4,Y1+sp/2);
    break;   
    case 'B' :
	here->drawLine(x+X+4*sp,Y1-sp/2,x+X+sp,Y1+sp/2);
	here->drawLine(x+X+sp,Y1-sp/2,x+X+sp/4,Y1-sp/2);
	here->drawLine(x+X+sp,Y1+sp/2,x+X+sp/4,Y1+sp/2);
    break;   
    case 'c':
        here->drawArc(x+X,Y1-sp/2,sp/4,sp,100*16,160*16);
	here->drawArc(x+X+1,Y1-sp/2,sp/4,sp,100*16,160*16);
    break;
    case 'C':
        here->drawArc(x+X,Y1-sp/2,sp/4,sp,280*16,160*16);
	here->drawArc(x+X,Y1-sp/2,sp/4+1,sp,280*16,160*16);
    break;
    
    case '|' :
	here->drawLine(x+X+sp/8,Y1-sp/2,x+X+sp/8,Y1+sp/2);
    break;   
    case '\\' :
	here->drawLine(x+X+sp/5,Y1+sp/2,x+X,Y1-sp/2);
    break;   
    case '/' :
	here->drawLine(x+X+sp/5,Y1-sp/2,x+X,Y1+sp/2);
    break;   
  } // end of switch
 }  // end of for
      if(active) here->setPen(blue);
      mother->Blocks[child1]->PaintIt(here,x+sp/4,y,corr);	
      if(child2!=-1) mother->Blocks[child2]->PaintIt(here,x+ox-Y2,y-oyu+mother->Blocks[child2]->getLYU(),corr);  
      if(active) here->setPen(black);
      if(next!=-1) mother->Blocks[next]->PaintIt(here,x+ox+FIXEDSPACE,y,corr);      
      break;
///////////////////////////////////////////////////////////////////////////   
      case 4:
      if(active) here->setPen(red);
      if (cont[0]=='l') here->drawLine(x,y,x+ox,y);


		 X1=mother->Blocks[child1]->getTX();    
                 Y1=mother->Blocks[child1]->getLYD();
		 X2=mother->Blocks[child2]->getTX();    
	         Y2=mother->Blocks[child2]->getLYU();
      if (X1>X2) {X2=(X1-X2)/2; X1=0; }else{X1=(X2-X1)/2; X2=0;}
      X=2;  
      if (cont[0]=='v') X=X2=X1=0;

      

#ifdef RECT
      here->drawRect(where);	
#endif
      if(active) here->setPen(blue);
      mother->Blocks[child1]->PaintIt(here,x+X1+X,y-Y1-sp/2,corr);
      if(active) here->setPen(blue);
      mother->Blocks[child2]->PaintIt(here,x+X2+X,y+Y2+sp/2,corr);
      if(active) here->setPen(black);
      if(next!=-1) mother->Blocks[next]->PaintIt(here,x+ox+FIXEDSPACE,y,corr);      
      break;
///////////////////////////////////////////////////////////////////////////   
      case 5:  
      if(active) here->setPen(red);   
       switch(cont[0]) {
         case '>':
	    here->drawLine(x,y-oyu+sp/2,x+ox,y-oyu+sp/2);
	    here->drawLine(x+ox*4/5,y-oyu,x+ox,y-oyu+sp/2);
            here->drawLine(x+ox*4/5,y-oyu+sp,x+ox,y-oyu+sp/2);
	    break;
         case '<':
	    here->drawLine(x,y-oyu+sp/2,x+ox,y-oyu+sp/2);
	    here->drawLine(x+ox/5,y-oyu,x,y-oyu+sp/2);
            here->drawLine(x+ox/5,y-oyu+sp,x,y-oyu+sp/2);
	    break;


         case '-':
	    here->drawLine(x,y-oyu+sp/2,x+ox,y-oyu+sp/2);
	    break;

           case 'o':
	    here->drawArc(x+ox/3,y-oyu,sp,sp,0,5760);
	    break;
        case '^':
	    here->drawLine(x,y-oyu+sp,x+ox/2,y-oyu);
	    here->drawLine(x+ox,y-oyu+sp,x+ox/2,y-oyu);
	    break;          
       }
      if(active) here->setPen(blue);
     // warning("draw child1 of Symb");
        if(child1!=-1)mother->Blocks[child1]->PaintIt(here,x,y,corr);	
      if(active) here->setPen(black);
      if(next!=-1) mother->Blocks[next]->PaintIt(here,x+ox+FIXEDSPACE,y,corr);      
      break;
///////////////////////////////////////////////////////////////////////////         
      }
//here->setPen(black);
}

int FormulaBlock::getClick(QPoint click)
{
int retu;
retu=0;
if (where.contains(click)) 
	{
	  if(child1 !=-1) retu=mother->Blocks[child1]->getClick(click);
	  if((retu==0)&&(child2!=-1)) retu=mother->Blocks[child2]->getClick(click);
 	  if(/*(type!=4)&&*/(retu==0)&&(child3!=-1)) retu=mother->Blocks[child3]->getClick(click);
          if((retu==0)&&(next!=-1)) retu=mother->Blocks[next]->getClick(click);
	  if(retu==0)retu=id;
 	} 
return( retu );

}

/*
LYU: Larger  Y (Up)
OYU: Object  Y (Up)
LYD: Larger  Y (Down)
OYD: Object  Y (Down)
tx: Total X
ox:Object x
		      
             _._._._._tx._._._._._._._   
         /\ |____ox______     |      |
     LYU || |/\_         |    |      | 
         \/ |\/ OYU      |    |      |
-midline----|            |----|      |
            |  OUR BLOCK |    |      |
        OYD=LYD          |    |______|
            |____________|._._._._._._

*/


void FormulaBlock::getDimensions()
{

int X1=0,X2=0;
QFontMetrics fm(fontc);
QRect di;

if(getdime) 
  switch (type)
      {
      case (-1):
      lyu=lyd=oyu=oyd=5;
      tx=ox=10;
      tx+=FIXEDSPACE;
      sp=-1;
      		
      if (next != -1) {
			mother->Blocks[next]->getDimensions();
			tx+=mother->Blocks[next]->getTX(); 
			X1=(mother->Blocks[next]->getLYU());
			X2=(mother->Blocks[next]->getLYD());
			if(lyu<X1) lyu=X1;
			if(lyd<X2) lyd=X2;
		      }
      break;

      case 0: 
 di=fm.boundingRect(cont);       
      ox=fm.width(cont);				    //get string X
      oyu=-di.top();
      oyd=di.bottom(); 
      sp=fm.strikeOutPos();  
      oyu-=sp;
      oyd+=sp;
    if(cont==""){
      lyu=lyd=oyu=oyd=5;
      tx=ox=10;
      tx+=FIXEDSPACE;
      sp=-1;
      type=-1;  
      }
    if (oyu<1) oyu=1;      
    if (oyd<1) oyd=1;
          if (child1!=-1){
		 mother->Blocks[child1]->getDimensions();
    		 X1=mother->Blocks[child1]->getTX();    //get exponent X
                 oyu+=(mother->Blocks[child1]->getTY() );
			}
      if (child2!=-1){
		mother->Blocks[child2]->getDimensions();
 		 X2=mother->Blocks[child2]->getTX();    //get index X
	         oyd+=(mother->Blocks[child2]->getTY() );
		     }
     lyu=oyu;
      lyd=oyd;	
 
      if (X1>=X2) ox+=X1; else ox+=X2;                      //add to OwnX the > x
      tx=ox;
      tx+=FIXEDSPACE;
      if (next != -1) {
			mother->Blocks[next]->getDimensions();
			tx+=mother->Blocks[next]->getTX(); 
			X1=(mother->Blocks[next]->getLYU());
			X2=(mother->Blocks[next]->getLYD());
			if(lyu<X1) lyu=X1;
			if(lyd<X2) lyd=X2;
		      }
      break;

      case 1:
       mother->Blocks[child1]->getDimensions();
//       oyd=mother->Blocks[child1]->getTY()/2;
//       oyu=oyd+4;
       oyd=mother->Blocks[child1]->getLYD();
       oyu=mother->Blocks[child1]->getLYU()+4;

       ox=mother->Blocks[child1]->getTX()+oyd/2;	
       ox+=(oyd+oyu)/4+1;
       
           if (child2 != -1) {
		           mother->Blocks[child2]->getDimensions();
			   X1=mother->Blocks[child2]->getTY();
 			   X1-=oyu/4;
			   if (X1>0)  oyu+=X1;
			   X1=mother->Blocks[child2]->getTX();
 			   X1-=oyd/4;
			   if (X1>0)  ox+=X1;
			 }
      lyu=oyu;
      lyd=oyd;	
      tx=ox;
      tx+=FIXEDSPACE;      
      if (next != -1) {
			mother->Blocks[next]->getDimensions();
			tx+=mother->Blocks[next]->getTX(); 
			X1=(mother->Blocks[next]->getLYU());
			X2=(mother->Blocks[next]->getLYD());
			if(lyu<X1) lyu=X1;
			if(lyd<X2) lyd=X2;
		      }

      break;
      case 2:

       mother->Blocks[child1]->getDimensions();
       oyd=mother->Blocks[child1]->getLYD();
       oyu=mother->Blocks[child1]->getLYU();
       ox=mother->Blocks[child1]->getTX();	
     	X2=0;
           if (child2 != -1) {
		           mother->Blocks[child2]->getDimensions();
			   oyu+=mother->Blocks[child2]->getTY();
 			   X1=mother->Blocks[child2]->getTX();
			   X2=X1;
			 }

           if (child3 != -1) {
		           mother->Blocks[child3]->getDimensions();
			   oyd+=mother->Blocks[child3]->getTY();
 			   X1=mother->Blocks[child3]->getTX();
			   if (X1>X2)  X2=X1;
			 } 
// oyu+=3;
// oyd+=3;
      ox+=X2;
      X2=oyu+oyd; 
      oyu+=X2/6;	
      oyd+=X2/6;
      ox+=X2/3;
      lyu=oyu;
      lyd=oyd;	
      tx=ox;
      tx+=FIXEDSPACE;      
      if (next != -1) {
			mother->Blocks[next]->getDimensions();
			tx+=mother->Blocks[next]->getTX(); 
			X1=(mother->Blocks[next]->getLYU());
			X2=(mother->Blocks[next]->getLYD());
			if(lyu<X1) lyu=X1;
			if(lyd<X2) lyd=X2;
		      }

      break;
      case 3:
        sp=0;
	       	 mother->Blocks[child1]->getDimensions();
    		 ox=mother->Blocks[child1]->getTX();    
    		 X2=mother->Blocks[child1]->getTY();
        	 oyu=(mother->Blocks[child1]->getLYU());
		 oyd=(mother->Blocks[child1]->getLYD());
		 sp=(X2 / 5) * 5; // /16;    
		 // if(sp*16<(X2)) sp++;
                 sp+=4; 
		 if (sp < 20) sp = 20;
	         X1=sp-X2;  //16*sp-X2;
	 	 ox+=(sp/2);
                 if (child2!=-1){
	       	   mother->Blocks[child2]->getDimensions();
 		   ox+=mother->Blocks[child2]->getTX();    //get index X
	           oyu+=(mother->Blocks[child2]->getTY() );
		 }

                 lyu=oyu+=X1/2;
		 lyd=oyd+=X1/2;
		 tx=ox;
                 tx+=FIXEDSPACE;
      if (next != -1) {
			mother->Blocks[next]->getDimensions();
			tx+=mother->Blocks[next]->getTX(); 
			X1=(mother->Blocks[next]->getLYU());
			X2=(mother->Blocks[next]->getLYD());
			if(lyu<X1) lyu=X1;
			if(lyd<X2) lyd=X2;
		      }

      break;

      case 4:
      //sp=child3;
		mother->Blocks[child1]->getDimensions();
		mother->Blocks[child2]->getDimensions();
    		 X1=mother->Blocks[child1]->getTX();    
                oyu=mother->Blocks[child1]->getTY()+sp/2;
		 X2=mother->Blocks[child2]->getTX();    
	         oyd=mother->Blocks[child2]->getTY()+sp/2;
      lyu=oyu;
      lyd=oyd;	
 
      if (X1>=X2) ox=X1+4; else ox=X2+4;                      //add to OwnX the > x
      tx=ox;
      tx+=FIXEDSPACE;
      if (next != -1) {
			mother->Blocks[next]->getDimensions();
			tx+=mother->Blocks[next]->getTX(); 
			X1=(mother->Blocks[next]->getLYU());
			X2=(mother->Blocks[next]->getLYD());
			if(lyu<X1) lyu=X1;
			if(lyd<X2) lyd=X2;
		      }

      break;
      case 5:
        if (child1!=-1) {       
                 mother->Blocks[child1]->getDimensions();
    		 ox=mother->Blocks[child1]->getTX();    
        	 oyu=(mother->Blocks[child1]->getLYU());
		 oyd=(mother->Blocks[child1]->getLYD());
                	} 
			  
      switch (cont[0]) {
         case '>' : 
	   sp=ox/4;
	   oyu+=sp;   
         break; 	
         case '<' : 
	   sp=ox/4;
	   oyu+=sp;   
         break; 	

         case '-' : 
	   sp=3;
	   oyu+=sp;   
         break; 	
         case 'o' : 
	   sp=ox/3;
	   oyu+=sp;   
	 break;                  
         case '^' : 
	   sp=ox/5;
	   oyu+=sp;   
	 break;                  
     }
     oyu+=1;
	 lyu=oyu;
    lyd=oyd;
    tx=ox;    
	 if (next != -1) {
			mother->Blocks[next]->getDimensions();
			tx+=mother->Blocks[next]->getTX(); 
			X1=(mother->Blocks[next]->getLYU());
			X2=(mother->Blocks[next]->getLYD());
			if(lyu<X1) lyu=X1;
			if(lyd<X2) lyd=X2;
		      }
      
      
      break;
      
      }


}
int FormulaBlock::getID()
{
return(id);
}

int FormulaBlock::getOX()
{
return(ox);
}
int FormulaBlock::getTX()
{
return(tx);
}
int FormulaBlock::getLYU()
{
return(lyu);
}
int FormulaBlock::getLYD()
{
return(lyd);
}
int FormulaBlock::getTY()
{
return(lyd+lyu);
}




#include "widget.moc"
