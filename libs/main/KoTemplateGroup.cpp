/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoTemplateGroup.h"

#include <KoTemplate.h>

#include <QFile>

KoTemplateGroup::KoTemplateGroup(const QString &name, const QString &dir, int _sortingWeight, bool touched)
    : m_name(name)
    , m_touched(touched)
    , m_sortingWeight(_sortingWeight)
{
    m_dirs.append(dir);
}

KoTemplateGroup::~KoTemplateGroup()
{
    qDeleteAll(m_templates);
}

bool KoTemplateGroup::isHidden() const
{
    QList<KoTemplate *>::const_iterator it = m_templates.begin();
    bool hidden = true;
    while (it != m_templates.end() && hidden) {
        hidden = (*it)->isHidden();
        ++it;
    }
    return hidden;
}

void KoTemplateGroup::setHidden(bool hidden) const
{
    foreach (KoTemplate *t, m_templates)
        t->setHidden(hidden);

    m_touched = true;
}

bool KoTemplateGroup::add(KoTemplate *t, bool force, bool touch)
{
    KoTemplate *myTemplate = find(t->name());
    if (myTemplate == nullptr) {
        m_templates.append(t);
        m_touched = touch;
        return true;
    } else if (myTemplate && force) {
        // kDebug( 30003 ) <<"removing :" << myTemplate->fileName();
        QFile::remove(myTemplate->fileName());
        QFile::remove(myTemplate->picture());
        QFile::remove(myTemplate->file());
        m_templates.removeAll(myTemplate);
        delete myTemplate;
        m_templates.append(t);
        m_touched = touch;
        return true;
    } else if (myTemplate) {
        m_templates.append(t);
        return true;
    }
    return false;
}

KoTemplate *KoTemplateGroup::find(const QString &name) const
{
    QList<KoTemplate *>::const_iterator it = m_templates.begin();
    KoTemplate *ret = nullptr;

    while (it != m_templates.end()) {
        if ((*it)->name() == name) {
            ret = *it;
            break;
        }

        ++it;
    }

    return ret;
}
