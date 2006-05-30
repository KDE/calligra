// Copyright (c) 2000 Phil Thompson <phil@river-bank.demon.co.uk>
//
// This file contains the definition of the interface to the Kugar KPart.


#ifndef _KUGAR_PART_H
#define _KUGAR_PART_H

#include <KoDocument.h>

#include "mpagecollection.h"
#include "mreportengine.h"

class KInstance;
class KugarBrowserExtension;

class KugarPart: public KoDocument
{
    Q_OBJECT

public:
    KugarPart( QWidget *parentWidget = 0, QObject* parent = 0, bool singleViewMode = false );
    virtual ~KugarPart();

    virtual bool loadOasis( const QDomDocument&, KoOasisStyles&,
                            const QDomDocument&, KoStore* );
    virtual bool saveOasis( KoStore*, KoXmlWriter* );

    virtual bool loadXML( QIODevice *, const QDomDocument & );
    //  virtual QDomDocument saveXML();

    virtual void paintContent( QPainter&, const QRect&,
                               bool /*transparent*/ = false,
                               double /*zoomX*/ = 1.0, double /*zoomY*/ = 1.0 )
    {
        ;
    }

    Kugar::MReportEngine *reportEngine()
    {
        return m_reportEngine;
    }

protected:
    virtual KoView* createViewInstance( QWidget* parent, const char* name );

public slots:
    void setForcedUserTemplate( const QString &/*name*/ )
    {
        ;
    }

private slots:
    void slotPreferredTemplate( const QString & );

private:
    QString m_reportData;
    Kugar::MReportEngine *m_reportEngine;
    bool m_templateOk;
    KUrl m_docURL;
};


#endif
