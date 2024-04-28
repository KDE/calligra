// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_TOKEN_H
#define RTFREADER_TOKEN_H

#include "rtfreader_export.h"
#include <QByteArray>
#include <QString>
namespace RtfReader
{
enum TokenType { OpenGroup, CloseGroup, Control, Plain, Binary, Invalid };

/**
  RTF token
  */
class RTFREADER_EXPORT Token
{
public: /* TODO: convert to getters / setters */
    void dump() const;
    TokenType type;
    QByteArray name;
    bool hasParameter;
    QString parameter;
    QByteArray binaryData;
};
}

#endif
