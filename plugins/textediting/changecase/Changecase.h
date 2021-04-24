/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHANGECASE_H
#define CHANGECASE_H

#include <QTextCursor>

#include <KoTextEditingPlugin.h>

class QTextDocument;
class QRadioButton;

class Changecase : public KoTextEditingPlugin
{
    Q_OBJECT
public:
    Changecase();

    void finishedWord(QTextDocument *document, int cursorPosition) override;
    void finishedParagraph(QTextDocument *document, int cursorPosition) override;
    void startingSimpleEdit(QTextDocument *document, int cursorPosition) override;
    void checkSection(QTextDocument *document, int startPosition, int endPosition) override;

private Q_SLOTS:
    void process();

private:
    void sentenceCase();
    void lowerCase();
    void upperCase();
    void initialCaps();
    void toggleCase();

    QRadioButton *m_sentenceCaseRadio;
    QRadioButton *m_lowerCaseRadio;
    QRadioButton *m_upperCaseRadio;
    QRadioButton *m_initialCapsRadio;
    QRadioButton *m_toggleCaseRadio;

    QTextCursor m_cursor;
    QTextDocument *m_document;
    int m_startPosition;
    int m_endPosition;
};

#endif
