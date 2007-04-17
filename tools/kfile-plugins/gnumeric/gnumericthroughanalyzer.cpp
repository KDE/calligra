/* This file is part of the KDE project
 * Copyright (C) 2007 Montel Laurent <montel@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#define STRIGI_IMPORT_API
#include <strigi/streamthroughanalyzer.h>
#include <strigi/analyzerplugin.h>
#include <strigi/fieldtypes.h>
#include <strigi/analysisresult.h>

#include <QDomDocument>

using namespace Strigi;

class GnumericThroughAnalyzerFactory;
class GnumericThroughAnalyzer : public StreamThroughAnalyzer {
    private:
        const GnumericThroughAnalyzerFactory* factory;
        AnalysisResult* idx;

        void setIndexable( AnalysisResult *i ) {
            idx = i;
        }
        InputStream* connectInputStream( InputStream *in );
        bool isReadyWithStream() { return true; }
    public:
        GnumericThroughAnalyzer( const GnumericThroughAnalyzerFactory* f ) : factory( f ) {}
};

class GnumericThroughAnalyzerFactory : public StreamThroughAnalyzerFactory {
private:
    const char* name() const {
        return "GnumericThroughAnalyzer";
    }
    StreamThroughAnalyzer* newInstance() const {
        return new GnumericThroughAnalyzer(this);
    }
    void registerFields( FieldRegister& );

    static const std::string authorFieldName;
    static const std::string titleFieldName;
    static const std::string abstractFieldName;
public:
    const RegisteredField* authorField;
    const RegisteredField* titleField;
    const RegisteredField* abstractField;
};

const std::string GnumericThroughAnalyzerFactory::authorFieldName( "content.author" );
const std::string GnumericThroughAnalyzerFactory::titleFieldName( "content.title" );
const std::string GnumericThroughAnalyzerFactory::abstractFieldName( "abstract" );

void GnumericThroughAnalyzerFactory::registerFields( FieldRegister& reg ) {
    authorField = reg.registerField( authorFieldName, FieldRegister::stringType, 1, 0 );
    titleField = reg.registerField( titleFieldName, FieldRegister::stringType, 1, 0 );
    abstractField = reg.registerField( abstractFieldName, FieldRegister::stringType, 1, 0 );
}

InputStream* GnumericThroughAnalyzer::connectInputStream( InputStream* in ) {
    if( !in )
        return in;

    const char *c;
    int nread = in->read( c, in->size(), in->size() );
    in->reset( 0 );
    if( nread == -2 )
        return in;

    QDomDocument doc;
    if( !doc.setContent( QByteArray( c, in->size() ) ) )
        return in;

    QDomElement docElem = doc.documentElement();
    QDomNode summary  = docElem.namedItem("gmr:Summary");
    QDomNode gmr_item = summary.namedItem("gmr:Item");

    QString author;
    QString title;
    QString abstract;

    while( !gmr_item.isNull() )
    {
        QDomNode gmr_name  = gmr_item.namedItem("gmr:name");
        QDomNode gmr_value = gmr_item.namedItem("gmr:val-string");
        if (gmr_name.toElement().text() == "title")
        {
            title=gmr_value.toElement().text();
        }
        else if (gmr_name.toElement().text() == "author")
        {
            author=gmr_value.toElement().text();
        }
        else if (gmr_name.toElement().text() == "comments")
        {
            abstract=gmr_value.toElement().text();
        }
        gmr_item = gmr_item.nextSibling();
    }


    // set author information
    idx->addValue( factory->authorField, (const char*)author.toUtf8());

    // set title information
    idx->addValue( factory->titleField, (const char*)title.toUtf8());

    // set abstract information
    idx->addValue( factory->abstractField, (const char*)abstract.toUtf8());

    return in;
}

class Factory : public AnalyzerFactoryFactory {
public:
    std::list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        std::list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new GnumericThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory) 
