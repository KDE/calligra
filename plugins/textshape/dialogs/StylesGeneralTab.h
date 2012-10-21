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

#ifndef STYLESGENERALTAB_H
#define STYLESGENERALTAB_H

#include <QWidget>

class KoCharacterStyle;
class KoStyleManager;
class KoStyleThumbnailer;
class FontDecorations;
class CharacterHighlighting;
class LanguageTab;
class AbstractStylesModel;
class StylesModel;
class ValidParentStylesProxyModel;

namespace Ui {
class StylesGeneralTab;
}

class StylesGeneralTab : public QWidget
{
    Q_OBJECT

public:
    explicit StylesGeneralTab(QWidget *parent = 0);
    ~StylesGeneralTab();

    void setStylesModel(AbstractStylesModel *model);
    void setStyleManager(KoStyleManager *sm);

    void setCurrentStyle(KoCharacterStyle *style);

public slots:
    void save(KoCharacterStyle *style = 0);

signals:
    void nameChanged(const QString &name);
    void styleChanged(); /// when user modifying

private slots:
    void slotClearNextStyle();
    void slotClearInheritStyle();

private:
    Ui::StylesGeneralTab *ui;

    enum Type {
        CharacterStyle,
        ParagraphStyle
    };

    Type m_styleType;
    KoCharacterStyle *m_style;
    KoStyleManager *m_styleManager;
    AbstractStylesModel *m_styleModel;
    ValidParentStylesProxyModel *m_validParentStylesModel;
};

#endif // STYLESGENERALTAB_H
