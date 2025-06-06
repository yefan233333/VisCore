#include <iostream>
#include <opencv2/opencv.hpp>
#include "vis_core/logging/logging.h"

int main()
{
    cv::VideoCapture vid;
    vid.open(0);
    if (!vid.isOpened())
    {
        std::cerr << "Error: Could not open video capture." << std::endl;
        return -1;
    }
    cv::Mat frame;

    size_t count = 0;

    while (true)
    {
        vid >> frame;
        count++;
        std::cout << "count = " << count << std::endl;
        if(count > 100)
        {
            VISCORE_HIGHLIGHT_INFO("Captured 1000 frames, exiting.");
            VISCORE_WARNING_INFO("This is a warning message after capturing 1000 frames.");
            VISCORE_PASS_INFO("Successfully captured 1000 frames.");
            VISCORE_ERROR_INFO("This is an error message after capturing 1000 frames.");
            VISCORE_NORMAL_INFO("This is a normal info message after capturing 1000 frames.");

            VISCORE_THROW_ERROR("Throwing an error after capturing 1000 frames.");
        }
        if (frame.empty())
        {
            std::cerr << "Error: Could not read frame." << std::endl;
            break;
        }
        cv::imshow("Video Feed", frame);
        if (cv::waitKey(30) >= 0) // Wait for 30 ms or until a key is pressed
            break;
    }

}