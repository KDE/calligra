/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org>
                 2006 Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPAGEWIDGETITEM_H
#define KOPAGEWIDGETITEM_H

#include "kowidgets_export.h"

class QWidget;
class QString;

// This class can be implemented when we want to extend the
// dialog with new, specific pages.
class KOWIDGETS_EXPORT KoPageWidgetItem
{
public:
    virtual ~KoPageWidgetItem() = default;

    virtual QWidget *widget() = 0;
    virtual QString name() const = 0;
    virtual QString iconName() const = 0;
    virtual bool shouldDialogCloseBeVetoed() = 0;
    virtual void apply() = 0;
};

#endif // KOPAGEWIDGETITEM_H
