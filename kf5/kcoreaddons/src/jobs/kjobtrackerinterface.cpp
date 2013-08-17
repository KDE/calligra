/*  This file is part of the KDE project
    Copyright (C) 2007 Kevin Ottens <ervin@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "kjobtrackerinterface.h"

#include "kjob.h"

class KJobTrackerInterface::Private
{
public:
    Private(KJobTrackerInterface *interface) : q(interface)
    {

    }

    KJobTrackerInterface *const q;
};

KJobTrackerInterface::KJobTrackerInterface(QObject *parent)
    : QObject(parent), d(new Private(this))
{

}

KJobTrackerInterface::~KJobTrackerInterface()
{
    delete d;
}

void KJobTrackerInterface::registerJob(KJob *job)
{
    QObject::connect(job, SIGNAL(finished(KJob*)),
                     this, SLOT(unregisterJob(KJob*)));
    QObject::connect(job, SIGNAL(finished(KJob*)),
                     this, SLOT(finished(KJob*)));

    QObject::connect(job, SIGNAL(suspended(KJob*)),
                     this, SLOT(suspended(KJob*)));
    QObject::connect(job, SIGNAL(resumed(KJob*)),
                     this, SLOT(resumed(KJob*)));

    QObject::connect(job, SIGNAL(description(KJob*, const QString&,
                                             const QPair<QString, QString>&,
                                             const QPair<QString, QString>&)),
                     this, SLOT(description(KJob*, const QString&,
                                            const QPair<QString, QString>&,
                                            const QPair<QString, QString>&)));
    QObject::connect(job, SIGNAL(infoMessage(KJob*,QString,QString)),
                     this, SLOT(infoMessage(KJob*,QString,QString)));
    QObject::connect(job, SIGNAL(warning(KJob*,QString,QString)),
                     this, SLOT(warning(KJob*,QString,QString)));

    QObject::connect(job, SIGNAL(totalAmount(KJob*,KJob::Unit,qulonglong)),
                     this, SLOT(totalAmount(KJob*,KJob::Unit,qulonglong)));
    QObject::connect(job, SIGNAL(processedAmount(KJob*,KJob::Unit,qulonglong)),
                     this, SLOT(processedAmount(KJob*,KJob::Unit,qulonglong)));
    QObject::connect(job, SIGNAL(percent(KJob*,ulong)),
                     this, SLOT(percent(KJob*,ulong)));
    QObject::connect(job, SIGNAL(speed(KJob*,ulong)),
                     this, SLOT(speed(KJob*,ulong)));
}

void KJobTrackerInterface::unregisterJob(KJob *job)
{
    job->disconnect(this);
}

void KJobTrackerInterface::finished(KJob *job)
{
    Q_UNUSED(job)
}

void KJobTrackerInterface::suspended(KJob *job)
{
    Q_UNUSED(job)
}

void KJobTrackerInterface::resumed(KJob *job)
{
    Q_UNUSED(job)
}

void KJobTrackerInterface::description(KJob *job, const QString &title,
                                       const QPair<QString, QString> &field1,
                                       const QPair<QString, QString> &field2)
{
    Q_UNUSED(job)
    Q_UNUSED(title)
    Q_UNUSED(field1)
    Q_UNUSED(field2)

}

void KJobTrackerInterface::infoMessage(KJob *job, const QString &plain, const QString &rich)
{
    Q_UNUSED(job)
    Q_UNUSED(plain)
    Q_UNUSED(rich)
}

void KJobTrackerInterface::warning(KJob *job, const QString &plain, const QString &rich)
{
    Q_UNUSED(job)
    Q_UNUSED(plain)
    Q_UNUSED(rich)
}

void KJobTrackerInterface::totalAmount(KJob *job, KJob::Unit unit, qulonglong amount)
{
    Q_UNUSED(job)
    Q_UNUSED(unit)
    Q_UNUSED(amount)
}

void KJobTrackerInterface::processedAmount(KJob *job, KJob::Unit unit, qulonglong amount)
{
    Q_UNUSED(job)
    Q_UNUSED(unit)
    Q_UNUSED(amount)
}

void KJobTrackerInterface::percent(KJob *job, unsigned long percent)
{
    Q_UNUSED(job)
    Q_UNUSED(percent)
}

void KJobTrackerInterface::speed(KJob *job, unsigned long value)
{
    Q_UNUSED(job)
    Q_UNUSED(value)
}

#include "moc_kjobtrackerinterface.cpp"
