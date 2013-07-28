#ifndef WIKICLIENTDOCKER_H
#define WIKICLIENTDOCKER_H

// Base classes
#include <QDockWidget>

class KoCanvasBase;

class WikiClientDocker : public QDockWidget
{
    Q_OBJECT
public:
    explicit WikiClientDocker();
    virtual ~WikiClientDocker();
    
signals:
    
public slots:
    /// Called when the docker changes area
    void ondockLocationChanged(Qt::DockWidgetArea area);

private:
    class Private;
    Private * const d;
    
};

#endif // WIKICLIENTDOCKER_H
