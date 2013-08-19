/*
    This file is part of the KDE libraries

    Copyright (C) 2008 Wang Hoi (zealot.hoi@gmail.com)

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
    Boston, MA 02110-1301, USA.

*/

#include "kencodingprober.h"

#include "probers/nsCharSetProber.h"
#include "probers/nsUniversalDetector.h"
#include "probers/ChineseGroupProber.h"
#include "probers/JapaneseGroupProber.h"
#include "probers/UnicodeGroupProber.h"
#include "probers/nsSBCSGroupProber.h"
#include "probers/nsMBCSGroupProber.h"

#include <string.h>

class KEncodingProberPrivate
{
public:
    KEncodingProberPrivate(): prober(NULL), mStart(true) {};
    ~KEncodingProberPrivate()
    {
        delete prober;
    }
    void setProberType(KEncodingProber::ProberType pType)
    {
        proberType = pType;
        /* handle multi-byte encodings carefully , because they're hard to detect,
        *   and have to use some Stastics methods.
        * for single-byte encodings (most western encodings), nsSBCSGroupProber is ok,
        *   because encoding state machine can detect many such encodings.
        */

        delete prober;

        switch (proberType) {
            case KEncodingProber::None:
                prober = NULL;
                break;
            case KEncodingProber::Arabic:
            case KEncodingProber::Baltic:
            case KEncodingProber::CentralEuropean:
            case KEncodingProber::Cyrillic:
            case KEncodingProber::Greek:
            case KEncodingProber::Hebrew:
            case KEncodingProber::NorthernSaami:
            case KEncodingProber::Other:
            case KEncodingProber::SouthEasternEurope:
            case KEncodingProber::Thai:
            case KEncodingProber::Turkish:
            case KEncodingProber::WesternEuropean:
                prober = new kencodingprober::nsSBCSGroupProber();
                break;
            case KEncodingProber::ChineseSimplified:
            case KEncodingProber::ChineseTraditional:
                prober = new kencodingprober::ChineseGroupProber();
                break;
            case KEncodingProber::Japanese:
                prober = new kencodingprober::JapaneseGroupProber();
                break;
            case KEncodingProber::Korean:
                prober = new kencodingprober::nsMBCSGroupProber();
                break;
            case KEncodingProber::Unicode:
                prober = new kencodingprober::UnicodeGroupProber();
                break;
            case KEncodingProber::Universal:
                prober = new kencodingprober::nsUniversalDetector();
                break;
            default:
                prober = NULL;
        }
    }
    void unicodeTest(const char *aBuf, int aLen)
    {
        if (mStart)
        {
            mStart = false;
            if (aLen > 3)
            switch (aBuf[0])
            {
                case '\xEF':
                    if (('\xBB' == aBuf[1]) && ('\xBF' == aBuf[2]))
                    // EF BB BF  UTF-8 encoded BOM
                    proberState = KEncodingProber::FoundIt;
                    break;
                case '\xFE':
                    if (('\xFF' == aBuf[1]) && ('\x00' == aBuf[2]) && ('\x00' == aBuf[3]))
                        // FE FF 00 00  UCS-4, unusual octet order BOM (3412)
                        proberState = KEncodingProber::FoundIt;
                    else if ('\xFF' == aBuf[1])
                        // FE FF  UTF-16, big endian BOM
                        proberState = KEncodingProber::FoundIt;
                        break;
                case '\x00':
                    if (('\x00' == aBuf[1]) && ('\xFE' == aBuf[2]) && ('\xFF' == aBuf[3]))
                        // 00 00 FE FF  UTF-32, big-endian BOM
                        proberState = KEncodingProber::FoundIt;
                    else if (('\x00' == aBuf[1]) && ('\xFF' == aBuf[2]) && ('\xFE' == aBuf[3]))
                        // 00 00 FF FE  UCS-4, unusual octet order BOM (2143)
                        proberState = KEncodingProber::FoundIt;
                        break;
                case '\xFF':
                    if (('\xFE' == aBuf[1]) && ('\x00' == aBuf[2]) && ('\x00' == aBuf[3]))
                        // FF FE 00 00  UTF-32, little-endian BOM
                        proberState = KEncodingProber::FoundIt;
                    else if ('\xFE' == aBuf[1])
                        // FF FE  UTF-16, little endian BOM
                        proberState = KEncodingProber::FoundIt;
                        break;
            }  // switch

        }
    }
    KEncodingProber::ProberType proberType;
    KEncodingProber::ProberState proberState;
    kencodingprober::nsCharSetProber *prober;
    bool mStart;
};

KEncodingProber::KEncodingProber(KEncodingProber::ProberType proberType): d(new KEncodingProberPrivate())
{
    setProberType(proberType);
}

KEncodingProber::~KEncodingProber()
{
    delete d;
}

void KEncodingProber::reset()
{
    d->proberState = KEncodingProber::Probing;
    d->mStart = true;
}

KEncodingProber::ProberState KEncodingProber::feed(const QByteArray &data)
{
    return feed(data.data(), data.size());
}

KEncodingProber::ProberState KEncodingProber::feed(const char* data, int len)
{
    if (!d->prober)
        return d->proberState;
    if (d->proberState == Probing) {
        if (d->mStart) {
            d->unicodeTest(data, len);
            if (d->proberState == FoundIt)
                return d->proberState;
        }
        d->prober->HandleData(data, len);
        switch (d->prober->GetState())
        {
            case kencodingprober::eNotMe:
                d->proberState = NotMe;
                break;
            case kencodingprober::eFoundIt:
                d->proberState = FoundIt;
                break;
            default:
                d->proberState = Probing;
                break;
        }
    }
#ifdef DEBUG_PROBE
    d->prober->DumpStatus();
#endif
    return d->proberState;
}

KEncodingProber::ProberState KEncodingProber::state() const
{
    return d->proberState;
}

QByteArray KEncodingProber::encoding() const
{
    if (!d->prober)
        return QByteArray("UTF-8");

    return QByteArray(d->prober->GetCharSetName());
}

float KEncodingProber::confidence() const
{
    if (!d->prober)
        return 0.0;

    return d->prober->GetConfidence();
}

KEncodingProber::ProberType KEncodingProber::proberType() const
{
    return d->proberType;
}

void KEncodingProber::setProberType(KEncodingProber::ProberType proberType)
{
    d->setProberType(proberType);
    reset();
}

KEncodingProber::ProberType KEncodingProber::proberTypeForName(const QString& lang)
{
    if (lang.isEmpty()) {
        return KEncodingProber::Universal;
    } else if (lang == tr("Disabled", "@item Text character set")) {
        return KEncodingProber::None;
    } else if (lang == tr("Universal", "@item Text character set")) {
        return KEncodingProber::Universal;
    } else if (lang == tr("Unicode", "@item Text character set")) {
        return KEncodingProber::Unicode;
    } else if (lang == tr("Cyrillic", "@item Text character set")) {
        return KEncodingProber::Cyrillic;
    } else if (lang == tr("Western European", "@item Text character set")) {
        return KEncodingProber::WesternEuropean;
    } else if (lang == tr("Central European", "@item Text character set")) {
        return KEncodingProber::CentralEuropean;
    } else if (lang == tr("Greek", "@item Text character set")) {
        return KEncodingProber::Greek;
    } else if (lang == tr("Hebrew", "@item Text character set")) {
        return KEncodingProber::Hebrew;
    } else if (lang == tr("Turkish", "@item Text character set")) {
        return KEncodingProber::Turkish;
    } else if (lang == tr("Japanese", "@item Text character set")) {
        return KEncodingProber::Japanese;
    } else if (lang == tr("Baltic", "@item Text character set")) {
        return KEncodingProber::Baltic;
    } else if (lang == tr("Chinese Traditional", "@item Text character set")) {
        return KEncodingProber::ChineseTraditional;
    } else if (lang == tr("Chinese Simplified", "@item Text character set")) {
        return KEncodingProber::ChineseSimplified;
    } else if (lang == tr("Korean", "@item Text character set")) {
        return KEncodingProber::Korean;
    } else if (lang == tr("Thai", "@item Text character set")) {
        return KEncodingProber::Thai;
    } else if (lang == tr("Arabic", "@item Text character set")) {
        return KEncodingProber::Arabic;
    }

    return KEncodingProber::Universal;
}

QString KEncodingProber::nameForProberType(KEncodingProber::ProberType proberType)
{
    switch (proberType)
    {
        case KEncodingProber::None:
            return tr("Disabled", "@item Text character set");
            break;
        case KEncodingProber::Universal:
            return tr("Universal", "@item Text character set");
            break;
        case KEncodingProber::Arabic:
            return tr("Arabic", "@item Text character set");
            break;
        case KEncodingProber::Baltic:
            return tr("Baltic", "@item Text character set");
            break;
        case KEncodingProber::CentralEuropean:
            return tr("Central European", "@item Text character set");
            break;
        case KEncodingProber::Cyrillic:
            return tr("Cyrillic", "@item Text character set");
            break;
        case KEncodingProber::Greek:
            return tr("Greek", "@item Text character set");
            break;
        case KEncodingProber::Hebrew:
            return tr("Hebrew", "@item Text character set");
            break;
        case KEncodingProber::Japanese:
            return tr("Japanese", "@item Text character set");
            break;
        case KEncodingProber::Turkish:
            return tr("Turkish", "@item Text character set");
            break;
        case KEncodingProber::WesternEuropean:
            return tr("Western European", "@item Text character set");
            break;
        case KEncodingProber::ChineseTraditional:
            return tr("Chinese Traditional", "@item Text character set");
            break;
        case KEncodingProber::ChineseSimplified:
            return tr("Chinese Simplified", "@item Text character set");
            break;
        case KEncodingProber::Korean:
            return tr("Korean", "@item Text character set");
            break;
        case KEncodingProber::Thai:
            return tr("Thai", "@item Text character set");
            break;
        case KEncodingProber::Unicode:
            return tr("Unicode", "@item Text character set");
            break;
        default:
            return QString();
        }
}
