/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TableOfContentsTemplate.h"

#include <KLocalizedString>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>
#include <KoTableOfContentsGeneratorInfo.h>

TableOfContentsTemplate::TableOfContentsTemplate(KoStyleManager *manager)
    : m_manager(manager)
{
    Q_ASSERT(manager);
}

QList<KoTableOfContentsGeneratorInfo *> TableOfContentsTemplate::templates()
{
    // if you are adding your own custom styles specifically for ToC, add it as an unused style in KoStyleManager
    //  when the ToC is used the style will be automatically move to the usedStyle section

    QList<KoTableOfContentsGeneratorInfo *> predefinedTemplates;
    KoTableOfContentsGeneratorInfo *firstTemplate = new KoTableOfContentsGeneratorInfo();
    firstTemplate->m_indexTitleTemplate.text = i18n("Table Of Contents");

    firstTemplate->m_indexTitleTemplate.styleId = m_manager->defaultTableOfcontentsTitleStyle()->styleId();
    firstTemplate->m_indexTitleTemplate.styleName = m_manager->defaultTableOfcontentsTitleStyle()->name();

    for (int level = 1; level <= firstTemplate->m_outlineLevel; ++level) {
        firstTemplate->m_entryTemplate[level - 1].styleId = m_manager->defaultTableOfContentsEntryStyle(level)->styleId();
        firstTemplate->m_entryTemplate[level - 1].styleName = m_manager->defaultTableOfContentsEntryStyle(level)->name();
    }

    KoTableOfContentsGeneratorInfo *secondTemplate = new KoTableOfContentsGeneratorInfo();
    secondTemplate->m_indexTitleTemplate.text = i18n("Contents");

    secondTemplate->m_indexTitleTemplate.styleId = m_manager->defaultTableOfcontentsTitleStyle()->styleId();
    secondTemplate->m_indexTitleTemplate.styleName = m_manager->defaultTableOfcontentsTitleStyle()->name();

    for (int level = 1; level <= firstTemplate->m_outlineLevel; ++level) {
        secondTemplate->m_entryTemplate[level - 1].styleId = m_manager->defaultTableOfContentsEntryStyle(level)->styleId();
        secondTemplate->m_entryTemplate[level - 1].styleName = m_manager->defaultTableOfContentsEntryStyle(level)->name();
    }

    predefinedTemplates.append(firstTemplate);
    predefinedTemplates.append(secondTemplate);
    return predefinedTemplates;
}

void TableOfContentsTemplate::moveTemplateToUsed(KoTableOfContentsGeneratorInfo *info)
{
    if (m_manager->unusedStyle(info->m_indexTitleTemplate.styleId)) {
        m_manager->moveToUsedStyles(info->m_indexTitleTemplate.styleId);
    }

    for (int level = 1; level <= info->m_outlineLevel; level++) {
        if (m_manager->unusedStyle(info->m_entryTemplate[level - 1].styleId)) {
            m_manager->moveToUsedStyles(info->m_entryTemplate[level - 1].styleId);
        }
    }
}
