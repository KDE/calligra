#include <xmltree.h>
#include <xmltree.moc>
#include <qtextstream.h>
#include <qfile.h>

XMLTree::XMLTree()
  : QObject()
{
  QDomElement tmp;

  root = new QDomDocument(QString("XML"));

  doc = root->createElement(QString("DOC")); 
  doc.setAttribute(QString("author"), QString("OLEFilter"));
  doc.setAttribute(QString("email"), QString("unknown"));
  doc.setAttribute(QString("editor"), QString("KSpread"));
  doc.setAttribute(QString("mime"), QString("application/x-kspread"));
  root->appendChild(doc);

  paper = root->createElement(QString("PAPER"));
  paper.setAttribute(QString("format"), QString("A4"));
  paper.setAttribute(QString("orientation"), QString("Portrait"));
  doc.appendChild(paper);

  tmp = root->createElement(QString("PAPERBORDERS"));
  tmp.setAttribute(QString("left"), 20);
  tmp.setAttribute(QString("top"), 20);
  tmp.setAttribute(QString("right"), 20);
  tmp.setAttribute(QString("bottom"), 20);
  paper.appendChild(tmp);

  map = root->createElement(QString("MAP"));
  doc.appendChild(map);

  table[0] = root->createElement(QString("TABLE"));
  //table1.setAttribute(QString("name"), QString("Table1"));
  map.appendChild(table[0]);

  table[1] = root->createElement(QString("TABLE"));
  //table2.setAttribute(QString("name"), QString("Table2"));
  map.appendChild(table[1]);

  table[2] = root->createElement(QString("TABLE"));
  //table3.setAttribute(QString("name"), QString("Table3"));
  map.appendChild(table[2]);
  
  current = 0;
}

XMLTree::~XMLTree() 
{
  if(root) {
    delete root;
    root=0L;
  }
}

const QString XMLTree::part()
{
  QString s;
  QTextStream t(s, IO_WriteOnly);

  t << "<?xml version=\"1.0\"?>\n";
  doc.save(t);
  t << '\0';

  return s;
}
 
bool XMLTree::_1904(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_addin(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_addmenu(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_array(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_autofilter(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_autofilterinfo(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_backup(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_blank(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_bof(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_bookbool(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_boolerr(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_bottommargin(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_boundsheet(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_calccount(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_calcmode(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_cf(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_condfmt(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_codename(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_codepage(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_colinfo(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_cont(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_coordlist(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_country(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_crn(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_dbcell(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_dcon(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_dconbin(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_dconname(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_dconref(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_defaultrowheight(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_defcolwidth(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_delmenu(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_delta(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_dimensions(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_docroute(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_dsf(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_dv(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_dval(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_edg(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_eof(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_externcount(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_externname(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_externsheet(Q_UINT16 size, QDataStream& body)
{
  static int count;

  Q_INT8 length, skip; 
  body >> length;
  body >> skip;
  char *name = new char[length];
  body.readRawBytes(name, length);
  QString s(name);
  
  if (count == 0) {
    table[current++].setAttribute(QString("name"), s);
    count++;
  }
  else if (count == 1) {
    table[current++].setAttribute(QString("name"), s);
    count++;
  }
  else if (count == 2) {
    table[current++].setAttribute(QString("name"), s);
    count++;
  } 
  else if (count == 3 || count == 4) {
    current = 0;
    count++;
  } 
  else if (count == 5 || count == 6) {
    current = 1;
    count++;
  }
  else if (count == 7 || count == 8) {
    current = 2;
    count++;
  }

  delete name;
  return true;
}

bool XMLTree::_extsst(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_filepass(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_filesharing(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_filesharing2(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_filtermode(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_fngroupcount(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_fngroupname(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_font(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_footer(Q_UINT16 size, QDataStream& body)
{
  static int count;

  if (count == 0) {
    QDomElement e;
    Q_INT8 length; 
    body >> length;
    char *name = new char[length];
    body.readRawBytes(name, length);
    QString s(name);

    e = root->createElement(QString("FOOT"));
    e.setAttribute(QString("left"), QString(""));
    e.setAttribute(QString("center"), s);
    e.setAttribute(QString("right"), QString(""));
    paper.appendChild(e);
    count++;
  }

  return true;
}

bool XMLTree::_format(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_formula(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_gcw(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_gridset(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_guts(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_hcenter(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_header(Q_UINT16 size, QDataStream& body)
{
  static int count;

  if (count == 0) {
    QDomElement e;
    Q_INT8 length; 
    body >> length;
    char *name = new char[length];
    body.readRawBytes(name, length);
    QString s(name);

    e = root->createElement(QString("HEAD"));
    e.setAttribute(QString("left"), QString(""));
    e.setAttribute(QString("center"), s);
    e.setAttribute(QString("right"), QString(""));
    paper.appendChild(e);
    count++;
  }

  return true;
}

bool XMLTree::_hideobj(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_hlink(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_horizontalpagebreaks(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_imdata(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_index(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_interfaceend(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_interfacehdr(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_iteration(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_label(Q_UINT16 size, QDataStream& body)
{
  Q_INT16 row, column, skip, length;
  body >> row;
  body >> column;
  body >> skip;
  body >> length;
  char *name = new char[length];
  body.readRawBytes(name, length);
  QString s(name);
  
  QDomElement e = root->createElement(QString("CELL"));
  e.setAttribute(QString("row"), ++row);
  e.setAttribute(QString("column"), ++column);
  e.appendChild(root->createTextNode(QString(s)));
  table[current].appendChild(e);

  delete name;
  return true;
}

bool XMLTree::_labelsst(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_leftmargin(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_lhngraph(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_lhrecord(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_lpr(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_mms(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_msodrawing(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_msodrawinggroup(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_msodrawingselection(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_mulblank(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_mulrk(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_name(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_note(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_number(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_obj(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_objprotect(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_obproj(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_olesize(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_palette(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_pane(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_paramqry(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_password(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_pls(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_precision(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_printgridlines(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_printheaders(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_protect(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_prot4rev(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_qsi(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_recipname(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_refmode(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_refreshall(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_rightmargin(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_rk(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_row(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_rstring(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_saverecalc(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_scenario(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_scenman(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_scenprotect(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_scl(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_selection(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_setup(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_shrfmla(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sort(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sound(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sst(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_standardwidth(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_string(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_style(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sub(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_supbook(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxdb(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxdbex(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxdi(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxex(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxext(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxfdbtype(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxfilt(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxformat(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxformula(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxfmla(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxidstm(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxivd(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxli(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxname(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxpair(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxpi(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxrule(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxstring(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxselect(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxtbl(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxtbpg(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxtbrgiitm(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxvd(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxvdex(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxvi(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxview(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_sxvs(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_tabid(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_tabidconf(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_table(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_templt(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_topmargin(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_txo(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_uddesc(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_uncalced(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_userbview(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_usersviewbegin(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_usersviewend(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_useselfs(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_vcenter(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_verticalpagebreaks(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_window1(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_window2(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_windowprotect(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_writeaccess(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_writeprot(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_wsbool(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_xct(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_xf(Q_UINT16 size, QDataStream& body)
{
  return false;
}

bool XMLTree::_xl5modify(Q_UINT16 size, QDataStream& body)
{
  return false;
}

