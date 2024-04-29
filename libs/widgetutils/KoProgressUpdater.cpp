/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoProgressUpdater.h"

#include <QApplication>
#include <QString>
#include <QTextStream>
#include <QTimer>

#include "KoProgressProxy.h"
#include "KoUpdater.h"
#include "KoUpdaterPrivate_p.h"

// 4 updates per second should be enough
#define PROGRESSUPDATER_GUITIMERINTERVAL 250

class Q_DECL_HIDDEN KoProgressUpdater::Private
{
public:
    Private(KoProgressUpdater *_parent, KoProgressProxy *p, Mode _mode, QTextStream *output_ = nullptr)
        : parent(_parent)
        , progressBar(p)
        , mode(_mode)
        , totalWeight(0)
        , currentProgress(0)
        , updated(false)
        , output(output_)
        , updateGuiTimer(_parent)
        , canceled(false)
    {
    }

    KoProgressUpdater *parent;
    KoProgressProxy *progressBar;
    Mode mode;
    int totalWeight;
    int currentProgress;
    bool updated; // is true whenever the progress needs to be recomputed
    QTextStream *output;
    QTimer updateGuiTimer; // fires regularly to update the progress bar widget
    QList<QPointer<KoUpdaterPrivate>> subtasks;
    QList<QPointer<KoUpdater>> subTaskWrappers; // We delete these
    QTime referenceTime;

    static void logEvents(QTextStream &out, KoProgressUpdater::Private *updater, QTime startTime, const QString &prefix);
    bool canceled;
};

// NOTE: do not make the KoProgressUpdater object part of the QObject
// hierarchy. Do not make KoProgressProxy its parent (note that KoProgressProxy
// is not necessarily castable to QObject ). This prevents proper functioning
// of progress reporting in multi-threaded environments.
KoProgressUpdater::KoProgressUpdater(KoProgressProxy *progressBar, Mode mode, QTextStream *output)
    : d(new Private(this, progressBar, mode, output))
{
    Q_ASSERT(d->progressBar);
    connect(&d->updateGuiTimer, &QTimer::timeout, this, &KoProgressUpdater::updateUi);
}

KoProgressUpdater::~KoProgressUpdater()
{
    if (d->output) {
        Private::logEvents(*d->output, d, referenceTime(), QLatin1String(""));
    }
    d->progressBar->setValue(d->progressBar->maximum());

    // make sure to stop the timer to avoid accessing
    // the data we are going to delete right now
    d->updateGuiTimer.stop();

    qDeleteAll(d->subtasks);
    d->subtasks.clear();

    qDeleteAll(d->subTaskWrappers);
    d->subTaskWrappers.clear();

    delete d;
}

void KoProgressUpdater::setReferenceTime(QTime referenceTime)
{
    d->referenceTime = referenceTime;
}

QTime KoProgressUpdater::referenceTime() const
{
    return d->referenceTime;
}

void KoProgressUpdater::start(int range, const QString &text)
{
    d->updateGuiTimer.start(PROGRESSUPDATER_GUITIMERINTERVAL);

    qDeleteAll(d->subtasks);
    d->subtasks.clear();

    qDeleteAll(d->subTaskWrappers);
    d->subTaskWrappers.clear();

    d->progressBar->setRange(0, range - 1);
    d->progressBar->setValue(0);

    if (!text.isEmpty()) {
        d->progressBar->setFormat(text);
    }
    d->totalWeight = 0;
    d->canceled = false;
}

QPointer<KoUpdater> KoProgressUpdater::startSubtask(int weight, const QString &name)
{
    KoUpdaterPrivate *p = new KoUpdaterPrivate(this, weight, name);
    d->totalWeight += weight;
    d->subtasks.append(p);
    connect(p, &KoUpdaterPrivate::sigUpdated, this, &KoProgressUpdater::update);

    QPointer<KoUpdater> updater = new KoUpdater(p);
    d->subTaskWrappers.append(updater);

    if (!d->updateGuiTimer.isActive()) {
        // we maybe need to restart the timer if it was stopped in updateUi() cause
        // other sub-tasks created before this one finished already.
        d->updateGuiTimer.start(PROGRESSUPDATER_GUITIMERINTERVAL);
    }

    return updater;
}

void KoProgressUpdater::cancel()
{
    for (KoUpdaterPrivate *updater : std::as_const(d->subtasks)) {
        updater->setProgress(100);
        updater->interrupt();
    }
    d->canceled = true;
    updateUi();
}

void KoProgressUpdater::update()
{
    d->updated = true;
    if (d->mode == Unthreaded) {
        qApp->processEvents();
    }
}

void KoProgressUpdater::updateUi()
{
    // This function runs in the app main thread. All the progress
    // updates arrive at the KoUpdaterPrivate instances through
    // queued connections, so until we relinquish control to the
    // event loop, the progress values cannot change, and that
    // won't happen until we return from this function (which is
    // triggered by a timer)

    if (d->updated) {
        int totalProgress = 0;
        for (QPointer<KoUpdaterPrivate> updater : std::as_const(d->subtasks)) {
            if (updater->interrupted()) {
                d->currentProgress = -1;
                return;
            }

            int progress = updater->progress();
            if (progress > 100 || progress < 0) {
                progress = updater->progress();
            }

            totalProgress += progress * updater->weight();
        }

        d->currentProgress = totalProgress / d->totalWeight;
        d->updated = false;
    }

    if (d->currentProgress == -1) {
        d->progressBar->setValue(d->progressBar->maximum());
        // should we hide the progressbar after a little while?
        return;
    }

    if (d->currentProgress >= d->progressBar->maximum()) {
        // we're done
        d->updateGuiTimer.stop(); // 10 updates/second should be enough?
    }
    d->progressBar->setValue(d->currentProgress);
}

bool KoProgressUpdater::interrupted() const
{
    return d->canceled;
}

bool KoProgressUpdater::hasOutput() const
{
    return d->output != nullptr;
}

void KoProgressUpdater::Private::logEvents(QTextStream &out, KoProgressUpdater::Private *updater, QTime startTime, const QString &prefix)
{
    // initial implementation: write out the names of all events
    for (QPointer<KoUpdaterPrivate> p : std::as_const(updater->subtasks)) {
        if (!p)
            continue;
        const auto points = p->getPoints();
        for (const KoUpdaterPrivate::TimePoint &tp : points) {
            out << prefix + p->objectName() << '\t' << startTime.msecsTo(tp.time) << '\t' << tp.value << Qt::endl;
        }
    }
}
