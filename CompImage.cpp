#include "CompImage.h"

CompImage::CompImage(Image img, int _num_freq, int _block_size)
{
    size_x = img.size_x;
    size_y = img.size_y;

    num_freq = _num_freq;
    block_size = _block_size;

    compressed = false;

    cv::cvtColor(img.data, data, cv::COLOR_BGR2YCrCb);

}

CompImage::CompImage(const std::string path) {
    compressed = true;
    load(path);
}

CompImage::CompImage()
{
}


CompImage::~CompImage()
{
}

void CompImage::set_data(Image img, int _num_freq, int _block_size)
{
    size_x = img.size_x;
    size_y = img.size_y;

    num_freq = _num_freq;
    block_size = _block_size;

    compressed = false;

    cv::cvtColor(img.data, data, cv::COLOR_BGR2YCrCb);

}

void CompImage::compress()
{

    std::vector<cv::Mat> dct_channels(3);
    std::vector<cv::Mat> img_channels(3);

    data.convertTo(data, CV_64FC3);
    // data = data.mul(1/255);
    data = data * 1.0/255.0 - 0.5;
    // double min, max;
    // cv::minMaxLoc(data, &min, &max, NULL, NULL);
    // std::cout << "org data range " << min << " " << max << " " << data.type() << std::endl;
    // cv::imshow("org data", data);
    // cv::waitKey(0);

    cv::split(data, img_channels);

    dct_channels[0] = cv::Mat::zeros(size_x, size_y, CV_64FC1);
    dct_channels[1] = cv::Mat::zeros(size_x, size_y, CV_64FC1);
    dct_channels[2] = cv::Mat::zeros(size_x, size_y, CV_64FC1);


    cv::Mat mask = cv::Mat::zeros(block_size, block_size, CV_64FC1);
    cv::Range cols(0, num_freq);
    cv::Range rows(0, num_freq);
    mask(rows, cols) = 1;
    // std::cout << mask << std::endl;
    for (int i=0; i<size_y; i+=block_size) {
        for (int j=0; j<size_x; j+=block_size) {
            cv::Range cols(i, i+block_size);
            cv::Range rows(j, j+block_size);

            cv::dct(img_channels[0](rows, cols), dct_channels[0](rows, cols));
            cv::dct(img_channels[1](rows, cols), dct_channels[1](rows, cols));
            cv::dct(img_channels[2](rows, cols), dct_channels[2](rows, cols));
            dct_channels[0](rows, cols) = dct_channels[0](rows, cols).mul(mask);
            dct_channels[1](rows, cols) = dct_channels[1](rows, cols).mul(mask);
            dct_channels[2](rows, cols) = dct_channels[2](rows, cols).mul(mask);
        }
    }

    compressed = true;

    cv::merge(dct_channels, data_dct);

    cv::minMaxLoc(data_dct, &dct_min, &dct_max, NULL, NULL);
    // dct_min += 0.5;
    // dct_max -= 0.5;
    // std::cout << "dct org range "<< dct_min << " " << dct_max << " " << data_dct.type() << std::endl;
    data_dct = 255.0*((data_dct - dct_min) / (dct_max - dct_min));
    data_dct.convertTo(data_dct, CV_8UC3);

    // cv::minMaxLoc(data_dct, &min, &max, NULL, NULL);
    // std::cout << "dct comp range " << min << " " << max << " " << data_dct.type() << std::endl;
    dct_zero_0 = *data_dct.ptr<u_char>(num_freq+1, num_freq+1);
    dct_zero_1 = *(data_dct.ptr<u_char>(num_freq+1, num_freq+1)+1);
    dct_zero_2 = *(data_dct.ptr<u_char>(num_freq+1, num_freq+1)+2);

    // cv::imshow("DCT", data_dct);
    // cv::waitKey(0);
}

cv::Mat CompImage::decompress()
{

    // cv::imshow("DCT saved", data_dct);
    // cv::waitKey(0);

    std::vector<cv::Mat> dct_channels(3);
    std::vector<cv::Mat> img_channels(3);

    data_dct.convertTo(data_dct, CV_64FC3);
    data_dct = (1.0/255.0)*data_dct*(dct_max - dct_min) + dct_min;
    // double min, max;
    // cv::minMaxLoc(data_dct, &min, &max, NULL, NULL);
    // std::cout << "dct reconstructed range " << min << " " << max << " " << data_dct.type() << std::endl;

    cv::split(data_dct, dct_channels);

    img_channels[0] = cv::Mat::zeros(size_x, size_y, CV_64FC1);
    img_channels[1] = cv::Mat::zeros(size_x, size_y, CV_64FC1);
    img_channels[2] = cv::Mat::zeros(size_x, size_y, CV_64FC1);

    for (int i=0; i<size_y; i+=block_size) {
        for (int j=0; j<size_x; j+=block_size) {
            cv::Range cols(i, i+block_size);
            cv::Range rows(j, j+block_size);

            cv::idct(dct_channels[0](rows, cols), img_channels[0](rows, cols));
            cv::idct(dct_channels[1](rows, cols), img_channels[1](rows, cols));
            cv::idct(dct_channels[2](rows, cols), img_channels[2](rows, cols));

            // cv::min(cv::max(img_channels[0], -0.5), 0.5, img_channels[0]);
            // cv::min(cv::max(img_channels[1], -0.5), 0.5, img_channels[1]);
            // cv::min(cv::max(img_channels[2], -0.5), 0.5, img_channels[2]);

        }
    }

    // cv::imshow("Y", img_channels[0]);
    // cv::imshow("Cr", img_channels[1]);
    // cv::imshow("Cb", img_channels[2]);
    // cv::waitKey(0);
    cv::merge(img_channels, data);
    cv::Mat out;

    // cv::minMaxLoc(data, &min, &max, NULL, NULL);
    // std::cout << "data out range " << min << " " << max << std::endl;

    data = (data + 0.5) * 255.0;
    // cv::minMaxLoc(data, &min, &max, NULL, NULL);
    // std::cout << "data reconstructed range " << min << " " << max << std::endl;
    data.convertTo(out, CV_8UC3);
    cv::cvtColor(out, out, cv::COLOR_YCrCb2BGR);
    return out;
}

void CompImage::save(const std::string path)
{
    if (!compressed) {
        compress();

    }

    std::ofstream file(path, std::ios::out | std::ios::binary);

    file.write((char*) &size_x, sizeof(size_x));
    file.write((char*) &size_y, sizeof(size_y));

    file.write((char*) &block_size, sizeof(block_size));
    file.write((char*) &num_freq, sizeof(num_freq));

    file.write((char*) &dct_min, sizeof(dct_min));
    file.write((char*) &dct_max, sizeof(dct_max));
    file.write((char*) &dct_zero_0, sizeof(dct_zero_0));
    file.write((char*) &dct_zero_1, sizeof(dct_zero_1));
    file.write((char*) &dct_zero_2, sizeof(dct_zero_2));

    for (int i=0; i<size_y; i++) {
        const uchar* ptr = data_dct.ptr<uchar>(i);
        for (int j=0; j<3*size_x; j+=3) {
            if ((i % block_size) <= num_freq && (j/3 % block_size) <= num_freq) {
                file.write((char *)ptr+j, sizeof(uchar));
                file.write((char *)ptr+j+1, sizeof(uchar));
                file.write((char *)ptr+j+2, sizeof(uchar));
            }

        }
    }
}

void CompImage::load(const std::string path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);

    file.read((char*) &size_x, sizeof(size_x));
    file.read((char*) &size_y, sizeof(size_y));

    file.read((char*) &block_size, sizeof(block_size));
    file.read((char*) &num_freq, sizeof(num_freq));

    file.read((char*) &dct_min, sizeof(dct_min));
    file.read((char*) &dct_max, sizeof(dct_max));
    file.read((char*) &dct_zero_0, sizeof(dct_zero_0));
    file.read((char*) &dct_zero_1, sizeof(dct_zero_1));
    file.read((char*) &dct_zero_2, sizeof(dct_zero_2));

    std::vector<cv::Mat> dct_zeros = {
        cv::Mat::zeros(size_x, size_y, CV_8UC1) + dct_zero_0,
        cv::Mat::zeros(size_x, size_y, CV_8UC1) + dct_zero_1,
        cv::Mat::zeros(size_x, size_y, CV_8UC1) + dct_zero_2,
    };

    cv::merge(dct_zeros, data_dct);

    for (int i=0; i<size_y; i++) {
        const uchar* ptr = data_dct.ptr<uchar>(i);
        for (int j=0; j<3*size_x; j+=3) {
            if ((i % block_size) <= num_freq && (j/3 % block_size) <= num_freq) {
                file.read((char *)ptr+j, sizeof(uchar));
                file.read((char *)ptr+j+1, sizeof(uchar));
                file.read((char *)ptr+j+2, sizeof(uchar));
             }
        }
    }
    compressed = true;

}
