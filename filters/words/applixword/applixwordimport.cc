/* This file is part of the KDE project
   Copyright (C) 2000 Enno Bartels <ebartels@nwn.de>
   Copyright (C) 2011 David Faure <faure@kde.org>

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

#include "applixwordimport.h"

#include <QMessageBox>
#include <QList>
#include <QDateTime>
#include <QTextStream>
#include <QByteArray>
#include <QColor>
#include <QBuffer>
#include <kdebug.h>
#include <KoFilterChain.h>
#include <kpluginfactory.h>
#include <KoOdfWriteStore.h>
#include <KoGenStyles.h>
#include <KoXmlWriter.h>

K_PLUGIN_FACTORY_WITH_JSON(APPLIXWORDImportFactory, "calligra_filter_applixword2odt.json", registerPlugin<APPLIXWORDImport>();)
//K_EXPORT_PLUGIN(APPLIXWORDImportFactory("calligrafilters"))


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
        emit sigProgress(m_progress) ;
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
        //kDebug(30517) << "POS:" << pos << " length:" << mystr.length() << " y:" << y;
        //kDebug(30517) << "<" << mystr << " >";
        if ((pos > 0) && (mystr[pos-1] == '\\')) {
            //kDebug(30517) << " escape character, keep going";
            y = pos + 1;
        } else {
            //kDebug(30517) << " String end //";
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
            kDebug(30517) << "  Color:" << colname << (*it).name();
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
        kError(30517) << "Unable to open input file!" << endl;
        in.close();
        return KoFilter::FileNotFound;
    }


    //create output files
    KoStore *store = KoStore::createStore(m_chain->outputFile(), KoStore::Write, to, KoStore::Zip);
    if (!store || store->bad()) {
        kWarning(30517) << "Unable to open output file!";
        delete store;
        return KoFilter::FileNotFound;
    }
    store->disallowNameExpansion();
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
    KoXmlWriter* paragraphWriter = 0;

    /**************************************************************************
     * Read header                                                            *
     **************************************************************************/
    if (! readHeader(stream)) return KoFilter::StupidError;


    while (!stream.atEnd()) {
        // Read one line
        mystr = readTagLine(stream);
        ok = true;

        //kDebug() << "mystr=" << mystr;

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
                    kDebug(30517) << "End styles";
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
                        kDebug(30517) << " Color :" << c << "" << m << "" << y << "" << k << "" << coltxt << "";

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
            kDebug(30517) << "\nEmbedded Applix object starts:";
            do {
                mystr = readTagLine(stream);
                if (mystr == "<end_data>") ok = false;
                else {
                    kDebug(30517) << "" << mystr;
                }
            } while (ok == true);
            kDebug(30517) << "Embedded Applix object ends";

        }
        /**********************************************************************
         * jump over header footer                                            *
         **********************************************************************/
        else if (mystr.startsWith("<start_hdrftr ")) {
            kDebug(30517) << "\nHeader/Footer starts:";
            do {
                mystr = readTagLine(stream);
                if (mystr == "<end_hdrftr>") ok = false;
                else {
                    kDebug(30517) << "" << mystr;
                }
            } while (ok == true);
            kDebug(30517) << "\nHeader/Footer ends";
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

            kDebug(30517) << " Para  Name:" << stylename;
            kDebug(30517) << "       Rest:" << mystr;

            // TODO use paragraph style name 'stylename'

            // parse paragraph properties, e.g. <P "Normal" justifyFull  size:14 >
            KoGenStyle paragStyle(KoGenStyle::ParagraphAutoStyle, "paragraph");

            const QStringList typeList = mystr.split(' ', QString::SkipEmptyParts);
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
                    kDebug() << "Unsupported paragraph formatting attribute" << type;
                }
            }

            const QString autoStyleName = mainStyles.insert(paragStyle);

            delete paragraphWriter;
            paragraphWriter = 0;
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
            kDebug(30517) << "Text:<" << textstr << " >" << pos << "  Rest:<" << mystr << ">";

            KoGenStyle style(KoGenStyle::TextAutoStyle, "text");
            //style.addAttribute("style:display-name", styleName);

            // split format
            const QStringList typeList = mystr.split(' ', QString::SkipEmptyParts);
            Q_FOREACH(const QString& type, typeList) {
                //kDebug(30517) << "Text formatting:" << type;
                if (!parseFontProperty(type, style)) {
                    kDebug(30517) << "Unhandled text format:" << type;
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
            kDebug() << "Unhandled tag:" << mystr;
        }
    }

    emit sigProgress(100);

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
        kWarning() << "Error while trying to write 'meta.xml'. Partition full?";
        delete store;
        return KoFilter::CreationError;
    }

    if ( !odfStore.closeManifestWriter() ) {
        kWarning() << "Error while trying to write 'META-INF/manifest.xml'. Partition full?";
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
    QChar chr;

    if ((a == 'n') && (b == 'p')) chr = 'ß';


    else if ((a == 'n') && (b == 'c')) chr = 'Ò';
    else if ((a == 'p') && (b == 'c')) chr = 'ò';

    else if ((a == 'n') && (b == 'd')) chr = 'Ó';
    else if ((a == 'p') && (b == 'd')) chr = 'ó';

    else if ((a == 'n') && (b == 'e')) chr = 'Ô';
    else if ((a == 'p') && (b == 'e')) chr = 'ô';

    else if ((a == 'n') && (b == 'f')) chr = 'Õ';
    else if ((a == 'p') && (b == 'f')) chr = 'õ';

    else if ((a == 'p') && (b == 'g')) chr = 'ö';
    else if ((a == 'n') && (b == 'g')) chr = 'Ö';



    else if ((a == 'n') && (b == 'j')) chr = 'Ù';
    else if ((a == 'p') && (b == 'j')) chr = 'ù';

    else if ((a == 'n') && (b == 'k')) chr = 'Ú';
    else if ((a == 'p') && (b == 'k')) chr = 'ú';

    else if ((a == 'n') && (b == 'l')) chr = 'Û';
    else if ((a == 'p') && (b == 'l')) chr = 'û';

    else if ((a == 'p') && (b == 'm')) chr = 'ü';
    else if ((a == 'n') && (b == 'm')) chr = 'Ü';



    else if ((a == 'm') && (b == 'a')) chr = 'À';
    else if ((a == 'o') && (b == 'a')) chr = 'à';

    else if ((a == 'm') && (b == 'b')) chr = 'Á';
    else if ((a == 'o') && (b == 'b')) chr = 'á';

    else if ((a == 'm') && (b == 'c')) chr = 'Â';
    else if ((a == 'o') && (b == 'c')) chr = 'â';

    else if ((a == 'm') && (b == 'd')) chr = 'Ã';
    else if ((a == 'o') && (b == 'd')) chr = 'ã';

    else if ((a == 'm') && (b == 'e')) chr = 'Ä';
    else if ((a == 'o') && (b == 'e')) chr = 'ä';

    else if ((a == 'm') && (b == 'f')) chr = 'Å';
    else if ((a == 'o') && (b == 'f')) chr = 'å';

    else if ((a == 'm') && (b == 'g')) chr = 'Æ';
    else if ((a == 'o') && (b == 'g')) chr = 'æ';



    else if ((a == 'm') && (b == 'i')) chr = 'È';
    else if ((a == 'o') && (b == 'i')) chr = 'è';

    else if ((a == 'm') && (b == 'j')) chr = 'É';
    else if ((a == 'o') && (b == 'j')) chr = 'é';

    else if ((a == 'm') && (b == 'k')) chr = 'Ê';
    else if ((a == 'o') && (b == 'k')) chr = 'ê';

    else if ((a == 'm') && (b == 'l')) chr = 'Ë';
    else if ((a == 'o') && (b == 'l')) chr = 'ë';






    else if ((a == 'm') && (b == 'm')) chr = 'Ì';
    else if ((a == 'o') && (b == 'm')) chr = 'ì';

    else if ((a == 'm') && (b == 'n')) chr = 'Í';
    else if ((a == 'o') && (b == 'n')) chr = 'í';

    else if ((a == 'm') && (b == 'o')) chr = 'Î';
    else if ((a == 'o') && (b == 'o')) chr = 'î';

    else if ((a == 'm') && (b == 'p')) chr = 'Ï';
    else if ((a == 'o') && (b == 'p')) chr = 'ï';


    else if ((a == 'n') && (b == 'b')) chr = 'Ñ';
    else if ((a == 'p') && (b == 'b')) chr = 'ñ';


    else if ((a == 'k') && (b == 'c')) chr = '¢';
    else if ((a == 'k') && (b == 'j')) chr = '©';
    else if ((a == 'l') && (b == 'f')) chr = 'µ';
    else if ((a == 'n') && (b == 'i')) chr = 'Ø';
    else if ((a == 'p') && (b == 'i')) chr = 'ø';

    else if ((a == 'l') && (b == 'j')) chr = '¹';
    else if ((a == 'l') && (b == 'c')) chr = '²';
    else if ((a == 'l') && (b == 'd')) chr = '³';

    else if ((a == 'l') && (b == 'm')) chr = '¼';
    else if ((a == 'l') && (b == 'n')) chr = '½';
    else if ((a == 'l') && (b == 'o')) chr = '¾';

    else if ((a == 'l') && (b == 'a')) chr = '°';

    else if ((a == 'k') && (b == 'o')) chr = '®';
    else if ((a == 'k') && (b == 'h')) chr = '§';
    else if ((a == 'k') && (b == 'd')) chr = '£';

    else if ((a == 'p') && (b == 'a')) chr = 'ð';
    else if ((a == 'n') && (b == 'a')) chr = 'Ð';

    else if ((a == 'l') && (b == 'l')) chr = '»';
    else if ((a == 'k') && (b == 'l')) chr = '«';

    else if ((a == 'l') && (b == 'k')) chr = 'º';

    else if ((a == 'l') && (b == 'h')) chr = '·';

    else if ((a == 'k') && (b == 'b')) chr = '¡';

    else if ((a == 'k') && (b == 'e')) chr = '¤';

    else if ((a == 'l') && (b == 'b')) chr = '±';

    else if ((a == 'l') && (b == 'p')) chr = '¿';

    else if ((a == 'k') && (b == 'f')) chr = '¥';

    else if ((a == 'p') && (b == 'o')) chr = 'þ';
    else if ((a == 'n') && (b == 'o')) chr = 'Þ';

    else if ((a == 'n') && (b == 'n')) chr = 'Ý';
    else if ((a == 'p') && (b == 'n')) chr = 'ý';
    else if ((a == 'p') && (b == 'p')) chr = 'ÿ';

    else if ((a == 'k') && (b == 'k')) chr = 'ª';

    else if ((a == 'k') && (b == 'm')) chr = '¬';
    else if ((a == 'p') && (b == 'h')) chr = '÷';

    else if ((a == 'k') && (b == 'g')) chr = '|';

    else if ((a == 'l') && (b == 'e')) chr = '\'';

    else if ((a == 'k') && (b == 'i')) chr = '¨';

    else if ((a == 'k') && (b == 'n')) chr = '­';

    else if ((a == 'k') && (b == 'p')) chr = '¯';

    else if ((a == 'l') && (b == 'g')) chr = '¶';

    else if ((a == 'l') && (b == 'i')) chr = '¸';

    else if ((a == 'm') && (b == 'h')) chr = 'Ç';
    else if ((a == 'o') && (b == 'h')) chr = 'ç';

    else if ((a == 'n') && (b == 'h')) chr = '×';

    else if ((a == 'k') && (b == 'a')) chr = ' ';

    else if ((a == 'a') && (b == 'j')) chr = '!';

    else  chr = '#';

    return chr;
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


    // 2. Replace part for this characters: applixwear qoutes
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
    int     rueck;
    int     vers[3] = { 0, 0, 0 };

    // Read Headline
    QString mystr = readTagLine(stream);

    rueck = sscanf((const char *) mystr.toLatin1() ,
                   "*BEGIN WORDS VERSION=%d/%d ENCODING=%dBIT",
                   &vers[0], &vers[1], &vers[2]);
    printf("Versions info: %d %d %d\n", vers[0], vers[1], vers[2]);

    // Check the headline
    if (rueck <= 0) {
        printf("Header not correkt - May be it is not an applixword file\n");
        printf("Headerline: <%s>\n", (const char *) mystr.toLatin1());

        QMessageBox::critical(0L, "Applixword header problem",
                              QString("The Applixword header is not correct. "
                                      "May be it is not an applixword file! <BR>"
                                      "This is the header line I did read:<BR><B>%1</B>").arg(mystr),
                              "Okay");

        // i18n( "What is the separator used in this file ? First line is \n%1" ).arg(firstLine),
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
