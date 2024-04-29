/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001, 2002, 2003, 2006 Montel Laurent <lmontel@mandrakesoft.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __kolanguagetab_h__
#define __kolanguagetab_h__

#include <ui_LanguageTab.h>

class KoCharacterStyle;

class LanguageTab : public QWidget
{
    Q_OBJECT

public:
    explicit LanguageTab(/*KSpell2::Loader::Ptr loader = KSpell2::Loader::Ptr()*/ bool uniqueFormat,
                         QWidget *parent = nullptr,
                         Qt::WindowFlags fl = Qt::WindowFlags());
    ~LanguageTab() override;

    QString language() const;
    void setDisplay(KoCharacterStyle *style);
    void save(KoCharacterStyle *style) const;

Q_SIGNALS:
    void languageChanged();

private:
    Ui::LanguageTab widget;

    bool m_uniqueFormat;
};

#endif
