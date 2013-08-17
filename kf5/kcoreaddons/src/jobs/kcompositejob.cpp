/*  This file is part of the KDE project
    Copyright (C) 2006 Kevin Ottens <ervin@kde.org>

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

#include "kcompositejob.h"
#include "kcompositejob_p.h"

KCompositeJobPrivate::KCompositeJobPrivate()
{
}

KCompositeJobPrivate::~KCompositeJobPrivate()
{
}

KCompositeJob::KCompositeJob( QObject *parent )
    : KJob( *new KCompositeJobPrivate, parent )
{
}

KCompositeJob::KCompositeJob( KCompositeJobPrivate &dd, QObject *parent )
    : KJob( dd, parent)
{
}

KCompositeJob::~KCompositeJob()
{
}

bool KCompositeJob::addSubjob( KJob *job )
{
    Q_D(KCompositeJob);
    if ( job == 0 || d->subjobs.contains( job ) )
    {
        return false;
    }

    job->setParent(this);
    d->subjobs.append(job);
    connect( job, SIGNAL(result(KJob*)),
             SLOT(slotResult(KJob*)) );

    // Forward information from that subjob.
    connect( job, SIGNAL(infoMessage(KJob*,QString,QString)),
             SLOT(slotInfoMessage(KJob*,QString,QString)) );

    return true;
}

bool KCompositeJob::removeSubjob( KJob *job )
{
    Q_D(KCompositeJob);
    if ( job == 0 )
    {
        return false;
    }

    job->setParent(0);
    d->subjobs.removeAll( job );

    return true;
}

bool KCompositeJob::hasSubjobs() const
{
    return !d_func()->subjobs.isEmpty();
}

const QList<KJob*> &KCompositeJob::subjobs() const
{
    return d_func()->subjobs;
}

void KCompositeJob::clearSubjobs()
{
    Q_D(KCompositeJob);
    Q_FOREACH(KJob *job, d->subjobs) {
        job->setParent(0);
    }
    d->subjobs.clear();
}

void KCompositeJob::slotResult( KJob *job )
{
    // Did job have an error ?
    if ( job->error() && !error() )
    {
        // Store it in the parent only if first error
        setError( job->error() );
        setErrorText( job->errorText() );
        emitResult();
    }

    removeSubjob(job);
}

void KCompositeJob::slotInfoMessage( KJob *job, const QString &plain, const QString &rich )
{
    emit infoMessage( job, plain, rich );
}

#include "moc_kcompositejob.cpp"
