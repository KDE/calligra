/* This file is part of the Calligra libraries
   SPDX-FileCopyrightText: 2001 Werner Trobin <trobin@kde.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "KoFilterChainLink.h"
#include "KoFilterEntry.h"
#include "KoFilterManager.h"
#include "KoProgressUpdater.h"
#include "KoUpdater.h"
#include <MainDebug.h>
#include <QMetaMethod>
#include <QPluginLoader>

namespace
{
const char SIGNAL_PREFIX[] = "commSignal";
const int SIGNAL_PREFIX_LEN = 10;
const char SLOT_PREFIX[] = "commSlot";
const int SLOT_PREFIX_LEN = 8;

KoUpdater *createUpdater(KoFilterChain *chain)
{
    QPointer<KoUpdater> updater = nullptr;
    Q_ASSERT(chain);
    Q_ASSERT(chain->manager());
    KoProgressUpdater *pu = chain->manager()->progressUpdater();
    if (pu) {
        updater = pu->startSubtask(1, "filter");
        updater->setProgress(0);
    }

    return updater;
}
}

namespace CalligraFilter
{

ChainLink::ChainLink(KoFilterChain *chain, KoFilterEntry::Ptr filterEntry, const QByteArray &from, const QByteArray &to)
    : m_chain(chain)
    , m_filterEntry(filterEntry)
    , m_from(from)
    , m_to(to)
    , m_filter(nullptr)
    , m_updater(createUpdater(chain))
{
}

ChainLink::~ChainLink() = default;

KoFilter::ConversionStatus ChainLink::invokeFilter(const ChainLink *const parentChainLink)
{
    if (!m_filterEntry) {
        errorFilter << "This filter entry is null. Strange stuff going on." << Qt::endl;
        return KoFilter::FilterEntryNull;
    }

    m_filter = m_filterEntry->createFilter(m_chain);

    if (!m_filter) {
        errorFilter << "Couldn't create the filter." << Qt::endl;
        return KoFilter::FilterCreationError;
    }

    if (m_updater) {
        // if there is an updater, use that for progress reporting
        m_filter->setUpdater(m_updater);
    }

    if (parentChainLink) {
        setupCommunication(parentChainLink->m_filter);
    }

    KoFilter::ConversionStatus status = m_filter->convert(m_from, m_to);
    delete m_filter;
    m_filter = nullptr;
    if (m_updater) {
        m_updater->setProgress(100);
    }
    return status;
}

void ChainLink::dump() const
{
    debugFilter << "   Link:" << m_filterEntry->fileName();
}

void ChainLink::setupCommunication(const KoFilter *const parentFilter) const
{
    if (!parentFilter)
        return;

    const QMetaObject *const parent = parentFilter->metaObject();
    const QMetaObject *const child = m_filter->metaObject();
    if (!parent || !child)
        return;

    setupConnections(parentFilter, m_filter);
    setupConnections(m_filter, parentFilter);
}

void ChainLink::setupConnections(const KoFilter *sender, const KoFilter *receiver) const
{
    const QMetaObject *const parent = sender->metaObject();
    const QMetaObject *const child = receiver->metaObject();
    if (!parent || !child)
        return;

    int senderMethodCount = parent->methodCount();
    for (int i = 0; i < senderMethodCount; ++i) {
        QMetaMethod metaMethodSignal = parent->method(i);
        if (metaMethodSignal.methodType() != QMetaMethod::Signal)
            continue;
        // ### untested (QMetaMethod::signature())
        if (strncmp(metaMethodSignal.methodSignature(), SIGNAL_PREFIX, SIGNAL_PREFIX_LEN) == 0) {
            int receiverMethodCount = child->methodCount();
            for (int j = 0; j < receiverMethodCount; ++j) {
                QMetaMethod metaMethodSlot = child->method(j);
                if (metaMethodSlot.methodType() != QMetaMethod::Slot)
                    continue;
                if (strncmp(metaMethodSlot.methodSignature().constData(), SLOT_PREFIX, SLOT_PREFIX_LEN) == 0) {
                    if (strcmp(metaMethodSignal.methodSignature().constData() + SIGNAL_PREFIX_LEN,
                               metaMethodSlot.methodSignature().constData() + SLOT_PREFIX_LEN)
                        == 0) {
                        QByteArray signalString;
                        signalString.setNum(QSIGNAL_CODE);
                        signalString += metaMethodSignal.methodSignature();
                        QByteArray slotString;
                        slotString.setNum(QSLOT_CODE);
                        slotString += metaMethodSlot.methodSignature();
                        QObject::connect(sender, signalString, receiver, slotString);
                    }
                }
            }
        }
    }
}

}
