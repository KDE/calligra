/* This file is part of the KDE project
   Copyright (C) 2000 Enno Bartels <ebartels@nwn.de>

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

#include <QMessageBox>
#include <QList>
#include <QDateTime>
#include <QTextStream>
#include <QByteArray>
#include <QColor>
#include <applixwordimport.h>
#include <applixwordimport.moc>
#include <kdebug.h>
#include <KoFilterChain.h>
#include <kpluginfactory.h>
#include <KoOdfWriteStore.h>
#include <KoGenStyles.h>
#include <KoXmlWriter.h>

K_PLUGIN_FACTORY(APPLIXWORDImportFactory, registerPlugin<APPLIXWORDImport>();)
K_EXPORT_PLUGIN(APPLIXWORDImportFactory("calligrafilters"))


struct t_mycolor {
    int c;
    int m;
    int y;
    int k;

    int r;
    int g;
    int b;
};

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

    int  rueck;
    int  pos;
    bool ok;
    char stylename[100];
    QString           mystr, textstr;
    QList<t_mycolor*>  mcol;
    QStringList       mcoltxt;
    bool inParagraph = false;

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
            t_mycolor *col = new t_mycolor; // delete is in place
            QString    coltxt ;
            int zaehler = 0; // Note: "zaehler" means "counter" in English
            do {
                mystr = readTagLine(stream);
                if (mystr == "<end_styles>") {
                    ok = false;
                    kDebug(30517) << "End styles";
                } else {
                    if (mystr.startsWith("<color ")) {
                        mystr.remove(0, 8);
                        pos = mystr.indexOf("\"");
                        coltxt = mystr.left(pos);
                        mystr.remove(0, pos + 1);
                        rueck = sscanf((const char *) mystr.toLatin1() ,
                                       ":%d:%d:%d:%d>",
                                       &col->c, &col->m, &col->y, &col->k);
                        kDebug(30517) << "  Color" <<  zaehler << "  :" << col->c << "" << col->m << "" << col->y << "" << col->k << "" << coltxt << "";
                        zaehler ++;

                        // Color transformation cmyk -> rgb
                        col->r = 255 - (col->c + col->k);
                        if (col->r < 0) col->r = 0;

                        col->g = 255 - (col->m + col->k);
                        if (col->g < 0) col->g = 0;

                        col->b = 255 - (col->y + col->k);
                        if (col->b < 0) col->b = 0;

                        mcol.append(col);
                        mcoltxt.append(coltxt);
                    } //end if ...<col...
                } //end else
            } // end while
            while (ok == true);
            delete col;
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
         * found a paragraph string                                           *
         **********************************************************************/
        else if (mystr.startsWith("<P ")) {
            sscanf((const char *) mystr.toLatin1(), "<P \"%99s\"", stylename);
            mystr.remove(0, 5 + strlen(stylename));
            kDebug(30517) << " Para  Name:" << stylename;
            kDebug(30517) << "       Rest:" << mystr;

            if (inParagraph)
                bodyWriter->endElement(); // text:p

            bodyWriter->startElement("text:p");
            inParagraph = true;
        }
        /**********************************************************************
         * found a textstring                                                 *
         **********************************************************************/
        else if (mystr.startsWith("<T ")) {
            QString colname;

            // Remove starting tab info
            mystr.remove(0, 4);

            // Remove ending >
            mystr.remove(mystr.length() - 1, 1);

            // Separate textstring "
            ok = true;
            int y = 0;
            do {
                pos = mystr.indexOf("\"", y);
                kDebug(30517) << "POS:" << pos << " length:" << mystr.length() << " y:" << y;

                kDebug(30517) << "<" << mystr << " >";
                if ((pos - 1 > -1) && (mystr[pos-1] == '\\')) {
                    kDebug(30517) << " No string end - but Gänsefüsschen";
                    y = pos + 1;
                } else {
                    kDebug(30517) << " String end //";
                    ok = false;
                }
            } while (ok == true);

            textstr = mystr.left(pos);
            mystr.remove(0, pos + 1);
            mystr = mystr.trimmed();
            kDebug(30517) << "Text:<" << textstr << " >" << pos << "  Rest:<" << mystr << ">";

            // split format
            const QStringList typeList = mystr.split(' ', QString::SkipEmptyParts);

            int fontsize = 12, bold = 0, italic = 0, underline = 0, colpos = -1;
            QString fontname;
            int nn = 0;
            Q_FOREACH(const QString& type, typeList) {
                kDebug(30517) << "   No:" << nn << "   >" << type << "< =";

                // Looking for bold
                if (type == "bold") {
                    bold = 1;
                    kDebug(30517) << "bold";
                } else if (type == "no-bold") {
                    bold = 0;
                    kDebug(30517) << "no bold";
                } else if (type == "italic") {
                    italic = 1;
                    kDebug(30517) << "italic";
                } else if (type == "no-italic") {
                    italic = 0;
                    kDebug(30517) << "no italic";
                } else if (type == "underline") {
                    underline = 1;
                    kDebug(30517) << "underline";
                } else if (type == "no-underline") {
                    underline = 0;
                    kDebug(30517) << "no underline";
                } else if (type.startsWith("size")) { // e.g. size:14
                    //type.remove(0, 5);
                    fontsize = type.mid(5).toInt();
                    kDebug(30517) << "fontsize:" << fontsize;
                } else if (type.startsWith("face")) { // e.g. face:"Symbol"
                    //type.remove(0, 6);
                    //type.remove(type.length() - 1, 1);
                    fontname = type.mid(6, type.length() - 6 - 1);
                    kDebug(30517) << "fontname:" << fontname;
                } else if (type.startsWith("color:")) { // e.g. color:"Black"
                    //type.remove(0, 7);
                    //type.remove(type.length() - 1, 1);
                    colname = type.mid(7, type.length() - 7 - 1);
                    colpos = mcoltxt.indexOf(colname);
                    kDebug(30517) << "  Color:" << colname << "" << colpos << "";
                } else {
                    kDebug(30517) << "" << type;
                }


            }
            kDebug(30517) << "";

            // Replaces Part for & <>, applixware special characters and quotes
            replaceSpecial(textstr);

            KoGenStyle style(KoGenStyle::TextAutoStyle, "text");
            //style.addAttribute("style:display-name", styleName);

            //if (bold == 1 || underline == 1 || italic == 1 || fontsize != 12 ||
            //        colpos != -1 || !fontname.isEmpty()) {
                if (!fontname.isEmpty()) {
                    style.addProperty("fo:font-family", fontname, KoGenStyle::TextType);
                }

                if (fontsize != 1) {
                    style.addPropertyPt("fo:font-size", fontsize, KoGenStyle::TextType);
                }

                if (italic == 1) {
                    style.addProperty("fo:font-style", "italic", KoGenStyle::TextType);
                }

                if (bold == 1) {
                    style.addProperty("fo:font-weight", "bold", KoGenStyle::TextType);
                }

                if (underline == 1) {
                   style.addProperty("style:text-underline-type", "single", KoGenStyle::TextType);
                   style.addProperty("style:text-underline-style", "solid", KoGenStyle::TextType);
                }

                if (colpos != -1) {
                    t_mycolor *mc = mcol.at(colpos);
                    QColor color(mc->r, mc->g, mc->b);
                    style.addProperty("style:fo-color", color.name(), KoGenStyle::TextType);
                }
            //}

            if (!inParagraph) {
                bodyWriter->startElement("text:p");
                inParagraph = true;
            }

            QString styleName = mainStyles.insert(style);
            bodyWriter->startElement("text:span");
            bodyWriter->addAttribute("text:style-name", styleName);
            bodyWriter->addTextSpan(textstr);
            bodyWriter->endElement(); // span
        }

    }
    if (inParagraph)
        bodyWriter->endElement(); // text:p

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
    int ok, pos;

    // 1. Replace Part for this characters: <, >, &
    textstr.replace('&', "&amp;");
    textstr.replace('<', "&lt;");
    textstr.replace('>', "&gt;");


    // 2. Replace part for this characters: applixwear qoutes
    ok = true;
    pos = 0;
    do {
        // Searching for an quote
        pos = textstr.indexOf('\"', pos);

        // Is it a textquote ?
        if ((pos > -1) && (textstr[pos-1] == '\\')) {
            textstr.replace(pos - 1, 2, "\"");
        } else {
            ok = false;
        }
    } while (ok == true);



    // 3. Replace part for Applix Characters
    int   foundSpecialCharakter;
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
    xmlWriter->addTextNode(QString("KOConverter/%1").arg(KOFFICE_VERSION_STRING));
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

