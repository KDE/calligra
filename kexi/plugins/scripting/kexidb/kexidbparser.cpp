/***************************************************************************
 * kexidbparser.cpp
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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/


#include "kexidbparser.h"
#include "kexidbschema.h"
#include "kexidbconnection.h"

#include <api/variant.h>

using namespace Kross::KexiDB;

KexiDBParser::KexiDBParser(KexiDBConnection* connection, ::KexiDB::Parser* parser)
    : Kross::Api::Class<KexiDBParser>("KexiDBParser")
    , m_connection(connection)
    , m_parser(parser)
{
    addFunction("parse", &KexiDBParser::parse);
    addFunction("clear", &KexiDBParser::clear);
    addFunction("operation", &KexiDBParser::operation);

    addFunction("table", &KexiDBParser::table);
    addFunction("query", &KexiDBParser::query);
    addFunction("connection", &KexiDBParser::connection);
    addFunction("statement", &KexiDBParser::statement);

    addFunction("errorType", &KexiDBParser::errorType);
    addFunction("errorMsg", &KexiDBParser::errorMsg);
    addFunction("errorAt", &KexiDBParser::errorAt);
}

KexiDBParser::~KexiDBParser()
{
}

const QString KexiDBParser::getClassName() const
{
    return "Kross::KexiDB::KexiDBParser";
}

Kross::Api::Object::Ptr KexiDBParser::parse(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           QVariant(m_parser->parse(Kross::Api::Variant::toString(args->item(0))),0));
}

Kross::Api::Object::Ptr KexiDBParser::clear(Kross::Api::List::Ptr)
{
    m_parser->clear();
    return 0;
}

Kross::Api::Object::Ptr KexiDBParser::operation(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_parser->operationString());
    return 0;
}

Kross::Api::Object::Ptr KexiDBParser::table(Kross::Api::List::Ptr)
{
    ::KexiDB::TableSchema* t = m_parser->table();
    if(! t) return 0;
    return new KexiDBTableSchema(t);
}

Kross::Api::Object::Ptr KexiDBParser::query(Kross::Api::List::Ptr)
{
    ::KexiDB::QuerySchema* q = m_parser->query();
    if(! q) return 0;
    return new KexiDBQuerySchema(q);
}

Kross::Api::Object::Ptr KexiDBParser::connection(Kross::Api::List::Ptr)
{
    return m_connection;
}

Kross::Api::Object::Ptr KexiDBParser::statement(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_parser->statement());
}

Kross::Api::Object::Ptr KexiDBParser::errorType(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_parser->error().type());
}

Kross::Api::Object::Ptr KexiDBParser::errorMsg(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_parser->error().error());
}

Kross::Api::Object::Ptr KexiDBParser::errorAt(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_parser->error().at());
}

