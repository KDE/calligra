/***************************************************************************
                          kohyphen.cpp  -  description
                             -------------------
    begin                : Tue Sep 12 2002
    copyright            : (C) 2002 by Alexander Dymo <cloudtemple@mksat.net>
                           (c) 2002 Lukas Tinkl <lukas.tinkl@suse.cz>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qdom.h>
#include <qfile.h>
#include <qtextcodec.h>
#include <qstring.h>

#include <klocale.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <kstaticdeleter.h>

#include "kohyphen.h"


#define DEBUG_HYPHENATOR 1

KoHyphenator* KoHyphenator::s_self;
static KStaticDeleter<KoHyphenator> kohyphensd;

KoHyphenator* KoHyphenator::self()
{
    if ( !s_self )
        kohyphensd.setObject( s_self, new KoHyphenator );
    return s_self;
}

KoHyphenator::KoHyphenator()
{
/*  Reading config for dictionary encodings from file...*/

    QString path = kapp->dirs()->findResource("data", "koffice/hyphdicts/dicts.xml");
    qDebug("%s", path.latin1());

    QFile *f;
    if (!path.isNull())
        f = new QFile(path);
    else
        throw KoHyphenatorException(i18n("Could not create KoHyphenator instance."));

    QDomDocument config;
    QDomNodeList records;
    config.setContent(f);

    for (QDomNode n = config.firstChild(); !n.isNull(); n = n.nextSibling())
        if (n.nodeName() == "dicts")
        {
            records = n.childNodes();
            for (uint i = 0; i < records.count(); i++)
            {
                QDomNamedNodeMap attr = records.item(i).attributes();
                if (attr.contains("lang") && attr.contains("encoding"))
                    encodings.insert( attr.namedItem("lang").nodeValue(),
                                      attr.namedItem("encoding").nodeValue() );
            }
        }

    delete f;
}

KoHyphenator::~KoHyphenator()
{
    for (QMap<QString, HyphenDict*>::iterator it = dicts.begin(); it != dicts.end(); it++)
    {
        if ((*it) != 0)
            hnj_hyphen_free((*it));
    }
}

char *KoHyphenator::hyphens(const QString& str, const QString& lang) const
{
    char *x = new char[str.length()+1];
    try
    {
        QTextCodec *codec = QTextCodec::codecForName(encodingForLang(lang));
        hnj_hyphen_hyphenate(dict(lang), (const char *)(codec->fromUnicode(str)), str.length(), x);
    }
    catch (KoHyphenatorException &e)
    {
#ifdef DEBUG_HYPHENATOR
        qDebug(e.message().latin1());
#endif
        for (uint j = 0; j < str.length(); j++)
            x[j] = '0';
        x[str.length()] = '\0';
    }
    return x;
}

QString KoHyphenator::hyphenate(const QString& str, const QString& lang) const
{
    char* x = new char[str.length()+1];
    QString res = str;
    try
    {
        QTextCodec *codec = QTextCodec::codecForName(encodingForLang(lang));
        hnj_hyphen_hyphenate(dict(lang), (const char *)(codec->fromUnicode(str)), str.length(), x);
    }
    catch (KoHyphenatorException &e)
    {
#ifdef DEBUG_HYPHENATOR
        qDebug("%s", e.message().latin1());
#endif
        delete[] x;
        return str;
    }
    int i = 0, j = 0;
    int len = strlen(x);
    for (; i < len; i++)
    {
#ifdef DEBUG_HYPHENATOR
        qDebug("loop: i=%d, j=%d, x=%s, res=%s", i, j, x, res.latin1());
#endif
        if ((x[i] % 2) != 0)
        {
            res.insert(j+1, QChar(0xad));
            j++;
        }
        j++;
    }
    delete[] x;
    return res;
}

bool KoHyphenator::checkHyphenPos(const QString& str, int pos, const QString& lang) const
{
#ifdef DEBUG_HYPHENATOR
    qDebug("string: %s", str.latin1());
#endif

    char *hyph = hyphens(str, lang);

#ifdef DEBUG_HYPHENATOR
    qDebug("result: %s", hyph);
    qDebug("checked position: %d", pos);
#endif
    bool ret = ((hyph[pos] % 2) != 0);
    delete[] hyph;
    return ret;
}

HyphenDict *KoHyphenator::dict(const QString &_lang) const
{
    QString lang( _lang );
    //only load dictionary when encoding info is present
    if (encodings.find(lang) == encodings.end())
    {
        int underscore = lang.find('_');
        if ( underscore > -1 ) {
            lang.truncate( underscore );
            if (encodings.find(lang) == encodings.end())
                throw KoHyphenatorException(i18n("No dictionary for %1").arg(lang));
        }
        else
            throw KoHyphenatorException(i18n("No dictionary for %1").arg(lang));
    }
    if (dicts.find(lang) == dicts.end())
    {
#ifdef DEBUG_HYPHENATOR
        qDebug("Searching dictionary for '%s' language...", lang.latin1());
#endif
        QString path = kapp->dirs()->findResource("data", "koffice/hyphdicts/hyph_" + lang + ".dic");
        if (!path.isNull())
        {
#ifdef DEBUG_HYPHENATOR
            qDebug("Loading dictionary for '%s' language: path = %s", lang.latin1(), path.latin1());
#endif
            const_cast<KoHyphenator*>(this)->dicts.insert( lang, hnj_hyphen_load(QFile::encodeName(path)) );
            if (dicts.find(lang) == dicts.end())
            {
#ifdef DEBUG_HYPHENATOR
                qDebug("No dictionary loaded");
#endif
                throw(KoHyphenatorException(i18n("Could not load dictionary for the language: %1").arg(lang)));
            }
        }
        else
            throw(KoHyphenatorException(i18n("Could not load dictionary for the language: %1").arg(lang)));
    }
    return dicts[lang];
}

QCString KoHyphenator::encodingForLang(const QString& lang) const
{
    QMap<QString, QString>::ConstIterator it = encodings.find(lang);
    if (it != encodings.end())
        return (*it).latin1();
    else
        return "utf8";
}
