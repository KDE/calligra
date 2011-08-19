/*
 * This file is part of KPlato
 *
 * Copyright (c) 2007 Sebastian Sauer <mail@dipe.org>
 * Copyright (c) 2008 Dag Andersen <danders@get2net.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTINGWIDGETS_H
#define SCRIPTINGWIDGETS_H

#include <QVariant>
#include <QWidget>

#include <kactionselector.h>

#include "ui_ScriptingDataQueryView.h"

class QTreeView;
class QStandardItemModel;
class QListWidget;

namespace Scripting {
    class Module;
}

/**
 * The ScriptingSheetsListView provides a listview-widget that displays
 * all sheets and lets the user choose 0..n of them plus specify
 * cell-ranges for all of them.
 */
class ScriptingScheduleListView : public QWidget
{
        Q_OBJECT
    public:

        /**
        * Constructor.
        * \param module The \a ScriptingModule instance that should be used
        * as datasource for what should be displayed within the listview.
        * \param parent The parent QWidget this widget should be displayed in.
        */
        ScriptingScheduleListView(Scripting::Module* module, QWidget* parent);

        /**
        * Destructor.
        */
        virtual ~ScriptingScheduleListView();

    public slots:
        /// Return the selected schedule
        QVariant currentSchedule() const;


    private:
        Scripting::Module* m_module;
        QTreeView* m_view;
};

/**
 * The ScriptingNodePropertyListView provides a selection listview that displays the nodes
 * accessible properties and lets the user select a number of them.
 */
class ScriptingNodePropertyListView : public KActionSelector
{
    Q_OBJECT
    public:

        /**
        * Constructor.
        * \param module The \a ScriptingModule instance that should be used
        * as datasource for what should be displayed within the listview.
        * \param parent The parent QWidget this widget should be displayed in.
         */
        ScriptingNodePropertyListView(Scripting::Module* module, QWidget* parent);

        /**
         * Destructor.
         */
        virtual ~ScriptingNodePropertyListView();

    public slots:
        /// Return the selected node properties
        QVariant selectedProperties() const;


    private:
        Scripting::Module *m_module;
};

/**
 * The ScriptingDataQueryView provides a selection listview that displays
 */
class ScriptingDataQueryView : public QWidget, Ui::ScriptingDataQueryView
{
    Q_OBJECT
    public:

        /**
     * Constructor.
     * \param module The \a ScriptingModule instance that should be used
     * as datasource for what should be displayed within the listview.
     * \param parent The parent QWidget this widget should be displayed in.
         */
        ScriptingDataQueryView(Scripting::Module* module, QWidget* parent);

        /**
         * Destructor.
         */
        virtual ~ScriptingDataQueryView();

    public slots:
        /// If true, include header texts
        QVariant includeHeaders() const;
        /// The type of object (Node, Resource, Account)
        QVariant objectType() const;
        /// Return the selected properties
        QVariant selectedProperties() const;


    private slots:
        void slotObjectTypeChanged( int index );
        
    private:
        void setup();
        void setupLists( QListWidget *list, const QString &tag, const QString &property, const QString &tooltip );
        
    private:
        Scripting::Module *m_module;
};

#endif
