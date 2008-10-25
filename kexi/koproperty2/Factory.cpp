/* This file is part of the KDE project
   Copyright (C) 2008 Jarosław Staniek <staniek@kde.org>

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

#include "Factory.h"
#include "DefaultFactory.h"
#include "Property.h"
#include "customproperty.h"
/*
#include "booledit.h"
#include "combobox.h"
#include "coloredit.h"
#include "cursoredit.h"
#include "dateedit.h"
#include "datetimeedit.h"
#include "dummywidget.h"
#include "fontedit.h"
//TODO #include "linestyleedit.h"
#include "pixmapedit.h"
#include "pointedit.h"
#include "rectedit.h"
#include "sizeedit.h"
#include "sizepolicyedit.h"
#include "spinbox.h"
#include "stringlistedit.h"
#include "stringedit.h"
#include "symbolcombo.h"
#include "timeedit.h"
#include "urledit.h"
*/
#include <kdebug.h>
#include <kglobal.h>

namespace KoProperty
{

Label::Label(QWidget *parent, const KoProperty::ValueDisplayInterface *iface)
    : QLabel(parent)
    , m_iface(iface)
{
  setAutoFillBackground(true);
  setContentsMargins(0,0,0,0);
  setIndent(0);
}

QVariant Label::value() const
{
    return m_value;
}

void Label::setValue(const QVariant& value)
{
    setText( m_iface->displayText(value) );
    m_value = value;
}

//---------------

//! @internal
class FactoryManager::Private
{
public:
    Private()
    {
    }
    ~Private()
    {
        qDeleteAll(factories);
    }

    QSet<Factory*> factories;
    QHash<int, EditorCreatorInterface*> editorCreators;
    QHash<int, ValuePainterInterface*> valuePainters;
    QHash<int, ValueDisplayInterface*> valueDisplays;
//    QHash<int, Factory*> factoryForType;
//    QHash<int, CustomPropertyFactory*> registeredCustomProperties;
};

//! @internal
class Factory::Private
{
public:
    Private()
    {
    }
    ~Private()
    {
        qDeleteAll(editorCreatorsSet);
        qDeleteAll(valuePaintersSet);
        qDeleteAll(valueDisplaysSet);
    }

    QHash<int, EditorCreatorInterface*> editorCreators;
    QHash<int, ValuePainterInterface*> valuePainters;
    QHash<int, ValueDisplayInterface*> valueDisplays;
    QSet<EditorCreatorInterface*> editorCreatorsSet;
    QSet<ValuePainterInterface*> valuePaintersSet;
    QSet<ValueDisplayInterface*> valueDisplaysSet;
};

}

using namespace KoProperty;

Factory::Factory()
    : d( new Private )
{
}

Factory::~Factory()
{
    delete d;
}

QHash<int, EditorCreatorInterface*> Factory::editorCreators() const
{
    return d->editorCreators;
}

QHash<int, ValuePainterInterface*> Factory::valuePainters() const
{
    return d->valuePainters;
}

QHash<int, ValueDisplayInterface*> Factory::valueDisplays() const
{
    return d->valueDisplays;
}

void Factory::addEditor(int type, EditorCreatorInterface *creator)
{
    addEditorInternal( type, creator, true );
    if (dynamic_cast<ValuePainterInterface*>(creator))
        addPainterInternal( type, dynamic_cast<ValuePainterInterface*>(creator), false/* !own*/ );
    if (dynamic_cast<ValueDisplayInterface*>(creator))
        addDisplayInternal( type, dynamic_cast<ValueDisplayInterface*>(creator), false/* !own*/ );
}

void Factory::addPainter(int type, ValuePainterInterface *painter)
{
    addPainterInternal(type, painter, true);
    if (dynamic_cast<EditorCreatorInterface*>(painter))
        addEditorInternal( type, dynamic_cast<EditorCreatorInterface*>(painter), false/* !own*/ );
    if (dynamic_cast<ValueDisplayInterface*>(painter))
        addDisplayInternal( type, dynamic_cast<ValueDisplayInterface*>(painter), false/* !own*/ );
}

void Factory::addDisplay(int type, ValueDisplayInterface *display)
{
    addDisplayInternal(type, display, true);
    if (dynamic_cast<EditorCreatorInterface*>(display))
        addEditorInternal( type, dynamic_cast<EditorCreatorInterface*>(display), false/* !own*/ );
    if (dynamic_cast<ValueDisplayInterface*>(display))
        addDisplayInternal( type, dynamic_cast<ValueDisplayInterface*>(display), false/* !own*/ );
}

void Factory::addEditorInternal(int type, EditorCreatorInterface *editor, bool own)
{
    if (own)
        d->editorCreatorsSet.insert(editor);
    d->editorCreators.insert(type, editor);
}

void Factory::addPainterInternal(int type, ValuePainterInterface *painter, bool own)
{
    if (own)
        d->valuePaintersSet.insert(painter);
    d->valuePainters.insert(type, painter);
}

void Factory::addDisplayInternal(int type, ValueDisplayInterface *display, bool own)
{
    if (own)
        d->valueDisplaysSet.insert(display);
    d->valueDisplays.insert(type, display);
}

CustomProperty* FactoryManager::createCustomProperty(Property *parent)
{
    const int type = parent->type();
/* TODO
    CustomPropertyFactory *factory = d->registeredWidgets[type];
    if (factory)
        return factory->createCustomProperty(parent);
*/
    switch (type) {
    case Size: case Size_Width: case Size_Height:
        return new SizeCustomProperty(parent);
    case Point: case Point_X: case Point_Y:
        return new PointCustomProperty(parent);
    case Rect: case Rect_X: case Rect_Y: case Rect_Width: case Rect_Height:
        return new RectCustomProperty(parent);
    case SizePolicy: case SizePolicy_HorStretch: case SizePolicy_VerStretch:
    case SizePolicy_HorData: case SizePolicy_VerData:
        return new SizePolicyCustomProperty(parent);
    default:;
    }
    return 0;
}

//------------

FactoryManager::FactoryManager()
        : QObject(0)
        , d(new Private)
{
    setObjectName("KoProperty::FactoryManager");
    registerFactory(new DefaultFactory);
}

FactoryManager::~FactoryManager()
{
    delete d;
}

FactoryManager* FactoryManager::self()
{
    K_GLOBAL_STATIC(KoProperty::FactoryManager, _self);
    return _self;
}

void FactoryManager::registerFactory(Factory *factory)
{
    d->factories.insert(factory);
    QHash<int, EditorCreatorInterface*>::ConstIterator editorCreatorsItEnd = factory->editorCreators().constEnd();
    for (QHash<int, EditorCreatorInterface*>::ConstIterator it( factory->editorCreators().constBegin() );
        it != editorCreatorsItEnd; ++it)
    {
        d->editorCreators.insert(it.key(), it.value());
    }
    QHash<int, ValuePainterInterface*>::ConstIterator valuePaintersItEnd = factory->valuePainters().constEnd();
    for (QHash<int, ValuePainterInterface*>::ConstIterator it( factory->valuePainters().constBegin() );
        it != valuePaintersItEnd; ++it)
    {
        d->valuePainters.insert(it.key(), it.value());
    }
    QHash<int, ValueDisplayInterface*>::ConstIterator valueDisplaysItEnd = factory->valueDisplays().constEnd();
    for (QHash<int, ValueDisplayInterface*>::ConstIterator it( factory->valueDisplays().constBegin() );
        it != valueDisplaysItEnd; ++it)
    {
        d->valueDisplays.insert(it.key(), it.value());
    }
}

QWidget * FactoryManager::createEditor( 
    int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const EditorCreatorInterface *creator = d->editorCreators.value(type);
    if (!creator)
        return 0;
    QWidget *w = creator->createEditor(type, parent, option, index);
    if (w) {
        if (creator->options.removeBorders) {
//! @todo get real border color from the palette
            w->setStyleSheet("border-top: 1px solid #c0c0c0;");
        }
    }
    return w;
}

bool FactoryManager::paint( int type, QPainter * painter, 
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const ValuePainterInterface *_painter = d->valuePainters.value(type);
    if (!_painter)
        return false;
    _painter->paint(painter, option, index);
    return true;
}

bool FactoryManager::canConvertValueToText( int type ) const
{
    return d->valueDisplays.value(type) != 0;
}

bool FactoryManager::canConvertValueToText( const Property* property ) const
{
    return d->valueDisplays.value( property->type() ) != 0;
}

QString FactoryManager::convertValueToText( const Property* property ) const
{
    const ValueDisplayInterface *display = d->valueDisplays.value( property->type() );
    return display ? display->displayText( property ) : property->value().toString();
}

EditorCreatorInterface::EditorCreatorInterface()
{
}

EditorCreatorInterface::~EditorCreatorInterface()
{
}

EditorCreatorInterface::Options::Options()
 : removeBorders(true)
{
}

ValuePainterInterface::ValuePainterInterface()
{
}

ValuePainterInterface::~ValuePainterInterface()
{
}

ValueDisplayInterface::ValueDisplayInterface()
{
}

ValueDisplayInterface::~ValueDisplayInterface()
{
}
