/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Enno Bartels <ebartels@nwn.de>
   SPDX-FileCopyrightText: 2011 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "applixwordimport.h"
#include "calligra-version.h"

#include <QMessageBox>
#include <QList>
#include <QDateTime>
#include <QTextStream>
#include <QByteArray>
#include <QColor>
#include <QBuffer>
#include <QDebug>
#include <QLoggingCategory>

#include <KPluginFactory>

#include <KoFilterChain.h>
#include <KoOdfWriteStore.h>
#include <KoGenStyles.h>
#include <KoXmlWriter.h>

const QLoggingCategory &AW_LOG()
{
    static const QLoggingCategory category("calligra.filter.applixword2odt");
    return category;
}

#define debugAw qCDebug(AW_LOG)
#define warnAw qCWarning(AW_LOG)
#define errorAw qCCritical(AW_LOG)


K_PLUGIN_FACTORY_WITH_JSON(APPLIXWORDImportFactory, "calligra_filter_applixword2odt.json",
                           registerPlugin<APPLIXWORDImport>();)


/******************************************************************************
 *  class: APPLIXWORDImport        function: APPLIXWORDImport                 *
 ******************************************************************************
 *                                                                            *
 *  Short description : Constructor                                           *
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
APPLIXWORDImport::APPLIXWORDImport(QObject *parent, const QVariantList&) :
        KoFilter(parent)
{
}

/******************************************************************************
 *  class: APPLIXWORDImport        function: nextLine                         *
 ******************************************************************************
 *                                                                            *
 *  Short description : Readline and update progressbar                       *
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
QString
APPLIXWORDImport::nextLine(QTextStream & stream)
{
    if (!m_nextPendingLine.isNull()) {
        const QString s = m_nextPendingLine;
        m_nextPendingLine.clear();
        return s;
    }

    // Read one Line
    QString s = stream.readLine();

    m_instep += s.length();
    if (m_instep > m_stepsize) {
        m_instep    = 0;
        m_progress += 2;
        Q_EMIT sigProgress(m_progress) ;
    }

    return s;
}

// Look for the next '"', skipping any escaped '"'.
// Usually called with startPos being at the character just after the opening '"'.
static int nextDoubleQuote(const QString& mystr, int startPos)
{
    int y = startPos;
    do {
        const int pos = mystr.indexOf('"', y);
        //debugAw << "POS:" << pos << " length:" << mystr.length() << " y:" << y;
        //debugAw << "<" << mystr << " >";
        if ((pos > 0) && (mystr[pos-1] == '\\')) {
            //debugAw << " escape character, keep going";
            y = pos + 1;
        } else {
            //debugAw << " String end //";
            return pos;
        }
    } while (true);
    return -1; // NOTREACHED
}

bool APPLIXWORDImport::parseFontProperty(const QString& type, KoGenStyle& style) const
{
    if (type == "bold") {
        style.addProperty("fo:font-weight", "bold", KoGenStyle::TextType);
        return true;
    } else if (type == "no-bold") {
        style.addProperty("fo:font-weight", "normal", KoGenStyle::TextType);
        return true;
    } else if (type == "italic") {
        style.addProperty("fo:font-style", "italic", KoGenStyle::TextType);
        return true;
    } else if (type == "no-italic") {
        style.addProperty("fo:font-style", "normal", KoGenStyle::TextType);
        return true;
    } else if (type == "underline") {
        style.addProperty("style:text-underline-type", "single", KoGenStyle::TextType);
        style.addProperty("style:text-underline-style", "solid", KoGenStyle::TextType);
        return true;
    } else if (type == "no-underline") {
        style.addProperty("style:text-underline-type", "none", KoGenStyle::TextType);
        style.addProperty("style:text-underline-style", "none", KoGenStyle::TextType);
        return true;
    } else if (type.startsWith("size:")) {
        style.addPropertyPt("fo:font-size", type.mid(5).toInt(), KoGenStyle::TextType);
        return true;
    } else if (type.startsWith("face:")) { // e.g. face:"Symbol"
        const QString fontname = type.mid(6, type.length() - 6 - 1);
        style.addProperty("fo:font-family", fontname, KoGenStyle::TextType);
        return true;
    } else if (type.startsWith("color:")) { // e.g. color:"Black"
        QString colname = type.mid(7, type.length() - 7 - 1);
        QMap<QString, QColor>::const_iterator it = m_colorMap.find(colname);
        if (it != m_colorMap.end()) {
            debugAw << "  Color:" << colname << (*it).name();
            style.addProperty("style:fo-color", (*it).name(), KoGenStyle::TextType);
        }
        return true;
    }
    return false;
}

/******************************************************************************
 *  class: APPLIXWORDImport        function: filter                           *
 ******************************************************************************
 *                                                                            *
 *  Short description :                                                       *
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
KoFilter::ConversionStatus APPLIXWORDImport::convert(const QByteArray& from, const QByteArray& to)
{

    if (to != "application/vnd.oasis.opendocument.text" || from != "application/x-applix-word")
        return KoFilter::NotImplemented;

    QFile in(m_chain->inputFile());
    if (!in.open(QIODevice::ReadOnly)) {
        errorAw << "Unable to open input file!" << Qt::endl;
        in.close();
        return KoFilter::FileNotFound;
    }


    //create output files
    KoStore *store = KoStore::createStore(m_chain->outputFile(), KoStore::Write, to, KoStore::Zip);
    if (!store || store->bad()) {
        warnAw << "Unable to open output file!";
        delete store;
        return KoFilter::FileNotFound;
    }
    KoOdfWriteStore odfStore(store);
    odfStore.manifestWriter(to);

    KoXmlWriter* contentWriter = odfStore.contentWriter();
    if (!contentWriter) {
        delete store;
        return KoFilter::CreationError;
    }

    KoGenStyles mainStyles;
    KoXmlWriter *bodyWriter = odfStore.bodyWriter();

    bodyWriter->startElement("office:body");
    bodyWriter->startElement("office:text");

    QTextStream stream(&in);

    m_stepsize = in.size() / 50;
    m_instep   = 0;
    m_progress = 0;

    int  pos;
    bool ok;
    QString           mystr, textstr;
    // TODO: table implementation below is incomplete
//     bool inTable = false;
//     bool inTableRow = false;

    // We'll get the paragraph style only at the end of the paragraph,
    // so bufferize the paragraph contents
    QBuffer paragraphBuffer;
    KoXmlWriter* paragraphWriter = nullptr;

    /**************************************************************************
     * Read header                                                            *
     **************************************************************************/
    if (! readHeader(stream)) return KoFilter::StupidError;


    while (!stream.atEnd()) {
        // Read one line
        mystr = readTagLine(stream);
        ok = true;

        //debugAw << "mystr=" << mystr;

        /**********************************************************************
         * jump over start_styles if it exists                                *
         **********************************************************************/
        if (mystr == "<start_styles>") {
            printf("Start styles\n");
            QString    coltxt ;
            do {
                mystr = readTagLine(stream);
                if (mystr == "<end_styles>") {
                    ok = false;
                    debugAw << "End styles";
                } else {
                    if (mystr.startsWith("<color ")) {
                        mystr.remove(0, 8);
                        pos = mystr.indexOf('"');
                        coltxt = mystr.left(pos);
                        mystr.remove(0, pos + 1);
                        int c, m, y, k;
                        sscanf((const char *) mystr.toLatin1() ,
                                       ":%d:%d:%d:%d>",
                                       &c, &m, &y, &k);
                        debugAw << " Color :" << c << "" << m << "" << y << "" << k << "" << coltxt << "";

                        m_colorMap.insert(coltxt, QColor::fromCmyk(c, m, y, k));
                    } //end if ...<col...
                } //end else
            } // end while
            while (ok == true);

        } // end if ...<start_styles>...
        /***********************************************************************
         * jump over embedded Applix docs                                      *
         ***********************************************************************/
        else if (mystr == "<start_data Applix>") {
            debugAw << "\nEmbedded Applix object starts:";
            do {
                mystr = readTagLine(stream);
                if (mystr == "<end_data>") ok = false;
                else {
                    debugAw << "" << mystr;
                }
            } while (ok == true);
            debugAw << "Embedded Applix object ends";

        }
        /**********************************************************************
         * jump over header footer                                            *
         **********************************************************************/
        else if (mystr.startsWith("<start_hdrftr ")) {
            debugAw << "\nHeader/Footer starts:";
            do {
                mystr = readTagLine(stream);
                if (mystr == "<end_hdrftr>") ok = false;
                else {
                    debugAw << "" << mystr;
                }
            } while (ok == true);
            debugAw << "\nHeader/Footer ends";
        }
        /**********************************************************************
         * found an "end of paragraph" marker, with the parag style
         **********************************************************************/
        else if (mystr.startsWith("<P ")) {

            // Extract quoted text
            pos = nextDoubleQuote(mystr, 4);
            const QString stylename = mystr.mid(4, pos - 4);
            mystr.remove(0, pos + 1);
            mystr.chop(1); // Remove ending >

            debugAw << " Para  Name:" << stylename;
            debugAw << "       Rest:" << mystr;

            // TODO use paragraph style name 'stylename'

            // parse paragraph properties, e.g. <P "Normal" justifyFull  size:14 >
            KoGenStyle paragStyle(KoGenStyle::ParagraphAutoStyle, "paragraph");

            const QStringList typeList = mystr.split(' ', Qt::SkipEmptyParts);
            Q_FOREACH(const QString& type, typeList) {
                if (type == "justifyFull") {
                    paragStyle.addAttribute("fo:text-align", "justify");
                } else if (type == "justifyCenter") {
                    paragStyle.addAttribute("fo:text-align", "center");
                } else if (type == "justifyLeft") {
                    paragStyle.addAttribute("fo:text-align", "left");
                } else if (type == "justifyRight") {
                    paragStyle.addAttribute("fo:text-align", "right");
                } else if (!parseFontProperty(type, paragStyle)) {
                    debugAw << "Unsupported paragraph formatting attribute" << type;
                }
            }

            const QString autoStyleName = mainStyles.insert(paragStyle);

            delete paragraphWriter;
            paragraphWriter = nullptr;
            paragraphBuffer.close();

            bodyWriter->startElement("text:p");
            bodyWriter->addAttribute("text:style-name", autoStyleName);
            bodyWriter->addCompleteElement(&paragraphBuffer);
            bodyWriter->endElement(); // text:p

            paragraphBuffer.setData(QByteArray());
        }
        /**********************************************************************
         * row start                                                          *
         **********************************************************************/
        else if (mystr.startsWith("<RS")) {
#if 0 // TODO
            if (!inTable) {
                writer.startElement( "table:table" );
                // TODO - but we have no idea about the number of columns...
#if 0
                for ( uint colNr = 0; colNr < getColumns(); ++colNr )
                {
                    writer.startElement( "table:table-column" );
                    KoGenStyle columnStyle( KWDocument::STYLE_TABLE_COLUMN, "table-column" );
                    columnStyle.addPropertyPt( "style:column-width", m_colPositions[colNr+1] - m_colPositions[colNr] );
                    const QString colStyleName = context.mainStyles().lookup( columnStyle, "col" );
                    writer.addAttribute( "table:style-name", colStyleName );
                    writer.endElement(); // table:table-column
                }

#endif
            }
            if (inTableRow)
                writer.endElement(); // table:table-row
            writer.startElement( "table:table-row" );
            inTableRow = true;
            writer.startElement( "table:table-cell" );
            inTableCell = true;
#endif
        }
        /**********************************************************************
         * found a textstring                                                 *
         **********************************************************************/
        else if (mystr.startsWith("<T ")) {

            if (!paragraphWriter) {
                paragraphBuffer.open(QIODevice::WriteOnly);
                paragraphWriter = new KoXmlWriter(&paragraphBuffer, bodyWriter->indentLevel() + 1);
            }

            // Remove starting tab info
            mystr.remove(0, 4);

            // Remove ending >
            mystr.chop(1);

            // Extract quoted text
            pos = nextDoubleQuote(mystr, 0);

            textstr = mystr.left(pos);
            mystr.remove(0, pos + 1);
            mystr = mystr.trimmed();
            debugAw << "Text:<" << textstr << " >" << pos << "  Rest:<" << mystr << ">";

            KoGenStyle style(KoGenStyle::TextAutoStyle, "text");
            //style.addAttribute("style:display-name", styleName);

            // split format
            const QStringList typeList = mystr.split(' ', Qt::SkipEmptyParts);
            Q_FOREACH(const QString& type, typeList) {
                //debugAw << "Text formatting:" << type;
                if (!parseFontProperty(type, style)) {
                    debugAw << "Unhandled text format:" << type;
                }
            }

            // Replaces Part for & <>, applixware special characters and quotes
            replaceSpecial(textstr);

            const QString styleName = mainStyles.insert(style);
            paragraphWriter->startElement("text:span");
            paragraphWriter->addAttribute("text:style-name", styleName);
            paragraphWriter->addTextSpan(textstr);
            paragraphWriter->endElement(); // span
        } else {
            debugAw << "Unhandled tag:" << mystr;
        }
    }

    Q_EMIT sigProgress(100);

    bodyWriter->endElement(); // office:text
    bodyWriter->endElement(); // office:body

    mainStyles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, contentWriter);
    odfStore.closeContentWriter();

    //add manifest line for content.xml
    odfStore.manifestWriter()->addManifestEntry("content.xml", "text/xml");
    if (!mainStyles.saveOdfStylesDotXml(odfStore.store(), odfStore.manifestWriter())) {
        delete store;
        return KoFilter::CreationError;
    }
    if (!createMeta(odfStore)) {
        warnAw << "Error while trying to write 'meta.xml'. Partition full?";
        delete store;
        return KoFilter::CreationError;
    }

    if ( !odfStore.closeManifestWriter() ) {
        warnAw << "Error while trying to write 'META-INF/manifest.xml'. Partition full?";
        delete store;
        return KoFilter::CreationError;
    }

    delete store;
    return KoFilter::OK;
}


/******************************************************************************
 *  function: specCharfind                                                    *
 ******************************************************************************/
QChar
APPLIXWORDImport::specCharfind(QChar a, QChar b) // TODO share this code with applixspreadimport.cc
{
    int chr;

    if ((a == 'n') && (b == 'p'))  chr = 0x00DF; // 'ß';


    else if ((a == 'n') && (b == 'c'))  chr = 0x00D2; // 'Ò';
    else if ((a == 'p') && (b == 'c'))  chr = 0x00F2; // 'ò';

    else if ((a == 'n') && (b == 'd'))  chr = 0x00D3; // 'Ó';
    else if ((a == 'p') && (b == 'd'))  chr = 0x00F3; // 'ó';

    else if ((a == 'n') && (b == 'e'))  chr = 0x00D4; // 'Ô';
    else if ((a == 'p') && (b == 'e'))  chr = 0x00F4; // 'ô';

    else if ((a == 'n') && (b == 'f'))  chr = 0x00D5; // 'Õ';
    else if ((a == 'p') && (b == 'f'))  chr = 0x00F5; // 'õ';

    else if ((a == 'n') && (b == 'g'))  chr = 0x00D6; // 'Ö';
    else if ((a == 'p') && (b == 'g'))  chr = 0x00F6; // 'ö';



    else if ((a == 'n') && (b == 'j'))  chr = 0x00D9; // 'Ù';
    else if ((a == 'p') && (b == 'j'))  chr = 0x00F9; // 'ù';

    else if ((a == 'n') && (b == 'k'))  chr = 0x00DA; // 'Ú';
    else if ((a == 'p') && (b == 'k'))  chr = 0x00FA; // 'ú';

    else if ((a == 'n') && (b == 'l'))  chr = 0x00DB; // 'Û';
    else if ((a == 'p') && (b == 'l'))  chr = 0x00FB; // 'û';

    else if ((a == 'n') && (b == 'm'))  chr = 0x00DC; // 'Ü';
    else if ((a == 'p') && (b == 'm'))  chr = 0x00FC; // 'ü';



    else if ((a == 'm') && (b == 'a'))  chr = 0x00C0; // 'À';
    else if ((a == 'o') && (b == 'a'))  chr = 0x00E0; // 'à';

    else if ((a == 'm') && (b == 'b'))  chr = 0x00C1; // 'Á';
    else if ((a == 'o') && (b == 'b'))  chr = 0x00E1; // 'á';

    else if ((a == 'm') && (b == 'c'))  chr = 0x00C2; // 'Â';
    else if ((a == 'o') && (b == 'c'))  chr = 0x00E2; // 'â';

    else if ((a == 'm') && (b == 'd'))  chr = 0x00C3; // 'Ã';
    else if ((a == 'o') && (b == 'd'))  chr = 0x00E3; // 'ã';

    else if ((a == 'm') && (b == 'e'))  chr = 0x00C4; // 'Ä';
    else if ((a == 'o') && (b == 'e'))  chr = 0x00E4; // 'ä';

    else if ((a == 'm') && (b == 'f'))  chr = 0x00C5; // 'Å';
    else if ((a == 'o') && (b == 'f'))  chr = 0x00E5; // 'å';

    else if ((a == 'm') && (b == 'g'))  chr = 0x00C6; // 'Æ';
    else if ((a == 'o') && (b == 'g'))  chr = 0x00E6; // 'æ';



    else if ((a == 'm') && (b == 'i'))  chr = 0x00C8; // 'È';
    else if ((a == 'o') && (b == 'i'))  chr = 0x00E8; // 'è';

    else if ((a == 'm') && (b == 'j'))  chr = 0x00C9; // 'É';
    else if ((a == 'o') && (b == 'j'))  chr = 0x00E9; // 'é';

    else if ((a == 'm') && (b == 'k'))  chr = 0x00CA; // 'Ê';
    else if ((a == 'o') && (b == 'k'))  chr = 0x00EA; // 'ê';

    else if ((a == 'm') && (b == 'l'))  chr = 0x00CB; // 'Ë';
    else if ((a == 'o') && (b == 'l'))  chr = 0x00EB; // 'ë';



    else if ((a == 'm') && (b == 'm'))  chr = 0x00CC; // 'Ì';
    else if ((a == 'o') && (b == 'm'))  chr = 0x00EC; // 'ì';

    else if ((a == 'm') && (b == 'n'))  chr = 0x00CD; // 'Í';
    else if ((a == 'o') && (b == 'n'))  chr = 0x00ED; // 'í';

    else if ((a == 'm') && (b == 'o'))  chr = 0x00CE; // 'Î';
    else if ((a == 'o') && (b == 'o'))  chr = 0x00EE; // 'î';

    else if ((a == 'm') && (b == 'p'))  chr = 0x00CF; // 'Ï';
    else if ((a == 'o') && (b == 'p'))  chr = 0x00EF; // 'ï';


    else if ((a == 'n') && (b == 'b'))  chr = 0x00D1; // 'Ñ';
    else if ((a == 'p') && (b == 'b'))  chr = 0x00F1; // 'ñ';


    else if ((a == 'k') && (b == 'c'))  chr = 0x00A2; // '¢';
    else if ((a == 'k') && (b == 'j'))  chr = 0x00A9; // '©';
    else if ((a == 'l') && (b == 'f'))  chr = 0x00B5; // 'µ';
    else if ((a == 'n') && (b == 'i'))  chr = 0x00D8; // 'Ø';
    else if ((a == 'p') && (b == 'i'))  chr = 0x00F8; // 'ø';

    else if ((a == 'l') && (b == 'j'))  chr = 0x00B9; // '¹';
    else if ((a == 'l') && (b == 'c'))  chr = 0x00B2; // '²';
    else if ((a == 'l') && (b == 'd'))  chr = 0x00B3; // '³';

    else if ((a == 'l') && (b == 'm'))  chr = 0x0152; // 'Œ';
    else if ((a == 'l') && (b == 'n'))  chr = 0x0153; // 'œ';
    else if ((a == 'l') && (b == 'o'))  chr = 0x0178; // 'Ÿ';

    else if ((a == 'l') && (b == 'a'))  chr = 0x00B0; // '°';

    else if ((a == 'k') && (b == 'o'))  chr = 0x00AE; // '®';
    else if ((a == 'k') && (b == 'h'))  chr = 0x00A7; // '§';
    else if ((a == 'k') && (b == 'd'))  chr = 0x00A3; // '£';

    else if ((a == 'p') && (b == 'a'))  chr = 0x00F0; // 'ð';
    else if ((a == 'n') && (b == 'a'))  chr = 0x00D0; // 'Ð';

    else if ((a == 'l') && (b == 'l'))  chr = 0x00BB; // '»';
    else if ((a == 'k') && (b == 'l'))  chr = 0x00AB; // '«';

    else if ((a == 'l') && (b == 'k'))  chr = 0x00BA; // 'º';

    else if ((a == 'l') && (b == 'h'))  chr = 0x00B7; // '·';

    else if ((a == 'k') && (b == 'b'))  chr = 0x00A1; // '¡';

    else if ((a == 'k') && (b == 'e'))  chr = 0x20AC; // '€';

    else if ((a == 'l') && (b == 'b'))  chr = 0x00B1; // '±';

    else if ((a == 'l') && (b == 'p'))  chr = 0x00BF; // '¿';

    else if ((a == 'k') && (b == 'f'))  chr = 0x00A5; // '¥';

    else if ((a == 'p') && (b == 'o'))  chr = 0x00FE; // 'þ';
    else if ((a == 'n') && (b == 'o'))  chr = 0x00DE; // 'Þ';

    else if ((a == 'n') && (b == 'n'))  chr = 0x00DD; // 'Ý';
    else if ((a == 'p') && (b == 'n'))  chr = 0x00FD; // 'ý';
    else if ((a == 'p') && (b == 'p'))  chr = 0x00FF; // 'ÿ';

    else if ((a == 'k') && (b == 'k'))  chr = 0x00AA; // 'ª';

    else if ((a == 'k') && (b == 'm'))  chr = 0x00AC; // '¬';
    else if ((a == 'p') && (b == 'h'))  chr = 0x00F7; // '÷';

    else if ((a == 'k') && (b == 'g'))  chr = 0x007C; // '|';

    else if ((a == 'l') && (b == 'e'))  chr = 0x0027; // '\'';

    else if ((a == 'k') && (b == 'i'))  chr = 0x0161; // 'š';

    else if ((a == 'k') && (b == 'n'))  chr = 0x00AD; // '­';

    else if ((a == 'k') && (b == 'p'))  chr = 0x00AF; // '¯';

    else if ((a == 'l') && (b == 'g'))  chr = 0x00B6; // '¶';

    else if ((a == 'l') && (b == 'i'))  chr = 0x017E; // 'ž';

    else if ((a == 'm') && (b == 'h'))  chr = 0x00C7; // 'Ç';
    else if ((a == 'o') && (b == 'h'))  chr = 0x00E7; // 'ç';

    else if ((a == 'n') && (b == 'h'))  chr = 0x00D7; // '×';

    else if ((a == 'k') && (b == 'a'))  chr = 0x0020; // ' ';

    else if ((a == 'a') && (b == 'j'))  chr = 0x0021; // '!';

    else  chr = 0x0023; // '#';

    return QChar(chr);
}



/******************************************************************************
 *  class: APPLIXWORDImport        function: readTagLine                      *
 ******************************************************************************
 *                                                                            *
 *  Short description :                                                       *
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
QString
APPLIXWORDImport::readTagLine(QTextStream &stream)
{
    // Read one line
    QString mystr = nextLine(stream);

    // Delete whitespaces
    mystr = mystr.trimmed();

    // Look if the tag continues on the next line
    if ((mystr.length() == 80) && (mystr[mystr.length()-1] == '\\')) {
        bool ok = true;
        do {
            // Read next line
            QString mystrn = nextLine(stream);

            // Is the new line a new tag line
            if (mystrn[0] == ' ') {
                // remove the whitespace at the start of the new line
                mystrn.remove(0, 1);

                // remove the '\' at the end of the old line
                mystr.remove(mystr.length() - 1, 1);

                // append the new line
                mystr += mystrn;
            } else {
                // keep this line for later
                m_nextPendingLine = mystrn;
                ok = false;
            }
        } while (ok == true);
    }

    return mystr;
}




/******************************************************************************
 *  class: APPLIXWORDImport        function: replaceSpecial                   *
 ******************************************************************************
 *                                                                            *
 *  Short description :                                                       *
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
void
APPLIXWORDImport::replaceSpecial(QString &textstr)
{
    // 1. Replace Part for this characters: <, >, &
    textstr.replace('&', "&amp;");
    textstr.replace('<', "&lt;");
    textstr.replace('>', "&gt;");


    // 2. Replace part for this characters: applixwear quotes
    bool ok = true;
    int pos = 0;
    do {
        // Searching for an quote
        pos = textstr.indexOf('\"', pos);

        // Is it a textquote ?
        if ((pos > -1) && (textstr[pos-1] == '\\')) {
            textstr.replace(pos - 1, 2, '"');
        } else {
            ok = false;
        }
    } while (ok == true);



    // 3. Replace part for Applix Characters
    bool  foundSpecialCharakter;
    QChar newchar;

    do {
        // initialize
        foundSpecialCharakter = false;

        pos = textstr.indexOf('^');

        // is there a special character ?
        if (pos > -1) {
            // i have found a special character !
            foundSpecialCharakter = true;

            // translate the applix special character
            newchar = specCharfind(textstr[pos+1], textstr[pos+2]);

            // replace the character
            textstr.replace(pos, 3, newchar);
        }
    } while (foundSpecialCharakter == true);
}



/******************************************************************************
 *  class: APPLIXWORDImport       function: readHeader                        *
 ******************************************************************************
 *                                                                            *
 *  Short description :                                                       *
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
int
APPLIXWORDImport::readHeader(QTextStream &stream)
{
    int     vers[3] = { 0, 0, 0 };

    // Read Headline
    QString mystr = readTagLine(stream);

    // Example: *BEGIN WORDS VERSION=430/320 ENCODING=7BIT
    int ret = sscanf(mystr.toLatin1(),
                   "*BEGIN WORDS VERSION=%d/%d ENCODING=%dBIT",
                   &vers[0], &vers[1], &vers[2]);
    if (ret <= 0) {
        // Older version. Example: *START WORDS VERSION=311 ENCODING=7BIT
        ret = sscanf(mystr.toLatin1(),
                   "*START WORDS VERSION=%d ENCODING=%dBIT",
                   &vers[0], &vers[2]);
        vers[1] = vers[0];
    }
    printf("Versions info: %d %d %d\n", vers[0], vers[1], vers[2]);

    // Check the headline
    if (ret <= 0) {
        printf("Incorrect header - maybe it is not an applixword file\n");
        printf("Headerline: <%s>\n", (const char *) mystr.toLatin1().constData());

        QMessageBox::critical(nullptr, "Applixword header problem",
                              QString("The Applixword header is not correct. "
                                      "May be it is not an applixword file! <BR>"
                                      "This is the header line I did read:<BR><B>%1</B>").arg(mystr),
                              "Okay");
        return false;
    } else return true;
}

bool APPLIXWORDImport::createMeta(KoOdfWriteStore &store)
{
    if (!store.store()->open("meta.xml")) {
        return false;
    }

    KoStoreDevice dev(store.store());
    KoXmlWriter* xmlWriter = KoOdfWriteStore::createOasisXmlWriter(&dev, "office:document-meta");
    xmlWriter->startElement("office:meta");

    xmlWriter->startElement("meta:generator");
    xmlWriter->addTextNode(QString("KOConverter/%1").arg(CALLIGRA_VERSION_STRING));
    xmlWriter->endElement();

    xmlWriter->startElement("meta:creation-date");
    xmlWriter->addTextNode(QDateTime::currentDateTime().toString(Qt::ISODate));
    xmlWriter->endElement();

    xmlWriter->endElement();
    xmlWriter->endElement(); // root element
    xmlWriter->endDocument(); // root element
    delete xmlWriter;
    if (!store.store()->close()) {
        return false;
    }
    store.manifestWriter()->addManifestEntry("meta.xml", "text/xml" );
    return true;
}

#include <applixwordimport.moc>
