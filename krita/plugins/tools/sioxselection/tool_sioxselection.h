#ifndef _SIOXSELECTION_H_
#define _SIOXSELECTION_H_

#include <QObject>
#include <QVariant>

class KisView;

class SioxSelectionPlugin : public QObject
{
    Q_OBJECT
public:
    SioxSelectionPlugin(QObject *parent, const QVariantList &);
    virtual ~SioxSelectionPlugin();

private:

    KisView * m_view;

};

#endif
