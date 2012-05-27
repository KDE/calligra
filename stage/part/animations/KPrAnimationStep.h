/* This file is part of the KDE project
 * Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#ifndef KPRANIMATIONSTEP_H
#define KPRANIMATIONSTEP_H

#include <QSequentialAnimationGroup>
#include "KPrAnimationData.h"

class KPrAnimationStep : public QSequentialAnimationGroup, KPrAnimationData
{
public:
    enum Node_Type {
        On_Click,
        After_Previous,
        With_Previous
    };

    enum Preset_Class {
        Entrance,
        Exit,
        Emphasis,
        Custom,
        Motion_Path,
        Ole_Action,
        Media_Call
    };
    KPrAnimationStep();
    virtual ~KPrAnimationStep();
    virtual void init(KPrAnimationCache *animationCache, int step);
    virtual bool saveOdf(KoPASavingContext & paContext) const;
    virtual void deactivate();
    void setNodeType(Node_Type type);
    void setPresetClass(Preset_Class presetClass);
    void setId(QString id);
    Node_Type NodeType() const;
    Preset_Class presetClass() const;
    QString id() const;
    QString presetClassText() const;

private:
    Node_Type m_triggerEvent;
    Preset_Class m_class;
    QString m_id;
};

#endif /* KPRANIMATIONSTEP_H */
