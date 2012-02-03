/*
 * Copyright (C) 2012 Cyrille Berger <cberger@cberger.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 sure
 * USA
 */
#include <QDeclarativeItem>

class KoColorProfile;
class KisDoc2;

class KtoCanvas : public QDeclarativeItem
{
public:
    KtoCanvas(QDeclarativeItem* parent = 0);
    virtual ~KtoCanvas();
    virtual void paint(QPainter* , const QStyleOptionGraphicsItem* , QWidget* );
    void setDocument(KisDoc2* _doc);
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
private:
    KisDoc2* m_doc;
    const KoColorProfile* m_displayProfile;
};