#include "kformula_doc.h"
#include "formuladef.h"
#include "BasicElement.h"
#include "TextElement.h"
#include "RootElement.h"

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
    theFirstElement= new TextElement(this,0L,-1,0L,"First");
    setActiveElement(0L);
    warning("SIZE OF:  basic:%i,text:%i,root:%i",sizeof(BasicElement),
		    sizeof(TextElement),sizeof(RootElement));
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
    out << otag << "<DOC author=\"" << "Andrea Rizzi" << "\" email=\"" 
	<< "rizzi@kde.org" << "\" editor=\"" << "KFormula" 
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

/*
void KFormulaDocument::setGreek( bool _on )
{
    if ( _on ) 
	Blocks[getCurrent()]->fontc.setFamily("Symbol");	
    else 
	Blocks[getCurrent()]->fontc.setFamily("utopia");

    emit sig_modified();
}*/

/*
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
*/
/*
void KFormulaDocument::pro()
{   
    // HACK
    
      new ProData(Blocks[getCurrent()],this);  //proprieties
      checkAndCreate(Blocks[getCurrent()]);
      ChText(Blocks[getCurrent()]->getcont());
      ChType(Blocks[getCurrent()]->gettype());
      setFocus(); 
      update();          
}*/


void KFormulaDocument::addRootElement()
{   
  BasicElement *nextElement;
  BasicElement *newElement;

        if(theActiveElement->type()==EL_BASIC)  //If current Element is a Basic
         {					//It change it into a Root
	  theActiveElement->substituteElement(newElement = new RootElement(this));
	  delete theActiveElement;
	 }         
        else 
	 {
	 nextElement=theActiveElement->getNext();
	 if(nextElement!=0L){       //If there's a next insert root before next
	  nextElement->insertElement(newElement=new RootElement(this));
	  }
	  else              //If there isn't a next append only.
 	   activeElement()->setNext(newElement=new RootElement(this,theActiveElement));
	 }
    setActiveElement(newElement);  
    //RootElement need a child[0] i.e. root content
    theActiveElement->setChild(newElement = new BasicElement(this,theActiveElement,4),0);	 
    setActiveElement(newElement); //I prefere to AutoActivate RootContent 
    emit sig_modified();
}
/*
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
*/

/*
 * If activeElement is a NON-BasicElement it add
 *   a TextElement between activeElement and its next element
 * If activeElement is a BasicElement it "substitute"
 *  this basicElement with a TextElement
 */
void KFormulaDocument::addIndex(int index)
{
  BasicElement *oldIndexElement;
  BasicElement *newElement;
   if(theActiveElement==0L)
     setActiveElement(theFirstElement);
  
  oldIndexElement=theActiveElement->getIndex(index);
  if(oldIndexElement==0L)        
    theActiveElement->setIndex(newElement = 
		new BasicElement(this,theActiveElement,index),index);
  else
   {
    oldIndexElement->insertElement(newElement = new BasicElement(this));
   }
    setActiveElement(newElement);
    emit sig_modified();
} 

void KFormulaDocument::addTextElement()
{   
BasicElement *nextElement;
BasicElement *newElement;
  if(theActiveElement==0L)
   setActiveElement(theFirstElement); 

        if(theActiveElement->type()==EL_BASIC)  //see addRootElement()
         {
	  theActiveElement->substituteElement(newElement = new TextElement(this));
	  delete theActiveElement;
	 }         
        else 
	 {
	 nextElement=theActiveElement->getNext();
	 if(nextElement!=0L){
	  nextElement->insertElement(newElement=new TextElement(this));
	  }else
	   activeElement()->setNext(newElement=new TextElement(this,theActiveElement));
	 }
    setActiveElement(newElement);
    emit sig_modified();
}
/*
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

*/

void KFormulaDocument::mousePressEvent( QMouseEvent *a,QWidget *wid)
{
 
   setActiveElement(theFirstElement->isInside(a->pos()));  
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
    mousepopup->insertItem(i18n("Properties"), this,SLOT(pro()), ALT+Key_E);
    mousepopup->insertItem(i18n("Delete"), this,SLOT(dele()), ALT+Key_E);
    mousepopup->insertSeparator();
    mousepopup->insertItem(i18n("Change font"),fontpopup);    
    mousepopup->insertItem(i18n("Export as"),convert);
//    mousepopup->insertSeparator();
//    mousepopup->insertItem(i18n("Quit"), _part, SLOT(slotQuit()), ALT+Key_Q);
    mousepopup->popup(wid->mapToGlobal(a->pos()));
 }
}

void KFormulaDocument::keyPressEvent( QKeyEvent *k )
{

/*
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
 */
 int elReturn=0;
 if((k->ascii()>32)&&(k->ascii()<127))
  { 
   if (theActiveElement!=0L) 
     elReturn=theActiveElement->takeAsciiFromKeyb(k->ascii());
  }
    else   //Not ascii
  {
   if (theActiveElement!=0L) 
     elReturn=theActiveElement->takeActionFromKeyb(k->key());  
  }	

    if (elReturn==FCOM_TEXTCLONE)
      {
          BasicElement *newElement;
	  theActiveElement->substituteElement(newElement = new TextElement(this));
	  BasicElement *b;
	  b=theActiveElement;
	  setActiveElement(newElement);
	  warning("delete %p",b);
	  delete b;
      }

//ChText(Blocks[getCurrent()]->getcont());
//ChType(Blocks[getCurrent()]->gettype());
//update();
 emit sig_modified();
}


void KFormulaDocument::paintEvent( QPaintEvent *_ev, QWidget *paintGround )
{
    
    thePainter->begin(paintGround);    
    thePainter->setPen( black );
    theFirstElement->checkSize();
    theFirstElement->draw(QPoint(0,0)-theFirstElement->getSize().topLeft());
    if(theActiveElement!=0L)
     if(theActiveElement->type()==EL_TEXT)
      thePainter->drawWinFocusRect(theCursor);
    thePainter->end();
}

#include "kformula_doc.moc"
