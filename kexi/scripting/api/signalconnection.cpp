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

#include <qvaluelist.h>

using namespace Kross::Api;

SignalConnection::SignalConnection(SignalHandler* signalhandler)
    : QObject(signalhandler)
    , m_signalhandler(signalhandler)
{
}

bool SignalConnection::connect()
{
    // try to determinate Qt parameters from signal
    QString signature = QString(signal).mid(1);
    int startpos = signature.find("(");
    int endpos = signature.findRev(")");
    if(startpos < 0 || startpos > endpos) {
        kdWarning() << QString("SignalHandler::connect() Invalid signal '%1' from sender '%2' on the try to connect with function '%3'.")
                       .arg(signal).arg(senderobj->name()).arg(function) << endl;
        return false;
    }
    QString params = signature.mid(startpos + 1, endpos - startpos - 1);
    QStringList paramlist = QStringList::split(",", params);
    QString signalname = signature.left(startpos);

    kdDebug() << QString("SignalHandler::connect signal='%1' signalname='%2' params='%3' function='%4'")
                         .arg(signal).arg(signalname).arg(params).arg(function) << endl;

    if(paramlist.isEmpty())
        return QObject::connect((QObject*)senderobj, signal, SLOT(callback()));

    if(paramlist.count() >= 5) {
        if(checkConnectArgs(signal, this, SLOT(callback_intintintintint(int, int, int, int, int))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_int(int, int)));
        if(checkConnectArgs(signal, this, SLOT(callback_intintintintbool(int, int, int, int, bool))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_int(int, int)));
    }
    if(paramlist.count() >= 4) {
        if(checkConnectArgs(signal, this, SLOT(callback_intintintint(int, int, int, int))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_intintintint(int, int, int, int)));
    }
    if(paramlist.count() >= 3) {
        if(checkConnectArgs(signal, this, SLOT(callback_intintint(int, int, int))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_intintint(int, int, int)));
        if(checkConnectArgs(signal, this, SLOT(callback_intintbool(int, int, bool))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_intintbool(int, int, bool)));
        if(checkConnectArgs(signal, this, SLOT(callback_intintstring(int, int, const QString&))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_intintstring(int, int, const QString&)));
        if(checkConnectArgs(signal, this, SLOT(callback_stringintint(const QString&, int, int))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_stringintint(const QString&, int, int)));
        if(checkConnectArgs(signal, this, SLOT(callback_stringboolbool(const QString&, bool, bool))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_stringboolbool(const QString&, bool, bool)));
        if(checkConnectArgs(signal, this, SLOT(callback_stringboolint(const QString&, bool, int))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_stringboolint(const QString&, bool, int)));
        if(checkConnectArgs(signal, this, SLOT(callback_stringstringstring(const QString&, const QString&, const QString&))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_stringstringstring(const QString&, const QString&, const QString&)));
    }

    if(paramlist.count() >= 2) {
        if(checkConnectArgs(signal, this, SLOT(callback_int(int, int))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_int(int, int)));
        if(checkConnectArgs(signal, this, SLOT(callback_intbool(int, bool))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_intbool(int, bool)));
        if(checkConnectArgs(signal, this, SLOT(callback_stringint(const QString&, int))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_stringint(const QString&, int)));
        if(checkConnectArgs(signal, this, SLOT(callback_stringuint(const QString&, uint))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_stringuint(const QString&, uint)));
        if(checkConnectArgs(signal, this, SLOT(callback_stringbool(const QString&, bool))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_stringbool(const QString&, bool)));
        if(checkConnectArgs(signal, this, SLOT(callback_stringstring(const QString&, const QString&))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_stringstring(const QString&, const QString&)));
    }

    if(paramlist.count() >= 1) {
        if(checkConnectArgs(signal, this, SLOT(callback_short(short))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_short(short)));
        if(checkConnectArgs(signal, this, SLOT(callback_int(int))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_int(int)));
        if(checkConnectArgs(signal, this, SLOT(callback_uint(uint))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_uint(uint)));
        if(checkConnectArgs(signal, this, SLOT(callback_long(long))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_ulong(long)));
        if(checkConnectArgs(signal, this, SLOT(callback_ulong(ulong))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_ulong(ulong)));
        if(checkConnectArgs(signal, this, SLOT(callback_double(double))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_double(double)));
        if(checkConnectArgs(signal, this, SLOT(callback_char(const char*))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_char(const char*)));
        if(checkConnectArgs(signal, this, SLOT(callback_bool(bool))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_bool(bool)));
        if(checkConnectArgs(signal, this, SLOT(callback_string(const QString&))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_string(const QString&)));
        if(checkConnectArgs(signal, this, SLOT(callback_stringlist(const QStringList&))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_stringlist(const QStringList&)));
        if(checkConnectArgs(signal, this, SLOT(callback_variant(const QVariant&))))
            return QObject::connect((QObject*)senderobj, signal, SLOT(callback_variant(const QVariant&)));
    }

    kdWarning() << QString("SignalHandler::connect Failed to connect signal='%1'").arg(signature) << endl;
    return false;
}

void SignalConnection::callback() {
    callback_variant(QVariant()); }
void SignalConnection::callback_short(short s) {
    callback_variant(QVariant(s)); }
void SignalConnection::callback_int(int i) {
    callback_variant(QVariant(i)); }
void SignalConnection::callback_int(int i1, int i2) {
    callback_variant(QVariant( QValueList<QVariant>() << i1 << i2 )); }
void SignalConnection::callback_intintint(int i1, int i2, int i3) {
    callback_variant(QVariant( QValueList<QVariant>() << i1 << i2 << i3 )); }
void SignalConnection::callback_intintintint(int i1, int i2, int i3, int i4) {
    callback_variant(QVariant( QValueList<QVariant>() << i1 << i2 << i3 << i4 )); }
void SignalConnection::callback_intintintintint(int i1, int i2, int i3, int i4, int i5) {
    callback_variant(QVariant( QValueList<QVariant>() << i1 << i2 << i3 << i4 << i5 )); }
void SignalConnection::callback_intintintintbool(int i1, int i2, int i3, int i4, bool b) {
    callback_variant(QVariant( QValueList<QVariant>() << i1 << i2 << i3 << i4 << b )); }
void SignalConnection::callback_intbool(int i1, bool b) {
    callback_variant(QVariant( QValueList<QVariant>() << i1 << b )); }
void SignalConnection::callback_intintbool(int i1, int i2, bool b) {
    callback_variant(QVariant( QValueList<QVariant>() << i1 << i2 << b )); }
void SignalConnection::callback_intintstring(int i1, int i2, const QString& s) {
    callback_variant(QVariant( QValueList<QVariant>() << i1 << i2 << s )); }
void SignalConnection::callback_uint(uint i) {
    callback_variant(QVariant(i)); }
void SignalConnection::callback_long(long l) {
    callback_variant(QVariant((Q_LLONG)l)); }
void SignalConnection::callback_ulong(ulong l) {
    callback_variant(QVariant((Q_ULLONG)l)); }
void SignalConnection::callback_double(double d) {
    callback_variant(QVariant(d)); }
void SignalConnection::callback_char(const char* c) {
    callback_variant(QVariant(c)); }
void SignalConnection::callback_bool(bool b) {
    callback_variant(QVariant(b)); }
void SignalConnection::callback_string(const QString& s) {
    callback_variant(QVariant(s)); }
void SignalConnection::callback_stringint(const QString& s, int i) {
    callback_variant(QVariant( QValueList<QVariant>() << s << i )); }
void SignalConnection::callback_stringintint(const QString& s, int i1, int i2) {
    callback_variant(QVariant( QValueList<QVariant>() << s << i1 << i2 )); }
void SignalConnection::callback_stringuint(const QString& s, uint i) {
    callback_variant(QVariant( QValueList<QVariant>() << s << i )); }
void SignalConnection::callback_stringbool(const QString& s, bool b) {
    callback_variant(QVariant( QValueList<QVariant>() << s << b )); }
void SignalConnection::callback_stringboolbool(const QString& s, bool b1, bool b2) {
    callback_variant(QVariant( QValueList<QVariant>() << s << b1 << b2 )); }
void SignalConnection::callback_stringboolint(const QString& s, bool b, int i) {
    callback_variant(QVariant( QValueList<QVariant>() << s << b << i )); }
void SignalConnection::callback_stringstring(const QString& s1, const QString& s2) {
    callback_variant(QVariant( QValueList<QVariant>() << s1 << s2 )); }
void SignalConnection::callback_stringstringstring(const QString& s1, const QString& s2, const QString& s3) {
    callback_variant(QVariant( QValueList<QVariant>() << s1 << s2 << s3 )); }
void SignalConnection::callback_stringlist(const QStringList& sl) {
    callback_variant(QVariant(sl)); }

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

