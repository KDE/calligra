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

#ifndef WIDGETWATCHER_H
#define WIDGETWATCHER_H

#include <qmap.h>
#include <qobject.h>
#include <qvariant.h>

class QWidget;
class PropertyBuffer;
class QDomDocument;

typedef QMap<QString, int> NameCounter;

namespace KFormEditor
{
	class WidgetProvider;
	class WidgetContainer;

	class WidgetWatcher : public QObject, public QMap<char *, QObject *>
	{
		Q_OBJECT

		public:
			WidgetWatcher(QObject *parent, PropertyBuffer *b, const char *name=0);
			~WidgetWatcher();

			QString	genName(const QString &base);
			QString genName(QObject *);

			QByteArray	store(WidgetContainer *parent);
			void		load(WidgetContainer *p, WidgetProvider *w, const QByteArray &data);

		protected:
			QDomElement property(QDomDocument *parent, const QString &name, const QVariant &value);
			void		setUpWidget(WidgetContainer *p, WidgetProvider *w, const QDomElement &d);

		public slots:
			void	slotNameChanged(QObject *, const char *);

		private:
			NameCounter	m_nameCounter;
			PropertyBuffer	*m_buffer;
	};
};

#endif
