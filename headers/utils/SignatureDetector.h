#ifndef SIGNATUREDETECTOR_H
#define SIGNATUREDETECTOR_H

#include <common.h>
#include <QRCodeDetector.h>
#include <opencv2/opencv.hpp>

class SignatureDetector {
    public:
        static bool verify(char * _image, size_t _image_size, int _num_of_signatures);
    private:
        static float SIG_CROP_HEIGHT;
        static float SIG_CANNY_THR1;
        static float SIG_CANNY_THR2;
        static float SIG_INK_REGION_RATIO;


        static vector<cv::Mat> retrieveSignatureBoxes(char* _image_data, size_t _image_size);
        static bool verifySigned(cv::Mat _image);
};

#endif