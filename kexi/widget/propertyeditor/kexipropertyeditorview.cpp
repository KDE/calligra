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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexipropertyeditorview.h"
#include "kexipropertyeditor.h"
#include "kexipropertybuffer.h"
#include "keximainwindow.h"

#include <klocale.h>
#include <kiconloader.h>

#include <qlayout.h>
#include <qlabel.h>

class KexiPropertyEditorView::Private
{
	public:
		Private()
		{
		}
		KexiPropertyEditor *editor;
		QLabel *objectIcon;
		QString iconName;
		QLabel *objectClassName;
};

KexiPropertyEditorView::KexiPropertyEditorView(KexiMainWindow *mainWin)
	: KexiViewBase(mainWin, mainWin, "KexiPropertyEditorView")
	, d(new Private())
{
	setCaption(i18n("Properties"));
	//TODO: set a nice icon
	setIcon(*mainWin->icon());

	QVBoxLayout *lyr = new QVBoxLayout(this);

	//add object class info
	QHBoxLayout *vlyr = new QHBoxLayout(lyr);
	d->objectIcon = new QLabel(this);
	d->objectIcon->setMargin(2);
	d->objectIcon->setFixedHeight( IconSize(KIcon::Small) + 2 + 2 );
	vlyr->addWidget(d->objectIcon);
	d->objectClassName = new QLabel(this);
	d->objectClassName->setMargin(2);
	d->objectClassName->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	vlyr->addWidget(d->objectClassName);

	d->editor = new KexiPropertyEditor(this, true /*AutoSync*/, "propeditor");
	lyr->addWidget(d->editor);
	setFocusProxy(d->editor);

	connect(d->editor, SIGNAL(bufferChanged(KexiPropertyBuffer*)), 
		this, SLOT(slotBufferChanged(KexiPropertyBuffer*)));

	d->iconName = "dummy";
	slotBufferChanged(0);
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

void KexiPropertyEditorView::setGeometry ( const QRect &r )
{
	KexiViewBase::setGeometry(r);
}

void KexiPropertyEditorView::resize (  int w, int h  )
{
	KexiViewBase::resize( w, h );
}

KexiPropertyEditor *KexiPropertyEditorView::editor() const
{
	return d->editor;
}

void KexiPropertyEditorView::slotBufferChanged(KexiPropertyBuffer* buffer)
{
	//update information about selected object
	QString iconName, className;
	if (buffer && buffer->hasProperty("this:className")) {
		className = (*buffer)["this:className"].value().toString();
		iconName = (*buffer)["this:iconName"].value().toString();
	}
	if (d->objectClassName->text() == className && d->iconName == iconName)
		return;
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
			d->objectIcon->setFixedWidth( IconSize(KIcon::Small) + 2 + 2 );
			d->objectIcon->setPixmap(SmallIcon(iconName));
		}
		d->objectClassName->setText(className);
		d->objectClassName->show();
		d->objectIcon->show();
	}
}

#include "kexipropertyeditorview.moc"
