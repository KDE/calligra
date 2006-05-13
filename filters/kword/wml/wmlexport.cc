/* This file is part of the KDE project
   Copyright (C) 2002 Ariya Hidayat <ariyahidayat@yahoo.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <QTextCodec>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
//Added by qt3to4:
#include <QByteArray>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <kgenericfactory.h>

#include <KWEFStructures.h>
#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>
#include <KWEFUtil.h>

#include "wmlexport.h"

typedef KGenericFactory<WMLExport> WMLExportFactory;
K_EXPORT_COMPONENT_FACTORY( libwmlexport, WMLExportFactory( "kofficefilters" ) )

class WMLWorker : public KWEFBaseWorker
{
  public:
    WMLWorker(void)  { }
    virtual ~WMLWorker(void) { }
    virtual bool doOpenFile(const QString& filenameOut, const QString& to);
    virtual bool doCloseFile(void);
    virtual bool doOpenDocument(void);
    virtual bool doCloseDocument(void);
    virtual bool doFullParagraph(const QString& paraText, const LayoutData& layout,
        const ValueListFormatData& paraFormatDataList);
  private:
    QString filename;
    QString result;
    bool m_bold, m_italic, m_underline;
};

bool WMLWorker::doOpenFile(const QString& filenameOut, const QString& /*to*/)
{
  filename = filenameOut;
  return TRUE;
}

bool WMLWorker::doCloseFile(void)
{
  QFile out( filename );
  if( !out.open( QIODevice::WriteOnly ) )
    return FALSE;
  QTextStream stream;
  stream.setDevice( &out );
  stream << result;
  return TRUE;
}

bool WMLWorker::doOpenDocument(void)
{
  result = "<!DOCTYPE wml PUBLIC \"-//WAPFORUM//DTD WML 1.1//EN\"\n";
  result += "      \"http://www.wapforum.org/DTD/wml_1.1.xml\" >\n";
  result += "<!-- Created using KWord, see www.koffice.org/kword -->\n";
  result += "<wml>\n";
  result += "<card>\n";

  m_bold = m_italic = m_underline = FALSE;

  return TRUE;
}

bool WMLWorker::doCloseDocument(void)
{
  result += "</card>\n";
  result += "</wml>";
  return TRUE;
}

bool WMLWorker::doFullParagraph(const QString& paraText,
  const LayoutData& layout, const ValueListFormatData& paraFormatDataList)
{
  QString wmlText;
  QString text = paraText;

  ValueListFormatData::ConstIterator it;
  for( it = paraFormatDataList.begin(); it!=paraFormatDataList.end(); ++it )
  {
    const FormatData& formatData = *it;

    // only if the format is for text (id==1)
    if( formatData.id == 1 )
    {
      QString partialText;
      partialText = text.mid( formatData.pos, formatData.len );

      // escape the text
      partialText = KWEFUtil::EscapeSgmlText( NULL, partialText, TRUE, TRUE );

      // apply formatting
      m_bold = formatData.text.weight >= 75;
      m_italic = formatData.text.italic;
      m_underline = formatData.text.underline;

      if( m_bold ) partialText = "<b>" + partialText + "</b>";
      if( m_italic ) partialText = "<i>" + partialText + "</i>";
      if( m_underline ) partialText = "<u>" + partialText + "</u>";


      wmlText += partialText;
    }
  }

  // sentinel check
  QString align = layout.alignment.lower();
  if( ( align!="left" ) && ( align!="right" ) && ( align!="center" ) )
    align = "left";

  result += "<p align=\"" + align + "\">" + wmlText + "</p>\n";

  return TRUE;
}

WMLExport::WMLExport( QObject* parent, const QStringList& ):
                     KoFilter(parent)
{
}

KoFilter::ConversionStatus WMLExport::convert( const QByteArray& from,
  const QByteArray& to )
{
  // check for proper conversion
  if( to!= "text/vnd.wap.wml" || from != "application/x-kword" )
     return KoFilter::NotImplemented;

  WMLWorker* worker = new WMLWorker();
  KWEFKWordLeader* leader = new KWEFKWordLeader( worker );

  KoFilter::ConversionStatus result;
  result = leader->convert( m_chain, from, to );

  delete worker;
  delete leader;

  return result;
}

#include "wmlexport.moc"
