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

#include <qwidget.h>
#include <openparts_ui.h>
#include <koFrame.h>
#include <koView.h>

#include "kformeditor.h"

class Form;
class KformEditorDoc;

class KformEditorView : public QWidget,
                        virtual public KoViewIf,
                        virtual public KformEditor::View_skel
{
  Q_OBJECT

public:

  KformEditorView( QWidget* , const char*, KformEditorDoc* );
  ~KformEditorView();
  KformEditorDoc* doc();

  virtual void cleanUp();
  bool printDlg();

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

protected:

  virtual void init();
  virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr );
  virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr );
  virtual void newView();
  virtual void helpUsing();
  bool event( const QCString & _event, const CORBA::Any& _value );
//void drawContents( QPainter* _painter, int _clipx, int _clipy, int _clipw, int _cliph );

private:

  OpenPartsUI::Menu_var m_vMenuEdit;
  long int m_idMenuEdit_Undo;
  long int m_idMenuEdit_Redo;
  long int m_idMenuEdit_Cut;
  long int m_idMenuEdit_Copy;
  long int m_idMenuEdit_Paste;
  long int m_idMenuEdit_FormSize;
  long int m_idMenuEdit_Background;

  OpenPartsUI::Menu_var m_vMenuInsert;
  long int m_idMenuInsert_Button;
  long int m_idMenuInsert_Label;
  long int m_idMenuInsert_LineEdit;
  long int m_idMenuInsert_ListBox;
  long int m_idMenuInsert_CheckBox;

  OpenPartsUI::Menu_var m_vMenuAlignment;
  long int m_idMenuAlignment_FitViewToForm;
  long int m_idMenuAlignment_Center;
  long int m_idMenuAlignment_Left;
  long int m_idMenuAlignment_HorizontalCenter;
  long int m_idMenuAlignment_Right;
  long int m_idMenuAlignment_Top;
  long int m_idMenuAlignment_VerticalCenter;
  long int m_idMenuAlignment_Bottom;

  OpenPartsUI::ToolBar_var m_vToolBarEdit;
  long int m_idToolBarEdit_Undo;
  long int m_idToolBarEdit_Redo;
  long int m_idToolBarEdit_Cut;
  long int m_idToolBarEdit_Copy;
  long int m_idToolBarEdit_Paste;

  OpenPartsUI::ToolBar_var m_vToolBarInsert;
  long int m_idToolBarInsert_Button;
  long int m_idToolBarInsert_Label;
  long int m_idToolBarInsert_LineEdit;
  long int m_idToolBarInsert_ListBox;
  long int m_idToolBarInsert_CheckBox;

  OpenPartsUI::ToolBar_var m_vToolBarAlignment;
  long int m_idToolBarAlignment_FitViewToForm;
  long int m_idToolBarAlignment_Center;
  long int m_idToolBarAlignment_Left;
  long int m_idToolBarAlignment_HorizontalCenter;
  long int m_idToolBarAlignment_Right;
  long int m_idToolBarAlignment_Top;
  long int m_idToolBarAlignment_VerticalCenter;
  long int m_idToolBarAlignment_Bottom;

  OpenPartsUI::StatusBar_var m_vStatusBar;

  Form* m_pForm;
  KformEditorDoc* m_pDoc;
};

#endif
