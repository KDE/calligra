/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ODFTEXTREADERDOCXBACKEND_H
#define ODFTEXTREADERDOCXBACKEND_H

// Calligra
#include <KoFilter.h>
#include <KoXmlStreamReader.h>
#include <OdfTextReaderBackend.h>

class KoOdfStyleProperties;
class OdfReaderContext;
class OdfReaderDocxContext;

class OdfTextReaderDocxBackend : public OdfTextReaderBackend
{
public:
    OdfTextReaderDocxBackend();
    ~OdfTextReaderDocxBackend() override;

    // ----------------------------------------------------------------
    // Text level functions: paragraphs, headings, sections, frames, objects, etc

    void elementTextH(KoXmlStreamReader &reader, OdfReaderContext *context) override;
    void elementTextP(KoXmlStreamReader &reader, OdfReaderContext *context) override;

    // ----------------------------------------------------------------
    // Paragraph level functions: spans, annotations, notes, text content itself, etc.

    void elementOfficeAnnotation(KoXmlStreamReader &reader, OdfReaderContext *context) override;

    void elementDcCreator(KoXmlStreamReader &reader, OdfReaderContext *context) override;
    void elementDcDate(KoXmlStreamReader &reader, OdfReaderContext *context) override;

    void elementTextSpan(KoXmlStreamReader &reader, OdfReaderContext *context) override;
    void elementTextS(KoXmlStreamReader &reader, OdfReaderContext *context) override;

    void characterData(KoXmlStreamReader &reader, OdfReaderContext *context) override;

private:
    void startRun(const KoXmlStreamReader &reader, OdfReaderDocxContext *context);
    void endRun(OdfReaderDocxContext *context);

private:
    int m_insideSpanLevel; // Number of nexted <text:span> levels.
    int m_currentOutlineLevel;
    int m_commentIndex;
    bool m_writeComment;
    bool m_insideComment;
    bool m_insideDcCreator;
    bool m_insideDcDate;
    KoOdfStyleProperties *m_currentParagraphTextProperties;
    QString m_currentParagraphParent;
};

#endif // ODFTEXTREADERDOCXBACKEND_H
