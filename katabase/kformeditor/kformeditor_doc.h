/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Michael Koch <koch@kde.org>
 
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

#ifndef __kformeditor_doc_h__
#define __kformeditor_doc_h__

#include <qlist.h>
#include <qobject.h>
#include <koFrame.h>
#include <koDocument.h>
#include <koPrintExt.h>

#include "kformeditor.h"
#include "formobject.h"

class KformEditorView;

#define MIME_TYPE "application/x-kformviewer"
#define EDITOR    "IDL:KformEditor/Document:1.0"

class KformEditorDoc : public QObject,
                       virtual public KoDocument,
                       virtual public KoPrintExt,
                       virtual public KformEditor::Document_skel
{
  Q_OBJECT

public:
  KformEditorDoc();
  ~KformEditorDoc();

  virtual bool loadXML( KOMLParser&, KOStore::Store_ptr );
  virtual void cleanUp();
  virtual void removeView( KformEditorView* _view );
  virtual bool initDoc();
  virtual KOffice::MainWindow_ptr createMainWindow();

  virtual OpenParts::View_ptr createView();
  virtual KformEditorView* createFormView( QWidget* _parent = 0 );
  virtual void viewList( OpenParts::Document::ViewList & _list ); 
  virtual int viewCount();
  virtual bool isEmpty();
  virtual void draw( QPaintDevice* _dev, long int _width, long int _height, float _scale );

  QCString mimeType() { return MIME_TYPE; }
  virtual bool hasToWriteMultipart() { return false; };

  uint getFormWidth() { return m_FormWidth; };
  uint getFormHeight() { return m_FormHeight; };
  void setBackgroundColor( const QColor& _color );
  QColor backgroundColor();
  void setFormName( const QString& _name );
  QString formName();

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
  QList<KformEditorView> m_lstViews;

public:
  /**
   *  List of form objects.
   */
  QValueList<FormObject*> m_lstFormObjects;

private:

  int m_FormWidth;
  int m_FormHeight;
  QColor m_backgroundColor;
  QString m_FormName;
};

#endif
