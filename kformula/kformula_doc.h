#ifndef KFORMULA_PART_H
#define KFORMULA_PART_H

class KFormulaDoc;
class BasicElement;


#include "formuladef.h"
#include "kformula_view.h"
#include "formula_container.h"

#include <qlist.h>
#include <qpainter.h>

#include <koDocument.h>

#define MIME_TYPE "application/x-kformula"



class KFormulaDoc :    virtual public KoDocument,public KFormulaContainer

{
    Q_OBJECT
public:
    KFormulaDoc( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
    ~KFormulaDoc();

    virtual KoMainWindow* createShell();

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );

    virtual bool initDoc();

    virtual bool loadXML( QIODevice *, const QDomDocument& doc );
    virtual QDomDocument saveXML();

    virtual QCString mimeType() const;

protected:
    virtual QString configFile() const;
    virtual KoView* createViewInstance( QWidget* parent, const char* name );


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

 public:
    KFormulaView* createFormulaView( QWidget* _parent = 0 );

//    virtual void addView( KFormulaView *_view );
//    virtual void removeView( KFormulaView *_view );

    virtual void emitModified();
    virtual bool isEmpty() { return m_bEmpty; };

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

public slots:
    void enlarge();
    void reduce();
    void enlargeRecur();
    void reduceRecur();
    void enlargeAll();
    void reduceAll();
    void pro();
    void dele();

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
