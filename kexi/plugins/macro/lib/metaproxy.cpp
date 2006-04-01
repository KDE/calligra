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

#include "metaproxy.h"
#include "metaobject.h"
#include "metamethod.h"
#include "variable.h"
#include "exception.h"

#include <qasciidict.h>
#include <qmetaobject.h>

#include <private/qucomextra_p.h>

#include <kdebug.h>

using namespace KoMacro;

namespace KoMacro {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class MetaProxy::Private
	{
		public:
			MetaObject::Ptr metaobject;
			QValueList<const char*> slotlist;
	};

}

MetaProxy::MetaProxy()
	: MetaProxyInterface()
	, d( new Private() ) // create the private d-pointer instance.
{
}

MetaProxy::~MetaProxy()
{
	delete d;
}

void MetaProxy::setSlots(QValueList<const char*> slotlist)
{
	kdDebug()<<"MetaProxy::setSlots(QValueList<const char*>) 1 classname="<<metaObject()->className()<<endl;

	// remember the slots which are set explicit.
	d->slotlist = slotlist;

	// number of slots we like to have.
	int numslots = slotlist.count();

	{
		// determinate the QMetaData-pointer this QObject has.
		QMetaData** slotdata = (QMetaData**)( (ulong)(metaObject()) + 24 );

		/*TODO delete previously created instances.
		if( *slotdata && slotmap.count() ) {
			for(i=0; i<(int)IdxSignalMap.count(); i++) {
				delete [] NCPTR((*slotdata)[i].name, char);
				delete NCPTR((*slotdata)[i].method, QUMethod);
			}
			delete [] *slotdata;
		}
		*/

		// allocated a new QMetaData-array.
		*slotdata = new QMetaData[ numslots ];

		// determinate the slot-dictonary.
		QAsciiDict<const QMetaData>** slotdict = (QAsciiDict<const QMetaData>**)( (ulong)(metaObject()) + 28 );

		// determinate the size of the new slot-dictonary.
		int n;
		if(numslots < 6)
			n = 5;
		else if(numslots < 10)
			n = 11;
		else if(numslots < 14)
			n = 17;
		else
			n = 23;

		// allocate a new slot-dictonary.
		*slotdict = new QAsciiDict<const QMetaData>(n, true, false);
	}

	{
		// get pointers to the new slotdata and slotdict which need to be filled now.
		QMetaData* slotdata = *( (QMetaData**)((ulong)(metaObject()) + 24) );
		QAsciiDict<const QMetaData>* slotdict = *( (QAsciiDict<const QMetaData>**)((ulong)(metaObject()) + 28) );

		// clear previous old items.
		slotdict->clear();

		// fill the slotdata and the slotdict with the new slots.
		for(int i = 0; i < numslots; i++) {

			// We need a slotname
			QCString s = QString("%1")//QString("%1_%2")
							//.arg(metaObject()->className())
							.arg(slotlist[i])
							.ascii();
			char* slotname = new char[ s.length() + 1 ];
			qstrcpy(slotname, s);
			kdDebug() << QString("MetaProxy::setSlots(QValueList<const char*>) add slot '%1'").arg(slotname) << endl;

			// The method. We handle it on the fly anyway. So, just add a
			// dummy method here.
			QUMethod* method = new QUMethod;
			method->name = slotname;
			method->count = 0;
			method->parameters = 0;

			// Fill our slotdata array.
			slotdata[i].name = slotname;
			slotdata[i].method = method;
			slotdata[i].access = QMetaData::Public;

			// Fill the slotdict dictonary.
			slotdict->insert( slotname , slotdata + i );
		}
	}
}

bool MetaProxy::connectSignal(const QObject* sender, const char * signal)
{
	QCString slotname = QString("slot_%1").arg(signal).ascii();
	d->slotlist.append(slotname);
	setSlots(d->slotlist);

	connect(sender, signal, QString("%1%2").arg(SLOT_CODE).arg(slotname).ascii());
	return true;
}

bool MetaProxy::qt_invoke(int index, QUObject* uo)
{
	kdDebug() << "MetaProxy::qt_invoke(int,QUObject*) index=" << index << endl;

	try {
		if(! d->metaobject) {
			// create the MetaObject instance on demand.
			d->metaobject = new MetaObject(this);
		}

		MetaMethod::Ptr metamethod = d->metaobject->method(index);

//if(metamethod->signatureTag().startsWith("print")) return MetaProxyInterface::qt_invoke(index, uo);
kdDebug() << "MetaProxy::qt_invoke(int,QUObject*) tag=" << metamethod->signatureTag() << endl;

		Variable::List arguments = metamethod->toVariableList(uo);

		emit slotCalled(arguments);
		return true;
	}
	catch(Exception& e) {
		kdWarning() << QString("EXCEPTION in MetaProxy::qt_invoke(int,QUObject*) %1").arg(e.errorMessage()) << endl;
		return false;
	}

	/*TESTCASE
	ou++;
	QString desc( ou->type->desc() );
	kdDebug()<<"desc="<<desc<<endl;
	if(desc == "QString") kdDebug()<<"string="<< static_QUType_QString.get(ou) <<endl;
	*/

	return true;
}

#include "metaproxy.moc"
