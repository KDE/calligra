/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef MSOOXMLREADER_P_H
#define MSOOXMLREADER_P_H

#ifndef MSOOXML_CURRENT_CLASS
#error Please include MsooXmlReader_p.h after defining MSOOXML_CURRENT_CLASS and MSOOXML_CURRENT_NS!
#endif

#define PASTE2(a, b) a##b
#define PASTE(a, b) PASTE2( a, b) // indirection needed because only function-like macro parameters can be pasted

#define PASTE3_(a, b, c) a##b##c
#define PASTE3(a, b, c) PASTE3_( a, b, c) // indirection needed because only function-like macro parameters can be pasted

#define JOIN2(a, b) a#b
#define JOIN(a, b) JOIN2( a, b) // indirection needed because only function-like macro parameters can be pasted

#define STRINGIFY(s) JOIN("", s)

//! Used to pass context: creates enum value {el}_{CURRENT_EL}
#define PASS_CONTEXT(el) PASTE3(el, _, CURRENT_EL)

#define TRY_READ_WITH_ARGS_INTERNAL(name, args, context) \
    args \
    RETURN_IF_ERROR( read_ ## name (context) )

#define TRY_READ_WITH_ARGS(name, args) \
    TRY_READ_WITH_ARGS_INTERNAL(name, m_read_ ## name ## _args = args,)

#define TRY_READ_WITH_ARGS_IN_CONTEXT(name, args) \
    TRY_READ_WITH_ARGS_INTERNAL(name, m_read_ ## name ## _args = args, PASS_CONTEXT(name))

#define TRY_READ(name) \
    TRY_READ_WITH_ARGS_INTERNAL(name, ,)

#define TRY_READ_IN_CONTEXT(name) \
    TRY_READ_WITH_ARGS_INTERNAL(name, , PASS_CONTEXT(name))

#ifdef MSOOXML_CURRENT_NS
# define QUALIFIED_NAME(name) \
    JOIN(MSOOXML_CURRENT_NS ":",name)
#else
# define QUALIFIED_NAME(name) \
    STRINGIFY(name)
#endif

#ifdef NDEBUG
# define PUSH_NAME_INTERNAL
# define POP_NAME_INTERNAL
#else // DEBUG
//! returns caller name at the current scope or "top level"
#define CALL_STACK_TOP_NAME (m_callsNamesDebug.isEmpty() \
    ? QByteArray("top level") : m_callsNamesDebug.top()).constData()
# define PUSH_NAME
//! put at beginning of each read_*() method on call stack, only in debug mode
# define PUSH_NAME_INTERNAL \
    /*kDebug() << CALL_STACK_TOP_NAME << "==>" << QUALIFIED_NAME(CURRENT_EL); */\
    m_callsNamesDebug.push(STRINGIFY(CURRENT_EL));
//! put at the end of each read_*() method on call stack, only in debug mode
# define POP_NAME_INTERNAL \
    m_callsNamesDebug.pop(); \
    /*kDebug() << CALL_STACK_TOP_NAME << "<==" << QUALIFIED_NAME(CURRENT_EL); */
#endif

#define READ_PROLOGUE2(method) \
    if (!expectEl(QUALIFIED_NAME(CURRENT_EL))) { \
        return KoFilter::WrongFormat; \
    } \
    PUSH_NAME_INTERNAL \

#define READ_PROLOGUE \
    READ_PROLOGUE2(CURRENT_EL)

#define READ_EPILOGUE_WITHOUT_RETURN \
    POP_NAME_INTERNAL \
    if (!expectElEnd(QUALIFIED_NAME(CURRENT_EL))) { \
        /*kDebug() << "READ_EPILOGUE:" << QUALIFIED_NAME(CURRENT_EL) << "not found!"; */\
        return KoFilter::WrongFormat; \
    } \
    /*kDebug() << "/READ_EPILOGUE_WITHOUT_RETURN";*/

#define READ_EPILOGUE \
    /*kDebug() << "READ_EPILOGUE";*/ \
    READ_EPILOGUE_WITHOUT_RETURN \
    return KoFilter::OK;

#define BREAK_IF_END_OF_QSTRING(name) \
    /*kDebug() << "BREAK_IF_END_OF" << name << "found:" << qualifiedName();*/ \
    if (isEndElement() && qualifiedName() == name) { \
        break; \
    }

#define BREAK_IF_END_OF(name) \
    BREAK_IF_END_OF_QSTRING(QLatin1String(QUALIFIED_NAME(name)))

//inline bool aaaa(const char * aa) { kDebug() << "aa" << aa; return true; }

#define QUALIFIED_NAME_IS(name) \
    (qualifiedName() == QLatin1String(QUALIFIED_NAME(name)))

#define TRY_READ_IF_INTERNAL(name, qname, context) \
    if (qualifiedName() == QLatin1String(qname)) { \
        if (!isStartElement()) { /* sanity check */ \
            raiseError(i18n("Start element \"%1\" expected, found \"%2\"", \
                       QLatin1String(STRINGIFY(name)), tokenString())); \
            return KoFilter::WrongFormat; \
        } \
        /*kDebug() << "TRY_READ_IF " STRINGIFY(name) " started";*/ \
        TRY_READ_WITH_ARGS_INTERNAL(name, , context) \
        /*kDebug() << "TRY_READ_IF " STRINGIFY(name) " finished";*/ \
    }

#define TRY_READ_IF_IN_CONTEXT_INTERNAL(name, context) \
    TRY_READ_IF_INTERNAL(name, QUALIFIED_NAME(name), context)

#define TRY_READ_IF_NS_IN_CONTEXT_INTERNAL(ns, name, context) \
    TRY_READ_IF_INTERNAL(name, JOIN(STRINGIFY(ns) ":", name), context)

//! Tries to read element @a name by entering into read_{name} function.
//! Must be enclosed within if (isStartElement()), otherwise error will be returned.
#define TRY_READ_IF_IN_CONTEXT(name) \
    TRY_READ_IF_IN_CONTEXT_INTERNAL(name, PASS_CONTEXT(name))

#define TRY_READ_IF_NS_IN_CONTEXT(ns, name) \
    TRY_READ_IF_NS_IN_CONTEXT_INTERNAL(ns, name, PASS_CONTEXT(name))

#define TRY_READ_IF(name) \
    TRY_READ_IF_IN_CONTEXT_INTERNAL(name,)

#define ELSE_TRY_READ_IF(name) \
    else TRY_READ_IF_IN_CONTEXT_INTERNAL(name,)

#define ELSE_TRY_READ_IF_IN_CONTEXT(name) \
    else TRY_READ_IF_IN_CONTEXT_INTERNAL(name, PASS_CONTEXT(name))

#define TRY_READ_IF_NS_INTERNAL(ns, name) \
    if (qualifiedName() == QLatin1String(JOIN(STRINGIFY(ns) ":", name))) { \
        /*kDebug() << "TRY_READ_IF_NS " JOIN(STRINGIFY(ns) ":", name) " started";*/ \
        TRY_READ(name); \
        /*kDebug() << "TRY_READ_IF_NS " JOIN(STRINGIFY(ns) ":", name) " finished";*/ \
    }

//! Like TRY_READ_IF() but namespace for explicit namespace @a ns.
#define TRY_READ_IF_NS(ns, name) \
    if (!isStartElement()) { /* sanity check */ \
        raiseError(i18n("Start element \"%1\" expected, found \"%2\"", QLatin1String(JOIN(STRINGIFY(ns) ":", name)), tokenString())); \
        return KoFilter::WrongFormat; \
    } \
    else TRY_READ_IF_NS_INTERNAL(ns, name)

#define ELSE_TRY_READ_IF_NS(ns, name) \
    else TRY_READ_IF_NS_INTERNAL(ns, name)

#define ELSE_WRONG_FORMAT \
    else { \
        return KoFilter::WrongFormat; \
    }

#define ELSE_WRONG_FORMAT_DEBUG(dbg) \
    else { \
        kDebug() << dbg; \
        return KoFilter::WrongFormat; \
    }

//! Reads optional attribute of name @a atrname and allocates variable of the same name.
/*! Requires the following line to be present above:
    @code
    const QXmlStreamAttributes attrs( attributes() );
    @endcode
*/
#define TRY_READ_ATTR(atrname) \
    QString atrname( attrs.value(QUALIFIED_NAME(atrname)).toString() );

//! Reads optional attribute of name @a atrname into the variable @a destination.
/*! Requires the following line to be present above:
    @code
    const QXmlStreamAttributes attrs( attributes() );
    @endcode
*/
#define TRY_READ_ATTR_INTO(atrname, destination) \
    destination = attrs.value(QUALIFIED_NAME(atrname)).toString(); \
    /*kDebug() << "TRY_READ_ATTR_INTO: " STRINGIFY(destination) << "=" << destination;*/

//! Reads optional attribute of name @a atrname with explicitly specified namespace @a ns.
/*! Creates QString variable with name \<ns\>_\<atrame\>
*/
/*! Requires the following line to be present above:
    @code
    const QXmlStreamAttributes attrs( attributes() );
    @endcode
*/
#define TRY_READ_ATTR_WITH_NS(ns, atrname) \
    QString PASTE3(ns, _, atrname)( attrs.value(JOIN(STRINGIFY(ns) ":", atrname)).toString() );

//! Reads optional attribute of name @a atrname with explicitly specified namespace @a ns
//! into the variable @a destination.
/*! Requires the following line to be present above:
    @code
    const QXmlStreamAttributes attrs( attributes() );
    @endcode
*/
#define TRY_READ_ATTR_WITH_NS_INTO(ns, atrname, destination) \
    destination = attrs.value(JOIN(STRINGIFY(ns) ":", atrname)).toString(); \
    /*kDebug() << "TRY_READ_ATTR_WITH_NS_INTO: " STRINGIFY(destination) << "=" << destination;*/

inline QString atrToString(const QXmlStreamAttributes& attrs, const char* atrname)
{
    const QStringRef v(attrs.value(atrname));
    return v.isNull() ? QString() : v.toString();
}

//! Reads optional attribute of name @a atrname without namespace.
/*! Creates QString variable with name \<atrname\>
*/
/*! Requires the following line to be present above:
    @code
    const QXmlStreamAttributes attrs( attributes() );
    @endcode
*/
#define TRY_READ_ATTR_WITHOUT_NS(atrname) \
    QString atrname( atrToString(attrs, STRINGIFY(atrname)) );

//! Reads required attribute of name @a atrname and allocates variable of the same name
//! If there is no such attribute, returns KoFilter::WrongFormat.
/*! Requires the following line to be present above:
    @code
    const QXmlStreamAttributes attrs( attributes() );
    @endcode
*/
#define READ_ATTR(atrname) \
    QString atrname; \
    if (attrs.hasAttribute(QUALIFIED_NAME(atrname))) { \
        atrname = attrs.value(QUALIFIED_NAME(atrname)).toString(); \
    } \
    ELSE_WRONG_FORMAT_DEBUG( "READ_ATTR: " QUALIFIED_NAME(atrname) " not found" )

//! Like @ref READ_ATTR(atrname) but reads the attribute into the variable @a destination.
#define READ_ATTR_INTO(atrname, destination) \
    if (attrs.hasAttribute(QUALIFIED_NAME(atrname))) { \
        destination = attrs.value(QUALIFIED_NAME(atrname)).toString(); \
    } \
    ELSE_WRONG_FORMAT_DEBUG( "READ_ATTR_INTO: " QUALIFIED_NAME(atrname) " not found" )

//! Reads required attribute of name @a atrname with explicitly specified namespace @a ns
/*! into the variable @a destination.
*/
/*! Requires the following line to be present above:
    @code
    const QXmlStreamAttributes attrs( attributes() );
    @endcode
*/
#define READ_ATTR_WITH_NS_INTO(ns, atrname, destination) \
    if (attrs.hasAttribute(JOIN(STRINGIFY(ns) ":", atrname))) { \
        destination = attrs.value(JOIN(STRINGIFY(ns) ":", atrname)).toString(); \
    } \
    ELSE_WRONG_FORMAT_DEBUG( "READ_ATTR_WITH_NS_INTO: " JOIN(STRINGIFY(ns) ":", atrname) " not found" )

//! Reads required attribute of name @a atrname with explicitly specified namespace @a ns.
/*! Creates QString variable with name \<ns\>_\<atrame\>
*/
/*! Requires the following line to be present above:
    @code
    const QXmlStreamAttributes attrs( attributes() );
    @endcode
*/
#define READ_ATTR_WITH_NS(ns, atrname) \
    QString PASTE3(ns, _, atrname); \
    if (attrs.hasAttribute(JOIN(STRINGIFY(ns) ":", atrname))) { \
        PASTE3(ns, _, atrname) = attrs.value(JOIN(STRINGIFY(ns) ":", atrname)).toString(); \
    } \
    ELSE_WRONG_FORMAT_DEBUG( "READ_ATTR_WITH_NS: " JOIN(STRINGIFY(ns) ":", atrname) " not found" )

//! Reads required attribute of name @a atrname without namespace.
/*! Creates QString variable with name \<atrname\>
*/
/*! Requires the following line to be present above:
    @code
    const QXmlStreamAttributes attrs( attributes() );
    @endcode
*/
#define READ_ATTR_WITHOUT_NS(atrname) \
    QString atrname; \
    if (attrs.hasAttribute(STRINGIFY(atrname))) { \
        atrname = attrs.value(STRINGIFY(atrname)).toString(); \
    } \
    ELSE_WRONG_FORMAT_DEBUG( "READ_ATTR_WITHOUT_NS: " STRINGIFY(atrname) " not found" )

//! Like @ref READ_ATTR_WITHOUT_NS(atrname) but reads the attribute into the variable @a destination.
#define READ_ATTR_WITHOUT_NS_INTO(atrname, destination) \
    if (attrs.hasAttribute(STRINGIFY(atrname))) { \
        destination = attrs.value(STRINGIFY(atrname)).toString(); \
    } \
    ELSE_WRONG_FORMAT_DEBUG( "READ_ATTR_WITHOUT_NS_INTO: " STRINGIFY(atrname) " not found" )


/*! Requires the following line to be present above:
    @code
    const QXmlStreamAttributes attrs( attributes() );
    @endcode
*/
#define TRY_READ_ATTR_QSTRING(atrname) \
    QString atrname( attrs.value(m_defaultNamespace + atrname).toString() );

//! Like @ref TRY_READ_ATTR_WITHOUT_NS(atrname) but reads the attribute into the variable @a destination.
#define TRY_READ_ATTR_WITHOUT_NS_INTO(atrname, destination) \
    destination = attrs.value(STRINGIFY(atrname)).toString();

//! reads boolean attribute "val" prefixed with default namespace, defaults to true
#define READ_BOOLEAN_VAL \
    readBooleanAttr(QUALIFIED_NAME(val), true)

//! Converts @a string into integer @a destination; returns KoFilter::WrongFormat on failure.
//! @warning @a destination is left unchanged if @a string is empty, so it is up to developer to initialize it.
#define STRING_TO_INT(string, destination, debugElement) \
    if (string.isEmpty()) {} else { \
        bool ok; \
        const int val_tmp = string.toInt(&ok); \
        if (!ok) { \
            kDebug() << "STRING_TO_INT: error converting" << string << "to int (attribute" << debugElement << ")"; \
            return KoFilter::WrongFormat; \
        } \
        destination = val_tmp; \
    }

//! Converts @a string into a qreal value in @a destination; returns KoFilter::WrongFormat on failure.
//! @warning @a destination is left unchanged if @a string is empty, so it is up to developer to initialize it.
#define STRING_TO_QREAL(string, destination, debugElement) \
    if (string.isEmpty()) {} else { \
        bool ok; \
        const qreal val_tmp = string.toDouble(&ok); \
        if (!ok) { \
            kDebug() << "STRING_TO_DOUBLE: error converting" << string << "to qreal (attribute" << debugElement << ")"; \
            return KoFilter::WrongFormat; \
        } \
        destination = val_tmp; \
    }

//! Skips everything until end of CURRENT_EL is pulled
#define SKIP_EVERYTHING \
    /*kDebug() << "Skipping everything in element" << qualifiedName() << "...";*/ \
    const QString qn(qualifiedName().toString()); \
    /*kDebug() << *this; */\
    while (true) { \
        if (atEnd()) \
            break; \
        if (isEndElement() && qualifiedName() == qn) { \
            break; \
        } \
        readNext(); \
    }

#define SKIP_EVERYTHING_AND_RETURN \
    SKIP_EVERYTHING \
    return KoFilter::OK;

#define BIND_READ_METHOD(name, method) \
    m_readMethods.insert(QLatin1String(name), &MSOOXML_CURRENT_CLASS::read_ ## method);

#define BIND_READ(name) \
    BIND_READ_METHOD(STRINGIFY(name), name)

#define BIND_READ_SKIP(name) \
    BIND_READ_METHOD(STRINGIFY(name), SKIP)

#define BIND_READ_METHOD_HASH(hash, name, method) \
    hash.insert(QLatin1String(name), &MSOOXML_CURRENT_CLASS::read_ ## method);

#define BIND_READ_HASH(hash, name) \
    BIND_READ_METHOD_HASH(hash, STRINGIFY(name), name)

#define BIND_READ_HASH_SKIP(hash, name) \
    BIND_READ_METHOD_HASH(hash, STRINGIFY(name), SKIP)

#endif
