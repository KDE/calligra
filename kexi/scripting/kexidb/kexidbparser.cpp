/***************************************************************************
 * kexidbparser.cpp
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


#include "kexidbparser.h"
#include "kexidbschema.h"
#include "../api/variant.h"

#include <klocale.h>
//#include <kdebug.h>

using namespace Kross::KexiDB;

KexiDBParser::KexiDBParser(::KexiDB::Parser* parser)
    : Kross::Api::Class<KexiDBParser>("KexiDBParser")
    , m_parser(parser)
{
    addFunction("parse", &KexiDBParser::parse,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Clears previous results and runs the parser on the as "
             "argument passed sql statement.")
    );
    addFunction("clear", &KexiDBParser::clear,
        Kross::Api::ArgumentList(),
        i18n("Clear results.")
    );
    addFunction("operation", &KexiDBParser::operation,
        Kross::Api::ArgumentList(),
        i18n("Return the resulting operation.")
    );
    addFunction("table", &KexiDBParser::table,
        Kross::Api::ArgumentList(),
        i18n("Return the KexiDBTableSchema object on a CREATE TABLE operation.")
    );
    addFunction("query", &KexiDBParser::query,
        Kross::Api::ArgumentList(),
        i18n("Return the KexiDBQuerySchema object on a SELECT operation.")
    );
    addFunction("connection", &KexiDBParser::connection,
        Kross::Api::ArgumentList(),
        i18n("Return the KexiDBConnection object pointing to the used "
             "database connection.")
    );
    addFunction("statement", &KexiDBParser::statement,
        Kross::Api::ArgumentList(),
        i18n("Return the sql query statement.")
    );

    addFunction("errorType", &KexiDBParser::errorType,
        Kross::Api::ArgumentList(),
        i18n("Return the type string of the last error.")
    );
    addFunction("errorMsg", &KexiDBParser::errorMsg,
        Kross::Api::ArgumentList(),
        i18n("Return the message of the last error.")
    );
    addFunction("errorAt", &KexiDBParser::errorAt,
        Kross::Api::ArgumentList(),
        i18n("Return the position where the last error occured.")
    );
}

KexiDBParser::~KexiDBParser()
{
}

const QString KexiDBParser::getClassName() const
{
    return "Kross::KexiDB::KexiDBParser";
}

const QString KexiDBParser::getDescription() const
{
    return i18n("KexiDB::Parser wrapper to parse sql statements.");
}

Kross::Api::Object* KexiDBParser::parse(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
           m_parser->parse(Kross::Api::Variant::toString(args->item(0))),
           "Kross::Api::KexiDBParser::parse::Bool");
}

Kross::Api::Object* KexiDBParser::clear(Kross::Api::List*)
{
    m_parser->clear();
    return 0;
}

Kross::Api::Object* KexiDBParser::operation(Kross::Api::List*)
{
    /*TODO
    return Kross::Api::Variant::create(m_parser->operationString(),
           "Kross::Api::KexiDBParser::operation::String");
    */
    return 0;
}

Kross::Api::Object* KexiDBParser::table(Kross::Api::List*)
{
    ::KexiDB::TableSchema* t = m_parser->table();
    if(! t) return 0;
    return new KexiDBTableSchema(t);
}

Kross::Api::Object* KexiDBParser::query(Kross::Api::List*)
{
    ::KexiDB::QuerySchema* q = m_parser->query();
    if(! q) return 0;
    return new KexiDBQuerySchema(q);
}

Kross::Api::Object* KexiDBParser::connection(Kross::Api::List*)
{
    /*TODO
    ::KexiDB::Connection* c = m_parser->db();
    if(! c) return 0;
    return new KexiDBConnection(c);
    */
    return 0;
}

Kross::Api::Object* KexiDBParser::statement(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_parser->statement(),
           "Kross::Api::KexiDBParser::statement::String");
}

Kross::Api::Object* KexiDBParser::errorType(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_parser->error().type(),
           "Kross::Api::KexiDBParser::errorType::String");
}

Kross::Api::Object* KexiDBParser::errorMsg(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_parser->error().error(),
           "Kross::Api::KexiDBParser::errorMsg::String");
}

Kross::Api::Object* KexiDBParser::errorAt(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_parser->error().at(),
           "Kross::Api::KexiDBParser::errorAt::Int");
}

