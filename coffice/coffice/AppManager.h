#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QThread>

class Document;
class Page;

class AppManager : public QThread
{
public:
    AppManager();
    virtual ~AppManager();
    bool openFile(Document *doc, const QString &file);
    void update(Page *page);

protected:
    virtual void run();

private:
    class Private;
    Private *const d;
};

Q_GLOBAL_STATIC(AppManager, s_appManager)

#endif // APPMANAGER_H
