/* This file is part of the KDE project
 * Copyright (C) 2002 Simon MacMullen <koffice@babysimon.co.uk>
 * Copyright (C) 2007 Matthias Lechner <matthias@lmme.de>
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
#include <strigi/cnstr.h>

#include <QDomDocument>

using namespace Strigi;

class KOfficeThroughAnalyzerFactory;
class KOfficeThroughAnalyzer : public StreamThroughAnalyzer {
    private:
        const KOfficeThroughAnalyzerFactory* factory;
        AnalysisResult* idx;

        void setIndexable( AnalysisResult *i ) {
            idx = i;
        }
        InputStream* connectInputStream( InputStream *in );
        bool isReadyWithStream() { return true; }
    public:
        KOfficeThroughAnalyzer( const KOfficeThroughAnalyzerFactory* f ) : factory( f ) {}
};

class KOfficeThroughAnalyzerFactory : public StreamThroughAnalyzerFactory {
private:
    const char* name() const {
        return "KOfficeThroughAnalyzer";
    }
    StreamThroughAnalyzer* newInstance() const {
        return new KOfficeThroughAnalyzer(this);
    }
    void registerFields( FieldRegister& );

    static const cnstr authorFieldName;
    static const cnstr titleFieldName;
    static const cnstr keywordsFieldName;
    static const cnstr abstractFieldName;
    static const cnstr editingCyclesFieldName;
public:
    const RegisteredField* authorField;
    const RegisteredField* titleField;
    const RegisteredField* keywordsField;
    const RegisteredField* abstractField;
    const RegisteredField* editingCyclesField;
};

const cnstr KOfficeThroughAnalyzerFactory::authorFieldName( "author" );
const cnstr KOfficeThroughAnalyzerFactory::titleFieldName( "title" );
const cnstr KOfficeThroughAnalyzerFactory::keywordsFieldName( "keywords" );
const cnstr KOfficeThroughAnalyzerFactory::abstractFieldName( "abstract" );
const cnstr KOfficeThroughAnalyzerFactory::editingCyclesFieldName( "editing cycles" );

void KOfficeThroughAnalyzerFactory::registerFields( FieldRegister& reg ) {
    authorField = reg.registerField( authorFieldName, FieldRegister::stringType, 1, 0 );
    titleField = reg.registerField( titleFieldName, FieldRegister::stringType, 1, 0 );
    keywordsField = reg.registerField( keywordsFieldName, FieldRegister::stringType, 1, 0 );
    abstractField = reg.registerField( abstractFieldName, FieldRegister::stringType, 1, 0 );
    editingCyclesField = reg.registerField( editingCyclesFieldName, FieldRegister::integerType, 1, 0 );
}

InputStream* KOfficeThroughAnalyzer::connectInputStream( InputStream* in ) {
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

    // check if this is a koffice document
    QDomNode rootNode = doc.documentElement();
    if( rootNode.nodeName() != "document-info" )
        return in;

    QDomNode aboutNode = doc.namedItem("document-info").namedItem("about");
    QDomNode authorNode = doc.namedItem("document-info").namedItem("author");

    // set author information
    idx->addValue( factory->authorField,
                   (const char*) authorNode.namedItem( "full-name" ).toElement().text().toUtf8() );

    // set title information
    idx->addValue( factory->titleField,
                   (const char*) authorNode.namedItem( "title" ).toElement().text().toUtf8() );

    // set keyword information
    idx->addValue( factory->keywordsField,
                   (const char*) aboutNode.namedItem( "keyword" ).toElement().text().toUtf8() );

    // set abstract information
    idx->addValue( factory->abstractField, 
                   (const char*) authorNode.namedItem( "abstract" ).toElement().text().toUtf8() );

    // set editing cycles information
    idx->addValue( factory->editingCyclesField, 
                   aboutNode.namedItem( "editing-cycles" ).toElement().text().toInt() );

    return in;
}

class Factory : public AnalyzerFactoryFactory {
public:
    std::list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        std::list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new KOfficeThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory) 
