#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <qtabdialog.h>

class KCommand;
class KPObject;
class KPrPage;
class KPresenterDoc;
class PenStyleWidget;
class BrushProperty;
class RectProperty;

class PropertyEditor : public QTabDialog
{
    Q_OBJECT

public:
    PropertyEditor( QWidget *parent = 0, const char *name = 0, KPrPage *page = 0, KPresenterDoc *doc = 0 );
    ~PropertyEditor();

    KCommand * getCommand();

private slots:
    void slotDone();

private:
    void setupTabs();
    void setupTabPen( bool configureLineEnds );
    void setupTabBrush();
    void setupTabRect();

    KPrPage *m_page;
    KPresenterDoc *m_doc;
    QPtrList<KPObject> m_objects;

    PenStyleWidget *m_penProperty;
    BrushProperty *m_brushProperty;
    RectProperty *m_rectProperty;

signals:
    void propertiesOk();
};

#endif /* PROPERTYEDITOR_H */
