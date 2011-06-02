#ifndef KPREDITCUSTOMSLIDESHOWSCOMMAND_H
#define KPREDITCUSTOMSLIDESHOWSCOMMAND_H

#include <QUndoCommand>
class KPrDocument;
class KoPAPageBase;
class KPrCustomSlideShowsModel;

class KPrEditCustomSlideShowsCommand: public QUndoCommand
{
public:
    KPrEditCustomSlideShowsCommand(KPrDocument * doc, KPrCustomSlideShowsModel *model, QString name, QList<KoPAPageBase *> newCustomShow, QUndoCommand *parent = 0 );

    virtual ~KPrEditCustomSlideShowsCommand();

    /// redo the command
    virtual void redo();
    /// revert the actions done in redo
    virtual void undo();

private:
    KPrDocument * m_doc;
    KPrCustomSlideShowsModel *m_model;
    QString m_name;
    QList<KoPAPageBase *> m_newCustomShow;
    QList<KoPAPageBase *> m_oldCustomShow;
    bool m_deleteNewCustomShow;

};

#endif // KPREDITCUSTOMSLIDESHOWSCOMMAND_H
