#include "kformula_doc.h"

#include <koIMR.h>
#include <komlMime.h>
#include <koStream.h>

#include <kurl.h>
#include <kiconloader.h>
#include <kapp.h>
#include <qpopupmenu.h>
#include <qmsgbox.h>
#include <qwmatrix.h>
#include <qcolor.h>
#include <qbitmap.h>
#include <qwidget.h>

#include <unistd.h>

#include <component_impl.h>

#define FIXEDSPACE 1             // Space between 2 blocks

KFormulaDocument::KFormulaDocument()
{
    // Use CORBA mechanism for deleting views
    m_lstViews.setAutoDelete( false );

    m_bModified = false;
    Blocks[0] = new FormulaBlock(this,-1,0,-1,-1,"");
    for(firstfree=1;firstfree<2000;firstfree++)
	isFree[firstfree] = true;
    // Blocks[firstfree]=NULL;
    isFree[0]=false;
    firstfree=1;
    pos = current = 0;
}

CORBA::Boolean KFormulaDocument::init()
{
    return true;
}

KFormulaDocument::~KFormulaDocument()
{
    cleanUp();
}

void KFormulaDocument::cleanUp()
{
    if ( m_bIsClean )
	return;

    assert( m_lstViews.count() == 0 );
  
    Document_impl::cleanUp();
}

bool KFormulaDocument::load( KOMLParser& parser )
{
    // HACK
    return false;
}

bool KFormulaDocument::save( ostream &out )
{
    out << "<?xml version=\"1.0\"?>" << endl;
    out << otag << "<DOC author=\"" << "Torben Weis" << "\" email=\"" 
	<< "weis@kde.org" << "\" editor=\"" << "KFormula" 
	<< "\" mime=\"" << MIME_TYPE << "\" >" << endl;
    out << etag << "</DOC>" << endl;
  
    return true;
}

void KFormulaDocument::viewList( OPParts::Document::ViewList*& _list )
{
    (*_list).length( m_lstViews.count() );

    int i = 0;
    QListIterator<KFormulaView> it( m_lstViews );
    for( ; it.current(); ++it )
	{
	    (*_list)[i++] = OPParts::View::_duplicate( it.current() );
	}
}

void KFormulaDocument::addView( KFormulaView *_view )
{
    m_lstViews.append( _view );
}

void KFormulaDocument::removeView( KFormulaView *_view )
{
    m_lstViews.setAutoDelete( false );
    m_lstViews.removeRef( _view );
    m_lstViews.setAutoDelete( true );
}

OPParts::View_ptr KFormulaDocument::createView()
{
    KFormulaView *p = new KFormulaView( 0L );
    p->setDocument( this );
  
    return OPParts::View::_duplicate( p );
}

void KFormulaDocument::emitModified()
{
    emit sig_modified();
}

int KFormulaDocument::getPos()
{
    return pos;
}

void KFormulaDocument::setCurrent(int cur)
{
    if( current != cur )
	pos = Blocks[cur]->getcont().length();
    Blocks[current]->setactive(0);
    Blocks[cur]->setactive(1); 
    current = cur; 

    emit sig_changeText( Blocks[cur]->getcont() );  //Signals
    emit sig_changeType( Blocks[cur]->gettype() );
}

int KFormulaDocument::getCurrent()
{
    return ( current );
}

void KFormulaDocument::setCursor( QRect re )
{
    cursorPos = re;
}

void KFormulaDocument::setGreek( bool _on )
{
    if ( _on ) 
	Blocks[getCurrent()]->fontc.setFamily("Symbol");	
    else 
	Blocks[getCurrent()]->fontc.setFamily("utopia");

    emit sig_modified();
}

void KFormulaDocument::change( const char * newcont )
{
    if ( Blocks[getCurrent()]->gettype() == -1 )
	Blocks[getCurrent()]->settype(0);
    Blocks[getCurrent()]->setcont(newcont);

    emit sig_modified();
}

void KFormulaDocument::reduce()
{   
    Blocks[getCurrent()]->reduceFont(0);
    emit sig_modified();
}

void KFormulaDocument::reduceRecur()
{   
    Blocks[getCurrent()]->reduceFont(1);
    emit sig_modified();
}
void KFormulaDocument::reduceAll()
{   
    Blocks[getCurrent()]->reduceFont(2);
    emit sig_modified();
}

void KFormulaDocument::enlarge()
{   
    Blocks[getCurrent()]->enlargeFont(0);
    emit sig_modified();
}
void KFormulaDocument::enlargeRecur()
{   
    Blocks[getCurrent()]->enlargeFont(1);
    emit sig_modified();
}
void KFormulaDocument::enlargeAll()
{   
    Blocks[getCurrent()]->enlargeFont(2);
    emit sig_modified();
}

void KFormulaDocument::addCh3()
{   
    warning("Add Ch3");
    int nextold=Blocks[getCurrent()]->getchild3();
    int Id=addBlock(-1,-1,nextold,getCurrent(),"");
    Blocks[getCurrent()]->setchild3(Id);
    if( Blocks[getCurrent()]->gettype() == 2 )
	Blocks[Id]->reduceFont(1);    
    setCurrent(Id);	
    emit sig_modified();
}

void KFormulaDocument::line(int i)
{   
    if( i == 1 )
	Blocks[getCurrent()]->setcont("line");
    else        
	Blocks[getCurrent()]->setcont("vertical space");
    emit sig_modified();
}

void KFormulaDocument::addCh2()
{   
    warning("Add Ch2");
    int nextold=Blocks[getCurrent()]->getchild2();
    int Id=addBlock(-1,-1,nextold,getCurrent(),"");
    Blocks[getCurrent()]->setchild2(Id); 
    if( Blocks[getCurrent()]->gettype() < 4 )
	Blocks[Id]->reduceFont(1);    
    setCurrent(Id);	
    emit sig_modified();
}
void KFormulaDocument::addCh1()
{
       warning("Add Ch1");
    int nextold=Blocks[getCurrent()]->getchild1();
    int Id=addBlock(-1,-1,nextold,getCurrent(),"");
    Blocks[getCurrent()]->setchild1(Id); 	
    if( Blocks[getCurrent()]->gettype() == 0 )
	Blocks[Id]->reduceFont(1);    
    setCurrent(Id);
    emit sig_modified();
}

void KFormulaDocument::pro()
{   
    // HACK
    /*
      new ProData(Blocks[getCurrent()],this);  //proprieties
      checkAndCreate(Blocks[getCurrent()]);
      ChText(Blocks[getCurrent()]->getcont());
      ChType(Blocks[getCurrent()]->gettype());
      setFocus(); 
      update();   */       
}

void KFormulaDocument::dele()
{   
    deleteIt( Blocks[ getCurrent() ] );
}

void KFormulaDocument::addB1()
{   
    int cur=getCurrent();
    int Id=cur;
    if( Blocks[cur]->gettype() != -1 )
	{
	    int nextold=Blocks[cur]->getnext();
	    Id = addBlock(1,-1,nextold,cur,"c");
	    Blocks[cur]->setnext(Id);
	}
    else 
	{
	    Blocks[cur]->settype(1);
	    checkAndCreate(Blocks[cur]);
	}
    setCurrent(Blocks[Id]->getchild1());
    emit sig_modified();
}

void KFormulaDocument::addB2()
{   
    int cur=getCurrent();
    int Id=cur;
    if( Blocks[cur]->gettype() != -1 )
	{
	    int nextold=Blocks[cur]->getnext();
	    Id=addBlock(2,-1,nextold,cur,"c");
	    Blocks[cur]->setnext(Id);
	}
    else
	{
	    Blocks[cur]->settype(2);
	    checkAndCreate(Blocks[cur]);
	}
    setCurrent(Blocks[Id]->getchild1());	
    emit sig_modified();
}

void KFormulaDocument::addB0()
{   
warning("call");
    int cur=getCurrent();
    warning("get cur");
    int nextold=Blocks[cur]->getnext();
    int Id=addBlock(-1,-1,nextold,cur,"");
    Blocks[cur]->setnext(Id); 
    warning("Aggiungo Block type 0");
    setCurrent(Id);	
    warning("Current ID impostato");
    emit sig_modified();
    warning("Sig emitted");
}

void KFormulaDocument::addB5()
{   
    int cur=getCurrent();
    int Id=cur;
    if( Blocks[cur]->gettype() != -1 )
	{ 
	    int nextold=Blocks[cur]->getnext();
	    Id=addBlock(5,-1,nextold,cur,"");
	    Blocks[cur]->setnext(Id);
	}
    else
	{    
	    Blocks[cur]->settype(5);
	    Blocks[cur]->setcont(">");
	    checkAndCreate(Blocks[cur]);
	} 
    setCurrent(Blocks[Id]->getchild1());	
    emit sig_modified();
}

void KFormulaDocument::addB4()
{   
    int nextold=-1; 
    int Id,cur;
    cur=getCurrent();
    Id=cur;
    if ( Blocks[cur]->gettype() != -1 )
	{
	    nextold=Blocks[cur]->getnext();
	    Id=addBlock(4,-1,nextold,cur,"line");
	    Blocks[cur]->setnext(Id);
	}
    else
	{
	    Blocks[cur]->settype(4);
	    Blocks[cur]->setcont("line");
	    checkAndCreate(Blocks[cur]);
	}
    setCurrent(Blocks[Id]->getchild1());	
    emit sig_modified();
}
void KFormulaDocument::addB4bis()
{   
    int nextold=-1; 
    int Id,cur;
    cur=getCurrent();
    Id=cur;
    if( Blocks[cur]->gettype() != -1 )
	{
	    nextold=Blocks[cur]->getnext();
	    Id=addBlock(4,-1,nextold,cur,"vertical space");
	    Blocks[cur]->setnext(Id); 
	}
    else
	{
	    Blocks[cur]->settype(4);
	    Blocks[cur]->setcont("vertical space");
	    checkAndCreate(Blocks[cur]);    
	}
    setCurrent(Blocks[Id]->getchild1());	
    emit sig_modified();
}

void KFormulaDocument::addB3()
{   
    int nextold=-1; 
    int Id,cur;
    cur=getCurrent();
    Id=cur;
    if( Blocks[cur]->gettype() != -1 )
	{
           warning("create"); 
	    nextold=Blocks[cur]->getnext();
	    Id=addBlock(3,-1,nextold,cur,"()");
	    Blocks[cur]->setnext(Id);
	}
    else
	{
            warning("change only");
	    Blocks[cur]->settype(3);
	    Blocks[cur]->setcont("()");
	    checkAndCreate(Blocks[cur]);   
            warning("C&C called");
	}
    setCurrent(Blocks[Id]->getchild1());	
    emit sig_modified();
}


int KFormulaDocument::addBlock(int Type,int ID,int nextID,int prevID,
			       QString Cont, int Child1,int Child2, int Child3)
{
    if ( ID == -1 )
	ID=firstfree;
    isFree[ID]=FALSE;  
    for( firstfree=1; !isFree[firstfree]; firstfree++ );
    QString s;  
    s.sprintf("First free block: %i",firstfree);    
        warning(s);   
    
    Blocks[ID]= new FormulaBlock(this,Type,ID,nextID,prevID,Cont,Child1,Child2,Child3);
    checkAndCreate(Blocks[ID]);
    return (ID);
}

void KFormulaDocument::deleteIt(FormulaBlock *bl)
{
    int id=bl->getID();
    if( id != 0 )
	{
	    int next=bl->getnext();
	    int prev=bl->getprev();
	    int child1=bl->getchild1();
	    int child2=bl->getchild2();
	    int child3=bl->getchild3(); 
	    if( next != -1 )
		Blocks[next]->setprev(prev);
	    if( prev != -1 )
		{
		    if( Blocks[prev]->getnext() == id )
			Blocks[prev]->setnext(next);
		    if( Blocks[prev]->getchild1() == id )
			Blocks[prev]->setchild1(next);
		    if( Blocks[prev]->getchild2() == id )
			Blocks[prev]->setchild2(next);
		    if( (Blocks[prev]->getchild3() == id )
			/*&&(Blocks[prev]->gettype()!=4)*/) Blocks[prev]->setchild3(next);
		}
	    if( child1 != -1 )
		deleteIt(Blocks[child1]);
	    if( child2 != -1 )
		deleteIt(Blocks[child2]);
	    if( ( child3 != -1 )
		/*&&(bl->gettype()!=4)*/) deleteIt(Blocks[child3]);
	    checkAndCreate(Blocks[prev]);
	    if( prev != -1 )
		setCurrent(prev);
	    isFree[id]=TRUE; 
	    delete bl;
	    emit sig_modified();
	}
    else
	QMessageBox::critical( 0L, i18n("Error"), i18n("Can not delete First Block !!"), i18n( "Ok" ) );
}

void KFormulaDocument::mousePressEvent( QMouseEvent *a,QWidget *wid)
{
   setCurrent(Blocks[0]->getClick(a->pos()));
   emit sig_modified();
if(a->button()==RightButton){
   QPopupMenu *mousepopup = new QPopupMenu;
   QPopupMenu *convert = new QPopupMenu;
   QPopupMenu *fontpopup = new QPopupMenu;    
       fontpopup->insertItem(i18n("Font +"), this, SLOT(enlarge()), ALT+Key_K);
    fontpopup->insertItem(i18n("Font -"), this, SLOT(reduce()), ALT+Key_K);
    fontpopup->insertItem(i18n("Font + (also Children)"), this, SLOT(enlargeRecur()), ALT+Key_K);
    fontpopup->insertItem(i18n("Font - (also Children)"), this, SLOT(reduceRecur()), ALT+Key_K);
    fontpopup->insertItem(i18n("All Font +"), this, SLOT(enlargeAll()), ALT+Key_K);
    fontpopup->insertItem(i18n("All Font -"), this, SLOT(reduceAll()), ALT+Key_K);
//    convert->insertItem(klocale("Simple Text"), parent->_part, SLOT(slotQuit()), ALT+Key_E);
//    convert->insertItem("AAAARGGHHH", _part, SLOT(slotQuit()), ALT+Key_E);
    mousepopup->insertItem(i18n("Proprieties"), this,SLOT(pro()), ALT+Key_E);
    mousepopup->insertItem(i18n("Delete"), this,SLOT(dele()), ALT+Key_E);
    mousepopup->insertSeparator();
    mousepopup->insertItem(i18n("Change font.."),fontpopup);    
    mousepopup->insertItem(i18n("Convert to.."),convert);
//    mousepopup->insertSeparator();
//    mousepopup->insertItem(i18n("Quit"), _part, SLOT(slotQuit()), ALT+Key_Q);
    mousepopup->popup(wid->mapToGlobal(a->pos()));
}
}

void KFormulaDocument::keyPressEvent( QKeyEvent *k )
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
 warning("Get everything...process key pressed...");
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
    warning("Ascii");
c1=c;
c2=c1;
if(Blocks[c1]->gettype()>0) {    
    next=Blocks[c1]->getnext();
    c2=addBlock(0,-1,next,c1,"");
    Blocks[c1]->setnext(c2);
    warning("Add a block type0"); 
     }
       else Blocks[c1]->settype(0);
    setCurrent(c2);	   
    warning("Set current");
    Blocks[getCurrent()]->getcont().insert(pos,k->ascii());
    warning("insert success");
    pos++; 
    warning("pos ++");
  } 
//ChText(Blocks[getCurrent()]->getcont());
//ChType(Blocks[getCurrent()]->gettype());
//update();
	    emit sig_modified();
}


void KFormulaDocument::paintEvent( QPaintEvent *_ev, QWidget *paintGround )
{
    QPainter painter(paintGround);
    Blocks[0]->getDimensions(); // Get dimension of Block0 (all formula dimension)
    painter.setPen( black );
    Blocks[0]->PaintIt(&painter,1+xOff,1+yOff+Blocks[0]->getLYU(),1);
    isBlack=false;
}

void KFormulaDocument::checkAndCreate(FormulaBlock *bl)
{
    int ID=bl->getID();
    switch ( bl->gettype() )
	{
	case 1:
            warning("case 1"); 
	    if ( bl->getchild1() == -1 )
		bl->setchild1(addBlock(-1,-1,-1,ID,""));
	    break;
	case 2:
            warning("case 2"); 
	    if ( bl->getchild1() == -1 )
		bl->setchild1(addBlock(-1,-1,-1,ID,""));
	    break;
	case 3:
            warning("case 3"); 
	    if( bl->getcont() == "" )
		bl->setcont("()");      
	    if ( bl->getchild1() == -1 )
		bl->setchild1(addBlock(-1,-1,-1,ID,""));
	    break;
	case 4:
            warning("case 4"); 
	    if ( bl->getcont() == "" )
		bl->setcont("line");
	    if ( bl->getchild1() == -1 )
		bl->setchild1(addBlock(-1,-1,-1,ID,""));
	    if ( bl->getchild2() == -1 )
		bl->setchild2(addBlock(-1,-1,-1,ID,"")); 
	    if ( bl->getsp() == -1 )
		bl->setsp(10);  
	    break;
	case 5:
            warning("case 5"); 
	    if ( bl->getcont() == "" )
		bl->setcont(">");
	    if ( bl->getchild1() == -1 )
		bl->setchild1(addBlock(-1,-1,-1,ID,""));
	    break;
	}
}

/****************************************************************
 *
 * KFormulaBlock
 *
 ****************************************************************/

/**
 * CLASS FormulaBlock
 *
 * a component of AFormula
 *
 * Structure of a Block:
 *
 *
 * Prev(BL)---BL-------Next(Bl)
 *          /  |  \
 *        /    |     \
 *  child1   child2  child3       
 *
 * Prev(child1)=prev(child2)=prev(child3)=BL
 * more info about child1,child2,child3 on widget.h
 */

FormulaBlock::FormulaBlock( KFormulaDocument *_doc, int Type, int ID, 
			    int nextID, int prevID,
			    QString Cont, int Child1, int Child2, int Child3 )
{
    active = 0;
    getdime = 1;
    sp = Child3;
    if ( Type == 4 )
	Child3 = -1;
    id = ID;
    m_pDoc = _doc;
    next = nextID;
    prev = prevID;
    type = Type;
    cont = Cont;
    child1 = Child1;
    child2 = Child2;
    child3 = Child3;
    
    if ( prev != -1 && !m_pDoc->isFree[prev] )
	{
	    fontc = m_pDoc->Blocks[prev]->fontc;
	}
    else
	{
	    fontc.setFamily( "utopia" );
	    fontc.setPointSize( 32 );
	    fontc.setWeight( QFont::Normal );
	    fontc.setItalic( FALSE );
	}
}

void FormulaBlock::reduceFont( int recur )
{
    int i = fontc.pointSize();
    i *= 3;
    i /= 4;
    if ( i < 1 )
	i = 1;
    fontc.setPointSize(i);
    if ( recur > 0 )
	{
	    if( child1 != -1 )
		m_pDoc->Blocks[child1]->reduceFont(2);
	    if( child2 != -1 )
		m_pDoc->Blocks[child2]->reduceFont(2);
	    if( child3 != -1 )
		m_pDoc->Blocks[child3]->reduceFont(2);
	    if( ( recur == 2 ) && ( next != -1 ) )
		m_pDoc->Blocks[next]->reduceFont(2);
	}
}

void FormulaBlock::enlargeFont(int recur)
{
    int i;
    i = fontc.pointSize();
    fontc.setPointSize( ( i * 4 ) / 3 );
    if ( recur > 0 )
	{
	    if ( child1 != -1 )
		m_pDoc->Blocks[child1]->enlargeFont(2);
	    if ( child2 != -1 )
		m_pDoc->Blocks[child2]->enlargeFont(2);
	    if ( child3 != -1 )
		m_pDoc->Blocks[child3]->enlargeFont(2);
	    if( recur == 2 && next != -1 )
		m_pDoc->Blocks[next]->enlargeFont(2);
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

void FormulaBlock::PaintIt( QPainter *here, int x, int y, int corr )
{
    int sp1 = 0,X1 = 0,Y1 = 0,Y2 = 0,X2 = 0,X = 0;
    QString str;
    QString filename = kapp->kde_datadir().copy();
    filename += "/kformula/pics/";
    
    //QBitmap *mask;
    QWMatrix m;
    KIconLoader il;
    QBitmap pm;
    //    QBitmap bm;    
    QPixmap newpm;    
    QFontMetrics fm = QFontMetrics(fontc);
    where.setRect(x,y-lyu-1,tx,lyu+lyd+2);
    //if (type==0) where.setRect(x,y-lyu,tx+1,lyu+lyd);
    if( active && type != 0 )
	m_pDoc->setCursor(QRect(x-1,y-oyu-1,ox+2,oyu+oyd+2));
    here->setBackgroundColor(white);
    
    switch (type)
	{
	case (-1) :
	    if( active )
		here->setPen(red);
	    here->drawRect(x,y-5,10,10);
	    if( active )
		here->setPen(black);
	    if( next != -1 )
		m_pDoc->Blocks[next]->PaintIt(here,x+ox+FIXEDSPACE,y,corr);
	    break;
	case 0:
	    if( active )
		here->setPen(red);
	    if ( cont == "" )
		{
		    here->drawRect(x,y-5,10,10); type=-1;
		}
	    if( child1 != -1 ) 
		{
		    Y1 = m_pDoc->Blocks[child1]->getTY();
		    X1 = m_pDoc->Blocks[child1]->getTX();
		}
	    if( child2 != -1 )
		{
		    X2=m_pDoc->Blocks[child2]->getTX();
		    Y2=m_pDoc->Blocks[child2]->getTY();
		}
	    if ( X1 >= X2 )
		X2 = X1;  //X2 = x in piu'
	    here->setFont(fontc);
	    
	    here->drawText(x,y+sp,cont);  // add a -co to Y  
	    if( active )
		m_pDoc->setCursor(QRect(x+fm.width(cont,m_pDoc->getPos()),y-oyu,2,oyu+oyd));    
#ifdef RECT
	    here->drawRect(where);	
#endif
	    if( active )
		here->setPen(blue);
	    if( child1 != -1 )
		m_pDoc->Blocks[child1]->PaintIt(here,x+ox-X2,y-oyu+ m_pDoc->Blocks[child1]->getLYU(),corr);
	    if( active )
		here->setPen(blue);
	    if( child2 != -1 )
		m_pDoc->Blocks[child2]->PaintIt(here,x+ox-X2,y+oyd- m_pDoc->Blocks[child2]->getLYD(),corr);
	    if( active )
		here->setPen(black);
	    if( next != -1 )
		m_pDoc->Blocks[next]->PaintIt(here,x+ox+FIXEDSPACE,y,corr);
	    break;
	    
	    //////////////////////////////////////////////////////////////////////////
	case 1:
	    if( active )
		here->setPen(red);
	    
	    Y1 = m_pDoc->Blocks[child1]->getTY();
	    X1 = m_pDoc->Blocks[child1]->getTX();
	    Y2 = m_pDoc->Blocks[child1]->getLYU();
	    if( child2 != -1 ) 
		{
		    X2 = m_pDoc->Blocks[child2]->getTX();
		}
	    X = X2;
	    X2 -= Y1 / 4; 	
	    if ( X2 < 0 )
		X2 = 0;	
	    here->drawLine(x+X2,y-Y2+Y1/4,x+X2+Y1/4,y+2+m_pDoc->Blocks[child1]->getLYD());	
	    here->drawLine(x+X2,y-Y2+Y1/4-1,x+X2+Y1/4,y+1+m_pDoc->Blocks[child1]->getLYD());	
	    here->drawLine(x+X2+Y1/4,y-m_pDoc->Blocks[child1]->getLYU()-4,x+X2+Y1/4,
			   y+2+m_pDoc->Blocks[child1]->getLYD());	
	    
	    here->drawLine(x+X2+Y1/4,y-m_pDoc->Blocks[child1]->getLYU()-4,x+ox,y-m_pDoc->Blocks[child1]->getLYU()-4);	
#ifdef RECT  
	    here->drawRect(where);	
#endif
	    if( active )
		here->setPen(blue);
	    m_pDoc->Blocks[child1]->PaintIt(here,x+3+Y1/4+X2,y,corr);
	    if( active )
		here->setPen(blue);
	    if( child2 != -1 ) 
		m_pDoc->Blocks[child2]->PaintIt(here,x+1,y-oyu+
						m_pDoc->Blocks[child2]->getLYU(),corr);
	    if( active )
		here->setPen(black);
	    if( next != -1 )
		m_pDoc->Blocks[next]->PaintIt(here,x+ox+FIXEDSPACE,y,corr);
	    break;
	    ///////////////////////////////////////////////////////////////////////////   
	case 2:
	    if( active )
		here->setPen(red);
	    
	    Y2 = m_pDoc->Blocks[child1]->getLYU();
	    X = 0;
	    if( child2 != -1 )
		{ 
		    X = m_pDoc->Blocks[child2]->getTX();	
		    X1 = m_pDoc->Blocks[child2]->getLYU();	
		}
	    if( child3 != -1 )
		{
		    X2 = m_pDoc->Blocks[child3]->getTX();	
		    if ( X2 > X )
			X = X2;
		    X2 = m_pDoc->Blocks[child3]->getLYD();		
		}
	    Y1 = (oyu+oyd)/8;
	    if( Y1 < 3 )
		Y1 = 3;	
	    here->drawLine(x+Y1,y+oyd-Y1/2,x+Y1,y-oyu+Y1/2);
	    here->drawArc(x+Y1,y-oyu,Y1,Y1,0,2880);
	    here->drawArc(x+corr,y+oyd-Y1,Y1,Y1,2880,2880);     
#ifdef RECT  
	    here->drawRect(where);	
#endif
	    if( active )
		here->setPen(blue);
	    m_pDoc->Blocks[child1]->PaintIt(here,x+2*Y1+X,y,corr);
	    if( active )
		here->setPen(blue);
	    if( child2 != -1 )
		m_pDoc->Blocks[child2]->PaintIt(here,x+Y1+2,y-oyu+X1+Y1,corr);
	    if( child3 != -1 )
		m_pDoc->Blocks[child3]->PaintIt(here,x+Y1+2,y+oyd-X2-Y1,corr);
	    if( active )
		here->setPen(black);
	    if( next != -1 )
		m_pDoc->Blocks[next]->PaintIt(here,x+ox+FIXEDSPACE,y,corr);
	    
	    break;
	    ///////////////////////////////////////////////////////////////////////////   
	case 3:
	    
	    if( active )
		here->setPen(red);  
	    Y1 = m_pDoc->Blocks[child1]->getTY();
	    Y2 = m_pDoc->Blocks[child1]->getLYU();      
	    Y2 -= Y1 / 2;
	    Y1 = y - Y2;
	    Y2 = 0;
	    if ( child2 != -1 )
		Y2 = m_pDoc->Blocks[child2]->getTX();
	    X1 = ox - sp / 4 - Y2;
	    corr = 0;
#ifdef RECT
	    here->drawRect(where);	
#endif     
	    for ( X2 = 0; X2 <= 1; X2++ )
		{
		    X = X1*X2;
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
	    
	    if( active )
		here->setPen(blue);
	    m_pDoc->Blocks[child1]->PaintIt(here,x+sp/4,y,corr);	
	    if( child2 != -1 )
		m_pDoc->Blocks[child2]->PaintIt(here,x+ox-Y2,y-oyu+m_pDoc->Blocks[child2]->getLYU(),corr);  
	    if( active )
		here->setPen(black);
	    if( next != -1 )
		m_pDoc->Blocks[next]->PaintIt(here,x+ox+FIXEDSPACE,y,corr);      
	    break;
	    ///////////////////////////////////////////////////////////////////////////   
	case 4:
	    if( active )
		here->setPen(red);
	    if ( cont[0] == 'l' )
		here->drawLine(x,y,x+ox,y);
	    
	    X1 = m_pDoc->Blocks[child1]->getTX();    
	    Y1 = m_pDoc->Blocks[child1]->getLYD();
	    X2 = m_pDoc->Blocks[child2]->getTX();    
	    Y2 = m_pDoc->Blocks[child2]->getLYU();
	    if ( X1 > X2 )
		{
		    X2 = (X1-X2)/2;
		    X1 = 0;
		}
	    else
		{
		    X1 = ( X2 - X1 ) / 2;
		    X2 = 0;
		}
	    X = 2;  
	    if ( cont[0] == 'v' )
		X = X2 = X1 = 0;

#ifdef RECT
	    here->drawRect(where);	
#endif
	    if( active )
		here->setPen(blue);
	    m_pDoc->Blocks[child1]->PaintIt(here,x+X1+X,y-Y1-sp/2,corr);
	    if( active )
		here->setPen(blue);
	    m_pDoc->Blocks[child2]->PaintIt(here,x+X2+X,y+Y2+sp/2,corr);
	    if( active )
		here->setPen(black);
	    if( next != -1 )
		m_pDoc->Blocks[next]->PaintIt(here,x+ox+FIXEDSPACE,y,corr);      
	    break;
	    ///////////////////////////////////////////////////////////////////////////   
	case 5:  
	    if( active )
		here->setPen(red);   
	    switch( cont[0] )
		{
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
	    if( active )
		here->setPen(blue);
	    // warning("draw child1 of Symb");
	    if( child1 != -1 )
		m_pDoc->Blocks[child1]->PaintIt(here,x,y,corr);	
	    if( active )
		here->setPen(black);
	    if( next != -1 )
		m_pDoc->Blocks[next]->PaintIt(here,x+ox+FIXEDSPACE,y,corr);      
	    break;
	    ///////////////////////////////////////////////////////////////////////////         
	}
    //here->setPen(black);
}

int FormulaBlock::getClick(QPoint click)
{
    int retu=0;
    if ( where.contains( click ) ) 
	{
	    if( child1 != -1 )
		retu = m_pDoc->Blocks[child1]->getClick(click);
	    if( ( retu == 0 ) && ( child2 != -1 ) )
		retu = m_pDoc->Blocks[child2]->getClick(click);
	    if( /*(type!=4)&&*/ ( retu == 0 ) && ( child3 != -1 ) )
		retu = m_pDoc->Blocks[child3]->getClick(click);
	    if( ( retu == 0 ) && ( next != -1 ) )
		retu = m_pDoc->Blocks[next]->getClick(click);
	    if( retu == 0 )
		retu = id;
	} 
    return( retu );
}

/**
 * LYU: Larger  Y (Up)
 * OYU: Object  Y (Up)
 * LYD: Larger  Y (Down)
 * OYD: Object  Y (Down)
 * tx: Total X
 * ox:Object x
 * 
 *          _._._._._tx._._._._._._._   
 *       /\ |____ox______     |      |
 *   LYU || |/\_         |    |      | 
 *       \/ |\/ OYU      |    |      |
-midline----|            |----|      |
 *          |  OUR BLOCK |    |      |
 *      OYD=LYD          |    |______|
 *          |____________|._._._._._._
 *
 */


void FormulaBlock::getDimensions()
{
    int X1 = 0, X2 = 0;
    QFontMetrics fm(fontc);
    QRect di;

    if( getdime ) 
	switch ( type )
	    {
	    case ( -1 ):
		lyu = lyd = oyu = oyd = 5;
		tx = ox = 10;
		tx += FIXEDSPACE;
		sp = -1;
      		
		if ( next != -1 )
		    {
			m_pDoc->Blocks[next]->getDimensions();
			tx += m_pDoc->Blocks[next]->getTX(); 
			X1 = (m_pDoc->Blocks[next]->getLYU());
			X2 = (m_pDoc->Blocks[next]->getLYD());
			if( lyu < X1 )
			    lyu = X1;
			if( lyd < X2 )
			    lyd = X2;
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
		if(cont=="")
		    {
			lyu=lyd=oyu=oyd=5;
			tx=ox=10;
			tx+=FIXEDSPACE;
			sp=-1;
			type=-1;  
		    }
		if (oyu<1) oyu=1;      
		if (oyd<1) oyd=1;
		if (child1!=-1)
		    {
			m_pDoc->Blocks[child1]->getDimensions();
			X1 = m_pDoc->Blocks[child1]->getTX();    //get exponent X
			oyu += (m_pDoc->Blocks[child1]->getTY() );
		    }
		if ( child2 != -1 )
		    {
			m_pDoc->Blocks[child2]->getDimensions();
			X2 = m_pDoc->Blocks[child2]->getTX();    //get index X
			oyd += (m_pDoc->Blocks[child2]->getTY() );
		    }
		lyu = oyu;
		lyd = oyd;	
 
		if ( X1 >= X2 )
		    ox += X1;
		else
		    ox += X2;                      //add to OwnX the > x
		tx = ox;
		tx += FIXEDSPACE;
		if ( next != -1 )
		    {
			m_pDoc->Blocks[next]->getDimensions();
			tx += m_pDoc->Blocks[next]->getTX(); 
			X1 = (m_pDoc->Blocks[next]->getLYU());
			X2 = (m_pDoc->Blocks[next]->getLYD());
			if( lyu < X1 )
			    lyu = X1;
			if( lyd < X2 )
			    lyd = X2;
		    }
		break;

	    case 1:
		m_pDoc->Blocks[child1]->getDimensions();
		//       oyd=m_pDoc->Blocks[child1]->getTY()/2;
		//       oyu=oyd+4;
		oyd = m_pDoc->Blocks[child1]->getLYD();
		oyu = m_pDoc->Blocks[child1]->getLYU()+4;

		ox = m_pDoc->Blocks[child1]->getTX()+oyd/2;	
		ox += ( oyd + oyu ) / 4 + 1;
       
		if ( child2 != -1)
		    {
			m_pDoc->Blocks[child2]->getDimensions();
			X1 = m_pDoc->Blocks[child2]->getTY();
			X1 -= oyu/4;
			if ( X1 > 0 )
			    oyu += X1;
			X1 = m_pDoc->Blocks[child2]->getTX();
			X1 -= oyd / 4;
			if ( X1 > 0 )
			    ox += X1;
		    }
		lyu=oyu;
		lyd=oyd;	
		tx=ox;
		tx+=FIXEDSPACE;      
		if ( next != -1 )
		    {
			m_pDoc->Blocks[next]->getDimensions();
			tx+=m_pDoc->Blocks[next]->getTX(); 
			X1=(m_pDoc->Blocks[next]->getLYU());
			X2=(m_pDoc->Blocks[next]->getLYD());
			if( lyu < X1 )
			    lyu=X1;
			if( lyd < X2 )
			    lyd=X2;
		    }
		break;

	    case 2:
		m_pDoc->Blocks[child1]->getDimensions();
		oyd=m_pDoc->Blocks[child1]->getLYD();
		oyu=m_pDoc->Blocks[child1]->getLYU();
		ox=m_pDoc->Blocks[child1]->getTX();	
		X2=0;
		if ( child2 != -1 )
		    {
			m_pDoc->Blocks[child2]->getDimensions();
			oyu+=m_pDoc->Blocks[child2]->getTY();
			X1=m_pDoc->Blocks[child2]->getTX();
			X2=X1;
		    }

		if ( child3 != -1 )
		    {
			m_pDoc->Blocks[child3]->getDimensions();
			oyd+=m_pDoc->Blocks[child3]->getTY();
			X1=m_pDoc->Blocks[child3]->getTX();
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
		if ( next != -1 )
		    {
			m_pDoc->Blocks[next]->getDimensions();
			tx+=m_pDoc->Blocks[next]->getTX(); 
			X1=(m_pDoc->Blocks[next]->getLYU());
			X2=(m_pDoc->Blocks[next]->getLYD());
			if(lyu<X1) lyu=X1;
			if(lyd<X2) lyd=X2;
		    }
		break;

	    case 3:
		sp=0;
		m_pDoc->Blocks[child1]->getDimensions();
		ox=m_pDoc->Blocks[child1]->getTX();    
		X2=m_pDoc->Blocks[child1]->getTY();
		oyu=(m_pDoc->Blocks[child1]->getLYU());
		oyd=(m_pDoc->Blocks[child1]->getLYD());
		sp=(X2 / 5) * 5; // /16;    
		// if(sp*16<(X2)) sp++;
		sp+=4; 
		if ( sp < 20 )
		    sp = 20;
		X1=sp-X2;  //16*sp-X2;
		ox+=(sp/2);
		if ( child2 != -1 )
		    {
			m_pDoc->Blocks[child2]->getDimensions();
			ox+=m_pDoc->Blocks[child2]->getTX();    //get index X
			oyu+=(m_pDoc->Blocks[child2]->getTY() );
		    }
	
		lyu=oyu+=X1/2;
		lyd=oyd+=X1/2;
		tx=ox;
		tx+=FIXEDSPACE;
		if ( next != -1 )
		    {
			m_pDoc->Blocks[next]->getDimensions();
			tx+=m_pDoc->Blocks[next]->getTX(); 
			X1=(m_pDoc->Blocks[next]->getLYU());
			X2=(m_pDoc->Blocks[next]->getLYD());
			if( lyu < X1 )
			    lyu=X1;
			if( lyd < X2 )
			    lyd=X2;
		    }
		break;

	    case 4:
		//sp=child3;
		m_pDoc->Blocks[child1]->getDimensions();
		m_pDoc->Blocks[child2]->getDimensions();
		X1=m_pDoc->Blocks[child1]->getTX();    
		oyu=m_pDoc->Blocks[child1]->getTY()+sp/2;
		X2=m_pDoc->Blocks[child2]->getTX();    
		oyd=m_pDoc->Blocks[child2]->getTY()+sp/2;
		lyu=oyu;
		lyd=oyd;	
	
		if ( X1 >= X2 )
		    ox=X1+4;
		else
		    ox=X2+4;                      //add to OwnX the > x
		tx=ox;
		tx+=FIXEDSPACE;
		if ( next != -1 )
		    {
			m_pDoc->Blocks[next]->getDimensions();
			tx+=m_pDoc->Blocks[next]->getTX(); 
			X1=(m_pDoc->Blocks[next]->getLYU());
			X2=(m_pDoc->Blocks[next]->getLYD());
			if( lyu < X1 )
			    lyu=X1;
			if( lyd < X2 )
			    lyd=X2;
		    }
		break;

	    case 5:
		if ( child1 != -1 )
		    {       
			m_pDoc->Blocks[child1]->getDimensions();
			ox=m_pDoc->Blocks[child1]->getTX();    
			oyu=(m_pDoc->Blocks[child1]->getLYU());
			oyd=(m_pDoc->Blocks[child1]->getLYD());
		    } 
	
		switch ( cont[0] )
		    {
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
		if ( next != -1 )
		    {
			m_pDoc->Blocks[next]->getDimensions();
			tx+=m_pDoc->Blocks[next]->getTX(); 
			X1=(m_pDoc->Blocks[next]->getLYU());
			X2=(m_pDoc->Blocks[next]->getLYD());
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


#include "kformula_doc.moc"
