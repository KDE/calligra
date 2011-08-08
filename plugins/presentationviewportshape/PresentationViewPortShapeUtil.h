#include <QtCore/QPointF>
#include <QtCore/QSizeF>
#include <QtCore/QList>
#include <QtCore/QVector>
#include <QtCore/QString>

class PresentationViewPortShapeUtil
{
public:
  static QList<QPointF> createListOfPoints(const QSizeF& size);
  static void printIndentation(QString& stream, unsigned int indent);
  static QVector< QVector < int > > createAdjMatrix(int noOfPoints);
};