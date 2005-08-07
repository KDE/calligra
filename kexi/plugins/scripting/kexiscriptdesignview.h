/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Sebastian Sauer <mail@dipe.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXISCRIPTDESIGNVIEW_H
#define KEXISCRIPTDESIGNVIEW_H

#include <kexiviewbase.h>

#include <koproperty/set.h>
#include <koproperty/property.h>

// Forward declarations.
class KexiScriptManager;
class KexiScriptContainer;
class KexiScriptEditor;
class KexiScriptDesignViewPrivate;

/**
 * The KexiScriptDesignView class provides the \a KexiViewBase to
 * manage script modules in the design-view. The design-view
 * is used to be able to view and edit the scripting code.
 */
class KexiScriptDesignView : public KexiViewBase
{
        Q_OBJECT

    public:

        /**
         * Constructor.
         */
        KexiScriptDesignView(KexiScriptManager* manager, KexiMainWindow *mainWin, QWidget *parent, const char *name = 0);

        /**
         * Destructor.
         */
        virtual ~KexiScriptDesignView();

        /**
         * \return a property set for this view.
         */
        virtual KoProperty::Set* propertySet();

        /**
         * Try to call \a storeData with new data we like to store. On
         * success the matching
         *
         * \param sdata The source \a KexiDB::SchemaData instance.
         * \param cancel Cancel on failure and don't try to clean
         *       possible temporary created data up.
         * \return The matching \a KexiDB::SchemaData instance or NULL
         *        if storing failed.
         */
        virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);

        /**
         * Try to store the modified data in the already opened and
         * currently used \a KexiDB::SchemaData instance.
         */
        virtual tristate storeData();

    private slots:

        /**
         * Handle changes in the property editor.
         */
        void slotPropertyChanged(KoProperty::Set& set, KoProperty::Property& property);

    private:
        KexiScriptDesignViewPrivate* d;

        /**
         * Load the data from XML source and fill the internally
         * used \a Kross::Api::ScriptContainer instance.
         */
        bool loadData();
};

#endif
