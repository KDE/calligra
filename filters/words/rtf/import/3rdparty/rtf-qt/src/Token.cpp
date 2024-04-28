// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "Token.h"
#include "rtfdebug.h"

namespace RtfReader
{
void Token::dump() const
{
    switch (type) {
    case OpenGroup:
        qCDebug(lcRtf) << "token type: OpenGroup";
        break;
    case CloseGroup:
        qCDebug(lcRtf) << "token type: CloseGroup";
        break;
    case Control:
        qCDebug(lcRtf) << "token type: Control";
        break;
    case Plain:
        qCDebug(lcRtf) << "token type: Plain";
        break;
    case Binary:
        qCDebug(lcRtf) << "token type: Binary";
        break;
    default:
        qCDebug(lcRtf) << "unexpected token type: " << type;
    }
    if (type > CloseGroup) {
        qCDebug(lcRtf) << "name: " << name;
        if (hasParameter) {
            qCDebug(lcRtf) << "parameter: " << parameter;
        }
    }
}
}
