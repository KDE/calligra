#ifndef __kformula_doc_h__
#define __kformula_doc_h__

class KFormulaDocument;
#include "formuladef.h"
#include "BasicElement.h"
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


class KFormulaDocument : public QObject,
			 public FormulaClass,
			 virtual public KoDocument,
			 virtual public KFormula::Document_skel
{
    Q_OBJECT
 public:
    // C++
    KFormulaDocument();
    ~KFormulaDocument();
    
    /*int addBlock( int Type = -1, int ID = -1, int nextID = -1, int prevID = -1,
		  QString Cont = "", int Child1 = -1, int Child2 = -1, int Child3 = -1 );
    void checkAndCreate( FormulaBlock *bl );
    void deleteIt( FormulaBlock *bl );
    */
    QString name;
    
/*    BasicElement *activeElement,*firstElement;
    FormulaClass *theFormula;
*/
    /**
     * This is just a hack til view and doc are really split
     */
    void mousePressEvent(QMouseEvent *a,  QWidget *wid); 
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
    
    void addTextElement();
    void addRootElement();
    /*
    void addMatrixElement();
    void addIntegralElement();
    void addFractionElement();
    void addSymbolElement();
    void addDecorationElement();
    void addBraceElement();
    */
    
 signals:
    void sig_modified();

    void sig_changeText( const char * );
    void sig_changeType( int );
  
 protected:
    QList<KFormulaView> m_lstViews;

    bool m_bModified;

    
    // QTimer *cursorTimer;
    bool showIt;

};

#endif

