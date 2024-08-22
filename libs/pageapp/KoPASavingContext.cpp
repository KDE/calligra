/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPASavingContext.h"

#include <QRegularExpression>

#include "KoPAPage.h"

KoPASavingContext::KoPASavingContext(KoXmlWriter &xmlWriter, KoGenStyles &mainStyles, KoEmbeddedDocumentSaver &embeddedSaver, int page)
    : KoShapeSavingContext(xmlWriter, mainStyles, embeddedSaver)
    , m_page(page)
    , m_masterPageIndex(0)
    , m_clearDrawIds(false)
{
}

KoPASavingContext::~KoPASavingContext() = default;

void KoPASavingContext::addMasterPage(const KoPAMasterPage *masterPage, const QString &name)
{
    m_masterPageNames.insert(masterPage, name);
}

QString KoPASavingContext::masterPageName(const KoPAMasterPage *masterPage) const
{
    QMap<const KoPAMasterPage *, QString>::const_iterator it(m_masterPageNames.find(masterPage));
    if (it != m_masterPageNames.constEnd()) {
        return it.value();
    }

    // this should not happen
    Q_ASSERT(it != m_masterPageNames.constEnd());
    return QString();
}

QString KoPASavingContext::masterPageElementName()
{
    if (!isSet(KoShapeSavingContext::UniqueMasterPages)) {
        ++m_masterPageIndex;
    }
    return QString("content_%1").arg(m_masterPageIndex);
}

void KoPASavingContext::incrementPage()
{
    m_page++;
}

int KoPASavingContext::page()
{
    return m_page;
}

void KoPASavingContext::setClearDrawIds(bool clear)
{
    m_clearDrawIds = clear;
}

bool KoPASavingContext::isSetClearDrawIds()
{
    return m_clearDrawIds;
}

QString KoPASavingContext::pageName(const KoPAPage *page)
{
    QString name;
    QMap<const KoPAPage *, QString>::ConstIterator it(m_pageToNames.constFind(page));
    if (it != m_pageToNames.constEnd()) {
        name = it.value();
    } else {
        name = page->name();
        QRegularExpression rx("^page[0-9]+$");
        if (name.isEmpty() || m_pageNames.contains(name) || name.indexOf(rx) != -1) {
            name = "page" + QString::number(m_page);
        }
        Q_ASSERT(!m_pageNames.contains(name));
        m_pageNames.insert(name);
        m_pageToNames.insert(page, name);
    }
    return name;
}
