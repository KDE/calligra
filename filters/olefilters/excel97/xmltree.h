/* This file is part of the KDE project

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef XMLTREE_H
#define XMLTREE_H

#include <qdom.h>
#include <qarray.h>
#include <qqueue.h>
#include <qobject.h>
#include <qintdict.h>

const int BIFF_5_7 = 0x0500;
const int BIFF_8 = 0x0600;

class XMLTree:public QObject
{
public:
  XMLTree();
  ~XMLTree();

  const QDomDocument * const part();
  const QDomElement getFormat(Q_UINT16 xf);
  void getFont(Q_UINT16 xf, QDomElement &f, Q_UINT16 fontid);
  void getPen(Q_UINT16 xf, QDomElement &f, Q_UINT16 fontid);

  bool _1904(Q_UINT16, QDataStream&);
  bool _addin(Q_UINT16, QDataStream&);
  bool _addmenu(Q_UINT16, QDataStream&);
  bool _array(Q_UINT16, QDataStream&);
  bool _autofilter(Q_UINT16, QDataStream&);
  bool _autofilterinfo(Q_UINT16, QDataStream&);
  bool _backup(Q_UINT16, QDataStream&);
  bool _blank(Q_UINT16, QDataStream&);
  bool _bof(Q_UINT16 size, QDataStream& body);
  bool _bookbool(Q_UINT16, QDataStream&);
  bool _boolerr(Q_UINT16, QDataStream&);
  bool _bottommargin(Q_UINT16 size, QDataStream& body);
  bool _boundsheet(Q_UINT16 size, QDataStream& body);
  bool _calccount(Q_UINT16, QDataStream&);
  bool _calcmode(Q_UINT16, QDataStream&);
  bool _cf(Q_UINT16, QDataStream&);
  bool _condfmt(Q_UINT16, QDataStream&);
  bool _codename(Q_UINT16, QDataStream&);
  bool _codepage(Q_UINT16, QDataStream&);
  bool _colinfo(Q_UINT16 size, QDataStream& body);
  bool _cont(Q_UINT16, QDataStream&);
  bool _coordlist(Q_UINT16, QDataStream&);
  bool _country(Q_UINT16, QDataStream&);
  bool _crn(Q_UINT16, QDataStream&);
  bool _dbcell(Q_UINT16, QDataStream&);
  bool _dcon(Q_UINT16, QDataStream&);
  bool _dconbin(Q_UINT16, QDataStream&);
  bool _dconname(Q_UINT16, QDataStream&);
  bool _dconref(Q_UINT16, QDataStream&);
  bool _defaultrowheight(Q_UINT16, QDataStream&);
  bool _defcolwidth(Q_UINT16, QDataStream&);
  bool _delmenu(Q_UINT16, QDataStream&);
  bool _delta(Q_UINT16, QDataStream&);
  bool _dimensions(Q_UINT16, QDataStream&);
  bool _docroute(Q_UINT16, QDataStream&);
  bool _dsf(Q_UINT16, QDataStream&);
  bool _dv(Q_UINT16, QDataStream&);
  bool _dval(Q_UINT16, QDataStream&);
  bool _edg(Q_UINT16, QDataStream&);
  bool _eof(Q_UINT16, QDataStream&);
  bool _externcount(Q_UINT16, QDataStream&);
  bool _externname(Q_UINT16, QDataStream&);
  bool _externsheet(Q_UINT16, QDataStream&);
  bool _extsst(Q_UINT16, QDataStream&);
  bool _filepass(Q_UINT16, QDataStream&);
  bool _filesharing(Q_UINT16, QDataStream&);
  bool _filesharing2(Q_UINT16, QDataStream&);
  bool _filtermode(Q_UINT16, QDataStream&);
  bool _fngroupcount(Q_UINT16, QDataStream&);
  bool _fngroupname(Q_UINT16, QDataStream&);
  bool _font(Q_UINT16 size, QDataStream& body);
  bool _footer(Q_UINT16 size, QDataStream& body);
  bool _format(Q_UINT16 size, QDataStream& body);
  bool _formula(Q_UINT16, QDataStream&);
  bool _gcw(Q_UINT16, QDataStream&);
  bool _gridset(Q_UINT16, QDataStream&);
  bool _guts(Q_UINT16, QDataStream&);
  bool _hcenter(Q_UINT16, QDataStream&);
  bool _header(Q_UINT16 size, QDataStream& body);
  bool _hideobj(Q_UINT16, QDataStream&);
  bool _hlink(Q_UINT16, QDataStream&);
  bool _horizontalpagebreaks(Q_UINT16, QDataStream&);
  bool _imdata(Q_UINT16, QDataStream&);
  bool _index(Q_UINT16, QDataStream&);
  bool _interfaceend(Q_UINT16, QDataStream&);
  bool _interfacehdr(Q_UINT16, QDataStream&);
  bool _iteration(Q_UINT16, QDataStream&);
  bool _label(Q_UINT16 size, QDataStream& body);
  bool _labelsst(Q_UINT16 size, QDataStream& body);
  bool _leftmargin(Q_UINT16 size, QDataStream& body);
  bool _lhngraph(Q_UINT16, QDataStream&);
  bool _lhrecord(Q_UINT16, QDataStream&);
  bool _lpr(Q_UINT16, QDataStream&);
  bool _mms(Q_UINT16, QDataStream&);
  bool _msodrawing(Q_UINT16, QDataStream&);
  bool _msodrawinggroup(Q_UINT16, QDataStream&);
  bool _msodrawingselection(Q_UINT16, QDataStream&);
  bool _mulblank(Q_UINT16, QDataStream&);
  bool _mulrk(Q_UINT16 size, QDataStream& body);
  bool _name(Q_UINT16, QDataStream&);
  bool _note(Q_UINT16, QDataStream&);
  bool _number(Q_UINT16 size, QDataStream& body);
  bool _obj(Q_UINT16, QDataStream&);
  bool _objprotect(Q_UINT16, QDataStream&);
  bool _obproj(Q_UINT16, QDataStream&);
  bool _olesize(Q_UINT16, QDataStream&);
  bool _palette(Q_UINT16, QDataStream&);
  bool _pane(Q_UINT16, QDataStream&);
  bool _paramqry(Q_UINT16, QDataStream&);
  bool _password(Q_UINT16, QDataStream&);
  bool _pls(Q_UINT16, QDataStream&);
  bool _precision(Q_UINT16, QDataStream&);
  bool _printgridlines(Q_UINT16, QDataStream&);
  bool _printheaders(Q_UINT16, QDataStream&);
  bool _protect(Q_UINT16, QDataStream&);
  bool _prot4rev(Q_UINT16, QDataStream&);
  bool _qsi(Q_UINT16, QDataStream&);
  bool _recipname(Q_UINT16, QDataStream&);
  bool _refmode(Q_UINT16, QDataStream&);
  bool _refreshall(Q_UINT16, QDataStream&);
  bool _rightmargin(Q_UINT16 size, QDataStream& body);
  bool _rk(Q_UINT16 size, QDataStream& body);
  bool _row(Q_UINT16 size, QDataStream& body);
  bool _rstring(Q_UINT16, QDataStream&);
  bool _saverecalc(Q_UINT16, QDataStream&);
  bool _scenario(Q_UINT16, QDataStream&);
  bool _scenman(Q_UINT16, QDataStream&);
  bool _scenprotect(Q_UINT16, QDataStream&);
  bool _scl(Q_UINT16, QDataStream&);
  bool _selection(Q_UINT16, QDataStream&);
  bool _setup(Q_UINT16, QDataStream&);
  bool _shrfmla(Q_UINT16, QDataStream&);
  bool _sort(Q_UINT16, QDataStream&);
  bool _sound(Q_UINT16, QDataStream&);
  bool _sst(Q_UINT16 size, QDataStream& body);
  bool _standardwidth(Q_UINT16, QDataStream&);
  bool _string(Q_UINT16, QDataStream&);
  bool _style(Q_UINT16, QDataStream&);
  bool _sub(Q_UINT16, QDataStream&);
  bool _supbook(Q_UINT16, QDataStream&);
  bool _sxdb(Q_UINT16, QDataStream&);
  bool _sxdbex(Q_UINT16, QDataStream&);
  bool _sxdi(Q_UINT16, QDataStream&);
  bool _sxex(Q_UINT16, QDataStream&);
  bool _sxext(Q_UINT16, QDataStream&);
  bool _sxfdbtype(Q_UINT16, QDataStream&);
  bool _sxfilt(Q_UINT16, QDataStream&);
  bool _sxformat(Q_UINT16, QDataStream&);
  bool _sxformula(Q_UINT16, QDataStream&);
  bool _sxfmla(Q_UINT16, QDataStream&);
  bool _sxidstm(Q_UINT16, QDataStream&);
  bool _sxivd(Q_UINT16, QDataStream&);
  bool _sxli(Q_UINT16, QDataStream&);
  bool _sxname(Q_UINT16, QDataStream&);
  bool _sxpair(Q_UINT16, QDataStream&);
  bool _sxpi(Q_UINT16, QDataStream&);
  bool _sxrule(Q_UINT16, QDataStream&);
  bool _sxstring(Q_UINT16, QDataStream&);
  bool _sxselect(Q_UINT16, QDataStream&);
  bool _sxtbl(Q_UINT16, QDataStream&);
  bool _sxtbpg(Q_UINT16, QDataStream&);
  bool _sxtbrgiitm(Q_UINT16, QDataStream&);
  bool _sxvd(Q_UINT16, QDataStream&);
  bool _sxvdex(Q_UINT16, QDataStream&);
  bool _sxvi(Q_UINT16, QDataStream&);
  bool _sxview(Q_UINT16, QDataStream&);
  bool _sxvs(Q_UINT16, QDataStream&);
  bool _tabid(Q_UINT16, QDataStream&);
  bool _tabidconf(Q_UINT16, QDataStream&);
  bool _table(Q_UINT16, QDataStream&);
  bool _templt(Q_UINT16, QDataStream&);
  bool _topmargin(Q_UINT16 size, QDataStream& body);
  bool _txo(Q_UINT16, QDataStream&);
  bool _uddesc(Q_UINT16, QDataStream&);
  bool _uncalced(Q_UINT16, QDataStream&);
  bool _userbview(Q_UINT16, QDataStream&);
  bool _usersviewbegin(Q_UINT16, QDataStream&);
  bool _usersviewend(Q_UINT16, QDataStream&);
  bool _useselfs(Q_UINT16, QDataStream&);
  bool _vcenter(Q_UINT16, QDataStream&);
  bool _verticalpagebreaks(Q_UINT16, QDataStream&);
  bool _window1(Q_UINT16, QDataStream&);
  bool _window2(Q_UINT16, QDataStream&);
  bool _windowprotect(Q_UINT16, QDataStream&);
  bool _writeaccess(Q_UINT16 size, QDataStream& body);
  bool _writeprot(Q_UINT16, QDataStream&);
  bool _wsbool(Q_UINT16, QDataStream&);
  bool _xct(Q_UINT16, QDataStream&);
  bool _xf(Q_UINT16 size, QDataStream& body);
  bool _xl5modify(Q_UINT16, QDataStream&);

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
    QString rgch;
  };

  struct xf_rec {
    Q_UINT16 ifnt,
      ifmt,
      attr,
      align,
      indent,
      borderStyle,
      sideBColor;
    Q_UINT32 topBColor;
    Q_UINT16 cellColor;
  };

  Q_UINT16 biff;

  QDomDocument *root;

  QIntDict<xf_rec> xfs;
  QIntDict<QString> sst;
  QIntDict<font_rec> fonts;
  QIntDict<format_rec> formats;

  QQueue<QDomElement> tables;

  QDomElement doc, paper, map, *table;

  // To avoid static variables:
  int fontCount, footerCount, headerCount, xfCount;
};

#endif
