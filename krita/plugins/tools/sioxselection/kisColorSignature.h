#ifndef _KIS_COLOR_SIGNATURE_H_
#define _KIS_COLOR_SIGNATURE_H_

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
 * - receive inputs through iterators istead of data
 * - return a pointer or interator in getSignature
 * - avoid copies in functions
 * - define a tuple type for limits in stageOne and stageTwo
 * - document stageOne and stageTwo
 */
template < typename PixelT >
class ColorSignature {
public:
    typedef QVector< const PixelT* > Type;

    ColorSignature() {}

private:
    // TODO - change to template paramenter.
    static const int POINT_LENGTH = 3;

    /**
      Stage one of clustering.
     */
    void stageOne(QVector< Type >& clusters, const QVector< const PixelT* >& points,
        float lLimit, float aLimit, float bLimit, quint32 depth);

    /**
      Stage two of clustering.
     */
    void stageTwo(QVector< QVector< PixelT > >& clusters,
        const QVector< QVector< PixelT > >& points, float lLimit, float aLimit,
        float bLimit, int total, float threshold, int depth);

public:
    /**
      Create a color signature for the given set of pixels.
     */
    void createSignature(QVector< QVector< PixelT > >& signature,
        const QVector< const PixelT* >& input, float lLimit, float aLimit,
        float bLimit, float threshold);
};

template < typename PixelT >
void ColorSignature< PixelT >::stageOne(QVector< Type >& clusters,
    const QVector< const PixelT* >& points, float lLimit, float aLimit,
    float bLimit, quint32 depth)
{
    if (points.size() < 1)
        return;

    const int currentDimension = depth % POINT_LENGTH;
    PixelT min = points[0][currentDimension];
    PixelT max = points[0][currentDimension];

    // find maximum and minimum
    for (int i = 1; i < points.size(); i++) {
        if (min > points[i][currentDimension]) {
            min = points[i][currentDimension];
        }

        if (max < points[i][currentDimension]) {
            max = points[i][currentDimension];
        }
    }

    // TODO - the limit must be PixelT
    float limit = currentDimension == 0 ?
        lLimit : currentDimension == 1 ?
        aLimit : bLimit;

    // Split according to Rubner-Rule.
    if (max - min > limit) {
        PixelT pivotvalue = ((max - min) / 2.0f) + min;

        QVector< const PixelT* > smallerpoints, biggerpoints;

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

template < typename PixelT >
void ColorSignature< PixelT >::stageTwo(QVector< QVector< PixelT > >& clusters,
    const QVector< QVector< PixelT > >& points, float lLimit, float aLimit,
    float bLimit, int total, float threshold, int depth)
{
    // TODO - here, points has cardinality of each cluster. It must be moved to
    // another structure.
    // - points here are centroids. points type should be better defined.

    if (points.size() < 1)
        return;

    int currentDimension = depth % POINT_LENGTH;
    PixelT min = points[0][currentDimension];
    PixelT max = points[0][currentDimension];

    // find maximum and minimum
    for (int i = 1; i < points.size(); i++) {
        if (min > points[i][currentDimension]) {
            min = points[i][currentDimension];
        }

        if (max < points[i][currentDimension]) {
            max = points[i][currentDimension];
        }
    }

    float limit = currentDimension == 0 ?
        lLimit : currentDimension == 1 ?
        aLimit : bLimit;

    // Split according to Rubner-Rule.
    if (max - min > limit) {
        float pivotvalue = ((max - min) / 2.0f) + min;

        QVector< QVector< PixelT > > smallerpoints, biggerpoints;

        for (int i = 0; i < points.size(); i++) {
            if (points[i][currentDimension] <= pivotvalue) {
                smallerpoints.push_back(points[i]);
            } else {
                biggerpoints.push_back(points[i]);
            }
        }

        // Create subtrees.
        stageTwo(clusters, smallerpoints, lLimit, aLimit, bLimit, total,
             threshold, depth + 1);
        stageTwo(clusters, biggerpoints, lLimit, aLimit, bLimit, total,
             threshold, depth + 1);

    } else {
        // Create leave.
        // TODO - cardinality is important here.
        int sum = 0;

        for (int i = 0; i < points.size(); i++) {
            sum += points[i][points[i].size()- 1];
        }

        if (((sum * 100.0) / total) >= threshold) {
            QVector< PixelT > point(POINT_LENGTH + 1);
            point.fill(0);

            for (int i = 0; i < points.size(); i++) {
                for (int j = 0; j < point.size(); j++) {
                    point[j] += points[i][j];
                }
            }

            for (int j = 0; j < point.size() - 1; j++) {
                point[j] /= points.size();
            }

            clusters.push_back(point);
        }
    }
}

template < typename PixelT >
void ColorSignature< PixelT >::createSignature(QVector< QVector< PixelT > >& signature,
    const QVector< const PixelT* >& input, float lLimit, float aLimit,
    float bLimit, float threshold)
{
    QVector< Type > clusters1;

    // Zero is the recursion depth for kd-tree.
    stageOne(clusters1, input, lLimit, aLimit, bLimit, 0);

    // TODO - move cardinality to another structure
    // +1 for the cardinality
    QVector< QVector< PixelT > > centroidSet(clusters1.size());

    for (int i = 0; i < clusters1.size(); i++) {
        Type& cluster = clusters1[i];

        QVector< PixelT >& centroid = centroidSet[i];
        centroid.fill(0, POINT_LENGTH + 1);

        for (int k = 0; k < cluster.size(); k++) {
            for (int j = 0; j < POINT_LENGTH; j++) {
                centroid[j] += cluster[k][j];
            }
        }

        for (int j = 0; j < POINT_LENGTH; j++) {
            centroid[j] /= cluster.size();
        }

        centroid[POINT_LENGTH] = cluster.size();
    }

    // Zero is the recursion depth for kd-tree.
    stageTwo(signature, centroidSet, lLimit, aLimit, bLimit, input.size(), threshold, 0);
}

#endif /* _KIS_COLOR_SIGNATURE_H_ */
