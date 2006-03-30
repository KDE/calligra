/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIPROPERTYEDITORVIEW_H
#define KEXIPROPERTYEDITORVIEW_H

//#include "kexiviewbase.h"
#include <qwidget.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3CString>

class QLabel;
class KexiMainWindow;

namespace KoProperty {
	class Editor;
	class Set;
}

//! Helper class displaying small icon with class name and object name
class KEXIEXTWIDGETS_EXPORT KexiObjectInfoLabel : public QWidget
{
	public:
		KexiObjectInfoLabel(QWidget* parent, const char* name = 0);
		~KexiObjectInfoLabel();

		void setObjectClassIcon(const Q3CString& name);
		Q3CString objectClassIcon() const { return m_classIcon; }
		void setObjectClassName(const QString& name);
		QString objectClassName() const { return m_className; }
		void setObjectName(const Q3CString& name);
		Q3CString objectName() const { return m_objectName; }
	protected:
		void updateName();

		QString m_className;
		Q3CString m_classIcon, m_objectName;
		QLabel *m_objectIconLabel, *m_objectNameLabel;
};

/*! The container (acts as a dock window) for KexiPropertyEditor
*/
class KEXIEXTWIDGETS_EXPORT KexiPropertyEditorView : public QWidget //KexiViewBase
{
	Q_OBJECT

	public:
		KexiPropertyEditorView(KexiMainWindow *mainWin, QWidget* parent);
		virtual ~KexiPropertyEditorView();

		virtual QSize sizeHint() const;
		virtual QSize minimumSizeHint() const;
		KoProperty::Editor *editor() const;

//	public slots:
//		virtual void setGeometry( const QRect &r );
//		virtual void resize( int w, int h );

	protected slots:
		void slotPropertySetChanged(KoProperty::Set* );

	protected:
		class Private;
		Private *d;
};

#endif
