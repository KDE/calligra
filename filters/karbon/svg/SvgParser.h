/* This file is part of the KDE project
    * Copyright (C) 2011 Aakriti Gupta <aakriti.a.gupta@gmail.com>
    *
    * This library is free software; you can redistribute it and/or
    * modify it under the terms of the GNU Library General Public
    * License as published by the Free Software Foundation; either
    * version 2 of the License, or (at your option) any later version.
    *
    * This library is distributed in the hope that it will be useful,
    * but WITHOUT ANY WARRANTY; without even the implied warranty of
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    * Library General Public License for more details.
    *
    * You should have received a copy of the GNU Library General Public License
    * along with this library; see the file COPYING.LIB.  If not, write to
    * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    * Boston, MA 02110-1301, USA.
    */

#ifndef SVG_PARSER_H
#define SVG_PARSER_H

#include "SvgParser_generic.h"

class SvgParser;
class KoResourceManager;

/**
 * This class inherits SvgParser_generic and parses an SVG document.
 * The functions for parsing app specific data, which are virtual functions from SvgParser_generic
 * have no use here, since Karbon does not save any data with an app specific tag name.
 * However, it can be added later.
 */
class SvgParser : public SvgParser_generic
{
public:
    /**
     * @brief SvgParser(KoResourceManager *documentResourceManager) - Constructor
     */
    SvgParser(KoResourceManager *documentResourceManager);
    /**
     * @brief ~SvgParser() - Destructor
     */
    virtual ~SvgParser();

    /**
     *Virtual functions from SvgParser_generic
     */
    void parseAppData(const KoXmlElement& e);
    void setAppDataTag();
    void createAppData();
    
};

#endif /*SVG_PARSER_H*/