/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SIMPLEFOOTENDNOTESWIDGET_H
#define SIMPLEFOOTENDNOTESWIDGET_H

#include <ui_SimpleFootEndNotesWidget.h>
#include <KoListStyle.h>

#include <QWidget>
#include <QTextBlock>
#include <KoInlineNote.h>

class TextTool;

class SimpleFootEndNotesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleFootEndNotesWidget(TextTool *tool,QWidget *parent = 0);
    Ui::SimpleFootEndNotesWidget widget;

Q_SIGNALS:
    void doneWithFocus();

private:
    QTextBlock m_currentBlock;
    TextTool *m_tool;
};

#endif
