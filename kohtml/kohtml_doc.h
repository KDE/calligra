/*

	Copyright (C) 1998 Simon Hausmann
                       <tronical@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
//
// $Id$
//

#ifndef __kohtml_doc_h__
#define __kohtml_doc_h__

class KoHTMLChild;
class KoHTMLDoc;

#include <koFrame.h>
#include <koDocument.h>
#include <koQueryTypes.h>
#include <koPrintExt.h>
#include <komlParser.h>
#include <komlStreamFeed.h>
#include <komlWriter.h>
#include <qlist.h>
#include <qtimer.h>
#include <krect.h>

#include <kurl.h>
#include <kurl.h>

#include <kio_job.h>

#include <string>

#include <iostream.h>

#include <khtmlview.h>

#include "kohtml.h"
#include "kohtml_view.h"
#include "kohtmljob.h"

#define MIME_TYPE "application/x-kohtml"
// #define EDITOR "IDL:KoHTML/Document:1.0"

class KoHTMLChild : public KoDocumentChild
{
public:
  KoHTMLChild(KoHTMLDoc *doc, const QRect &rect, KOffice::Document_ptr koDoc);
  KoHTMLChild(KoHTMLDoc *doc);
  ~KoHTMLChild();
  
  QRect kGeometry() { return m__geometry; }
  void setkGeometry(QRect g) { m__geometry = g; }
  
  KoHTMLDoc *parent() { return m_parent; }

protected:
  KoHTMLDoc *m_parent;
  QRect m__geometry;  
};

class KoHTMLDoc : public QObject,
                 virtual public KoDocument,
		 virtual public KoPrintExt,
		 virtual public KoHTML::KoHTMLDocument_skel
{
  Q_OBJECT
public:
  KoHTMLDoc();
  ~KoHTMLDoc();

  virtual char *htmlURL();
  virtual void openURL( const char *_url, CORBA::Boolean reload );

  virtual void documentStarted();
  virtual void documentDone();

  virtual CORBA::Boolean documentLoading();
  
  virtual void stopLoading();

  virtual KoHTML::KoHTMLDocument::SaveLoadMode saveLoadMode();
  virtual void setSaveLoadMode( KoHTML::KoHTMLDocument::SaveLoadMode mode );

  virtual void draw( QPaintDevice *dev, CORBA::Long width, CORBA::Long height,
		     CORBA::Float _scale );
  virtual void drawDocument( QPaintDevice *dev, CORBA::Long width, CORBA::Long height,
		             CORBA::Float _scale );

  virtual void drawChildren( QPainter *painter, CORBA::Float _scale);

  virtual bool loadXML(KOMLParser &parser, KOStore::Store_ptr store);
  virtual bool loadChildren( KOStore::Store_ptr store);
  virtual bool save( ostream &out, const char *format);

  virtual KOffice::MainWindow_ptr createMainWindow();

  virtual KoHTMLView *createKoHTMLView( QWidget* _parent = 0 );

  virtual void removeView(KoHTMLView *view);

  virtual OpenParts::View_ptr createView();  
  
  virtual void viewList(OpenParts::Document::ViewList *&list);
  
  virtual int viewCount();
  
  virtual char * mimeType() { return CORBA::string_dup(MIME_TYPE); }

  virtual CORBA::Boolean isModified() { return m_bModified; }
  virtual void setModified(bool m)
  {
    m_bModified = m;
    if (m) m_bEmpty = false;
  }
  
  virtual bool isEmpty() { return m_bEmpty; }
  
  virtual CORBA::Boolean initDoc();
  
  virtual void cleanUp();
  
  virtual bool hasToWriteMultipart();

  virtual void makeChildListIntern(KOffice::Document_ptr root, const char *path);
  
  virtual const char *copyright(); 
  virtual const char *comment();
  
  void insertObject(const QRect &rect, KoDocumentEntry &de);
  void insertChild(KoHTMLChild *child);
  void changeChildGeometry(KoHTMLChild *child, const QRect &rect);
  QListIterator<KoHTMLChild> childIterator();

  // these two functions are needed to make using the internal view (for
  // the printing extension) possible.
  void addHTMLView( KMyHTMLView *view );
  void removeHTMLView( KMyHTMLView *view );
  
  void requestDocument( KMyHTMLView *view, const char *url, bool reload );
  void cancelDocument( KMyHTMLView *view, const char *url );
  
  void viewFinished( KMyHTMLView *view );
  
  void requestImage( KMyHTMLView *view, const char *url, bool reload );
  void cancelImage( KMyHTMLView *view, const char *url );

signals:
  void sig_insertObject(KoHTMLChild *child);
  void sig_updateChildGeometry(KoHTMLChild *child);
  void contentChanged();
  
protected slots:
//  void slotDocumentStarted(KHTMLView *view);
//  void slotDocumentDone(KHTMLView *view);

  void slotJobData( KoHTMLJob *job, const char *data, int len, bool eof );
  
  void slotDocumentDoneInternal(KHTMLView *view);

  void slotHTMLLoadError(KoHTMLJob *job, const char *errMsg);
  void slotHTMLRedirect(int id, const char *url);
  void slotTimeout();

private:
  KoHTMLJob *findJob( KMyHTMLView *view, const char *url, KoHTMLJob::JobType type );

  QTimer m_timer;
  
  bool m_bModified;
  bool m_bEmpty;
  bool m_bDocumentDone;

  bool m_bRepaintDocument;
  QPixmap *m_pDocumentPixmap;
  CORBA::Long m_lastWidth;
  CORBA::Long m_lastHeight;
  CORBA::Float m_lastScale;
  int m_lastXOffset;
  int m_lastYOffset;

  bool m_bLoadError;
  QString m_strErrorMsg;
  
  QString m_strCurrentURL;

  KoHTML::KoHTMLDocument::SaveLoadMode m_eSaveLoadMode;

  KMyHTMLView *m_pInternalView;

  QList<KoHTMLView> m_lstViews;
  QList<KoHTMLChild> m_lstChildren;
  QList<KMyHTMLView> m_lstHTMLViews;
  QList<KoHTMLJob> m_lstJobs;
};

#endif		 
