/* This file is part of the KDE project
 * Copyright (C) 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * Copyright (C) 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
 * Copyright (C) 2009 Thorsten Zachmann <zachmann@kde.org>
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
#ifndef KPRPRESENTATIONSTRATEGYBASE_H
#define KPRPRESENTATIONSTRATEGYBASE_H

class QWidget;
class QKeyEvent;
class KPrPresentationToolEventForwarder;
class KoPACanvas;
class KPrPresentationTool;

class KPrPresentationStrategyBase
{
public:
    KPrPresentationStrategyBase( KPrPresentationTool * tool );
    virtual ~KPrPresentationStrategyBase();

    /**
     * If the event is handled or should be ignored by the tool return true. Otherwise
     * the event will be handled in the tool.
     */
    virtual bool keyPressEvent( QKeyEvent * event ) = 0;

    KPrPresentationToolEventForwarder * widget();

protected:
    void setToolWidgetParent( QWidget * widget );
    void activateDefaultStrategy();
    KoPACanvas * canvas();

    KPrPresentationTool *m_tool;
    KPrPresentationToolEventForwarder *m_widget;

};

#endif /* KPRPRESENTATIONSTRATEGYBASE_H */

