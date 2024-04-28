
/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Smit Patel <smitpatel24@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "BibliographyTemplate.h"

#include <BibliographyGenerator.h>
#include <KLocalizedString>
#include <KoBibliographyInfo.h>
#include <KoOdfBibliographyConfiguration.h>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>

BibliographyTemplate::BibliographyTemplate(KoStyleManager *manager)
    : m_manager(manager)
{
    Q_ASSERT(manager);
}

QList<KoBibliographyInfo *> BibliographyTemplate::templates()
{
    // if you are adding your own custom styles specifically for bibliography, add it as an unused style in KoStyleManager
    //  when the bibliography is used the style will be automatically move to the usedStyle section

    QList<KoBibliographyInfo *> predefinedTemplates;
    KoBibliographyInfo *firstTemplate = new KoBibliographyInfo();
    firstTemplate->m_indexTitleTemplate.text = i18n("Bibliography");

    firstTemplate->m_indexTitleTemplate.styleId = m_manager->defaultBibliographyTitleStyle()->styleId();
    firstTemplate->m_indexTitleTemplate.styleName = m_manager->defaultBibliographyTitleStyle()->name();

    foreach (const QString &bibType, KoOdfBibliographyConfiguration::bibTypes) {
        firstTemplate->m_entryTemplate[bibType].styleId = m_manager->defaultBibliographyEntryStyle(bibType)->styleId();
        firstTemplate->m_entryTemplate[bibType].styleName = m_manager->defaultBibliographyEntryStyle(bibType)->name();
    }
    firstTemplate->m_entryTemplate = BibliographyGenerator::defaultBibliographyEntryTemplates();

    KoBibliographyInfo *secondTemplate = new KoBibliographyInfo();
    secondTemplate->m_indexTitleTemplate.text = i18n("References");

    secondTemplate->m_indexTitleTemplate.styleId = m_manager->defaultBibliographyTitleStyle()->styleId();
    secondTemplate->m_indexTitleTemplate.styleName = m_manager->defaultBibliographyTitleStyle()->name();

    foreach (const QString &bibType, KoOdfBibliographyConfiguration::bibTypes) {
        secondTemplate->m_entryTemplate[bibType].styleId = m_manager->defaultBibliographyEntryStyle(bibType)->styleId();
        secondTemplate->m_entryTemplate[bibType].styleName = m_manager->defaultBibliographyEntryStyle(bibType)->name();
    }
    secondTemplate->m_entryTemplate = BibliographyGenerator::defaultBibliographyEntryTemplates();

    predefinedTemplates.append(firstTemplate);
    predefinedTemplates.append(secondTemplate);
    return predefinedTemplates;
}

void BibliographyTemplate::moveTemplateToUsed(KoBibliographyInfo *info)
{
    if (m_manager->unusedStyle(info->m_indexTitleTemplate.styleId)) {
        m_manager->moveToUsedStyles(info->m_indexTitleTemplate.styleId);
    }

    foreach (const QString &bibType, KoOdfBibliographyConfiguration::bibTypes) {
        if (m_manager->unusedStyle(info->m_entryTemplate[bibType].styleId)) {
            m_manager->moveToUsedStyles(info->m_entryTemplate[bibType].styleId);
        }
    }
}
