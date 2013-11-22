/* This file is part of the KDE project
   Copyright (C) 2009, 2011, 2012 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "icalendarexport.h"

#include <kptmaindocument.h>
#include <kpttask.h>
#include <kptnode.h>
#include <kptresource.h>
#include <kptdocuments.h>
#include "kptdebug.h"

#include <kcalcore/attendee.h>
#include <kcalcore/attachment.h>
#include <kcalcore/icalformat.h>
#include <kcalcore/memorycalendar.h>

#include <QTextCodec>
#include <QByteArray>
#include <QString>
#include <QTextStream>
#include <QFile>

#include <kdebug.h>
#include <kpluginfactory.h>
#include <kurl.h>

#include <KoFilterChain.h>
#include <KoFilterManager.h>
#include <KoDocument.h>


using namespace KPlato;

K_PLUGIN_FACTORY(ICalendarExportFactory, registerPlugin<ICalendarExport>();)
K_EXPORT_PLUGIN(ICalendarExportFactory("calligrafilters"))

ICalendarExport::ICalendarExport(QObject* parent, const QVariantList &)
        : KoFilter(parent)
{
}

KoFilter::ConversionStatus ICalendarExport::convert(const QByteArray& from, const QByteArray& to)
{
    kDebug(planDbg()) << from << to;
    if ( ( from != "application/x-vnd.kde.plan" ) || ( to != "text/calendar" ) ) {
        return KoFilter::NotImplemented;
    }
    bool batch = false;
    if ( m_chain->manager() ) {
        batch = m_chain->manager()->getBatchMode();
    }
    if ( batch ) {
        //TODO
        kDebug(planDbg()) << "batch";
        return KoFilter::UsageError;
    }
    kDebug(planDbg())<<"online:"<<m_chain->inputDocument();
    MainDocument *doc = dynamic_cast<MainDocument*>( m_chain->inputDocument() );
    if (doc == 0) {
        kError() << "Cannot open Plan document";
        return KoFilter::InternalError;
    }
    if (m_chain->outputFile().isEmpty()) {
        kError() << "Output filename is empty";
        return KoFilter::InternalError;
    }
    QFile file(m_chain->outputFile());
    if (! file.open(QIODevice::WriteOnly)) {
        kError() << "Failed to open output file:" << file.fileName();
        return KoFilter::StorageCreationError;
    }

    KoFilter::ConversionStatus status = convert(doc->getProject(), file);
    file.close();
    //kDebug(planDbg()) << "Finished with status:"<<status;
    return status;
}

KoFilter::ConversionStatus ICalendarExport::convert(const Project &project, QFile &file)
{
    KCalCore::Calendar::Ptr cal(new KCalCore::MemoryCalendar("UTC"));

    //TODO: schedule selection dialog
    long id = ANYSCHEDULED;
    bool baselined = project.isBaselined(id);
    QList<ScheduleManager*> lst = project.allScheduleManagers();
    foreach(const ScheduleManager *m, lst) {
        if (! baselined) {
            id = lst.last()->scheduleId();
            //kDebug(planDbg())<<"last:"<<id;
            break;
        }
        if (m->isBaselined()) {
            id = m->scheduleId();
            //kDebug(planDbg())<<"baselined:"<<id;
            break;
        }
    }
    //kDebug(planDbg())<<id;
    createTodos(cal, &project, id);

    KCalCore::ICalFormat format;
    qint64 n = file.write(format.toString(cal).toUtf8());
    if (n < 0) {
        return KoFilter::InternalError;
    }
    return KoFilter::OK;
}

void ICalendarExport::createTodos(KCalCore::Calendar::Ptr cal, const Node *node, long id, KCalCore::Todo::Ptr parent)
{
    KCalCore::Todo::Ptr todo(new KCalCore::Todo());
    todo->setUid( node->id() );
    todo->setSummary(node->name());
    todo->setDescription(node->description());
    todo->setCategories(QLatin1String("Plan"));
    if (! node->projectNode()->leader().isEmpty()) {
        todo->setOrganizer(node->projectNode()->leader());
    }
    if ( node->type() != Node::Type_Project && ! node->leader().isEmpty()) {
        KCalCore::Person::Ptr p = KCalCore::Person::fromFullName(node->leader());
        KCalCore::Attendee::Ptr a(new KCalCore::Attendee(p->name(), p->email()));
        a->setRole(KCalCore::Attendee::NonParticipant);
        todo->addAttendee(a);
    }
    DateTime st = node->startTime(id);
    DateTime et = node->endTime(id);
    if (st.isValid()) {
#if PLAN_KDEPIMLIBS_HEXVERSION < KDE_MAKE_VERSION(4,11,0)
        todo->setHasStartDate(true);
#endif
        todo->setDtStart( KDateTime( st ) );
    }
    if (et.isValid()) {
#if PLAN_KDEPIMLIBS_HEXVERSION < KDE_MAKE_VERSION(4,11,0)
        todo->setHasDueDate(true);
#endif
        todo->setDtDue( KDateTime( et ) );
    }
    if (node->type() == Node::Type_Task) {
        const Task *task = qobject_cast<Task*>(const_cast<Node*>(node));
        Schedule *s = task->schedule(id);
        if (id < 0 || s == 0) {
            // Not scheduled, use requests
            const QList<Resource*> lst = task->requestedResources();
            foreach(const Resource *r, lst) {
                if (r->type() == Resource::Type_Work) {
                    todo->addAttendee(KCalCore::Attendee::Ptr(new KCalCore::Attendee(r->name(), r->email())));
                }
            }
        } else {
            foreach(const Resource *r, s->resources()) {
                if (r->type() == Resource::Type_Work) {
                    todo->addAttendee(KCalCore::Attendee::Ptr(new KCalCore::Attendee(r->name(), r->email())));
                }
            }

        }
    } else if (node->type() == Node::Type_Milestone) {
        const Task *task = qobject_cast<Task*>(const_cast<Node*>(node));
#if PLAN_KDEPIMLIBS_HEXVERSION < KDE_MAKE_VERSION(4,11,0)
        todo->setHasStartDate(false);
#else
        todo->setDtStart(KDateTime());
#endif
        todo->setPercentComplete(task->completion().percentFinished());
    }
    foreach(const Document *doc, node->documents().documents()) {
        todo->addAttachment(KCalCore::Attachment::Ptr(new KCalCore::Attachment(doc->url().url())));
    }
    if (! parent.isNull()) {
        todo->setRelatedTo(parent->uid(), KCalCore::Incidence::RelTypeParent);
    }
    cal->addTodo(todo);
    foreach(const Node *n, node->childNodeIterator()) {
        createTodos(cal, n, id, todo);
    }
}

#include "icalendarexport.moc"
