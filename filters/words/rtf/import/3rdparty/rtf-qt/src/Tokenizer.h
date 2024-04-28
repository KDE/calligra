// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_TOKENIZER_H
#define RTFREADER_TOKENIZER_H

#include "Token.h"

#include "rtfreader_export.h"
#include <QFile>
namespace RtfReader
{
/**
  RTF tokenizer
  */
class RTFREADER_EXPORT Tokenizer
{
public:
    explicit Tokenizer(QFile *inputDevice)
        : m_inputDevice(inputDevice)
    {
    }
    Token fetchToken();

private:
    QFile *m_inputDevice;
    void pullControl(Token *token);
    void pullControlWord(Token *token);
    void pullControlSymbol(Token *token);
    void pullPlainText(Token *token);
};
}

#endif
