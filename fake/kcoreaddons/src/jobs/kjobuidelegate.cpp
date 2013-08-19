/* This file is part of the KDE libraries
    Copyright (C) 2000 Stephan Kulow <coolo@kde.org>
                       David Faure <faure@kde.org>
    Copyright (C) 2006 Kevin Ottens <ervin@kde.org>

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

#include "kjobuidelegate.h"
#include "kjob.h"

class KJobUiDelegate::Private
{
public:
    Private(KJobUiDelegate *delegate)
        : q(delegate), job( 0 ),
          autoErrorHandling( false ),
          autoWarningHandling( true ) { }

    KJobUiDelegate * const q;

    KJob *job;
    bool autoErrorHandling : 1;
    bool autoWarningHandling : 1;

    void connectJob(KJob *job);
    void _k_result(KJob *job);
};

KJobUiDelegate::KJobUiDelegate()
    : QObject(), d(new Private(this))
{

}

KJobUiDelegate::~KJobUiDelegate()
{
    delete d;
}

bool KJobUiDelegate::setJob( KJob *job )
{
    if ( d->job!=0 )
    {
        return false;
    }

    d->job = job;
    setParent( job );

    return true;
}

KJob *KJobUiDelegate::job() const
{
    return d->job;
}

void KJobUiDelegate::showErrorMessage()
{
}

void KJobUiDelegate::setAutoErrorHandlingEnabled( bool enable)
{
    d->autoErrorHandling = enable;
}

bool KJobUiDelegate::isAutoErrorHandlingEnabled() const
{
    return d->autoErrorHandling;
}

void KJobUiDelegate::setAutoWarningHandlingEnabled( bool enable )
{
    d->autoWarningHandling = enable;
}

bool KJobUiDelegate::isAutoWarningHandlingEnabled() const
{
    return d->autoWarningHandling;
}

void KJobUiDelegate::slotWarning(KJob *job, const QString &plain,
                                  const QString &rich)
{
    Q_UNUSED(job)
    Q_UNUSED(plain)
    Q_UNUSED(rich)
}

void KJobUiDelegate::connectJob(KJob *job)
{
    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(_k_result(KJob*)));

    connect(job, SIGNAL(warning(KJob*,QString,QString)),
            this, SLOT(slotWarning(KJob*,QString,QString)));
}

void KJobUiDelegate::Private::_k_result(KJob *job2)
{
    Q_UNUSED(job2)
    if ( job->error() && autoErrorHandling )
        q->showErrorMessage();
}

#include "moc_kjobuidelegate.cpp"
