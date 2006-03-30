/* This file is part of the KDE project
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "utils.h"
#include "utils_p.h"

#include <qregexp.h>
//Added by qt3to4:
#include <Q3CString>

#include <kdebug.h>
#include <kcursor.h>
#include <kapplication.h>

using namespace KexiUtils;

DelayedCursorHandler::DelayedCursorHandler() {
	connect(&timer, SIGNAL(timeout()), this, SLOT(show()));
}
void DelayedCursorHandler::start(bool noDelay) {
	timer.start(noDelay ? 0 : 1000, true);
}
void DelayedCursorHandler::stop() {
	timer.stop();
	QApplication::restoreOverrideCursor();
}
void DelayedCursorHandler::show() {
	QApplication::setOverrideCursor( KCursor::waitCursor() );
}

DelayedCursorHandler _delayedCursorHandler;

void KexiUtils::setWaitCursor(bool noDelay) {
	if (kapp->guiEnabled())
		_delayedCursorHandler.start(noDelay);
}
void KexiUtils::removeWaitCursor() {
	if (kapp->guiEnabled())
		_delayedCursorHandler.stop();
}

WaitCursor::WaitCursor(bool noDelay)
{
	setWaitCursor(noDelay);
}

WaitCursor::~WaitCursor()
{
	removeWaitCursor();
}

//--------------------------------------------------------------------------------

QString KexiUtils::fileDialogFilterString(const KMimeType::Ptr& mime, bool kdeFormat)
{
	if (mime==0)
		return QString::null;

	QString str;
	if (kdeFormat) {
		if (mime->patterns().isEmpty())
			str = "*";
		else
			str = mime->patterns().join(" ");
		str += "|";
	}
	str += mime->comment();
	if (!mime->patterns().isEmpty() || !kdeFormat) {
		str += " (";
		if (mime->patterns().isEmpty())
			str += "*";
		else
			str += mime->patterns().join("; ");
		str += ")";
	}
	if (kdeFormat)
		str += "\n";
	else
		str += ";;";
	return str;
}

QString KexiUtils::fileDialogFilterString(const QString& mimeString, bool kdeFormat)
{
	KMimeType::Ptr ptr = KMimeType::mimeType(mimeString);
	return fileDialogFilterString( ptr, kdeFormat );
}

QString KexiUtils::fileDialogFilterStrings(const QStringList& mimeStrings, bool kdeFormat)
{
	QString ret;
	QStringList::ConstIterator endIt = mimeStrings.constEnd();
	for(QStringList::ConstIterator it = mimeStrings.constBegin(); it != endIt; ++it)
		ret += fileDialogFilterString(*it, kdeFormat);
	return ret;
}

QColor KexiUtils::blendedColors(const QColor& c1, const QColor& c2, int factor1, int factor2)
{
	return QColor(
		int( (c1.red()*factor1+c2.red()*factor2)/(factor1+factor2) ),
		int( (c1.green()*factor1+c2.green()*factor2)/(factor1+factor2) ),
		int( (c1.blue()*factor1+c2.blue()*factor2)/(factor1+factor2) ) );
}

QColor KexiUtils::contrastColor(const QColor& c)
{
	int g = qGray( c.rgb() );
	if (g>110)
		return c.dark(200);
	else if (g>80)
		return c.light(150);
	else if (g>20)
		return c.light(300);
	return Qt::gray;
}

QColor KexiUtils::bleachedColor(const QColor& c, int factor)
{
	int h, s, v;
	c.getHsv( &h, &s, &v );
	QColor c2;
	if (factor < 100)
		factor = 100;
	if (s>=250 && v>=250) //for colors like cyan or red, make the result more white
		s = qMax(0, s - factor - 50);
	else if (s<=5 && s<=5)
		v += factor-50;
	c2.setHsv(h, s, qMin(255,v + factor-100));
	return c2;
}

void KexiUtils::serializeMap(const QMap<QString,QString>& map, QByteArray& array)
{
	QDataStream ds( &array,QIODevice::WriteOnly);
	ds.setVersion(QDataStream::Qt_3_1);
	ds << map;
}

void KexiUtils::serializeMap(const QMap<QString,QString>& map, QString& string)
{
	QByteArray array;
	QDataStream ds( &array,QIODevice::WriteOnly);
	ds.setVersion(QDataStream::Qt_3_1);
	ds << map;
	kDebug() << array[3] << " " << array[4] << " " << array[5] << endl;
	const uint size = array.size();
	string = QString::null;
	string.reserve(size);
	for (uint i=0; i<size; i++) {
		string[i]=QChar(ushort(array[i]+1));
	}
}

QMap<QString,QString> KexiUtils::deserializeMap(const QByteArray& array)
{
	QMap<QString,QString> map;
	QDataStream ds( &array,QIODevice::ReadOnly);
	ds.setVersion(QDataStream::Qt_3_1);
	ds >> map;
	return map;
}

QMap<QString,QString> KexiUtils::deserializeMap(const QString& string)
{
	const uint size = string.length();
	Q3CString cstr(string.latin1());
	QByteArray array( size );
	for (uint i=0; i<size; i++) {
		array[i] = char(string[i].unicode()-1);
	}
	QMap<QString,QString> map;
	QDataStream ds( &array,QIODevice::ReadOnly);
	ds.setVersion(QDataStream::Qt_3_1);
	ds >> map;
	return map;
}

QString KexiUtils::stringToFileName(const QString& string)
{
	QString _string(string);
	_string.replace(QRegExp("[\\\\/:\\*?\"<>|]"), " ");
	return _string.simplified();
}

void KexiUtils::simpleCrypt(QString& string)
{
	for (uint i=0; i<string.length(); i++)
		string[i] = QChar( string[i].unicode() + 47 + i );
}

void KexiUtils::simpleDecrypt(QString& string)
{
	for (uint i=0; i<string.length(); i++)
		string[i] = QChar( string[i].unicode() - 47 - i );
}

#include "utils_p.moc"
