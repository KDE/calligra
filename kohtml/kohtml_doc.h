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

#define MIME_TYPE "application/x-kohtml"
#define EDITOR "IDL:KoHTML/Document:1.0"

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

class KoHTMLJob: public KIOJob
{
  Q_OBJECT
public:
  enum JobType { HTML, Image };

  KoHTMLJob(KHTMLView *_topParent, KHTMLView *_parent, const char *_url, JobType _jType);
  ~KoHTMLJob();
  
  void start();
  
  JobType getType() { return jType; }
  const char *getURL() { return url.data(); }
  KHTMLView *getParent() { return parent; }
  KHTMLView *getTopParent() { return topParent; }
  
signals:  
  void jobDone(KoHTMLJob *job, KHTMLView *topParent, KHTMLView *parent, const char *url, const char *filename);
  
protected slots:
  void slotJobFinished();
  void slotError();
  
private:
  KHTMLView *topParent, *parent;
  QString url;
  QString tmpFile;
  JobType jType;
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

  virtual char *getHTMLData();
  virtual char *getURL();
  virtual void openURL(const char *_url);
  virtual void feedData(const char *url, const char *data);

  virtual CORBA::Boolean documentDone() { return (CORBA::Boolean)m_bDocumentDone; }

  virtual void draw( QPaintDevice *dev, CORBA::Long width, CORBA::Long height,
		     CORBA::Float _scale );

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

  void slotHTMLCodeLoaded(KoHTMLJob *, KHTMLView *, KHTMLView *, const char *, const char *file);
  void slotHTMLLoadError(int id, int errid, const char *txt);  

private:
  KoHTMLJob *findJob(KHTMLView *view, const char *url, KoHTMLJob::JobType jType);
  
  bool m_bModified;
  bool m_bEmpty;
  bool m_bDocumentDone;
  
  QString htmlData;
  
  QString m_vCurrentURL;

  KHTMLView_Patched *m_vInternalView;

  KoHTMLJob *m_pMainJob;
  
  QList<KoHTMLView> m_lstViews;
  QList<KoHTMLChild> m_lstChildren;
  QList<KHTMLView> m_lstHTMLViews;
  QList<KoHTMLJob> m_lstJobs;
};

#endif		 
