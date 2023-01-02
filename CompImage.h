#include <cstdint>
#include <math.h>
#include <vector>
#include <fstream>


#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <opencv4/opencv2/core/types.hpp>
#include <opencv4/opencv2/highgui.hpp>

#include "Image.h"

class CompImage
{
private:
    bool compressed;
public:
    int block_size;
    int num_freq;
    int size_x, size_y;
    cv::Mat data;
    cv::Mat data_dct;
    double dct_min, dct_max;
    u_char dct_zero_0, dct_zero_1, dct_zero_2;

    CompImage();
    CompImage(Image img, int _num_freq = 4, int _block_size = 8);
    CompImage(const std::string path);

    ~CompImage();

    void compress();
    cv::Mat decompress();
    void set_data(Image img, int _num_freq, int _block_size);

    void save(const std::string path);
    void load(const std::string path);

};