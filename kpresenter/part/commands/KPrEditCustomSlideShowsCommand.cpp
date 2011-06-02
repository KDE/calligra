#include "KPrEditCustomSlideShowsCommand.h"
#include "KPrDocument.h"
#include "KoPAPageBase.h"
#include "KPrCustomSlideShowsModel.h"

KPrEditCustomSlideShowsCommand::KPrEditCustomSlideShowsCommand(
    KPrDocument * doc, KPrCustomSlideShowsModel *model, QString name, QList<KoPAPageBase *> newCustomShow, QUndoCommand *parent)
: QUndoCommand( parent )
, m_doc(doc)
, m_model(model)
, m_name(name)
, m_newCustomShow(newCustomShow)
, m_oldCustomShow(doc->customSlideShows()->getByName(name))
{
    setText( i18n("Edit custom slide shows") );
}

KPrEditCustomSlideShowsCommand::~KPrEditCustomSlideShowsCommand()
{

}

void KPrEditCustomSlideShowsCommand::redo()
{
    m_model->updateCustomShow(m_name, m_newCustomShow);
    m_deleteNewCustomShow = false;
}

void KPrEditCustomSlideShowsCommand::undo()
{
    m_model->updateCustomShow(m_name, m_oldCustomShow);
    m_deleteNewCustomShow = true;
}
