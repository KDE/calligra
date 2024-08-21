/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PARAGRAPHGENERAL_H
#define PARAGRAPHGENERAL_H

#include "CharacterGeneral.h"

#include <QList>

class KoParagraphStyle;
class KoStyleThumbnailer;
class KoStyleManager;
class KoImageCollection;
class KoUnit;
class ParagraphBulletsNumbers;
class ParagraphIndentSpacing;
class ParagraphLayout;
class ParagraphDecorations;
class ParagraphDropCaps;
class StylesModel;

class ParagraphGeneral : public CharacterGeneral
{
    Q_OBJECT
public:
    explicit ParagraphGeneral(QWidget *parent = nullptr);

    void setStyle(KoParagraphStyle *style, int level, bool directFormattingMode);
    void setUnit(const KoUnit &unit);

    void switchToGeneralTab();
    void hideStyleName(bool hide);
    bool isStyleChanged();
    QString styleName() const;
    void selectName();

    void setImageCollection(KoImageCollection *imageCollection);
    KoImageCollection *imageCollection();
    void setStyleManager(KoStyleManager *sm);

    KoParagraphStyle *style() const;

public Q_SLOTS:
    void save(KoParagraphStyle *style = nullptr);

Q_SIGNALS:
    void nameChanged(const QString &name);
    void styleAltered(const KoParagraphStyle *style); /// when saving

private Q_SLOTS:
    void setPreviewParagraphStyle();

private:
    bool m_nameHidden;

    ParagraphIndentSpacing *m_paragraphIndentSpacing;
    ParagraphLayout *m_paragraphLayout;
    ParagraphBulletsNumbers *m_paragraphBulletsNumbers;
    ParagraphDecorations *m_paragraphDecorations;
    ParagraphDropCaps *m_paragraphDropCaps;

    KoParagraphStyle *m_style;
    QList<KoParagraphStyle *> m_paragraphStyles;
    KoStyleManager *m_styleManager;

    KoStyleThumbnailer *m_thumbnail;
    StylesModel *m_paragraphInheritedStyleModel;
};

#endif
