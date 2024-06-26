/* This file is part of the KDE libraries
 *
 * SPDX-FileCopyrightText: 2003 Thierry Lorthiois (lorthioist@wanadoo.fr)
 *               2009-2011 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-only
 */

#include "WmfAbstractBackend.h"
#include "WmfParser.h"

#include <VectorImageDebug.h>

#include <QFile>
#include <QString>

/**
   Namespace for Windows Metafile (WMF) classes
*/
namespace Libwmf
{

WmfAbstractBackend::WmfAbstractBackend()
{
    m_parser = new WmfParser();
}

WmfAbstractBackend::~WmfAbstractBackend()
{
    delete m_parser;
}

bool WmfAbstractBackend::load(const QString &filename)
{
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly)) {
        debugVectorImage << "Cannot open file" << QFile::encodeName(filename);
        return false;
    }

    bool ret = m_parser->load(file.readAll());
    file.close();

    return ret;
}

bool WmfAbstractBackend::load(const QByteArray &array)
{
    return m_parser->load(array);
}

bool WmfAbstractBackend::play()
{
    return m_parser->play(this);
}

bool WmfAbstractBackend::isValid(void) const
{
    return m_parser->mValid;
}

bool WmfAbstractBackend::isStandard(void) const
{
    return m_parser->mStandard;
}

bool WmfAbstractBackend::isPlaceable(void) const
{
    return m_parser->mPlaceable;
}

bool WmfAbstractBackend::isEnhanced(void) const
{
    return m_parser->mEnhanced;
}

QRect WmfAbstractBackend::boundingRect(void) const
{
    return QRect(QPoint(m_parser->mBBoxLeft, m_parser->mBBoxTop),
                 QSize(m_parser->mBBoxRight - m_parser->mBBoxLeft, m_parser->mBBoxBottom - m_parser->mBBoxTop));
}

int WmfAbstractBackend::defaultDpi(void) const
{
    if (m_parser->mPlaceable) {
        return m_parser->mDpi;
    } else {
        return 0;
    }
}

void WmfAbstractBackend::setDebug(int nbrFunc)
{
    m_parser->mNbrFunc = nbrFunc;
}
}
