/* This file is part of the KDE project
   Copyright (C) 2005-2008 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "KexiCustomPropertyFactory.h"
#include "KexiCustomPropertyFactory_p.h"

#include <KPropertyFactory>
#include <KCustomProperty>

#include <KDb>

#if 0
//! @internal
class PixmapIdCustomProperty : public KCustomProperty
{
public:
    explicit PixmapIdCustomProperty(Property *parent)
            : KCustomProperty(parent) {
    }
    virtual ~PixmapIdCustomProperty() {}
    virtual void setValue(const QVariant &value, bool rememberOldValue) {
        Q_UNUSED(value);
        Q_UNUSED(rememberOldValue);
    }
    virtual QVariant value() const {
        return m_property->value();
    }
    virtual bool handleValue() const {
        return false;
    }
};

//! @internal
class IdentifierCustomProperty : public KCustomProperty
{
public:
    explicit IdentifierCustomProperty(Property *parent)
            : KCustomProperty(parent) {
    }
    virtual ~IdentifierCustomProperty() {}
    virtual void setValue(const QVariant &value, bool rememberOldValue) {
        Q_UNUSED(rememberOldValue);
        if (!value.toString().isEmpty())
            m_value = KexiUtils::stringToIdentifier(value.toString()).toLower();
    }
    virtual QVariant value() const {
        return m_value;
    }
    virtual bool handleValue() const {
        return true;
    }
    QString m_value;
};
#endif

//! @todo
#if 0 //TODO
class KexiImagePropertyEditorDelegate : public KPropertyEditorCreatorInterface,
                                        public KPropertyValuePainterInterface
{
public:
    KexiImagePropertyEditorDelegate() {}
    virtual QWidget * createEditor( int type, QWidget *parent, 
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    virtual void paint( QPainter * painter, 
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};
#endif

class KexiIdentifierPropertyEditorDelegate : public KPropertyEditorCreatorInterface
{
public:
    KexiIdentifierPropertyEditorDelegate() {}
    virtual QWidget * createEditor( int type, QWidget *parent, 
        const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        Q_UNUSED(type);
        Q_UNUSED(option);
        Q_UNUSED(index);
        return new KexiIdentifierPropertyEdit(parent);
    }
};

//---------------

KexiCustomPropertyFactory::KexiCustomPropertyFactory()
        : KPropertyFactory()
{
//! @todo addEditor( KexiCustomPropertyFactory::PixmapId, new KexiImagePropertyEditorDelegate );
    addEditor( KexiCustomPropertyFactory::Identifier, new KexiIdentifierPropertyEditorDelegate );
}

void KexiCustomPropertyFactory::init()
{
    if (KPropertyFactoryManager::self()->isEditorForTypeAvailable(KexiCustomPropertyFactory::PixmapId))
        return; //already registered
    KPropertyFactoryManager::self()->registerFactory( new KexiCustomPropertyFactory );
}
