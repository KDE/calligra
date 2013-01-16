/* This file is part of the KDE project
   Copyright (C) 2003-2012 Jarosław Staniek <staniek@kde.org>

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
#include "KexiRecentProjects.h"
#include "KexiMainWindowIface.h"
#include <kexiutils/identifier.h>
#include <db/msghandler.h>
#include <KoIcon.h>

#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QPixmapCache>
#include <QColor>
#include <QFileInfo>
#include <QLabel>

#include <kdebug.h>
#include <kcursor.h>
#include <kapplication.h>
#include <ksharedptr.h>
#include <kglobalsettings.h>

using namespace Kexi;

//! used for speedup
//! @internal
class KexiInternal
{
public:
    static KexiInternal *_int;

    KexiInternal()
            : connset(0)
    {
    }
    ~KexiInternal() {
        delete connset;
    }

    static KexiInternal* self() {
        static bool created = false;
        if (!created) {
            _int = new KexiInternal;
            created = true;
        }
        return _int;
    }

    static void destroy() {
        delete _int;
        _int = 0;
    }

    KexiDBConnectionSet* connset;
    KexiRecentProjects recentProjects;
    KexiDBConnectionSet recentConnections;
    KexiDB::DriverManager driverManager;
    KexiPart::Manager partManager;
};

KexiInternal *KexiInternal::_int = 0;

KexiDBConnectionSet& Kexi::connset()
{
    //delayed
    if (!KexiInternal::self()->connset) {
        //load stored set data, OK?
        KexiInternal::self()->connset = new KexiDBConnectionSet();
        KexiInternal::self()->connset->load();
    }
    return *KexiInternal::self()->connset;
}

KexiRecentProjects* Kexi::recentProjects()
{
    return &KexiInternal::self()->recentProjects;
}

KexiDB::DriverManager& Kexi::driverManager()
{
    return KexiInternal::self()->driverManager;
}

KexiPart::Manager& Kexi::partManager()
{
    return KexiInternal::self()->partManager;
}

void Kexi::deleteGlobalObjects()
{
    KexiInternal::self()->destroy();
}

//temp

bool _tempShowMacros = true;
bool& Kexi::tempShowMacros()
{
#ifndef KEXI_MACROS_SUPPORT
    _tempShowMacros = false;
#endif
    return _tempShowMacros;
}

bool _tempShowScripts = true;
bool& Kexi::tempShowScripts()
{
#ifndef KEXI_SCRIPTS_SUPPORT
    _tempShowScripts = false;
#endif
    return _tempShowScripts;
}

//--------------------------------------------------------------------------------
QString Kexi::nameForViewMode(ViewMode mode, bool withAmpersand)
{
    if (!withAmpersand)
        return Kexi::nameForViewMode(mode, true).replace("&", "");

    if (mode == NoViewMode)
        return i18n("&No View");
    else if (mode == DataViewMode)
        return i18n("&Data View");
    else if (mode == DesignViewMode)
        return i18n("D&esign View");
    else if (mode == TextViewMode)
        return i18n("&Text View");

    return i18n("&Unknown");
}

//--------------------------------------------------------------------------------
QString Kexi::iconNameForViewMode(ViewMode mode)
{
    const char *const id =
        (mode == DataViewMode) ? koIconNameCStr("state_data") :
        (mode == DesignViewMode) ? koIconNameCStr("state_edit") :
        (mode == TextViewMode) ? koIconNameCStr("state_sql"): 
        0;

    return QLatin1String(id);
}

//--------------------------------------------------------------------------------

QString Kexi::msgYouCanImproveData()
{
    return i18n("You can correct data in this record or use \"Cancel record changes\" function.");
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
    this->dbObj = 0;
    this->message = message;
    this->description = description;
}

void ObjectStatus::setStatus(KexiDB::Object* dbObject, const QString& message, const QString& description)
{
    if (dynamic_cast<QObject*>(dbObject)) {
        dbObj = dynamic_cast<QObject*>(dbObject);
    }
    this->message = message;
    this->description = description;
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
    } else
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
    message.clear();
    description.clear();
}

QString ObjectStatus::singleStatusString() const
{
    if (message.isEmpty() || description.isEmpty())
        return message;
    return message + " " + description;
}

void ObjectStatus::append(const ObjectStatus& otherStatus)
{
    if (message.isEmpty()) {
        message = otherStatus.message;
        description = otherStatus.description;
        return;
    }
    const QString s(otherStatus.singleStatusString());
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
            , m_status(status) {
    }
    virtual ~ObjectStatusMessageHandler() {
    }

    virtual void showErrorMessage(const QString &title,
                                  const QString &details = QString()) {
        m_status->setStatus(title, details);
    }

    virtual void showErrorMessage(KexiDB::Object *obj, const QString& msg = QString()) {
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

void Kexi::initCmdLineArgs(int argc, char *argv[], const KexiAboutData& aboutData)
{
    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineArgs::addCmdLineOptions(kexi_options());
}

void KEXI_UNFINISHED_INTERNAL(const QString& feature_name, const QString& extra_text,
                              QString* line1, QString* line2)
{
    if (feature_name.isEmpty())
        *line1 = i18n("This function is not available for version %1 of %2 application.",
                   QString(KEXI_VERSION_STRING), QString(KEXI_APP_NAME));
    else {
        QString feature_name_(feature_name);
        *line1 = i18n(
                  "\"%1\" function is not available for version %2 of %3 application.",
                  feature_name_.replace("&", ""), QString(KEXI_VERSION_STRING), QString(KEXI_APP_NAME));
    }

    *line2 = extra_text;
}

void KEXI_UNFINISHED(const QString& feature_name, const QString& extra_text)
{
    QString line1, line2;
    KEXI_UNFINISHED_INTERNAL(feature_name, extra_text, &line1, &line2);
    if (!line2.isEmpty())
        line2.prepend("\n");
    KMessageBox::sorry(0, line1 + line2);
}

QLabel *KEXI_UNFINISHED_LABEL(const QString& feature_name, const QString& extra_text)
{
    QString line1, line2;
    KEXI_UNFINISHED_INTERNAL(feature_name, extra_text, &line1, &line2);
    QLabel *label = new QLabel(QLatin1String("<b>") + line1 + QLatin1String("</b><br>")
        + line2);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    label->setAutoFillBackground(true);
    label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    return label;
}
