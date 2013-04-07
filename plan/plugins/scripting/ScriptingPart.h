/*
 * This file is part of KPlato
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 * Copyright (c) 2008 Dag Andersen <danders@get2net>
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

#ifndef KPlatoSCRIPTINGPART_H
#define KPlatoSCRIPTINGPART_H

#include <kpluginfactory.h>

#include <KoScriptingPart.h>

class KPlatoScriptingFactory : public KPluginFactory
{
public:
    explicit KPlatoScriptingFactory(const char *componentName = 0, const char *catalogName = 0, QObject *parent = 0);

     /// This function is called when the factory asked to create a KPlatoScriptingFactory object.
    virtual QObject *create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args, const QString &keyword);
};

/**
* The KPlatoScriptingPart class implements a KPart component
* to integrate scripting into KPlato.
*/
class KPlatoScriptingPart : public KoScriptingPart
{
        Q_OBJECT
    public:
        explicit KPlatoScriptingPart(QObject* parent, const QStringList& args=QStringList());
        virtual ~KPlatoScriptingPart();
    private:
        /// \internal d-pointer class.
        class Private;
        /// \internal d-pointer instance.
        Private* const d;
};

#endif
