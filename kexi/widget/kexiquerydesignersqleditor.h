/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIQUERYDESIGNERSQLEDITOR_H
#define KEXIQUERYDESIGNERSQLEDITOR_H

#include <qwidget.h>
class KTextEdit;

namespace KTextEditor
{
	class Document;
	class View;
}

class KEXIEXTWIDGETS_EXPORT KexiQueryDesignerSQLEditor : public QWidget
{
	Q_OBJECT

	public:
		KexiQueryDesignerSQLEditor(QWidget *parent, const char *name = 0);
		~KexiQueryDesignerSQLEditor();

		QString		getText();
		void		jump(int col);

	protected:
		virtual bool	eventFilter(QObject *w, QEvent *ev);

	signals:
		void		execQ();

	private:
#ifndef Q_WS_WIN //(TEMP)
		KTextEditor::Document	*m_doc;
		KTextEditor::View	*m_view;
#else
		KTextEdit *m_view;
#endif
};

#endif
