#ifndef __kformula_doc_h__
#define __kformula_doc_h__

class KFormulaDocument;

#include <koDocument.h>
#include <document_impl.h>
#include <view_impl.h>

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

#define MIME_TYPE "application/x-kformula"
#define KFormulaRepoID "IDL:KFormula/Document:1.0"

class FormulaBlock
{
 public:
    FormulaBlock( KFormulaDocument *_doc,int Type = -1,int ID = -1,int nextID = -1,int prevID = -1,
		  QString Cont = "", int Child1 = -1,int Child2=-1, int Child3=-1); 
    /**
     * Return ID of a block
     */
    int getID();
    /**
     * Return the width of the block 
     */ 
    int getOX();
    /**
     * Return the Height of the block 
     */
    int getOY();
    /**
     * Return the Total width of Block and next Bloks
     */
    int getTX();
    /**
     * Return the Max Height of Block and next Bloks
     */
    int getTY();
    /**
     * Return the Max height of first (Up) half of Block and next Bloks
     */
    int getLYU();
    /**
     * Return the Max height of second (Down) half of Block and next Bloks
     */  
    int getLYD();
    /**
     * Return the ID of the child block where mouse clicked (Use it on Block 0!)
     */
    int getClick(QPoint click);
    /**
     * Reduce/Enlarge the font size of the Block (recur=0) of Block & Children (recur = 1) of block
     * children & Next Blocks (recur = 2)
     */  
    void reduceFont(int recur);
    void enlargeFont(int recur);
    /*
     * Check the dimension of the block and call getDimensions() for children
     */  
    void getDimensions();
    /*
     * Paint the block & his children with QPainter here in x,y
     * set corr=1 for monitor
     *     corr=0 for printer 
     */
    void PaintIt(QPainter *here,int x,int y,int corr);    
  
    QFont fontc;

 private:
    KFormulaDocument *m_pDoc;
    int id;
    int lyu,lyd,oyu,oyd,ox,tx;   //,co; 
    int active,getdime;
    int next;
    int prev;
    int type;      //0=string  1=root   2=integral   3=brace      4=oriz Lin/spa  5 Over Simb
    QString cont; // content                        2chr:open/close  "l" or "v"
    int child1; //   exponent  Bl.cont.  Bl.cont.    Bl.cont.        numerator 
    int child2; //   index     index     upindex     exponent        denominator
    int child3; //   unused              downindex   unused          space btwn
    int sp;      
    QRect where;

 public:
    int getactive() { return active; }
    int getnext() { return next; }
    int getprev() { return prev; }
    int gettype() { return type; }  
    QString getcont() { return cont; } 
    int getchild1() { return child1; } 
    int getchild2() { return child2; } 
    int getchild3() { return child3; } 
    int getsp() { return sp; } 
    QRect getwhere() { return where; } 

    void setactive( int ac ) { active = ac; }
    void setnext( int ne ) { next = ne; }
    void setprev( int pr ) { prev = pr; }
    void settype( int ty ) { type = ty; }  
    void setcont( const char * co ) { cont = co; } 
    void setchild1( int c1 ) { child1 = c1; } 
    void setchild2( int c2 ) {  child2 = c2; } 
    void setchild3( int c3 ) { child3 = c3; } 
    void setsp( int s ) {  sp = s; } 
    void setwhere( QRect w ) { where = w; } 
};


class KFormulaDocument : public QObject,
			 virtual public KoDocument,
			 virtual public KFormula::Document_skel
{
    Q_OBJECT
 public:
    // C++
    KFormulaDocument();
    ~KFormulaDocument();

    int getPos();
    void setCursor(QRect re);
    void setCurrent(int cur);
    int getCurrent();
    int addBlock( int Type = -1, int ID = -1, int nextID = -1, int prevID = -1,
		  QString Cont = "", int Child1 = -1, int Child2 = -1, int Child3 = -1 );
    void checkAndCreate( FormulaBlock *bl );
    void deleteIt( FormulaBlock *bl );

    FormulaBlock *Blocks[2000];
    bool isFree[2000];
    int firstfree;
    int corr, xOff, yOff;
    QString name;

    /**
     * This is just a hack til view and doc are really split
     */
    void mousePressEvent(QMouseEvent *a); 
    void paintEvent( QPaintEvent *_ev, QWidget *paintGround );
    void keyPressEvent( QKeyEvent *k );
 protected:
    virtual void cleanUp();
    /**
     * We dont have embedded parts or binary data => No multipart files needed.
     */
    virtual bool hasToWriteMultipart() { return false; }
  
 public:
    // IDL
    virtual CORBA::Boolean init();

    // C++
    virtual bool load( KOMLParser& parser );
    /**
     * We dont load any children.
     */
    virtual bool loadChildren( OPParts::MimeMultipartDict_ptr _dict ) { return true; }
    virtual bool save( ostream& out );

    // IDL
    virtual OPParts::View_ptr createView();

    virtual void viewList( OPParts::Document::ViewList*& _list );

    virtual char* mimeType() { return CORBA::string_dup( MIME_TYPE ); }
  
    virtual CORBA::Boolean isModified() { return m_bModified; }
 
    // C++
    virtual void addView( KFormulaView *_view );
    virtual void removeView( KFormulaView *_view );

    void emitModified();

    // IDL
    void setGreek( bool on );
    void change( const char * newcont );
    void addB0();
    void addB1();
    void addB2();
    void addB3();
    void addB4();
    void addB5();
    void addB4bis();
    void line( int i );
    void addCh1();
    void addCh2();
    void addCh3();
    void pro();
    void dele();
    void reduceAll();
    void reduce();
    void reduceRecur();
    void enlargeAll();
    void enlarge();
    void enlargeRecur();

 signals:
    void sig_modified();

    void sig_changeText( const char * );
    void sig_changeType( int );
  
 protected:
    QList<KFormulaView> m_lstViews;

    bool m_bModified;

    QPainter *painter; 
    // QTimer *cursorTimer;
    int current;
    int pos;
    QRect cursorPos;
    bool isBlack;
    bool showIt;

};

#endif

