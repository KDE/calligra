/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2011 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 * SPDX-FileCopyrightText: 2011-2012 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SIMPLEPARAGRAPHWIDGET_H
#define SIMPLEPARAGRAPHWIDGET_H

#include <ui_SimpleParagraphWidget.h>

#include <QPixmap>
#include <QTextBlock>
#include <QWidget>

#include <KoListLevelProperties.h>

class TextTool;
class KoStyleManager;
class KoParagraphStyle;
class KoStyleThumbnailer;

class StylesModel;
class DockerStylesComboModel;
class StylesDelegate;

namespace Lists
{
struct ListStyleItem;
};

class SimpleParagraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleParagraphWidget(TextTool *tool, QWidget *parent = nullptr);
    ~SimpleParagraphWidget() override;

    void setInitialUsedStyles(QVector<int> list);

public Q_SLOTS:
    void setCurrentBlock(const QTextBlock &block);
    void setCurrentFormat(const QTextBlockFormat &format);
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
    enum DirectionButtonState { LTR, RTL, Auto };

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
