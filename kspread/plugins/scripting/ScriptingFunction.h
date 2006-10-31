/*
 * This file is part of KSpread
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTINGFUNCTION_H
#define SCRIPTINGFUNCTION_H

#include <QObject>

/**
 * The ScriptingFunction class provides access to the KSpread::Function
 * functionality to deal with formula functions.
 */
class ScriptingFunction : public QObject
{
        Q_OBJECT

        Q_PROPERTY(QString name READ name)
        Q_PROPERTY(int minparam READ minParam WRITE setMinParam)
        Q_PROPERTY(int maxparam READ maxParam WRITE setMaxParam)
        Q_PROPERTY(QString comment READ comment WRITE setComment)
        Q_PROPERTY(QString syntax READ syntax WRITE setSyntax)

    public:
        ScriptingFunction(QObject* parent);
        virtual ~ScriptingFunction();

        QString name() const;
        void setName(const QString& name);
        int minParam() const;
        void setMinParam(int minparam);
        int maxParam() const;
        void setMaxParam(int maxparam);
        QString comment() const;
        void setComment(const QString& comment);
        QString syntax() const;
        void setSyntax(const QString& syntax);

    public slots:
        void addExample(const QString& example);
        void addParameter(const QString& typeName, const QString& comment);
        bool registerFunction();

    private:
        class Private;
        Private* const d;
};

#endif
