#ifndef _KIS_COLOR_SIGNATURE_H_
#define _KIS_COLOR_SIGNATURE_H_

#include <utility>
#include <QVector>

/**
 * Color signature based on clusterization of an image area.
 *
 * The clusterization is made by a modified kd-tree where the points are pixels
 * with coordinates in a certain colorspace. Instead of splitting a set at its
 * median, the rule is to divide the given interval into two equally sized
 * subintervals.
 *
 * TODO
 * - receive inputs in constructor
 * - define a tuple type for limits in stageOne and stageTwo
 * - document stageOne and stageTwo
 */
template<typename PixelT, typename SignatureT>
class ColorSignature {
private:
    typedef std::pair<QVector<PixelT> /* Centroid */, int /* Cardinality */ >
        CentroidPair;

    typedef QVector<CentroidPair> CentroidSet;

    // TODO - change to template paramenter.
    static const int POINT_LENGTH = 3;

    /**
      Stage one of clustering.
     */
    static void stageOne(QVector< QVector<const PixelT*> >& clusters,
        const QVector<const PixelT*>& points, float lLimit, float aLimit,
        float bLimit, quint32 depth);

    /**
      Stage two of clustering.
     */
    static void stageTwo(QVector< QVector<SignatureT> >& signature,
        const CentroidSet& points, float lLimit, float aLimit, float bLimit,
        int total, float threshold, int depth);

public:
    /**
      Create a color signature for the given set of pixels.
     */
    static void createSignature(QVector< QVector<SignatureT> >& signature,
        const QVector<const PixelT*>& input, float lLimit, float aLimit,
        float bLimit, float threshold);
};

template<typename PixelT, typename SignatureT>
void ColorSignature<PixelT, SignatureT>::stageOne(QVector< QVector<const PixelT*> >& clusters,
    const QVector<const PixelT*>& points, float lLimit, float aLimit,
    float bLimit, quint32 depth)
{
    if (points.size() < 1)
        return;

    const int currentDimension = depth % POINT_LENGTH;
    PixelT min = points[0][currentDimension];
    PixelT max = points[0][currentDimension];

    for (int i = 1; i < points.size(); i++) {
        if (min > points[i][currentDimension])
            min = points[i][currentDimension];

        if (max < points[i][currentDimension])
            max = points[i][currentDimension];
    }

    // TODO - verify correct type
    SignatureT limit = currentDimension == 0 ?
        lLimit : currentDimension == 1 ?
        aLimit : bLimit;

    // Split according to Rubner-Rule.
    // TODO - verify correct type
    if (max - min > limit) {
        // TODO - verify correct type
        PixelT pivotvalue = (max + min) / 2;

        QVector<const PixelT*> smallerpoints, biggerpoints;

        for (int i = 0; i < points.size(); i++) {
            if (points[i][currentDimension] <= pivotvalue) {
                smallerpoints.push_back(points[i]);
            } else {
                biggerpoints.push_back(points[i]);
            }
        }

        // Create subtrees.
        stageOne(clusters, smallerpoints, lLimit, aLimit, bLimit, depth + 1);
        stageOne(clusters, biggerpoints, lLimit, aLimit, bLimit, depth + 1);

    } else {
        // Create leave.
        clusters.push_back(points);
    }
}

template<typename PixelT, typename SignatureT>
void ColorSignature<PixelT, SignatureT>::stageTwo(QVector< QVector<SignatureT> >& signature,
    const CentroidSet& points, float lLimit, float aLimit,
    float bLimit, int total, float threshold, int depth)
{
    if (points.size() < 1)
        return;

    int currentDimension = depth % POINT_LENGTH;
    PixelT min = points[0].first[currentDimension];
    PixelT max = points[0].first[currentDimension];

    for (int i = 1; i < points.size(); i++) {
        if (min > points[i].first[currentDimension])
            min = points[i].first[currentDimension];

        if (max < points[i].first[currentDimension])
            max = points[i].first[currentDimension];
    }

    float limit = currentDimension == 0 ?
        lLimit : currentDimension == 1 ?
        aLimit : bLimit;

    // Split according to Rubner-Rule.
    // TODO - convert types before compare
    if (max - min > limit) {
        float pivotvalue = ((max - min) / 2.0f) + min;

        CentroidSet smallerpoints, biggerpoints;

        for (int i = 0; i < points.size(); i++) {
            if (points[i].first[currentDimension] <= pivotvalue)
                smallerpoints.push_back(points[i]);
            else
                biggerpoints.push_back(points[i]);
        }

        // Create subtrees.
        stageTwo(signature, smallerpoints, lLimit, aLimit, bLimit, total,
             threshold, depth + 1);
        stageTwo(signature, biggerpoints, lLimit, aLimit, bLimit, total,
             threshold, depth + 1);

    } else {
        // Create leave.
        int sum = 0;
        for (int i = 0; i < points.size(); i++)
            sum += points[i].second;

        if (((sum * 100.0) / total) >= threshold) {
            QVector<SignatureT> signaturePoint(POINT_LENGTH, 0);

            for (int i = 0; i < points.size(); i++) {
                for (int j = 0; j < POINT_LENGTH; j++)
                    signaturePoint[j] += points[i].first[j];
            }

            for (int j = 0; j < POINT_LENGTH; j++)
                signaturePoint[j] /= points.size();

            signature.push_back(signaturePoint);
        }
    }
}

template<typename PixelT, typename SignatureT>
void ColorSignature<PixelT, SignatureT>::createSignature(QVector< QVector<SignatureT> >& signature,
    const QVector<const PixelT*>& input, float lLimit, float aLimit,
    float bLimit, float threshold)
{
    QVector< QVector<const PixelT*> > clusterSet;

    // Zero is the recursion depth for kd-tree.
    stageOne(clusterSet, input, lLimit, aLimit, bLimit, 0);

    CentroidSet centroidSet(clusterSet.size());

    for (int i = 0; i < clusterSet.size(); i++) {
        QVector<const PixelT*>& cluster = clusterSet[i];

        QVector<PixelT>& centroid = centroidSet[i].first;
        centroid.fill(0, POINT_LENGTH);

        for (int k = 0; k < cluster.size(); k++) {
            for (int j = 0; j < POINT_LENGTH; j++)
                centroid[j] += cluster[k][j];
        }

        for (int j = 0; j < POINT_LENGTH; j++)
            centroid[j] /= cluster.size();

        centroidSet[i].second = cluster.size();
    }

    // Zero is the recursion depth for kd-tree.
    stageTwo(signature, centroidSet, lLimit, aLimit, bLimit, input.size(), threshold, 0);
}

#endif /* _KIS_COLOR_SIGNATURE_H_ */
