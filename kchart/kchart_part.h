/**
 * Kalle Dalheimer <kalle@kde.org>
 */

#ifndef KCHART_PART_H
#define KCHART_PART_H


#include <koChart.h>
#include <kconfig.h>


class KChartParams;


class KChartPart : public KoChart::Part
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

    QStringList  *axisLabelTextLong()          { return &m_longLabels;  }
    QStringList  *axisLabelTextShort()         { return &m_shortLabels; }

    bool showWizard();
    void initLabelAndLegend();
    void loadConfig(KConfig *conf);
    void saveConfig(KConfig *conf);
    void defaultConfig();

    KoChart::Data  *data()                     { return &m_currentData; }
    KChartParams   *params() const             { return m_params;      }

    // Save and load
    virtual       QDomDocument saveXML();
    virtual bool  loadXML( QIODevice *, const QDomDocument& doc );
    virtual bool  loadOasis( const QDomDocument& doc,
			     KoOasisStyles& oasisStyles,
			     const QDomDocument& settings,
			     KoStore *store );
    virtual bool  saveOasis(KoStore*, KoXmlWriter*);

    bool  m_bLoading; // FIXME: Kalle: get rid of that one :)
    bool  isLoading()        const             { return m_bLoading;        }
    bool  canChangeValue()   const             { return m_bCanChangeValue; }
    virtual void  setCanChangeValue(bool b )   { m_bCanChangeValue = b;    }

signals:
    void docChanged();

protected:
    void  initRandomData();
    virtual KoView* createViewInstance( QWidget* parent, const char* name );
    bool  loadOldXML( const QDomDocument& doc );
    bool  loadData( const QDomDocument& doc, KoChart::Data& currentData );

private:
    QDomElement  createElement(const QString &tagName,
			       const QFont &font, 
			       QDomDocument &doc) const;
    QFont        toFont(QDomElement &element)     const;

private:
    // Members
    KoChart::Data  m_currentData;
    QStringList    m_longLabels;
    QStringList    m_shortLabels;
    KChartParams  *m_params;
    QWidget       *m_parentWidget;
    bool           m_bCanChangeValue;
};

class WizardExt : public KoChart::WizardExtension
{
public:
    WizardExt( KoChart::Part *part )
        : KoChart::WizardExtension( part ) {};

    virtual bool show() { return static_cast<KChartPart *>( part() )->showWizard(); }
};

#endif
