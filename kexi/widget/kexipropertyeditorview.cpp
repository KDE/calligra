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

#include "kexipropertyeditorview.h"
#include "keximainwindow.h"
#include <koproperty/set.h>
#include <koproperty/editor.h>
#include <koproperty/property.h>

#include <klocale.h>
#include <kiconloader.h>

#include <qlayout.h>
#include <qlabel.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <Q3CString>

KexiObjectInfoLabel::KexiObjectInfoLabel(QWidget* parent, const char* name)
 : QWidget(parent, name)
{
	Q3HBoxLayout *hlyr = new Q3HBoxLayout(this);
	m_objectIconLabel = new QLabel(this);
	m_objectIconLabel->setMargin(2);
	setFixedHeight( IconSize(K3Icon::Small) + 2 + 2 );
	hlyr->addWidget(m_objectIconLabel);
	m_objectNameLabel = new QLabel(this);
	m_objectNameLabel->setMargin(2);
	m_objectNameLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	hlyr->addWidget(m_objectNameLabel);
}

KexiObjectInfoLabel::~KexiObjectInfoLabel()
{
}

void KexiObjectInfoLabel::setObjectClassIcon(const Q3CString& name)
{
	m_classIcon = name;
	if (m_classIcon.isEmpty())
		m_objectIconLabel->setFixedWidth( 0 );
	else
		m_objectIconLabel->setFixedWidth( IconSize(K3Icon::Small) + 2 + 2 );
	m_objectIconLabel->setPixmap( SmallIcon(name) );
}

void KexiObjectInfoLabel::setObjectClassName(const QString& name)
{
	m_className = name;
	updateName();
}

void KexiObjectInfoLabel::setObjectName(const Q3CString& name)
{
	m_objectName = name;
	updateName();
}

void KexiObjectInfoLabel::updateName()
{
	QString txt = m_className;
	if (!m_objectName.isEmpty())
		txt += QString(" \"%1\"").arg(m_objectName);
	m_objectNameLabel->setText(txt);
}

//------------------------------

//! @internal
class KexiPropertyEditorView::Private
{
	public:
		Private()
		{
		}
		KoProperty::Editor *editor;
//		QLabel *objectIcon;
//		QString iconName;
//		QLabel *objectClassName;
		KexiObjectInfoLabel *objectInfoLabel;
};

//------------------------------

KexiPropertyEditorView::KexiPropertyEditorView(KexiMainWindow *mainWin, QWidget* parent)
	: QWidget(parent, "KexiPropertyEditorView")
	, d(new Private())
{
	setCaption(i18n("Properties"));
	//TODO: set a nice icon
	setIcon(*mainWin->icon());

	Q3VBoxLayout *lyr = new Q3VBoxLayout(this);

	//add object class info
	d->objectInfoLabel = new KexiObjectInfoLabel(this, "KexiObjectInfoLabel");
	lyr->addWidget(d->objectInfoLabel);

	/*
	QHBoxLayout *vlyr = new QHBoxLayout(lyr);
	d->objectIcon = new QLabel(this);
	d->objectIcon->setMargin(2);
	d->objectIcon->setFixedHeight( IconSize(K3Icon::Small) + 2 + 2 );
	vlyr->addWidget(d->objectIcon);
	d->objectClassName = new QLabel(this);
	d->objectClassName->setMargin(2);
	d->objectClassName->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	vlyr->addWidget(d->objectClassName);*/

	d->editor = new KoProperty::Editor(this, true /*AutoSync*/, "propeditor");
	lyr->addWidget(d->editor);
	setFocusProxy(d->editor);

	connect(d->editor, SIGNAL(propertySetChanged(KoProperty::Set*)), 
		this, SLOT(slotPropertySetChanged(KoProperty::Set*)));

//	d->iconName = "dummy";
	slotPropertySetChanged(0);
}

KexiPropertyEditorView::~KexiPropertyEditorView()
{
	delete d;
}

QSize KexiPropertyEditorView::sizeHint() const
{
	return QSize(200,200);//m_editor->sizeHint();
}

QSize KexiPropertyEditorView::minimumSizeHint() const
{
	return QSize(200,200);//m_editor->sizeHint();
}

/*void KexiPropertyEditorView::setGeometry ( const QRect &r )
{
	QWidget::setGeometry(r);
}

void KexiPropertyEditorView::resize (  int w, int h  )
{
	QWidget::resize( w, h );
}*/

KoProperty::Editor *KexiPropertyEditorView::editor() const
{
	return d->editor;
}

void KexiPropertyEditorView::slotPropertySetChanged(KoProperty::Set* set)
{
	//update information about selected object
	QString className;
	Q3CString iconName, objectName;
	if (set) {
		if (set->contains("this:classString"))
			className = (*set)["this:classString"].value().toString();
		if (set->contains("this:iconName"))
			iconName = (*set)["this:iconName"].value().toCString();
		if (set->contains("name"))
			objectName = (*set)["name"].value().toCString();
	}

	if (className.isEmpty()) {
		d->objectInfoLabel->hide();
	}
	else {
		d->objectInfoLabel->show();
	}

	if (d->objectInfoLabel->objectClassName() == className 
		&& d->objectInfoLabel->objectClassIcon() == iconName
		&& d->objectInfoLabel->objectName() == objectName)
		return;

	d->objectInfoLabel->setObjectClassIcon(iconName);
	d->objectInfoLabel->setObjectClassName(className);
	d->objectInfoLabel->setObjectName(objectName);

/*
	if (className.isEmpty()) {
		d->objectClassName->hide();
		d->objectIcon->hide();
		d->objectIcon->setFixedWidth( 0 );
	}
	else {
		if (iconName.isEmpty()) {
			d->objectIcon->setFixedWidth( 0 );
			d->objectIcon->hide();
			d->objectIcon->setPixmap(QPixmap());
		}
		else {
			d->objectIcon->setFixedWidth( IconSize(K3Icon::Small) + 2 + 2 );
			d->objectIcon->setPixmap(SmallIcon(iconName));
		}
		d->objectClassName->setText(className);
		d->objectClassName->show();
		d->objectIcon->show();
	}*/
}

#include "kexipropertyeditorview.moc"
