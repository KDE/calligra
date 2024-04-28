/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Smit Patel <smitpatel24@gmail.com>

 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef BIBLIOGRAPHYGENERATOR_H
#define BIBLIOGRAPHYGENERATOR_H

#include "kotext_export.h"
#include <KoBibliographyInfo.h>

#include <QTextBlock>

class KOTEXT_EXPORT BibliographyGenerator : public QObject, public BibliographyGeneratorInterface
{
    Q_OBJECT
public:
    explicit BibliographyGenerator(QTextDocument *bibDocument, const QTextBlock &block, KoBibliographyInfo *bibInfo);
    ~BibliographyGenerator() override;

    static QMap<QString, BibliographyEntryTemplate> defaultBibliographyEntryTemplates();

public Q_SLOTS:
    void generate();

private:
    QTextDocument *m_document;
    QTextDocument *m_bibDocument;
    KoBibliographyInfo *m_bibInfo;
    QTextBlock m_block;
    qreal m_maxTabPosition;
};

#endif
