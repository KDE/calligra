
#include "kconfigviewstatesaver.h"

#include "kconfiggroup.h"

static const char selectionKey[] = "Selection";
static const char expansionKey[] = "Expansion";
static const char currentKey[] = "Current";
static const char scrollStateHorizontalKey[] = "HorizontalScroll";
static const char scrollStateVerticalKey[] = "VerticalScroll";

KConfigViewStateSaver::KConfigViewStateSaver(QObject *parent)
  : KViewStateSerializer(parent)
{

}

void KConfigViewStateSaver::restoreState(const KConfigGroup& configGroup)
{
  restoreSelection(configGroup.readEntry(selectionKey, QStringList()));
  restoreCurrentItem(configGroup.readEntry(currentKey, QString()));
  restoreExpanded(configGroup.readEntry(expansionKey, QStringList()));
  restoreScrollState(configGroup.readEntry(scrollStateVerticalKey, -1), configGroup.readEntry(scrollStateHorizontalKey, -1));

  KViewStateSerializer::restoreState();
}

void KConfigViewStateSaver::saveState(KConfigGroup& configGroup)
{
  if (selectionModel())
  {
    configGroup.writeEntry( selectionKey, selectionKeys() );
    configGroup.writeEntry( currentKey, currentIndexKey() );
  }

  if (view())
  {
    QStringList expansion = expansionKeys();

    configGroup.writeEntry( expansionKey, expansion );
  }

  if (view())
  {
    QPair<int, int> _scrollState = scrollState();
    configGroup.writeEntry( scrollStateVerticalKey, _scrollState.first );
    configGroup.writeEntry( scrollStateHorizontalKey, _scrollState.second );
  }
}
