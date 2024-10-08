// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "Tokenizer.h"
#include "rtfdebug.h"
#include <ctype.h>

namespace RtfReader
{
void Tokenizer::pullControlWord(Token *token)
{
    char next;
    while (m_inputDevice->getChar(&next)) {
        if ((next == ' ') || (next == '\r') || (next == '\n')) {
            break;
        } else if (isalpha(next)) {
            token->name.append(next);
        } else if (isdigit(next) || (next == '-')) {
            token->parameter.append(next);
            token->hasParameter = true;
        } else {
            m_inputDevice->ungetChar(next);
            break;
        }
    }
}

void Tokenizer::pullControlSymbol(Token *token)
{
    if (token->name == "\'") {
        char highNibbleHexDigit;
        char lowNibbleHexDigit;
        if (m_inputDevice->getChar(&highNibbleHexDigit) && m_inputDevice->getChar(&lowNibbleHexDigit) && isxdigit(highNibbleHexDigit)
            && isxdigit(lowNibbleHexDigit)) {
            QString hexDigits;
            hexDigits.append(QChar(highNibbleHexDigit));
            hexDigits.append(QChar(lowNibbleHexDigit));
            uint codepoint = hexDigits.toUInt(nullptr, 16);
            token->type = Plain;
            token->name = QByteArray(1, codepoint);
        }
    } else if (token->name == "\\") {
        token->type = Plain;
        token->name = "\\";
    } else if (token->name == "{") {
        token->type = Plain;
        token->name = "{";
    } else if (token->name == "}") {
        token->type = Plain;
        token->name = "}";
    } else if (token->name == "*") {
        // don't need anything else here
    } else {
        qCDebug(lcRtf) << "unhandled control symbol in Tokenizer:" << token->name;
    }
}

void Tokenizer::pullControl(Token *token)
{
    char c;
    m_inputDevice->getChar(&c);
    token->name.append(c);
    if (isalpha(c)) {
        pullControlWord(token);
    } else {
        pullControlSymbol(token);
    }
}

void Tokenizer::pullPlainText(Token *token)
{
    char c;
    while (m_inputDevice->getChar(&c)) {
        switch (c) {
        case '{':
        case '}':
        case '\\':
            m_inputDevice->ungetChar(c);
            return;
            break;
        case '\r':
        case '\n':
            break;
        default:
            token->name.append(c);
        }
    }
}

Token Tokenizer::fetchToken()
{
    Token token;
    token.type = Invalid;
    token.hasParameter = false;
    char c;
    while (m_inputDevice->getChar(&c)) {
        switch (c) {
        case '{':
            token.type = OpenGroup;
            return token;
            break;
        case '}':
            token.type = CloseGroup;
            return token;
            break;
        case '\r':
            break;
        case '\n':
            break;
        case '\\':
            token.type = Control;
            pullControl(&token);
            return token;
            break;
        default:
            token.type = Plain;
            token.name.append(c);
            pullPlainText(&token);
            return token;
        }
    }
    return token;
}
}
