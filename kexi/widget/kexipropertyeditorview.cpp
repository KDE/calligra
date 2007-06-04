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

#include "kexipropertyeditorview.h"
#include <KexiMainWindowIface.h>
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

KexiObjectInfoLabel::KexiObjectInfoLabel(QWidget* parent)
 : QWidget(parent)
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

void KexiObjectInfoLabel::setObjectClassIcon(const QString& name)
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

void KexiObjectInfoLabel::setObjectName(const QString& name)
{
	m_objectName = name;
	updateName();
}

void KexiObjectInfoLabel::updateName()
{
	QString txt( m_className );
	if (txt.isEmpty())
		txt = m_objectName;
	else if (!m_objectName.isEmpty())
		txt += QString(" \"%1\"").arg(m_objectName);
	m_objectNameLabel->setText(txt);
}

void KexiObjectInfoLabel::setBuddy( QWidget * buddy )
{
	m_objectNameLabel->setBuddy(buddy);
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

KexiPropertyEditorView::KexiPropertyEditorView(QWidget* parent)
	: QWidget(parent)
	, d(new Private())
{
	setWindowTitle(i18n("Properties"));
	//TODO: set a nice icon
	setWindowIcon(KexiMainWindowIface::global()->thisWidget()->windowIcon());

	Q3VBoxLayout *lyr = new Q3VBoxLayout(this);

	//add object class info
	d->objectInfoLabel = new KexiObjectInfoLabel(this);
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
	d->objectInfoLabel->setBuddy(d->editor);
	setFocusPolicy(Qt::WheelFocus);

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

/*! Updates \a infoLabel widget by reusing properties provided by property set \a set.
 Read documentation of KexiPropertyEditorView class for information about accepted properties.
 If \a set is 0 and \a textToDisplayForNullSet string is not empty, this string is displayed 
 (without icon or any other additional part). 
 If \a set is 0 and \a textToDisplayForNullSet string is empty, the \a infoLabel widget becomes 
 hidden.
*/
void KexiPropertyEditorView::updateInfoLabelForPropertySet(KexiObjectInfoLabel *infoLabel, 
	KoProperty::Set* set, const QString& textToDisplayForNullSet)
{
	QString className, iconName, objectName;
	if (set) {
		if (set->contains("this:classString"))
			className = (*set)["this:classString"].value().toString();
		if (set->contains("this:iconName"))
			iconName = (*set)["this:iconName"].value().toString();
		const bool useCaptionAsObjectName = set->contains("this:useCaptionAsObjectName")
			&& (*set)["this:useCaptionAsObjectName"].value().toBool();
		if (set->contains(useCaptionAsObjectName ? "caption" : "name"))
			objectName = (*set)[useCaptionAsObjectName ? "caption" : "name"].value().toString();
	}
	if (!set || objectName.isEmpty()) {
		objectName = textToDisplayForNullSet;
		className.clear();
		iconName.clear();
	}

	if (className.isEmpty() && objectName.isEmpty())
		infoLabel->hide();
	else
		infoLabel->show();

	if (infoLabel->objectClassName() == className 
		&& infoLabel->objectClassIcon() == iconName
		&& infoLabel->objectName() == objectName)
		return;

	infoLabel->setObjectClassIcon(iconName);
	infoLabel->setObjectClassName(className);
	infoLabel->setObjectName(objectName);
}

void KexiPropertyEditorView::slotPropertySetChanged(KoProperty::Set* set)
{
	//update information about selected object
	updateInfoLabelForPropertySet(d->objectInfoLabel, set);
	d->editor->setEnabled(set);
}

#include "kexipropertyeditorview.moc"
