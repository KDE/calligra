/*
 *  kimageshop_doc.h - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch    <mkoch@kde.org>
 *                1999 Matthias Elter  <me@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __kimageshop_doc_h__
#define __kimageshop_doc_h__

#include <qlist.h>
#include <qstring.h>

#include <koFrame.h>
#include <koDocument.h>
#include <koPrintExt.h>
#include <koUndo.h>

#include "kimageshop.h"
#include "kimageshop_view.h"
#include "canvas.h"

#define MIME_TYPE "application/x-kimageshop"
#define EDITOR "IDL:KImageShop/Document:1.0"

class KImageShopDoc : public Canvas,
		      virtual public KoDocument,
		      virtual public KoPrintExt,
		      virtual public KImageShop::Document_skel
{
  Q_OBJECT
    
public:

  KImageShopDoc(int w = 510, int h = 510);
  ~KImageShopDoc();

  virtual bool save( ostream&, const char* _format );
//virtual bool completeSaving( KOStore::Store_ptr _store );
  virtual bool hasToWriteMultipart() { return true; }
//virtual bool loadXML( KOMLParser&, KOStore::Store_ptr _store );
  virtual void cleanUp();
  virtual void removeView( KImageShopView* _view );

  virtual CORBA::Boolean initDoc();
  virtual KOffice::MainWindow_ptr createMainWindow();
  
  // Wrapper for @ref #createImageView
  virtual OpenParts::View_ptr createView();
  virtual KImageShopView* createImageView( QWidget* _parent = 0 );

  virtual void viewList( OpenParts::Document::ViewList*& _list );
  virtual char* mimeType();
  virtual CORBA::Boolean isModified();
  virtual int viewCount();
  virtual void setModified( bool _c );
  virtual bool isEmpty();
  virtual void print( QPaintDevice* _dev );
  virtual void draw( QPaintDevice* _dev, CORBA::Long _width, CORBA::Long _height, CORBA::Float _scale );

//bool openDocument( const char* _filename, const char* _format = 0L );
//bool saveDocument( const char* _filename, const char* _format = 0L );

  KoCommandHistory* commandHistory() { return &m_commands; };

public slots:

  void slotUpdateViews( const QRect &area );
  void slotUndoRedoChanged( QString _undo, QString _redo );
  void slotUndoRedoChanged( QStringList _undo, QStringList _redo );

signals:

  void sigUpdateView( const QRect &area );
  
protected:

//virtual bool completeLoading( KOStore::Store_ptr /* _store */ );

  // List of views, that are connectet to the document.
  QList<KImageShopView> m_lstViews;

  KoCommandHistory m_commands;

  QString m_strImageFormat;
  bool m_bEmpty;
};

#endif






