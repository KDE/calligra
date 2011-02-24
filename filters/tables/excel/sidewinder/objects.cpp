/* Swinder - Portable library for spreadsheet
   Copyright (C) 2009,2010 Sebastian Sauer <sebsauer@kdab.com>

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
   Boston, MA 02110-1301, USA
*/

#include "objects.h"

#include "generated/simpleParser.h"
#include "generated/leinputstream.h"
//#include "drawstyle.h"
//#include "ODrawToOdf.h"
//#include "pictures.h"

using namespace Swinder;

Object::Object(Type t, unsigned long id)
    : m_colL(0)
    , m_dxL(0)
    , m_rwT(0)
    , m_dyT(0)
    , m_colR(0)
    , m_dxR(0)
    , m_rwB(0)
    , m_dyB(0)
    , m_type(t)
    , m_id(id)
{
}

Object::~Object()
{
}

bool Object::applyDrawing(const MSO::OfficeArtDgContainer &container)
{
    bool ok = false;

    foreach(MSO::OfficeArtSpgrContainerFileBlock b, container.groupShape->rgfb) {
        std::cout << "Object(" << m_type << ") isOfficeArtSpContainer=" << b.anon.is<MSO::OfficeArtSpContainer>() << " isOfficeArtSpgrContainer=" << b.anon.is<MSO::OfficeArtSpgrContainer>() << " isOfficeArtFSP=" << b.anon.is<MSO::OfficeArtFSP>() << " isOfficeArtFSPGR=" << b.anon.is<MSO::OfficeArtFSPGR>() << " isOfficeArtClientAnchor=" << b.anon.is<MSO::OfficeArtClientAnchor>() << " isOfficeArtClientData=" << b.anon.is<MSO::OfficeArtClientData>() << std::endl;
        MSO::OfficeArtSpContainer* spc = b.anon.get<MSO::OfficeArtSpContainer>();
        if(spc && spc->shapePrimaryOptions) {

            // Extract the unique identifier for the drawing to be able to access the drawing again from outside.
            foreach(MSO::OfficeArtFOPTEChoice c, spc->shapePrimaryOptions->fopt) {
                if(c.anon.is<MSO::Pib>())
                    m_id = c.anon.get<MSO::Pib>()->pib;
                else if(c.anon.is<MSO::ITxid>())
                    m_id = c.anon.get<MSO::ITxid>()->iTxid;
            }

            // The drawing may attached to an anchor which contains the informations where our drawing will be located.
            MSO::XlsOfficeArtClientAnchor* anchor = spc->clientAnchor ? spc->clientAnchor->anon.get<MSO::XlsOfficeArtClientAnchor>() : 0;
            if(anchor) {
                m_colL = anchor->colL;
                m_dxL = anchor->dxL;
                m_rwT = anchor->rwT;
                m_dyT = anchor->dyT;
                m_colR = anchor->colR;
                m_dxR = anchor->dxR;
                m_rwB = anchor->rwB;
                m_dyB = anchor->dyB;
            }

            ok = true;
            continue;
        }

        MSO::OfficeArtSpgrContainer* spgr = b.anon.get<MSO::OfficeArtSpgrContainer>();
        if(spgr) {
            foreach(MSO::OfficeArtSpgrContainerFileBlock b, spgr->rgfb) {
                std::cout << "  spgr isOfficeArtSpContainer=" << b.anon.is<MSO::OfficeArtSpContainer>() << " isOfficeArtSpgrContainer=" << b.anon.is<MSO::OfficeArtSpgrContainer>() << " isOfficeArtFSP=" << b.anon.is<MSO::OfficeArtFSP>() << " isOfficeArtFSPGR=" << b.anon.is<MSO::OfficeArtFSPGR>() << " isOfficeArtClientAnchor=" << b.anon.is<MSO::OfficeArtClientAnchor>() << " isOfficeArtClientData=" << b.anon.is<MSO::OfficeArtClientData>() << std::endl;
                MSO::OfficeArtSpContainer* spc2 = b.anon.get<MSO::OfficeArtSpContainer>();
                if(spc2) {
                    std::cout << "     sp"
                              << " shapeGroup=" << (spc2->shapeGroup?1:0)
                              << " deletedshape=" << (spc2->deletedshape?1:0)
                              << " shapePrimaryOptions=" << (spc2->shapePrimaryOptions?1:0)
                              << " shapeSecondaryOptions1=" << (spc2->shapeSecondaryOptions1?1:0)
                              << " shapeTertiaryOptions1=" << (spc2->shapeTertiaryOptions1?1:0)
                              << " childAnchor=" << (spc2->childAnchor?1:0)
                              << " clientAnchor=" << (spc2->clientAnchor?1:0)
                              << " clientData=" << (spc2->clientData?1:0)
                              << " clientTextbox=" << (spc2->clientTextbox?1:0)
                              << " shapeSecondaryOptions2=" << (spc2->shapeSecondaryOptions2?1:0)
                              << " shapeTertiaryOptions2=" << (spc2->shapeTertiaryOptions2?1:0)
                              << std:: endl;
                }
            }
        }
    }

    return ok;
}

OfficeArtObject::OfficeArtObject(const MSO::OfficeArtSpContainer &object)
    : m_object(object)
{
}

OfficeArtObject::~OfficeArtObject()
{
}

MSO::OfficeArtSpContainer OfficeArtObject::object() const
{
    return m_object;
}

void OfficeArtObject::setText(const TxORecord &text)
{
    m_text = text;
}

TxORecord OfficeArtObject::text() const
{
    return m_text;
}
