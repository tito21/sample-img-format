/*This source code copyrighted by Lazy Foo' Productions (2004-2022)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <stdio.h>
#include <cstdint>
#include <iostream>
#include <fstream>

#include <opencv4/opencv2/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>

#include "Image.h"
#include "CompImage.h"


const std::string keys =
    "{ help | | Image compressor and display program}"
    "{ @image1 | | Input image}"
    "{ @image2 |<none>| Output image}"
    "{ num-freq | 4 | Number of frequencies to mantling in the compressed image}"
    "{ block-size | 8 | Block size in the compressed image}";

int main( int argc, char* args[] )
{
    cv::CommandLineParser parser(argc, args, keys);

    // parser.about("");
    if (argc == 1 || parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }

    std::string input_path = parser.get<std::string>("@image1");
    std::string output_path = parser.get<std::string>("@image2");

    int num_freq = parser.get<int>("num-freq");
    int block_size = parser.get<int>("block-size");

    Image img1;
    CompImage img_comp;
    cv::Mat out;

    if (input_path.substr(input_path.size() - 3) == "cmp") {
        img_comp.load(input_path);
        out = img_comp.decompress();
    }
    else if (input_path.substr(input_path.size() - 3) == "raw") {
        img1 = Image(input_path);
    }
    else {
        cv::Mat data = cv::imread(input_path);

        const int WIDTH = data.size[0];
        const int HEIGHT = data.size[1];
        img1.size_x = WIDTH;
        img1.size_y = HEIGHT;
        img1.data = data;

        img_comp.set_data(img1, num_freq, block_size);
        img_comp.compress();
    }

    if (output_path != "") {
        if (output_path.substr(output_path.size() - 3) == "cmp") {
            img_comp.save(output_path);
            out = img_comp.decompress();

        }
        if (output_path.substr(output_path.size() - 3) == "raw") {
            img1.save(output_path);
        }
    }

    if (!img1.data.empty()) {
        cv::imshow("Original Image", img1.data);
    }

    if (!out.empty()) {
        cv::imshow("Compressed Image", out);
    }
    if (!img1.data.empty() && !out.empty()) {
        std::cout << "PSNR (dB) " << cv::PSNR(img1.data, out) << std::endl;
    }

    std::cout << "Press any key to exit" << std::endl;
    cv::waitKey(0);

    return 0;

}
