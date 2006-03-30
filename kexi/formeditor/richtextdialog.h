/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef RICHTEXTEDIT_DIALOG_H
#define RICHTEXTEDIT_DIALOG_H

#include <kdialogbase.h>
#include <ktextedit.h>

class KToolBar;
class KFontCombo;
class KColorCombo;

namespace KFormDesigner {

//! A simple dialog to edit rich text
/*! It allows to change font name, style and color, alignment. */
class KFORMEDITOR_EXPORT RichTextDialog : public KDialogBase
{
	Q_OBJECT

	public:
		RichTextDialog(QWidget *parent, const QString &text);
		~RichTextDialog(){;}

		QString  text();

		enum VerticalAlignment{AlignNormal = Q3TextEdit::AlignNormal, AlignSuperScript = Q3TextEdit::AlignSuperScript, AlignSubScript = Q3TextEdit::AlignSubScript};

	public slots:
		void  changeFont(const QString &);
		void  changeColor(const QColor&);
		void  buttonToggled(int);
		void  cursorPositionChanged(int, int);
		void  slotVerticalAlignmentChanged(VerticalAlignment align);

	private:
		enum { TBFont = 100, TBColor, TBBold, TBItalic, TBUnder, TBSuper, TBSub, TBLeft = 201, TBCenter, TBRight, TBJustify };
		KToolBar  *m_toolbar;
		KTextEdit  *m_edit;
		KFontCombo  *m_fcombo;
		KColorCombo  *m_colCombo;
};

}

#endif
