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

#include "metamethod.h"
#include "metaobject.h"
#include "metaparameter.h"
#include "variable.h"
#include "exception.h"

#include <qobject.h>
#include <qmetaobject.h>

// to access the Qt3 QUObject API.
#include <private/qucom_p.h>
#include <private/qucomextra_p.h>

#include <kdebug.h>

using namespace KoMacro;

namespace KoMacro {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class MetaMethod::Private
	{
		public:

			/**
			* The signature this @a MetaMethod has.
			*/
			QString signature;

			/**
			* The signature tagname this @a MetaMethod has.
			*/
			QString signaturetag;

			/**
			* The signature arguments this @a MetaMethod has.
			*/
			QString signaturearguments;

			/**
			* Cached signature arguments parsed into a list
			* of @a MetaParameter instances.
			*/
			MetaParameter::List arguments;

			/**
			* The @a MetaObject this @a MetaMethod belongs to or is NULL
			* if this @a MetaMethod doesn't belong to any @a MetaObject
			* yet.
			*/
			MetaObject::Ptr object;

			/**
			* The @a MetaMethod::Type this method provides access
			* to.
			*/
			MetaMethod::Type type;
	};

}

MetaMethod::MetaMethod(const QString& signature, Type type, KSharedPtr<MetaObject> object)
	: KShared()
	, d( new Private() ) // create the private d-pointer instance.
{
	d->signature = signature;
	d->object = object;
	d->type = type;

	int startpos = d->signature.find("(");
	int endpos = d->signature.findRev(")");
	if(startpos < 0 || startpos > endpos) {
		throw Exception(
			QString("Invalid signature \"%1\"").arg(d->signature),
			"KoMacro::MetaMethod"
		);
	}

	d->signaturetag = d->signature.left(startpos).stripWhiteSpace();
	if(d->signaturetag.isEmpty()) {
		throw Exception(
			QString("Invalid tagname in signature \"%1\"").arg(d->signature),
			"KoMacro::MetaMethod"
		);
	}

	d->signaturearguments = d->signature.mid(startpos + 1, endpos - startpos - 1).stripWhiteSpace();

	do {
		int commapos = d->signaturearguments.find(",");
		int starttemplatepos = d->signaturearguments.find("<");
		if(starttemplatepos >= 0 && (commapos < 0 || starttemplatepos < commapos)) {
			int endtemplatepos = d->signaturearguments.find(">", starttemplatepos);
			if(endtemplatepos <= 0) {
				throw Exception(
					QString("No closing template-definiton in signature \"%1\"").arg(d->signature),
					"KoMacro::MetaMethod"
				);
			}
			commapos = d->signaturearguments.find(",", endtemplatepos);
		}

		if(commapos > 0) {
			QString s = d->signaturearguments.left(commapos).stripWhiteSpace();
			if(! s.isEmpty()) {
				d->arguments.append( new MetaParameter(s) );
			}
			d->signaturearguments = d->signaturearguments.right(d->signaturearguments.length() - commapos - 1);
		}
		else {
			QString s = d->signaturearguments.stripWhiteSpace();
			if(! s.isEmpty()) {
				d->arguments.append( new MetaParameter(s) );
			}
			break;
		}
	} while(true);
}

MetaMethod::~MetaMethod()
{
	delete d;
}

MetaObject::Ptr const MetaMethod::object() const
{
	return d->object;
}

const QString MetaMethod::signature() const
{
	return d->signature;
}

const QString MetaMethod::signatureTag() const
{
	return d->signaturetag;
}

const QString MetaMethod::signatureArguments() const
{
	return d->signaturearguments;
}

MetaMethod::Type MetaMethod::type() const
{
	return d->type;
}

MetaParameter::List MetaMethod::arguments() const
{
	return d->arguments;
}

QUObject* MetaMethod::toQUObject(Variable::List arguments)
{
	uint argsize = d->arguments.size();

	if(arguments.size() <= argsize) {
		throw Exception(
			QString("To less arguments for slot with siganture \"%1\"").arg(d->signature),
			"KoMacro::MetaMethod::quobject"
		);
	}

	// The first item in the QUObject-array is for the returnvalue
	// while everything >=1 are the passed parameters.
	QUObject* uo = new QUObject[ argsize + 1 ];

	uo[0] = QUObject(); // empty placeholder for the returnvalue.

	for(uint i = 0; i < argsize; i++) {
		MetaParameter::Ptr metaargument = d->arguments[i];
		Variable::Ptr variable = arguments[i + 1];

		if ( !variable ) {
	 		throw Exception(
				QString("Variable is undefined !"),
				"KoMacro::MetaMethod::quobject"
			);
		}
	
		if(metaargument->type() != variable->type()) {
			throw Exception(
				QString("Wrong variable type in method \"%1\". Expected \"%2\" but got \"%3\"").arg(d->signature).arg(metaargument->type()).arg(variable->type()),
				"KoMacro::MetaMethod::quobject"
			);
		}

		switch(metaargument->type()) {

			case Variable::TypeNone: {
				kdDebug() << "Variable::TypeNone" << endl;
				uo[i + 1] = QUObject();
			} break;

			case Variable::TypeVariant:  {
				kdDebug() << "Variable::TypeVariant" << endl;

				const QVariant variant = variable->variant();
				switch(metaargument->variantType()) {
					case QVariant::String: {
						const QString s = variant.toString();
						static_QUType_QString.set( &(uo[i + 1]), s );
					} break;
					case QVariant::Int: {
						const int j = variant.toInt();
						static_QUType_int.set( &(uo[i + 1]), j );
					} break;
					case QVariant::Bool: {
						const bool b = variant.toBool();
						static_QUType_bool.set( &(uo[i + 1]), b );
					} break;
					case QVariant::Double: {
						const double d = variant.toDouble();
						static_QUType_double.set( &(uo[i + 1]), d );
					} break;
					case QVariant::Invalid: {
						static_QUType_QVariant.set( &(uo[i + 1]), variant );
					}

					/*FIXME
					static_QUType_charstar
					static_QUType_ptr.get(uo); QObject *qobj = (QObject *)(ptr);
					*/

					default: {
						throw Exception(
							//QString("Invalid parameter \"%1\" in slot siganture \"%2\"").arg(p).arg(d->signature),
							QString("Invalid parameter !!!!!!!!!!!!!!!!!!!!!!!"),
							"KoMacro::MetaMethod::quobject"
						);
					} break;
				}
			} break;

			case Variable::TypeObject:  {
				kdDebug() << "Variable::TypeObject" << endl;

				const QObject* obj = arguments[i + 1]->object();
				if(! obj) { //FIXME: move check to MetaParameter?!
					throw Exception(
						QString("No QObject !"),
						"KoMacro::MetaMethod::quobject"
					);
				}
				static_QUType_ptr.set( &(uo[i + 1]), obj );
			} break;

			default:  {
				throw Exception(
					QString("Invalid variable type"),
					"KoMacro::MetaMethod::quobject"
				);
			} break;
		}

	}

	return uo;
}

Variable::Ptr MetaMethod::toVariable(QUObject* uo)
{
	const QString desc( uo->type->desc() );

	if(desc == "null") {
		return new Variable();
	}

	if(desc == "QString") {
		const QString s = static_QUType_QString.get(uo);
		return new Variable(s);
	}

	if(desc == "int") {
		const int j = static_QUType_int.get(uo);
		return new Variable(j);
	}

	if(desc == "bool") {
		const bool b = static_QUType_bool.get(uo);
		return new Variable(b);
	}

	if(desc == "double") {
		const double d = static_QUType_double.get(uo);
		return new Variable(d);
	}

	if(desc == "QVariant") {
		QVariant v = static_QUType_QVariant.get(uo);
		return new Variable(v);
	}

	throw Exception(
		QString("Invalid parameter '%1'").arg(desc),
		"KoMacro::MetaMethod::toVariable"
	);
}

Variable::List MetaMethod::toVariableList(QUObject* uo)
{
	Variable::List list;

	for(MetaParameter::List::Iterator it = d->arguments.begin(); it != d->arguments.end(); ++it) {
		list.append( toVariable(uo) );
		uo++;
	}

	return list;
}

Variable::Ptr MetaMethod::invoke(Variable::List arguments)
{
	kdDebug() << "Variable::Ptr MetaMethod::invoke(Variable::List arguments)" << endl; 

	if(! d->object) {
		throw Exception("MetaObject is undefined.", "KoMacro::MetaMethod::invoke(Variable::List)");
	}

	QObject* obj = d->object->object();
	Variable::Ptr returnvalue;
	QUObject* qu = 0;

	try {
		qu = toQUObject(arguments);

		switch( d->type ) {
			case Signal: {
				int index = d->object->indexOfSignal( d->signature.latin1() );
				obj->qt_emit(index, qu);
			} break;
			case Slot: {
				int index = d->object->indexOfSlot( d->signature.latin1() );
				obj->qt_invoke(index, qu);
			} break;
			default: {
				throw Exception("Unknown type.", "KoMacro::MetaMethod::invoke(Variable::List)");
			} break;
		}
		returnvalue = toVariable( &qu[0] );
	}
	catch(Exception& e) {
		delete [] qu; // free the QUObject array and
		kdDebug() << "EXCEPTION in KoMacro::MetaMethod::invoke(Variable::List)" << endl;
		e.addTraceMessage("KoMacro::MetaMethod::invoke(Variable::List)");
		throw e; // re-throw the exception
	}

	delete [] qu;
	return returnvalue;
}
