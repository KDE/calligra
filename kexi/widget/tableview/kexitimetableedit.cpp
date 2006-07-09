/* This file is part of the KDE project
   Copyright (C) 2004,2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexitimetableedit.h"

#include <qapplication.h>
#include <qpainter.h>
#include <qvariant.h>
#include <qrect.h>
#include <qpalette.h>
#include <qcolor.h>
#include <qfontmetrics.h>
#include <qdatetime.h>
#include <qcursor.h>
#include <qpoint.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qdatetimeedit.h>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdatepicker.h>
#include <kdatetbl.h>
#include <klineedit.h>
#include <kpopupmenu.h>
#include <kdatewidget.h>

#include <kexiutils/utils.h>

KexiTimeFormatter::KexiTimeFormatter()
: m_hmsRegExp("(\\d*):(\\d*):(\\d*).*( am| pm){,1}", false/*!CS*/)
 , m_hmRegExp("(\\d*):(\\d*).*( am| pm){,1}", false/*!CS*/)
{
	QString tf( KGlobal::locale()->timeFormat() );
	//m_hourpos, m_minpos, m_secpos; are result of tf.find()
	QString hourVariable, minVariable, secVariable;

	//detect position of HOUR section: find %H or %k or %I or %l
	m_24h = true;
	m_hoursWithLeadingZero = true;
	m_hourpos = tf.find("%H", 0, true);
	if (m_hourpos>=0) {
		m_24h = true;
		m_hoursWithLeadingZero = true;
	}
	else {
		m_hourpos = tf.find("%k", 0, true);
		if (m_hourpos>=0) {
			m_24h = true;
			m_hoursWithLeadingZero = false;
		}
		else {
			m_hourpos = tf.find("%I", 0, true);
			if (m_hourpos>=0) {
				m_24h = false;
				m_hoursWithLeadingZero = true;
			}
			else {
				m_hourpos = tf.find("%l", 0, true);
				if (m_hourpos>=0) {
					m_24h = false;
					m_hoursWithLeadingZero = false;
				}
			}
		}
	}
	m_minpos = tf.find("%M", 0, true);
	m_secpos = tf.find("%S", 0, true); //can be -1
	m_ampmpos = tf.find("%p", 0, true); //can be -1

	if (m_hourpos<0 || m_minpos<0) {
		//set default: hr and min are needed, sec are optional
		tf = "%H:%M:%S";
		m_24h = true;
		m_hoursWithLeadingZero = false;
		m_hourpos = 0;
		m_minpos = 3;
		m_secpos = m_minpos + 3;
		m_ampmpos = -1;
	}
	hourVariable = tf.mid(m_hourpos, 2);

	m_inputMask = tf;
//	m_inputMask.replace( hourVariable, "00" );
//	m_inputMask.replace( "%M", "00" );
//	m_inputMask.replace( "%S", "00" ); //optional
	m_inputMask.replace( hourVariable, "99" );
	m_inputMask.replace( "%M", "99" );
	m_inputMask.replace( "%S", "00" ); //optional
	m_inputMask.replace( "%p", "AA" ); //am or pm
	m_inputMask += ";_";

	m_outputFormat = tf;
}

KexiTimeFormatter::~KexiTimeFormatter()
{
}

QString KexiTimeFormatter::timeToString( const QTime& time ) const
{
	if (!time.isValid())
		return QString::null;

	QString s(m_outputFormat);
	if (m_24h) {
		if (m_hoursWithLeadingZero)
			s.replace( "%H", QString::fromLatin1(time.hour()<10 ? "0" : "") + QString::number(time.hour()) );
		else
			s.replace( "%k", QString::number(time.hour()) );
	}
	else {
		int time12 = (time.hour()>12) ? (time.hour()-12) : time.hour();
		if (m_hoursWithLeadingZero)
			s.replace( "%I", QString::fromLatin1(time12<10 ? "0" : "") + QString::number(time12) );
		else
			s.replace( "%l", QString::number(time12) );
	}
	s.replace( "%M", QString::fromLatin1(time.minute()<10 ? "0" : "") + QString::number(time.minute()) );
	if (m_secpos>=0)
		s.replace( "%S", QString::fromLatin1(time.second()<10 ? "0" : "") + QString::number(time.second()) );
	if (m_ampmpos>=0)
		s.replace( "%p", KGlobal::locale()->translate( time.hour()>=12 ? "pm" : "am") );
	return s;
}

QTime KexiTimeFormatter::stringToTime( const QString& str )
{
	int hour, min, sec;
	bool pm = false;

	bool tryWithoutSeconds = true;
	if (m_secpos>=0) {
		if (-1 != m_hmsRegExp.search(str)) {
			hour = m_hmsRegExp.cap(1).toInt();
			min = m_hmsRegExp.cap(2).toInt();
			sec = m_hmsRegExp.cap(3).toInt();
			if (m_ampmpos >= 0 && m_hmsRegExp.numCaptures()>3)
				pm = m_hmsRegExp.cap(4).stripWhiteSpace().lower()=="pm";
			tryWithoutSeconds = false;
		}
	}
	if (tryWithoutSeconds) {
		if (-1 == m_hmRegExp.search(str))
			return QTime(99,0,0);
		hour = m_hmRegExp.cap(1).toInt();
		min = m_hmRegExp.cap(2).toInt();
		sec = 0;
		if (m_ampmpos >= 0 && m_hmRegExp.numCaptures()>2)
			pm = m_hmsRegExp.cap(4).lower()=="pm";
	}

	if (pm && hour < 12)
		hour += 12; //PM
	return QTime(hour, min, sec);
}

//------------------------------------------------

KexiTimeTableEdit::KexiTimeTableEdit(KexiTableViewColumn &column, QScrollView *parent)
 : KexiInputTableEdit(column, parent)
{
	setName("KexiTimeTableEdit");

//! @todo add QValidator so time like "99:88:77" cannot be even entered

	m_lineedit->setInputMask( m_formatter.inputMask() );
}

KexiTimeTableEdit::~KexiTimeTableEdit()
{
}

void KexiTimeTableEdit::setValueInternal(const QVariant& add_, bool removeOld)
{
	if (removeOld) {
		//new time entering... just fill the line edit
//! @todo cut string if too long..
		QString add(add_.toString());
		m_lineedit->setText(add);
		m_lineedit->setCursorPosition(add.length());
		return;
	}
	m_lineedit->setText( 
		m_formatter.timeToString( 
			//hack to avoid converting null variant to valid QTime(0,0,0)
			m_origValue.isValid() ? m_origValue.toTime() : QTime(99,0,0) 
		)
	);
	m_lineedit->setCursorPosition(0); //ok?
}

void KexiTimeTableEdit::setupContents( QPainter *p, bool focused, QVariant val, 
	QString &txt, int &align, int &x, int &y_offset, int &w, int &h )
{
	Q_UNUSED(p);
	Q_UNUSED(focused);
	Q_UNUSED(x);
	Q_UNUSED(w);
	Q_UNUSED(h);
#ifdef Q_WS_WIN
	y_offset = -1;
#else
	y_offset = 0;
#endif
	if (!val.isNull() && val.canCast(QVariant::Time))
		txt = m_formatter.timeToString(val.toTime());
	align |= AlignLeft;
}

bool KexiTimeTableEdit::valueIsNull()
{
	if (m_lineedit->text().replace(':',"").stripWhiteSpace().isEmpty())
		return true;
	return !timeValue().isValid();
}

bool KexiTimeTableEdit::valueIsEmpty()
{
	return valueIsNull();// OK? TODO (nonsense?)
}

QTime KexiTimeTableEdit::timeValue()
{
	return m_formatter.stringToTime( m_lineedit->text() );
}

QVariant KexiTimeTableEdit::value()
{
	if (m_lineedit->text().replace(':',"").stripWhiteSpace().isEmpty())
		return QVariant();
	return timeValue();

	//QDateTime - a hack needed because QVariant(QTime) has broken isNull()
//	return QVariant(QDateTime( m_cleared ? QDate() : QDate(0,1,2)/*nevermind*/, m_edit->time()));
}

bool KexiTimeTableEdit::valueIsValid()
{
	if (m_lineedit->text().replace(':',"").stripWhiteSpace().isEmpty()) //empty time is valid
		return true;
	return m_formatter.stringToTime( m_lineedit->text() ).isValid();
}

KEXI_CELLEDITOR_FACTORY_ITEM_IMPL(KexiTimeEditorFactoryItem, KexiTimeTableEdit)

#include "kexitimetableedit.moc"
