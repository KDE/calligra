/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#ifndef KCHART_PART_H
#define KCHART_PART_H

#include <koDocument.h>
#include <ktable.h>
#include <kconfig.h>

#include <qvariant.h>

class KChartParameters;

struct KChartValue {
    QVariant value; // either a string (then it is interpreted as a
                    // label) or a double (then it is interpreted as a value
    bool exists;
};


typedef KTable<QString,QString,KChartValue> KChartData;

class KChartPart : public KoDocument
{
    Q_OBJECT
public:
    KChartPart( KoDocument* parent = 0, const char* name = 0 );
    ~KChartPart();

    virtual View* createView( QWidget* parent = 0, const char* name = 0 );
    virtual Shell* createShell();

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );

    virtual bool initDoc();

    virtual QCString mimeType() const;
    
    void setPart( const KChartData& data );
    void loadConfig(KConfig *conf);
    void saveConfig(KConfig *conf);
    KChartData *data() {return &currentData; };
    KChartParameters* params() const { return _params; };

signals:
    void docChanged();

protected:
    virtual QString configFile() const;

private:
    KChartData currentData;
    KChartParameters* _params;
};

#endif
