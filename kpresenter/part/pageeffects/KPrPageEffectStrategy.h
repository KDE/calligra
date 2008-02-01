/* This file is part of the KDE project
   Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPRPAGEEFFECTSTRATEGY_H
#define KPRPAGEEFFECTSTRATEGY_H

#include "KPrPageEffect.h"

class KPrPageEffectStrategy
{
public:
    KPrPageEffectStrategy( KPrPageEffect::SubType subType );
    virtual ~KPrPageEffectStrategy();

    KPrPageEffect::SubType subType() const;

    virtual void setup( const KPrPageEffect::Data &data, QTimeLine &timeLine ) = 0;

    virtual void paintStep( QPainter &p, int curPos, const KPrPageEffect::Data &data ) = 0;

    virtual void next( const KPrPageEffect::Data &data ) = 0;

    /**
     * The default implementation triggers an update of the whole widget. If you only need to 
     * update a smaller part of the widget reimplement this function.
     */
    virtual void finish( const KPrPageEffect::Data &data );

private:
    KPrPageEffect::SubType m_subType;
};

#endif /* KPRPAGEEFFECTSTRATEGY_H */
