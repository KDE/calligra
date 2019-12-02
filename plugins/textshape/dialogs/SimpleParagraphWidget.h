/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2010 C. Boemann <cbo@boemann.dk>
 * Copyright (C) 2011 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 * Copyright (C) 2011-2012 Pierre Stirnweiss <pstirnweiss@googlemail.com>
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
#ifndef SIMPLEPARAGRAPHWIDGET_H
#define SIMPLEPARAGRAPHWIDGET_H

#include <ui_SimpleParagraphWidget.h>

#include <QWidget>
#include <QTextBlock>
#include <QPixmap>

#include <KoListLevelProperties.h>

class TextTool;
class KoStyleManager;
class KoParagraphStyle;
class KoStyleThumbnailer;

class StylesModel;
class DockerStylesComboModel;
class StylesDelegate;

namespace Lists {class ListStyleItem;};

class SimpleParagraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleParagraphWidget(TextTool *tool, QWidget *parent = 0);
    ~SimpleParagraphWidget() override;

    void setInitialUsedStyles(QVector<int> list);

public Q_SLOTS:
    void setCurrentBlock(const QTextBlock &block);
    void setCurrentFormat(const QTextBlockFormat& format);
    void setStyleManager(KoStyleManager *sm);
    void slotShowStyleManager(int index);
    void slotParagraphStyleApplied(const KoParagraphStyle *style);

Q_SIGNALS:
    void doneWithFocus();
    void paragraphStyleSelected(KoParagraphStyle *);
    void newStyleRequested(const QString &name);
    void showStyleManager(int styleId);

private Q_SLOTS:
    void defineLevelFormat();
    void listStyleChanged(int id);
    void deleteLevelFormat();
    void editLevelFormat();
    void styleSelected(int index);
    void styleSelected(const QModelIndex &index);

private:
    enum DirectionButtonState {
        LTR,
        RTL,
        Auto
    };

    void updateDirection(DirectionButtonState state);


    QPixmap generateListLevelPixmap(const KoListLevelProperties &llp);
    void fillListButtons();

    Ui::SimpleParagraphWidget widget;
    QList<KoListLevelProperties> m_levelLibrary;
    KoStyleManager *m_styleManager;
    bool m_blockSignals;
    QTextBlock m_currentBlock;
    QTextBlockFormat m_currentBlockFormat;
    TextTool *m_tool;
    DirectionButtonState m_directionButtonState;
    KoStyleThumbnailer *m_thumbnailer;
    QList<KoListLevelProperties> m_recentListFormats;
    ItemChooserAction *m_recentChooserAction;
    ItemChooserAction *m_libraryChooserAction;
    StylesModel *m_stylesModel;
    DockerStylesComboModel *m_sortedStylesModel;
    StylesDelegate *m_stylesDelegate;
};

#endif
