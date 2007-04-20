/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXITOOLTIP_H
#define KEXITOOLTIP_H

#include <kexi_export.h>

#include <QWidget>
#include <QVariant>

//! \brief A tooltip that can display rich content
class KEXIGUIUTILS_EXPORT KexiToolTip : public QWidget
{
	Q_OBJECT
	public:
		KexiToolTip(const QVariant& value, QWidget* parent);
		virtual ~KexiToolTip();

		virtual QSize sizeHint() const;

	public slots:
		virtual void show();

	protected:
		virtual void paintEvent( QPaintEvent *pev );
		virtual void drawFrame(QPainter& p);
		virtual void drawContents(QPainter& p);

		QVariant m_value;
};

#endif
