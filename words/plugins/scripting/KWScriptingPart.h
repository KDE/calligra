// This file is part of Words
// SPDX-FileCopyrightText: 2006 Sebastian Sauer <mail@dipe.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef KWSCRIPTINGPART_H
#define KWSCRIPTINGPART_H

#include <KoScriptingPart.h>

/**
* The KWScriptingPart class implements a KPart component
* to integrate scripting into Words.
* \internal
*/
class KWScriptingPart : public KoScriptingPart
{
    Q_OBJECT
public:
    KWScriptingPart(QObject *parent, const QVariantList&);
    virtual ~KWScriptingPart();
};

#endif
