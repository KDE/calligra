/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/     

#ifndef __kformviewer_doc_h__
#define __kformviewer_doc_h__

#include <qlist.h>
#include <qobject.h>
#include <koFrame.h>
#include <koDocument.h>
#include <koPrintExt.h>

#include "kformviewer.h"
#include "formobject.h"

class KformViewerView;

#define MIME_TYPE "application/x-kformviewer"
#define EDITOR    "IDL:KformViewer/Document:1.0"

class KformViewerDoc : public QObject,
                       virtual public KoDocument,
                       virtual public KoPrintExt,
                       virtual public KformViewer::Document_skel
{
  Q_OBJECT

public:
  KformViewerDoc();
  ~KformViewerDoc();

  virtual bool loadXML( KOMLParser&, KOStore::Store_ptr );
  virtual void cleanUp();
  virtual void removeView( KformViewerView* _view );
  virtual CORBA::Boolean initDoc();
  virtual KOffice::MainWindow_ptr createMainWindow();

  virtual OpenParts::View_ptr createView();
  virtual KformViewerView* createFormView( QWidget* _parent = 0 );
  virtual void viewList( OpenParts::Document::ViewList*& _list ); 
  virtual int viewCount();
  virtual bool isEmpty();
  virtual void draw( QPaintDevice* _dev, CORBA::Long _width, CORBA::Long _height, CORBA::Float _scale );

  char* mimeType();
  bool openDocument( const char* _filename, const char* _format = 0L );
  bool saveDocument( const char* _filename, const char* _format = 0L );
  virtual bool hasToWriteMultipart() { return false; };

  uint getFormWidth() { return 600; };
  uint getFormHeight() { return 400; };

signals:

  void sigUpdateView();
  
protected:

  virtual bool completeLoading( KOStore::Store_ptr );

  /**
   *  Indicates whether an form is loaded or not.
   */
  bool m_bEmpty;

  /**
   *  List of views, that are connectet to the document.
   */
  QList<KformViewerView> m_lstViews;

  /**
   *  List of form objects.
   */
  QList<FormObject> m_lstFormObjects;
};

#endif
