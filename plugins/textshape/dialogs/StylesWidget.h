/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef STYLESWIDGET_H
#define STYLESWIDGET_H

#include <QFrame>
#include <QList>
#include <QTextBlockFormat>
#include <QTextCharFormat>

#include <ui_StylesWidget.h>

class KoStyleManager;
class KoStyleThumbnailer;
class KoParagraphStyle;
class KoCharacterStyle;
class StylesModel;
class StylesDelegate;

class StylesWidget : public QFrame
{
    Q_OBJECT
public:
    explicit StylesWidget(QWidget *parent = nullptr, bool paragraphMode = true, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~StylesWidget();

    virtual QSize sizeHint() const;

public Q_SLOTS:
    void setStyleManager(KoStyleManager *sm);
    void setCurrentFormat(const QTextBlockFormat &format);
    void setCurrentFormat(const QTextCharFormat &format);

Q_SIGNALS:
    void doneWithFocus();
    void paragraphStyleSelected(KoParagraphStyle *paragraphStyle, bool canDelete);
    void characterStyleSelected(KoCharacterStyle *characterStyle, bool canDelete);

private Q_SLOTS:
    void applyParagraphStyle();
    void applyCharacterStyle();

Q_SIGNALS:
    void paragraphStyleSelected(KoParagraphStyle *style);
    void characterStyleSelected(KoCharacterStyle *style);

private:
    Ui::StylesWidget widget;
    KoStyleManager *m_styleManager;
    KoStyleThumbnailer *m_styleThumbnailer;

    QTextBlockFormat m_currentBlockFormat;
    QTextCharFormat m_currentCharFormat;
    StylesModel *m_stylesModel;
    StylesDelegate *m_stylesDelegate;
    bool m_blockSignals;
    bool m_isEmbedded;
    bool m_isHovered;
};

#endif
