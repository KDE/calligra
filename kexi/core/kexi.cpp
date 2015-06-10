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
#include "KexiRecentProjects.h"
#include "KexiMainWindowIface.h"
#include "kexipartmanager.h"
#include <KexiIcon.h>

#include <KDb>
#include <KDbMessageHandler>
#include <KDbDriverManager>
#include <KDbUtils>

#include <KMessageBox>
#include <KIconTheme>
#include <KAboutData>

#include <QPixmap>
#include <QPixmapCache>
#include <QLabel>
#include <QMimeDatabase>
#include <QMimeType>

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
    KDbDriverManager driverManager;
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

KDbDriverManager& Kexi::driverManager()
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
        return Kexi::nameForViewMode(mode, true).remove('&');

    if (mode == NoViewMode)
        return xi18n("&No View");
    else if (mode == DataViewMode)
        return xi18n("&Data View");
    else if (mode == DesignViewMode)
        return xi18n("D&esign View");
    else if (mode == TextViewMode)
        return xi18n("&Text View");

    return xi18n("&Unknown");
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

ObjectStatus::ObjectStatus()
        : m_resultable(0), m_msgHandler(0)
{
}

ObjectStatus::ObjectStatus(const QString& message, const QString& description)
        : m_resultable(0), m_msgHandler(0)
{
    setStatus(message, description);
}

ObjectStatus::ObjectStatus(KDbResultable* resultable, const QString& message, const QString& description)
        : m_resultable(0), m_msgHandler(0)
{
    setStatus(resultable, message, description);
}

ObjectStatus::~ObjectStatus()
{
    delete m_msgHandler;
}

const ObjectStatus& ObjectStatus::status() const
{
    return *this;
}

bool ObjectStatus::error() const
{
    return !message.isEmpty()
           || (m_resultable && m_resultable->result().isError());
}

void ObjectStatus::setStatus(const QString& message, const QString& description)
{
    m_resultable = 0;
    this->message = message;
    this->description = description;
}

void ObjectStatus::setStatus(KDbResultable* resultable, const QString& message, const QString& description)
{
    m_resultable = resultable;
    this->message = message;
    this->description = description;
}

void ObjectStatus::setStatus(KDbResultInfo* resultInfo, const QString& message, const QString& description)
{
    if (resultInfo) {
        if (message.isEmpty())
            this->message = resultInfo->msg;
        else
            this->message = message + " " + resultInfo->msg;

        if (description.isEmpty())
            this->description = resultInfo->desc;
        else
            this->description = description + " " + resultInfo->desc;
    } else
        clearStatus();
}

void ObjectStatus::setStatus(KDbResultable* resultable, KDbResultInfo* resultInfo,
                             const QString& message, const QString& description)
{
    if (!resultable)
        setStatus(resultInfo, message, description);
    else if (!resultInfo)
        setStatus(resultable, message, description);
    else {
        setStatus(resultable, message, description);
        setStatus(resultInfo, this->message, this->description);
    }
}

void ObjectStatus::setStatus(const KDbResult &result, KDbResultInfo* resultInfo,
                             const QString& message, const QString& description)
{
    //! @todo KEXI3 test this
    if (!result.isError()) {
        setStatus(resultInfo, message, description);
    }
    else {
        KDbResult r = result;
        r.prependMessage(message);
        r.prependMessage(description);
        setStatus(resultInfo, result.messageTitle(), result.message());
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
class ObjectStatusMessageHandler : public KDbMessageHandler
{
public:
    explicit ObjectStatusMessageHandler(ObjectStatus *status)
            : KDbMessageHandler()
            , m_status(status) {
    }
    virtual ~ObjectStatusMessageHandler() {
    }

    virtual void showErrorMessage(KDbMessageHandler::MessageType messageType,
                                  const QString &msg,
                                  const QString &details = QString(),
                                  const QString &caption = QString())
    {
        Q_UNUSED(messageType);
        Q_UNUSED(caption);
        m_status->setStatus(msg, details);
    }

    virtual void showErrorMessage(const KDbResult& result,
                                  KDbMessageHandler::MessageType messageType = KDbMessageHandler::Error,
                                  const QString& msg = QString(),
                                  const QString& caption = QString())
    {
        Q_UNUSED(messageType);
        m_status->setStatus(result, 0, caption, msg);
    }

    ObjectStatus *m_status;
};

ObjectStatus::operator KDbMessageHandler*()
{
    if (!m_msgHandler)
        m_msgHandler = new ObjectStatusMessageHandler(this);
    return m_msgHandler;
}

void Kexi::initCmdLineArgs(int argc, char *argv[], const KAboutData& aboutData)
{
    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineArgs::addCmdLineOptions(kexi_options());
}

void KEXI_UNFINISHED_INTERNAL(const QString& feature_name, const QString& extra_text,
                              QString* line1, QString* line2)
{
    if (feature_name.isEmpty())
        *line1 = xi18n("This function is not available for version %1 of %2 application.",
                   QString(KEXI_VERSION_STRING), QString(KEXI_APP_NAME));
    else {
        QString feature_name_(feature_name);
        *line1 = xi18n(
                  "\"%1\" function is not available for version %2 of %3 application.",
                  feature_name_.remove('&'), QString(KEXI_VERSION_STRING), QString(KEXI_APP_NAME));
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

//--------------------------------------------------------------------------------

static bool isSpecialIconTheme()
{
    const QString theme(KIconTheme::current().toLower());
    return theme.contains(QLatin1String("breeze"))
           || theme.contains(QLatin1String("highcontrast"));
}

QString KexiIconName(const QString &baseName)
{
    if (isSpecialIconTheme()) {
        //! @todo use prefix based on KIconTheme::current()?
        return QLatin1String("breeze-") + baseName;
    }
    return baseName;
}

QIcon KexiIcon(const QString &baseName)
{
    return QIcon::fromTheme(KexiIconName(baseName));
}

QString Kexi::defaultFileBasedDriverIconName()
{
    if (!isSpecialIconTheme()) {
        QMimeDatabase db;
        QMimeType mimeType(db.mimeTypeForName(KDb::defaultFileBasedDriverMimeType()));
        if (mimeType.isValid()) {
            return mimeType.iconName();
        }
        qWarning() << KDb::defaultFileBasedDriverMimeType() << "mimetype not installed!";
    }
    return koIconName("breeze-kexi-file-database");
}

QIcon Kexi::defaultFileBasedDriverIcon()
{
    return QIcon::fromTheme(defaultFileBasedDriverIconName());
}

QString Kexi::serverIconName()
{
    return KexiIconName("network-server-database");
}

QIcon Kexi::serverIcon()
{
    return QIcon::fromTheme(serverIconName());
}

