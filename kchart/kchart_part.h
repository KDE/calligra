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
  KChartPart( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
  ~KChartPart();

  virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );

  virtual bool initDoc();

  virtual void setData( const KoChart::Data& data );

  QStringList *axisLabelTextLong() { return &longLabels; }
  QStringList *axisLabelTextShort() { return &shortLabels; }

  bool showWizard();
  void initLabelAndLegend();
  void loadConfig(KConfig *conf);
  void saveConfig(KConfig *conf);
  void defaultConfig();
  KoChart::Data *data() {return &currentData; };
  KChartParams* params() const { return _params; };
  // save and load
  virtual QDomDocument saveXML();
  virtual bool loadXML( QIODevice *, const QDomDocument& doc );
  virtual bool loadOasis( const QDomDocument& doc, KoOasisStyles& oasisStyles, const QDomDocument& settings, KoStore* store );
  virtual bool saveOasis(KoStore*, KoXmlWriter*);

  bool m_bLoading; // Kalle: get rid of that one :)
  bool isLoading() {
    return m_bLoading;
  }
    bool canChangeValue() const { return m_bCanChangeValue;}
    virtual void setCanChangeValue(bool b ) { m_bCanChangeValue = b;}
 signals:
  void docChanged();

 protected:
  void initRandomData();
  virtual KoView* createViewInstance( QWidget* parent, const char* name );
  bool loadOldXML( const QDomDocument& doc );
  bool loadData( const QDomDocument& doc, KoChart::Data& currentData );

 private:
  QDomElement createElement(const QString &tagName, const QFont &font, QDomDocument &doc) const;
  QFont toFont(QDomElement &element) const;
  KoChart::Data currentData;
  QStringList longLabels, shortLabels;
  KChartParams* _params;
  QWidget* _parentWidget;
  bool m_bCanChangeValue;
};

class WizardExt : public KoChart::WizardExtension
{
public:
    WizardExt( KoChart::Part *part )
        : KoChart::WizardExtension( part ) {};

    virtual bool show() { return static_cast<KChartPart *>( part() )->showWizard(); }
};

#endif
