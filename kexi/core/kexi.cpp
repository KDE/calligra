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
#include "kexi_utils.h"

#include <qtimer.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qpixmapcache.h>

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
bool _tempShowForms = 0;
bool& Kexi::tempShowForms() { return _tempShowForms; }


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

QString Kexi::string2FileName(const QString &s)
{
	QString fn = s.simplifyWhiteSpace();
	fn.replace(' ',"_"); fn.replace('$',"_");
	fn.replace('\\',"-"); fn.replace('/',"-"); 
	fn.replace(':',"-"); fn.replace('*',"-"); 
	return fn;
}

QString Kexi::string2Identifier(const QString &s)
{
	QString r, id = s.simplifyWhiteSpace();
	r.reserve(id.length());
	if (id.isEmpty())
		return id;
//		return "_";
	id.replace(' ',"_"); 
	QChar c = id[0].lower();
	if (!(c>='a' && c<='z') && c!='_')
		r="_";
	else
		r+=id[0];
	if (c>='0' && c<='9')
		r+=c;
	for (uint i=1; i<id.length(); i++) {
		QChar c = id.at(i).lower();
		if (!(c>='a' && c<='z') && !(c>='0' && c<='9') && c!='_')
			r+='_';
		else
			r+=c;
	}
	return r;
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
	input = string2Identifier(input);
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
	message = i18n("Value of \"%1\" column must be an identifier.\n"
		"\"%2\" is not a valid identifier.").arg(valueName).arg(v.toString());
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
	_delayedCursorHandler.start();
}
void Kexi::removeWaitCursor() {
	_delayedCursorHandler.stop();
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

#define UPD_SINGLE_CACHE(size, state) \
	img = loader->loadIcon(origIconName, size, 0, state).convertToImage(); \
	img2 = loader->loadIcon("new_sign", size, 0, state).convertToImage(); \
	bitBlt( &img, 0, 0, &img2, 0, 0, img2.width(), img2.height(), 0); \
	pix.convertFromImage(img); \
	QPixmapCache::insert(key.arg(IconSize(size)).arg(ie->fingerprint( size, state )), pix)

#define UPD_CACHE(size) \
	UPD_SINGLE_CACHE(size, KIcon::DefaultState); \
	UPD_SINGLE_CACHE(size, KIcon::ActiveState); \
	UPD_SINGLE_CACHE(size, KIcon::DisabledState)

void Kexi::generateIconSetWithStar(const QString& origIconName, const QString& newIconName)
{
//eg: $kico_query_newobj_16_2:0.3:notrans:#ffffff_0
	KIconLoader *loader = kapp->iconLoader();
	KIconEffect *ie = loader->iconEffect();
	QString key = QString("$kico_") + newIconName + "_%1_%2_0";
	QPixmap pix;
	QImage img, img2;

	UPD_CACHE(KIcon::Small);
	UPD_CACHE(KIcon::Toolbar);
	UPD_CACHE(KIcon::Desktop);
}

//--------------------------------------------------------------------------------

#include "kexi_p.moc"

