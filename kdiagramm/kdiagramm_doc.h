#ifndef __kdiagramm_doc_h__
#define __kdiagramm_doc_h__

class KDiagrammDoc;

#include <koFrame.h>
#include <koDocument.h>

#include <iostream.h>
#include <qlist.h>

#include <qobject.h>
#include <qrect.h>
#include <qstring.h>
#include <qpainter.h>
#include <qprinter.h>


#include "kdiagramm.h"
#include "kdiagramm_view.h"
#include "koDiagramm.h"
#include "chart.h"

#include <koPageLayoutDia.h>


#define MIME_TYPE "application/x-kdiagramm"
#define EDITOR "IDL:KDiagramm/Document:1.0"

/*
 */
class KDiagrammDoc : public QObject,
		     virtual public KoDocument,
		     virtual public Chart::SimpleChart_skel,
		     virtual public KDiagramm::Document_skel
{
  Q_OBJECT
public:
  // C++
  KDiagrammDoc();
  ~KDiagrammDoc();

  // C++
  virtual bool save( ostream&, const char *_format );

  // C++
  // virtual bool loadChildren( KOStore::Store_ptr _store ) { return m_pMap->loadChildren( _store ); }
  virtual bool loadXML( KOMLParser&, KOStore::Store_ptr _store );
  
  virtual void cleanUp();

  virtual void removeView( KDiagrammView* _view );

  // C++
  virtual KDiagrammView* createDiagrammView();

  // IDL
  virtual CORBA::Boolean init();
  
  /**
   * Wrapper for @ref #createDiagrammView
   */
  virtual OpenParts::View_ptr createView();
  
  virtual void viewList( OpenParts::Document::ViewList*& _list );
  
  virtual char* mimeType() { return CORBA::string_dup( MIME_TYPE ); }
  
  virtual CORBA::Boolean isModified() { return m_bModified; }
  
  virtual bool hasToWriteMultipart() { return false; }

  // C++
  virtual int viewCount();
  
  // C++
  virtual void setModified( bool _c ) { m_bModified = _c; if ( _c ) m_bEmpty = false; }
  virtual bool isEmpty() { return m_bEmpty; }

  // C++
  table_t& data() { return m_table; }
  
  // C++
  virtual void editData();
  
  // IDL
  virtual void showWizard();
  virtual void fill( const Chart::Range& range, const Chart::Matrix& matrix );

signals:
  // Document signals
  void sig_updateView();
  
protected:
  virtual bool completeLoading( KOStore::Store_ptr /* _store */ );
  
  table_t m_table;
  bool m_bEmpty;

  QList<KDiagrammView> m_lstViews;

  Chart::Range m_range;
};

#endif
