/***************************************************************************
 * signalconnection.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "signalconnection.h"

#include "interpreter.h"
#include "qtobject.h"
#include "variant.h"
#include "../main/scriptcontainer.h"
#include "signalhandler.h"

using namespace Kross::Api;

SignalConnection::SignalConnection(SignalHandler* signalhandler)
    : QObject(signalhandler)
    , m_signalhandler(signalhandler)
{
}

void SignalConnection::callback() { callback_variant(QVariant()); }
void SignalConnection::callback_short(short s) { callback_variant(QVariant(s)); }
void SignalConnection::callback_int(int i) { callback_variant(QVariant(i)); }
void SignalConnection::callback_uint(uint i) { callback_variant(QVariant(i)); }
void SignalConnection::callback_long(long l) { callback_variant(QVariant((Q_LLONG)l)); }
void SignalConnection::callback_ulong(ulong l) { callback_variant(QVariant((Q_ULLONG)l)); }
void SignalConnection::callback_llong(Q_LLONG l) { callback_variant(QVariant(l)); }
void SignalConnection::callback_ullong(Q_ULLONG l) { callback_variant(QVariant(l)); }
void SignalConnection::callback_double(double d) { callback_variant(QVariant(d)); }
void SignalConnection::callback_char(const char* c) { callback_variant(QVariant(c)); }
void SignalConnection::callback_bool(bool b) { callback_variant(QVariant(b)); }
void SignalConnection::callback_string(const QString& s) { callback_variant(QVariant(s)); }
void SignalConnection::callback_stringlist(const QStringList& sl) { callback_variant(QVariant(sl)); }

void SignalConnection::callback_variant(const QVariant& variant)
{
    QObject* obj = (QObject*)sender();
    if(! obj) {
        kdWarning() << "SignalHandler::callback() failed cause sender is not a QObject." << endl;
        return;
    }

    kdDebug() << QString("SignalConnection::callback() sender='%1' signal='%2' function='%3'")
                 .arg(obj->name()).arg(signal).arg(function) << endl;

    QValueList<Object*> args;
    if(variant.isValid())
        args.append( Kross::Api::Variant::create(variant) );

    m_signalhandler->m_scriptcontainer->callFunction(
        function,
        args.isEmpty() ? 0 : Kross::Api::List::create(args)
    );
}

