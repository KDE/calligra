/* This file is part of the KDE project

   Copyright 1999-2007  Kalle Dalheimer <kalle@kde.org>
   Copyright 2005-2007  Inge Wallin <inge@lysator.liu.se>
   Copyright 2007 Thorsten Zachmann <zachmann@kde.org>

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
   Boston, MA 02110-1301, USA.
*/


#ifndef KCHARTPART_H
#define KCHARTPART_H


// Local
#include "kchart_global.h"

// Qt
#include <QPixmap>
#include <QStandardItemModel>

// KDE
#include <kconfig.h>

// KOffice
#include <KoXmlReader.h>
#include <koChart.h>
#include <KoShapeControllerBase.h>

// KDChart
#include "KDChartChart"

// KChart
#include "kchart_export.h"
#include "ChartShape.h"


// Some class references that don't need real includes.
class QStandardItemModel;

class KoXmlWriter;
class KoGenStyles;
class KoDataCenterBase;

class KoShape;
class KoOdfStylesReader;

namespace KChart
{

class KCHARTCOMMON_EXPORT KChartPart : public KoChart::Part, public KoShapeControllerBase
{
    Q_OBJECT

public:
    explicit KChartPart( QWidget *parentWidget = 0,
			 QObject* parent = 0,
			 bool singleViewMode = false );
    ~KChartPart();

    // Methods inherited from KoDocument:
    virtual void  paintContent( QPainter& painter, const QRect& rect);

    virtual void addShape( KoShape* );
    virtual void removeShape( KoShape* );

    // Methods unique to KChart, and available in the new interface
    // (see /interfaces/koChart.h.)

    virtual void  setCanChangeValue( bool b )  { m_bCanChangeValue = b;    }

    // ----------------------------------------------------------------

    void  analyzeHeaders( const QStandardItemModel &data );

    void loadConfig(KConfig *conf);
    void saveConfig(KConfig *conf);
    void defaultConfig();

    ChartShape         *chart()     const { return m_chartShape;  }
    QStandardItemModel *data()      const { return m_chartData; }

    // Convenience functions: Types
    ChartType     chartType()    const { return m_chartShape->chartType(); }
    ChartSubtype  chartSubtype() const { return m_chartShape->chartSubType(); }

    // Save and load
    virtual QDomDocument  saveXML();
    virtual bool loadXML( const KoXmlDocument& doc, KoStore *store );
    virtual bool          loadOdf( KoOdfReadStore & odfStore );
    virtual bool          saveOdf( SavingContext &documentContext );

    bool  canChangeValue()   const             { return m_bCanChangeValue; }

    void  initNullChart();

    virtual bool showEmbedInitDialog(QWidget* parent);

public slots:
    void  slotModified();
    virtual void initEmpty();

signals:
    void docChanged();

protected:
    virtual KoView* createViewInstance( QWidget* parent );

    bool  loadOasisData( const KoXmlElement& tableElem );
    void writeAutomaticStyles( KoXmlWriter& contentWriter,
                               KoGenStyles& mainStyles ) const;

private:
    // Helper methods for painting.
    void         createLabelsAndLegend( QStringList  &longLabels,
					QStringList  &shortLabels );


    QDomElement  createElement(const QString &tagName,
			       const QFont &font,
			       QDomDocument &doc) const;
    QFont        toFont(QDomElement &element)     const;

    void         setChartDefaults();

private:
    ChartShape         *m_chartShape;
    QStandardItemModel *m_chartData;

    // Other auxiliary values
    bool                m_bCanChangeValue;
};


}  //KChart namespace


#endif // KCHARTPART_H
