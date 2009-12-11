/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#include <kptpart.h>
#include <kpttask.h>
#include <kptnode.h>
#include <kptresource.h>
#include <kptdocuments.h>

#include <kcal/attendee.h>
#include <kcal/attachment.h>
#include <kcal/todo.h>
#include <kcal/icalformat.h>

#include <QTextCodec>
#include <QByteArray>
#include <QString>
#include <QTextStream>
#include <QFile>

#include <kdebug.h>
#include <kgenericfactory.h>
#include <kurl.h>

#include <KoFilterChain.h>
#include <KoFilterManager.h>
#include <KoDocument.h>

using namespace KPlato;

typedef KGenericFactory<ICalendarExport> ICalendarExportFactory;
K_EXPORT_COMPONENT_FACTORY(libicalendarexport, ICalendarExportFactory("kofficefilters"))

ICalendarExport::ICalendarExport(QObject* parent, const QStringList &)
        : KoFilter(parent)
{
}

KoFilter::ConversionStatus ICalendarExport::convert(const QByteArray& from, const QByteArray& to)
{
    kDebug() << from << to;
    if ((from != "application/x-vnd.kde.kplato") || (to != "text/calendar")) {
        return KoFilter::NotImplemented;
    }
    const Part *part = 0;
    bool batch = false;
    if (m_chain->manager()) {
        batch = m_chain->manager()->getBatchMode();
    }
    if (batch) {
        //TODO
        kDebug() << "batch";
    } else {
        //kDebug()<<"online";
        part = qobject_cast<Part*>(m_chain->inputDocument());
    }
    if (part == 0) {
        kError() << "Cannot open KPlato document";
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

    KoFilter::ConversionStatus status = convert(part->getProject(), file);
    file.close();
    //kDebug() << "Finished with status:"<<status;
    return status;
}

KoFilter::ConversionStatus ICalendarExport::convert(const Project &project, QFile &file)
{
    KCal::CalendarLocal cal("UTC");

    //TODO: schedule selection dialog
    long id = ANYSCHEDULED;
    bool baselined = project.isBaselined(id);
    QList<ScheduleManager*> lst = project.allScheduleManagers();
    foreach(const ScheduleManager *m, lst) {
        if (! baselined) {
            id = lst.last()->id();
            //kDebug()<<"last:"<<id;
            break;
        }
        if (m->isBaselined()) {
            id = m->id();
            //kDebug()<<"baselined:"<<id;
            break;
        }
    }
    //kDebug()<<id;
    createTodos(cal, &project, id);

    KCal::ICalFormat format;
    qint64 n = file.write(format.toString(&cal).toUtf8());
    if (n < 0) {
        return KoFilter::InternalError;
    }
    return KoFilter::OK;
}

void ICalendarExport::createTodos(KCal::CalendarLocal &cal, const Node *node, long id, KCal::Todo *parent)
{
    KCal::Todo *todo = new KCal::Todo();
    todo->setSummary(node->name());
    todo->setDescription(node->description());
    todo->setCategories("KPlato");
    if (! node->projectNode()->leader().isEmpty()) {
        todo->setOrganizer(node->projectNode()->leader());
    }
    if (! node->leader().isEmpty()) {
        KCal::Person p = KCal::Person::fromFullName(node->leader());
        KCal::Attendee *a = new KCal::Attendee(p.name(), p.email());
        a->setRole(KCal::Attendee::NonParticipant);
        todo->addAttendee(a);
    }
    if (node->type() == Node::Type_Project || node->type() == Node::Type_Summarytask) {
        todo->setHasStartDate(true);
        todo->setDtStart(node->startTime(id));
        todo->setHasDueDate(true);
        todo->setDtDue(node->endTime(id));
    } else if (node->type() == Node::Type_Task) {
        const Task *task = qobject_cast<Task*>(const_cast<Node*>(node));
        todo->setPercentComplete(task->completion().percentFinished());
        todo->setHasStartDate(id >= 0);
        todo->setDtStart(node->startTime(id));
        todo->setHasDueDate(id >= 0);
        todo->setDtDue(node->endTime(id));

        Schedule *s = task->schedule(id);
        if (id < 0 || s == 0) {
            // Not scheduled, use requests
            const QList<Resource*> lst = task->requestedResources();
            foreach(const Resource *r, lst) {
                if (r->type() == Resource::Type_Work) {
                    todo->addAttendee(new KCal::Attendee(r->name(), r->email()));
                }
            }
        } else {
            foreach(const Resource *r, s->resources()) {
                if (r->type() == Resource::Type_Work) {
                    todo->addAttendee(new KCal::Attendee(r->name(), r->email()));
                }
            }

        }
    } else if (node->type() == Node::Type_Milestone) {
        const Task *task = qobject_cast<Task*>(const_cast<Node*>(node));
        todo->setPercentComplete(task->completion().percentFinished());
        todo->setDtStart(node->startTime(id));     //NOTE Needed to get time included in due
        todo->setHasDueDate(id >= 0);
        todo->setDtDue(node->endTime(id));
    }
    foreach(const Document *doc, node->documents().documents()) {
        todo->addAttachment(new KCal::Attachment(doc->url().url()));
    }
    if (parent) {
        todo->setRelatedTo(parent);
    }
    cal.addTodo(todo);
    foreach(const Node *n, node->childNodeIterator()) {
        createTodos(cal, n, id, todo);
    }
}

#include "icalendarexport.moc"
