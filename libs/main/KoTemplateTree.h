/*
   This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef koTemplateTree_h
#define koTemplateTree_h

#include "komain_export.h"
#include <QList>

class KoTemplate;
class KoTemplateGroup;

class KOMAIN_EXPORT KoTemplateTree
{
public:
    explicit KoTemplateTree(const QString &templatesResourcePath, bool readTree = false);
    ~KoTemplateTree();

    QString templatesResourcePath() const
    {
        return m_templatesResourcePath;
    }
    void readTemplateTree();
    void writeTemplateTree();

    void add(KoTemplateGroup *g);
    KoTemplateGroup *find(const QString &name) const;

    KoTemplateGroup *defaultGroup() const
    {
        return m_defaultGroup;
    }
    KoTemplate *defaultTemplate() const
    {
        return m_defaultTemplate;
    }

    QList<KoTemplateGroup *> groups() const
    {
        return m_groups;
    }

private:
    void readGroups();
    void readTemplates();
    void writeTemplate(KoTemplate *t, KoTemplateGroup *group, const QString &localDir);

    QString m_templatesResourcePath;
    QList<KoTemplateGroup *> m_groups;
    KoTemplateGroup *m_defaultGroup;
    KoTemplate *m_defaultTemplate;
};

#endif
