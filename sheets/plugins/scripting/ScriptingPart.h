// This file is part of KSpread
// SPDX-FileCopyrightText: 2005 Cyrille Berger <cberger@cberger.net>
// SPDX-FileCopyrightText: 2006 Isaac Clerencia <isaac@warp.es>
// SPDX-FileCopyrightText: 2006 Sebastian Sauer <mail@dipe.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef SCRIPTINGPART_H
#define SCRIPTINGPART_H

#include <KoScriptingPart.h>
#include <QVariantList>
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
    * \param parent The parent QObject. This will be the \a KSpread::View instance
    * this KParts plugin got loaded for. Once those view got destroyed the
    * plugin got unloaded. Since KSpread may use multiple views, it's quit
    * possible that multiple instances of this plugin exist the same time.
    * \param args The optional list of arguments.
    */
    ScriptingPart(QObject* parent, const QVariantList& args);

    /**
    * Destructor.
    */
    ~ScriptingPart() override;

private:
    Q_DISABLE_COPY(ScriptingPart)
};

#endif
