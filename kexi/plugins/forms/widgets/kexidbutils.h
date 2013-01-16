/* This file is part of the KDE project
   Copyright (C) 2006-2012 Jarosław Staniek <staniek@kde.org>

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

#ifndef KDBWIDGETS_UTILS_H
#define KDBWIDGETS_UTILS_H

#include <KMenu>
#include <core/kexidataiteminterface.h>

//! Form-related utilities
class KexiFormUtils
{
public:
    //! @return color blended from 33% of background color and 66% of base color
    static QColor lighterGrayBackgroundColor(const QPalette& palette);

    //! @return pre-loaded data-source-tag icon pixmap of appropriate size
    static QPixmap dataSourceTagIcon();

    //! @return pre-loaded data-source-tag icon pixmap of appropriate size (RTL version)
    static QPixmap dataSourceRTLTagIcon();
};

//! @short Used for extending editor widgets' context menu.
/*! @internal This is performed by adding a title and disabling editing
 actions when "read only" flag is true. */
class KexiDBWidgetContextMenuExtender : public QObject
{
    Q_OBJECT
public:
    KexiDBWidgetContextMenuExtender(QObject* parent, KexiDataItemInterface* iface);
    ~KexiDBWidgetContextMenuExtender();

    //! Perform context menu exec() for @a globalPos. KMenu is used
    void exec(QMenu *menu, const QPoint &globalPos);

    //! Creates title for context menu \a menu
    void createTitle(KMenu *menu);

    //! Enables or disables context menu actions for @a menu that can modify the value.
    void updatePopupMenuActions(QMenu *menu);

    /*! Updates title for context menu based on data item \a iface caption or name
     Used in createTitle(QMenu *menu) and KexiDBImageBox.
     \return true is the title has been added. */
    static bool updateContextMenuTitleForDataItem(QMenu *menu, KexiDataItemInterface* iface,
    const QString& icon = QString());

protected:

    class Private;
    Private * const d;

};

class KexiDBAutoField;

//! An interface allowing to define custom behaviour for subwidget of the KexiDBAutoField
class KexiSubwidgetInterface
{
public:
    KexiSubwidgetInterface();
    virtual ~KexiSubwidgetInterface();

    virtual bool appendStretchRequired(KexiDBAutoField* autoField) const {
        Q_UNUSED(autoField); return false;
    }
    virtual bool subwidgetStretchRequired(KexiDBAutoField* autoField) const {
        Q_UNUSED(autoField); return false;
    }
};



#endif
