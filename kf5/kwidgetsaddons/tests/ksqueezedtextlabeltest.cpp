#include "ksqueezedtextlabel.h"
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>

int main( int argc, char **argv )
{
  QApplication::setApplicationName(QStringLiteral("test"));
  QApplication app(argc, argv);

  QWidget* box = new QWidget();
  QVBoxLayout* layout = new QVBoxLayout(box);

  KSqueezedTextLabel *l1 = new KSqueezedTextLabel( QStringLiteral("This is a rather long string"), box);
  KSqueezedTextLabel *l2 = new KSqueezedTextLabel( QStringLiteral("This is another long string, selectable by mouse"), box );
  l2->setTextElideMode( Qt::ElideRight );
  l2->setTextInteractionFlags(Qt::TextSelectableByMouse);
  KSqueezedTextLabel* urlLabel = new KSqueezedTextLabel(QStringLiteral("http://www.example.com/this/url/is/selectable/by/mouse"), box);
  urlLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

  layout->addWidget(l1);
  layout->addWidget(l2);
  layout->addWidget(urlLabel);
  layout->addWidget(new QLabel(QStringLiteral("This is a normal QLabel"), box));

  QLabel* selectableLabel = new QLabel(QStringLiteral("This is a normal QLabel, selectable by mouse"), box);
  selectableLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

  layout->addWidget(selectableLabel);

  box->show();

  return app.exec();
}
