/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Smit Patel <smitpatel24@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef BIBLIOGRAPHYTEMPLATE_H
#define BIBLIOGRAPHYTEMPLATE_H

#include <QList>

class KoBibliographyInfo;
class KoStyleManager;

class BibliographyTemplate
{
public:
    explicit BibliographyTemplate(KoStyleManager *manager);

    QList<KoBibliographyInfo *> templates();

    /// this method moves the styles used in info Bibliography from unused styles list to used
    void moveTemplateToUsed(KoBibliographyInfo *info);

private:
    KoStyleManager *m_manager;
};

#endif // BIBLIOGRAPHYTEMPLATE_H
