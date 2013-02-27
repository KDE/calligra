#include "SvgPresExport.h"
#include <QDebug>
#include <SvgWriter.h>
#include <SvgCustomSavingContext.h>

#include <KoFilterChain.h>
#include <KPrPage.h>
#include <KPrDocument.h>
#include <KPluginFactory>
#include <KoShape.h>

#include <QString>

K_PLUGIN_FACTORY(SvgPresExportFactory, registerPlugin<SvgPresExport>();)
K_EXPORT_PLUGIN(SvgPresExportFactory("calligrafilters"))

SvgPresExport::SvgPresExport(QObject*parent, const QVariantList&)
    : KoFilter(parent)
{
}

KoFilter::ConversionStatus SvgPresExport::convert(const QByteArray& from, const QByteArray& to)
{
  qDebug() << "in svg filter";
  if (to != "image/svg+xml" || from != "application/vnd.oasis.opendocument.presentation")
        return KoFilter::NotImplemented;
  
    
    KPrDocument * document = dynamic_cast<KPrDocument*>(m_chain->inputDocument());
    if (!document)
        return KoFilter::ParsingError;

    QList<KoPAPageBase*> slideShow = document->slideShow();
    //assume this slideshow has only one slide
    KPrPage * slide = dynamic_cast<KPrPage*>(slideShow.front());
    QList<KoShape*> shapes = slide->shapes();

    SvgWriter writer(shapes, slide->size());
    
    {
    qreal width;
    qreal height;
    
    width = slide->size().width();
    height = slide->size().height();
        
    QString header("<?xml version=\"1.0\" standalone=\"no\"?>\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n<!-- Created using Stage, part of Calligra: http://www.calligra-suite.org/karbon -->\n<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:sozi=\"http://sozi.baierouge.fr\" ");
    header.append(QString("width=\"%1pt\" height=\"%2pt\">").arg(width, 0, 'g', 3).arg(height, 0, 'g', 3));
            
    writer.setHeader(header);
    
    SvgCustomSavingContext *savingContext = new SvgCustomSavingContext();
    writer.setSavingContext(*savingContext);
    }
    
    if (!writer.save(m_chain->outputFile(), true))
        return KoFilter::CreationError;
        
    return KoFilter::OK;
}

#include "SvgPresExport.moc"
