/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef SML_CONNECTOR_H
#define SML_CONNECTOR_H


#include "kivio_arrowhead.h"
#include "kivio_connector_point.h"
#include "kivio_custom_drag_data.h"
#include "kivio_intra_stencil_data.h"
#include "kiviostencilfactory.h"
#include "kivio_text_style.h"
#include "kivio_point.h"
#include "kivio_stencil.h"

class KivioPage;
class KivioShape;
#include <qstringlist.h>
#include <QColor>
#include <qdom.h>

#include "kivio_1d_stencil.h"


class KivioSMLConnector : public Kivio1DStencil
{
private:
	bool loadPath(const QString &file);
    //void drawOpenPath( KivioShape *, KivioIntraStencilData * );
    KivioShape *m_shape;
    QPtrList<KivioConnectorPoint> m_PointList;

    QString m_name;
protected:

    KivioArrowHead *m_startAH;
    KivioArrowHead *m_endAH;

    bool loadArrowHeads( const QDomElement & );
    QDomElement saveArrowHeads( QDomDocument & );
        virtual bool loadCustom( const QDomElement & );
    virtual bool saveCustom( QDomElement &, QDomDocument & );

public:
    KivioSMLConnector(const QString& name);
    virtual ~KivioSMLConnector();

    virtual void setStartPoint( double, double );
    virtual void setEndPoint( double, double );

    virtual KivioCollisionType checkForCollision( KivioPoint *, double );

    virtual KivioStencil* duplicate();

    virtual void paint( KivioIntraStencilData * );
    virtual void paintOutline( KivioIntraStencilData * );




    virtual void setStartAHType( int i )        { m_startAH->setType(i); }
    virtual void setStartAHWidth( double f )     { m_startAH->setWidth(f); }
    virtual void setStartAHLength( double f )    { m_startAH->setLength(f); }
    virtual void setEndAHType( int i )          { m_endAH->setType(i); }
    virtual void setEndAHWidth( double f )       { m_endAH->setWidth(f); }
    virtual void setEndAHLength( double f )      { m_endAH->setLength(f); }

    virtual int startAHType()                   { return m_startAH->type(); }
    virtual double startAHWidth()                { return m_startAH->width(); }
    virtual double startAHLength()               { return m_startAH->length(); }
    virtual int endAHType()                     { return m_endAH->type(); }
    virtual double endAHWidth()                  { return m_endAH->width(); }
    virtual double endAHLength()                 { return m_endAH->length(); }

};

class KivioSMLConnectorFactory : public KivioStencilFactory
{
	Q_OBJECT
    public:
    	KivioSMLConnectorFactory(QObject *parent=0, const char* name=0, const QStringList& args = QStringList());
	KivioStencil *NewStencil(const QString& name);
	KivioStencil *NewStencil();
    	QPixmap *GetIcon();
    	KivioStencilSpawnerInfo *GetSpawnerInfo();
};
#endif

