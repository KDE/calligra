/* This file is part of the KDE project

   Copyright 1999-2007  Kalle Dalheimer <kalle@kde.org>
   Copyright 2005-2007  Inge Wallin <inge@lysator.liu.se>

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

// KDChart
#include "KDChartChart.h"

// KChart
#include "kchart_export.h"
#include "ChartShape.h"


// Some class references that don't need real includes.
class QStandardItemModel;

class KoXmlWriter;
class KoGenStyles;


namespace KChart
{



class KCHART_EXPORT KChartPart : public KoChart::Part
{
    Q_OBJECT

public:
    explicit KChartPart( QWidget *parentWidget = 0,
			 QObject* parent = 0,
			 bool singleViewMode = false );
    ~KChartPart();

    // Methods inherited from KoDocument:
    virtual void  paintContent( QPainter& painter, const QRect& rect);

    ChartShape *shape() const { return m_chartShape; }

    // Methods unique to KChart, and available in the new interface
    // (see /interfaces/koChart.h.)

    virtual void  resizeData( int rows, int columns );
    virtual void  setCellData( int row, int column, const QVariant &);
    virtual void  analyzeHeaders( );
    virtual void  setCanChangeValue( bool b )  { m_bCanChangeValue = b;    }

    // ----------------------------------------------------------------

    void  analyzeHeaders( const QStandardItemModel &data );
    void  doSetData( const QStandardItemModel &data,
		     bool  firstRowHeader,
		     bool  firstColHeader );

    void initLabelAndLegend();
    void loadConfig(KConfig *conf);
    void saveConfig(KConfig *conf);
    void defaultConfig();

    OdfChartType         chartType() const       { return m_chartShape->chartType();        }
    QStandardItemModel  *data()                  { return m_currentData; }
    KDChart::Chart      *chart()     const       { return m_chart;       }

    // Data in rows or columns.
    DataDirection  dataDirection() const    { return m_dataDirection; }
    void           setDataDirection( DataDirection _dir ) {
	m_dataDirection = _dir;
    }

    // First row / column as data or label?
    bool       firstRowAsLabel() const { return m_firstRowAsLabel; }
    void       setFirstRowAsLabel( bool _val );
    bool       firstColAsLabel() const { return m_firstColAsLabel; }
    void       setFirstColAsLabel( bool _val );

    // 
    QStringList       &rowLabelTexts()         { return m_rowLabels;  }
    QStringList       &colLabelTexts()         { return m_colLabels;  }

    // Save and load
    virtual QDomDocument  saveXML();
    virtual bool          loadXML( QIODevice *, const KoXmlDocument& doc );
    virtual bool          loadOasis( const KoXmlDocument& doc,
				     KoOasisStyles& oasisStyles,
				     const KoXmlDocument& settings,
				     KoStore *store );
    virtual bool          saveOasis( KoStore* store,
                                     KoXmlWriter* manifestWriter );

    bool  canChangeValue()   const             { return m_bCanChangeValue; }

    void  initNullChart();

    // Functions that generate templates (not used yet):
    void  generateBarChartTemplate();

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
    int          createDisplayData();
    void         createLabelsAndLegend( QStringList  &longLabels,
					QStringList  &shortLabels );


    QDomElement  createElement(const QString &tagName,
			       const QFont &font,
			       QDomDocument &doc) const;
    QFont        toFont(QDomElement &element)     const;

    void         setChartDefaults();

private:
    ChartShape    *m_chartShape;

    // ----------------------------------------------------------------
    // FIXME: Most of the following is already in shape/ChartShape or
    //        will move there soon.

    // The chart and its contents
    //OdfChartType             m_type;
    //OdfChartSubtype          m_subtype;
    KDChart::Chart          *m_chart;
    QStandardItemModel      *m_currentData;

    // Info about the data.
    DataDirection  m_dataDirection; // Rows or Columns
    bool           m_firstRowAsLabel;
    bool           m_firstColAsLabel;

    QStringList              m_rowLabels;
    QStringList              m_colLabels;
    //QString                  m_regionName;

    // Other auxiliary values
    bool                     m_bCanChangeValue;

    // Graphics
    QWidget                 *m_parentWidget;

    // Used when displaying.
    QStandardItemModel       m_displayData;

    QPixmap                  m_bufferPixmap;
};


}  //KChart namespace


#endif // KCHARTPART_H
