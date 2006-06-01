/* This file is part of the KDE project
   Copyright (C) 2003 Percy Leonhardt

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef OOIMPRESSEXPORT_H
#define OOIMPRESSEXPORT_H

#include "stylefactory.h"

#include <qdom.h>

#include <KoFilter.h>

class QDomElement;
class KoStore;

class OoImpressExport : public KoFilter
{
    Q_OBJECT
public:
    OoImpressExport( QObject * parent,const QStringList & );
    virtual ~OoImpressExport();

    virtual KoFilter::ConversionStatus convert( const QByteArray & from,
                                                const QByteArray & to );

private:
    KoFilter::ConversionStatus openFile();

    void exportBody( QDomDocument & doccontent, QDomElement & body );
    void createDocumentMeta( QDomDocument & docmeta );
    void createDocumentStyles( QDomDocument & docstyles );
    void createDocumentContent( QDomDocument & doccontent );
    void createDocumentManifest( QDomDocument & docmanifest );
    void createDocumentSettings( QDomDocument & docsetting );
    void appendTextbox( QDomDocument & doc, QDomElement & source, QDomElement & target );
    void appendParagraph( QDomDocument & doc, QDomElement & source, QDomElement & target );
    void appendText( QDomDocument & doc, QDomElement & source, QDomElement & target );
    void appendLine( QDomDocument & doc, QDomElement & source, QDomElement & target );
    void appendRectangle( QDomDocument & doc, QDomElement & source, QDomElement & target );
    void appendEllipse( QDomDocument & doc, QDomElement & source, QDomElement & target, bool pieObject = false );
    void set2DGeometry( QDomElement & source, QDomElement & target, bool pieObject = false, bool multiPoint = false );
    void setLineGeometry( QDomElement & source, QDomElement & target );
    void appendPolyline( QDomDocument & doc, QDomElement & source, QDomElement & target,  bool polygone = false);
    void appendPicture( QDomDocument & doc, QDomElement & source, QDomElement & target );
    void createPictureList( QDomNode &pictures );
    void appendNote( QDomDocument & doc, QDomElement & source, QDomElement & target );
    void appendGroupObject( QDomDocument & doc, QDomElement & source, QDomElement & target );
    QString rotateValue( double val );
    QString pictureKey( QDomElement &element );
    void createHelpLine( QDomNode &helpline );
    void createAttribute( QDomNode &attributeValue );
    void appendObjects(QDomDocument & doccontent, QDomNode &objects, QDomElement &drawPage);

    int m_currentPage;
    int m_objectIndex;
    float m_pageHeight;
    StyleFactory m_styleFactory;
    QString m_masterPageStyle;
    QDomElement m_styles;
    QDomDocument m_maindoc;
    QDomDocument m_documentinfo;
    QMap<QString, QString> m_pictureLst;

    QString m_helpLine;
    int m_activePage;
    double m_gridX, m_gridY;
    bool m_snapToGrid;

    //load from kpresenter file format
    QMap<QString, QString> m_kpresenterPictureLst;
    int m_pictureIndex;
    KoStore *m_storeinp;
    KoStore *m_storeout;
};

#endif
