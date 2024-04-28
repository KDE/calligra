/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * SPDX-FileCopyrightText: 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KPRPRESENTATIONSTRATEGYBASE_H
#define KPRPRESENTATIONSTRATEGYBASE_H

class QWidget;
class QKeyEvent;
class KPrPresentationToolEventForwarder;
class KoPACanvasBase;
class KPrPresentationTool;

class KPrPresentationStrategyBase
{
public:
    explicit KPrPresentationStrategyBase(KPrPresentationTool *tool);
    virtual ~KPrPresentationStrategyBase();

    /**
     * If the event is handled or should be ignored by the tool return true. Otherwise
     * the event will be handled in the tool.
     */
    virtual bool keyPressEvent(QKeyEvent *event) = 0;

    KPrPresentationToolEventForwarder *widget();

protected:
    void setToolWidgetParent(QWidget *widget);
    void activateDefaultStrategy();
    KoPACanvasBase *canvas();

    KPrPresentationTool *m_tool;
    KPrPresentationToolEventForwarder *m_widget;
};

#endif /* KPRPRESENTATIONSTRATEGYBASE_H */
