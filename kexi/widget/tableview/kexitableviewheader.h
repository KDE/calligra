/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and,or
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

#ifndef KEXITABLEVIEWHEADER_H
#define KEXITABLEVIEWHEADER_H

#include <kexi_global.h>
#include <q3header.h>
#include <QPointer>
#include <QStyle>

//! @short A table view header with additional actions.
/*! Displays field description (Field::description()) text as tool tip, if available.
 Displays tool tips if a pointed section is not wide enough to fit its label text.

 \todo react on indexChange ( int section, int fromIndex, int toIndex ) signal
*/
class KEXIDATATABLE_EXPORT KexiTableViewHeader : public Q3Header
{
	Q_OBJECT

	public:
		KexiTableViewHeader(QWidget * parent = 0);

		virtual ~KexiTableViewHeader();

		int addLabel( const QString & s, int size = -1 );

		int addLabel( const QIconSet & iconset, const QString & s, int size = -1 );

		void removeLabel( int section );

		/*! Sets \a toolTip for \a section. */
		void setToolTip( int section, const QString & toolTip );

		virtual bool eventFilter(QObject * watched, QEvent * e);

		void setSelectedSection(int section);
		int selectedSection() const;

		QColor selectionBackgroundColor() const;
		void setSelectionBackgroundColor(const QColor &color);

	protected slots:
		void slotSizeChange(int section, int oldSize, int newSize );

	protected:
		virtual bool event( QEvent *event );
		virtual void paintSection( QPainter * p, int index, const QRect & fr );
		virtual void styleChanged();

		int m_lastToolTipSection;
		QRect m_toolTipRect;

		QStringList m_toolTips;
		QColor m_selectionBackgroundColor;
		QPointer<QStyle> m_privateStyle;
		int m_selectedSection;
		bool m_styleChangeEnabled : 1;
};

#endif
