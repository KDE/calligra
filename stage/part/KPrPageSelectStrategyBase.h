/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPAGESELECTSTRATEGYBASE_H
#define KPRPAGESELECTSTRATEGYBASE_H

class KoPAPageBase;

/**
 * Class for selecting the page used for painting the shapes
 */
class KPrPageSelectStrategyBase
{
public:
    KPrPageSelectStrategyBase();
    virtual ~KPrPageSelectStrategyBase();

    virtual const KoPAPageBase *page() const = 0;
};

#endif /* KPRPAGESELECTSTRATEGY_H */
