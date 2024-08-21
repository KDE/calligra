/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Smit Patel <smitpatel24@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoBibliographyInfo.h"

#include <KoOdfBibliographyConfiguration.h>
#include <KoParagraphStyle.h>
#include <KoTextSharedLoadingData.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

int KoBibliographyInfo::styleNameToStyleId(KoTextSharedLoadingData *sharedLoadingData, const QString &styleName)
{
    KoParagraphStyle *style = sharedLoadingData->paragraphStyle(styleName, true);
    if (style) {
        return style->styleId();
    }

    return 0;
}

KoBibliographyInfo::KoBibliographyInfo()
    : m_generator(nullptr)
{
}

KoBibliographyInfo::~KoBibliographyInfo()
{
    foreach (const BibliographyEntryTemplate &entryTemplate, m_entryTemplate) {
        qDeleteAll(entryTemplate.indexEntries);
    }
    delete m_generator;
    m_generator = nullptr; // just to be safe
}

void KoBibliographyInfo::loadOdf(KoTextSharedLoadingData *sharedLoadingData, const KoXmlElement &element)
{
    Q_ASSERT(element.localName() == "bibliography-source" && element.namespaceURI() == KoXmlNS::text);

    KoXmlElement p;
    forEachElement(p, element)
    {
        if (p.namespaceURI() != KoXmlNS::text) {
            continue;
        }

        // first child
        if (p.localName() == "index-title-template") {
            m_indexTitleTemplate.styleName = p.attribute("style-name");
            m_indexTitleTemplate.styleId = styleNameToStyleId(sharedLoadingData, m_indexTitleTemplate.styleName);
            m_indexTitleTemplate.text = p.text();
            // second child
        } else if (p.localName() == "bibliography-entry-template") {
            BibliographyEntryTemplate bibEntryTemplate;
            bibEntryTemplate.styleName = p.attribute("style-name");
            bibEntryTemplate.bibliographyType = p.attribute("bibliography-type");
            bibEntryTemplate.styleId = styleNameToStyleId(sharedLoadingData, bibEntryTemplate.styleName);

            KoXmlElement indexEntry;
            forEachElement(indexEntry, p)
            {
                if (indexEntry.namespaceURI() != KoXmlNS::text) {
                    continue;
                }

                if (indexEntry.localName() == "index-entry-bibliography") {
                    // use null String if the style name is not present, it means that we inherit it from the parent
                    IndexEntryBibliography *entryBibliography = new IndexEntryBibliography(indexEntry.attribute("style-name", QString()));

                    entryBibliography->dataField = indexEntry.attribute("bibliography-data-field", "article");
                    bibEntryTemplate.indexEntries.append(static_cast<IndexEntry *>(entryBibliography));

                } else if (indexEntry.localName() == "index-entry-span") {
                    IndexEntrySpan *entrySpan = new IndexEntrySpan(indexEntry.attribute("style-name", QString()));
                    entrySpan->text = indexEntry.text();
                    bibEntryTemplate.indexEntries.append(static_cast<IndexEntry *>(entrySpan));

                } else if (indexEntry.localName() == "index-entry-tab-stop") {
                    IndexEntryTabStop *entryTabStop = new IndexEntryTabStop(indexEntry.attribute("style-name", QString()));

                    QString type = indexEntry.attribute("type", "right"); // left or right
                    if (type == "left") {
                        entryTabStop->tab.type = QTextOption::LeftTab;
                    } else {
                        entryTabStop->tab.type = QTextOption::RightTab;
                    }
                    entryTabStop->setPosition(indexEntry.attribute("position", QString()));
                    entryTabStop->tab.leaderText = indexEntry.attribute("leader-char", ".");
                    bibEntryTemplate.indexEntries.append(static_cast<IndexEntry *>(entryTabStop));
                }
            }
            m_entryTemplate[bibEntryTemplate.bibliographyType] = bibEntryTemplate;

            // third child
        }
    } // forEachElement
}

void KoBibliographyInfo::saveOdf(KoXmlWriter *writer) const
{
    writer->startElement("text:bibliography-source");

    m_indexTitleTemplate.saveOdf(writer);

    foreach (const BibliographyEntryTemplate &entry, m_entryTemplate) {
        entry.saveOdf(writer);
    }

    writer->endElement();
}

void KoBibliographyInfo::setGenerator(BibliographyGeneratorInterface *generator)
{
    delete m_generator;
    m_generator = generator;
}

void KoBibliographyInfo::setEntryTemplates(QMap<QString, BibliographyEntryTemplate> &entryTemplates)
{
    m_entryTemplate = entryTemplates;
}

KoBibliographyInfo *KoBibliographyInfo::clone()
{
    KoBibliographyInfo *newBibInfo = new KoBibliographyInfo();
    newBibInfo->m_entryTemplate.clear();
    newBibInfo->m_name = QString(m_name);
    newBibInfo->m_styleName = QString(m_name);
    newBibInfo->m_indexTitleTemplate = m_indexTitleTemplate;

    for (int i = 0; i < m_entryTemplate.size(); i++) {
        newBibInfo->m_entryTemplate.insert(KoOdfBibliographyConfiguration::bibTypes.at(i), m_entryTemplate[KoOdfBibliographyConfiguration::bibTypes.at(i)]);
    }

    return newBibInfo;
}

BibliographyGeneratorInterface *KoBibliographyInfo::generator() const
{
    return m_generator;
}
