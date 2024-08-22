/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Smit Patel <smitpatel24@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOBIBLIOGRAPHYINFO_H
#define KOBIBLIOGRAPHYINFO_H

#include "ToCBibGeneratorInfo.h"
#include "kotext_export.h"
#include <KoXmlReaderForward.h>

class KoTextSharedLoadingData;
class KoXmlWriter;

class KOTEXT_EXPORT BibliographyGeneratorInterface
{
public:
    BibliographyGeneratorInterface() = default;
    virtual ~BibliographyGeneratorInterface() = default;
    // virtual void setMaxTabPosition(qreal maxTabPosition) = 0;
};

class KOTEXT_EXPORT KoBibliographyInfo
{
public:
    KoBibliographyInfo();

    ~KoBibliographyInfo();

    void loadOdf(KoTextSharedLoadingData *sharedLoadingData, const KoXmlElement &element);
    void saveOdf(KoXmlWriter *writer) const;

    void setGenerator(BibliographyGeneratorInterface *generator);
    void setEntryTemplates(QMap<QString, BibliographyEntryTemplate> &entryTemplates);

    KoBibliographyInfo *clone();
    BibliographyGeneratorInterface *generator() const;

    QString m_name;
    QString m_styleName;
    IndexTitleTemplate m_indexTitleTemplate;
    QMap<QString, BibliographyEntryTemplate> m_entryTemplate;

private:
    int styleNameToStyleId(KoTextSharedLoadingData *sharedLoadingData, const QString &styleName);
    BibliographyGeneratorInterface *m_generator;
};

Q_DECLARE_METATYPE(KoBibliographyInfo *)

#endif // KOBIBLIOGRAPHYINFO_H
