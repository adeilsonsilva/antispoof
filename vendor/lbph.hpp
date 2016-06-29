#ifndef LBPH_HPP
#define LBPH_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <vector>
#include <map>
#include <iostream>
#include <set>
#include <limits>
#include "descriptor.hpp"

class Lbph : public Descriptor {
private:
    int _grid_x;
    int _grid_y;
    int _radius;
    int _neighbors;
    double _threshold;
    std::vector<cv::Mat> _histograms;

public:
    Lbph(int radius_=1, int neighbors_=8, int gridx=8, int gridy=8, double threshold = DBL_MAX) :
    _grid_x(gridx), _grid_y(gridy), _radius(radius_), _neighbors(neighbors_), _threshold(threshold) {}
    virtual ~Lbph() {}
    Lbph(Lbph &&) = delete;
    Lbph(const Lbph &) = default;
    Lbph& operator=(const Lbph &) = delete;

    void describe(const std::vector<cv::Mat> &);
    double compute_similarity(const cv::Mat &) const;
    void update(const std::vector<cv::Mat> &);
    cv::Mat describe(const cv::Mat &);
    const cv::Mat & operator[](int) const;


    std::vector<cv::Mat> & getHistograms();
    void getImage(const cv::Mat &, cv::Mat &);
};

#endif /* end of include guard: LBPH_HPP */
