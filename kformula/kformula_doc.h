#ifndef KFORMULA_PART_H
#define KFORMULA_PART_H
#include "formula_container.h"
#include <koDocument.h>
#include "BasicElement.h"

class KFormulaDoc;

#include "formuladef.h"
#include "BasicElement.h"
#include <iostream>
#include "kformula_view.h"
#include <qlist.h>
#include <qobject.h>
#include <qrect.h>
#include <qpoint.h>
#include <qfont.h>
#include <qstring.h>
#include <qpainter.h>

#define MIME_TYPE "application/x-kformula"



class KFormulaDoc :    virtual public KoDocument,public KFormulaContainer

{
    Q_OBJECT
public:
    KFormulaDoc( KoDocument* parent = 0, const char* name = 0 );
    ~KFormulaDoc();
    virtual KoView* createView( QWidget* parent = 0, const char* name = 0 );
    virtual KoMainWindow* createShell();

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );

    virtual bool initDoc();

    virtual QCString mimeType() const;
    
protected:
    virtual QString configFile() const;


public:    
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
//    virtual int viewCount();

 protected:
    /**
     * We dont have embedded parts or binary data => No multipart files needed.
     */
    virtual bool hasToWriteMultipart() { return false; }
  
 public:
    // IDL
//    virtual CORBA::Boolean initDoc();

    /**
     * We dont load any children.
     */
//    virtual bool loadChildren( KOStore::Store_ptr ) { return true; }

    virtual bool save( ostream& out, const char* _format );

    // C++
    KFormulaView* createFormulaView( QWidget* _parent = 0 );
  
    // C++
//    virtual void addView( KFormulaView *_view );
//    virtual void removeView( KFormulaView *_view );

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
