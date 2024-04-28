/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef AUTOCORRECT_H
#define AUTOCORRECT_H

#include <KoTextEditingPlugin.h>
#include <libs/autocorrection.h>

class QTextDocument;

class Autocorrect : public KoTextEditingPlugin, public AutoCorrection
{
    Q_OBJECT

public:
    Autocorrect();
    ~Autocorrect() override;

    void finishedWord(QTextDocument *document, int cursorPosition) override;
    void finishedParagraph(QTextDocument *document, int cursorPosition) override;
    void startingSimpleEdit(QTextDocument *document, int cursorPosition) override;
    int characterInserted(QTextDocument *document, int cursorPosition) override;

    void setAutoNumbering(bool b)
    {
        m_autoNumbering = b;
    }
    void setAutoFormatBulletList(bool b)
    {
        m_autoFormatBulletList = b;
    }
    void setTrimParagraphs(bool b)
    {
        m_trimParagraphs = b;
    }

    bool getTrimParagraphs() const
    {
        return m_trimParagraphs;
    }
    bool getAutoNumbering() const
    {
        return m_autoNumbering;
    }
    bool getAutoFormatBulletList() const
    {
        return m_autoFormatBulletList;
    }

private Q_SLOTS:
    void setEnabled(bool enable);
    void configureAutocorrect();

private:
    void readConfiguration() override;
    void writeConfiguration() override;

    void readConfigurationInternal();
    void writeConfigurationInternal();

    bool autoNumbering();
    bool autoFormatBulletList();
    bool autoTrimParagraph();

private:
    bool m_trimParagraphs; // remove spaces at beginning and end of paragraphs
    bool m_autoNumbering; // use autonumbering for numbered paragraphs
    bool m_autoFormatBulletList; // use list formatting for bulleted paragraphs.
};

#endif
