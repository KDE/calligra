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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexisectionheader.h"
#include "kexiviewbase.h"
#include "kexi_utils.h"


class BoxLayout : public QBoxLayout
{
	public:
		BoxLayout( KexiSectionHeader* parent, Direction d, int margin = 0, int spacing = -1, const char * name = 0 );
		virtual void BoxLayout::addItem( QLayoutItem * item );
		QGuardedPtr<KexiViewBase> view;
};


KexiSectionHeader::KexiSectionHeader(const QString &caption, Orientation o, QWidget* parent, const char * name )
	: QWidget(parent, name)
	, m_orientation(o)
{
	m_lyr = new BoxLayout( this, m_orientation==Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight );
	m_lyr->setAutoAdd(true);
	m_lbl = new QLabel(QString(" ")+caption, this);
	m_lbl->installEventFilter(this);
	setCaption(caption);
}

bool KexiSectionHeader::eventFilter( QObject *o, QEvent *e )
{
	if (o == m_lbl && e->type()==QEvent::MouseButtonRelease) {//|| e->type()==QEvent::FocusOut) {// && o->inherits("QWidget")) {
		if (m_lyr->view)
			m_lyr->view->setFocus();
//		if (Kexi::hasParent( this, static_cast<QWidget*>(o))) {
//			m_lbl->setPaletteBackgroundColor( e->type()==QEvent::FocusIn ? red : blue);
//		}
	}
	return false;
}


void KexiSectionHeader::slotFocus(bool in) {
	in = in || focusWidget()==this;
	m_lbl->setPaletteBackgroundColor( 
		in ? palette().active().color(QColorGroup::Highlight) : palette().active().color(QColorGroup::Background) );
	m_lbl->setPaletteForegroundColor( 
		in ? palette().active().color(QColorGroup::HighlightedText) : palette().active().color(QColorGroup::Foreground) );
}

QSize KexiSectionHeader::sizeHint() const
{
	if (!m_lyr->view)
		return QWidget::sizeHint();
	QSize s = m_lyr->view->sizeHint();
	return QSize(s.width(), m_lbl->sizeHint().height() + s.height());
}



BoxLayout::BoxLayout( KexiSectionHeader* parent, Direction d, int margin, int spacing, const char * name )
 : QBoxLayout(parent, d, margin, spacing, name )
{
}

void BoxLayout::addItem( QLayoutItem * item ) {
	QBoxLayout::addItem( item );
	item->widget()->installEventFilter( mainWidget() );
	if (item->widget()->inherits("KexiViewBase")) {
		view = static_cast<KexiViewBase*>(item->widget());
		KexiSectionHeader *sh = static_cast<KexiSectionHeader*>(mainWidget());
		connect(view,SIGNAL(focus(bool)),sh,SLOT(slotFocus(bool)));
	}
}


#include "kexisectionheader.moc"

