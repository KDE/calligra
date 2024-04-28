/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Sebastian Sauer <mail@dipe.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef CHAPTERVARIABLE_H
#define CHAPTERVARIABLE_H

#include <QObject>

#include <KoTextPage.h>
#include <KoVariable.h>

class KoShapeSavingContext;

/**
 * This is a KoVariable for chapter variables.
 */
class ChapterVariable : public KoVariable
{
    Q_OBJECT
public:
    ChapterVariable();

    // reimplemented
    QWidget *createOptionsWidget() override;
    void readProperties(const KoProperties *props);
    void saveOdf(KoShapeSavingContext &context) override;
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

private Q_SLOTS:
    void formatChanged(int format);
    void levelChanged(int level);

private:
    void resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd) override;

    enum FormatTypes { ChapterName, ChapterNumber, ChapterNumberName, ChapterPlainNumber, ChapterPlainNumberName };

    FormatTypes m_format;
    int m_level;
};

#endif
