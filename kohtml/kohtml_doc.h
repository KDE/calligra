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
#include <krect.h>

#include <kurl.h>
#include <k2url.h>

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
  KoHTMLChild(KoHTMLDoc *doc, const KRect &rect, KOffice::Document_ptr koDoc);
  KoHTMLChild(KoHTMLDoc *doc);
  ~KoHTMLChild();
  
  KRect kGeometry() { return m__geometry; }
  void setkGeometry(KRect g) { m__geometry = g; }
  
  KoHTMLDoc *parent() { return m_parent; }

protected:
  KoHTMLDoc *m_parent;
  KRect m__geometry;  
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

  virtual char *htmlData();
  virtual char *htmlURL();
  virtual void openURL(const char *_url);
  virtual void feedData(const char *url, const char *data);

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

  virtual KoHTMLView *createKoHTMLView();

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
  
  virtual CORBA::Boolean init();
  
  virtual void cleanUp();
  
  virtual bool hasToWriteMultipart();

  virtual void makeChildListIntern(KOffice::Document_ptr root, const char *path);
  
  virtual const char *copyright(); 
  virtual const char *comment();
  
  void insertObject(const KRect &rect, KoDocumentEntry &de);
  void insertChild(KoHTMLChild *child);
  void changeChildGeometry(KoHTMLChild *child, const KRect &rect);
  QListIterator<KoHTMLChild> childIterator();

  void addHTMLView(KHTMLView *view);
  void removeHTMLView(KHTMLView *view);

signals:
  void sig_insertObject(KoHTMLChild *child);
  void sig_updateChildGeometry(KoHTMLChild *child);
  void contentChanged();
  
  void imageLoaded(const char *url, const char *filename);
  
protected slots:
  void slotDocumentRequest(KHTMLView *view, const char *url);
  void slotCancelDocumentRequest(KHTMLView *view, const char *url);
  void slotImageRequest(KHTMLView *view, const char *url);
  void slotCancelImageRequest(KHTMLView *view, const char *url);
  void slotDocumentStarted(KHTMLView *view);
  void slotDocumentDone(KHTMLView *view);

  void slotDocumentLoaded(KoHTMLJob *job, KHTMLView *topParent, KHTMLView *parent, const char *url, const char *filename);
  void slotImageLoaded(KoHTMLJob *job, KHTMLView *topParent, KHTMLView *parent, const char *url, const char *filename);  

  void slotUpdateInternalView();
  void slotDocumentDoneInternal(KHTMLView *view);

  void slotHTMLCodeLoaded(KoHTMLJob *job, KHTMLView *topParent, KHTMLView *parent, const char *url, const char *data, int len);
  void slotHTMLLoadError(const char *errMsg);
  void slotHTMLRedirect(int id, const char *url);

private:
  KoHTMLJob *findJob(KHTMLView *view, const char *url, KoHTMLJob::JobType jType);
  
  bool m_bModified;
  bool m_bEmpty;
  bool m_bDocumentDone;

  bool m_bRepaintDocument;
  QPixmap *m_pDocumentPixmap;
  CORBA::Long m_lastWidth;
  CORBA::Long m_lastHeight;
  CORBA::Float m_lastScale;

  bool m_bLoadError;
  QString m_strErrorMsg;
  
  QString m_strHTMLData;
  
  QString m_strCurrentURL;

  int m_htmlDocumentCounter; // number of frame documents or
                             // 1 if document is no frameset

  KoHTML::KoHTMLDocument::SaveLoadMode m_eSaveLoadMode;

  KHTMLView_Patched *m_pInternalView;

  QList<KoHTMLView> m_lstViews;
  QList<KoHTMLChild> m_lstChildren;
  QList<KHTMLView> m_lstHTMLViews;
  QList<KoHTMLJob> m_lstJobs;
};

#endif		 
