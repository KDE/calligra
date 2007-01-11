/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

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

class QLabel;
class KexiMainWindow;

namespace KoProperty {
	class Editor;
	class Set;
}

//! @short Helper class displaying small icon with class name and object name
/*! The info label is displayed in a form:
 <i>[ObjectClassIcon] ClassName "ObjectName"</i>

 The <i>ObjectClassIcon</i> is optional. If "ClassName" is empty, the information 
 is displayed as:
 <i>[ObjectClassIcon] ObjectName</i>

 Example uses:
 - [button_icon] Button "quit"
 - [label_icon] Label "welcome"
*/
class KEXIEXTWIDGETS_EXPORT KexiObjectInfoLabel : public QWidget
{
	public:
		KexiObjectInfoLabel(QWidget* parent, const char* name = 0);
		~KexiObjectInfoLabel();

		void setObjectClassIcon(const QString& name);
		QString objectClassIcon() const { return m_classIcon; }
		void setObjectClassName(const QString& name);
		QString objectClassName() const { return m_className; }
		void setObjectName(const QString& name);
		QString objectName() const { return m_objectName; }
		void setBuddy( QWidget * buddy );
	protected:
		void updateName();

		QString m_className;
		QString m_classIcon, m_objectName;
		QLabel *m_objectIconLabel, *m_objectNameLabel;
};

//! @short The container (acts as a dock window) for KexiPropertyEditor.
/*! The widget displays KexiObjectInfoLabel on its top, to show user what
 object the properties belong to. Read KexiObjectInfoLabel documentation for 
 the description what information is displayed.

 There are properties obtained from KexiMainWindow's current property set 
 that help to customize displaying this information:
 - "this:classString property" of type string describes object's class name
 - "this:iconName" property of type string describes class name
 - "name" or "caption" property of type string describes object's name
 - "this:useCaptionAsObjectName" property of type boolean forces displaying "caption"
   property instead of "name" - this can be usable when we know that "caption" properties
   are available for a given type of objects (this is the case for Table Designer fields)
*/
class KEXIEXTWIDGETS_EXPORT KexiPropertyEditorView : public QWidget
{
	Q_OBJECT

	public:
		KexiPropertyEditorView(KexiMainWindow *mainWin, QWidget* parent);
		virtual ~KexiPropertyEditorView();

		/*! Helper function. Updates \a infoLabel widget by reusing properties provided 
		 by property set \a set.
		 Read documentation of KexiPropertyEditorView class for information about accepted properties.
		 If \a set is 0 and \a textToDisplayForNullSet string is not empty, this string is displayed 
		 (without icon or any other additional part). 
		 If \a set is 0 and \a textToDisplayForNullSet string is empty, the \a infoLabel widget becomes 
		 hidden. */
		static void updateInfoLabelForPropertySet(
			KexiObjectInfoLabel *infoLabel, KoProperty::Set* set, 
			const QString& textToDisplayForNullSet = QString::null);

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
