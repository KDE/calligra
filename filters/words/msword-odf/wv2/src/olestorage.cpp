/* This file is part of the wvWare 2 project
   SPDX-FileCopyrightText: 2001-2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02111-1307, USA.
*/

#include "olestorage.h"
#include "olestream.h"
#include "pole.h"
#include "wvlog.h"

using namespace wvWare;

OLEStorage::OLEStorage()
    : m_storage(nullptr)
    , m_fileName("")
{
}

OLEStorage::OLEStorage(const std::string &fileName)
    : m_storage(nullptr)
    , m_fileName(fileName)
{
}

OLEStorage::~OLEStorage()
{
    close(); // just in case
}

bool OLEStorage::open(Mode /*mode*/)
{
    if (!m_storage) {
        m_storage = new POLE::Storage(m_fileName.c_str());
    }

    return m_storage->open();
}

void OLEStorage::close()
{
    m_streams.clear();

    if (m_storage) {
        m_storage->close();
        delete m_storage;
        m_storage = nullptr;
    }
}

bool OLEStorage::isValid() const
{
    return (m_storage != nullptr);
}

OLEStreamReader *OLEStorage::createStreamReader(const std::string &stream)
{
    if (!m_storage)
        return nullptr;

    // the stream is deleted by m_storage
    POLE::Stream *poleStream = new POLE::Stream(m_storage, stream);
    OLEStreamReader *reader = new OLEStreamReader(poleStream, this);
    m_streams.push_back(reader);
    return reader;
}

OLEStreamWriter *OLEStorage::createStreamWriter(const std::string & /*stream*/)
{
    return nullptr;
}
