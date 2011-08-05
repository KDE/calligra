#ifndef _KIS_TOOL_SIOXSELECTION_H_
#define _KIS_TOOL_SIOXSELECTION_H_

#include <QVector>

#include <kis_tool.h>
#include <KoToolFactoryBase.h>

class KisCanvas2;

class KisToolSioxSelection : public KisTool {
    Q_OBJECT
public:
    KisToolSioxSelection(KoCanvasBase * canvas);
    virtual ~KisToolSioxSelection();

    //
    // KisToolPaint interface
    //

    virtual quint32 priority() { return 3; }
    virtual void mousePressEvent(KoPointerEvent *event);
    virtual void mouseMoveEvent(KoPointerEvent *event);
    virtual void mouseReleaseEvent(KoPointerEvent *event);

// Uncomment if you have a configuration widget
//     QWidget* createOptionWidget();
//     virtual QWidget* optionWidget();

public slots:
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);
    void deactivate();

protected:
    
    virtual void paint(QPainter& gc, const KoViewConverter &converter);

protected:
    KisCanvas2* m_canvas;
};


class KisToolSioxSelectionFactory : public KoToolFactoryBase {

public:
    KisToolSioxSelectionFactory(const QStringList&)
        : KoToolFactoryBase("KisToolSioxSelection")
        {
            setToolTip( i18n( "SioxSelection" ) );
            setToolType( TOOL_TYPE_VIEW );
            setIcon( "tool_sioxselection" );
            setPriority( 0 );
        };


    virtual ~KisToolSioxSelectionFactory() {}

    virtual KoToolBase * createTool(KoCanvasBase * canvas) {
        return new KisToolSioxSelection(canvas);
    }

};


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
    typedef QVector< QVector < PixelT > > Type;

    ColorSignature() {}

private:

    /**
     * Stage one of clustering.
     */
    void stageOne(const Type& points, int depth, QVector< Type >& clusters,
        PixelT limits[], int length);

    /**
     * Stage two of clustering.
     */
    void stagetwo(const Type& points, int depth, QVector< Type >& clusters,
        PixelT limits[], int total, float threshold);

public:
    /**
     * Create a color signature for the given set of pixels.
     */
    Type createSignature(const Type& input, int length, PixelT limits[],
        float threshold);
};

template < typename PixelT >
void ColorSignature< PixelT >::stageOne(const Type& points, int depth, QVector< Type >& clusters,
    PixelT limits[], int length)
{
    if (length < 1) {
        return;
    }

    int dims = points[0].length;
    int curdim = depth % dims;
    float min = points[0][curdim];
    float max = points[0][curdim];

    // find maximum and minimum
    for (int i = 1; i < length; i++) {
        if (min > points[i][curdim]) {
            min = points[i][curdim];
        }
        if (max < points[i][curdim]) {
            max = points[i][curdim];
        }
    }

    if (max - min > limits[curdim]) { // Split according to Rubner-Rule
        // split
        float pivotvalue = ((max - min) / 2.0f) + min;

        int countsm = 0;
        int countgr = 0;

        for (int i = 0; i < length; i++) { // find out cluster sizes
            if (points[i][curdim] <= pivotvalue) {
                countsm++;
            } else {
                countgr++;
            }
        }

        Type smallerpoints[countsm][dims];
        Type biggerpoints[countgr][dims];

        int smallc = 0;
        int bigc = 0;

        for (int i = 0; i < length; i++) { // do actual split
            if (points[i][curdim] <= pivotvalue) {
                smallerpoints[smallc++] = points[i];
            } else {
                biggerpoints[bigc++] = points[i];
            }
        }

        // create subtrees
        stageOne(smallerpoints, depth + 1, clusters, limits, smallerpoints.length);
        stageOne(biggerpoints, depth + 1, clusters, limits, biggerpoints.length);

    } else {
        // create leave
        clusters.add(points);
    }
}

template < typename PixelT >
void ColorSignature< PixelT >::stagetwo(const Type& points, int depth, QVector< Type >& clusters,
    PixelT limits[], int total, float threshold)
{
    if (points.size() < 1) {
        return;
    }

    int dims = points[0].size() - 1; // without cardinality
    int curdim = depth % dims;
    float min = points[0][curdim];
    float max = points[0][curdim];

    // find maximum and minimum
    for (int i = 1; i < points.size(); i++) {
        if (min > points[i][curdim]) {
            min = points[i][curdim];
        }
        if (max < points[i][curdim]) {
            max = points[i][curdim];
        }
    }

    if (max - min > limits[curdim]) { // Split according to Rubner-Rule
        // split
        float pivotvalue = ((max - min) / 2.0f) + min;

        int countsm = 0;
        int countgr = 0;

        for (int i = 0; i < points.size(); i++) { // find out cluster sizes
            if (points[i][curdim] <= pivotvalue) {
                countsm++;
            } else {
                countgr++;
            }
        }

        Type smallerpoints[countsm][dims];
        Type biggerpoints[countgr][dims];

        int smallc = 0;
        int bigc = 0;

        for (int i = 0; i < points.size(); i++) { // do actual split
            if (points[i][curdim] <= pivotvalue) {
                smallerpoints[smallc++] = points[i];
            } else {
                biggerpoints[bigc++] = points[i];
            }
        }

        // create subtrees
        stagetwo(smallerpoints, depth + 1, clusters, limits, total, threshold);
        stagetwo(biggerpoints, depth + 1, clusters, limits, total, threshold);

    } else {
        // create leave
        int sum = 0;

        for (int i = 0; i < points.size(); i++) {
            sum += points[i][points[i].size()- 1];
        }

        if (((sum * 100.0) / total) >= threshold) {
            QVector< PixelT > point(points[0].size());

            for (int i = 0; i < points.size(); i++) {
                for (int j = 0; j < points[0].size(); j++) {
                    point[j] += points[i][j];
                }
            }

            for (int j = 0; j < points[0].size() - 1; j++) {
                point[j] /= points.size();
            }

            clusters.add(point);
        }
    }
}

template < typename PixelT >
typename ColorSignature< PixelT >::Type ColorSignature< PixelT >::createSignature(const Type& input,
    int length, PixelT limits[], float threshold)
{
    QVector< Type > clusters1, clusters2;

    stageOne(input, 0, clusters1, limits, length);

    Type centroids(clusters1.size());

    for (int i = 0; i < clusters1.size(); i++) {
        Type cluster(clusters1[i]);
        // +1 for the cardinality
        QVector< PixelT > centroid(cluster[0].size() + 1);

        for (int k = 0; k < cluster.length; k++) {
            for (int j = 0; j < cluster[k].length; j++) {
                centroid[j] += cluster[k][j];
            }
        }

        for (int j = 0; j < cluster[0].length; j++) {
            centroid[j] /= cluster.length;
        }

        centroid[cluster[0].size()] = cluster.size();
        centroids[i] = centroid;
    }

    stagetwo(centroids, 0, clusters2, limits, length, threshold);

    return clusters2;
}

#endif

