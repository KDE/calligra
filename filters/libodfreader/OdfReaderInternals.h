/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012-2014 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ODFREADERINTERNALS_H
#define ODFREADERINTERNALS_H

// ----------------------------------------------------------------
//                     Reader functions

#define DECLARE_READER_FUNCTION(name) void readElement##name(KoXmlStreamReader &reader)

#define IMPLEMENT_READER_FUNCTION_START(readername, name)                                                                                                      \
    void readername::readElement##name(KoXmlStreamReader &reader)                                                                                              \
    {                                                                                                                                                          \
        DEBUGSTART();                                                                                                                                          \
        m_backend->element##name(reader, m_context);

#define IMPLEMENT_READER_FUNCTION_END(name)                                                                                                                    \
    m_backend->element##name(reader, m_context);                                                                                                               \
    DEBUGEND();                                                                                                                                                \
    }

#define IMPLEMENT_READER_FUNCTION_NO_CHILDREN(readername, name)                                                                                                \
    IMPLEMENT_READER_FUNCTION_START(readername, name)                                                                                                          \
    reader.skipCurrentElement();                                                                                                                               \
    IMPLEMENT_READER_FUNCTION_END(name)

#define IMPLEMENT_READER_FUNCTION_ONE_CHILD(readername, name, element, childfunction)                                                                          \
    IMPLEMENT_READER_FUNCTION_START(readername, name)                                                                                                          \
    while (reader.readNextStartElement()) {                                                                                                                    \
        QString tagName = reader.qualifiedName().toString();                                                                                                   \
                                                                                                                                                               \
        if (tagName == element) {                                                                                                                              \
            readElement##childfunction(reader);                                                                                                                \
        } else {                                                                                                                                               \
            reader.skipCurrentElement();                                                                                                                       \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
    IMPLEMENT_READER_FUNCTION_END(name)

// ----------------------------------------------------------------
//                     Backend functions

#define DECLARE_BACKEND_FUNCTION(name) virtual void element##name(KoXmlStreamReader &reader, OdfReaderContext *context)

#define IMPLEMENT_BACKEND_FUNCTION(readername, name)                                                                                                           \
    void readername##Backend::element##name(KoXmlStreamReader &reader, OdfReaderContext *context)                                                              \
    {                                                                                                                                                          \
        Q_UNUSED(reader);                                                                                                                                      \
        Q_UNUSED(context);                                                                                                                                     \
    }

#endif // ODFREADERINTERNALS_H
