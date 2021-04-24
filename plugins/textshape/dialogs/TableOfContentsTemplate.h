/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TABLEOFCONTENTSTEMPLATE_H
#define TABLEOFCONTENTSTEMPLATE_H

#include <QList>

class KoTableOfContentsGeneratorInfo;
class KoStyleManager;

class TableOfContentsTemplate
{
public:
    explicit TableOfContentsTemplate(KoStyleManager *manager);

    QList<KoTableOfContentsGeneratorInfo *> templates();

    /// this method moves the styles used in info ToC from unused styles list to used
    void moveTemplateToUsed(KoTableOfContentsGeneratorInfo *info);

private:
    KoStyleManager *m_manager;
};

#endif // TABLEOFCONTENTSTEMPLATE_H
