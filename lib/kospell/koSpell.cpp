/* This file is part of the KDE libraries
   Copyright (C) 2002-2003 Laurent Montel <lmontel@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koSpell.h"
#include "koSpell.moc"
#include "koSconfig.h"

#ifdef HAVE_LIBASPELL
#include "koaspell.h"
#endif

#include "koispell.h"
#include <qtextcodec.h>
#include <kdebug.h>
#include "koSpelldlg.h"

KOSpell *KOSpell::createKoSpell( QWidget *parent, const QString &caption, QObject *receiver, const char *slot, KOSpellConfig *_kcs, bool modal, bool _autocorrect )
{
    KOSpellConfig *ksc;
    if (_kcs!=0)
        ksc = new KOSpellConfig (*_kcs);
    else
        ksc = new KOSpellConfig;

    int clt = ksc->client();
    kdDebug()<<" client :"<<clt<<endl;
    if( clt == KOS_CLIENT_ISPELL)
        return new KOISpell(parent, caption,
                            receiver, slot, ksc/*config all other parameter*/ );
#ifdef HAVE_LIBASPELL
    else if (clt == KOS_CLIENT_ASPELL)
        return new KOASpell(parent,caption,ksc,modal,_autocorrect);
#endif
#if 0
    //for the futur :)
    else if( clt == KOS_CLIENT_MYSPELL)
        getAvailDictsMyspell();
#endif
     return new KOISpell(parent, caption,
                            receiver, slot, ksc/*config all other parameter*/ );

}

int KOSpell::modalCheck( QString& text)
{
    return KOSpell::modalCheck( text, 0L );
}

int KOSpell::modalCheck( QString& text, KOSpellConfig * _ksc )
{
    KOSpellConfig *ksc;
    if (_ksc!=0)
        ksc = new KOSpellConfig (*_ksc);
    else
        ksc = new KOSpellConfig;
    int clt = ksc->client();
    kdDebug()<<" client :"<<clt<<endl;
    if( clt == KOS_CLIENT_ISPELL)
        return KOISpell::modalCheck( text, ksc );
#ifdef HAVE_LIBASPELL
    else if (clt == KOS_CLIENT_ASPELL)
        return KOASpell::modalCheck( text, ksc );
#endif
#if 0
    //for the futur :)
    else if( clt == KOS_CLIENT_MYSPELL)
        return KOMySpell::modalCheck( text, ksc );
#endif
     return KOISpell::modalCheck( text, ksc );

}


KOSpell::KOSpell(QWidget *_parent, const QString &caption,KOSpellConfig *kcs,
            bool modal, bool _autocorrect )
{
    parent=_parent;
    modaldlg=modal;
    autocorrect = _autocorrect;
    initSpell( kcs );
}

KOSpell::KOSpell( KOSpellConfig *_ksc )
{
    parent= 0L;
    modaldlg=false;
    autocorrect = false;
    initSpell( _ksc);
}

KOSpell::~KOSpell()
{
    delete ksconfig;
}

void KOSpell::initSpell( KOSpellConfig *_ksc )
{
    autoDelete=false;
    //won't be using the dialog in ksconfig, just the option values
    if (_ksc!=0)
        ksconfig = new KOSpellConfig (*_ksc);
    else
        ksconfig = new KOSpellConfig;

    codec = 0;
    switch (ksconfig->encoding())
    {
    case KOS_E_LATIN1:
        codec = QTextCodec::codecForName("ISO 8859-1");
        break;
    case KOS_E_LATIN2:
        codec = QTextCodec::codecForName("ISO 8859-2");
        break;
    case KOS_E_LATIN3:
        codec = QTextCodec::codecForName("ISO 8859-3");
        break;
    case KOS_E_LATIN4:
        codec = QTextCodec::codecForName("ISO 8859-4");
        break;
    case KOS_E_LATIN5:
        codec = QTextCodec::codecForName("ISO 8859-5");
        break;
    case KOS_E_LATIN7:
        codec = QTextCodec::codecForName("ISO 8859-7");
        break;
    case KOS_E_LATIN8:
        codec = QTextCodec::codecForName("ISO 8859-8");
        break;
    case KOS_E_LATIN9:
        codec = QTextCodec::codecForName("ISO 8859-9");
        break;
    case KOS_E_LATIN13:
        codec = QTextCodec::codecForName("ISO 8859-13");
        break;
    case KOS_E_LATIN15:
        codec = QTextCodec::codecForName("ISO 8859-15");
        break;
    case KOS_E_UTF8:
        codec = QTextCodec::codecForName("UTF-8");
        break;
    case KOS_E_KOI8R:
        codec = QTextCodec::codecForName("KOI8-R");
        break;
    case KOS_E_KOI8U:
        codec = QTextCodec::codecForName("KOI8-U");
        break;
    case KOS_E_CP1251:
        codec = QTextCodec::codecForName("CP1251");
        break;
    default:
        break;
    }

    kdDebug(750) << __FILE__ << ":" << __LINE__ << " Codec = " << (codec ? codec->name() : "<default>") << endl;

    // copy ignore list from ksconfig
    ignorelist += ksconfig->ignoreList();

    replacelist += ksconfig->replaceAllList();
    m_status = Starting;


}

void KOSpell::setIgnoreUpperWords(bool _ignore)
{
    m_bIgnoreUpperWords=_ignore;
}

void KOSpell::setIgnoreTitleCase(bool _ignore)
{
    m_bIgnoreTitleCase=_ignore;
}

KOSpellConfig KOSpell::ksConfig () const
{
    ksconfig->setIgnoreList(ignorelist);
    ksconfig->setReplaceAllList(replacelist);
    return *ksconfig;
}

void KOSpell::hide()
{
    if ( ksdlg )
        ksdlg->hide();
}

int KOSpell::heightDlg() const
{
    if ( ksdlg )
        return ksdlg->height();
    else
        return -1;
}

int KOSpell::widthDlg() const
{
    if ( ksdlg )
        return ksdlg->width();
    else
        return -1;
}

void KOSpell::moveDlg (int x, int y)
{
    if ( !ksdlg )
        return;
  QPoint pt (x,y), pt2;
  pt2=parent->mapToGlobal (pt);
  ksdlg->move (pt2.x(),pt2.y());
}
