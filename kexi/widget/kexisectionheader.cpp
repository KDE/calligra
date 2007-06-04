/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexisectionheader.h"
#include <kexiutils/utils.h>
#include <KexiView.h>

#include <qlabel.h>
#include <QBoxLayout>
#include <khbox.h>
#include <qtooltip.h>
#include <QEvent>

#include <KIcon>
#include <kpushbutton.h>

class KexiSectionHeader::BoxLayout : public QBoxLayout
{
	public:
		BoxLayout( KexiSectionHeader* parent, QBoxLayout::Direction d, int margin = 0, 
			int spacing = -1 );
		virtual void addItem( QLayoutItem * item );
		QPointer<KexiView> view;
};

//==========================

//! @internal
class KexiSectionHeader::Private
{
	public:
		Private() 
		{
		}
	
		Qt::Orientation orientation;
		QLabel *lbl;
		KexiSectionHeader::BoxLayout *lyr;
		KHBox *lbl_b;
};

//==========================

KexiSectionHeader::KexiSectionHeader(const QString &caption, 
	Qt::Orientation o, QWidget* parent )
	: QWidget(parent)
	, d( new Private() )
{
	setObjectName("KexiSectionHeader");
	d->orientation = o;
	d->lyr = new BoxLayout( this, 
		d->orientation==Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight );
	d->lbl_b = new KHBox(this);
	d->lyr->addWidget(d->lbl_b);
	d->lbl = new QLabel(QString(" ")+caption, d->lbl_b);
	d->lbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	d->lbl->setFocusPolicy(Qt::StrongFocus);
	d->lbl->installEventFilter(this);
	installEventFilter(this);
	setWindowTitle(caption);
}

KexiSectionHeader::~KexiSectionHeader()
{
	delete d;
}

void KexiSectionHeader::setWidget( QWidget * widget )
{
	QLayoutItem *item = d->lyr->itemAt(1); //for sanity
	if (item)
		d->lyr->removeItem(item);
	d->lyr->addWidget(widget);
}

void KexiSectionHeader::addButton(const KIcon& icon, const QString& toolTip,
	const QObject * receiver, const char * member)
{
	KPushButton *btn = new KPushButton(icon, QString(), d->lbl_b);
	btn->setFlat(true);
	btn->setFocusPolicy(Qt::NoFocus);
	btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	if (receiver && member) {
		connect(btn, SIGNAL(clicked()), receiver, member);
	}

	if (!icon.isNull()) {
		QFontMetrics fm(d->lbl->font());
		btn->setMaximumHeight( qMax(fm.height(), 16) );
	}
	if (!toolTip.isEmpty())
		btn->setToolTip( toolTip);
}

bool KexiSectionHeader::eventFilter( QObject *o, QEvent *e )
{
	if (o == d->lbl && e->type()==QEvent::MouseButtonRelease) {//|| e->type()==QEvent::FocusOut) {// && o->inherits("QWidget")) {
		if (d->lyr->view)
			d->lyr->view->setFocus();
//		if (KexiUtils::hasParent( this, static_cast<QWidget*>(o))) {
//			d->lbl->setPaletteBackgroundColor( e->type()==QEvent::FocusIn ? red : blue);
//		}
	}
	return QWidget::eventFilter(o,e);
}

void KexiSectionHeader::slotFocus(bool in)
{
	in = in || focusWidget()==this;
	d->lbl->setPaletteBackgroundColor( 
		in ? palette().active().color(QColorGroup::Highlight) : palette().active().color(QColorGroup::Background) );
	d->lbl->setPaletteForegroundColor( 
		in ? palette().active().color(QColorGroup::HighlightedText) : palette().active().color(QColorGroup::Foreground) );
}

QSize KexiSectionHeader::sizeHint() const
{
	if (!d->lyr->view)
		return QWidget::sizeHint();
	QSize s = d->lyr->view->sizeHint();
	return QSize(s.width(), d->lbl->sizeHint().height() + s.height());
}

/*void KexiSectionHeader::setFocus()
{
	if (d->lyr->view)
		d->lyr->view->setFocus();
	else
		QWidget::setFocus();
}*/

//======================

KexiSectionHeader::BoxLayout::BoxLayout( KexiSectionHeader* parent, Direction d, int margin, 
	int spacing )
 : QBoxLayout(parent, d, margin, spacing )
{
}

void KexiSectionHeader::BoxLayout::addItem( QLayoutItem * item )
{
	QBoxLayout::addItem( item );
	if (item->widget()) {
		item->widget()->installEventFilter( mainWidget() );
		if (dynamic_cast<KexiView*>(item->widget())) {
			view = dynamic_cast<KexiView*>(item->widget());
			KexiSectionHeader *sh = static_cast<KexiSectionHeader*>(mainWidget());
			connect(view,SIGNAL(focus(bool)),sh,SLOT(slotFocus(bool)));
			sh->d->lbl->setBuddy(item->widget());
		}
	}
}


#include "kexisectionheader.moc"

