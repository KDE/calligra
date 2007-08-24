/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002 Till Busch <till@bux.at>
   Copyright (C) 2002 Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIRECORDMARKER_H
#define KEXIRECORDMARKER_H

#include <kexi_export.h>
#include <QWidget>
#include <QPaintEvent>

class QImage;

//! \brief Record marker, usually displayed at the left side of a table view or a continuous form.
class KEXIGUIUTILS_EXPORT KexiRecordMarker : public QWidget
{
	Q_OBJECT

	public:
		KexiRecordMarker(QWidget *parent);
		virtual ~KexiRecordMarker();

		int rows() const;

		static QImage* penImage();
		static QImage* plusImage();

	public slots:
		void setOffset(int offset);
		void setCellHeight(int cellHeight);
		void setCurrentRow(int row);
		void setHighlightedRow(int row);

		/*! Sets 'edit row' flag for \a row. Use row==-1 if you want to switch the flag off. */
		void setEditRow(int row);
		void showInsertRow(bool show);

		QColor selectionBackgroundColor() const;
		void setSelectionBackgroundColor(const QColor &color);

		void addLabel(bool upd=true);
		void removeLabel(bool upd=true);

		/*! Adds \a num labels */
		void addLabels(int num, bool upd=true);

		void clear(bool upd=true);

	protected:
		virtual void paintEvent(QPaintEvent *e);
		
		class Private;
		Private * const d;
};

#endif
