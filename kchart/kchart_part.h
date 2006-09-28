/**
 * Kalle Dalheimer <kalle@kde.org>
 */

#ifndef KCHART_PART_H
#define KCHART_PART_H


#include <kconfig.h>

#include <koChart.h>
#include "kchart_params.h"
#include "koffice_export.h"

class KoXmlWriter;
class KoGenStyles;
namespace KChart
{

class KChartParams;


class KCHART_EXPORT KChartPart : public KoChart::Part
{
    Q_OBJECT

public:
    KChartPart( QWidget *parentWidget = 0, const char *widgetName = 0,
		QObject* parent = 0, const char* name = 0,
		bool singleViewMode = false );
    ~KChartPart();

    // Methods inherited from KoDocument:

    virtual bool  initDoc(InitDocFlags flags, QWidget* parentWidget=0);

    virtual void  paintContent( QPainter& painter, const QRect& rect,
				bool transparent = false,
				double zoomX = 1.0, double zoomY = 1.0 );

    // Methods unique to KChart, and available in the new interface
    // (see ../interfaces/koChart.h.)

    virtual void  resizeData( int rows, int columns );
    virtual void  setCellData( int row, int column, const QVariant &);
    virtual void  analyzeHeaders( );
    virtual void  setCanChangeValue( bool b )  { m_bCanChangeValue = b;    }

    // ----------------------------------------------------------------

    void  analyzeHeaders( const KDChartTableData& data );
    void  doSetData( const KDChartTableData&  data,
		     bool  firstRowHeader,
		     bool  firstRowHeader );

    bool showWizard( QString &area );
    void initLabelAndLegend();
    void loadConfig(KConfig *conf);
    void saveConfig(KConfig *conf);
    void defaultConfig();

    KChartParams::ChartType  chartType() const { return (KChartParams::ChartType) params()->chartType(); }
    KDChartTableData  *data()                  { return &m_currentData; }
    KChartParams      *params() const          { return m_params;       }

    QStringList       &rowLabelTexts()         { return m_rowLabels;  }
    QStringList       &colLabelTexts()         { return m_colLabels;  }

    // Save and load
    virtual QDomDocument  saveXML();
    virtual bool          loadXML( QIODevice *, const QDomDocument& doc );
    virtual bool          loadOasis( const QDomDocument& doc,
				     KoOasisStyles& oasisStyles,
				     const QDomDocument& settings,
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
    virtual KoView* createViewInstance( QWidget* parent, const char* name );
    bool  loadOldXML( const QDomDocument& doc );
    bool  loadAuxiliary( const QDomDocument& doc );
    bool  loadData( const QDomDocument& doc, KDChartTableData& currentData );
    bool  loadOasisData( const QDomElement& tableElem );
    void  saveOasisData( KoXmlWriter* bodyWriter, KoGenStyles& mainStyles ) const;
    void writeAutomaticStyles( KoXmlWriter& contentWriter, KoGenStyles& mainStyles ) const;

private:
    // Helper methods for painting.
    int          createDisplayData();
    void         createLabelsAndLegend( QStringList  longLabels,
					QStringList  shortLabels );


    QDomElement  createElement(const QString &tagName,
			       const QFont &font,
			       QDomDocument &doc) const;
    QFont        toFont(QDomElement &element)     const;

    void         setChartDefaults();

private:
    // The chart and its contents
    KChartParams            *m_params;      // Everything about the chart
    KDChartTableData         m_currentData; // The data in the chart.
    QStringList              m_rowLabels;
    QStringList              m_colLabels;
    //QString                  m_regionName;

    // Other auxiliary values
    bool                     m_bCanChangeValue;

    // Graphics
    QWidget                 *m_parentWidget;

    // Used when displaying.
    KDChartTableData         m_displayData;

    QPixmap                  m_bufferPixmap;
};


class WizardExt : public KoChart::WizardExtension
{
public:
    WizardExt( KoChart::Part *part )
        : KoChart::WizardExtension( part ) {};

    virtual bool show( QString &area ) {
        return static_cast<KChartPart *>( part() )->showWizard( area );
    }
};

}  //KChart namespace

#endif
