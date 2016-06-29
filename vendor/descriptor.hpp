#ifndef DESCRIPTOR_HPP
#define DESCRIPTOR_HPP

#include <opencv2/opencv.hpp>
#include <opencv/cv.h>

class Descriptor {
public:
    virtual void describe(const std::vector<cv::Mat> &) = 0;
    virtual void update(const std::vector<cv::Mat> &) = 0;
    virtual double compute_similarity(const cv::Mat &) const = 0;
    virtual cv::Mat describe(const cv::Mat &) = 0;
};

#endif /* end of include guard: DESCRIPTOR_HPP */
