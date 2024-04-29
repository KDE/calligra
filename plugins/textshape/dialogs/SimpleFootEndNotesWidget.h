/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SIMPLEFOOTENDNOTESWIDGET_H
#define SIMPLEFOOTENDNOTESWIDGET_H

#include <KoListStyle.h>
#include <ui_SimpleFootEndNotesWidget.h>

#include <KoInlineNote.h>
#include <QTextBlock>
#include <QWidget>

class TextTool;

class SimpleFootEndNotesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleFootEndNotesWidget(TextTool *tool, QWidget *parent = nullptr);
    Ui::SimpleFootEndNotesWidget widget;

Q_SIGNALS:
    void doneWithFocus();

private:
    QTextBlock m_currentBlock;
    TextTool *m_tool;
};

#endif
