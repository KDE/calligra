/*
 *  koffice/filters/excel97/xmltree.h
 *
 *  Copyright (C) 1999 Percy Leonhardt
 *
 */

#ifndef XMLTREE_H
#define XMLTREE_H

#include <qdom.h>
#include <qarray.h>
#include <qqueue.h>
#include <qobject.h>

const int BIFF_5_7 = 0x0500;
const int BIFF_8 = 0x0600;

class XMLTree : public QObject 
{

  Q_OBJECT

public:
  XMLTree();
  ~XMLTree();

  const QString part();
  const QDomElement getFont(Q_UINT16 xf);
  const QDomElement getFormat(Q_UINT16 xf);

  bool _1904(Q_UINT16 size, QDataStream& body);
  bool _addin(Q_UINT16 size, QDataStream& body);
  bool _addmenu(Q_UINT16 size, QDataStream& body);
  bool _array(Q_UINT16 size, QDataStream& body);
  bool _autofilter(Q_UINT16 size, QDataStream& body);
  bool _autofilterinfo(Q_UINT16 size, QDataStream& body);
  bool _backup(Q_UINT16 size, QDataStream& body);
  bool _blank(Q_UINT16 size, QDataStream& body);
  bool _bof(Q_UINT16 size, QDataStream& body);
  bool _bookbool(Q_UINT16 size, QDataStream& body);
  bool _boolerr(Q_UINT16 size, QDataStream& body);
  bool _bottommargin(Q_UINT16 size, QDataStream& body);
  bool _boundsheet(Q_UINT16 size, QDataStream& body);
  bool _calccount(Q_UINT16 size, QDataStream& body);
  bool _calcmode(Q_UINT16 size, QDataStream& body);
  bool _cf(Q_UINT16 size, QDataStream& body);
  bool _condfmt(Q_UINT16 size, QDataStream& body);
  bool _codename(Q_UINT16 size, QDataStream& body); 
  bool _codepage(Q_UINT16 size, QDataStream& body); 
  bool _colinfo(Q_UINT16 size, QDataStream& body);
  bool _cont(Q_UINT16 size, QDataStream& body);
  bool _coordlist(Q_UINT16 size, QDataStream& body);
  bool _country(Q_UINT16 size, QDataStream& body);
  bool _crn(Q_UINT16 size, QDataStream& body);
  bool _dbcell(Q_UINT16 size, QDataStream& body);
  bool _dcon(Q_UINT16 size, QDataStream& body);
  bool _dconbin(Q_UINT16 size, QDataStream& body);
  bool _dconname(Q_UINT16 size, QDataStream& body);
  bool _dconref(Q_UINT16 size, QDataStream& body);
  bool _defaultrowheight(Q_UINT16 size, QDataStream& body);
  bool _defcolwidth(Q_UINT16 size, QDataStream& body);
  bool _delmenu(Q_UINT16 size, QDataStream& body);
  bool _delta(Q_UINT16 size, QDataStream& body);
  bool _dimensions(Q_UINT16 size, QDataStream& body);
  bool _docroute(Q_UINT16 size, QDataStream& body);
  bool _dsf(Q_UINT16 size, QDataStream& body);
  bool _dv(Q_UINT16 size, QDataStream& body);
  bool _dval(Q_UINT16 size, QDataStream& body);
  bool _edg(Q_UINT16 size, QDataStream& body);
  bool _eof(Q_UINT16 size, QDataStream& body);
  bool _externcount(Q_UINT16 size, QDataStream& body);
  bool _externname(Q_UINT16 size, QDataStream& body);
  bool _externsheet(Q_UINT16 size, QDataStream& body);
  bool _extsst(Q_UINT16 size, QDataStream& body);
  bool _filepass(Q_UINT16 size, QDataStream& body);
  bool _filesharing(Q_UINT16 size, QDataStream& body);
  bool _filesharing2(Q_UINT16 size, QDataStream& body);
  bool _filtermode(Q_UINT16 size, QDataStream& body);
  bool _fngroupcount(Q_UINT16 size, QDataStream& body);
  bool _fngroupname(Q_UINT16 size, QDataStream& body);
  bool _font(Q_UINT16 size, QDataStream& body);
  bool _footer(Q_UINT16 size, QDataStream& body);
  bool _format(Q_UINT16 size, QDataStream& body);
  bool _formula(Q_UINT16 size, QDataStream& body);
  bool _gcw(Q_UINT16 size, QDataStream& body);
  bool _gridset(Q_UINT16 size, QDataStream& body);
  bool _guts(Q_UINT16 size, QDataStream& body);
  bool _hcenter(Q_UINT16 size, QDataStream& body);
  bool _header(Q_UINT16 size, QDataStream& body);
  bool _hideobj(Q_UINT16 size, QDataStream& body);
  bool _hlink(Q_UINT16 size, QDataStream& body);
  bool _horizontalpagebreaks(Q_UINT16 size, QDataStream& body);
  bool _imdata(Q_UINT16 size, QDataStream& body);
  bool _index(Q_UINT16 size, QDataStream& body);
  bool _interfaceend(Q_UINT16 size, QDataStream& body);
  bool _interfacehdr(Q_UINT16 size, QDataStream& body);
  bool _iteration(Q_UINT16 size, QDataStream& body);
  bool _label(Q_UINT16 size, QDataStream& body);
  bool _labelsst(Q_UINT16 size, QDataStream& body);
  bool _leftmargin(Q_UINT16 size, QDataStream& body);
  bool _lhngraph(Q_UINT16 size, QDataStream& body);
  bool _lhrecord(Q_UINT16 size, QDataStream& body);
  bool _lpr(Q_UINT16 size, QDataStream& body);
  bool _mms(Q_UINT16 size, QDataStream& body);
  bool _msodrawing(Q_UINT16 size, QDataStream& body);
  bool _msodrawinggroup(Q_UINT16 size, QDataStream& body);
  bool _msodrawingselection(Q_UINT16 size, QDataStream& body);
  bool _mulblank(Q_UINT16 size, QDataStream& body);
  bool _mulrk(Q_UINT16 size, QDataStream& body);
  bool _name(Q_UINT16 size, QDataStream& body);
  bool _note(Q_UINT16 size, QDataStream& body);
  bool _number(Q_UINT16 size, QDataStream& body);
  bool _obj(Q_UINT16 size, QDataStream& body);
  bool _objprotect(Q_UINT16 size, QDataStream& body);
  bool _obproj(Q_UINT16 size, QDataStream& body);
  bool _olesize(Q_UINT16 size, QDataStream& body);
  bool _palette(Q_UINT16 size, QDataStream& body);
  bool _pane(Q_UINT16 size, QDataStream& body);
  bool _paramqry(Q_UINT16 size, QDataStream& body);
  bool _password(Q_UINT16 size, QDataStream& body);
  bool _pls(Q_UINT16 size, QDataStream& body);
  bool _precision(Q_UINT16 size, QDataStream& body);
  bool _printgridlines(Q_UINT16 size, QDataStream& body);
  bool _printheaders(Q_UINT16 size, QDataStream& body);
  bool _protect(Q_UINT16 size, QDataStream& body);
  bool _prot4rev(Q_UINT16 size, QDataStream& body);
  bool _qsi(Q_UINT16 size, QDataStream& body);
  bool _recipname(Q_UINT16 size, QDataStream& body);
  bool _refmode(Q_UINT16 size, QDataStream& body);
  bool _refreshall(Q_UINT16 size, QDataStream& body);
  bool _rightmargin(Q_UINT16 size, QDataStream& body);
  bool _rk(Q_UINT16 size, QDataStream& body);
  bool _row(Q_UINT16 size, QDataStream& body);
  bool _rstring(Q_UINT16 size, QDataStream& body);
  bool _saverecalc(Q_UINT16 size, QDataStream& body);
  bool _scenario(Q_UINT16 size, QDataStream& body);
  bool _scenman(Q_UINT16 size, QDataStream& body);
  bool _scenprotect(Q_UINT16 size, QDataStream& body);
  bool _scl(Q_UINT16 size, QDataStream& body);
  bool _selection(Q_UINT16 size, QDataStream& body);
  bool _setup(Q_UINT16 size, QDataStream& body);
  bool _shrfmla(Q_UINT16 size, QDataStream& body);
  bool _sort(Q_UINT16 size, QDataStream& body);
  bool _sound(Q_UINT16 size, QDataStream& body);
  bool _sst(Q_UINT16 size, QDataStream& body);
  bool _standardwidth(Q_UINT16 size, QDataStream& body);
  bool _string(Q_UINT16 size, QDataStream& body);
  bool _style(Q_UINT16 size, QDataStream& body);
  bool _sub(Q_UINT16 size, QDataStream& body);
  bool _supbook(Q_UINT16 size, QDataStream& body);
  bool _sxdb(Q_UINT16 size, QDataStream& body);
  bool _sxdbex(Q_UINT16 size, QDataStream& body);
  bool _sxdi(Q_UINT16 size, QDataStream& body);
  bool _sxex(Q_UINT16 size, QDataStream& body);
  bool _sxext(Q_UINT16 size, QDataStream& body);
  bool _sxfdbtype(Q_UINT16 size, QDataStream& body);
  bool _sxfilt(Q_UINT16 size, QDataStream& body);
  bool _sxformat(Q_UINT16 size, QDataStream& body);
  bool _sxformula(Q_UINT16 size, QDataStream& body);
  bool _sxfmla(Q_UINT16 size, QDataStream& body);
  bool _sxidstm(Q_UINT16 size, QDataStream& body);
  bool _sxivd(Q_UINT16 size, QDataStream& body);
  bool _sxli(Q_UINT16 size, QDataStream& body);
  bool _sxname(Q_UINT16 size, QDataStream& body);
  bool _sxpair(Q_UINT16 size, QDataStream& body);
  bool _sxpi(Q_UINT16 size, QDataStream& body);
  bool _sxrule(Q_UINT16 size, QDataStream& body);
  bool _sxstring(Q_UINT16 size, QDataStream& body);
  bool _sxselect(Q_UINT16 size, QDataStream& body);
  bool _sxtbl(Q_UINT16 size, QDataStream& body);
  bool _sxtbpg(Q_UINT16 size, QDataStream& body);
  bool _sxtbrgiitm(Q_UINT16 size, QDataStream& body);
  bool _sxvd(Q_UINT16 size, QDataStream& body);
  bool _sxvdex(Q_UINT16 size, QDataStream& body);
  bool _sxvi(Q_UINT16 size, QDataStream& body);
  bool _sxview(Q_UINT16 size, QDataStream& body);
  bool _sxvs(Q_UINT16 size, QDataStream& body);
  bool _tabid(Q_UINT16 size, QDataStream& body);
  bool _tabidconf(Q_UINT16 size, QDataStream& body);
  bool _table(Q_UINT16 size, QDataStream& body);
  bool _templt(Q_UINT16 size, QDataStream& body);
  bool _topmargin(Q_UINT16 size, QDataStream& body);
  bool _txo(Q_UINT16 size, QDataStream& body);
  bool _uddesc(Q_UINT16 size, QDataStream& body);
  bool _uncalced(Q_UINT16 size, QDataStream& body);
  bool _userbview(Q_UINT16 size, QDataStream& body);
  bool _usersviewbegin(Q_UINT16 size, QDataStream& body);
  bool _usersviewend(Q_UINT16 size, QDataStream& body);
  bool _useselfs(Q_UINT16 size, QDataStream& body);
  bool _vcenter(Q_UINT16 size, QDataStream& body);
  bool _verticalpagebreaks(Q_UINT16 size, QDataStream& body);
  bool _window1(Q_UINT16 size, QDataStream& body);
  bool _window2(Q_UINT16 size, QDataStream& body);
  bool _windowprotect(Q_UINT16 size, QDataStream& body); 
  bool _writeaccess(Q_UINT16 size, QDataStream& body);
  bool _writeprot(Q_UINT16 size, QDataStream& body);
  bool _wsbool(Q_UINT16 size, QDataStream& body);
  bool _xct(Q_UINT16 size, QDataStream& body);
  bool _xf(Q_UINT16 size, QDataStream& body);
  bool _xl5modify(Q_UINT16 size, QDataStream& body);

private:
  struct format_rec {
    Q_UINT8 cch;
    char* rgch;
  };

  struct font_rec {
    Q_UINT16 dyHeight, 
      grbit, 
      icv, 
      bls, 
      sss;
    Q_UINT8 uls, 
      bFamily, 
      bCharSet,
      reserved,
      cch;
    char* rgch;
  };

  struct xf_rec {
    Q_UINT16 ifnt,
      ifmt,
      info1,
      info2,
      info3,
      info4,
      info5,
      info6,
      info7;
  };
  
  Q_UINT16 biff;
  QDomDocument *root;
  QArray<format_rec*> formats;
  QArray<font_rec*> fonts;
  QArray<xf_rec*> xfs;
  QQueue<format_rec> formats_q;
  QQueue<font_rec> fonts_q;
  QQueue<xf_rec> xfs_q;
  QQueue<QDomElement> tables;
  QDomElement doc, paper, map, *table;
};

#endif // XMLTREE_H
