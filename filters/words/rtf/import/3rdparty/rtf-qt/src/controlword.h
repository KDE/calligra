// SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "rtfreader_export.h"
#include <QString>
namespace RtfReader
{

/**
   Representation of an RTF Control Word
*/
class RTFREADER_EXPORT ControlWord
{
public:
    /**
       Constructor
    */
    explicit ControlWord(const QString &name);

    /**
       Test whether this control word is a supported destination
    */
    bool isSupportedDestination() const;

    /**
       Test whether a control word name is a destination (even if we don't support it)
    */
    static bool isDestination(const QString &controlword);

    /**
       Test whether this control word is one we recognise
    */
    bool isKnown() const;

private:
    QString m_name;
};

}
