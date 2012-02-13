/* This file is part of the KDE project
 * Copyright (C) 2011 Yue Liu <yue.liu@mail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#ifndef OUTPUTHANDLER_H
#define OUTPUTHANDLER_H

#include "DocumentElement.hxx"
#include "OdfDocumentHandler.hxx"
#include <QByteArray>

class OutputHandler : public OdfDocumentHandler
{
public:
    OutputHandler();
    virtual void startDocument() {}
    virtual void endDocument();
    virtual void startElement(const char *psName, const WPXPropertyList &xPropList);
    virtual void endElement(const char *psName);
    virtual void characters(const WPXString &sCharacters);
    QByteArray array() const;

private:
    QByteArray *m_array;
    bool mbIsTagOpened;
    WPXString msOpenedTagName;
};
#endif //OUTPUTHANDLER_H
