/*
    kcodecaction.cpp

    Copyright (c) 2003 Jason Keirstead   <jason@keirstead.org>
    Copyrigth (c) 2006 Michel Hermier    <michel.hermier@gmail.com>
    Copyright (c) 2007 Nick Shaforostoff <shafff@ukr.net>

    ********************************************************************
    *                                                                  *
    * This library is free software; you can redistribute it and/or    *
    * modify it under the terms of the GNU Lesser General Public       *
    * License as published by the Free Software Foundation; either     *
    * version 2 of the License, or (at your option) any later version. *
    *                                                                  *
    * This library is distributed in the hope that it will be useful,  *
    * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
    * GNU Lesser General Public License for more details.              *
    *                                                                  *
    * You should have received a copy of the GNU Lesser General Public *
    * License along with this library; if not, write to the            *
    * Free Software Foundation, Inc., 51 Franklin Street,              *
    * Fifth Floor, Boston, MA  02110-1301  USA                         *
    *                                                                  *
    ********************************************************************
*/

#include "kcodecaction.h"

#include <kcharsets.h>
#include <klocalizedstring.h>

#include <QDebug>
#include <QMenu>
#include <QVariant>
#include <QtCore/QTextCodec>

// Acording to http://www.iana.org/assignments/ianacharset-mib
// the default/unknown mib value is 2.
#define MIB_DEFAULT 2

class KCodecAction::Private
{
public:
    Private(KCodecAction *parent)
        : q(parent),
          defaultAction(0),
          currentSubAction(0)
    {
    }

  	void init(bool);

    void _k_subActionTriggered(QAction*);

    KCodecAction *q;
    QAction *defaultAction;
    QAction *currentSubAction;
};

KCodecAction::KCodecAction(QObject *parent,bool showAutoOptions)
    : KSelectAction(parent)
    , d(new Private(this))
{
    d->init(showAutoOptions);
}

KCodecAction::KCodecAction(const QString &text, QObject *parent,bool showAutoOptions)
    : KSelectAction(text, parent)
    , d(new Private(this))
{
    d->init(showAutoOptions);
}

KCodecAction::KCodecAction(const QIcon &icon, const QString &text, QObject *parent,bool showAutoOptions)
    : KSelectAction(icon, text, parent)
    , d(new Private(this))
{
    d->init(showAutoOptions);
}

KCodecAction::~KCodecAction()
{
    delete d;
}

void KCodecAction::Private::init(bool showAutoOptions)
{
    q->setToolBarMode(MenuMode);
    defaultAction = q->addAction(i18nc("Encodings menu", "Default"));

    int i;
    foreach(const QStringList &encodingsForScript, KCharsets::charsets()->encodingsByScript())
    {
        KSelectAction* tmp = new KSelectAction(encodingsForScript.at(0),q);
        if (showAutoOptions)
        {
            KEncodingProber::ProberType scri = KEncodingProber::proberTypeForName(encodingsForScript.at(0));
            if (scri != KEncodingProber::None)
            {
                tmp->addAction(i18nc("Encodings menu","Autodetect"))->setData(QVariant((uint)scri));
                tmp->menu()->addSeparator();
            }
        }
        for (i=1; i<encodingsForScript.size(); ++i)
        {
            tmp->addAction(encodingsForScript.at(i));
        }
        q->connect(tmp,SIGNAL(triggered(QAction*)),q,SLOT(_k_subActionTriggered(QAction*)));
        tmp->setCheckable(true);
        q->addAction(tmp);
    }
    q->setCurrentItem(0);
}

int KCodecAction::mibForName(const QString &codecName, bool *ok) const
{
    // FIXME logic is good but code is ugly

    bool success = false;
    int mib = MIB_DEFAULT;
    KCharsets *charsets = KCharsets::charsets();

    if (codecName == d->defaultAction->text())
        success = true;
    else
    {
        QTextCodec *codec = charsets->codecForName(codecName, success);
        if (!success)
        {
            // Maybe we got a description name instead
            codec = charsets->codecForName(charsets->encodingForName(codecName), success);
        }

        if (codec)
            mib = codec->mibEnum();
    }

    if (ok)
        *ok = success;

    if (success)
        return mib;

    qWarning() << "Invalid codec name: "  << codecName;
    return MIB_DEFAULT;
}

QTextCodec *KCodecAction::codecForMib(int mib) const
{
    if (mib == MIB_DEFAULT)
    {
        // FIXME offer to change the default codec
        return QTextCodec::codecForLocale();
    }
    else
        return QTextCodec::codecForMib(mib);
}

void KCodecAction::actionTriggered(QAction *action)
{
//we don't want to emit any signals from top-level items
//except for the default one
    if (action==d->defaultAction)
    {
        emit triggered(KEncodingProber::Universal);
        emit defaultItemTriggered();
    }
}

void KCodecAction::Private::_k_subActionTriggered(QAction *action)
{
    if (currentSubAction==action)
        return;
    currentSubAction=action;
    bool ok = false;
    int mib = q->mibForName(action->text(), &ok);
    if (ok)
    {
        emit q->triggered(action->text());
        emit q->triggered(q->codecForMib(mib));
    }
    else
    {
        if (!action->data().isNull())
            emit q->triggered((KEncodingProber::ProberType) action->data().toUInt());
    }
}

QTextCodec *KCodecAction::currentCodec() const
{
    return codecForMib(currentCodecMib());
}

bool KCodecAction::setCurrentCodec( QTextCodec *codec )
{
    if (!codec)
        return false;

    int i,j;
    for (i=0;i<actions().size();++i)
    {
        if (actions().at(i)->menu())
        {
            for (j=0;j<actions().at(i)->menu()->actions().size();++j)
            {
                if (!j && !actions().at(i)->menu()->actions().at(j)->data().isNull())
                    continue;
                if (codec==KCharsets::charsets()->codecForName(actions().at(i)->menu()->actions().at(j)->text()))
                {
                    d->currentSubAction=actions().at(i)->menu()->actions().at(j);
                    d->currentSubAction->trigger();
                    return true;
                }
            }
        }
    }
    return false;

}

QString KCodecAction::currentCodecName() const
{
    return d->currentSubAction->text();
}

bool KCodecAction::setCurrentCodec( const QString &codecName )
{
    return setCurrentCodec(KCharsets::charsets()->codecForName(codecName));
}

int KCodecAction::currentCodecMib() const
{
    return mibForName(currentCodecName());
}

bool KCodecAction::setCurrentCodec( int mib )
{
    if (mib == MIB_DEFAULT)
        return setCurrentAction(d->defaultAction);
    else
        return setCurrentCodec(codecForMib(mib));
}

KEncodingProber::ProberType KCodecAction::currentProberType() const
{
    return d->currentSubAction->data().isNull()?
            KEncodingProber::None            :
            (KEncodingProber::ProberType)d->currentSubAction->data().toUInt();
}

bool KCodecAction::setCurrentProberType(KEncodingProber::ProberType scri)
{
    if (scri==KEncodingProber::Universal)
    {
        d->currentSubAction=d->defaultAction;
        d->currentSubAction->trigger();
        return true;
    }

    int i;
    for (i=0;i<actions().size();++i)
    {
        if (actions().at(i)->menu())
        {
            if (!actions().at(i)->menu()->actions().isEmpty()
                 &&!actions().at(i)->menu()->actions().at(0)->data().isNull()
                 &&actions().at(i)->menu()->actions().at(0)->data().toUInt()==(uint)scri
               )
            {
                d->currentSubAction=actions().at(i)->menu()->actions().at(0);
                d->currentSubAction->trigger();
                return true;
            }
        }
    }
    return false;
}

#include "moc_kcodecaction.cpp"
