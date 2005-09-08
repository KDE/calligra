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

#ifndef KEXISCRIPTING_H
#define KEXISCRIPTING_H

#include <qobject.h>
#include <qstringlist.h>

// Forward declarations.
class KexiScriptContainerPrivate;
class KexiScriptManager;
class KexiScriptManagerPrivate;
class KexiMainWindow;

/**
 * The KexiScriptContainer is a thin wrapper around
 * \a Kross::Api::ScriptContainer instances if they are build
 * in. If not cause scripting is disabled, then this
 * class provides some basic functionality to still let
 * other classes using this class work as expected without
 * any #ifdef's.
 */
class KEXICORE_EXPORT KexiScriptContainer : public QObject
{
        Q_OBJECT
    public:

        /**
        * Constructor.
        *
        * \param manager The \a KexiScriptManager this
        *       \a KexiScriptContainer belongs to.
        * \param name The unique name this \a KexiScriptContainer
        *       is reachable as.
        */
        KexiScriptContainer(KexiScriptManager* manager, const QString& name);

        /**
        * Destructor.
        */
        virtual ~KexiScriptContainer();

        /**
        * \return the name of the used scripting language.
        */
        QString getInterpreterName();

        /**
        * Set the name of the used scripting language.
        *
        * \param interpretername The name of the used interpreter. This
        *       could be something like "python" for the python
        *       interpreter.
        */
        void setInterpreterName(const QString& interpretername);

        /**
        * \return the scripting code.
        */
        QString getCode();

        /**
        * Set the scripting code.
        *
        * \param code The scripting code.
        */
        void setCode(const QString& code);

        /**
        * \return the stdout and stderr output.
        */
        QStringList getOutput();

    signals:

        /**
        * Emitted if the output got cleared.
        */
        void clearOutput();

        /**
        * Emitted if a new logmessage got added to the output.
        */
        void addOutput(const QString&);

        /**
        * Emitted on error if we know the line number where the
        * error happened.
        */
        void lineNo(long);

    public slots:

        /**
        * Try to execute the scripting code.
        */
        bool execute();

    private slots:

        /**
        * Add a new logmessage to stdout.
        */
        void addStdOut(const QString&);

        /**
        * Add a new logmessage to stderr.
        */
        void addStdErr(const QString&);

    private:
        /// Private d-pointer class.
        KexiScriptContainerPrivate* d;
};

/**
 * The KexiScriptManager manages the scripting code. Internaly we
 * use \a Kross::Api::ScriptContainer as scripting bridge.
 */
class KEXICORE_EXPORT KexiScriptManager : public QObject
{
    public:

        /**
         * Constructor.
         *
         * \param mainwindow The \a KexiMainWindow this
         *       \a KexiScriptManager belongs to.
         */
        KexiScriptManager(KexiMainWindow* mainwindow);

        /**
         * Destructor.
         */
        virtual ~KexiScriptManager();

        /**
         * \return the \a KexiMainWindow instance used to create
         *        this KexiScriptManager.
         */
        KexiMainWindow* getKexiMainWindow();

        /**
         * \return true if there exists already a
         * \a KexiScriptContainer instance defined with \p name .
         */
        bool hasScriptContainer(const QString& name);

        /**
         * \return the \a KexiScriptContainer defined with \p name .
         * If there exists no such \a KexiScriptContainer instance
         * with that name and \p create is false return NULL. If
         * \p create is true create a new instance if it doesn't
         * exists already and return this one.
         */
        KexiScriptContainer* getScriptContainer(const QString& name, bool create);

        /**
         * \return a list of interpreter names.
         */
        const QStringList getInterpreters();

    private:
        /// Private d-pointer class.
        KexiScriptManagerPrivate* d;
};

#endif
