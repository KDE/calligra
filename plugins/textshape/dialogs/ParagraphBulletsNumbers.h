/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PARAGRAPHBULLETSNUMBERS_H
#define PARAGRAPHBULLETSNUMBERS_H

#include <ListItemsHelper.h>
#include <ui_ParagraphBulletsNumbers.h>

#include <KoListStyle.h>

#include <QWidget>

class KoParagraphStyle;
class KoCharacterStyle;
class KoImageCollection;
class KJob;
class KoImageData;

class ParagraphBulletsNumbers : public QWidget
{
    Q_OBJECT
public:
    explicit ParagraphBulletsNumbers(QWidget *parent);

    void setDisplay(KoParagraphStyle *style, int level, bool directFormattingMode);

    void save(KoParagraphStyle *style);

    int addStyle(const Lists::ListStyleItem &lsi);

    void setImageCollection(KoImageCollection *imageCollection);

Q_SIGNALS:
    void parStyleChanged();

public Q_SLOTS:
    void setFontSize(const KoCharacterStyle *style);

private Q_SLOTS:
    void styleChanged(int);
    void customCharButtonPressed();
    void recalcPreview();
    void labelFollowedByIndexChanged(int);
    void selectListImage();
    void setImageData(KJob *job);

private:
    Ui::ParagraphBulletsNumbers widget;

    QHash<int, KoListStyle::LabelType> m_mapping;
    int m_previousLevel;
    int m_blankCharIndex;
    bool m_alignmentMode;
    KoImageCollection *m_imageCollection;
    KoImageData *m_data;
    int m_fontSize;
};

#endif
