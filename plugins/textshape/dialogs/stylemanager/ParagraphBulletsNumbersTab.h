/* This file is part of the KDE project
 * Copyright (C) 2007, 2010 Thomas Zander <zander@kde.org>
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

#ifndef PARAGRAPHBULLETSNUMBERSTAB_H
#define PARAGRAPHBULLETSNUMBERSTAB_H

#include <ui_ParagraphBulletsNumbersTab.h>
#include <ListItemsHelper.h>

#include <KoListStyle.h>

#include <QWidget>

class KoParagraphStyle;
class KoCharacterStyle;
class KoImageCollection;
class KJob;
class KoImageData;

class ParagraphBulletsNumbersTab : public QWidget
{
    Q_OBJECT
public:
    ParagraphBulletsNumbersTab(QWidget *parent = 0);

    void setDisplay(KoParagraphStyle *style, int level = 0);

    void save(KoParagraphStyle *style);

    int addStyle(const Lists::ListStyleItem &lsi);

    void setImageCollection(KoImageCollection *imageCollection);

signals:
    void parStyleChanged();

public slots:
    void setFontSize(const KoCharacterStyle *style);

private slots:
    void styleChanged(int);
    void customCharButtonPressed();
    void recalcPreview();
    void labelFollowedByIndexChanged(int);
    void selectListImage();
    void setImageData(KJob *job);

private:
    Ui::ParagraphBulletsNumbersTab widget;

    QHash<int, KoListStyle::Style> m_mapping;
    int m_previousLevel;
    int m_blankCharIndex;
    bool m_alignmentMode;
    KoImageCollection *m_imageCollection;
    KoImageData *m_data;
    int m_fontSize;
};

#endif
