/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDBFORM_H
#define KEXIDBFORM_H

#include <qwidget.h>
#include <qpixmap.h>

#include <kexidataiteminterface.h>
#include "form.h"

//! A DB-aware form widget
class KexiDBForm : public QWidget, public KFormDesigner::FormWidget, public KexiDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true);
	//Q_PROPERTY(bool RecordNavigator READ navigatorShown WRITE showRecordNavigator DESIGNABLE true)

	public:
		KexiDBForm(QWidget *parent, const char *name="kexi_dbform");
		virtual ~KexiDBForm();

		inline QString dataSource() const { return KexiDataItemInterface::dataSource(); }
		inline void setDataSource(const QString &ds) { KexiDataItemInterface::setDataSource(ds); }

		//! no effect
		QVariant value() { return QVariant(); }

		virtual void drawRect(const QRect& r, int type);
		virtual void drawRects(const QValueList<QRect> &list, int type);
		virtual void initBuffer();
		virtual void clearForm();
		virtual void highlightWidgets(QWidget *from, QWidget *to/*, const QPoint &p*/);

		virtual QSize sizeHint() const;

	protected:
		//! no effect
		virtual void setValueInternal(const QVariant&) {};

		QPixmap buffer; //!< stores grabbed entire form's area for redraw
		QRect prev_rect; //!< previously selected rectangle
};

#endif

