#include "kformula_doc.h"
#include "formuladef.h"
#include "BasicElement.h"
#include "TextElement.h"
#include "FractionElement.h"
#include "RootElement.h"
#include "BracketElement.h"
#include "MatrixElement.h"

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
    thePainter = new QPainter();
    
    warning("General Settings");
    theFont.setFamily( "utopia" );
    theFont.setPointSize(32);
    theFont.setWeight( QFont::Normal );
    theFont.setItalic( false ); 
    theColor=black;
    warning("General Font OK");
    theActiveElement = 0;

    // Use CORBA mechanism for deleting views
    m_lstViews.setAutoDelete( false );
    m_bModified = false;
    theFirstElement= new BasicElement(this,0L,-1,0L,"");
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

bool KFormulaDocument::load( KOMLParser&  )
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
    emit sig_changeType(theActiveElement);
    emit sig_modified();
}

void KFormulaDocument::addRootElement()
{   
    BasicElement *newElement;
    if(theActiveElement==0L)
	setActiveElement(theFirstElement);
  
    //If current Element is a Basic, change it into a Root
    if (typeid(*theActiveElement) == typeid(BasicElement)) {
	warning("substitueted");
	newElement = new RootElement(this);
	theActiveElement->substituteElement(newElement);
	delete theActiveElement;
	theActiveElement = 0;
    } else {
	BasicElement *nextElement=theActiveElement->getNext();
	if(nextElement!=0L){       //If there's a next insert root before next
	    nextElement->insertElement(newElement=new RootElement(this));
	} else  //If there isn't a next append only.
	    activeElement()->setNext(newElement=new RootElement(this,theActiveElement));
    }
    setActiveElement(newElement);  
    //RootElement need a child[0] i.e. root content
    newElement = new BasicElement(this,theActiveElement,4);
    theActiveElement->setChild(newElement,0);
    setActiveElement(newElement); //I prefere to AutoActivate RootContent 
    emitModified();
}

void KFormulaDocument::addFractionElement(QString cont)
{   
    BasicElement *nextElement;
    BasicElement *newElement;
    if(theActiveElement==0L)
	setActiveElement(theFirstElement);
  
    if(typeid(*theActiveElement) == typeid(BasicElement))  //If current Element is a Basic
	{					//It change it into a Root
	    theActiveElement->substituteElement(newElement = new FractionElement(this));
	    delete theActiveElement;
	    theActiveElement = 0;
	}         
    else 
	{
	    nextElement=theActiveElement->getNext();
	    if(nextElement!=0L){       //If there's a next insert root before next
		nextElement->insertElement(newElement=new FractionElement(this));
	    }
	    else              //If there isn't a next append only.
		activeElement()->setNext(newElement=new FractionElement(this,theActiveElement));
	}
    newElement->setContent(cont);	 
    setActiveElement(newElement);  
    //RootElement need a child[0] i.e. numer
    theActiveElement->setChild(newElement = new BasicElement(this,theActiveElement,5),1);	 
    theActiveElement->setChild(newElement = new BasicElement(this,theActiveElement,4),0);	 
    setActiveElement(newElement); //I prefere to AutoActivate numerator 
    emitModified();
    
}

void KFormulaDocument::addMatrixElement(QString cont)
{   
    int rows=atoi(cont.mid(3,3));
    int cols=atoi(cont.mid(6,3));

    BasicElement *nextElement;
    BasicElement *newElement;
    if(theActiveElement==0L)
	setActiveElement(theFirstElement);
  
    if(typeid(*theActiveElement) == typeid(BasicElement))  //If current Element is a Basic
	{					//It change it into a Root
	    theActiveElement->substituteElement(newElement = new MatrixElement(this));
	    delete theActiveElement;
	    theActiveElement = 0;
	}         
    else 
	{
	    nextElement=theActiveElement->getNext();
	    if(nextElement!=0L){       //If there's a next insert root before next
		nextElement->insertElement(newElement=new MatrixElement(this));
	    }
	    else              //If there isn't a next append only.
		activeElement()->setNext(newElement=new MatrixElement(this,theActiveElement));
	}
    newElement->setContent(cont);	 
    setActiveElement(newElement);  
    //RootElement need a child[0] i.e. numer
    ((MatrixElement *)(theActiveElement))->setChildrenNumber(rows*cols);
    for (int i=rows*cols-1;i>=0;i--)
	theActiveElement->setChild(newElement = new BasicElement(this,theActiveElement,i+4),i);	 
    setActiveElement(newElement); //I prefere to AutoActivate numerator 
    emitModified();
    
}

void KFormulaDocument::addBracketElement(QString cont)
{   
    BasicElement *nextElement;
    BasicElement *newElement;
    if(theActiveElement==0L)
	setActiveElement(theFirstElement);
  
    if(typeid(*theActiveElement) == typeid(BasicElement))  //If current Element is a Basic
	{					//It change it into a Bracket
	    theActiveElement->substituteElement(newElement = new BracketElement(this));
	    delete theActiveElement;
	}         
    else 
	{
	    nextElement=theActiveElement->getNext();
	    if(nextElement!=0L){       //If there's a next insert  Brackets before next
		nextElement->insertElement(newElement=new BracketElement(this));
	    }
	    else              //If there isn't a next append only.
		activeElement()->setNext(newElement=new BracketElement(this,theActiveElement));
	} 
    newElement->setContent(cont);	 
    setActiveElement(newElement);  
    //RootElement need a child[0] i.e. parenthesis content
    theActiveElement->setChild(newElement = new BasicElement(this,theActiveElement,4),0);	 
    setActiveElement(newElement); //I prefere to AutoActivate RootContent 
    emitModified();
}

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
    emitModified();
} 

void KFormulaDocument::addTextElement()
{   
    BasicElement *nextElement;
    BasicElement *newElement;
    if(theActiveElement==0L)
	setActiveElement(theFirstElement); 

    if(typeid(*theActiveElement) == typeid(BasicElement))  //see addRootElement()
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
    emitModified();
}

void KFormulaDocument::mousePressEvent( QMouseEvent *a,QWidget *wid)
{
 
    setActiveElement(theFirstElement->isInside(a->pos()));  
    emitModified();
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
	    if((k->ascii()>32)&&(k->ascii()<127))
		theActiveElement->takeAsciiFromKeyb(k->ascii());
	    warning("delete %p",b);
	    delete b;
	}

    //ChText(Blocks[getCurrent()]->getcont());
    //ChType(Blocks[getCurrent()]->gettype());
    //update();
    emitModified();
}


void KFormulaDocument::paintEvent( QPaintEvent *, QWidget *paintGround )
{
    
    thePainter->begin(paintGround);    
    thePainter->setPen( black );
    theFirstElement->checkSize();
    theFirstElement->draw(QPoint(0,0)-theFirstElement->getSize().topLeft());
    if(theActiveElement && typeid(*theActiveElement) == typeid(TextElement))
	thePainter->drawWinFocusRect(theCursor);
    thePainter->end();
}

void KFormulaDocument::setActiveElement(BasicElement* c)
{ 
    if(theActiveElement) 
	theActiveElement->setActive(false);
    theActiveElement = c;
    if (theActiveElement)
	theActiveElement->setActive(true);
}

void KFormulaDocument::setFirstElement(BasicElement* c)
{ 
  
    if (c) 
	theFirstElement = c;
    else
	warning("Try to set first element to 0L");
}


#include "kformula_doc.moc"
