/*
 *  koffice/kspread/filters/csv/csvfilter.cc
 *
 *  Comma-separated input filter for KSpread
 *
 *  Copyright (C) 1999 David Faure <faure@kde.org>
 *  Covered by the GPL license
 */

#include <qstring.h>
#include <iostream.h> // debug output

#include <csvfilter.h>
#include <csvfilter.moc>

#define CSV_DELIMITER ';' // I never understood why CSV is actually separated by semicolons...

// In fact, Excel's opinion of CSV is semicolons, 
// but Applix's is commas (in a broken way : thousands are separated by commas as well !!)
// I would recommend exporting from Applix using XLS instead, or ASCII grid (and s/<tab>/;/g)

CSVFilter::CSVFilter(QTextStream &_inputStream, const QString & _tableName) 
  : inputStream ( _inputStream ), // keep a copy
    tree ( _tableName ), // XMLTree constructor takes the table name as argument
    bReady( false )
{
}

CSVFilter::~CSVFilter() 
{
}

const bool CSVFilter::filter()
{
  bReady = true; // note that filter() has been called

  QString line;
  int sep, nextsep;
  while ( !inputStream.eof() )
  {
    line = inputStream.readLine(); // read one line of input
    if ( !line.isEmpty() )
    {
      sep = 0;
      while ( (nextsep = line.find( CSV_DELIMITER, sep ) ) >= 0 ) // find next ";"
      {
        // hmm... I'm afraid this method of parsing might be a bit slow...
        // And it doesn't support ';' nested in double quotes.
        // I'll write a state machine soon.
        //
        //   first_field;second_field;last_field
        //               ^ sep       ^ nextsep
        tree.cell( line.mid( sep, nextsep-sep ) ); // extract field
        sep = nextsep + 1;
      }
      // No more ';'. Store last cell in the line.
      //   first_field;second_field;last_field
      //                            ^ sep     ^ length
      tree.cell( line.right( line.length() - sep ) );
    }
    tree.newline();
  }

  bSuccess = true;
  return bSuccess;
}

const QString CSVFilter::part() 
{

  if(bReady && bSuccess) {
    cout << tree.part(); // debug output
    return tree.part();
  }
  else {
    QString str;
    str+="<?xml version=\"1.0\"?>\n"
      "<DOC author=\"Torben Weis\" email=\"weis@kde.org\" editor=\"KSpread\" mime=\"application/x-kspread\" >\n"
      "<PAPER format=\"A4\" orientation=\"Portrait\">\n"
      "<PAPERBORDERS left=\"20\" top=\"20\" right=\"20\" bottom=\"20\"/>\n"
      "<HEAD left=\"\" center=\"\" right=\"\"/>\n"
      "<FOOT left=\"\" center=\"\" right=\"\"/>\n"
      "</PAPER>\n"
      "<MAP>\n"
      "<TABLE name=\"Table1\">\n"
      "<CELL row=\"1\" column=\"1\">\n"
      "<FORMAT align=\"4\" precision=\"-1\" float=\"3\" floatcolor=\"2\" faktor=\"1\"/>\n"
      "Sorry :(\n"
      "</CELL>\n"
      "</TABLE>\n"
      "</MAP>\n"
      "</DOC>";
    return str;
  }
}
