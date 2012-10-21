/* This file is part of the KDE project
 * Copyright (C) 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef GENERALSTYLESTAB_H
#define GENERALSTYLESTAB_H

#include <QWidget>

class KoCharacterStyle;
class KoStyleManager;
class KoStyleThumbnailer;
class FontDecorations;
class CharacterHighlighting;
class LanguageTab;
class StylesModel;
class ValidParentStylesProxyModel;

namespace Ui {
class GeneralStylesTab;
}

class GeneralStylesTab : public QWidget
{
    Q_OBJECT
    
public:
    explicit GeneralStylesTab(QWidget *parent = 0);
    ~GeneralStylesTab();

    void setStylesModel(StylesModel *model);

    void setStyle(KoCharacterStyle *style);
//    void hideStyleName(bool hide);
    bool isStyleChanged();
//    QString styleName() const;
//    void selectName();
//    void setStyleManager(KoStyleManager *sm);


//    void updateNextStyleCombo(KoParagraphStyle *style); // set current style in next style combo
//    int nextStyleId(); //return the current style id in next style combo

public slots:
    void save(KoCharacterStyle *style = 0);

//    void switchToGeneralTab();

signals:
    void nameChanged(const QString &name);
    void styleAltered(const KoCharacterStyle *style); // when saving
    void styleChanged(); /// when user modifying

private slots:
//    void setPreviewCharacterStyle();

private:
    Ui::GeneralStylesTab *ui;

//    bool m_nameHidden;

//    FontDecorations *m_characterDecorations;
//    CharacterHighlighting *m_characterHighlighting;
//    LanguageTab *m_languageTab;

    KoCharacterStyle *m_style;
//    KoStyleManager *m_styleManager;
//    KoStyleThumbnailer *m_thumbnailer;
    StylesModel *m_StyleModel;
    ValidParentStylesProxyModel *m_validParentStylesModel;
//    StylesModel *m_characterInheritedStyleModel;
};

#endif // GENERALSTYLESTAB_H
