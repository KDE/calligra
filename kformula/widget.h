/**
 * KFormula - Formula Editor
 *
 * This code is free. Do with it what you want.
 *
 * (c) KDE Project
 * Andrea Rizzi
 * Bilibao@ouverture.it
 */

#ifndef xclgui_h
#define xclgui_h
#include <klocale.h>
#include <kspinbox.h>
#include <kfontdialog.h>
#include <qdialog.h>
#include <qchkbox.h>
#include <klined.h>
#include <qradiobt.h>

#include <qscrbar.h>
#include <qwidget.h>
#include <qpushbt.h>

class FormulaDisplay;
class Kformulaw;
#include "part.h"
class FormulaBlock
{
public:
  FormulaBlock(FormulaDisplay *Mother,int Type = -1,int ID = -1,int nextID = -1,int prevID = -1,
  		QString Cont = "", int Child1 = -1,int Child2=-1, int Child3=-1); 
/*
Return ID of a block
*/
  int getID();
/*
 Return the width of the block 
*/ 
  int getOX();
/*
 Return the Height of the block 
*/
  int getOY();
/*
 Return the Total width of Block and next Bloks
*/
  int getTX();
/*
 Return the Max Height of Block and next Bloks
*/
  int getTY();
/*
 Return the Max height of first (Up) half of Block and next Bloks
*/
  int getLYU();
/*
 Return the Max height of second (Down) half of Block and next Bloks
*/  
  int getLYD();
/*
 Return the ID of the child block where mouse clicked (Use it on Block 0!)
*/
  int getClick(QPoint click);
/*
 Reduce/Enlarge the font size of the Block (recur=0) of Block & Children (recur = 1) of block
 children & Next Blocks (recur = 2)
*/  
  void reduceFont(int recur);
  void enlargeFont(int recur);
/*
 Check the dimension of the block and call getDimensions() for children
*/  
  void getDimensions();
/*
  Paint the block & his children with QPainter here in x,y
  set corr=1 for monitor
      corr=0 for printer 
*/
  void PaintIt(QPainter *here,int x,int y,int corr);    
 
  QFont fontc;
private:
  FormulaDisplay *mother;
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
  int getsp()      { return sp; } 
  QRect getwhere() { return where; } 

  void    setactive(int ac) {active=ac; }
  void    setnext(int ne) { next=ne; }
  void    setprev(int pr) { prev=pr; }
  void    settype(int ty) {  type=ty; }  
  void    setcont(const char * co) { cont=co; } 
  void    setchild1(int c1) {  child1=c1; } 
  void    setchild2(int c2) {  child2=c2; } 
  void    setchild3(int c3) {  child3=c3; } 
  void    setsp(int s)      {  sp=s; } 
  void    setwhere(QRect w) {  where=w; } 
 

};
class FormulaDisplay : public QWidget
{
Q_OBJECT
public:
    int getPos();
    FormulaDisplay(  QWidget *parent=0, const char *name=0);
//   ~FormulaDisplay();
    QString	name;
    void setCursor(QRect re);
    void setCurrent(int cur);
    int  getCurrent();
    int addBlock(int Type = -1,int ID = -1,int nextID = -1,int prevID = -1,
       	QString Cont = "", int Child1 = -1,int Child2=-1, int Child3=-1);
    void checkAndCreate(FormulaBlock *bl); 
    void deleteIt(FormulaBlock *bl);
    FormulaBlock *Blocks[2000];
    bool    isFree[2000];
    int firstfree;
    int corr,xOff,yOff;
protected slots:
    void keyPressEvent(QKeyEvent *k);
    void mousePressEvent( QMouseEvent *a );
    void toggleCursor();
    void setGreek(bool on);
    void change(const char * newcont);
    void addB0();
    void addB1();
    void addB2();
    void addB3();
    void addB4();
    void addB5();
    void addB4bis();
    void line(int i);    
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

protected:
   void drawCursor();
   void	paintEvent( QPaintEvent * );
private:
    KLocale  *klo;
    QPainter *painter; 
    QTimer *cursorTimer;
    int current;
    int pos;
    QRect cursorPos;
    bool    isBlack;
    bool    showIt;
signals:
    void  ChText(const char *);
    void  ChType(int);
};



class Kformulaw : public QWidget
{
    Q_OBJECT
public:
    Kformulaw( FormulaEditor *_part );
    ~Kformulaw();
    
    QWidget* getPaperWidget() { return paperWidget; }
    QScrollBar* getHorzScrollBar() { return horzScrollBar; }    
    QScrollBar* getVertScrollBar() { return vertScrollBar; }    

    /**
     * @see MyPart::showGUI
     * @see MyPart::hideGUI
     */
    void showGUI( bool _status );

    /**
     * @see #xOffset
     */
    int getXOffset() { return xOffset; }
    /**
     * @see #yOffset
     */
    int getYOffset() { return yOffset; }
    
public slots:
    /**
     * This function scrolls the @ref paperWidget and all child KParts.
     */
    void slotScrollH( int );
    /**
     * This function scrolls the @ref paperWidget and all child KParts.
     */
    void slotScrollV( int );
    /**
     * This function updates the text in Klined lined.
     */
    void chtext(const char * text);
    /**
     * This function hide/show the buttons on thistypebar of the current 
     * blocktype
     */
    void chtype(int type);
    
protected:
    virtual void resizeEvent( QResizeEvent *_ev );
    KLined *lined;
    QScrollBar *horzScrollBar;
    QScrollBar *vertScrollBar;
    FormulaDisplay *paperWidget;
    KLocale  *klo;
    KToolBar *toolbar;
    KToolBar *toolbar1;
    KToolBar *thistypebar;
    KToolBar *fontbar;
    /**
     * @see #showGUI
     */
    bool bShowGUI;

    /**
     * I assume that you @ref #paperWidget is scrollable. In this case
     * we store the horizontal offset here. This is just a suggestion how
     * to do it.
     *
     * @ref #slotScrollH
     */
    int xOffset;
    /**
     * I assume that you @ref #paperWidget is scrollable. In this case
     * we store the vertical offset here. This is just a suggestion how
     * to do it.
     *
     * @ref #slotScrollH
     */
    int yOffset;
};






class ProData : public QDialog
{
    Q_OBJECT

public:

    ProData(FormulaBlock *edbl,QWidget* parent = NULL,const char* name = NULL);

    virtual ~ProData();

public slots:


protected slots:

    virtual void selectFont();
    virtual void CancelPressed();
    virtual void OkPressed();
    virtual void getValue1(bool);
    virtual void getValue3(bool);
    virtual void getValue2(bool);
    virtual void getValue4(bool);

protected:
    FormulaBlock *EdBl;
    KLined* TextLE;
    QRadioButton* cb[6];
    QCheckBox* fcb[5];
    KNumericSpinBox* spb[5];
};

#endif
