#include <qprinter.h>
#include "kformula_doc.h"
#include "kformula_shell.h"

#include "formuladef.h"
#include "BasicElement.h"
#include "TextElement.h"
#include "FractionElement.h"
#include "RootElement.h"
#include "BracketElement.h"
#include "MatrixElement.h"
#include "PrefixedElement.h"

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

//#define FIXEDSPACE 1             // Space between 2 blocks
#define DEFAULT_FONT_SIZE 24

KFormulaDoc::KFormulaDoc()
{
//    setFocusPolicy( ClickFocus );
    eList.clear();
    eList.setAutoDelete(TRUE);
    thePosition=0;
    thePainter = new QPainter();
    warning("General Settings");
    theFont.setFamily( "utopia" );
    theFont.setPointSize(DEFAULT_FONT_SIZE);  
    theFont.setWeight( QFont::Normal );
    theFont.setItalic( false );
    theColor=black;
    warning("General Font OK");
//    theActiveElement = 0;

    // Use CORBA mechanism for deleting views
    m_lstViews.setAutoDelete( false );

    m_bModified = false;
    m_bEmpty = true;

    theFirstElement= new BasicElement(this,0L,-1,0L,"");
    PosType *p=new PosType;
    p->element=theFirstElement;
    p->pos=0;
    eList.append(p);
    p= new PosType;
    p->element=theFirstElement;
    p->pos=-1;
    eList.append(p);    
    eList.at(0);
    thePosition=0;

    warning("SIZE OF:  basic:%i,text:%i,root:%i",sizeof(BasicElement),
	    sizeof(TextElement),sizeof(RootElement));
}

CORBA::Boolean KFormulaDoc::init()
{
    return true;
}

KFormulaDoc::~KFormulaDoc()
{
    cleanUp();
}

void KFormulaDoc::cleanUp()
{
    if ( m_bIsClean )
	return;

    assert( m_lstViews.count() == 0 );

    KoDocument::cleanUp();
}

bool KFormulaDoc::loadXML( KOMLParser& parser, KOStore::Store_ptr _store )
{
    // HACK
    return false;
}

bool KFormulaDoc::save( ostream& out, const char* /* format */ )
{
    out << "<?xml version=\"1.0\"?>" << endl;
    out << otag << "<DOC author=\"" << "Andrea Rizzi" << "\" email=\""
	<< "rizzi@kde.org" << "\" editor=\"" << "KFormula"
	<< "\" mime=\"" << MIME_TYPE << "\" >" << endl;
    
    out << "<FORMULA>" << endl;

    out << "<ELEM FIRST ";
    theFirstElement->save(out);
    
    out << "</FORMULA>" << endl;    
    out << etag << "</DOC>" << endl;
 
    return true;
}

KOffice::MainWindow_ptr KFormulaDoc::createMainWindow()
{
  KFormulaShell* shell = new KFormulaShell;
  shell->show();
  shell->setDocument( this );

  return KOffice::MainWindow::_duplicate( shell->koInterface() );
}

int KFormulaDoc::viewCount()
{
  return m_lstViews.count();
}

void KFormulaDoc::viewList( OpenParts::Document::ViewList*& _list )
{
    (*_list).length( m_lstViews.count() );

    int i = 0;
    QListIterator<KFormulaView> it( m_lstViews );
    for( ; it.current(); ++it )
	{
	    (*_list)[i++] = OpenParts::View::_duplicate( it.current() );
	}
}

void KFormulaDoc::addView( KFormulaView *_view )
{
    m_lstViews.append( _view );
}

void KFormulaDoc::removeView( KFormulaView *_view )
{
    m_lstViews.setAutoDelete( false );
    m_lstViews.removeRef( _view );
    m_lstViews.setAutoDelete( true );
}

KFormulaView* KFormulaDoc::createFormulaView()
{
  KFormulaView *p = new KFormulaView( 0L, 0L, this );
  //p->QWidget::show();
  m_lstViews.append( p );

  return p;
}

OpenParts::View_ptr KFormulaDoc::createView()
{
  return OpenParts::View::_duplicate( createFormulaView() );
}

void KFormulaDoc::emitModified()
{
  m_bModified = true;
  m_bEmpty = false;
  eList.clear();
  theFirstElement->makeList();  
  eList.at(thePosition);
  emit sig_changeType( eList.current()->element );
  emit sig_modified();
}

void KFormulaDoc::addRootElement()
{
    BasicElement *newElement;
     if(eList.current()->element==0L)
     setActiveElement(theFirstElement);

    if (typeid(*eList.current()->element) == typeid(BasicElement)) {
	warning("substitueted");
	newElement = new RootElement(this);
	eList.current()->element->substituteElement(newElement);
	delete	eList.current()->element;
        eList.current()->element=0;
	warning("done");
    } else {
	BasicElement *nextElement=eList.current()->element->getNext();
	if(nextElement!=0L){       //If there's a next insert root before next
	    nextElement->insertElement(newElement=new RootElement(this));
	} else  //If there isn't a next append only.
	eList.current()->element->setNext(newElement=new RootElement(this,eList.current()->element));
    }
     newElement->check();
//    warning("Set activeelement...");
    setActiveElement(newElement);
//    warning("done");
    //RootElement need a child[0] i.e. root content
//    newElement = new BasicElement(this,eList.current()->element,4);
//    eList.current()->element->setChild(newElement,0);
    
    setActiveElement(newElement->getChild(0)); //I prefere to AutoActivate RootContent
    
    emitModified();
}

void KFormulaDoc::addPrefixedElement(QString cont)
{
    BasicElement *newElement;
//     if(eList.at()==-1)
//         eList.first();
    if(eList.current()->element==0L)
	setActiveElement(theFirstElement);


    //If current Element is a Basic, change it into a Prefixed
    if (typeid(*eList.current()->element) == typeid(BasicElement)) {
	warning("substitueted");
	newElement = new PrefixedElement(this);
	eList.current()->element->substituteElement(newElement);
	delete 	eList.current()->element;
        eList.current()->element=0;
	//theActiveElement = 0;
    } else {
	BasicElement *nextElement=eList.current()->element->getNext();
	if(nextElement!=0L){       //If there's a next insert root before next
	    nextElement->insertElement(newElement=new PrefixedElement(this));
	} else  //If there isn't a next append only.
	    eList.current()->element->setNext(newElement=new PrefixedElement(this,eList.current()->element));
    }
    setActiveElement(newElement);
    eList.current()->element->setContent(cont);
    newElement = new BasicElement(this,eList.current()->element,4);
    eList.current()->element->setChild(newElement,0);
    setActiveElement(newElement); //I prefere to AutoActivate RootContent
    emitModified();
}

void KFormulaDoc::addFractionElement(QString cont)
{
    BasicElement *nextElement;
    BasicElement *newElement;
    if(eList.current()->element==0L)
	setActiveElement(theFirstElement);

    if(typeid(*eList.current()->element) == typeid(BasicElement))  //If current Element is a Basic
	{					//It change it into a Root
	    eList.current()->element->substituteElement(newElement = new FractionElement(this));
	    delete eList.current()->element;
	    eList.current()->element = 0;
	}
    else
	{
	    nextElement=eList.current()->element->getNext();
	    if(nextElement!=0L){       //If there's a next insert root before next
		nextElement->insertElement(newElement=new FractionElement(this));
	    }
	    else              //If there isn't a next append only.
		eList.current()->element->setNext(newElement=new FractionElement(this,eList.current()->element));
	}
    newElement->setContent(cont);	
    setActiveElement(newElement);
    //RootElement need a child[0] i.e. numer
    eList.current()->element->setChild(newElement = new BasicElement(this,eList.current()->element,5),1);	
    eList.current()->element->setChild(newElement = new BasicElement(this,eList.current()->element,4),0);	
    setActiveElement(newElement); //I prefere to AutoActivate numerator
    emitModified();

}

void KFormulaDoc::addMatrixElement(QString cont)
{
    int rows=atoi(cont.mid(3,3));
    int cols=atoi(cont.mid(6,3));

    BasicElement *nextElement;
    BasicElement *newElement;
    if(eList.current()->element==0L)
	setActiveElement(theFirstElement);

    if(typeid(*eList.current()->element) == typeid(BasicElement))  //If current Element is a Basic
	{					//It change it into a Root
	    eList.current()->element->substituteElement(newElement = new MatrixElement(this));
	    delete eList.current()->element;
	    eList.current()->element = 0;
	}
    else
	{
	    nextElement=eList.current()->element->getNext();
	    if(nextElement!=0L){       //If there's a next insert root before next
		nextElement->insertElement(newElement=new MatrixElement(this));
	    }
	    else              //If there isn't a next append only.
		eList.current()->element->setNext(newElement=new MatrixElement(this,eList.current()->element));
	}
    newElement->setContent(cont);	
    setActiveElement(newElement);
    //RootElement need a child[0] i.e. numer
    ((MatrixElement *)(eList.current()->element))->setChildrenNumber(rows*cols);
    for (int i=rows*cols-1;i>=0;i--)
	eList.current()->element->setChild(newElement = new BasicElement(this,eList.current()->element,i+4),i);	
    setActiveElement(newElement); //I prefere to AutoActivate numerator
    emitModified();

}

void KFormulaDoc::addBracketElement(QString cont)
{
    BasicElement *nextElement;
    BasicElement *newElement;
    if(eList.current()->element==0L)
	setActiveElement(theFirstElement);

    if(typeid(*eList.current()->element) == typeid(BasicElement))  //If current Element is a Basic
	{					//It change it into a Bracket
	    eList.current()->element->substituteElement(newElement = new BracketElement(this));
	    delete eList.current()->element;
                eList.current()->element=0;
	}
    else
	{
	    nextElement=eList.current()->element->getNext();
	    if(nextElement!=0L){       //If there's a next insert  Brackets before next
		nextElement->insertElement(newElement=new BracketElement(this));
	    }
	    else              //If there isn't a next append only.
		activeElement()->setNext(newElement=new BracketElement(this,eList.current()->element));
	}
    newElement->setContent(cont);	
    setActiveElement(newElement);
    //RootElement need a child[0] i.e. parenthesis content
    eList.current()->element->setChild(newElement = new BasicElement(this,eList.current()->element,4),0);	
    setActiveElement(newElement); //I prefere to AutoActivate RootContent
    emitModified();
}

/*
 * If activeElement is a NON-BasicElement it add
 *   a TextElement between activeElement and its next element
 * If activeElement is a BasicElement it "substitute"
 *  this basicElement with a TextElement
 */
BasicElement * KFormulaDoc::addIndex(int index)
{
    BasicElement *oldIndexElement;
    BasicElement *newElement;
    if(eList.current()->element==0L)
	setActiveElement(theFirstElement);

    oldIndexElement=eList.current()->element->getIndex(index);
    if(oldIndexElement==0L)
	eList.current()->element->setIndex(newElement =
				   new BasicElement(this,eList.current()->element,index),index);
    else
	{
	    oldIndexElement->insertElement(newElement = new BasicElement(this));
	}
    newElement->scaleNumericFont(FN_REDUCE | FN_P43 | FN_ELEMENT);	
    setActiveElement(newElement);
    emitModified();
    return(newElement);
}
BasicElement * KFormulaDoc::addChild(int child)
{
    BasicElement *oldChildElement;
    BasicElement *newElement;
    if(eList.current()->element==0L)
	setActiveElement(theFirstElement);

    oldChildElement=eList.current()->element->getChild(child);
    if(oldChildElement==0L)
	eList.current()->element->setChild(newElement =
				   new BasicElement(this,eList.current()->element,child+4),child);
    else
	{
	    oldChildElement->insertElement(newElement = new BasicElement(this));
	}
    setActiveElement(newElement);
    emitModified();
    return(newElement);
}
void KFormulaDoc::addTextElement(QString cont="")
{
    BasicElement *nextElement;
    BasicElement *newElement;
    if(eList.current()->element==0L)
	setActiveElement(theFirstElement);

    if(typeid(*eList.current()->element) == typeid(BasicElement))  //see addRootElement()
	{
	    eList.current()->element->substituteElement(newElement = new TextElement(this));
	    delete eList.current()->element;
	        eList.current()->element=0;
	}
    else
	{
	    nextElement=eList.current()->element->getNext();
	    if(nextElement!=0L){
		nextElement->insertElement(newElement=new TextElement(this));
	    }else
		eList.current()->element->setNext(newElement=new TextElement(this,eList.current()->element));
	}
    newElement->setContent(cont);	
    setActiveElement(newElement);
    emitModified();
}

void KFormulaDoc::mousePressEvent( QMouseEvent *a,QWidget *wid)
{

    setActiveElement(theFirstElement->isInside(a->pos()));
/*    if(eList.current()->element!=0)
     eList.current()->element->setPosition(-2);
*/    emitModified();
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

void KFormulaDoc::keyPressEvent( QKeyEvent *k )
{
cerr << " key received , " << k->ascii() << endl;
warning("Key pressed %i, ascii:%i",k->key(),k->ascii());

    int elReturn=0;

    if((k->ascii()>=32)&&(k->ascii()<127))
	{
	 switch(k->ascii())
	    {
	      case '[':
		addBracketElement("[]");
              break;
	      case '(':
		addBracketElement(DEFAULT_DELIMITER);
              break;
	      case '|':
		addBracketElement("||");
              break;
	      case '/':
		addFractionElement(DEFAULT_FRACTION);
              break;
	      case '@':
		addRootElement();
              break;
	      case '^':
		addIndex(IN_TOPRIGHT);
              break;
	      case '_':
		addIndex(IN_BOTTOMRIGHT);
              break;	      
	     default:
	     {

	       if (eList.current()->element!=0L)
	         {
	          int po=eList.current()->pos;
	           if (po>0)
	           {
		    QString text;
		    po--;
		    warning("Internal Text Position %d",po);
		    text=eList.current()->element->getContent().copy();
		    text.insert(po,k->ascii());
		    eList.current()->element->setContent(text);
		    thePosition=eList.at();
	    	    warning("thePosition %d  int:%d",thePosition,eList.current()->pos);
		    eList.clear();
            	    theFirstElement->makeList();
		    thePosition++;              
		    eList.at(thePosition);		
	    	    warning("the New Position %d int:%d",thePosition,eList.current()->pos);
	           }
		   else
		    elReturn=FCOM_ADDTEXT;
	         }
	     } //default
	}  //Switch
      }	 //if
    else   //Not ascii
	{
	    int action=k->key();
	       if (eList.current()->element!=0L)
	         {
	          int po=eList.current()->pos;
	           if (po>0)
	           {
        

	    	   }
		  }

if(action==Qt::Key_BackSpace)
{
 if(eList.prev()->element!=0)
  action=Qt::Key_Delete;
 else
   eList.next(); 
}  

if(action==Qt::Key_Delete)
 {
    warning("Key Delete");
   if(eList.current()->pos>=0)
    {
    QString text;
    text=eList.current()->element->getContent().copy();
    text.remove(eList.current()->pos-1,1);
    eList.current()->element->setContent(text);
    thePosition=eList.at();
    eList.clear();
    theFirstElement->makeList();
    eList.at(thePosition);		
   }

    
    else {
    if(eList.current()->pos==0)
     elReturn=FCOM_DELETEME;
    else 
     if(eList.next()->element!=0)
      elReturn=FCOM_DELETEME;       	    
    else
    eList.prev();
   } 
 }


if(action==Qt::Key_Left)
 {

/*   if(eList.current()->pos==0) 
   {
    if(eList.prev()->pos==-1)
       eList.prev();
   }
    else*/
       eList.prev();

 }
if(action==Qt::Key_Right)
 {/*
   if(eList.current()->pos==-1) 
   {
    if(eList.next()->pos==0)
       eList.next();
  
   }
    else
    */   eList.next();

  }



	    warning("Not Ascii return %d",elReturn);
	    thePosition=eList.at();
	}	
        
	if (elReturn==FCOM_ADDTEXT) 
 	    { 
	     if(eList.current()->pos==0)
	      {
	          BasicElement *newElement;
	         eList.current()->element->insertElement(newElement=new TextElement(this));
	          
		  setActiveElement(newElement); 
	      } else  addTextElement();
	     
	     if((k->ascii()>32)&&(k->ascii()<127))
	    {
	       QString text;
		    int po=0;
	       
	       warning("Internal Text Position %d",po);
		    text=eList.current()->element->getContent().copy();
		    text.insert(po,k->ascii());
		    eList.current()->element->setContent(text);
		    thePosition=eList.at();
	    	    warning("thePosition %d  int:%d",thePosition,eList.current()->pos);
		    eList.clear();
            	    theFirstElement->makeList();  
            	    thePosition++;      
		    eList.at(thePosition);		
	    	    warning("the New Position %d int:%d",thePosition,eList.current()->pos);
	           
	    }

	    }
	if (elReturn==FCOM_DELETEME)
	    {

		 BasicElement *newActive;
		 BasicElement *prev;
	         newActive=eList.current()->element->getNext();
		 prev=eList.current()->element->getPrev();
		 eList.current()->element->deleteElement();
		 if(prev!=0)
		   prev->check();
		 setActiveElement(newActive);

	    }

    emitModified();
}


void KFormulaDoc::paintEvent( QPaintEvent *, QWidget *paintGround )
{
/*		    //paintGround->size()    
    QPixmap pm(100,100);  //double buffer
    pm.fill();
    QPainter q;
    thePainter=&q;
*/
    thePainter->begin(paintGround);
    thePainter->setPen( black );
    theFirstElement->checkSize();
    if(eList.current()->element!=0L)
      eList.current()->element->setActive(true);
    theFirstElement->draw(QPoint(5,5)-theFirstElement->getSize().topLeft());
//    if(eList.current()->element && typeid(*eList.current()->element) == typeid(TextElement))
//	{
	 //thePainter->drawWinFocusRect(theCursor);
	 theCursor=eList.current()->element->getCursor(eList.current()->pos);
	 thePainter->drawLine(theCursor.topLeft()+QPoint(0,1),theCursor.topRight()+QPoint(0,1));
	 thePainter->drawLine(theCursor.bottomLeft()-QPoint(0,1),theCursor.bottomRight()-QPoint(0,1));
	 thePainter->drawLine(theCursor.topLeft()+QPoint(theCursor.width()/2,1),
	                     theCursor.bottomLeft()+QPoint(theCursor.width()/2,-1));	 
//	}
    thePainter->end();
//    bitBlt(paintGround,0,0,&pm,0,0,-1,-1);
}

void KFormulaDoc::print( QPrinter *thePrt)
{
    setActiveElement(0L);
    thePainter->begin(thePrt);
    thePainter->setPen( black );
    theFirstElement->checkSize();
    theFirstElement->draw(QPoint(0,0)-theFirstElement->getSize().topLeft());
    thePainter->end();
}

void KFormulaDoc::setActiveElement(BasicElement* c)
{
    warning("Set active element old %p  new %p",eList.current()->element,c);

    eList.clear();
    thePosition=-1;
    theFirstElement->makeList();
    for(eList.first();eList.current()!=0;eList.next())
     if(eList.current()->element==c)
       thePosition=eList.at();
    eList.at(thePosition);
    
/*    if(eList.current()->element)
	eList.current()->element->setActive(false);
    if (c!=0)
	c->setActive(true);
    eList.clear();
    warning("clear list");
    thePosition=-1;
    theFirstElement->makeList(1); 
    eList.at(thePosition);*/	
warning("New Active Element  %p",c);
}

void KFormulaDoc::setFirstElement(BasicElement* c)
{

    if (c)
	theFirstElement = c;
    else
	warning("Try to set first element to 0L");
}


#include "kformula_doc.moc"
