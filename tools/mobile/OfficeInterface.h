/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Lassi Nieminen <lassniem@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef OFFICEINTERFACE_H
#define OFFICEINTERFACE_H

#include <QtPlugin>
#include <QWidget>
#include <QStringList>

class OfficeInterface
{
public:

    virtual ~OfficeInterface() {}

    // This is used to give information about the currently active doc to the plugin
    // 0 if there is no document open
    virtual void setDocument(void *doc) = 0;

    // Asks the plugin to create it's view for the main program to use
    virtual QWidget *view() = 0;

    // Should return the name of the plugin (not translated)
    virtual QString pluginName() = 0;

    // Should return the translated title of the plugin
    virtual QString pluginTitle() = 0;

    // Should return list of document types in presence of which this plugin
    // should be shown in the menubar.
    // Possible values : All, presentation, document, spreadsheet
    //
    // NOTE: this is not actually used yet in the main program and may be extended
    // in the future.
    virtual QStringList pluginSupportTypes() = 0;
};

Q_DECLARE_INTERFACE(OfficeInterface, "com.office.CalligraMobile.OfficeInterface/1.0")

#endif
