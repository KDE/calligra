/*
 * SPDX-FileCopyrightText: 2011 Lukáš Tvrdý <lukas.tvrdy@ixonos.com>
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_TABLE_OF_CONTENTS_GENERATOR_INFO
#define KO_TABLE_OF_CONTENTS_GENERATOR_INFO

#define ppVar(var) #var << "=" << var
// #define DEBUG_TOC_STRUCTURE

#include <QList>
#include <QString>

#include "ToCBibGeneratorInfo.h"

class KoTextSharedLoadingData;
class KoXmlWriter;

class KOTEXT_EXPORT KoTableOfContentsGeneratorInfo
{
public:
    explicit KoTableOfContentsGeneratorInfo(bool generateEntryTemplate = true);
    ~KoTableOfContentsGeneratorInfo();
    void loadOdf(KoTextSharedLoadingData *sharedLoadingData, const KoXmlElement &element);
    void saveOdf(KoXmlWriter *writer) const;
    KoTableOfContentsGeneratorInfo *clone();

    QString m_name;
    QString m_styleName;
    // TODO: add support for those according ODF v1.2
    // text: protected
    // text: protection-key
    // text:protection-key-digest-algorithm
    // xml:id
    QString m_indexScope; // enum {document, chapter}
    int m_outlineLevel;
    bool m_relativeTabStopPosition;
    bool m_useIndexMarks;
    bool m_useIndexSourceStyles;
    bool m_useOutlineLevel;

    IndexTitleTemplate m_indexTitleTemplate;
    QList<TocEntryTemplate> m_entryTemplate; // N-entries
    QList<IndexSourceStyles> m_indexSourceStyles;

private:
    int styleNameToStyleId(KoTextSharedLoadingData *sharedLoadingData, const QString &styleName);
};

Q_DECLARE_METATYPE(KoTableOfContentsGeneratorInfo *)

#endif
