/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXITABLEHEADER_H
#define KEXITABLEHEADER_H

#include <qheader.h>

class QPainter;

class KEXIDATATABLE_EXPORT KexiTableHeader : public QHeader
{
	Q_OBJECT

	public:
		KexiTableHeader(QWidget *parent, const char *name=0);
		~KexiTableHeader();
		
		void		setCurrentRow(int row=-1);
		void		setInsertRow(int row=-1);
		
		void		setCellHeight(int height=14);
//		void	set

	protected:
		void		paintEvent(QPaintEvent *ev);
		void		paintSectionLabel(QPainter *p, int index, const QRect & fr);

		int		m_currentRow; 
		int		m_insertRow;

		int		m_cellHeight;

		QPainter	*m_painter;
};

#endif
