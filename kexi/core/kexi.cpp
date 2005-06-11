/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexi.h"
#include "kexi_p.h"
#include "utils/identifier.h"
#include "kexi_utils.h"

#include <qtimer.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qcolor.h>

#include <kdebug.h>
#include <klocale.h>
#include <kcursor.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kiconeffect.h>
#include <ksharedptr.h>

using namespace Kexi;

//used for speedup
class KexiInternal : public KShared
{
	public:
		KexiInternal() : KShared()
		{}
		~KexiInternal()
		{}
		KexiDBConnectionSet connset;
		KexiProjectSet recentProjects;
		KexiDB::DriverManager driverManager;
		KexiPart::Manager partManager;
};

KSharedPtr<KexiInternal> _int;

#define _INIT_SHARED if (!_int) _int = new KexiInternal()

KexiDBConnectionSet& Kexi::connset()
{
	_INIT_SHARED;
	return _int->connset;
}

KexiProjectSet& Kexi::recentProjects() { 
	_INIT_SHARED;
	return _int->recentProjects;
}

KexiDB::DriverManager& Kexi::driverManager()
{
	_INIT_SHARED;
	return _int->driverManager;
}

KexiPart::Manager& Kexi::partManager()
{
	_INIT_SHARED;
	return _int->partManager;
}

void Kexi::deleteGlobalObjects()
{
	delete _int;
}

//temp
bool _tempShowForms = true;
bool& Kexi::tempShowForms() { 
#ifndef KEXI_FORMS_SUPPORT
	_tempShowForms = false; 
#endif
	return _tempShowForms;
}

bool _tempShowReports = true;
bool& Kexi::tempShowReports() { 
#ifndef KEXI_REPORTS_SUPPORT
	_tempShowReports = false; 
#endif
	return _tempShowReports;
}

bool _tempShowScripts = true;
bool& Kexi::tempShowScripts() { 
#ifndef KEXI_SCRIPTS_SUPPORT
	_tempShowScripts = false; 
#endif
	return _tempShowScripts;
}

//--------------------------------------------------------------------------------
QString Kexi::nameForViewMode(int m)
{
	if (m==NoViewMode) return i18n("No View");
	else if (m==DataViewMode) return i18n("Data View");
	else if (m==DesignViewMode) return i18n("Design View");
	else if (m==TextViewMode) return i18n("Text View");

	return i18n("Unknown");
}

QString Kexi::identifierExpectedMessage(const QString &valueName, const QVariant& v)
{
	return "<p>"+i18n("Value of \"%1\" column must be an identifier.").arg(valueName)
		+"</p><p>"+i18n("\"%1\" is not a valid identifier.").arg(v.toString())+"</p>";
}

//--------------------------------------------------------------------------------

IdentifierValidator::IdentifierValidator(QObject * parent, const char * name)
: KexiValidator(parent,name)
{
}

IdentifierValidator::~IdentifierValidator()
{
}

QValidator::State IdentifierValidator::validate( QString& input, int& pos ) const
{
	uint i;
	for (i=0; i<input.length() && input.at(i)==' '; i++)
		;
	pos -= i; //i chars will be removed from beginning
	if (i<input.length() && input.at(i)>='0' && input.at(i)<='9')
		pos++; //_ will be added at the beginning
	bool addspace = (input.right(1)==" ");
	input = KexiUtils::string2Identifier(input);
	if (addspace)
		input += "_";
	if((uint)pos>input.length())
		pos=input.length();
	return input.isEmpty() ? Valid : Acceptable;
}

KexiValidator::Result IdentifierValidator::internalCheck(
	const QString &valueName, const QVariant& v, 
	QString &message, QString & /*details*/)
{
	if (Kexi::isIdentifier(v.toString()))
		return KexiValidator::Ok;
	message = Kexi::identifierExpectedMessage(valueName, v);
	return KexiValidator::Error;
}

//--------------------------------------------------------------------------------

KexiDBObjectNameValidator::KexiDBObjectNameValidator(
	KexiDB::Driver *drv, QObject * parent, const char * name)
: KexiValidator(parent,name)
{
	m_drv = drv;
}

KexiDBObjectNameValidator::~KexiDBObjectNameValidator()
{
}

KexiValidator::Result KexiDBObjectNameValidator::internalCheck(
	const QString & /*valueName*/, const QVariant& v, 
	QString &message, QString &details)
{

	if (m_drv.isNull() ? !KexiDB::Driver::isKexiDBSystemObjectName(v.toString())
		 : !m_drv->isSystemObjectName(v.toString()))
		return KexiValidator::Ok;
	message = i18n("You cannot use name \"%1\" for your object.\n"
		"It is reserved for internal Kexi objects. Please choose another name.")
		.arg(v.toString());
	details = i18n("Names of internal Kexi objects are starting with \"kexi__\".");
	return KexiValidator::Error;
}

//--------------------------------------------------------------------------------

/*! @internal */
DelayedCursorHandler::DelayedCursorHandler() {
	connect(&timer, SIGNAL(timeout()), this, SLOT(show()));
}
void DelayedCursorHandler::start() {
	timer.start(1000, true);
}
void DelayedCursorHandler::stop() {
	timer.stop();
	QApplication::restoreOverrideCursor();
}
void DelayedCursorHandler::show() {
	QApplication::setOverrideCursor( KCursor::waitCursor() );
}

DelayedCursorHandler _delayedCursorHandler;

void Kexi::setWaitCursor() {
	if (kapp->guiEnabled())
		_delayedCursorHandler.start();
}
void Kexi::removeWaitCursor() {
	if (kapp->guiEnabled())
		_delayedCursorHandler.stop();
}

WaitCursor::WaitCursor()
{
	setWaitCursor();
}

WaitCursor::~WaitCursor()
{
	removeWaitCursor();
}

//--------------------------------------------------------------------------------

ObjectStatus::ObjectStatus()
{
}

ObjectStatus::ObjectStatus(const QString& message, const QString& description)
{
	setStatus(message, description);
}

ObjectStatus::ObjectStatus(KexiDB::Object* dbObject, const QString& message, const QString& description)
{
	setStatus(dbObject, message, description);
}

const ObjectStatus& ObjectStatus::status() const
{
	return *this;
}

bool ObjectStatus::error() const
{
	return !message.isEmpty() || !message.isEmpty();
}

void ObjectStatus::setStatus(const QString& message, const QString& description)
{
	this->dbObj=0;
	this->message=message;
	this->description=description;
}

void ObjectStatus::setStatus(KexiDB::Object* dbObject, const QString& message, const QString& description)
{
	this->dbObj=dbObject;
	this->message=message;
	this->description=description;
}

void ObjectStatus::clearStatus()
{
	message=QString::null;
	description=QString::null;
}

QString ObjectStatus::singleStatusString() const { 
	if (message.isEmpty() || description.isEmpty())
		return message;
	return message + " " + description;
}

void ObjectStatus::append( const ObjectStatus& otherStatus ) {
	if (message.isEmpty()) {
		message = otherStatus.message;
		description = otherStatus.description;
		return;
	}
	QString s = otherStatus.singleStatusString();
	if (s.isEmpty())
		return;
	if (description.isEmpty()) {
		description = s;
		return;
	}
	description = description + " " + s;
}

//--------------------------------------------------------------------------------

QString fileDialogFilterString(const KMimeType::Ptr& mime, bool kdeFormat)
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

QString fileDialogFilterString(const QString& mimeString, bool kdeFormat)
{
	KMimeType::Ptr ptr = KMimeType::mimeType(mimeString);
	return fileDialogFilterString( ptr, kdeFormat );
}

QString fileDialogFilterStrings(const QStringList& mimeStrings, bool kdeFormat)
{
	QString ret;
	foreach( QStringList::ConstIterator, it, mimeStrings)
		ret += fileDialogFilterString(*it, kdeFormat);
	return ret;
}

QColor blendColors(const QColor& c1, const QColor& c2, int factor1, int factor2)
{
	return QColor(
		int( (c1.red()*factor1+c2.red()*factor2)/(factor1+factor2) ),
		int( (c1.green()*factor1+c2.green()*factor2)/(factor1+factor2) ),
		int( (c1.blue()*factor1+c2.blue()*factor2)/(factor1+factor2) ) );
}

#include "kexi_p.moc"

