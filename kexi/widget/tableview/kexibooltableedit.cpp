/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexibooltableedit.h"

#include <kexidb/field.h>

#include <qpainter.h>
#include <qapplication.h>
#include <qclipboard.h>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <kglobalsettings.h>


KexiBoolTableEdit::KexiBoolTableEdit(KexiTableViewColumn &column, QWidget *parent)
 : KexiTableEdit(column, parent)
{
	kDebug() << "KexiBoolTableEdit: m_origValue.typeName()==" << m_origValue.typeName() << endl;
	kDebug() << "KexiBoolTableEdit: type== " << field()->typeName() << endl;
	m_hasFocusableWidget = false;
	m_acceptEditorAfterDeleteContents = true;
	m_usesSelectedTextColor = false;
}

KexiBoolTableEdit::~KexiBoolTableEdit()
{
}

void KexiBoolTableEdit::setValueInternal(const QVariant& /*add*/, bool /*removeOld*/)
{
	m_currentValue = m_origValue;
	//nothing to do more...
}

bool KexiBoolTableEdit::valueIsNull()
{
	return m_currentValue.isNull();
}

bool KexiBoolTableEdit::valueIsEmpty()
{
	return m_currentValue.isNull();
}

QVariant KexiBoolTableEdit::value()
{
//	ok = true;
	return m_currentValue;
}

void KexiBoolTableEdit::clear()
{
	if (field()->isNotNull())
		m_currentValue = QVariant(false);
	else
		m_currentValue = QVariant();
}

bool KexiBoolTableEdit::cursorAtStart()
{
	return true;
}

bool KexiBoolTableEdit::cursorAtEnd()
{
	return true;
}

void KexiBoolTableEdit::setupContents( QPainter *p, bool focused, const QVariant& val, 
	QString &txt, int &align, int &x, int &y_offset, int &w, int &h  )
{
	Q_UNUSED(focused);
	Q_UNUSED(txt);
	Q_UNUSED(align);
	Q_UNUSED(x);
#ifdef Q_WS_WIN
//	x = 1;
	y_offset = -1;
#else
//	x = 1;
	y_offset = 0;
#endif
	if (p) {
		int s = qMax(h - 5, 12);
		s = qMin( h-3, s );
		s = qMin( w-3, s );//avoid too large box
		QRect r( qMax( w/2 - s/2, 0 ) , h/2 - s/2 /*- 1*/, s, s);
//already set ouotside:		p->setPen(QPen(colorGroup().text(), 1));
		p->drawRect(r);
		if (val.isNull()) { // && !field()->isNotNull()) {
			p->drawText( r, Qt::AlignCenter, "?" );
		}
		else if (val.toBool()) {
			p->drawLine(r.x(), r.y(), r.right(), r.bottom());
			p->drawLine(r.x(), r.bottom(), r.right(), r.y());
		}
	}
}

void KexiBoolTableEdit::clickedOnContents()
{
	if (field()->isNotNull())
		m_currentValue = QVariant( !m_currentValue.toBool() );
	else {
		// null allowed: use the cycle: true -> false -> null
		if (m_currentValue.isNull())
			m_currentValue = QVariant( true );
		else
			m_currentValue = m_currentValue.toBool() ? QVariant( false ) : QVariant();
	}
}

void KexiBoolTableEdit::handleAction(const QString& actionName)
{
	if (actionName=="edit_paste") {
		emit editRequested();
		bool ok;
		const int value = qApp->clipboard()->text( QClipboard::Clipboard ).toInt(&ok);
		if (ok) {
			m_currentValue = (value==0) ? QVariant(false) : QVariant(true);
		}
		else {
			m_currentValue = field()->isNotNull() 
				? QVariant(0)/*0 instead of NULL - handle case when null is not allowed*/
				: QVariant();
		}
		repaintRelatedCell();
	}
	else if (actionName=="edit_cut") {
		emit editRequested();
//! @todo handle defaultValue...
		m_currentValue = field()->isNotNull() 
			? QVariant(0)/*0 instead of NULL - handle case when null is not allowed*/
			: QVariant();
		handleCopyAction(m_origValue, QVariant());
		repaintRelatedCell();
	}
}

void KexiBoolTableEdit::handleCopyAction(const QVariant& value, const QVariant& visibleValue)
{
	Q_UNUSED(visibleValue);
	if (value.type()==QVariant::Bool)
		qApp->clipboard()->setText(value.toBool() ? "1" : "0");
	else
		qApp->clipboard()->setText(QString::null);
}

int KexiBoolTableEdit::widthForValue( QVariant &val, const QFontMetrics &fm )
{
	Q_UNUSED(fm);
	return val.value<QPixmap>().width();
}

//======================================================

KEXI_CELLEDITOR_FACTORY_ITEM_IMPL(KexiBoolEditorFactoryItem, KexiBoolTableEdit)

#include "kexibooltableedit.moc"
