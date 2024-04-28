/*
   This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef koTemplateGroup_h
#define koTemplateGroup_h

#include "komain_export.h"
#include <QList>
#include <QStringList>

class KoTemplate;

class KOMAIN_EXPORT KoTemplateGroup
{
public:
    explicit KoTemplateGroup(const QString &name, const QString &dir = QString(), int _sortingWeight = 0, bool touched = false);
    ~KoTemplateGroup();

    QString name() const
    {
        return m_name;
    }
    QStringList dirs() const
    {
        return m_dirs;
    }
    void addDir(const QString &dir)
    {
        m_dirs.append(dir);
        m_touched = true;
    }
    int sortingWeight() const
    {
        return m_sortingWeight;
    }
    void setSortingWeight(int weight)
    {
        m_sortingWeight = weight;
    }
    /// If all children are hidden, we are hidden too
    bool isHidden() const;
    /// if we should hide, we hide all the children
    void setHidden(bool hidden = true) const;

    QList<KoTemplate *> templates() const
    {
        return m_templates;
    }

    bool add(KoTemplate *t, bool force = false, bool touch = true);
    KoTemplate *find(const QString &name) const;

    bool touched() const
    {
        return m_touched;
    }

private:
    QString m_name;
    QStringList m_dirs;
    QList<KoTemplate *> m_templates;
    mutable bool m_touched;
    int m_sortingWeight;
};

#endif
