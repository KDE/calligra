#ifndef KIS_LINDENMAYER_SCRIPT_ERROR_REPEATER_H
#define KIS_LINDENMAYER_SCRIPT_ERROR_REPEATER_H

#include <QObject>
#include <QHash>
#include <QMutex>

class KisLindenmayerScriptErrorRepeater : public QObject
{
    Q_OBJECT

    static KisLindenmayerScriptErrorRepeater* m_instance;
    KisLindenmayerScriptErrorRepeater();

    QMutex m;
public:
    static KisLindenmayerScriptErrorRepeater* instance();
    static void destroy();
    void repeatErrors(QList<QPair<int, QString> > errors);

signals:
    void errorsOccured(QList<QPair<int, QString> > errors);
};

#endif // KIS_LINDENMAYER_SCRIPT_ERROR_REPEATER_H
