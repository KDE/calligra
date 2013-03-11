/* This file is part of the KDE project
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXI_ACTION_CATEGORIES_H
#define KEXI_ACTION_CATEGORIES_H

#include <ksharedptr.h>
#include "kexipart.h"

namespace Kexi
{

enum ActionCategory {
    NoActionCategory = 0,      //!< no category at all
    GlobalActionCategory = 1,  //!< global application action like editcopy;
    //!< can be applied to focused widget (of many types)
    PartItemActionCategory = 2,//!< action related to part item, e.g. data_execute;
    //!< requires context, used only in the navigator
    WindowActionCategory = 4   //!< action related to active window, which can display
    //!< table, query, form, report...
};

//! @short A set of functions used to declare action categories
/*! Note: we do not declare actions used in design/text view modes,
 because the categories are used in the data view,
 for now in the 'assign action to a push button' function. */
class KEXICORE_EXPORT ActionCategories : public KShared
{
public:
    ActionCategories();
    ~ActionCategories();

    /*! Declares action \a name for categories \a category (a combination of ActionCategory enum values).
     The categories is merged with the previous declaration (if any).
     \a supportedObjectTypes can be specified for ActionCategory::WindowAction to declare what object types
     the action allows, it is a combination of KexiPart::ObjectType enum values. */
    void addAction(const char* name, int categories,
                   KexiPart::ObjectType supportedObjectType1 = KexiPart::UnknownObjectType,
                   KexiPart::ObjectType supportedObjectType2 = KexiPart::UnknownObjectType,
                   KexiPart::ObjectType supportedObjectType3 = KexiPart::UnknownObjectType,
                   KexiPart::ObjectType supportedObjectType4 = KexiPart::UnknownObjectType,
                   KexiPart::ObjectType supportedObjectType5 = KexiPart::UnknownObjectType,
                   KexiPart::ObjectType supportedObjectType6 = KexiPart::UnknownObjectType,
                   KexiPart::ObjectType supportedObjectType7 = KexiPart::UnknownObjectType,
                   KexiPart::ObjectType supportedObjectType8 = KexiPart::UnknownObjectType);

    void addGlobalAction(const char* name) {
        addAction(name, Kexi::GlobalActionCategory);
    }

    //! Convenience function for adding action of category "part item", uses \ref addAction().
    void addPartItemAction(const char* name) {
        addAction(name, Kexi::PartItemActionCategory);
    }

    /*! Convenience function for adding action of category "window", uses \ref addAction().
     \a supportedObjectTypes is a combination of KexiPart::ObjectType enum values describing
     object types supported by the action. */
    void addWindowAction(const char* name,
                         KexiPart::ObjectType supportedObjectType1 = KexiPart::UnknownObjectType,
                         KexiPart::ObjectType supportedObjectType2 = KexiPart::UnknownObjectType,
                         KexiPart::ObjectType supportedObjectType3 = KexiPart::UnknownObjectType,
                         KexiPart::ObjectType supportedObjectType4 = KexiPart::UnknownObjectType,
                         KexiPart::ObjectType supportedObjectType5 = KexiPart::UnknownObjectType,
                         KexiPart::ObjectType supportedObjectType6 = KexiPart::UnknownObjectType,
                         KexiPart::ObjectType supportedObjectType7 = KexiPart::UnknownObjectType,
                         KexiPart::ObjectType supportedObjectType8 = KexiPart::UnknownObjectType)
    {
        addAction(name, Kexi::WindowActionCategory, supportedObjectType1, supportedObjectType2,
                  supportedObjectType3, supportedObjectType4, supportedObjectType5, supportedObjectType6,
                  supportedObjectType7, supportedObjectType8);
    }

    /*! If \a set is true, action with name \a name will support any possible object type
     that can be checked by actionSupportsObjectType().
     Makes sense for action of category Kexi::WindowActionCategory. */
    void setAllObjectTypesSupported(const char* name, bool set);

    //! \return categories for action \a name (a combination of ActionCategory enum values).
    //! If there is no such actions declared at all, -1 is returned.
    int actionCategories(const char* name) const;

    /*! \return true if action \a name supports \a objectType.
     Only works for actions of WindowAction category. */
    bool actionSupportsObjectType(const char* name, KexiPart::ObjectType objectType) const;
protected:
    class Private;
    Private * const d;
};

//! \return ActionCategories singleton object
KEXICORE_EXPORT ActionCategories *actionCategories();

}

#endif
