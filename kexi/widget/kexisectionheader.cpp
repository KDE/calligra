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
#include "kexiviewbase.h"
#include <kexiutils/utils.h>

#include <qlabel.h>
#include <qlayout.h>
#include <q3hbox.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <QEvent>

#include <kiconloader.h>
#include <kpushbutton.h>

class KexiSectionHeader::BoxLayout : public Q3BoxLayout
{
	public:
		BoxLayout( KexiSectionHeader* parent, Qt::Orientation d, int margin = 0, 
			int spacing = -1, const char * name = 0 );
		virtual void addItem( QLayoutItem * item );
		QPointer<KexiViewBase> view;
};

//==========================

//! @internal
class KexiSectionHeaderPrivate
{
	public:
		KexiSectionHeaderPrivate() 
		{
		}
	
		Qt::Orientation orientation;
		QLabel *lbl;
		KexiSectionHeader::BoxLayout *lyr;
		Q3HBox *lbl_b;
};

//==========================

KexiSectionHeader::KexiSectionHeader(const QString &caption, Orientation o, QWidget* parent )
	: QWidget(parent, )
	, d( new KexiSectionHeaderPrivate() )
{
	setObjectName("KexiSectionHeader");
	d->orientation = o;
	d->lyr = new BoxLayout( this, d->orientation==Qt::Vertical ? Q3BoxLayout::TopToBottom : Q3BoxLayout::LeftToRight );
	d->lyr->setAutoAdd(true);
	d->lbl_b = new Q3HBox(this);
	d->lbl = new QLabel(QString(" ")+caption, d->lbl_b);
	d->lbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	d->lbl->setFocusPolicy(StrongFocus);
	d->lbl->installEventFilter(this);
	installEventFilter(this);
	setCaption(caption);
}

KexiSectionHeader::~KexiSectionHeader()
{
	delete d;
}

void KexiSectionHeader::addButton(const QString& icon, const QString& toolTip,
	const QObject * receiver, const char * member)
{
	KPushButton *btn = new KPushButton(d->lbl_b);
	btn->setFlat(true);
	btn->setFocusPolicy(Qt::NoFocus);
	btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	if (receiver && member) {
		connect(btn, SIGNAL(clicked()), receiver, member);
	}

	if (!icon.isEmpty()) {
		QIcon iset = KIcon(icon);
		btn->setIconSet( iset );
		QFontMetrics fm(d->lbl->font());
		btn->setMaximumHeight( qMax(fm.height(), 16) );
	}
	if (!toolTip.isEmpty()) {
		btn->setToolTip( toolTip);
	}
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

KexiSectionHeader::BoxLayout::BoxLayout( KexiSectionHeader* parent, Direction d, int margin, int spacing, const char * name )
 : Q3BoxLayout(parent, d, margin, spacing, name )
{
}

void KexiSectionHeader::BoxLayout::addItem( QLayoutItem * item )
{
	Q3BoxLayout::addItem( item );
	if (item->widget()) {
		item->widget()->installEventFilter( mainWidget() );
		if (item->widget()->inherits("KexiViewBase")) {
			view = static_cast<KexiViewBase*>(item->widget());
			KexiSectionHeader *sh = static_cast<KexiSectionHeader*>(mainWidget());
			connect(view,SIGNAL(focus(bool)),sh,SLOT(slotFocus(bool)));
			sh->d->lbl->setBuddy(item->widget());
		}
	}
}


#include "kexisectionheader.moc"

