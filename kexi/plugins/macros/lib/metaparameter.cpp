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

#include "metaparameter.h"
#include "exception.h"
#include "variable.h"

#include <kdebug.h>

using namespace KoMacro;

namespace KoMacro {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class MetaParameter::Private
	{
		public:

			/**
			* The signatures argument that represents this MetaParameter.
			* This could be something like "const QString&", "int" or 
			* "QMap &lt; QString, QVariant &gt; ".
			*/
			QString signatureargument;

			/**
			* The type of the @a Variable .
			*/
			MetaParameter::Type type;

			/**
			* If the @a MetaParameter::Type is a Variant this QVariant::Type
			* is used to defined what kind of Variant it is.
			*/
			QVariant::Type varianttype;

	};

}

MetaParameter::MetaParameter(const QString& signatureargument)
	: KShared()
	, d( new Private() ) // create the private d-pointer instance.
{
	d->type = TypeNone;

	if(! signatureargument.isNull()) {
		setSignatureArgument( signatureargument );
	}
}

MetaParameter::~MetaParameter()
{
	delete d;
}

MetaParameter::Type MetaParameter::type() const
{
	return d->type;
}

const QString MetaParameter::typeName() const
{
	switch( d->type ) {
		case TypeNone:
			return "None";
		case TypeVariant:
			return "Variant";
		case TypeObject:
			return "Object";
	}
	return QString::null;
}

void MetaParameter::setType(MetaParameter::Type type)
{
	d->type = type;
	d->varianttype = QVariant::Invalid;
}

QVariant::Type MetaParameter::variantType() const
{
	return d->varianttype;
}

void MetaParameter::setVariantType(QVariant::Type varianttype)
{
	d->type = TypeVariant;
	d->varianttype = varianttype;
}

void MetaParameter::setSignatureArgument(const QString& signatureargument)
{
	d->signatureargument = signatureargument;

	QString argument = signatureargument;
	if(argument.startsWith("const")) {
		argument = argument.mid(5).stripWhiteSpace();
	}

	if(argument.endsWith("&")) {
		argument = argument.left( argument.length() - 1 ).stripWhiteSpace();
	}

	if(argument.isEmpty()) {
		throw Exception(
			QString("Empty signature argument passed."),
			"KoMacro::MetaParameter::setSignatureArgument"
		);
	}
	if(argument == "QVariant") {
		setVariantType( QVariant::Invalid );
	}
	
	QVariant::Type type = argument.isNull() ? QVariant::Invalid : QVariant::nameToType(argument.latin1());
	if (type != QVariant::Invalid) {
		setVariantType( type );
	}
	else {
		setType( TypeObject );
	}
}

bool MetaParameter::validVariable(Variable::Ptr variable) const
{
	if( type() != variable->type() ) {
		return false;
	}
	return true;
}
