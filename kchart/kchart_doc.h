#ifndef __kchart_doc_h__
#define __kchart_doc_h__

class KChartDocument;

#include <koDocument.h>
#include <document_impl.h>
#include <view_impl.h>

#include <iostream>

#include "kchart_view.h"
#include "kchart.h"

#include <qlist.h>
#include <qobject.h>

#include <KChart.h>
#include <KChartData.h>

#include "chart.h"

#define MIME_TYPE "application/x-kchart"
#define KChartRepoID "IDL:KCharter/Document:1.0"

class KChartDocument : public QObject,
		       virtual public KoDocument,
		       virtual public Chart::SimpleChart_skel,
		       virtual public KCharter::Document_skel
{
  Q_OBJECT
public:
  // C++
  KChartDocument();
  ~KChartDocument();
  
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
 
  // IDL
  virtual void fill( const Chart::Range& range, const Chart::Matrix& matrix );
  
  // C++
  virtual void addView( KChartView *_view );
  virtual void removeView( KChartView *_view );

  KChart& chart() { return m_chart; }
  
signals:
  void sig_modified();
  
protected:
  QList<KChartView> m_lstViews;

  KChart m_chart;
  KChartData* m_pData;
  
  bool m_bModified;
};

#endif
