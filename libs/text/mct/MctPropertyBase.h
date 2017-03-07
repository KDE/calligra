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

#include <QVariant>

// The properties are stored in a different way in calligra (not in string)
using PropertyDictionary = QVariantMap;

/**
 * Legacy base class for all kind of properties
 *
 * The legacy version of the MCT plugin was developed for UNO's Xinterface which handles
 * properties differently than Calligra does.
 *
 * @todo we can not use UNO's Xinterface, the props need to loaded directly from Calligra
 * @todo refactoring is highly suggested here
 */
class MctPropertyBase
{
public:
    /// constructor
    MctPropertyBase(const QString &name = "undefined");
    virtual ~MctPropertyBase();

    /// getter
    QString propertyName() const;
    /// setter
    void setPropertyName(const QString &name);
    /// getter
    PropertyDictionary * props() const;
    /// setter
    void setProps(PropertyDictionary* props);
    /// getter
    PropertyDictionary * props2Export() const;
    /// setter
    void setProps2Export(PropertyDictionary* props);
    /// getter
    QString innerUrl()const ;
    /// setter
    void setInnerUrl(const QString &url);

protected:
#if 0
    void fillUpProperties(Xinterface *interface);   // This fills up properties through the Xinterface
#endif
    /**
     * load properties into formatting properties dictionary
     */
    virtual void fillUpProperties();

    QString m_propName;                 ///<  name of the frame object
    PropertyDictionary *m_props;        ///< properties of the texttable object
    PropertyDictionary *m_props2export; ///< properties to be exported into the graph
    QString m_innerURL;                 ///< inside URL of the backed up file
};

#endif // MCTPROPERTYBASE_H
