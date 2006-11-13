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

#include "kexi.h"
#include "kexiaboutdata.h"
#include "kexicmdlineargs.h"
#include <config-kexi.h>
#include <kexiutils/identifier.h>
#include <kexidb/msghandler.h>

#include <qtimer.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qcolor.h>
#include <qfileinfo.h>

#include <kdebug.h>
#include <kcursor.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kiconeffect.h>
#include <ksharedptr.h>
#include <kmimetype.h>
#include <kstaticdeleter.h>
#include <kglobalsettings.h>

using namespace Kexi;

//! used for speedup
//! @internal
class KexiInternal : public KShared
{
	public:
		KexiInternal() : KShared()
		 , connset(0)
		 , smallFont(0)
		{
		}
		~KexiInternal()
		{
			delete connset;
			delete smallFont;
		}
		KexiDBConnectionSet* connset;
		KexiProjectSet recentProjects;
		KexiDBConnectionSet recentConnections;
		KexiDB::DriverManager driverManager;
		KexiPart::Manager partManager;
		QFont *smallFont;
};

static KStaticDeleter<KexiInternal> Kexi_intDeleter;
KexiInternal* _int = 0;

#define _INIT_SHARED { if (!_int) Kexi_intDeleter.setObject( _int, new KexiInternal() ); }

KexiDBConnectionSet& Kexi::connset()
{
	_INIT_SHARED;
	//delayed
	if (!_int->connset) {
		//load stored set data, OK?
		_int->connset = new KexiDBConnectionSet();
		_int->connset->load();
	}
	return *_int->connset;
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

bool _tempShowMacros = true;
bool& Kexi::tempShowMacros() {
#ifndef KEXI_MACROS_SUPPORT
	_tempShowMacros = false; 
#endif
	return _tempShowMacros;
}

bool _tempShowScripts = true;
bool& Kexi::tempShowScripts() { 
#ifndef KEXI_SCRIPTS_SUPPORT
	_tempShowScripts = false; 
#endif
	return _tempShowScripts;
}

//--------------------------------------------------------------------------------

QFont Kexi::smallFont(QWidget *init)
{
	_INIT_SHARED;
	if (!_int->smallFont) {
		_int->smallFont = new QFont( init->font() );
		const int wdth = KGlobalSettings::desktopGeometry(init).width();
		int size = 10 + qMax(0, wdth - 1100) / 100;
		size = qMin( init->fontInfo().pixelSize(), size );
		_int->smallFont->setPixelSize( size );
	}
	return *_int->smallFont;
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

//--------------------------------------------------------------------------------

QString Kexi::msgYouCanImproveData() {
	return i18n("You can correct data in this row or use \"Cancel row changes\" function.");
}

//--------------------------------------------------------------------------------

ObjectStatus::ObjectStatus()
: msgHandler(0)
{
}

ObjectStatus::ObjectStatus(const QString& message, const QString& description)
: msgHandler(0)
{
	setStatus(message, description);
}

ObjectStatus::ObjectStatus(KexiDB::Object* dbObject, const QString& message, const QString& description)
: msgHandler(0)
{
	setStatus(dbObject, message, description);
}

ObjectStatus::~ObjectStatus()
{
	delete msgHandler;
}

const ObjectStatus& ObjectStatus::status() const
{
	return *this;
}

bool ObjectStatus::error() const
{
	return !message.isEmpty() 
		|| (dynamic_cast<KexiDB::Object*>((QObject*)dbObj) && dynamic_cast<KexiDB::Object*>((QObject*)dbObj)->error());
}

void ObjectStatus::setStatus(const QString& message, const QString& description)
{
	this->dbObj=0;
	this->message=message;
	this->description=description;
}

void ObjectStatus::setStatus(KexiDB::Object* dbObject, const QString& message, const QString& description)
{
	if (dynamic_cast<QObject*>(dbObject)) {
		dbObj = dynamic_cast<QObject*>(dbObject);
	}
	this->message=message;
	this->description=description;
}

void ObjectStatus::setStatus(KexiDB::ResultInfo* result, const QString& message, const QString& description)
{
	if (result) {
		if (message.isEmpty())
			this->message = result->msg;
		else
			this->message = message + " " + result->msg;

		if (description.isEmpty())
			this->description = result->desc;
		else
			this->description = description + " " + result->desc;
	}
	else
		clearStatus();
}

void ObjectStatus::setStatus(KexiDB::Object* dbObject, KexiDB::ResultInfo* result, 
	const QString& message, const QString& description)
{
	if (!dbObject)
		setStatus(result, message, description);
	else if (!result)
		setStatus(dbObject, message, description);
	else {
		setStatus(dbObject, message, description);
		setStatus(result, this->message, this->description);
	}
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
	const QString s( otherStatus.singleStatusString() );
	if (s.isEmpty())
		return;
	if (description.isEmpty()) {
		description = s;
		return;
	}
	description = description + " " + s;
}

//! @internal
class ObjectStatusMessageHandler : public KexiDB::MessageHandler
{
	public:
		ObjectStatusMessageHandler(ObjectStatus *status) 
			: KexiDB::MessageHandler()
			, m_status(status)
		{
		}
		virtual ~ObjectStatusMessageHandler()
		{
		}

		virtual void showErrorMessage(const QString &title, 
			const QString &details = QString::null)
		{
			m_status->setStatus(title, details);
		}
		
		virtual void showErrorMessage(KexiDB::Object *obj, const QString& msg = QString::null)
		{
			m_status->setStatus(obj, msg);
		}

		ObjectStatus *m_status;
};

ObjectStatus::operator KexiDB::MessageHandler*()
{
	if (!msgHandler)
		msgHandler = new ObjectStatusMessageHandler(this);
	return msgHandler;
}

void Kexi::initCmdLineArgs(int argc, char *argv[], KAboutData* aboutData)
{
	KAboutData *about = aboutData;
	if (!about)
		about = Kexi::createAboutData();
#ifdef CUSTOM_VERSION
# include "../custom_startup.h"
#endif
	KCmdLineArgs::init( argc, argv, about );
	KCmdLineArgs::addCmdLineOptions( options );
}

void KEXI_UNFINISHED(const QString& feature_name, const QString& extra_text) 
{
	QString msg;
	if (feature_name.isEmpty())
		msg = i18n("This function is not available for version %1 of %2 application.")
			.arg(KEXI_VERSION_STRING)
			.arg(KEXI_APP_NAME); 
	else {
		QString feature_name_(feature_name);
		msg = i18n("\"%1\" function is not available for version %2 of %3 application.")
			.arg(feature_name_.replace("&",""))
			.arg(KEXI_VERSION_STRING)
			.arg(KEXI_APP_NAME);
	}

	QString extra_text_(extra_text);
	if (!extra_text_.isEmpty())
		extra_text_.prepend("\n");

	KMessageBox::sorry(0, msg + extra_text_);
}
