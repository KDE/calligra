/* This file is part of the KDE project
   Copyright (C)  2001 Montel Laurent <lmontel@mandrakesoft.com>

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

#ifndef __fontdia_h__
#define __fontdia_h__

#include <qdialog.h>
#include <kfontdialog.h>
#include <qcheckbox.h>
#include <qfont.h>


class KWFontDia : public QDialog
{
  Q_OBJECT
public:
  KWFontDia(  QWidget* parent, const char* name, const QFont &_font, bool _subscript,bool _superscript, bool _withSubSuperScript=true);

  bool getSuperScript() const { return m_superScript->isChecked();}
  bool getSubScript() const { return m_subScript->isChecked();}
  QFont getNewFont() const {return newFont;}

private:
  KFontChooser *m_chooseFont;
  QCheckBox *m_underline;
  QCheckBox *m_superScript;
  QCheckBox *m_subScript;
  QCheckBox *m_strikeOut;
  QFont newFont;

public slots:
  void slotOk();
  void slotCancel();
  void slotSuperScriptClicked();
  void slotSubScriptClicked();
  void slotUnderlineClicked();
  void slotStrikeOutClicked();
  void slotFontChanged(const QFont &);
signals:
  void okClicked();
};

#endif
