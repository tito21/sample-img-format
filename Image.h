#pragma once

#include <string>
#include <cstdint>
#include <fstream>
#include <iostream>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>

class Image
{
private:

public:
    int size_x, size_y;
    cv::Mat data;
    Image();
    Image(const std::string path);
    ~Image();

    void save(const std::string path);
};

