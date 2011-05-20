/* This file is part of the KDE libraries
 *
 * Copyright (c) 2003      Thierry Lorthiois (lorthioist@wanadoo.fr)
 *               2009-2011 Inge Wallin <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "WmfAbstractBackend.h"
#include "kowmfreadprivate.h"

#include <kdebug.h>

#include <QtCore/QFile>
#include <QtCore/QString>

WmfAbstractBackend::WmfAbstractBackend()
{
    mKwmf = new KoWmfReadPrivate();
}

WmfAbstractBackend::~WmfAbstractBackend()
{
    delete mKwmf;
}


bool WmfAbstractBackend::load(const QString& filename)
{
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly)) {
        kDebug() << "Cannot open file" << QFile::encodeName(filename);
        return false;
    }

    bool ret = mKwmf->load(file.readAll());
    file.close();

    return ret;
}


bool WmfAbstractBackend::load(const QByteArray& array)
{
    return mKwmf->load(array);
}


bool WmfAbstractBackend::play()
{
    return mKwmf->play(this);
}


bool WmfAbstractBackend::isValid(void) const
{
    return mKwmf->mValid;
}


bool WmfAbstractBackend::isStandard(void) const
{
    return mKwmf->mStandard;
}


bool WmfAbstractBackend::isPlaceable(void) const
{
    return mKwmf->mPlaceable;
}


bool WmfAbstractBackend::isEnhanced(void) const
{
    return mKwmf->mEnhanced;
}


QRect WmfAbstractBackend::boundingRect(void) const
{
    return QRect(QPoint(mKwmf->mBBoxLeft, mKwmf->mBBoxTop),
                 QSize(mKwmf->mBBoxRight - mKwmf->mBBoxLeft,
                       mKwmf->mBBoxBottom - mKwmf->mBBoxTop));
}


int WmfAbstractBackend::defaultDpi(void) const
{
    if (mKwmf->mPlaceable) {
        return mKwmf->mDpi;
    } else {
        return  0;
    }
}


void WmfAbstractBackend::setDebug(int nbrFunc)
{
    mKwmf->mNbrFunc = nbrFunc;
}

