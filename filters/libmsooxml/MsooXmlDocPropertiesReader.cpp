#include "MsooXmlDocPropertiesReader.h"
#include "MsooXmlSchemas.h"
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>

#define MSOOXML_CURRENT_NS "cp"
#define MSOOXML_CURRENT_CLASS MsooXmlDocPropertiesReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include "MsooXmlReader_p.h"

using namespace MSOOXML;

MsooXmlDocPropertiesReader::MsooXmlDocPropertiesReader(KoOdfWriters* writers) : MsooXmlReader(writers)
{
    elemMap.insert(QLatin1String("dc:creator"), QLatin1String("dc:creator"));
    elemMap.insert(QLatin1String("dc:description"), QLatin1String("dc:description"));
    elemMap.insert(QLatin1String("dc:subject"), QLatin1String("dc:subject"));
    elemMap.insert(QLatin1String("dc:title"), QLatin1String("dc:title"));
    elemMap.insert(QLatin1String("cp:keywords"), QLatin1String("meta:keyword"));
}


KoFilter::ConversionStatus MsooXmlDocPropertiesReader::read(MsooXmlReaderContext*)
{
    kDebug() << "=============================";

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    readNext();

    if (!expectEl("cp:coreProperties")) 
        return KoFilter::WrongFormat;
    
    if (!expectNS(MSOOXML::Schemas::core_properties))
        return KoFilter::WrongFormat;

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("cp", MSOOXML::Schemas::core_properties))) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::core_properties));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    kDebug() << qualifiedName();
    TRY_READ(coreProperties)
    
    kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL coreProperties
KoFilter::ConversionStatus MsooXmlDocPropertiesReader::read_coreProperties()
{
    READ_PROLOGUE

    while(true) {
        BREAK_IF_END_OF(CURRENT_EL);
        readNext();
        if (isStartElement()) {
            const QString qn = qualifiedName().toString();
            while (!isEndElement() && !isCharacters())
                readNext();

            const QMap<QString,QString>::ConstIterator it = elemMap.find(qn);
            if (it == elemMap.end()) {
                kDebug() << "Unknown metadata ignored:" << qn;
                while (!isEndElement())
                    readNext();
                continue;
            }
            kDebug() << "Found:" << it.key() << "Mapped to:" << it.value();
            const QString t = text().toString();
            meta->startElement(qPrintable(it.value()));
            meta->addTextNode(t.toUtf8());
            meta->endElement();
            while (!isEndElement())
                readNext();
        }
    }

    READ_EPILOGUE
}

