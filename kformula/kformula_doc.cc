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
    thePainter = new QPainter();
    warning("General Settings");
    theFont.setFamily( "utopia" );
    theFont.setPointSize(DEFAULT_FONT_SIZE);  
    theFont.setWeight( QFont::Normal );
    theFont.setItalic( false );
    theColor=black;
    warning("General Font OK");
    theActiveElement = 0;

    // Use CORBA mechanism for deleting views
    m_lstViews.setAutoDelete( false );

    m_bModified = false;
    m_bEmpty = true;

    theFirstElement= new BasicElement(this,0L,-1,0L,"");
    setActiveElement(0L);
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

  emit sig_changeType( theActiveElement );
  emit sig_modified();
}

void KFormulaDoc::addRootElement()
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

void KFormulaDoc::addPrefixedElement(QString cont)
{
    BasicElement *newElement;
    if(theActiveElement==0L)
	setActiveElement(theFirstElement);

    //If current Element is a Basic, change it into a Prefixed
    if (typeid(*theActiveElement) == typeid(BasicElement)) {
	warning("substitueted");
	newElement = new PrefixedElement(this);
	theActiveElement->substituteElement(newElement);
	delete theActiveElement;
	theActiveElement = 0;
    } else {
	BasicElement *nextElement=theActiveElement->getNext();
	if(nextElement!=0L){       //If there's a next insert root before next
	    nextElement->insertElement(newElement=new PrefixedElement(this));
	} else  //If there isn't a next append only.
	    activeElement()->setNext(newElement=new PrefixedElement(this,theActiveElement));
    }
    setActiveElement(newElement);
    theActiveElement->setContent(cont);
    newElement = new BasicElement(this,theActiveElement,4);
    theActiveElement->setChild(newElement,0);
    setActiveElement(newElement); //I prefere to AutoActivate RootContent
    emitModified();
}

void KFormulaDoc::addFractionElement(QString cont)
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

void KFormulaDoc::addMatrixElement(QString cont)
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

void KFormulaDoc::addBracketElement(QString cont)
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
BasicElement * KFormulaDoc::addIndex(int index)
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
    newElement->scaleNumericFont(FN_REDUCE | FN_P43 | FN_ELEMENT);	
    setActiveElement(newElement);
    emitModified();
    return(newElement);
}
BasicElement * KFormulaDoc::addChild(int child)
{
    BasicElement *oldChildElement;
    BasicElement *newElement;
    if(theActiveElement==0L)
	setActiveElement(theFirstElement);

    oldChildElement=theActiveElement->getChild(child);
    if(oldChildElement==0L)
	theActiveElement->setChild(newElement =
				   new BasicElement(this,theActiveElement,child+4),child);
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
    newElement->setContent(cont);	
    setActiveElement(newElement);
    emitModified();
}

void KFormulaDoc::mousePressEvent( QMouseEvent *a,QWidget *wid)
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

void KFormulaDoc::keyPressEvent( QKeyEvent *k )
{
cerr << " key received , " << k->ascii() << endl;
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
	    if (theActiveElement!=0L)
		elReturn=theActiveElement->takeAsciiFromKeyb(k->ascii());
              }
	      
	    }
	}
    else   //Not ascii
	{
	    if (theActiveElement!=0L)
		elReturn=theActiveElement->takeActionFromKeyb(k->key());
	}	
        
	if (elReturn==FCOM_ADDTEXT) 
 	    { 
	     addTextElement();
	     if((k->ascii()>32)&&(k->ascii()<127))
 		theActiveElement->takeAsciiFromKeyb(k->ascii());
	    }



    //ChText(Blocks[getCurrent()]->getcont());
    //ChType(Blocks[getCurrent()]->gettype());
    //update();
    emitModified();
}


void KFormulaDoc::paintEvent( QPaintEvent *, QWidget *paintGround )
{
//    QPixmap pm(paintGround->size());  //double buffer
    thePainter->begin(paintGround);
    thePainter->setPen( black );
    theFirstElement->checkSize();
    theFirstElement->draw(QPoint(0,0)-theFirstElement->getSize().topLeft());
    if(theActiveElement && typeid(*theActiveElement) == typeid(TextElement))
	thePainter->drawWinFocusRect(theCursor);
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
    if(theActiveElement)
	theActiveElement->setActive(false);
    theActiveElement = c;
    if (theActiveElement)
	theActiveElement->setActive(true);
}

void KFormulaDoc::setFirstElement(BasicElement* c)
{

    if (c)
	theFirstElement = c;
    else
	warning("Try to set first element to 0L");
}


#include "kformula_doc.moc"
