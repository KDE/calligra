/*
 * This file is part of KSpread
 *
 * Copyright (c) 2006, 2007 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTINGFUNCTION_H
#define SCRIPTINGFUNCTION_H

#include <QObject>
#include <QVariant>

/**
 * The ScriptingFunction class provides access to the KSpread::Function
 * functionality to deal with formula functions that are written in
 * a scripting language like Python or Ruby.
 *
 * The following sample scripting formula function written in python
 * displays the current datetime.
 * \code
 * import time, Kross, KSpread
 * func = KSpread.function("PYTIME")
 * func.minparam = 0
 * func.maxparam = 1
 * func.comment = "The PYTIME() function displays the current datetime."
 * func.syntax = "PYTIME(string)"
 * func.addParameter("String", "The datetime format string.")
 * func.addExample("PYTIME()")
 * func.addExample("PYTIME(\"%Y-%M-%d %H:%M.%S\")")
 * def update(args):
 *     try:
 *         func.result = time.strftime(args[0] or "%H:%M.%S")
 *     except:
 *         func.error = "Invalid format"
 * func.connect("called(QVariantList)", update)
 * func.registerFunction()
 * \endcode
 */
class ScriptingFunction : public QObject
{
    Q_OBJECT

    /**
    * The name the function has.
    */
    Q_PROPERTY(QString name READ name)

    /**
    * Set the name of the return-type. Could be e.g. "String", "int", "bool" or "float".
    */
    Q_PROPERTY(QString typeName READ typeName WRITE setTypeName)

    /**
    * Minimum number of parameters the function expects.
    */
    Q_PROPERTY(int minparam READ minParam WRITE setMinParam)

    /**
    * Maximum number of parameters the function expects.
    */
    Q_PROPERTY(int maxparam READ maxParam WRITE setMaxParam)

    /**
    * The comment that describes what the function does.
    */
    Q_PROPERTY(QString comment READ comment WRITE setComment)

    /**
    * The syntax string the function looks like.
    */
    Q_PROPERTY(QString syntax READ syntax WRITE setSyntax)

    /**
    * The error-message if there was an error.
    */
    Q_PROPERTY(QString error READ error WRITE setError)

    /**
    * The result of the function call.
    */
    Q_PROPERTY(QVariant result READ result WRITE setResult)

public:
    explicit ScriptingFunction(QObject* parent);
    virtual ~ScriptingFunction();

    /// \return the name the function has.
    QString name() const;
    /// Set the name the function has.
    void setName(const QString& name);

    /// \return the name the function has.
    QString typeName() const;
    /// Set the name the function has.
    void setTypeName(const QString& typeName);

    /// \return the minimum number of parameters the function expects.
    int minParam() const;
    /// Set the minimum number of parameters the function expects.
    void setMinParam(int minparam);
    /// \return the maximum number of parameters the function expects.
    int maxParam() const;
    /// Set the maximum number of parameters the function expects.
    void setMaxParam(int maxparam);

    /// \return the comment that describes what the function does.
    QString comment() const;
    /// Set the comment that describes what the function does.
    void setComment(const QString& comment);

    /// \return the syntax string the function looks like.
    QString syntax() const;
    /// Set the syntax string the function looks like.
    void setSyntax(const QString& syntax);

    /// \return the error-message if there was an error.
    QString error() const;
    /// Set the error-message.
    void setError(const QString& error = QString());

    /// \return the result of the function call.
    QVariant result() const;
    /// Set the result of the function call.
    void setResult(const QVariant& result);

public slots:

    /**
    * Add an example to demonstrate the usage of the function.
    */
    void addExample(const QString& example);

    /**
    * Add details about a parameter the function may expect.
    *
    * Valid values for \p typeName are String, Int, Float, Double,
    * Boolean, Date, Range or Any while the \p comment is a
    * describing string.
    */
    void addParameter(const QString& typeName, const QString& comment);

    /**
    * Register this function.
    */
    bool registerFunction();

signals:

    /**
    * This signal is emitted if the function got called.
    */
    void called(QVariantList args);

private:
    Q_DISABLE_COPY(ScriptingFunction)

    /// \internal d-pointer class.
    class Private;
    /// \internal d-pointer instance.
    Private* const d;
};

#endif
