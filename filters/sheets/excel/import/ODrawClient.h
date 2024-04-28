/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef ODRAWCLIENT_H
#define ODRAWCLIENT_H

#include <ODrawToOdf.h>
#include <excel.h>

class KoStyleManager;

namespace Swinder
{
class Sheet;
}

class ODrawClient : public ODrawToOdf::Client
{
public:
    explicit ODrawClient(Swinder::Sheet *sheet);
    QRectF getRect(const MSO::OfficeArtClientAnchor &anchor) override;
    QRectF getReserveRect(void) override;
    QRectF getGlobalRect(const MSO::OfficeArtClientAnchor &anchor);
    QString getPicturePath(const quint32 pib) override;
    bool onlyClientData(const MSO::OfficeArtClientData &o) override;
    void processClientData(const MSO::OfficeArtClientTextBox *ct, const MSO::OfficeArtClientData &o, Writer &out) override;
    void processClientTextBox(const MSO::OfficeArtClientTextBox &ct, const MSO::OfficeArtClientData *cd, Writer &out) override;
    bool processRectangleAsTextBox(const MSO::OfficeArtClientData &cd) override;
    KoGenStyle createGraphicStyle(const MSO::OfficeArtClientTextBox *ct, const MSO::OfficeArtClientData *cd, const DrawStyle &ds, Writer &out) override;
    void addTextStyles(const MSO::OfficeArtClientTextBox *clientTextbox, const MSO::OfficeArtClientData *clientData, KoGenStyle &style, Writer &out) override;
    QColor toQColor(const MSO::OfficeArtCOLORREF &c) override;
    QString formatPos(qreal v) override;

    const MSO::OfficeArtDggContainer *getOfficeArtDggContainer() override;
    const MSO::OfficeArtSpContainer *getMasterShapeContainer(quint32 spid) override;

    void setStyleManager(KoStyleManager *styleManager);
    KoStyleManager *styleManager() const;

    void setZIndexAttribute(Writer &out);

    void setZIndex(quint32 zindex)
    {
        m_zIndex = zindex;
    }
    quint32 zIndex() const
    {
        return m_zIndex;
    }

    void setShapeText(const Swinder::TxORecord &text);

private:
    Swinder::Sheet *m_sheet;
    Swinder::TxORecord m_shapeText;
    quint32 m_zIndex;
    KoStyleManager *m_styleManager;
};

#endif // ODRAWCLIENT_H
