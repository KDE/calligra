/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
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

#ifndef MCTPROPERTYBASE_H
#define MCTPROPERTYBASE_H

//#include <QString>;
//#include <QMap>;
#include <QVariant>

// The properties are stored in a different way in calligra (not in string)
using PropertyDictionary = QVariantMap;

// TODO: we can not use UNO's Xinterface, the props need to loaded directly from Calligra
class MctPropertyBase
{
public:
    MctPropertyBase(QString name = "undefined");
    virtual ~MctPropertyBase();

    QString getPropertyName() const;
    void setPropertyName(QString name);
    PropertyDictionary * getProps() const;
    void setProps(PropertyDictionary* props);
    PropertyDictionary * getProps2Export() const;
    void setProps2Export(PropertyDictionary* props);
    QString getInnerUrl()const ;
    void setInnerUrl(QString url);

protected:
#if 0
    void fillUpProperties(Xinterface *interface);   // This fills up properties through the Xinterface
#endif
    virtual void fillUpProperties();

    QString propName;                      // The name of the frame object
    PropertyDictionary *props;         // Properties of the texttable object
    PropertyDictionary *props2export;  // Properties to be exported into the graph
    QString innerURL;           // inside URL of the backed up file
};

#endif // MCTPROPERTYBASE_H
