/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#define TRY_READ_WITH_ARGS_INTERNAL(name, args) \
    args \
    RETURN_IF_ERROR( read_ ## name () )

#define TRY_READ_WITH_ARGS(name, args) \
    TRY_READ_WITH_ARGS_INTERNAL(name, m_read_ ## name ## _args = args)

#define TRY_READ(name) \
    TRY_READ_WITH_ARGS_INTERNAL(name,)

#define PASTE2(a, b) a##b
#define PASTE(a, b) PASTE2( a, b) // indirection needed because only function-like macro parameters can be pasted

#define PASTE3_(a, b, c) a##b##c
#define PASTE3(a, b, c) PASTE3_( a, b, c) // indirection needed because only function-like macro parameters can be pasted

#define JOIN2(a, b) a#b
#define JOIN(a, b) JOIN2( a, b) // indirection needed because only function-like macro parameters can be pasted

#define STRINGIFY(s) JOIN("", s)

#ifdef MSOOXML_CURRENT_NS
# define QUALIFIED_NAME(name) \
    JOIN(MSOOXML_CURRENT_NS ":",name)
#else
# define QUALIFIED_NAME(name) \
    STRINGIFY(name)
#endif

#ifndef NDEBUG
# define PUSH_NAME \
    kDebug() << (m_callsNames.isEmpty() ? QByteArray("top level") : m_callsNames.top()).constData() \
    << "==>" << STRINGIFY(CURRENT_EL); \
    m_callsNames.push(STRINGIFY(CURRENT_EL));
# define POP_NAME \
    m_callsNames.pop(); \
    kDebug() << (m_callsNames.isEmpty() ? QByteArray("top level") : m_callsNames.top()).constData() \
    << "<==" << STRINGIFY(CURRENT_EL);
#else
# define PUSH_NAME
# define POP_NAME
#endif

#define READ_PROLOGUE \
    m_calls.push(PASTE(&MSOOXML_CURRENT_CLASS::read_, CURRENT_EL)); \
    PUSH_NAME \
    /*kDebug() << *this;*/ \
    if (!expectEl(QUALIFIED_NAME(CURRENT_EL))) { \
        return KoFilter::WrongFormat; \
    }

#define READ_EPILOGUE_WITHOUT_RETURN \
    m_calls.pop(); \
    POP_NAME \
    kDebug() << "READ_EPILOGUE_WITHOUT_RETURN"; \
    if (!expectElEnd(QUALIFIED_NAME(CURRENT_EL))) { \
        kDebug() << "READ_EPILOGUE:" << QUALIFIED_NAME(CURRENT_EL) << "not found!"; \
        return KoFilter::WrongFormat; \
    } \
    kDebug() << "/READ_EPILOGUE_WITHOUT_RETURN";

#define READ_EPILOGUE \
    READ_EPILOGUE_WITHOUT_RETURN \
    return KoFilter::OK;

#define BREAK_IF_END_OF_QSTRING(name) \
    kDebug() << "BREAK_IF_END_OF" << name << "found:" << qualifiedName(); \
    if (isEndElement() && qualifiedName() == name) { \
        break; \
    }

#define BREAK_IF_END_OF(name) \
    BREAK_IF_END_OF_QSTRING(QLatin1String(QUALIFIED_NAME(name)))

//inline bool aaaa(const char * aa) { kDebug() << "aa" << aa; return true; }

#define QUALIFIED_NAME_IS(name) \
    (/*aaaa(STRINGIFY(name)) &&*/ qualifiedName() == QLatin1String(QUALIFIED_NAME(name)))

#define TRY_READ_IF(name) \
    if (QUALIFIED_NAME_IS(name)) { \
        kDebug() << "TRY_READ_IF " STRINGIFY(name) " started"; \
        TRY_READ(name); \
        kDebug() << "TRY_READ_IF " STRINGIFY(name) " finished"; \
    }

#define ELSE_TRY_READ_IF(name) \
    else TRY_READ_IF(name)

#define TRY_READ_IF_NS(ns, name) \
    if (qualifiedName() == QLatin1String(JOIN(STRINGIFY(ns) ":", name))) { \
        kDebug() << "TRY_READ_IF_NS " JOIN(STRINGIFY(ns) ":", name) " started"; \
        TRY_READ(name); \
        kDebug() << "TRY_READ_IF_NS " JOIN(STRINGIFY(ns) ":", name) " finished"; \
    }

#define ELSE_TRY_READ_IF_NS(ns, name) \
    else TRY_READ_IF_NS(ns, name)

#define ELSE_WRONG_FORMAT \
    else { \
        return KoFilter::WrongFormat; \
    }

#define ELSE_WRONG_FORMAT_DEBUG(dbg) \
    else { \
        kDebug() << dbg; \
        return KoFilter::WrongFormat; \
    }

#define ERROR_NO_ELEMENT(el) \
    raiseError(i18n("Element \"%1\" not found", QString(el))); \
    return KoFilter::WrongFormat;

#define ERROR_UNEXPECTED_SECOND_OCCURENCE(el) \
    raiseError(i18n("Unexpected second occurence of \"%1\" element", QLatin1String(STRINGIFY(el)))); \
    return KoFilter::WrongFormat;

#define ERROR_NO_ATTRIBUTE(attr) \
    raiseError(i18n("Attribute \"%1\" not found", QString(attr))); \
    return KoFilter::WrongFormat;

//! Reads optional attribute of name @a atrname and allocates variable of the same name.
/*! Requires the following line to be present above:
    @code
    const QXmlStreamAttributes attrs( attributes() );
    @endcode
*/
#define TRY_READ_ATTR(atrname) \
    QString atrname( attrs.value(QUALIFIED_NAME(atrname)).toString() );

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
    kDebug() << "TRY_READ_ATTR_WITH_NS_INTO: " STRINGIFY(destination) << "=" << destination;

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
    ELSE_WRONG_FORMAT

//! Like @ref READ_ATTR(atrname) but reads the attribute into the variable @a destination.
#define READ_ATTR_INTO(atrname, destination) \
    if (attrs.hasAttribute(QUALIFIED_NAME(atrname))) { \
        destination = attrs.value(QUALIFIED_NAME(atrname)).toString(); \
    } \
    ELSE_WRONG_FORMAT

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
    ELSE_WRONG_FORMAT

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
    ELSE_WRONG_FORMAT

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
    ELSE_WRONG_FORMAT

//! Like @ref READ_ATTR_WITHOUT_NS(atrname) but reads the attribute into the variable @a destination.
#define READ_ATTR_WITHOUT_NS_INTO(atrname, destination) \
    if (attrs.hasAttribute(STRINGIFY(atrname))) { \
        destination = attrs.value(STRINGIFY(atrname)).toString(); \
    } \
    ELSE_WRONG_FORMAT


/*! Requires the following line to be present above:
    @code
    const QXmlStreamAttributes attrs( attributes() );
    @endcode
*/
#define TRY_READ_ATTR_QSTRING(atrname) \
    QString atrname( attrs.value(m_defaultNamespace + atrname).toString() );

////! Like @ref READ_ATTR_WITHOUT_NS(atrname) but reads the attribute into the variable @a destination.
//#define TRY_READ_ATTR_WITH_NS_INTO(ns, atrname, destination)
//    destination = attrs.value(JOIN(STRINGIFY(ns) ":", atrname)).toString();

//! Like @ref TRY_READ_ATTR_WITHOUT_NS(atrname) but reads the attribute into the variable @a destination.
#define TRY_READ_ATTR_WITHOUT_NS_INTO(atrname, destination) \
    destination = attrs.value(STRINGIFY(atrname)).toString();

#define READ_BOOLEAN_ATTR \
    readBooleanAttr(QUALIFIED_NAME(CURRENT_EL))

//! converts @a string into integer @a destination; returns KoFilter::WrongFormat on failure
#define STRING_TO_INT(string, destination, debugElement) \
    if (string.isEmpty()) {} else { \
        bool ok; \
        const int val = string.toInt(&ok); \
        if (!ok) { \
            kDebug() << "STRING_TO_INT: error converting" << string << "to int (attribute" << debugElement << ")"; \
            return KoFilter::WrongFormat; \
        } \
        destination = val; \
    }

#define STRING_TO_QREAL(string, destination, debugElement) \
    if (string.isEmpty()) {} else { \
        bool ok; \
        const qreal val = string.toDouble(&ok); \
        if (!ok) { \
            kDebug() << "STRING_TO_DOUBLE: error converting" << string << "to qreal (attribute" << debugElement << ")"; \
            return KoFilter::WrongFormat; \
        } \
        destination = val; \
    }

//! Creates condition that checks what's the calling method (what means parent element)
//! Example use:
/*! @code
    ... read_foo()
    {
      ReadMethod caller = m_calls.top(); // <-- needed
      ...
      if (CALLER_IS(r)) {
        ...
      }
    }
    @endcode
*/
#define CALLER_IS(name) \
    (caller == PASTE(&MSOOXML_CURRENT_CLASS::read_, name))

//! Skips everything until end of CURRENT_EL is pulled
#define SKIP_EVERYTHING \
    kDebug() << "Skipping everything in element" << qualifiedName() << "..."; \
    const QString qn(qualifiedName().toString()); \
    kDebug() << *this; \
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
