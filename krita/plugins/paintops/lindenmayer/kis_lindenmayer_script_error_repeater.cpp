#include "kis_lindenmayer_script_error_repeater.h"

#include <QMetaType>

KisLindenmayerScriptErrorRepeater* KisLindenmayerScriptErrorRepeater::m_instance = 0;

KisLindenmayerScriptErrorRepeater::KisLindenmayerScriptErrorRepeater() : QObject()
{
    qRegisterMetaType<QList<QPair<int,QString> > >("QList<QPair<int,QString> >");
}

KisLindenmayerScriptErrorRepeater* KisLindenmayerScriptErrorRepeater::instance() {
//    m.lock();
    if(m_instance == 0) {
        m_instance = new KisLindenmayerScriptErrorRepeater();
    }

//    m.unlock();
    return m_instance;
}

void KisLindenmayerScriptErrorRepeater::destroy() {
//    m.lock();
    if(m_instance!=0)
        delete m_instance;

    m_instance = 0;
//    m.unlock();
}


void KisLindenmayerScriptErrorRepeater::repeatErrors(QList<QPair<int, QString> > errors) {
    QMutex m;

    m.lock();
    emit errorsOccured(errors);
    m.unlock();
}

