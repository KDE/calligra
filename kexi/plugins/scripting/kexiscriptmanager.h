/* This file is part of the KDE project
   Copyright (C) 2005 Sebastian Sauer <mail@dipe.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXISCRIPTMANAGER_H
#define KEXISCRIPTMANAGER_H

#include <qobject.h>

// Forward declarations.
class KexiScriptPart;
class KexiScriptManager;
class KexiScriptContainerPrivate;

/**
 * The KexiScriptContainer class wraps \a Kross::Api::ScriptContainer
 * instances if build in.
 */
class KexiScriptContainer : public QObject
{
        Q_OBJECT
    public:

        /**
        * Constructor.
        */
        KexiScriptContainer(KexiScriptManager* manager, const QString& name);

        /**
        * Destructor.
        */
        virtual ~KexiScriptContainer();

        /**
        * Return the name of the used scripting language.
        */
        QString getInterpreterName();

        /**
        * Set the name of the used scripting language.
        */
        void setInterpreterName(const QString& interpretername);

        /**
        * Return the scripting code.
        */
        QString getCode();

        /**
        * Set the scripting code.
        */
        void setCode(const QString& code);

    signals:
        void stdout(const QString&);
        void stderr(const QString&);

    public slots:

        /**
        * Try to execute the scripting code.
        */
        bool execute();

    private:
        KexiScriptContainerPrivate* d;

};

/**
 * The KexiScriptManager manages the scripting code. Internaly we
 * use \a Kross::Api::ScriptContainer as scripting bridge.
 */
class KexiScriptManager : public QObject
{
    public:

        /**
         * Constructor.
         */
        KexiScriptManager(KexiScriptPart* part);

        /**
         * Destructor.
         */
        virtual ~KexiScriptManager();

        /**
         * \return the \a KexiScriptContainer defined with \param name.
         * The KexiScriptContainer's are cached.
         */
        KexiScriptContainer* getScriptContainer(const QString& name);

    private:
        QMap<QString, KexiScriptContainer*> m_scriptcontainers;
};

#endif
