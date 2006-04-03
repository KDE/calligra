/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2005 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2005 by Tobi Krebs (tobi.krebs@gmail.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "variable.h"
#include "exception.h"

#include <kdebug.h>

using namespace KoMacro;

namespace KoMacro {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class Variable::Private
	{
		public:

			/**
			* If @a Variable::Type is @a Variable::TypeVariant this QVariant
			* holds the value else it's invalid.
			*/
			QVariant variant;

			/**
			* If @a Variable::Type is @a Variable::TypeObject this QObject is
			* the value else it's NULL.
			*/
			const QObject* object;

	};

}

Variable::Variable()
	: MetaParameter()
	, d( new Private() ) // create the private d-pointer instance.
{
	setType(TypeNone);
	d->object = 0;
}

Variable::Variable(const QVariant& variant)
	: MetaParameter()
	, d( new Private() ) // create the private d-pointer instance.
{
	setVariantType(variant.type());
	d->variant = variant;
	d->object = 0;
}

Variable::Variable(const QObject* object)
	: MetaParameter()
	, d( new Private() ) // create the private d-pointer instance.
{
	setType(TypeObject);
	d->object = object;
}

Variable::Variable(const QDomElement& element)
	: MetaParameter()
	, d( new Private() ) // create the private d-pointer instance.
{

	QString typesignature = element.attribute("type", "const QString&");
	QString value = element.text();

	setSignatureArgument( typesignature );

	switch( type() ) {
		case KoMacro::MetaParameter::TypeVariant: {
			//kdDebug() << QString("KoMacro::Variable(QDomElement) KoMacro::MetaParameter::TypeVariant") << endl;
			// Set the variant without overwritting the previously detected varianttype.
			setVariant( QVariant(value), false );
		} break;
		case KoMacro::MetaParameter::TypeObject: {
			//kdDebug() << QString("KoMacro::Variable(QDomElement) KoMacro::MetaParameter::TypeObject") << endl;
			//TODO setObject();
		} break;
		default: {
			kdWarning() << QString("KoMacro::Variable(QDomElement) KoMacro::MetaParameter::TypeNone") << endl;
		} break;
	}
}

Variable::~Variable()
{
	delete d;
}

const QVariant Variable::variant() const
{
	if(type() != MetaParameter::TypeVariant) {
		throw Exception(
			QString("Variable is not a variant type."),
			"KoMacro::Variable::variant()"
		);
	}

	//if(variantType() == QVariant::Invalid) return QVariant();
	return d->variant;
}

void Variable::setVariant(const QVariant& variant, bool detecttype)
{
	if(detecttype) {
		setVariantType( variant.type() );
	}
	d->variant = variant;
}

const QObject* Variable::object() const
{
	if(! d->object) {
		throw Exception(
			QString("Variable is not a object type."),
			"KoMacro::Variable::object()"
		);
	}

	return d->object;
}

void Variable::setObject(const QObject* object)
{
	setType(TypeObject);
	d->object = object;
}

Variable::operator QVariant () const
{
	return variant();
}

Variable::operator const QObject* () const
{
	return object();
}

const QString Variable::toString() const
{
	switch( type() ) {
		case KoMacro::MetaParameter::TypeVariant: {
			return variant().toString();
		} break;
		case KoMacro::MetaParameter::TypeObject: {
			return QString("[%1]").arg( object()->name() );
		} break;
		default: {
			//throw Exception("Type is undefined.", "KoMacro::Variable::toString()");
		} break;
	}
	return QString::null;
}

int Variable::toInt() const
{
	return variant().toInt();
}

//#include "variable.moc"
