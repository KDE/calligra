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
#include <QIODevice>
#include <QString>
#include <KFilterDev>
#include <kdebug.h>
using namespace Strigi;
using namespace std;
class AbiwordThroughAnalyzerFactory;
class AbiwordThroughAnalyzer : public StreamThroughAnalyzer {
    private:
        const AbiwordThroughAnalyzerFactory* factory;
        AnalysisResult* idx;

        void setIndexable( AnalysisResult *i ) {
            idx = i;
        }
        InputStream* connectInputStream( InputStream *in );
        bool isReadyWithStream() { return true; }
    public:
        AbiwordThroughAnalyzer( const AbiwordThroughAnalyzerFactory* f ) : factory( f ) {}
};

class AbiwordThroughAnalyzerFactory : public StreamThroughAnalyzerFactory {
private:
    const char* name() const {
        return "AbiwordThroughAnalyzer";
    }
    StreamThroughAnalyzer* newInstance() const {
        return new AbiwordThroughAnalyzer(this);
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

const std::string AbiwordThroughAnalyzerFactory::authorFieldName( "content.author" );
const std::string AbiwordThroughAnalyzerFactory::titleFieldName( "content.title" );
const std::string AbiwordThroughAnalyzerFactory::abstractFieldName( "abstract" );

void AbiwordThroughAnalyzerFactory::registerFields( FieldRegister& reg ) {
    authorField = reg.registerField( authorFieldName, FieldRegister::stringType, 1, 0 );
    titleField = reg.registerField( titleFieldName, FieldRegister::stringType, 1, 0 );
    abstractField = reg.registerField( abstractFieldName, FieldRegister::stringType, 1, 0 );
}

InputStream* AbiwordThroughAnalyzer::connectInputStream( InputStream* input ) {
    if( !input )
	return input;
    const string& path = idx->path();
    QString f(path.c_str());

    //Find the last extension
    QString strExt;
    const int result=f.lastIndexOf('.');
    if (result>=0)
    {
        strExt=f.mid(result);
    }
    QString strMime; // Mime type of the compressor (default: unknown)
    if ((strExt==".gz")||(strExt==".GZ")        //in case of .abw.gz (logical extension)
        ||(strExt==".zabw")||(strExt==".ZABW")) //in case of .zabw (extension used prioritary with AbiWord)
    {
        // Compressed with gzip
        strMime="application/x-gzip";
    }
    else if ((strExt==".bz2")||(strExt==".BZ2") //in case of .abw.bz2 (logical extension)
        ||(strExt==".bzabw")||(strExt==".BZABW")) //in case of .bzabw (extension used prioritary with AbiWord)
    {
        // Compressed with bzip2
        strMime="application/x-bzip";
    }

    QIODevice* in = KFilterDev::deviceForFile(f,strMime);
    if ( !in )
    {
        kError() << "Cannot create device for uncompressing! Aborting!" << endl;
        return false;
    }

    if (!in->open(QIODevice::ReadOnly))
    {
        kError() << "Cannot open file for uncompressing! Aborting!" << endl;
        delete in;
        return false;
    }
    QDomDocument doc;
    doc.setContent( in );
    in->close();
    QDomElement docElem = doc.documentElement();
    QDomNode summary  = docElem.namedItem("metadata");
    QDomNode m_item = summary.namedItem("m");

    QString author;
    QString title;
    QString abstract;

    while( !m_item.isNull() )
    {
        QString key = m_item.toElement().attribute( "key" );
        if ( key.isEmpty() )
            continue;
        else if ( key=="dc.creator" )
            author=m_item.toElement().text();
        else if ( key=="dc.description" )
            abstract=m_item.toElement().text();
        else if ( key=="dc.title" )
            title=m_item.toElement().text();
        else
	{
            kDebug()<<" Other key :"<<key<<endl;
	}
        m_item = m_item.nextSibling();
    }
    // set author information
    idx->addValue( factory->authorField, (const char*)author.toUtf8());

    // set title information
    idx->addValue( factory->titleField, (const char*)title.toUtf8());

    // set abstract information
    idx->addValue( factory->abstractField, (const char*)abstract.toUtf8());

    delete in;
    return input;
}

class Factory : public AnalyzerFactoryFactory {
public:
    std::list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        std::list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new AbiwordThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory) 
