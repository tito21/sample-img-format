#include "Image.h"

Image::Image()
{
}

Image::Image(const std::string path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);

    file.read((char*) &size_x, sizeof(size_x));
    file.read((char*) &size_y, sizeof(size_y));

    // std::cout << size_x << " " << size_y << std::endl;

    data.create(size_x, size_y, CV_8UC3);

    for (int i=0; i<size_y; i++) {
        const uchar* ptr = data.ptr<uchar>(i);
        for (int j=0; j<3*size_x; j+=3) {
            file.read((char *)ptr+j, sizeof(uchar));
            file.read((char *)ptr+j+1, sizeof(uchar));
            file.read((char *)ptr+j+2, sizeof(uchar));

        }
    }

}

Image::~Image()
{
}

void Image::save(const std::string path) {

    std::ofstream file(path, std::ios::out | std::ios::binary);

    file.write((char*) &size_x, sizeof(size_x));
    file.write((char*) &size_y, sizeof(size_y));

    for (int i=0; i<size_y; i++) {
        const uchar* ptr = data.ptr<uchar>(i);
        for (int j=0; j<3*size_x; j+=3) {
            file.write((char *)ptr+j, sizeof(uchar));
            file.write((char *)ptr+j+1, sizeof(uchar));
            file.write((char *)ptr+j+2, sizeof(uchar));

        }
    }

    file.close();

}