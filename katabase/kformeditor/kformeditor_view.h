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

  void editUndo();
  void editRedo();
  void editCut();
  void editCopy();
  void editPaste();
  void editFormSize();
  void editBackground();
  void insertButton();
  void insertLabel();
  void insertLineEdit();
  void insertListBox();
  void insertCheckBox();
  void alignmentFitViewToForm();
  void alignmentCenter();
  void alignmentLeft();
  void alignmentHorizontalCenter();
  void alignmentRight();
  void alignmentTop();
  void alignmentVerticalCenter();
  void alignmentBottom();

signals:

  void unselectAll();

public slots:

  void slotUpdateView();
  void slotClick( WidgetWrapper* _widget );
  void slotShiftClick( WidgetWrapper* _widget );

protected:

  virtual void init();
  virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr );
  virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr );
  virtual void newView();
  virtual void helpUsing();
  virtual bool eventFilter( QObject*, QEvent* );
  void resizeEvent( QResizeEvent* _event );
  void initChilds();
  bool event( const char* _event, const CORBA::Any& _value );
  void drawContents( QPainter* _painter, int _clipx, int _clipy, int _clipw, int _cliph );

private:

  OpenPartsUI::Menu_var m_vMenuEdit;
  CORBA::Long m_idMenuEdit_Undo;
  CORBA::Long m_idMenuEdit_Redo;
  CORBA::Long m_idMenuEdit_Cut;
  CORBA::Long m_idMenuEdit_Copy;
  CORBA::Long m_idMenuEdit_Paste;
  CORBA::Long m_idMenuEdit_FormSize;
  CORBA::Long m_idMenuEdit_Background;

  OpenPartsUI::Menu_var m_vMenuInsert;
  CORBA::Long m_idMenuInsert_Button;
  CORBA::Long m_idMenuInsert_Label;
  CORBA::Long m_idMenuInsert_LineEdit;
  CORBA::Long m_idMenuInsert_ListBox;
  CORBA::Long m_idMenuInsert_CheckBox;

  OpenPartsUI::Menu_var m_vMenuAlignment;
  CORBA::Long m_idMenuAlignment_FitViewToForm;
  CORBA::Long m_idMenuAlignment_Center;
  CORBA::Long m_idMenuAlignment_Left;
  CORBA::Long m_idMenuAlignment_HorizontalCenter;
  CORBA::Long m_idMenuAlignment_Right;
  CORBA::Long m_idMenuAlignment_Top;
  CORBA::Long m_idMenuAlignment_VerticalCenter;
  CORBA::Long m_idMenuAlignment_Bottom;

  OpenPartsUI::ToolBar_var m_vToolBarEdit;
  CORBA::Long m_idToolBarEdit_Undo;
  CORBA::Long m_idToolBarEdit_Redo;
  CORBA::Long m_idToolBarEdit_Cut;
  CORBA::Long m_idToolBarEdit_Copy;
  CORBA::Long m_idToolBarEdit_Paste;

  OpenPartsUI::ToolBar_var m_vToolBarInsert;
  CORBA::Long m_idToolBarInsert_Button;
  CORBA::Long m_idToolBarInsert_Label;
  CORBA::Long m_idToolBarInsert_LineEdit;
  CORBA::Long m_idToolBarInsert_ListBox;
  CORBA::Long m_idToolBarInsert_CheckBox;

  OpenPartsUI::ToolBar_var m_vToolBarAlignment;
  CORBA::Long m_idToolBarAlignment_FitViewToForm;
  CORBA::Long m_idToolBarAlignment_Center;
  CORBA::Long m_idToolBarAlignment_Left;
  CORBA::Long m_idToolBarAlignment_HorizontalCenter;
  CORBA::Long m_idToolBarAlignment_Right;
  CORBA::Long m_idToolBarAlignment_Top;
  CORBA::Long m_idToolBarAlignment_VerticalCenter;
  CORBA::Long m_idToolBarAlignment_Bottom;

  OpenPartsUI::StatusBar_var m_vStatusBar;
  
  KformEditorDoc* m_pDoc;
  QWidget* m_background;
  WidgetWrapper* m_primaryWidget;
  int m_countSelectedWidgets;
};

#endif
