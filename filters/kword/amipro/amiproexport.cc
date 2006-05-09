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

#include <qtextcodec.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
//Added by qt3to4:
#include <Q3CString>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <kgenericfactory.h>

#include <KWEFStructures.h>
#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>
#include <KWEFUtil.h>

#include <amiproexport.h>

typedef KGenericFactory<AmiProExport> AmiProExportFactory;
K_EXPORT_COMPONENT_FACTORY( libamiproexport, AmiProExportFactory( "kofficefilters" ) )
class AmiProWorker : public KWEFBaseWorker
{
  public:
    AmiProWorker(void)  { }
    virtual ~AmiProWorker(void) { }
    virtual bool doOpenFile(const QString& filenameOut, const QString& to);
    virtual bool doCloseFile(void);
    virtual bool doOpenDocument(void);
    virtual bool doCloseDocument(void);
    virtual bool doFullParagraph(const QString& paraText, const LayoutData& layout,
        const ValueListFormatData& paraFormatDataList);
  private:
    QString filename;
    QString result;
    bool m_bold, m_italic, m_underline, m_underlineDouble;
    bool m_strike, m_subscript, m_superscript;
};

bool AmiProWorker::doOpenFile(const QString& filenameOut, const QString& /*to*/)
{
  filename = filenameOut;

  return TRUE;
}

bool AmiProWorker::doCloseFile(void)
{
  QFile out( filename );
  if( !out.open( QIODevice::WriteOnly ) )
    return FALSE;
  QTextStream stream;
  stream.setDevice( &out );
  stream << result;
  return TRUE;
}

bool AmiProWorker::doOpenDocument(void)
{
  result = "[ver]\n\t4\n";
  result += "[sty]\n\t\n";
  result += "[lay]\n";
  result += "\tStandard\n";
  result += "\t516\n";
  result += "\t[rght]\n";

  // don't ask me, I have no idea what these numbers are
  int magic[] = { 16833, 11908, 1, 1440, 1440, 1, 1440, 1440,
   0, 1, 0, 1, 0, 2, 1, 1440, 10465, 12, 1, 720, 1, 1440,
   1, 2160, 1, 2880, 1, 3600, 1, 4320, 1, 5040, 1, 5760,
   1, 6480, 1, 7200, 1, 7920, 1, 8640 };
  for( uint i=0; i<sizeof(magic)/sizeof(magic[0]); i++ )
    result += "\t\t" + QString::number(magic[i]) + "\n";

  result += "[elay]\n";
  result += "[edoc]\n";

  m_bold = m_italic = m_underline = m_underlineDouble =
  m_strike = m_subscript = m_superscript = FALSE;

  return TRUE;
}

bool AmiProWorker::doCloseDocument(void)
{
  result += ">\n\n";
  return TRUE;
}

static QString AmiProEscape( const QString& text )
{
  QString result;

  for( unsigned i=0; i<text.length(); i++ )
  {
    QChar ch = text[i];
    switch( ch.unicode() )
    {
      case '<': result += "<<"; break;
      case '>': result += "<;>"; break;
      case '[': result += "<[>"; break;
      case '@': result += "@@"; break;
      case '\'': result += "</R>"; break;
      default: result += ch; break;
    }
  }

  return result;
}

bool AmiProWorker::doFullParagraph(const QString& paraText,
  const LayoutData& /*layout*/, const ValueListFormatData& paraFormatDataList)
{
  QString amiproText = "";
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

      partialText = AmiProEscape( partialText );

      // apply formatting
      m_bold = formatData.text.weight >= 75;
      m_italic = formatData.text.italic;
      m_underline = formatData.text.underline;
      m_underlineDouble = formatData.text.underlineValue == "double";
      m_subscript = formatData.text.verticalAlignment == 1;
      m_superscript = formatData.text.verticalAlignment == 2;
      m_strike = formatData.text.strikeout;

      if( m_bold ) partialText = "<+!>" + partialText + "<-!>";
      if( m_italic ) partialText = "<+\">" + partialText + "<-\">";
      if( m_underline && !m_underlineDouble ) partialText = "<+#>" + partialText + "<-#>";
      if( m_underlineDouble ) partialText = "<+)>" + partialText + "<-)>";
      if( m_subscript ) partialText = "<+'>" + partialText + "<-'>";
      if( m_superscript ) partialText = "<+&>" + partialText + "<-&>";
      if( m_strike) partialText = "<+%>" + partialText + "<-%>";

      amiproText += partialText;
    }
  }

  result += amiproText + "\n\n";

  return TRUE;
}

AmiProExport::AmiProExport( QObject* parent, const QStringList& ):
                     KoFilter(parent)
{
}

KoFilter::ConversionStatus
AmiProExport::convert( const QByteArray& from,
  const QByteArray& to )
{
  // check for proper conversion
  if( to!= "application/x-amipro" || from != "application/x-kword" )
     return KoFilter::NotImplemented;

  AmiProWorker* worker = new AmiProWorker();
  KWEFKWordLeader* leader = new KWEFKWordLeader( worker );

  KoFilter::ConversionStatus result;
  result = leader->convert( m_chain, from, to );

  delete worker;
  delete leader;

  return result;
}

#include "amiproexport.moc"
