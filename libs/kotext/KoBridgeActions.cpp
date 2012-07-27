/* This file is part of the KDE project
 * Copyright (C) 2012 Smit Patel <smitpatel24@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoBridgeActions.h"
#include "BibliographyDb.h"
#include "KoInlineCite.h"
#include "KoTextEditor.h"
#include "KoBibliographyInfo.h"
#include <KoOdfBibliographyConfiguration.h>

#include <QDataStream>
#include <QVariantMap>

KoBridgeAction::KoBridgeAction(const QVariantMap &map, Action _name) :
    m_data(map),
    m_out(&m_block, QIODevice::WriteOnly),
    name(_name)
{
}

KoBridgeAction::~KoBridgeAction()
{

}

QByteArray KoBridgeAction::data()
{
    return m_block;
}

InsertCitationBridgeAction::InsertCitationBridgeAction(const QVariantMap &map, KoTextEditor *editor) :
    KoBridgeAction(map, KoBridgeAction::InsertCitation),
    m_editor(editor)
{
    Q_ASSERT(m_editor);

    performAction();
}

void InsertCitationBridgeAction::performAction()
{
    KoInlineCite *cite = m_editor->insertCitation();

    foreach(const QString field, KoOdfBibliographyConfiguration::bibDataFields) {
        cite->setField(field, m_data[field].toString());
    }
}

InsertBibliographyBridgeAction::InsertBibliographyBridgeAction(const QVariantMap &map, KoTextEditor *editor) :
    KoBridgeAction(map, KoBridgeAction::InsertBibliography),
    m_editor(editor)
{
    performAction();
}

void InsertBibliographyBridgeAction::performAction()
{
    KoBibliographyInfo *info = new KoBibliographyInfo;
    QVariantMap bibSource = m_data["bibliography-source"].toMap();
    info->m_indexTitleTemplate.text = bibSource["index-title"].toString();

    foreach(QVariant temp, bibSource["bibliography-entry-template"].toList()) {
        QVariantMap entryTemplateMap = temp.toMap();

        if (KoOdfBibliographyConfiguration::bibTypes.contains(entryTemplateMap["-bibliography-type"].toString())) {
            BibliographyEntryTemplate entryTemplate;
            entryTemplate.bibliographyType = entryTemplateMap["-bibliography-type"].toString();

            foreach (QVariant e, entryTemplateMap["index-entry"].toList()) {
                QVariantMap entryMap = e.toMap();

                if (entryMap["-entry-type"].toString() == "bibliography") {
                    IndexEntryBibliography *bibIndexEntry = new IndexEntryBibliography(QString());
                    bibIndexEntry->dataField = entryMap["-bibliography-data-field"].toString();
                    entryTemplate.indexEntries.append(bibIndexEntry);
                } else if (entryMap["-entry-type"].toString() == "span") {
                    IndexEntrySpan *spanIndexEntry = new IndexEntrySpan(QString());
                    spanIndexEntry->text = entryMap["-span-text"].toString();
                    entryTemplate.indexEntries.append(spanIndexEntry);
                }
            }
            info->m_entryTemplate[entryTemplateMap["-bibliography-type"].toString()] = entryTemplate;
        }
    }

    QList<KoInlineCite *> cites;
    foreach(QVariant c, m_data["bibliography-entry"].toList()) {
        QVariantMap citeMap = c.toMap();

        KoInlineCite *cite = new KoInlineCite(KoInlineCite::Citation);
        foreach(QString field, citeMap.keys()) {
            cite->setField(field, citeMap[field].toString());
        }
        cites.append(cite);
    }
    m_editor->insertBibliography(info, cites);
}

InsertCiteRecordBridgeAction::InsertCiteRecordBridgeAction(const QVariantMap &map, BibliographyDb *biblioDb) :
    KoBridgeAction(map, KoBridgeAction::InsertCiteRecord),
    m_db(biblioDb)
{
    performAction();
}

void InsertCiteRecordBridgeAction::performAction()
{
    KoInlineCite *cite = new KoInlineCite(KoInlineCite::Citation);

    foreach(const QString field, KoOdfBibliographyConfiguration::bibDataFields) {
        cite->setField(field, m_data[field].toString());
    }
    m_db->insertCitation(cite);
}
