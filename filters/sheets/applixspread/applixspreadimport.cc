/* This file is part of the KDE project
   Copyright (C) 2001 Enno Bartels <ebartels@nwn.de>

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

#include "applixspreadimport.h"

#include <QMessageBox>
#include <QStringList>
#include <QRegExp>
#include <QList>
#include <QTextStream>
#include <QByteArray>
#include <kdebug.h>
#include <math.h>
#include <KoFilterChain.h>
#include <kpluginfactory.h>
#include <sheets/Util.h>

K_PLUGIN_FACTORY_WITH_JSON(APPLIXSPREADImportFactory, "calligra_filter_applixspread2kspread.json",
                           registerPlugin<APPLIXSPREADImport>();)


APPLIXSPREADImport::APPLIXSPREADImport(QObject *parent, const QVariantList&)
        : KoFilter(parent)
{
}

QString APPLIXSPREADImport::nextLine(QTextStream & stream)
{
    if (!m_nextPendingLine.isNull()) {
        const QString s = m_nextPendingLine;
        m_nextPendingLine.clear();
        return s;
    }

    QString s = stream.readLine();
    m_instep += s.length();
    if (m_instep > m_stepsize) {
        m_instep = 0;
        m_progress += 2;
        emit sigProgress(m_progress);
    }
    return s;
}

struct t_mycolor {
    int r;
    int g;
    int b;

    int c;
    int m;
    int y;
    int k;
};

struct t_rc {
    QStringList tabname;
    QStringList rc;
};

// Store shared formula definitions
struct t_sharedFormula {
    int origRow;
    int origColumn;
    QString formula;
};

KoFilter::ConversionStatus APPLIXSPREADImport::convert(const QByteArray& from, const QByteArray& to)
{

    if (to != "application/x-kspread" || from != "application/x-applix-spreadsheet")
        return KoFilter::NotImplemented;

    QFile in(m_chain->inputFile());
    if (!in.open(QIODevice::ReadOnly)) {
        kError(30502) << "Unable to open input file!" << endl;
        in.close();
        return KoFilter::FileNotFound;
    }

    QString str;
    QList<t_mycolor*> mcol;

    str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
           "<!DOCTYPE spreadsheet>\n"
           "<spreadsheet mime=\"application/x-kspread\" editor=\"KSpread\" >\n"
           " <paper format=\"A4\" orientation=\"Portrait\" >\n"
           "  <borders right=\"20\" left=\"20\" bottom=\"20\" top=\"20\" />\n"
           "  <head/>\n"
           "  <foot/>\n"
           " </paper>\n"
//    str += " <locale positivePrefixCurrencySymbol=\"True\" negativeMonetarySignPosition=\"1\" negativePrefixCurrencySymbol=\"True\" fracDigits=\"2\" thousandsSeparator=\" \" dateFormat=\"%A, %e. %B %Y\" timeFormat=\"%H:%M:%S\" monetaryDecimalSymbol=\",\" weekStartsMonday=\"True\" currencySymbol=\"DM\" negativeSign=\"-\" positiveSign=\"\" positiveMonetarySignPosition=\"1\" decimalSymbol=\",\" monetaryThousandsSeparator=\" \" dateFormatShort=\"%d.%m.%Y\" />\n";
           " <map markerColumn=\"1\" activeTable=\"Table1\" markerRow=\"1\" >\n";
//      str += "  <table columnnumber=\"0\" borders=\"0\" hide=\"0\" hidezero=\"0\" firstletterupper=\"0\" grid=\"1\" formular=\"0\" lcmode=\"0\" name=\"Tabelle1\" >\n";


    // QTextStream
    QTextStream stream(&in);
    m_stepsize = in.size() / 50;
    m_instep   = 0;
    m_progress = 0;
    int  pos;
    QString  tabctr ;  // Tab control (current tab name)
    QStringList typefacetab;
    QHash<QString, t_sharedFormula> sharedFormulas;

    t_rc my_rc;



    /**************************************************************************
     * Read header                                                            *
     **************************************************************************/
    if (! readHeader(stream)) return KoFilter::StupidError;

    while (!stream.atEnd()) {
        // Read one line
        QString mystr = nextLine(stream);

        kDebug() << "INPUT :" << mystr;


        /**********************************************************************
         *  Looking for the colormap                                          *
         **********************************************************************/
        if (mystr.startsWith("COLORMAP")) {
            readColormap(stream, mcol);
        }

        /**********************************************************************
         *  Looking for the typeface table                                    *
         **********************************************************************/
        else if (mystr.startsWith("TYPEFACE TABLE")) {
            readTypefaceTable(stream, typefacetab);
        }

        /**********************************************************************
         *  Looking for some View-Information                                 *
         **********************************************************************/
        else if (mystr.startsWith("View Start, Name:")) {
            readView(stream, mystr, my_rc);
        }


        /**********************************************************************
         *   Detect ( at the first place of the Line                          *
         **********************************************************************/
        else if (mystr[0] == '(') {

            // Delete  '('
            mystr.remove(0, 1);

            // Remember length of the string
            if (mystr.length() >= 80 - 1) {
                //kDebug() << " Line >= 80 chars";
                bool ok = true;
                do {
                    QString mystrn = nextLine(stream);
                    if (mystrn[0] == ' ') {
                        mystrn.remove(0, 1);
                        mystr += mystrn;
                    } else {
                        m_nextPendingLine = mystrn;
                        ok = false;
                    }
                } while (ok);
                kDebug() << " Long line -> new input line:" << mystr;
            }

            // Search for ')'
            pos = mystr.indexOf(')');
            QString typestr = mystr.left(pos);

            // Delete typeformat info incl. Space
            mystr.remove(0, pos + 1);

            // At this point mystr looks like " A!E15: 10"
            Q_ASSERT(mystr.startsWith(' '));

            // Extract table number/name
            pos = mystr.indexOf('!');

            // Copy tabnumber information
            QString tabnostr = mystr.left(pos).mid(1);

            // Delete tabnumber information
            mystr.remove(0, pos + 1);

            // At this point mystr looks like "E15: 10"
            pos = 0;
            while (mystr[pos].isLetter()) {
                ++pos;
            }
            const QString cellcolstr = mystr.mid(0, pos);
            // Transform ascii column to int column
            const int icol = translateColumnNumber(cellcolstr);

            int endPos = pos;
            while (mystr[endPos].isDigit()) {
                ++endPos;
            }

            const QString rowstr = mystr.mid(pos, endPos - pos);
            bool ok;
            const int irow = rowstr.toInt(&ok);
            Q_ASSERT(ok);

            // OK, what do we have now?
            const QChar contentType = mystr.at(endPos);

            // Delete cellnumber information
            mystr.remove(0, endPos + 1);
            if (mystr.startsWith(' ')) {
                mystr.remove(0, 1);
            }

            // ';' // first instance (definition) of a shared formula
            // '.' // instance (usage) of a shared formula
            // ':' // simple value

            if (contentType == ';' || contentType == '.') {
                // Skip the value
                int pos = 0;
                while (!mystr.at(pos).isSpace()) {
                    ++pos;
                }
                while (mystr.at(pos).isSpace()) {
                    ++pos;
                }
                kDebug() << "Skipping value" << mystr.mid(0, pos);
                mystr.remove(0, pos);

                if (contentType == ';') {
                    if (mystr.at(0) == '+')
                        mystr[0] = '=';
                    Q_ASSERT(mystr.at(0) == '=');
                }
            }

            // Replace part for this characters: <, >, &
            mystr.replace(QRegExp("&"), "&amp;");
            mystr.replace(QRegExp("<"), "&lt;");
            mystr.replace(QRegExp(">"), "&gt;");


            // Replace part for Applix Characters
            bool foundSpecialCharakter;

            do {
                // initialize
                foundSpecialCharakter = false;

                pos = mystr.indexOf('^');

                // is there a special character ?
                if (pos > -1) {
                    // i have found a special character !
                    foundSpecialCharakter = true;

                    // translate the applix special character
                    const QChar newchar = specCharfind(mystr[pos+1], mystr[pos+2]);

                    // replace the character
                    mystr.replace(pos, 3, newchar);
                }

            } while (foundSpecialCharakter == true);


            // examine the typestring
            // split typestring in 3 parts by an |

            const int pos1 = typestr.indexOf('|');
            const int pos2 = typestr.lastIndexOf('|');
            const QString typeFormStr = typestr.left(pos1);
            const QString typeCharStr = typestr.mid(pos1 + 1,  pos2 - pos1 - 1);
            const QString typeCellStr = typestr.right(typestr.length() - pos2 - 1);

            // Is it a new table
            if (tabctr != tabnostr) {
                // is it not the first table
                if (!(tabctr.isNull()))  str += "  </table>\n";

                str += "  <table columnnumber=\"0\" borders=\"0\" hide=\"0\" hidezero=\"0\" firstletterupper=\"0\" grid=\"1\" formular=\"0\" lcmode=\"0\" name=\"" +
                       tabnostr +
                       "\" >\n";

                tabctr = tabnostr;

                // Searching for the rowcol part and adding to the hole string
                pos = my_rc.tabname.indexOf(tabnostr);
                if (pos > -1) str += my_rc.rc[pos];
            }

            //kDebug()<<" Data : Text :"<<mystr<<" tab :"<<tabnostr<<""<< cellnostr <<"" <<ccol<<"" << irow<<""<< typeFormStr<<"" <<typeCharStr<<"" <<typeCellStr;

            /********************************************************************
             * Support for shared formulas                                      *
             ********************************************************************/

            if (contentType == ';') {

                mystr = convertFormula(mystr);

                const QString formulaRefLine = nextLine(stream); // "Formula: 358"
                kDebug() << "shared formula: next line is" << formulaRefLine;
                if (!formulaRefLine.startsWith("Formula: ")) {
                    kError() << "Missing formula ID after" << mystr;
                } else {
                    const QString key = formulaRefLine.mid(9);
                    t_sharedFormula sf;
                    sf.origColumn = icol;
                    sf.origRow = irow;
                    sf.formula = mystr;
                    sharedFormulas.insert(key, sf);
                }
            } else if (contentType == '.') {
                const QString key = mystr;
                const t_sharedFormula sf = sharedFormulas.value(key);

                // adjust the formula: if it came from C1, with =A1+B1,
                // and we're now in C3, then it needs to become =A3+B3, just like copy/paste would do.
                mystr = Calligra::Sheets::Util::adjustFormulaReference(sf.formula, sf.origRow, sf.origColumn, irow, icol);
            }

            /********************************************************************
             * examine character format String, split it up in basic parts      *
             ********************************************************************/
            int bold = 0, italic = 0, underline = 0, fontsize = 12, fontnr = -1;
            int fg = -1; // fg = foregound

            const QStringList typeCharList = typeCharStr.split(',', QString::SkipEmptyParts);
            Q_FOREACH(const QString& typeChar, typeCharList) {
                // Output
                kDebug() << "typeChar: " << typeChar;

                if (typeChar == "B") {
                    kDebug() << " bold";
                    bold  = 1;
                } else if (typeChar == "I") {
                    kDebug() << "   = italic";
                    italic = 1;
                } else if (typeChar == "U") {
                    kDebug() << "   = underline";
                    underline = 1;
                } else if (typeChar.startsWith("FG")) {
                    fg = typeChar.mid(2).toInt();
                    kDebug() << "  = Colornr" << fg;
                } else if (typeChar.startsWith("TF")) {
                    fontnr = typeChar.mid(2).toInt();
                    kDebug() << " = Font :" << fontnr << "" << typefacetab[fontnr];
                } else if (typeChar.startsWith('P')) {
                    fontsize = typeChar.mid(1).toInt();
                    kDebug() << "   = Fontsize" << fontsize;
                } else {
                    kDebug() << "   = ??? Unknown typeChar:" << typeChar;
                }
            }
            kDebug();


            /********************************************************************
             * examine pos format String, split it up in basic parts           *
             ********************************************************************/
            int align = 0, valign = 0;

            const QStringList typeFormList = typeFormStr.split(',', QString::SkipEmptyParts);
            Q_FOREACH(const QString& typeFormat, typeFormList) {
                // Grep horizontal alignment
                if (typeFormat == "1") {
                    kDebug() << " = left align";
                    align = 1; // left
                } else if (typeFormat == "2") {
                    kDebug() << " = right align";
                    align = 3; // right
                } else if (typeFormat == "3") {
                    kDebug() << " = center align";
                    align = 2; // center
                }

                // Grep vertical alignment
                else if (typeFormat == "VT") {
                    kDebug() << " = top valign";
                    valign =  1; // top
                } else if (typeFormat == "VC") {
                    kDebug() << " = center valign";
                    valign =  0; // center - default (2)
                } else if (typeFormat == "VB") {
                    kDebug() << " = bottom valign";
                    valign =  3; // bottom
                } else {
                    kDebug() << "   = ??? unknown typeFormat" << typeFormat;
                }
            }


            /********************************************************************
             * examine cell format String, split it up in basic parts           *
             ********************************************************************/
            int topPenWidth = 0, bottomPenWidth = 0, leftPenWidth = 0, rightPenWidth = 0, fg_bg = -1;
            int topPenStyle = 0, bottomPenStyle = 0, leftPenStyle = 0, rightPenStyle = 0;
            int brushstyle = 0,     brushcolor = 1;
            int topbrushstyle = 0,  topbrushcolor = 1, topfg_bg = 1;
            int leftbrushstyle = 0, leftbrushcolor = 1, leftfg_bg = 1;
            int rightbrushstyle = 0, rightbrushcolor = 1, rightfg_bg = 1;
            int bottombrushstyle = 0, bottombrushcolor = 1, bottomfg_bg = 1;

            const QStringList typeCellList = typeCellStr.split(',', QString::SkipEmptyParts);
            Q_FOREACH(/*can't use const QString&*/ QString typeCell, typeCellList) {

                if (typeCell[0] == 'T') {
                    kDebug() << " = top";
                    transPenFormat(typeCell, &topPenWidth, &topPenStyle);

                    if (typeCell.length() > 2) {
                        typeCell.remove(0, 2);
                        filterSHFGBG(typeCell, &topbrushstyle, &topbrushcolor, &topfg_bg);
                    }

                }

                else if (typeCell[0] == 'B') {
                    kDebug() << " = bottom";
                    transPenFormat(typeCell, &bottomPenWidth, &bottomPenStyle);

                    if (typeCell.length() > 2) {
                        typeCell.remove(0, 2);
                        filterSHFGBG(typeCell, &bottombrushstyle, &bottombrushcolor, &bottomfg_bg);
                    }
                }

                else if (typeCell[0] == 'L') {
                    kDebug() << " = left";
                    transPenFormat(typeCell, &leftPenWidth, &leftPenStyle);

                    if (typeCell.length() > 2) {
                        typeCell.remove(0, 2);
                        filterSHFGBG(typeCell, &leftbrushstyle, &leftbrushcolor, &leftfg_bg);
                    }
                }

                else if (typeCell[0] == 'R') {
                    kDebug() << " = right";
                    transPenFormat(typeCell, &rightPenWidth, &rightPenStyle);

                    if (typeCell.length() > 2) {
                        typeCell.remove(0, 2);
                        filterSHFGBG(typeCell, &rightbrushstyle, &rightbrushcolor, &rightfg_bg);
                    }
                }

                else if ((typeCell.startsWith("SH")) || (typeCell.startsWith("FG")) ||
                         (typeCell.startsWith("BG"))) {
                    kDebug() << " =";
                    filterSHFGBG(typeCell, &brushstyle, &fg_bg, &brushcolor);
                }

                else {
                    kDebug() << "   = ??? unknown typeCell" << typeCell;
                }

            }





            QString col;

            // create kspread fileformat output
            str += "   <cell row=\"" + QString::number(irow) + "\""
                   " column=\""      + QString::number(icol) + "\">\n";
            if (bold == 1  || italic == 1 || underline == 1 ||
                    align != 0 || valign != 0 ||
                    topPenStyle  != 0  || bottomPenStyle != 0 ||
                    leftPenStyle != 0  || rightPenStyle  != 0 || fg != -1 || fg_bg != -1 ||
                    fontsize != 12 || brushstyle != 0 || fontnr != -1) {
                str += "    <format";
                if (brushstyle != 0) {
                    str += " brushstyle=\""  + QString::number(brushstyle) + "\" "
                           " brushcolor=\"" +
                           writeColor(mcol.at(brushcolor)) +
                           "\"";
                }

                if (align   != 0)  str += " align=\""  + QString::number(align) + "\" ";
                if (valign  != 0)  str += " alignY=\"" + QString::number(valign) + "\" ";
                if (fg_bg != -1) {
                    str += " bgcolor=\"" +
                           writeColor(mcol.at(fg_bg)) +
                           "\" ";
                }
                str += ">\n";

                // Font color
                if (fg != -1) {
                    str += "    <pen width=\"0\" style=\"1\" color=\"" +
                           writeColor(mcol.at(fg)) +
                           "\" />\n";
                }

                // Left border
                if (leftPenWidth > 0) {
                    str += "    <left-border>\n";
                    col = writeColor(mcol.at(leftfg_bg));
                    writePen(str, leftPenWidth, leftPenStyle, col);
                    str += "    </left-border>\n";
                }

                // Right border
                if (rightPenWidth > 0) {
                    str += "    <right-border>\n";
                    col = writeColor(mcol.at(rightfg_bg));
                    writePen(str, rightPenWidth, rightPenStyle, col);
                    str += "    </right-border>\n";
                }

                // Bottom border
                if (bottomPenWidth > 0) {
                    str += "    <bottom-border>\n";
                    col = writeColor(mcol.at(bottomfg_bg));
                    writePen(str, bottomPenWidth, bottomPenStyle, col);
                    str += "    </bottom-border>\n";
                }

                // Top border
                if (topPenWidth > 0) {
                    str += "    <top-border>\n";
                    col = writeColor(mcol.at(topfg_bg));
                    writePen(str, topPenWidth, topPenStyle, col);
                    str += "    </top-border>\n";
                }

                // Font (size and family)
                if ((fontsize != 12) || (fontnr != -1)) {
                    str += "     <font ";
                    // Fontsize
                    if (fontsize != 12) {
                        str += "size=\"" +
                               QString::number(fontsize) +
                               "\" ";
                    }
                    // Fontfamily
                    if (fontnr != -1) {
                        str += "family=\"" +
                               typefacetab[fontnr].toLatin1() +
                               "\" ";
                    }
                    str += "weight=\"0\"";

                    if (italic    == 1) str += " italic=\"yes\"";
                    if (bold      == 1) str += " bold=\"yes\"";
                    if (underline == 1) str += " underline=\"yes\"";

                    str += " />\n";
                }
                str += "    </format>\n";
            }
            str += "    <text>" + mystr + "</text>\n"
                   "   </cell>\n";
        }

    }
    emit sigProgress(100);

    str += "  </table>\n"
           " </map>\n"
           "</spreadsheet>\n";
//  str += "</DOC>\n";

    kDebug() << "Text" << str;

    KoStoreDevice* out = m_chain->storageFile("root", KoStore::Write);

    if (!out) {
        kError(38000/*30502*/) << "Unable to open output file!" << endl;
        in.close();
        return KoFilter::StorageCreationError;
    }

    QByteArray cstring = str.toUtf8();
    out->write(cstring, cstring.length());

    in.close();
    return KoFilter::OK;
}




/******************************************************************************
 *  function: specCharfind                                                    *
 ******************************************************************************/
QChar
APPLIXSPREADImport::specCharfind(QChar a, QChar b)
{
    QChar chr;

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

    return chr;
}



/******************************************************************************
 *  function:   writePen                                                      *
 ******************************************************************************/
void
APPLIXSPREADImport::writePen(QString &str, int penwidth, int penstyle, const QString &framecolor)
{
    str += "     <pen width=\"" +

    // width of the pen
           QString::number(penwidth) +
           "\" style=\"" +

    // style of the pen
           QString::number(penstyle) +
           "\" color=\"" +

    // color of the pen
           framecolor +
           "\" />\n";

}



/******************************************************************************
 *  function:   writeColor                                                    *
 ******************************************************************************/
QString
APPLIXSPREADImport::writeColor(t_mycolor *mc)
{
    char rgb[20];

//    printf ("                 WriteColor: <%d>-<%d>-<%d>   <%d>-<%d>-<%d>-<%d>\n",
//            mc->r, mc->g, mc->b,
//            mc->c, mc->m, mc->y, mc->k);

    sprintf(rgb, "#%02X%02X%02X", mc->r, mc->g, mc->b);
    QString bla = rgb;


    return bla;
}




/******************************************************************************
 *  function:   readTypefaceTable                                             *
 ******************************************************************************/
void
APPLIXSPREADImport::readTypefaceTable(QTextStream &stream, QStringList &typefacetab)
{
    int tftabCounter = 0;
    QString mystr;

    // Read the colormap
    kDebug() << "Reading typeface table:";

    bool ok = true;
    do {
        mystr = nextLine(stream);
        // FIXME: What happens if the magic words are not present in the stream?
        if (mystr == "END TYPEFACE TABLE") ok = false;
        else {
            //printf ("  %2d: <%s>\n", tftabCounter, mystr.toLatin1());
            typefacetab.append(mystr);
            tftabCounter++;
        }
    } while (ok == true);

    kDebug() << "... done";
}



/******************************************************************************
 *  function:   readColormap                                                  *
 ******************************************************************************/
void
APPLIXSPREADImport::readColormap(QTextStream &stream,  QList<t_mycolor*> &mcol)
{
    int contcount, pos;

    QString colstr, mystr;
    kDebug() << "Reading colormap:";

    bool ok = true;

    do {

        mystr = nextLine(stream).trimmed();

        if (mystr == "END COLORMAP") ok = false;
        else {
            kDebug() << "  ->" << mystr;

            // Count the number of  whitespaces
            contcount = mystr.count(' ');
            kDebug() << "contcount:" << contcount;
            contcount -= 5;

            // Begin off interest
            pos = mystr.indexOf(" 0 ");

            // get colorname
            colstr = mystr.left(pos);
            mystr.remove(0, pos + 1);
            mystr = mystr.trimmed();

            t_mycolor *tmc = new t_mycolor;

            // get sub colors
            pos = sscanf(mystr.toLatin1(), "0 %d %d %d %d 0",
                         &tmc->c, &tmc->m, &tmc->y, &tmc->k);

            printf("  - <%-20s> <%-15s> <%3d> <%3d> <%3d> <%3d>  pos: %d\n",
                   mystr.toLatin1().data(),
                   colstr.toLatin1().data(),
                   tmc->c, tmc->m, tmc->y, tmc->k, pos);

            // Color transformation cmyk -> rgb
            tmc->r = 255 - (tmc->c + tmc->k);
            if (tmc->r < 0) tmc->r = 0;

            tmc->g = 255 - (tmc->m + tmc->k);
            if (tmc->g < 0) tmc->g = 0;

            tmc->b = 255 - (tmc->y + tmc->k);
            if (tmc->b < 0) tmc->b = 0;

            mcol.append(tmc);
        }

    } while (ok == true);

    kDebug() << "... done" << mcol.count();

    foreach(t_mycolor* emp, mcol) {
        printf(" c:%3d m:%3d y:%3d k:%3d   r:%3d g:%3d b:%3d\n",
               emp->c, emp->m, emp->y, emp->k, emp->r, emp->g, emp->b);
    }
}




/******************************************************************************
 *  function:   readColormap                                                  *
 ******************************************************************************/
void
APPLIXSPREADImport::readView(QTextStream &stream, const QString &instr, t_rc &rc)
{
    QString rowcolstr;
    QString mystr, tabname;

    kDebug() << "Reading View";

    tabname = instr;

    tabname.remove(0, 19);
    tabname.remove(tabname.length() - 2, 2);
    kDebug() << "  - Table name:" << tabname;

    bool ok = true;
    do {
        mystr = nextLine(stream);

        kDebug() << "" << mystr;
        if (mystr.startsWith("View End, Name:")) ok = false;
        else {
            // COLUMN Widths
            if (mystr.startsWith("View Column Widths")) {
                kDebug() << "   - Column Widths";
                mystr.remove(0, 20);
                kDebug() << "" << mystr;

                int  colwidth, icolumn;
                char ccolumn;

                // loop
                QStringList ColumnList;
                ColumnList = mystr.split(' ');

                for (QStringList::Iterator it = ColumnList.begin(); it != ColumnList.end(); ++it) {

                    sscanf((*it).toLatin1(), "%c:%d", &ccolumn, &colwidth);
                    int len = (*it).length();
                    int pos = (*it).indexOf(':');
                    (*it).remove(pos, len - pos);

                    printf("     >%s<- -<%c><%d>  \n", (*it).toLatin1().data(), ccolumn, colwidth);

                    // Transformat ascii column to int column
                    icolumn = translateColumnNumber(*it);

                    //icolumn = ccolumn - 64;
                    // Translate the column width right from applix to kspread
                    icolumn = icolumn * 5;


                    rowcolstr += "  <column width=\"" +
                                 QString::number(colwidth) +
                                 "\" column=\"" +
                                 QString::number(icolumn) +
                                 "\" >\n"
                                 "   <format/>\n"
                                 "  </column>\n";
                }
            }

            // ROW Heights
            else if (mystr.startsWith("View Row Heights")) {
                kDebug() << "   - Row Heights";
                mystr.remove(0, 17);
                kDebug() << "" << mystr;

                int irow, rowheight;

                // loop
                QStringList RowList;
                RowList = mystr.split(' ');

                for (QStringList::Iterator it = RowList.begin(); it != RowList.end(); ++it) {
                    sscanf((*it).toLatin1(), " %d:%d",
                           &irow, &rowheight);
                    printf("   row: %2d   height: %2d\n", irow, rowheight);
                    if (rowheight > 32768) rowheight -= 32768;
                    printf("              height: %2d\n", rowheight);
                    rowcolstr += "  <row row=\"" +
                                 QString::number(irow) +
                                 "\" height=\"" +
                                 QString::number(rowheight) +
                                 "\" >\n"
                                 "   <format/>\n"
                                 "  </row>\n";
                }


            }
        } // else != END COLORMAP
    } while (ok == true);

    // tabname append to my list
    // tabname append to my list
    rc.tabname.append(tabname);
    rc.rc.append(rowcolstr);

    printf("%s %s\n", tabname.toLatin1().data(),
           rowcolstr.toLatin1().data());

    printf("...done \n\n");
}





/******************************************************************************
 *  function:   filterSHFGBG                                                  *
 ******************************************************************************/
void
APPLIXSPREADImport::filterSHFGBG(const QString &it, int *style, int *bgcolor,
                                 int *fgcolor)
{
    QString tmpstr;
    int     pos;
    int     m2 = 0, m3 = 0;

    // filter SH = Brushstyle Background
    pos = it.indexOf("SH");
    if (pos > -1) {
        tmpstr = it;
        if (pos > 0)   tmpstr.remove(0, pos);
        pos = sscanf(tmpstr.toLatin1(), "SH%d",
                     style);

        printf("style: %d(%d)  ",
               *style, pos);
    }


    // filter FG = FGCOLOR
    pos = it.indexOf("FG");
    if (pos > -1) {
        tmpstr = it;
        if (pos > 0)   tmpstr.remove(0, pos);
        pos = sscanf(tmpstr.toLatin1(), "FG%d",
                     fgcolor);
        printf("fg: %d(%d)  ",
               *fgcolor, pos);
        m2 = 1;
    }


    // filter BG = BGCOLOR
    pos = it.indexOf("BG");
    if (pos > -1) {
        tmpstr = it;
        if (pos > 0)   tmpstr.remove(0, pos);
        pos = sscanf(tmpstr.toLatin1(), "BG%d",
                     bgcolor);
        printf("bgcolor: %d(%d)  ",
               *bgcolor, pos);
        m3 = 1;
    }


    printf("\n");


    // correct the bgcolor to the fgcolor if the background is plain
    if ((*style == 8) && (m2 == 1) && (m3 == 0)) {
        *bgcolor = *fgcolor;
    }


    // Translate brushstyle to kspread brushstyle
    if (*style != 0) {
        if (*style ==  1) *style =  0;
        else if (*style ==  2) *style =  7;
        else if (*style ==  3) *style =  0;
        else if (*style ==  4) *style =  4;
        else if (*style ==  5) *style =  3;
        else if (*style ==  6) *style =  2;
        else if (*style ==  7) *style =  0;
        else if (*style ==  8) *style =  0;
        else if (*style ==  9) *style = 10;
        else if (*style == 10) *style =  9;
        else if (*style == 11) *style = 11;
        else if (*style == 12) *style = 12;
        else if (*style == 13) *style = 13;
        else if (*style == 14) *style = 14;
        else if (*style == 15) *style =  0;
        else if (*style == 16) *style =  0;
        else if (*style == 17) *style =  0;
        else if (*style == 18) *style =  0;
        else if (*style == 19) *style =  0;
    }
}



/******************************************************************************
 *  function:   filterSHFGBG                                                  *
 ******************************************************************************/
void
APPLIXSPREADImport::transPenFormat(const QString &it, int *PenWidth, int *PenStyle)
{

    if (it[1] == '1') {
        *PenWidth = 1;
        *PenStyle = 1;
    }

    else if (it[1] == '2') {
        *PenWidth = 2;
        *PenStyle = 1;
    }

    else if (it[1] == '3') {
        *PenWidth = 3;
        *PenStyle = 1;
    }

    else if (it[1] == '4') {
        *PenWidth = 1;
        *PenStyle = 3;
    }

    else if (it[1] == '5') {
        *PenWidth = 5;
        *PenStyle = 1;
    }

    printf("frame (w:%d - s:%d) \n", *PenWidth, *PenStyle);
}




/******************************************************************************
 *  function: readHeader                                                       *
 ******************************************************************************/
int
APPLIXSPREADImport::readHeader(QTextStream &stream)
{
    QString mystr;
    int     vers[3] = { 0, 0, 0 };
    int     rueck;


    // Read Headline
    mystr = nextLine(stream);
    rueck = sscanf(mystr.toLatin1(),
                   "*BEGIN SPREADSHEETS VERSION=%d/%d ENCODING=%dBIT",
                   &vers[0], &vers[1], &vers[2]);
    printf("Versions info: %d %d %d\n", vers[0], vers[1], vers[2]);

    // Check the headline
    if (rueck <= 0) {
        printf("Header not correct - May be it is not an applixspreadsheet file\n");
        printf("Headerline: <%s>\n", mystr.toLatin1().data());

        QMessageBox::critical(0L, "Applix spreadsheet header problem",
                              QString("The Applix Spreadsheet header is not correct. "
                                      "May be it is not an applix spreadsheet file! <BR>"
                                      "This is the header line I did read:<BR><B>%1</B>").arg(mystr),
                              "Okay");


        return false;
    } else {
        return true;
    }
}


/******************************************************************************
 *  function: translateColumnNumber                                           *
 ******************************************************************************/
int
APPLIXSPREADImport::translateColumnNumber(const QString& colstr)
{
    int icol = 0;
    const int len = colstr.length();
    int p = len - 1;
    int x = 1;

    //kDebug() << "len=" << len;
    while (p >= 0) {
        //kDebug() << "x=" << x << "p=" << p << "char=" << colstr[p].toLatin1();
        const char c = colstr[p].toLatin1();
        // Upper chars
        if ((c >= 'A') && (c <= 'Z')) {
            //kDebug() << " UPPER";
            icol += ((int)pow((double)x, 26) * (c - 'A' + 1));
            ++x;
        }
        // lower chars
        else if ((c >= 'a') && (c <= 'z')) {
            //kDebug() << " lower";
            icol += ((int)pow((double)x, 26) * (c - 'a' + 1));
            ++x;
        }
        p--;
    }

    kDebug() << colstr << "->" << icol;
    return icol;
}

// Converts =SUM(F1,4) into =SUM(F1;4) -- well, plus possible nesting
QString APPLIXSPREADImport::convertFormula(const QString& input) const
{
    // Let me be stupid for now
    QString ret = input;
    ret.replace(',', ';');
    return ret;
}

#include "applixspreadimport.moc"
