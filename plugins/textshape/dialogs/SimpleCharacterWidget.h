/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2011-2012 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SIMPLECHARACTERWIDGET_H
#define SIMPLECHARACTERWIDGET_H

#include <KoListStyle.h>
#include <ui_SimpleCharacterWidget.h>

#include <QTextBlock>
#include <QWidget>

class TextTool;
class KoStyleManager;
class KoCharacterStyle;
class KoStyleThumbnailer;
class DockerStylesComboModel;
class StylesDelegate;
class StylesModel;

class SimpleCharacterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleCharacterWidget(TextTool *tool, QWidget *parent = nullptr);
    ~SimpleCharacterWidget() override;

    void setInitialUsedStyles(QVector<int> list);

public Q_SLOTS:
    void setStyleManager(KoStyleManager *sm);
    void setCurrentFormat(const QTextCharFormat &format, const QTextCharFormat &refBlockCharFormat);
    void setCurrentBlockFormat(const QTextBlockFormat &format);
    void slotCharacterStyleApplied(const KoCharacterStyle *style);

private Q_SLOTS:
    void fontFamilyActivated(int index);
    void fontSizeActivated(int index);
    void styleSelected(int index);
    void styleSelected(const QModelIndex &index);
    void slotShowStyleManager(int index);

Q_SIGNALS:
    void doneWithFocus();
    void characterStyleSelected(KoCharacterStyle *);
    void newStyleRequested(const QString &name);
    void showStyleManager(int styleId);

private:
    void clearUnsetProperties(QTextFormat &format);

    Ui::SimpleCharacterWidget widget;
    KoStyleManager *m_styleManager;
    bool m_blockSignals;
    bool m_comboboxHasBidiItems;
    int m_lastFontFamilyIndex;
    int m_lastFontSizeIndex;
    TextTool *m_tool;
    QTextCharFormat m_currentCharFormat;
    QTextBlockFormat m_currentBlockFormat;
    KoStyleThumbnailer *m_thumbnailer;
    StylesModel *m_stylesModel;
    DockerStylesComboModel *m_sortedStylesModel;
    StylesDelegate *m_stylesDelegate;
};

#endif
