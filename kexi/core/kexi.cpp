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
#include <kexiutils/identifier.h>

#include <qtimer.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qcolor.h>

#include <kdebug.h>
#include <kcursor.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kiconeffect.h>
#include <ksharedptr.h>
#include <kmimetype.h>

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

