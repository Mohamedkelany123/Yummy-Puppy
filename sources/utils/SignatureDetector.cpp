#include <SignatureDetector.h>

/// @brief Height of signature box.
float SignatureDetector::SIG_CROP_HEIGHT = 50;

/// @brief Lower threshold value for edge detection.
float SignatureDetector::SIG_CANNY_THR1 = 20;

/// @brief Upper threshold value for edge detection.
float SignatureDetector::SIG_CANNY_THR2 = 120;

/// @brief Signature ink threshold to be considered a signature.
float SignatureDetector::SIG_INK_REGION_RATIO = 0.2;

/// @brief Verifies image contains specified number of signatures passing threshold.
/// @author @sherifhg and @OmarQorish
/// @param[in] _image image data.
/// @param[in] _image_size image size data.
/// @param[in] _num_of_signatures number of signatures that should be in image.
/// @return `true` if the image contains `_num_of_signatures` boxes and each signature box is signed.
bool SignatureDetector::verify(char * _image, size_t _image_size, int _num_of_signatures){

    std::vector<cv::Mat> signatureBoxes = retrieveSignatureBoxes(_image, _image_size);
    if (signatureBoxes.size() != _num_of_signatures) return false;
    
    for (cv::Mat & box : signatureBoxes){
        if (!verifySigned(box)) return false;
    }

    return true;
}

/// @brief loads image from binary data and retrieves QR code coordinates from QRCodeDetector::retrieveQRCodes then calculates 
/// relative position of signature boxes (on the left of the QR code) and crops image of the signature box accordingly.
/// @author @OmarQorish and @sherifhg
/// @param[in] _image image data.
/// @param[in] _image_size image size data.
/// @return Vector<cv::Mat> cropped images of signature boxes.
vector<cv::Mat> SignatureDetector::retrieveSignatureBoxes(char* _image_data, size_t _image_size) {
    vector<vector<pair<float, float>>> qr_boxes;
    vector<vector<pair<float, float>>> signature_boxes;
    vector<cv::Mat> signatureImages;

    // Load the image from binary data using OpenCV
    std::vector<unsigned char> buffer(_image_data, _image_data + _image_size);
    cv::Mat image = cv::imdecode(buffer, cv::IMREAD_COLOR);
    if (image.empty()) {
        cerr << "Error: Unable to load image." << endl;
        return signatureImages;
    }

    // Retrieve QR code boxes
    qr_boxes = QRCodeDetector::retrieveQRCodes(_image_data, _image_size);
    
    // Calculate the signature boxes based on QR code positions
    for (auto box : qr_boxes) {
        if (box.size() != 4) {
            continue;  // Skip if the box does not have 4 points
        }
        
        vector<pair<float, float>> signature_box;
        signature_box.emplace_back(0, box[0].second);
        signature_box.emplace_back(0, box[1].second);
        signature_box.emplace_back(box[1].first, box[1].second);
        signature_box.emplace_back(box[0].first, box[0].second);
        signature_boxes.push_back(signature_box);
    }

    // Crop the signature boxes from the image
    for (const auto& signature_box : signature_boxes) {
        // for(auto point:signature_box){
        //     cout<<"x: "<<point.first<<"\ny:"<<point.second<<endl<<endl;
        // }
        // Convert the signature box coordinates to a Rect for cropping
        int x = static_cast<int>(signature_box[0].first);
        int y = static_cast<int>(signature_box[0].second);
        int width = static_cast<int>(signature_box[2].first - signature_box[0].first);
        int height = static_cast<int>(signature_box[1].second - signature_box[0].second);

        // Ensure the dimensions are within the image bounds
        if (x >= 0 && y >= 0 && (x + width) <= image.cols && (y + height) <= image.rows) {
            cv::Rect roi(x, y, width, height);
            cv::Mat croppedSignature = image(roi);
            signatureImages.push_back(croppedSignature);
        }
    }

    return signatureImages;
}

/// @brief Verifies signature box is signed.
/// @author @sherifhg and @OmarQorish
/// @param[in] _image signature box data.
/// @return `true` if the signature box contains signature passing certain threshold.
bool SignatureDetector::verifySigned(cv::Mat _image){

    // Resize image with a specific height while reserving aspect ratio of image
    cv::Mat resized_image;
    double aspect_ratio = (double)SignatureDetector::SIG_CROP_HEIGHT / _image.rows;
    int new_width = (int)(_image.cols * aspect_ratio);
    cv::resize(_image, resized_image, cv::Size(new_width, SignatureDetector::SIG_CROP_HEIGHT));

    int num_pixels = resized_image.cols * resized_image.rows;

    // Detect edges (non white pixels)
    // The Canny algorithm detects edges based on the intensity gradients of the image.
    // 'SignatureDetector::SIG_CANNY_THR1' and 'SignatureDetector::SIG_CANNY_THR2'
    // represent the lower and upper threshold values for edge detection.
    cv::Mat edged;
    cv::Canny(resized_image, edged, SignatureDetector::SIG_CANNY_THR1, SignatureDetector::SIG_CANNY_THR2);

    // Apply a morphological 'closing' operation on the 'edged' image.
    // The 'closing' operation is a dilation followed by an erosion, which helps 
    // to close small gaps or holes in the edges.
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 3));
    cv::morphologyEx(edged, edged, cv::MORPH_CLOSE, kernel);


    double sig_pixels = cv::sum(edged)[0] / 255.0;
    double ratio = sig_pixels / num_pixels;

    return ratio > SignatureDetector::SIG_INK_REGION_RATIO;
}