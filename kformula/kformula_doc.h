#ifndef __kformula_doc_h__
#define __kformula_doc_h__

class KFormulaDoc;

#include "formuladef.h"
#include "BasicElement.h"
#include <koDocument.h>

#include <iostream>

#include "kformula_view.h"
#include "kformula.h"

#include <qlist.h>
#include <qobject.h>
#include <qrect.h>
#include <qpoint.h>
#include <qfont.h>
#include <qstring.h>
#include <qpainter.h>

#include "formula_container.h"

#define MIME_TYPE "application/x-kformula"

class KFormulaDoc : public KFormulaContainer, 
		    virtual public KoDocument,
		    virtual public KFormula::Document_skel
{
    Q_OBJECT
	public:
    // C++
    KFormulaDoc();
    ~KFormulaDoc();
    
    /*int addBlock( int Type = -1, int ID = -1, int nextID = -1, int prevID = -1,
      QString Cont = "", int Child1 = -1, int Child2 = -1, int Child3 = -1 );
      void checkAndCreate( FormulaBlock *bl );
      void deleteIt( FormulaBlock *bl );
    */
    QString name;
    
    /*    BasicElement *activeElement,*firstElement;
	  KFormulaDoc *theFormula;
    */
    /**
     * This is just a hack til view and doc are really split
     */
    void mousePressEvent(QMouseEvent *a,  QWidget *wid); 
    void paintEvent( QPaintEvent *_ev, QWidget *paintGround );
    void print( QPrinter *thePrt);
    void keyPressEvent( QKeyEvent *k );

    virtual void cleanUp();

  
    // C++
    virtual int viewCount();

 protected:
    /**
     * We dont have embedded parts or binary data => No multipart files needed.
     */
    virtual bool hasToWriteMultipart() { return false; }
  
 public:
    // IDL
    virtual CORBA::Boolean initDoc();

    // C++
    virtual bool loadXML( KOMLParser& parser, KOStore::Store_ptr _store );
    /**
     * We dont load any children.
     */
    virtual bool loadChildren( KOStore::Store_ptr ) { return true; }

    virtual bool save( ostream& out, const char* _format );

    // IDL
    virtual OpenParts::View_ptr createView();
    // C++
    KFormulaView* createFormulaView( QWidget* _parent = 0 );
  
    // IDL
    virtual void viewList( OpenParts::Document::ViewList*& _list );

    virtual char* mimeType() { return CORBA::string_dup( MIME_TYPE ); }
  
    virtual CORBA::Boolean isModified() { return m_bModified; }

    virtual KOffice::MainWindow_ptr createMainWindow();
 
    // C++
    virtual void addView( KFormulaView *_view );
    virtual void removeView( KFormulaView *_view );

    virtual void emitModified();
    virtual bool isEmpty() { return m_bEmpty; };
  
    // IDL
    void addTextElement(QString cont="");
    void addRootElement();
    BasicElement * addIndex(int index);
    BasicElement * addChild(int child);
    void addFractionElement(QString cont);    
    void addMatrixElement(QString cont);
    void addPrefixedElement(QString cont);
    
    /*
      void addSymbolElement();
      void addDecorationElement();
    */    
    void addBracketElement(QString cont);

    void setActiveElement(BasicElement* c); 
    void setCursor(const QRect& r) { theCursor=r; }

    int thePosition;
    
    BasicElement *activeElement() const { return theActiveElement; } 
    QRect cursor() const { return theCursor; }
    
 signals:
    void sig_modified();

    void sig_changeText( const char * );
    void sig_changeType(const BasicElement* );
  
 protected:
    QList<KFormulaView> m_lstViews;

    bool m_bModified;
    bool m_bEmpty;
    
    // QTimer *cursorTimer;
    bool showIt;

    
    BasicElement *theActiveElement;
    
    QRect theCursor;

};

#endif

