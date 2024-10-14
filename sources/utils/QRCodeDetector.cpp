#include <QRCodeDetector.h>

/// @brief Verifies signature box is signed.
/// @author @sherifhg and @OmarQorish
/// @param[in] _image image data.
/// @param[in] _image_size image size data.
/// @return coordinates of 4 cornors of every QR code in image.
vector<vector<pair<float, float>>> QRCodeDetector::retrieveQRCodes(char * _image, size_t _size){

    // Convert binary data to a cv::Mat object (decode image in memory)
    cv::Mat img = cv::imdecode(cv::Mat(1, _size, CV_8UC1, _image), cv::IMREAD_GRAYSCALE);

    // Check if the image was loaded successfully
    if (img.empty()) {
        printf("Error: Could not decode the image from the binary data!\n");
        return {};
    }

    // Initialize a ZBar image scanner
    // Configure the scanner to enable QR code detection
    zbar::ImageScanner scanner;
    scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);

    // ZBar expects the image in grayscale format, identified by the string "Y800".
    // The ZBar image is created using the dimensions and pixel data of the OpenCV image.
    zbar::Image zbarImage(img.cols, img.rows, "Y800", img.data, img.cols * img.rows);

    // performs the actual scanning process, returning the number of QR codes detected
    int n = scanner.scan(zbarImage);

    if (n == 0) {
        printf("No qrcodes found\n");
        return {};
    }

    vector<vector<pair<float, float>>> points;

    // Loop over every QR code detected and append the coordinates of the 4 corners
    for (auto symbol = zbarImage.symbol_begin(); symbol != zbarImage.symbol_end(); ++symbol) {        
        vector<pair<float, float>> currPoints;
        for (int i = 0; i < symbol->get_location_size(); i++) {
            currPoints.emplace_back(symbol->get_location_x(i), symbol->get_location_y(i));
        }

        points.push_back(currPoints);
    }

    return points;
}