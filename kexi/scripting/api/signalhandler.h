/***************************************************************************
 * signalhandler.h
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

#ifndef KROSS_API_SIGNALHANDLER_H
#define KROSS_API_SIGNALHANDLER_H

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
    class SigalConnection;

    /**
     * The SignalHandler spends us the bridge between
     * Qt QObject instances and our underlying Kross-API.
     */
    class SignalHandler : public QObject
    {
            Q_OBJECT
            friend class SignalConnection;

        public:
            SignalHandler(ScriptContainer* scriptcontainer, QtObject* qtobj = 0);
            ~SignalHandler();

            bool connect(QObject *sender, const char *signal, const QString& functionname);
            bool disconnect(QObject *sender, const char *signal, const QString& functionname);

            // we don't have a qobject-receiver, or?!
            //void connect(const char *signal, QObject *receiver, const char *slot);
            //bool disconnect(const char *signal, QObject *receiver, const char *slot);

        private:
            ScriptContainer* m_scriptcontainer;
            QtObject* m_qtobj;
            QValueList<SignalConnection*> m_connections;
    };

}}

#endif

