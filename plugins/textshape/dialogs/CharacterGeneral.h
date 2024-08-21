/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHARACTERGENERAL_H
#define CHARACTERGENERAL_H

#include <ui_CharacterGeneral.h>

#include <QWidget>

class KoCharacterStyle;
class KoStyleManager;
class KoStyleThumbnailer;
class FontDecorations;
class CharacterHighlighting;
class LanguageTab;
class StylesModel;

class CharacterGeneral : public QWidget
{
    Q_OBJECT
public:
    explicit CharacterGeneral(QWidget *parent = nullptr);

    void setStyle(KoCharacterStyle *style, bool directFormattingMode);
    void hideStyleName(bool hide);
    bool isStyleChanged();
    QString styleName() const;
    void selectName();
    void setStyleManager(KoStyleManager *sm); // set style manager for m_paragraph style model
    void updateNextStyleCombo(KoParagraphStyle *style); // set current style in next style combo
    int nextStyleId(); // return the current style id in next style combo

    KoCharacterStyle *style() const;

public Q_SLOTS:
    void save(KoCharacterStyle *style = nullptr);

    void switchToGeneralTab();

Q_SIGNALS:
    void nameChanged(const QString &name);
    void styleAltered(const KoCharacterStyle *style); // when saving
    void styleChanged(); /// when user modifying

private Q_SLOTS:
    void setPreviewCharacterStyle();

protected:
    Ui::CharacterGeneral widget;

private:
    bool m_nameHidden;

    FontDecorations *m_characterDecorations;
    CharacterHighlighting *m_characterHighlighting;
    LanguageTab *m_languageTab;

    KoCharacterStyle *m_style;
    KoStyleManager *m_styleManager;
    KoStyleThumbnailer *m_thumbnail;
    StylesModel *m_paragraphStyleModel;
    StylesModel *m_characterInheritedStyleModel;
};

#endif
