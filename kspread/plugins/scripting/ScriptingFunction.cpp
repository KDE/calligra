/*
 * This file is part of KSpread
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
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

#include "ScriptingFunction.h"

#include <QDomDocument>
#include <QPointer>

#include <klocale.h>

//#include <Doc.h>
//#include <View.h>
#include <Value.h>
#include <Functions.h>

//#define KROSS_MAIN_EXPORT KDE_EXPORT
#include <kross/core/manager.h>
#include <kross/core/actioncollection.h>

/***************************************************************************
 * ScriptingFunctionImpl
 */

/// \internal implementation of the ScriptingFunction
class ScriptingFunctionImpl : public KSpread::Function
{
    public:

        static KSpread::Value callback(KSpread::valVector args, KSpread::ValueCalc* calc, KSpread::FuncExtra* extra)
        {
            Q_UNUSED(calc);
            Q_ASSERT(extra && extra->function);
            ScriptingFunctionImpl* funcimpl = static_cast< ScriptingFunctionImpl* >( extra->function );

            kDebug() << "ScriptingFunctionImpl::callback" << endl;

            if( ! funcimpl->m_function) {
                kDebug() << QString("ScriptingFunctionImpl::callback ScriptingFunction instance is NULL.") << endl;
                KSpread::Value err = KSpread::Value::errorNA();
                err.setError( '#' + i18n("No such script.") );
                return err;
            }

            kDebug() << QString("ScriptingFunctionImpl::callback name=%1 argcount=%2").arg(funcimpl->m_function->name()).arg(args.count()) << endl;

            QVariantList list;
            int size = args.size();
            for(int i = 0; i < size; ++i) {
                //TODO needs to be more generic!
                list.append( args[i].asString() );
            }

            funcimpl->m_function->setResult( QVariant() );

            if( ! QMetaObject::invokeMethod(funcimpl->m_function, "called", QGenericReturnArgument(), Q_ARG(QVariantList, list)) ) {
                KSpread::Value err = KSpread::Value::errorVALUE(); //errorNAME();
                err.setError( '#' + i18n("No such script function.") );
                return err;
            }

            //TODO
            QVariant result = funcimpl->m_function->result();
            if( ! result.isValid() ) {
                KSpread::Value err = KSpread::Value::errorVALUE(); //errorNAME();
                err.setError( '#' + i18n("No return value.") );
                return err;
            }
            kDebug() << "result=" << result.toString() << endl;
            return KSpread::Value( result.toString() );

            //kDebug() << "result=" << result << endl;
            //return KSpread::Value( result );
        }

        ScriptingFunctionImpl(ScriptingFunction* function, const QDomElement& description)
            : KSpread::Function(function->name(), ScriptingFunctionImpl::callback)
            , m_function(function)
        {
            setNeedsExtra(true);

            // if there exists no "Scripts" group yet, add it
            KSpread::FunctionRepository* repo = KSpread::FunctionRepository::self();
            if( ! repo->groups().contains( i18n("Scripts") ) )
                repo->addGroup( i18n("Scripts") );

            // register ourself at the repository
            repo->add(this);

            // create a new description for the function
            KSpread::FunctionDescription* desc = new KSpread::FunctionDescription(description);
            desc->setGroup( i18n("Scripts") );
            repo->add(desc);
        }

        virtual ~ScriptingFunctionImpl() {}

    private:
        QPointer<ScriptingFunction> m_function;
};

/***************************************************************************
 * ScriptingFunction
 */

/// \internal d-pointer class.
class ScriptingFunction::Private
{
    public:
        QString name;
        int minparam;
        int maxparam;
        QString comment;
        QString syntax;
        QVariant result;

        QDomDocument document;
        QDomElement funcElement;
        QDomElement helpElement;

        Private() : minparam(0), maxparam(-1) {}
};

ScriptingFunction::ScriptingFunction(QObject* parent)
    : QObject(parent)
    , d(new Private())
{
    kDebug() << "ScriptingFunction::ScriptingFunction" << endl;
    d->funcElement = d->document.createElement("Function");
    d->helpElement = d->document.createElement("Help");
}

ScriptingFunction::~ScriptingFunction()
{
    kDebug() << "ScriptingFunction::~ScriptingFunction" << endl;
    delete d;
}

QString ScriptingFunction::name() const { return d->name; }
void ScriptingFunction::setName(const QString& name) { d->name = name; }
int ScriptingFunction::minParam() const { return d->minparam; }
void ScriptingFunction::setMinParam(int minparam) { d->minparam = minparam; }
int ScriptingFunction::maxParam() const { return d->maxparam; }
void ScriptingFunction::setMaxParam(int maxparam) { d->maxparam = maxparam; }
QString ScriptingFunction::comment() const { return d->comment; }
void ScriptingFunction::setComment(const QString& comment) { d->comment = comment; }
QString ScriptingFunction::syntax() const { return d->syntax; }
void ScriptingFunction::setSyntax(const QString& syntax) { d->syntax = syntax; }
QVariant ScriptingFunction::result() const { return d->result; }
void ScriptingFunction::setResult(const QVariant& result) { d->result = result; }

void ScriptingFunction::addExample(const QString& example)
{
    QDomElement helpExampleElem = d->document.createElement("Example");
    helpExampleElem.appendChild( d->document.createTextNode(example) );
    d->helpElement.appendChild(helpExampleElem);
}

void ScriptingFunction::addParameter(const QString& typeName, const QString& comment)
{
    QDomElement paramElem = d->document.createElement("Parameter");
    QDomElement paramCommentElem = d->document.createElement("Comment");
    paramCommentElem.appendChild( d->document.createTextNode(comment) );
    paramElem.appendChild(paramCommentElem);
    QDomElement paramTypeElem = d->document.createElement("Type");
    paramTypeElem.appendChild( d->document.createTextNode(typeName) );
    paramElem.appendChild(paramTypeElem);
    d->funcElement.appendChild(paramElem);
}

bool ScriptingFunction::registerFunction()
{
    kDebug() << "ScriptingFunction::registerFunction" << endl;

    if( d->name.isEmpty() ) {
        kWarning() << "ScriptingFunction::registerFunction() name is empty!" << endl;
        return false;
    }

    QDomElement nameelem = d->document.createElement("Name");
    nameelem.appendChild( d->document.createTextNode(d->name) );
    d->funcElement.appendChild(nameelem);

    QDomElement typeelem = d->document.createElement("Type");
    typeelem.appendChild( d->document.createTextNode("String") );
    d->funcElement.appendChild(typeelem);

    QDomElement helpTextElem = d->document.createElement("Text");
    helpTextElem.appendChild( d->document.createTextNode(d->comment) );
    d->helpElement.appendChild(helpTextElem);

    QDomElement helpSyntaxElem = d->document.createElement("Syntax");
    helpSyntaxElem.appendChild( d->document.createTextNode(d->syntax) );
    d->helpElement.appendChild(helpSyntaxElem);

    d->funcElement.appendChild(d->helpElement);

    // Create a new ScriptingFunctionImpl instance which will publish itself to the
    // FunctionRepository. The FunctionRepository takes ownership of the instance
    // which may live longer then this ScriptingFunction instance.
    ScriptingFunctionImpl* funcimpl = new ScriptingFunctionImpl(this, d->funcElement);
    funcimpl->setParamCount(d->minparam, d->maxparam);
    funcimpl->setAcceptArray();
    return true;
}

#include "ScriptingFunction.moc"
