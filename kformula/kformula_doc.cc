#include "kformula_doc.h"
#include "kformula_view.h"
#include "kformula_factory.h"
#include "BasicElement.h"

#include "formuladef.h"
#include "BasicElement.h"
#include "TextElement.h"
#include "FractionElement.h"
#include "RootElement.h"
#include "BracketElement.h"
#include "MatrixElement.h"
#include "PrefixedElement.h"

#include <kurl.h>
#include <kiconloader.h>
#include <kapp.h>
#include <qpopupmenu.h>
#include <qdom.h>
#include <qwmatrix.h>
#include <qcolor.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qprinter.h>

#include <unistd.h>

#include <klocale.h>
#include <kstddirs.h>
#include <kaboutdialog.h>
#include <kdebug.h>


//#define FIXEDSPACE 1             // Space between 2 blocks
//#define DEFAULT_FONT_SIZE 24


KFormulaDoc::KFormulaDoc( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
  setInstance( KFormulaFactory::global(), false );
  //    setFocusPolicy( ClickFocus );
  thePosition=0;
  thePainter = new QPainter();
  kdDebug(39001) <<"General Settings"<<endl;
  //theFont.setFamily( "utopia" );
  //theFont.setPointSize(DEFAULT_FONT_SIZE);
  //theFont.setWeight( QFont::Normal );
  //theFont.setItalic( false );
  //theColor=black;
  //warning("General Font OK");
  //    theActiveElement = 0;

  // Use CORBA mechanism for deleting views
  m_lstViews.setAutoDelete( false );

  m_bModified = false;
  m_bEmpty = true;

  //theFirstElement= new BasicElement(this,0L,-1,0L,"");
  theFirstElement= new BasicElement(this,0L,-1,0L);
  addElement(theFirstElement, 0);
  addElement(theFirstElement, -1);
  eList.at(0);
  thePosition=0;

  /*warning("SIZE OF:  basic:%i,text:%i,root:%i",sizeof(BasicElement),
            sizeof(TextElement),sizeof(RootElement));*/
}


KFormulaDoc::~KFormulaDoc()
{
//    cleanUp();
}

void KFormulaDoc::cleanUp()
{
//    if ( m_bIsClean )
//      return;

//    assert( m_lstViews.count() == 0 );

//    KoDocument::cleanUp();
}


QDomDocument KFormulaDoc::saveXML()
{
  QDomDocument doc( "formular" );
  doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
  QDomElement kformular = doc.createElement( "kformular" );
  kformular.setAttribute( "author", "Andrea Rizzi, Laurent Montel" );
  kformular.setAttribute( "email", "rizzi@kde.org, lmontel@mandrakesoft.com" );
  kformular.setAttribute( "editor", "KFormula" );
  kformular.setAttribute( "mime", "application/x-kformula" );
  doc.appendChild( kformular );
  //QDomElement formula   = doc.createElement("formula");
  QDomElement firstElem = doc.createElement("element_first");
  QDomElement value  = theFirstElement->save(doc);
  firstElem.appendChild( value );

  /*out << "<FORMULA>" << endl;
    out << "<ELEM FIRST ";
    theFirstElement->save(out);
    out << "</FORMULA>" << endl;
    out << etag << "</DOC>" << endl;*/
  doc.appendChild( firstElem );
  return doc;
}

bool KFormulaDoc::loadXML( QIODevice *, const QDomDocument& doc )
{
  if ( doc.doctype().name() != "formular" )
        return false;

  QDomElement formular = doc.documentElement();
  if ( formular.attribute( "mime" ) != "application/x-kformula" )
        return false;

  return true;
}


KoView* KFormulaDoc::createViewInstance( QWidget* _parent, const char *name )
{
  return new KFormulaView( this, _parent, name);
}

void KFormulaDoc::emitModified()
{
  m_bModified = true;
  m_bEmpty = false;
  eList.clear();
  theFirstElement->makeList();
  eList.at(thePosition);
  emit sig_changeType( currentElement() );
  emit sig_modified();
}

void KFormulaDoc::addRootElement()
{
  BasicElement *newElement;
  if(currentElement()==0L)
    setActiveElement(theFirstElement);

  if (typeid(*currentElement()) == typeid(BasicElement)) {
    kdDebug(39001) <<"substitueted"<<endl;
    newElement = new RootElement(this);
    currentElement()->substituteElement(newElement);
    delete      currentElement();
    eList.current()->element = 0;
    kdDebug(39001) <<"done"<<endl;
  }
  else
    {
      BasicElement *nextElement=currentElement()->getNext();
      if(nextElement!=0L)
        {       //If there's a next insert root before next
          nextElement->insertElement(newElement=new RootElement(this));
        }
      else  //If there isn't a next append only.
        currentElement()->setNext(newElement=new RootElement(this,currentElement()));
    }
  newElement->check();
  kdDebug(39001) <<"Set activeelement..."<<endl;
  setActiveElement(newElement);

  //kdDebug(39001) <<"done"<<endl;
  //RootElement need a child[0] i.e. root content
  //    newElement = new BasicElement(this,currentElement(),4);
  //    currentElement()->setChild(newElement,0);

  setActiveElement(newElement->getChild(0));
  //I prefere to AutoActivate RootContent

  emitModified();
}

void KFormulaDoc::addPrefixedElement(QString cont)
{
  BasicElement *newElement;
  //     if(eList.at()==-1)
  //         eList.first();
  if(currentElement()==0L)
    setActiveElement(theFirstElement);


  //If current Element is a Basic, change it into a Prefixed
  if (typeid(*currentElement()) == typeid(BasicElement))
    {

      kdDebug(39001) <<"substitueted"<<endl;
      newElement = new PrefixedElement(this);
      currentElement()->substituteElement(newElement);
      delete    currentElement();
      eList.current()->element =0;
      //theActiveElement = 0;
    }
  else
    {
      BasicElement *nextElement=currentElement()->getNext();
      if(nextElement!=0L)
        {       //If there's a next insert root before next
          nextElement->insertElement(newElement=new PrefixedElement(this));
        }
      else
        {
          //If there isn't a next append only.
          currentElement()->setNext(newElement=new PrefixedElement(this,currentElement()));
        }
    }
  setActiveElement(newElement);
  currentElement()->setContent(cont);
  newElement = new BasicElement(this,currentElement(),4);
  currentElement()->setChild(newElement,0);
  setActiveElement(newElement); //I prefere to AutoActivate RootContent
  emitModified();
}

void KFormulaDoc::addFractionElement(QString cont)
{
  BasicElement *nextElement;
  BasicElement *newElement;
  if(currentElement()==0L)
    setActiveElement(theFirstElement);

  if(typeid(*currentElement()) == typeid(BasicElement))  //If current Element is a Basic
    {                                   //It change it into a Root
      currentElement()->substituteElement(newElement = new FractionElement(this));
      delete currentElement();
      eList.current()->element = 0;
    }
  else
    {
      nextElement=currentElement()->getNext();
      if(nextElement!=0L)
        {       //If there's a next insert root before next
          nextElement->insertElement(newElement=new FractionElement(this));
        }
      else              //If there isn't a next append only.
        currentElement()->setNext(newElement=new FractionElement(this,currentElement()));
    }
  newElement->setContent(cont);
  setActiveElement(newElement);
  //RootElement need a child[0] i.e. numer
  currentElement()->setChild(newElement = new BasicElement(this,currentElement(),5),1);
  currentElement()->setChild(newElement = new BasicElement(this,currentElement(),4),0);
  setActiveElement(newElement); //I prefere to AutoActivate numerator
  emitModified();

}

void KFormulaDoc::addMatrixElement(QString cont)
{
  int rows=cont.mid(3,3).toInt();
  int cols=cont.mid(6,3).toInt();

  BasicElement *nextElement;
  BasicElement *newElement;
  if(currentElement()==0L)
    setActiveElement(theFirstElement);

  if(typeid(*currentElement()) == typeid(BasicElement))  //If current Element is a Basic
    {                                   //It change it into a Root
      currentElement()->substituteElement(newElement = new MatrixElement(this));
      delete currentElement();
      eList.current()->element = 0;
    }
  else
    {
      nextElement=currentElement()->getNext();
      if(nextElement!=0L)
        {       //If there's a next insert root before next
          nextElement->insertElement(newElement=new MatrixElement(this));
        }
      else              //If there isn't a next append only.
        currentElement()->setNext(newElement=new MatrixElement(this,currentElement()));
    }
  newElement->setContent(cont);
  setActiveElement(newElement);
  //RootElement need a child[0] i.e. numer
  ((MatrixElement *)(currentElement()))->setChildrenNumber(rows*cols);
  for (int i=rows*cols-1;i>=0;i--)
    currentElement()->setChild(newElement = new BasicElement(this,currentElement(),i+4),i);
  setActiveElement(newElement); //I prefere to AutoActivate numerator
  emitModified();

}

void KFormulaDoc::addBracketElement(QString cont)
{
  BasicElement *nextElement;
  BasicElement *newElement;
  if(currentElement()==0L)
    setActiveElement(theFirstElement);

  if(typeid(*currentElement()) == typeid(BasicElement))  //If current Element is a Basic
    {
      //It change it into a Bracket
      currentElement()->substituteElement(newElement = new BracketElement(this));
      delete currentElement();
      eList.current()->element = 0;
    }
  else
    {
      nextElement=currentElement()->getNext();
      if(nextElement!=0L)
        {       //If there's a next insert  Brackets before next
          nextElement->insertElement(newElement=new BracketElement(this));
        }
      else              //If there isn't a next append only.
        {
          currentElement()->setNext(newElement=new BracketElement(this,currentElement()));
        }
    }

  newElement->setContent(cont);

  setActiveElement(newElement);
  //RootElement need a child[0] i.e. parenthesis content
  currentElement()->setChild(newElement = new BasicElement(this,currentElement(),4),0);
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
  if(currentElement()==0L)
    setActiveElement(theFirstElement);

  oldIndexElement=currentElement()->getIndex(index);
  if(oldIndexElement==0L)
    currentElement()->setIndex(newElement =
                               new BasicElement(this,currentElement(),index),index);
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
    if(currentElement()==0L)
        setActiveElement(theFirstElement);

    oldChildElement=currentElement()->getChild(child);
    if(oldChildElement==0L)
        currentElement()->setChild(newElement =
                                   new BasicElement(this,currentElement(),child+4),child);
    else
      {
        oldChildElement->insertElement(newElement = new BasicElement(this));
      }
    setActiveElement(newElement);
    emitModified();
    return(newElement);
}
void KFormulaDoc::addTextElement(QString cont)
{
    BasicElement *nextElement;
    BasicElement *newElement;
    if(currentElement()==0L)
        setActiveElement(theFirstElement);

    if(typeid(*currentElement()) == typeid(BasicElement))  //see addRootElement()
        {
            currentElement()->substituteElement(newElement = new TextElement(this));
            delete currentElement();
            eList.current()->element = 0;
        }
    else
        {
            nextElement=currentElement()->getNext();
            if(nextElement!=0L)
              {
                nextElement->insertElement(newElement=new TextElement(this));
              }
            else
              currentElement()->setNext(newElement=new TextElement(this,currentElement()));
        }
    newElement->setContent(cont);
    setActiveElement(newElement);
    emitModified();
}

void KFormulaDoc::mousePressEvent( QMouseEvent *a,QWidget *wid)
{
  if(!isReadWrite() )
    return;

  setActiveElement(theFirstElement->isInside(a->pos()));
  /*    if(currentElement()!=0)
        currentElement()->setPosition(-2);
  */
  emitModified();
  if(a->button()==RightButton)
    {
      QPopupMenu *mousepopup = new QPopupMenu;
      QPopupMenu *convert = new QPopupMenu;
      QPopupMenu *fontpopup = new QPopupMenu;
      fontpopup->insertItem(i18n("Font +"), this, SLOT(enlarge()));
      fontpopup->insertItem(i18n("Font -"), this, SLOT(reduce()));
      fontpopup->insertItem(i18n("Font + (also Children)"), this, SLOT(enlargeRecur()));
      fontpopup->insertItem(i18n("Font - (also Children)"), this, SLOT(reduceRecur()));
      fontpopup->insertItem(i18n("All Font +"), this, SLOT(enlargeAll()));
      fontpopup->insertItem(i18n("All Font -"), this, SLOT(reduceAll()));
      //    convert->insertItem(klocale("Simple Text"), parent->_part, SLOT(slotQuit()), ALT+Key_E);
      //    convert->insertItem("AAAARGGHHH", _part, SLOT(slotQuit()), ALT+Key_E);
      mousepopup->insertItem(i18n("Properties"), this,SLOT(pro()));
      mousepopup->insertItem(i18n("Delete"), this,SLOT(dele()));
      mousepopup->insertSeparator();
      mousepopup->insertItem(i18n("Change font"),fontpopup);
      mousepopup->insertItem(i18n("Export as"),convert);
      //    mousepopup->insertSeparator();
      //    mousepopup->insertItem(i18n("Quit"), _part, SLOT(slotQuit()), ALT+Key_Q);
      mousepopup->popup(wid->mapToGlobal(a->pos()));
    }
}


void KFormulaDoc::enlarge()
{
  kdDebug(39001) <<"enlarge()\n";
}

void KFormulaDoc::reduce()
{
  kdDebug(39001) <<"reduce()\n";
}

void KFormulaDoc::enlargeRecur()
{
  kdDebug(39001) <<"enlargeRecur()\n";
}
void KFormulaDoc::reduceRecur()
{
  kdDebug(39001) <<"redureRecur()\n";
}
void KFormulaDoc::enlargeAll()
{
  kdDebug(39001) <<"enlargeAll()\n";
}
void KFormulaDoc::reduceAll()
{
  kdDebug(39001) <<"reduceAll()\n";
}
void KFormulaDoc::pro()
{
  kdDebug(39001) <<"pro()\n";
}
void KFormulaDoc::dele()
{
  kdDebug(39001) <<"delete()\n";
}

void KFormulaDoc::keyPressEvent( QKeyEvent *k )
{
if(!isReadWrite() )
        return;
    kdDebug(39001) <<"Key pressed :"<<k->key()<<" ascii: "<<k->ascii()<<endl;
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
            //It's not possible to add mathematic symbol <  or > if
            // I assigne < to <>
          /*case '<':
            addBracketElement("<>");
            break;*/
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

              if (currentElement()!=0L)
                {
                  int po=eList.current()->pos;
                  if (po>0)
                    {
                      QString text;
                      po--;

                      kdDebug(39001) <<"Internal Text Position "<<po<<endl;
                      text=currentElement()->getContent().copy();
                      kdDebug(39001) <<"Text "<<text.ascii()<<endl;
                      text.insert(po,k->ascii());
                      currentElement()->setContent(text);
                      thePosition=eList.at();
                      kdDebug(39001) <<"thePosition  "<<thePosition<<" int:"<<eList.current()->pos<<endl;
                      eList.clear();
                      theFirstElement->makeList();
                      thePosition++;
                      eList.at(thePosition);
                      kdDebug(39001) <<"the New Position "<<thePosition<<" int: "<<eList.current()->pos<<endl;

                    }
                  else
                    elReturn=FCOM_ADDTEXT;
                }
            } //default
          }  //Switch
      }  //if
    else   //Not ascii
      {
        int action=k->key();
        if (currentElement()!=0L)
          {
            int po=eList.current()->pos;
            if (po>0)
              {


              }
          }

        if(action==Qt::Key_BackSpace)
          {
              uint c = eList.at();
              if(eList.prev()!=0) {
                  if (eList.current()->element!=0)
                      action=Qt::Key_Delete;
              }
              else
                  eList.at(c);
          }

        if(action==Qt::Key_Delete)
          {
            kdDebug(39001) <<"Key Delete"<<endl;
            if(eList.current()->pos>0)
              {
                QString text;
                text=currentElement()->getContent().copy();
                text.remove(eList.current()->pos-1,1);
                currentElement()->setContent(text);
                thePosition=eList.at();
                eList.clear();
                theFirstElement->makeList();
                eList.at(thePosition);
              }


            else
              {
                  if(eList.current()->pos==0) {
                      elReturn=FCOM_DELETEME;
                  }
                  else
                  {
                      // If you press `delete' in an empty document
                      // there is no next pos.
                      uint c = eList.at();
                      if (eList.next()!=0) {
                          if (eList.current()->element!=0)
                              elReturn=FCOM_DELETEME;
                      }
                      else
                          eList.at(c);
                  }
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
           */
            eList.next();

          }

        kdDebug(39001) <<"Not Ascii return "<<elReturn<<endl;
        thePosition=eList.at();
      }

    if (elReturn==FCOM_ADDTEXT)
      {
        if(eList.current()->pos==0)
          {
            BasicElement *newElement;
            currentElement()->insertElement(newElement=new TextElement(this));

            setActiveElement(newElement);
          }
        else
          addTextElement();

        if((k->ascii()>32)&&(k->ascii()<127))
          {
            QString text;
            int po=0;

            kdDebug(39001) <<"Internal Text Position "<<po<<endl;
            text=currentElement()->getContent().copy();
            text.insert(po,k->ascii());
            currentElement()->setContent(text);
            thePosition=eList.at();
            kdDebug(39001) <<"thePosition "<<thePosition<<"  int: "<<eList.current()->pos<<endl;
            eList.clear();
            theFirstElement->makeList();
            thePosition++;
            eList.at(thePosition);
            kdDebug(39001) <<"the New Position "<<thePosition<<" int:"<<eList.current()->pos<<endl;
          }

      }
    if (elReturn==FCOM_DELETEME)
      {

        BasicElement *newActive;
        BasicElement *prev;
        newActive=currentElement()->getNext();
        prev=currentElement()->getPrev();
        currentElement()->deleteElement();
        if(prev!=0)
          prev->check();
        setActiveElement(newActive);

      }

    emitModified();
}


void KFormulaDoc::paintEvent( QPaintEvent *, QWidget *paintGround )
{
    /*              //paintGround->size()
                    QPixmap pm(100,100);  //double buffer
                    pm.fill();
                    QPainter q;
                    thePainter=&q;
    */
    thePainter->begin(paintGround);
    thePainter->setPen( black );
    theFirstElement->checkSize();
    if(currentElement()!=0L)
        currentElement()->setActive(true);
    theFirstElement->draw(QPoint(5,5)-theFirstElement->getSize().topLeft());
    //    if(currentElement() && typeid(*currentElement()) == typeid(TextElement))
    //  {
    //thePainter->drawWinFocusRect(theCursor);
    theCursor=currentElement()->getCursor(eList.current()->pos);
    thePainter->drawLine(theCursor.topLeft()+QPoint(0,1),theCursor.topRight()+QPoint(0,1));
    thePainter->drawLine(theCursor.bottomLeft()-QPoint(0,1),theCursor.bottomRight()-QPoint(0,1));
    thePainter->drawLine(theCursor.topLeft()+QPoint(theCursor.width()/2,1),
                         theCursor.bottomLeft()+QPoint(theCursor.width()/2,-1));
    //  }
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
    kdDebug(39001) <<"Set active element old "<<currentElement()<<"  new "<<c<<endl;
    eList.clear();
    thePosition=-1;
    theFirstElement->makeList();
    for(eList.first();eList.current()!=0;eList.next())
        if(currentElement()==c)
            thePosition=eList.at();
    eList.at(thePosition);

    /*    if(currentElement())
          currentElement()->setActive(false);
          if (c!=0)
          c->setActive(true);
          eList.clear();
          warning("clear list");
          thePosition=-1;
          theFirstElement->makeList(1);
          eList.at(thePosition);*/
    //warning("New Active Element  %p",c);
}



bool KFormulaDoc::initDoc()
{
    // If nothing is loaded, do initialize here
    return TRUE;
}

void KFormulaDoc::paintContent( QPainter& painter, const QRect& /*rect*/, bool /*transparent*/ )
{
    // ####### handle transparency

    // Need to draw only the document rectangle described in the parameter rect.
    QPainter *old=thePainter;
    thePainter=&painter;
    theFirstElement->checkSize();
    if(currentElement()!=0L)
        currentElement()->setActive(false);
    theFirstElement->draw(QPoint(5,5)-theFirstElement->getSize().topLeft());
    if(currentElement()!=0L)
        currentElement()->setActive(true);
    thePainter=old;

}

QString KFormulaDoc::configFile() const
{
//    return readConfigFile( locate( "data", "kformula/kformula.rc",
//                                 KFormulaFactory::global() ) );

//    return readConfigFile( "kformula.rc" );
    return QString::null;
}

#include "kformula_doc.moc"
