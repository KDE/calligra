#include <qstring.h>

#include <xmltree.h>
#include <records.h>
#include <excelfilter.h>
#include <excelfilter.moc>

ExcelFilter::ExcelFilter(const myFile &mainStream) 
  : FilterBase() 
{

  filterOk=false;

  QByteArray a;
  a.setRawData((char*) mainStream.data, (int) mainStream.length);

  s = new QDataStream(a, IO_ReadOnly);
  s->setByteOrder(QDataStream::LittleEndian);

  tree = new XMLTree();
}

ExcelFilter::~ExcelFilter() 
{
  if(s) {
    delete s;
    s=0L;
  }

  if(tree) {
    delete tree;
    tree=0L;
  }
}

const bool ExcelFilter::filter()
{
// Sorry Percy - had to comment it out due to crashes :)
/*  QByteArray rec;
  Q_UINT16 opcode, size, i;

  bool ret;
  char *buffer = new char[MAX_RECORD_SIZE];

  while (!s->eof()) {
    *s >> opcode;
    *s >> size;
    if (size > MAX_RECORD_SIZE) 
      fatal("ExcelFilter: Record larger than MAX_RECORD_SIZE!");
    s->readRawBytes(buffer, size);
    rec.setRawData(buffer,size);

    QDataStream *body = new QDataStream(rec, IO_ReadOnly);
    body->setByteOrder(QDataStream::LittleEndian);

    for (i = 0; biff[i].opcode != opcode && biff[i].opcode != 0; i++);

    if (biff[i].opcode == opcode) {
      ret = (tree->*(biff[i].func))(size, *body);
    } 
    else {
      debug("ExcelFilter: Oops, unknown opcode %x.", opcode);
    }

    rec.resetRawData(buffer, size);
  }

  //tree->part();

  //delete body;
  //delete rec;
*/
  filterOk=false;
  return false;
}

const QString ExcelFilter::part() {

    if(filterOk)
        return tree->part();
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
