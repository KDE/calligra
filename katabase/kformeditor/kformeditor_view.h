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

#ifndef __kformeditor_gui_h__
#define __kformeditor_gui_h__

#include <qscrollview.h>
#include <openparts_ui.h>
#include <koFrame.h>
#include <koView.h>

#include "kformeditor.h"

class KformEditorDoc;
class WidgetWrapper;

class KformEditorView : public QScrollView,
                        virtual public KoViewIf,
                        virtual public KformEditor::View_skel
{
  Q_OBJECT

public:

  KformEditorView( QWidget* , const char*, KformEditorDoc* );
  ~KformEditorView();
  KformEditorDoc* doc();

  virtual void cleanUp();
  CORBA::Boolean printDlg();

  void editFormSize();
  void editBackground();
  void insertButton();
  void insertLabel();
  void insertLineEdit();
  void insertListBox();
  void insertCheckBox();
  void orientationCenter();

signals:
  void unselectAll();

public slots:

  void slotUpdateView();
  void slotWidgetSelected( WidgetWrapper* _widget );

protected:

  virtual void init();
  virtual bool event( const char* , const CORBA::Any& );
  virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr );
  virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr );
  virtual void newView();
  virtual void helpUsing();
  void resizeEvent( QResizeEvent* _event );
  void mouseMoveEvent( QMouseEvent* _event );

private:

  OpenPartsUI::Menu_var m_vMenuEdit;
  CORBA::Long m_idMenuEdit_FormSize;
  CORBA::Long m_idMenuEdit_Background;

  OpenPartsUI::ToolBar_var m_vToolBarInsert;
  CORBA::Long m_idToolBarInsert_Button;
  CORBA::Long m_idToolBarInsert_Label;
  CORBA::Long m_idToolBarInsert_LineEdit;
  CORBA::Long m_idToolBarInsert_ListBox;
  CORBA::Long m_idToolBarInsert_CheckBox;

  OpenPartsUI::ToolBar_var m_vToolBarOrientation;
  CORBA::Long m_idToolBarOrientation_HorizontalLeft;
  CORBA::Long m_idToolBarOrientation_HorizontalCenter;
  CORBA::Long m_idToolBarOrientation_HorizontalRight;
  CORBA::Long m_idToolBarOrientation_VerticalTop;
  CORBA::Long m_idToolBarOrientation_VerticalCenter;
  CORBA::Long m_idToolBarOrientation_VerticalBottom;
  CORBA::Long m_idToolBarOrientation_Center;

  KformEditorDoc* m_pDoc;  
};

#endif
