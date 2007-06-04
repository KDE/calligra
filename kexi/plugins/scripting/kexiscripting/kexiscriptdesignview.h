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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXISCRIPTDESIGNVIEW_H
#define KEXISCRIPTDESIGNVIEW_H

#include <KexiView.h>

#include <koproperty/set.h>
#include <koproperty/property.h>

// Forward declarations.
class KexiScriptContainer;
class KexiScriptEditor;
class KexiScriptDesignViewPrivate;

namespace Kross { namespace Api {
  class ScriptAction;
}}

/**
 * The KexiScriptDesignView class provides the KexiView to
 * manage script modules in the design-view. The design-view
 * is used to be able to view and edit the scripting code via
 * a \a KexiScriptEditor instance.
 */
class KexiScriptDesignView : public KexiView
{
        Q_OBJECT

    public:

        /**
         * Constructor.
         */
        KexiScriptDesignView(QWidget *parent, Kross::Api::ScriptAction* scriptaction);

        /**
         * Destructor.
         */
        virtual ~KexiScriptDesignView();

        /**
         * \return the \a Kross::Api::ScriptAction this \a KexiScriptDesignView
         * is responsible for.
         */
        Kross::Api::ScriptAction* scriptAction() const;

        /**
         * \return a property set for this view.
         */
        virtual KoProperty::Set* propertySet();

        /**
         * Try to call \a storeData with new data we like to store. On
         * success the matching \a KexiDB::SchemaData is returned.
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
        virtual tristate storeData(bool dontAsk = false);

    private slots:

        /**
         * Deferred initialization.
         */
        void initialize();

        /**
         * Handle changes in the property editor.
         */
        void slotPropertyChanged(KoProperty::Set& set, KoProperty::Property& property);

        /**
         * Update the \a KoProperty::Property::Dict propertymap of the
         * interpreter-dependent options.
         */
        void updateProperties();

        /**
         * Execute the scripting code.
         */
        void execute();

    private:
        KexiScriptDesignViewPrivate* d;

        /**
         * Load the data from XML source and fill the internally
         * used \a Kross::Api::ScriptContainer instance.
         */
        bool loadData();
};

#endif
