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

#include "../qrichtext_p.h"

#include "kohyphen.h"


#define DEBUG_HYPHENATOR 1

bool KoHyphenator::isHyphenated(KoTextString *string, int /*pos*/, int lastBreak)
{
    if (string->at(lastBreak).c != QChar(0xad))
    {
        return true;
    }
    return false;
}

KoHyphenator::KoHyphenator()
{
/*  Reading config for dictionary encodings from file...*/

    QString path = kapp->dirs()->findResource("data", "koffice/hyphdicts/dicts.xml");

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
            for (int i = 0; i < records.count(); i++)
            {
    			QDomNamedNodeMap attr = records.item(i).attributes();
                if (attr.contains("lang") && attr.contains("encoding"))
                    encodings[attr.namedItem("lang").nodeValue()] =
                        attr.namedItem("encoding").nodeValue();
            }
		}

    delete f;
}

KoHyphenator::~KoHyphenator()
{
    for (std::map<QString, HyphenDict*>::iterator it = dicts.begin(); it != dicts.end(); it++)
    {
        if (it->second != 0)
            hnj_hyphen_free(it->second);
    }
}

char *KoHyphenator::hyphens(QString str, QString lang)
{
    char *x;
    x = new char(str.length()+1);
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
        for (int j = 0; j < str.length(); j++)
            x[j] = '0';
        x[str.length()] = '\0';
    }
    return x;
}

QString KoHyphenator::hyphenate(QString str, QString lang)
{
    char x[str.length()];
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
        return str;
    }
    int i = 0, j = 0;
    for (; i < strlen(x); i++)
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
    return res;
}

bool KoHyphenator::checkHyphenPos(QString str, int pos, QString lang)
{
#ifdef DEBUG_HYPHENATOR
    qDebug("string: %s", str.latin1());
#endif

    char *hyph = hyphens(str, lang);

#ifdef DEBUG_HYPHENATOR
    qDebug("result: %s", hyph);
    qDebug("checked position: %d", pos);
#endif
    if ((hyph[pos] % 2) != 0)
        return true;
    else
        return false;
}

HyphenDict *KoHyphenator::dict(QString &lang)
{
    //only load dictionary when encoding info is present
    if (encodings[lang].isNull())
        throw KoHyphenatorException(i18n("No dictionary"));
    if (dicts[lang] == 0)
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
            dicts[lang] = hnj_hyphen_load(QFile::encodeName(path));
            if (dicts[lang] == 0)
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

const char * KoHyphenator::encodingForLang(QString lang)
{
    if (!encodings[lang].isNull())
        return encodings[lang].ascii();
    else
        return "utf8";
}
