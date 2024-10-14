#ifndef QRCODEDETECTOR_H
#define QRCODEDETECTOR_H

#include <common.h>
#include <zbar.h>
#include <opencv2/opencv.hpp>

class QRCodeDetector {
    public:
        static vector<vector<pair<float, float>>> retrieveQRCodes(char * _image, size_t _size);
};

#endif