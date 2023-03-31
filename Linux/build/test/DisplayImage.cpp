#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/ml.hpp>
#include <cstdlib>
#include <string>


void Rechtsafgaan() {
    std::cout << "hier gaan we recht" << std::endl;
}

void Linksafgaan() {
    std::cout << "hier gaan we links" << std::endl;
}

void Achteruitgaan() {
    std::cout << "hier gaan we achteruit" << std::endl;
}

void driving(std::string a) {
    if (a == "1") {
        Rechtsafgaan();
        std::cout << "ik ben ook echt rechts gegaan" << std::endl;
    }
    else if (a == "2") {
        Linksafgaan();
        std::cout << "ik ben ook echt links gegaan" << std::endl;
    }
    else if (a == "3") {
        Achteruitgaan();
        std::cout << "ik ben ook echt achteruit gegaan" << std::endl;
    }
    else if (a == "4") {
        std::cout << "ik rij lekker recht door" << std::endl;
    }
    else {
        // open website url
        std::string cmd = "xdg-open " + a;
        system(cmd.c_str());
    }
}

void detectQrCode()
{
    // Open the camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "Failed to open camera!" << std::endl;
        return;
    }

    // Create a QR code detector
    cv::QRCodeDetector qr_code_detector;

    // Continuously capture frames from the camera
    while (true) {
        cv::Mat frame;
        cap.read(frame);

        // Detect and decode QR codes in the frame
        std::vector<cv::Point> corners;
        std::string data = qr_code_detector.detectAndDecode(frame, corners);

        // If a QR code was detected, draw a border around it and display its contents
        if (!data.empty()) {
            // give the decoded QR code string to the function driving
            driving(data);

            // Convert the corner points to a rectangle
            cv::Rect qr_code_rect = cv::boundingRect(corners);

            // Draw a green border around the QR code
            cv::rectangle(frame, qr_code_rect, cv::Scalar(0, 255, 0), 2);

            // Display the contents of the QR code
            cv::putText(frame, data, cv::Point(qr_code_rect.x, qr_code_rect.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
        }

        // Display the frame
        cv::imshow("Camera", frame);

        // Exit if the user presses the ESC key
        if (cv::waitKey(30) == 27) {
            break;
        }
    }
}

int main()
{
    detectQrCode();

    return 0;
}