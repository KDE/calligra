#include <cfloat>
#include <boost/tuple/tuple.hpp>
#include <kis_iterator_ng.h>
#include <kis_paint_device.h>
#include <kis_types.h>
#include <KoColorSpaceMaths.h>
#include <KoColorModelStandardIds.h>
#include "kisColorSignature.h"
#include "kisSioxSegmentator.h"


namespace {


/**
  Returns the squared euclidian distance between two Lab color pixels.
 */
template<typename Point1T, typename Point2T>
inline quint64 getLabColorDiffSquared(const Point1T& point1, const Point2T& point2);

/**
  Blurs confidence matrix with a given symmetrically weighted kernel. In the
  standard case confidence matrix entries are between 0...1 and the weight
  factors sum up to 1.
 */
void smoothCondidenceMatrix(float matrix[], int xres, int yres, float weight1,
    float weight2, float weight3);

/**
  Normalizes the matrix to values to [0..1].
 */
void normalizeConfidenceMatrix(float confidenceMatrix[], int length);

void rotatePointers(quint8 **p, quint32 n) {
    quint32 i;
    quint8  *p0 = p[0];
    for (i = 0; i < n - 1; i++) {
        p[i] = p[i + 1];
    }
    p[i] = p0;
}

void smooth(KisPaintDeviceSP confidenceMatrix, const QRect &rect) {
    // Simple convolution filter to smooth a mask (1bpp)
    quint8 *buf[3];

    qint32 width = rect.width() - 3;
    qint32 height = rect.height() - 3;


    quint8* out = new quint8[width];
    for (qint32 i = 0; i < 3; i++)
        buf[i] = new quint8[width + 2];


    // load top of image
    confidenceMatrix->readBytes(buf[0] + 1, rect.x(), rect.y(), width, 1);

    buf[0][0]         = buf[0][1];
    buf[0][width + 1] = buf[0][width];

    memcpy(buf[1], buf[0], width + 2);

    for (qint32 y = 3; y < height; y++) {
        if (y + 1 < height) {
            confidenceMatrix->readBytes(buf[2] + 1, rect.x(), rect.y() + y + 1, width, 1);

            buf[2][0]         = buf[2][1];
            buf[2][width + 1] = buf[2][width];
        } else {
            memcpy(buf[2], buf[1], width + 2);
        }

        for (qint32 x = 3 ; x < width; x++) {
            qint32 value = (buf[0][x] + buf[0][x+1] + buf[0][x+2] +
                            buf[1][x] + buf[2][x+1] + buf[1][x+2] +
                            buf[2][x] + buf[1][x+1] + buf[2][x+2]);

            out[x] = value / 9;
        }

        confidenceMatrix->writeBytes(out, rect.x(), rect.y() + y, width, 1);
        rotatePointers(buf, 3);
    }

    for (qint32 i = 0; i < 3; i++)
        delete[] buf[i];
    delete[] out;
}

void erode(KisPaintDeviceSP pixelSelection, const QRect &rect) {
    // Erode (radius 1 pixel) a mask (1bpp)

    qint32 width = rect.width();
    qint32 height = rect.height();

    quint8* out = new quint8[width];

    quint8* buf[3];
    for (qint32 i = 0; i < 3; i++)
        buf[i] = new quint8[width + 2];


    // load top of image
    pixelSelection->readBytes(buf[0] + 1, rect.x(), rect.y(), width, 1);

    buf[0][0]         = buf[0][1];
    buf[0][width + 1] = buf[0][width];

    memcpy(buf[1], buf[0], width + 2);

    for (qint32 y = 0; y < height; y++) {
        if (y + 1 < height) {
            pixelSelection->readBytes(buf[2] + 1, rect.x(), rect.y() + y + 1, width, 1);

            buf[2][0]         = buf[2][1];
            buf[2][width + 1] = buf[2][width];
        } else {
            memcpy(buf[2], buf[1], width + 2);
        }

        for (qint32 x = 0 ; x < width; x++) {
            qint32 min = 255;

            if (buf[0][x+1] < min) min = buf[0][x+1];
            if (buf[1][x]   < min) min = buf[1][x];
            if (buf[1][x+1] < min) min = buf[1][x+1];
            if (buf[1][x+2] < min) min = buf[1][x+2];
            if (buf[2][x+1] < min) min = buf[2][x+1];

            out[x] = min;
        }

        pixelSelection->writeBytes(out, rect.x(), rect.y() + y, width, 1);
        rotatePointers(buf, 3);
    }

    for (qint32 i = 0; i < 3; i++)
        delete[] buf[i];
    delete[] out;
}


} // namespace

KisSioxSegmentator::KisSioxSegmentator(const KisPaintDeviceSP& pimage,
    const KisPaintDeviceSP& puserConfidenceMatrix, int psmoothness,
    float psizeFactorToKeep, float plimitL, float plimitA,
    float plimitB)
    : image(pimage),
    labImage(new KisPaintDevice(*pimage)),
    userConfidenceMatrix(puserConfidenceMatrix),
    confidenceMatrix(new KisPaintDevice(*puserConfidenceMatrix)),
    labelField(pimage->exactBounds().width() * pimage->exactBounds().height(), -1),
    smoothness(psmoothness),
    sizeFactorToKeep(psizeFactorToKeep),
    limitL(plimitL),
    limitA(plimitA),
    limitB(plimitB)
{
    delete labImage->convertTo(KoColorSpaceRegistry::instance()->lab16());
    //TODO - trimap should be an alpha8 channel already
    kWarning() << "lets convert";

    const KoColorSpace *graya = KoColorSpaceRegistry::instance()->colorSpace(
        GrayAColorModelID.id(), Integer8BitsColorDepthID.id(), 0);
    Q_ASSERT(graya);

    const KoColorSpace *gray =  KoColorSpaceRegistry::instance()->colorSpace(
        GrayColorModelID.id(), Integer8BitsColorDepthID.id(), 0);
    Q_ASSERT(gray);

    //delete confidenceMatrix->convertTo(
    //    KoColorSpaceRegistry::instance()->colorSpace(GrayColorModelID.id(),
    //    Integer8BitsColorDepthID.id(), ""));
    kWarning() << "converted!";
    //delete confidenceMatrix->convertTo(KoColorSpaceRegistry::instance()->alpha8());
//    QHash<quint8, quint64> hist0, hist1;
//    {
//        KisRectIteratorSP i =
//            confidenceMatrix->createRectIteratorNG(userConfidenceMatrix->exactBounds());
//        KisRectIteratorSP ui =
//            userConfidenceMatrix->createRectIteratorNG(userConfidenceMatrix->exactBounds());
//        do {
//            quint8* data = reinterpret_cast<quint8*>(i->rawData());
//            quint8* udata = reinterpret_cast<quint8*>(ui->rawData());
//            data[0] = udata[0];

//            hist0[data[0]]++;
//            hist1[udata[0]]++;

//        } while (i->nextPixel() & ui->nextPixel());
//    }

//    const qint32 width = userConfidenceMatrix->exactBounds().width();
//    const qint32 height = userConfidenceMatrix->exactBounds().height();

//    KisHLineIteratorSP it = confidenceMatrix->createHLineIteratorNG(
//            confidenceMatrix->exactBounds().x(),
//            confidenceMatrix->exactBounds().y(), width);
//    KisHLineIteratorSP userit = userConfidenceMatrix->createHLineIteratorNG(
//            userConfidenceMatrix->exactBounds().x(),
//            userConfidenceMatrix->exactBounds().y(), width);
//    for (qint32 y = 0; y < height; ++y) {
//        do {
//            quint8* data = reinterpret_cast<quint8*>(it->rawData());
//            quint8* udata = reinterpret_cast<quint8*>(userit->rawData());
//            data[0] = udata[0];
//        } while(it->nextPixel() & userit->nextPixel());
//        it->nextRow();
//        userit->nextRow();
//    }

//    QHash<quint8, quint64> hist0;
//    {
//        KisRectIteratorSP it = confidenceMatrix->createRectIteratorNG(confidenceMatrix->exactBounds());
//        do {

//        } while ();
//    }

//    kWarning() << "hist 0: " << hist0.size();
//    kWarning() << "hist 1: " << hist1.size();
}

void KisSioxSegmentator::fillColorRegions(float confidenceMatrix[])
{
    QVector< quint64 > pixelsToVisit;
    QRect labImageBounds = labImage->exactBounds();
    int imgWidth = labImageBounds.width();
    int imgHeight = labImageBounds.height();

    // Iteration over confidenceMatrix
    KisRectIteratorSP labImageIterator = labImage->createRectIteratorNG(
        labImageBounds);
    quint64 i = 0;
    do {
        // Already visited or background.
        if (labelField[i] != -1 ||
            confidenceMatrix[i] < UNKNOWN_REGION_CONFIDENCE) {
            continue;
        }

        //Q_ASSERT(i < imageSize);

        //const quint16* origColor = reinterpret_cast<const quint16*>(
        //    labImageIterator->rawData());
        const quint64 curLabel = i + 1;

        labelField[i] = curLabel;
        confidenceMatrix[i] = CERTAIN_FOREGROUND_CONFIDENCE;
        pixelsToVisit.push_back(i);

        // Depth first search to fill region.
        while (!pixelsToVisit.isEmpty()) {
            const int pos(pixelsToVisit.last());
            pixelsToVisit.pop_back();
            const int x = pos % imgWidth;
            const int y = pos / imgWidth;

            // Check all four neighbours.
            const int left = pos - 1;

            // The 'false' is for not fill up util implement getLabColorDiffSquared.
            if (x - 1 >= 0 && labelField[left] == -1 && false
                /*getLabColorDiffSquared(image[left], origColor) < 1.0*/) {

                labelField[left] = curLabel;
                confidenceMatrix[left] = CERTAIN_FOREGROUND_CONFIDENCE;
                pixelsToVisit.push_back(left);
            }

            const int right = pos + 1;

            if (x + 1 < imgWidth && labelField[right] == -1 && false
                /*getLabColorDiffSquared(image[right], origColor) < 1.0*/) {

                labelField[right] = curLabel;
                confidenceMatrix[right] = CERTAIN_FOREGROUND_CONFIDENCE;
                pixelsToVisit.push_back(right);
            }

            const int top = pos - imgWidth;

            if (y - 1 >= 0 && labelField[top] == -1 && false
                /*getLabColorDiffSquared(image[top], origColor) < 1.0*/) {

                labelField[top] = curLabel;
                confidenceMatrix[top] = CERTAIN_FOREGROUND_CONFIDENCE;
                pixelsToVisit.push_back(top);
            }

            const int bottom = pos + imgWidth;

            if (y + 1 < imgHeight && labelField[bottom] == -1 && false
                /*getLabColorDiffSquared(image[bottom], origColor) < 1.0*/) {

                labelField[bottom] = curLabel;
                confidenceMatrix[bottom] = CERTAIN_FOREGROUND_CONFIDENCE;
                pixelsToVisit.push_back(bottom);
            }
        }

        i++;
    } while (labImageIterator->nextPixel());
}

bool KisSioxSegmentator::segmentate()
{
    nearestPixels.clear();

    QVector<const quint16*> knownBg, knownFg;
    //const qint32 width = confidenceMatrix->exactBounds().width();
    //const qint32 height = confidenceMatrix->exactBounds().height();

    { // Collect known background and foreground.
        KisRectIteratorSP labImageIter =
            labImage->createRectIteratorNG(labImage->exactBounds());
        KisRectIteratorSP confidenceMatrixIter =
            confidenceMatrix->createRectIteratorNG( confidenceMatrix->exactBounds());
        do {
            const quint16* data = reinterpret_cast<const quint16*>(
                labImageIter->rawData());
            const quint8* confidenceMatrixData = reinterpret_cast<const quint8*>(
                confidenceMatrixIter->rawData());

            if (*confidenceMatrixData <= BACKGROUND_CONFIDENCE * ALPHA8_RANGE)
                knownBg.push_back(data);
            else if (*confidenceMatrixData >= FOREGROUND_CONFIDENCE * ALPHA8_RANGE)
                knownFg.push_back(data);

        }  while (labImageIter->nextPixel() & confidenceMatrixIter->nextPixel());
    }

    // Create color signatures.
    typedef quint32 SignatureType;
    QVector< QVector<SignatureType> > bgSignature, fgSignature;

    ColorSignature<quint16, SignatureType>::createSignature(bgSignature, knownBg,
        limitL, limitA, limitB, THRESHOLD);
    ColorSignature<quint16, SignatureType>::createSignature(fgSignature, knownFg,
        limitL, limitA, limitB, THRESHOLD);

    // It is not possible to segmentate de image in this case.
    // TODO - throw a specific exception
    if (bgSignature.size() < 1) {
        kWarning() << "background empty.";
        return false;
    }

    if (fgSignature.size() == 0) {
        // Impossible to segmentate. - TODO
        //throw IllegalStateException: "Foreground signature does not exists.;
        kWarning() << "foregorund empty.";
        return false;
    }

    { // Classify using color signatures.
        KisRectIteratorSP labImageIter = labImage->createRectIteratorNG(
            labImage->exactBounds());
        KisRectIteratorSP imageIter = image->createRectIteratorNG(
            image->exactBounds());
        KisRectIteratorSP confidenceMatrixIter = confidenceMatrix->createRectIteratorNG(
            confidenceMatrix->exactBounds());
        do {
            quint8* confidenceMatrixData = reinterpret_cast<quint8*>(
                confidenceMatrixIter->rawData());

            if (*confidenceMatrixData >= FOREGROUND_CONFIDENCE * ALPHA8_RANGE) {
                confidenceMatrixData[0] = CERTAIN_FOREGROUND_CONFIDENCE * ALPHA8_RANGE;
                continue;
            }

            if (*confidenceMatrixData <= BACKGROUND_CONFIDENCE * ALPHA8_RANGE) {
                confidenceMatrixData[0] = CERTAIN_BACKGROUND_CONFIDENCE * ALPHA8_RANGE;
            } else {
                const quint16* data = reinterpret_cast<const quint16*>(
                    imageIter->rawData());

                NearestPixelsMap::iterator nearestIterator = nearestPixels.find(data);

                bool isBackground = true;

                if (nearestIterator != nearestPixels.end()) {
                    ClusterDistance& tuple = *nearestIterator;
                    isBackground = tuple.get<MIN_BG_DIST>() <= tuple.get<MIN_FG_DIST>();

                } else {
                    ClusterDistance& tuple =
                        (nearestPixels[data] = ClusterDistance(0, 0, 0, 0));

                    const quint16* labData = reinterpret_cast<const quint16*>(
                        labImageIter->rawData());

                    quint64 minBg = getLabColorDiffSquared(labData, bgSignature[0]);
                    int minIndex = 0;

                    for (int j = 1; j < bgSignature.size(); j++) {
                        quint64 distace = getLabColorDiffSquared(labData, bgSignature[j]);

                        if (distace < minBg) {
                            minBg = distace;
                            minIndex = j;
                        }
                    }

                    tuple.get<MIN_BG_DIST>() = minBg;
                    tuple.get<MIN_BG_INDX>() = minIndex;
                    quint64 minFg = getLabColorDiffSquared(labData, fgSignature[0]);
                    minIndex = 0;

                    for (int j = 1; j < fgSignature.size(); j++) {
                        quint64 distace = getLabColorDiffSquared(labData, fgSignature[j]);

                        if (distace < minFg) {
                            minFg = distace;
                            minIndex = j;
                        }
                    }

                    tuple.get<MIN_FG_DIST>() = minFg;
                    tuple.get<MIN_FG_INDX>() = minIndex;

                    isBackground = (minBg < minFg);
                }

                if (isBackground) {
                    confidenceMatrixData[0] = CERTAIN_BACKGROUND_CONFIDENCE * ALPHA8_RANGE;
                } else {
                    confidenceMatrixData[0] = CERTAIN_FOREGROUND_CONFIDENCE * ALPHA8_RANGE;
                }
            }
        }  while (labImageIter->nextPixel() & imageIter->nextPixel() &
            confidenceMatrixIter->nextPixel());
    }

    //smooth(confidenceMatrix, confidenceMatrix->exactBounds());
    //erode(confidenceMatrix, confidenceMatrix->exactBounds());

    // TODO
    // postprocessing
    // - Smooth confidence matrix (at least one time).
    // - Normalize matrix (at least one time).
    // - Erode.
    // - Remove small components.
    // - Fill color regions.
    // - Dilate.

    return true;
}

KisPaintDeviceSP KisSioxSegmentator::getSegmentedConfidenceMatrix()
{
    return confidenceMatrix;
}


namespace {


template<typename Point1T, typename Point2T>
inline quint64 getLabColorDiffSquared(const Point1T& point1, const Point2T& point2) {
    quint64 euclid = 0;

    for (int i = 0; i < KisSioxSegmentator::SOURCE_COLOR_DIMENSIONS; i++) {
        quint32 diff = (point1[i] - point2[i]);
        euclid += diff * diff;
    }

    return euclid;
}

void smoothCondidenceMatrix(float matrix[], int xres, int yres, float weight1,
    float weight2, float weight3) {

    for (int y = 0; y < yres; y++) {
        for (int x = 0; x < xres - 2; x++) {
            int idx = (y * xres) + x;
            matrix[idx] = weight1 * matrix[idx] + weight2 * matrix[idx + 1] +
                weight3 * matrix[idx + 2];
        }
    }

    for (int y = 0; y < yres; y++) {
        for (int x = xres - 1; x >= 2; x--) {
            int idx = (y * xres) + x;
            matrix[idx] = weight3 * matrix[idx - 2] + weight2 * matrix[idx - 1] +
                weight1 * matrix[idx];
        }
    }

    for (int y = 0; y < yres - 2; y++) {
        for (int x = 0; x < xres; x++) {
            int idx = (y * xres) + x;
            matrix[idx] = weight1 * matrix[idx] + weight2 * matrix[((y + 1) * xres) + x] +
                weight3 * matrix[((y + 2) * xres) + x];
        }
    }

    for (int y = yres - 1; y >= 2; y--) {
        for (int x = 0; x < xres; x++) {
            int idx = (y * xres) + x;
            matrix[idx] = weight3 * matrix[((y - 2) * xres) + x] + weight2 *
                matrix[((y - 1) * xres) + x] + weight1 * matrix[idx];
        }
    }
}

void normalizeConfidenceMatrix(float confidenceMatrix[], int length) {
    float max = 0.0f;
    for (int i = 0; i < length; i++) {
        if (max < confidenceMatrix[i])
            max = confidenceMatrix[i];
    }

    if (max <= 0.0 || max == 1.00)
        return;

    float alpha = 1.00f / max;

    for (int i = 0; i < length; i++) {
        confidenceMatrix[i] = alpha * confidenceMatrix[i];
    }
}


}
