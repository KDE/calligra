/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#ifndef KCHART_PART_H
#define KCHART_PART_H

#include <koChart.h>
#include <kconfig.h>

class KChartParameters;

class KChartPart : public KoChart::Part
{
  Q_OBJECT
    public:
  KChartPart( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
  ~KChartPart();

  virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );

  virtual bool initDoc();

  virtual void setData( const KoChart::Data& data );
  void showWizard();
  void initLabelAndLegend();
  void loadConfig(KConfig *conf);
  void saveConfig(KConfig *conf);
  void defaultConfig();
  KoChart::Data *data() {return &currentData; };
  KChartParameters* params() const { return _params; };
  // save and load
  virtual QDomDocument saveXML();
  virtual bool loadXML( QIODevice *, const QDomDocument& doc );
  bool m_bLoading; // Kalle: get rid of that one :)
  bool isLoading() {
    return m_bLoading;
  }

 signals:
  void docChanged();

 protected:
  void initRandomData();
  virtual KoView* createViewInstance( QWidget* parent, const char* name );

 private:
  QDomElement createElement(const QString &tagName, const QFont &font, QDomDocument &doc) const;
  QFont toFont(QDomElement &element) const;
  KoChart::Data currentData;
  KChartParameters* _params;
  QWidget* _parentWidget;
};

class WizardExt : public KoChart::WizardExtension
{
public:
    WizardExt( KoChart::Part *part )
        : KoChart::WizardExtension( part ) {};

    virtual void show() { static_cast<KChartPart *>( part() )->showWizard(); }
};

#endif
