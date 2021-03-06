/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SIMPLECAPTIONSWIDGET_H
#define SIMPLECAPTIONSWIDGET_H

#include <ui_SimpleCaptionsWidget.h>
#include <KoListStyle.h>

#include <QWidget>
#include <QTextBlock>

class TextTool;
class KoStyleManager;

class SimpleCaptionsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleCaptionsWidget(QWidget *parent = 0);

public Q_SLOTS:
    void setStyleManager(KoStyleManager *sm);

Q_SIGNALS:
    void doneWithFocus();
    
private:
    Ui::SimpleCaptionsWidget widget;
    KoStyleManager *m_styleManager;
    bool m_blockSignals;
    bool m_comboboxHasBidiItems;
    QTextBlock m_currentBlock;
    TextTool *m_tool;
};

#endif
