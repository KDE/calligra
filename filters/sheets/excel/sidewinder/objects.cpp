/* Swinder - Portable library for spreadsheet
   SPDX-FileCopyrightText: 2009, 2010 Sebastian Sauer <sebsauer@kdab.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "objects.h"

#include "generated/leinputstream.h"
#include "generated/simpleParser.h"
// #include "drawstyle.h"
// #include "ODrawToOdf.h"
// #include "pictures.h"

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

Object::~Object() = default;

bool Object::applyDrawing(const MSO::OfficeArtDgContainer &container)
{
    bool ok = false;

    foreach (const MSO::OfficeArtSpgrContainerFileBlock &b, container.groupShape->rgfb) {
        qCDebug(lcSidewinder) << "Object(" << m_type << ") isOfficeArtSpContainer=" << b.anon.is<MSO::OfficeArtSpContainer>()
                              << "isOfficeArtSpgrContainer=" << b.anon.is<MSO::OfficeArtSpgrContainer>() << "isOfficeArtFSP=" << b.anon.is<MSO::OfficeArtFSP>()
                              << "isOfficeArtFSPGR=" << b.anon.is<MSO::OfficeArtFSPGR>()
                              << "isOfficeArtClientAnchor=" << b.anon.is<MSO::OfficeArtClientAnchor>()
                              << "isOfficeArtClientData=" << b.anon.is<MSO::OfficeArtClientData>();
        const MSO::OfficeArtSpContainer *spc = b.anon.get<MSO::OfficeArtSpContainer>();
        if (spc && spc->shapePrimaryOptions) {
            // Extract the unique identifier for the drawing to be able to access the drawing again from outside.
            foreach (const MSO::OfficeArtFOPTEChoice &c, spc->shapePrimaryOptions->fopt) {
                if (c.anon.is<MSO::Pib>())
                    m_id = c.anon.get<MSO::Pib>()->pib;
                else if (c.anon.is<MSO::ITxid>())
                    m_id = c.anon.get<MSO::ITxid>()->iTxid;
            }

            // The drawing may attached to an anchor which contains the information where our drawing will be located.
            MSO::XlsOfficeArtClientAnchor *anchor = spc->clientAnchor ? spc->clientAnchor->anon.get<MSO::XlsOfficeArtClientAnchor>() : nullptr;
            if (anchor) {
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

        const MSO::OfficeArtSpgrContainer *spgr = b.anon.get<MSO::OfficeArtSpgrContainer>();
        if (spgr) {
            foreach (const MSO::OfficeArtSpgrContainerFileBlock &b, spgr->rgfb) {
                qCDebug(lcSidewinder) << "  spgr isOfficeArtSpContainer=" << b.anon.is<MSO::OfficeArtSpContainer>()
                                      << "isOfficeArtSpgrContainer=" << b.anon.is<MSO::OfficeArtSpgrContainer>()
                                      << "isOfficeArtFSP=" << b.anon.is<MSO::OfficeArtFSP>() << "isOfficeArtFSPGR=" << b.anon.is<MSO::OfficeArtFSPGR>()
                                      << "isOfficeArtClientAnchor=" << b.anon.is<MSO::OfficeArtClientAnchor>()
                                      << "isOfficeArtClientData=" << b.anon.is<MSO::OfficeArtClientData>();
                const MSO::OfficeArtSpContainer *spc2 = b.anon.get<MSO::OfficeArtSpContainer>();
                if (spc2) {
                    qCDebug(lcSidewinder) << "     sp"
                                          << "shapeGroup=" << (spc2->shapeGroup ? 1 : 0) << "deletedshape=" << (spc2->deletedshape ? 1 : 0)
                                          << "shapePrimaryOptions=" << (spc2->shapePrimaryOptions ? 1 : 0)
                                          << "shapeSecondaryOptions1=" << (spc2->shapeSecondaryOptions1 ? 1 : 0)
                                          << "shapeTertiaryOptions1=" << (spc2->shapeTertiaryOptions1 ? 1 : 0) << "childAnchor=" << (spc2->childAnchor ? 1 : 0)
                                          << "clientAnchor=" << (spc2->clientAnchor ? 1 : 0) << "clientData=" << (spc2->clientData ? 1 : 0)
                                          << "clientTextbox=" << (spc2->clientTextbox ? 1 : 0)
                                          << "shapeSecondaryOptions2=" << (spc2->shapeSecondaryOptions2 ? 1 : 0)
                                          << "shapeTertiaryOptions2=" << (spc2->shapeTertiaryOptions2 ? 1 : 0);
                }
            }
        }
    }

    return ok;
}

OfficeArtObject::OfficeArtObject(const MSO::OfficeArtSpContainer &object, quint32 index)
    : m_object(object)
    , m_index(index)
{
}

OfficeArtObject::~OfficeArtObject() = default;

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

void OfficeArtObject::setIndex(quint32 index)
{
    m_index = index;
}

quint32 OfficeArtObject::index() const
{
    return m_index;
}
