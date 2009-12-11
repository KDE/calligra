/* This file is part of the KDE project
   Copyright (C) 2001 Robert JACOLIN <rjacolin@ifrance.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef __LATEXFILTER_CONFIG_H__
#define __LATEXFILTER_CONFIG_H__


enum _ELatexType {
    TYPE_UNKNOWN,
    TYPE_DOC,
    TYPE_EMBEDED
};

enum _ELatexEncoding {
    ENC_UNKNOWN,
    ENC_LATIN1,
    ENC_UNICODE
};

typedef enum _ELatexType ELatexType;
typedef enum _ELatexEncoding ELatexEncoding;

class Config
{
public:
    Config();

    ~Config();

    ELatexType getType() const {
        return _type;
    }

    ELatexEncoding getEncoding() const {
        return _encoding;
    }

    void setType(ELatexType type) {
        _type = type;
    }

    void setEncoding(ELatexEncoding encoding) {
        _encoding = encoding;
    }

private:
    static ELatexType _type;
    static ELatexEncoding _encoding;

};

#endif /* __LATEXFILTER_CONFIG_H__ */
