/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001, 2002, 2003 Montel Laurent <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
   SPDX-FileCopyrightText: 2008 Pierre Stirnweiss <pstirnweiss@googlemail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FONTDIA_H
#define FONTDIA_H

#include <KoCharacterStyle.h>
#include <KoDialog.h>

class KoTextEditor;

class CharacterGeneral;

class FontDia : public KoDialog
{
    Q_OBJECT
public:
    explicit FontDia(KoTextEditor *cursor, QWidget *parent = nullptr);

protected Q_SLOTS:
    void styleChanged(bool state = true);

    void slotReset();
    void slotApply();
    void slotOk();

private:
    void initTabs();

    CharacterGeneral *m_characterGeneral;

    KoTextEditor *m_editor;
    QTextCharFormat m_initialFormat;

    bool m_uniqueFormat;
    bool m_styleChanged;
};

#endif
