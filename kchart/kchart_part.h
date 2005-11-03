/**
 * Kalle Dalheimer <kalle@kde.org>
 */

#ifndef KCHART_PART_H
#define KCHART_PART_H


#include <kconfig.h>

#include <koChart.h>
#include "koffice_export.h"

namespace KChart
{

class KChartParams;


// This struct only exists because the configuration editor needs
// access to it.
//
// You might wonder why this is a struct instead of a class. Well, if
// it was a class, we would have to provide all members with
// accessors, and then we would have won nothing.
//
struct KChartAuxiliary {
    typedef  enum {DataRows = 0, DataColumns = 1} DataDirection;

    DataDirection  m_dataDirection; // Rows or Columns
};


class KCHART_EXPORT KChartPart : public KoChart::Part
{
    Q_OBJECT

public:
    KChartPart( QWidget *parentWidget = 0, const char *widgetName = 0, 
		QObject* parent = 0, const char* name = 0, 
		bool singleViewMode = false );
    ~KChartPart();

    virtual void  paintContent( QPainter& painter, const QRect& rect,
				bool transparent = false,
				double zoomX = 1.0, double zoomY = 1.0 );

    virtual bool  initDoc(InitDocFlags flags, QWidget* parentWidget=0);

    virtual void  setData( const KoChart::Data& data );

    void  doSetData( const KoChart::Data&  data,
		     bool  hasRowHeader,
		     bool  hasRowHeader );

    bool showWizard();
    void initLabelAndLegend();
    void loadConfig(KConfig *conf);
    void saveConfig(KConfig *conf);
    void defaultConfig();

    KoChart::Data   *data()                    { return &m_currentData; }
    KChartParams    *params() const            { return m_params;       }
    KChartAuxiliary *auxdata()                 { return &m_auxiliary;   }
    QStringList     &rowLabelTexts()           { return m_rowLabels;  }
    QStringList     &colLabelTexts()           { return m_colLabels;  }

    // Save and load
    virtual QDomDocument  saveXML();
    virtual bool          loadXML( QIODevice *, const QDomDocument& doc );
    virtual bool          loadOasis( const QDomDocument& doc,
				     KoOasisStyles& oasisStyles,
				     const QDomDocument& settings,
				     KoStore *store );
    virtual bool          saveOasis(KoStore*, KoXmlWriter*);

    bool  canChangeValue()   const             { return m_bCanChangeValue; }
    virtual void  setCanChangeValue(bool b )   { m_bCanChangeValue = b;    }

    void  initNullChart();
    // Functions that generate templates (not used yet):
    void  generateBarChartTemplate();

public slots:
    void  slotModified();

signals:
    void docChanged();

protected:
    virtual KoView* createViewInstance( QWidget* parent, const char* name );
    bool  loadOldXML( const QDomDocument& doc );
    bool  loadAuxiliary( const QDomDocument& doc );
    bool  loadData( const QDomDocument& doc, KoChart::Data& currentData );

private:
    QDomElement  createElement(const QString &tagName,
			       const QFont &font, 
			       QDomDocument &doc) const;
    QFont        toFont(QDomElement &element)     const;

    void         setChartDefaults();

private:
    // The chart and its contents
    KoChart::Data  m_currentData;
    QStringList    m_rowLabels;
    QStringList    m_colLabels;
    KChartParams  *m_params;

    // Auxiliary values that are part of the document, and thus will
    // be included in saved files.
    KChartAuxiliary  m_auxiliary;

    // Auxiliary values
    bool           m_bCanChangeValue;

    // Graphics
    QWidget       *m_parentWidget;

    // Used when displaying.
    KoChart::Data  m_displayData;
};


class WizardExt : public KoChart::WizardExtension
{
public:
    WizardExt( KoChart::Part *part )
        : KoChart::WizardExtension( part ) {};

    virtual bool show() { return static_cast<KChartPart *>( part() )->showWizard(); }
};

}  //KChart namespace

#endif
