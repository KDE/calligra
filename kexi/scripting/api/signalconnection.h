/***************************************************************************
 * signalconnection.h
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

#ifndef KROSS_API_SIGNALCONNECTION_H
#define KROSS_API_SIGNALCONNECTION_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qvariant.h>
#include <qsignalmapper.h>
#include <qguardedptr.h>
#include <qobject.h>
#include <kdebug.h>

namespace Kross { namespace Api {

    // Forward declarations.
    class ScriptContainer;
    class QtObject;
    class SignalHandler;

    /**
     * Each Qt signal and slot connection between a QObject
     * instance and a functionname is represented with
     * a SignalConnection and handled by \a SignalHandler.
     */
    class SignalConnection : public QObject
    {
            Q_OBJECT

        public:
            SignalConnection(SignalHandler* signalhandler);
            virtual ~SignalConnection() {}

            QGuardedPtr<QObject> senderobj;
            //QGuardedPtr<QObject> receiver;
            const char* signal;
            QString function;

        private:
            SignalHandler* m_signalhandler;

        public slots:
            // Stupid signals and slots. To get the passed
            // arguments we need to have all cases of slots
            // avaiable for SignalHandler::connect() signals.
            void callback();
            void callback_short(short);
            void callback_int(int);
            void callback_int(int, int);
            void callback_intintint(int, int, int);
            void callback_intintintint(int, int, int, int);
            void callback_intintintintint(int, int, int, int, int);
            void callback_intintintintbool(int, int, int, int, bool);
            void callback_intbool(int, bool);
            void callback_intintbool(int, int, bool);
            void callback_intintstring(int, int, const QString&);
            void callback_uint(uint);
            void callback_long(long);
            void callback_ulong(ulong);
            void callback_llong(Q_LLONG);
            void callback_ullong(Q_ULLONG);
            void callback_double(double);
            void callback_char(const char*);
            void callback_bool(bool);
            void callback_string(const QString&);
            void callback_stringint(const QString&, int);
            void callback_stringintint(const QString&, int, int);
            void callback_stringuint(const QString&, uint);
            void callback_stringbool(const QString&, bool);
            void callback_stringboolbool(const QString&, bool, bool);
            void callback_stringboolint(const QString&, bool, int);
            void callback_stringstring(const QString&, const QString&);
            void callback_stringstringstring(const QString&, const QString&, const QString&);
            void callback_stringlist(const QStringList&);
            void callback_variant(const QVariant&);
    };

}}

#endif

