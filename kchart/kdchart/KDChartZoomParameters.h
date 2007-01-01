#ifndef ZOOMPARAMETERS_H
#define ZOOMPARAMETERS_H

namespace KDChart {
    class ZoomParameters {
    public:
        ZoomParameters()
        : xFactor( 1.0 ),
          yFactor( 1.0 ),
          xCenter( 0.5 ),
          yCenter( 0.5)
        {
        }
    
        double xFactor;
        double yFactor;

        double xCenter;
        double yCenter;
    };
}

#endif
