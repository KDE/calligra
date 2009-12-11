/*
 * This file is part of KSpread
 *
 * Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 * Copyright (c) 2006 Isaac Clerencia <isaac@warp.es>
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
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

#ifndef SCRIPTINGPART_H
#define SCRIPTINGPART_H

#include <KoScriptingPart.h>

/**
* The ScriptingPart class implements a KPart component
* to integrate scripting into KSpread.
*/
class ScriptingPart : public KoScriptingPart
{
    Q_OBJECT
public:

    /**
    * Constructor.
    * \param The parent QObject. This will be the \a KSpread::View instance
    * this KParts plugin got loaded for. Once those view got destroyed the
    * plugin got unloaded. Since KSpread may use multiple views, it's quit
    * possible that multiple instances of this plugin exist the same time.
    * \param args The optional list of arguments.
    */
    ScriptingPart(QObject* parent, const QStringList& args);

    /**
    * Destructor.
    */
    virtual ~ScriptingPart();

private:
    Q_DISABLE_COPY(ScriptingPart)

    /// \internal d-pointer class.
    class Private;
    /// \internal d-pointer instance.
    Private* const d;
};

#endif
