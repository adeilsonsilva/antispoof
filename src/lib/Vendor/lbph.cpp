#include "../../../vendor/lbph.hpp"

void Lbph::describe(const std::vector<cv::Mat> & imgs) {
    _histograms.clear();
    update(imgs);
}

std::vector<cv::Mat> & Lbph::getHistograms() {
    return _histograms;
}

template <typename _Tp> void olbp_(cv::InputArray _src, cv::OutputArray _dst) {
    // get matrices
    cv::Mat src = _src.getMat();
    // allocate memory for result
    _dst.create(src.rows-2, src.cols-2, CV_8UC1);
    cv::Mat dst = _dst.getMat();
    // zero the result matrix
    dst.setTo(0);
    // calculate patterns
    for (int i=1;i<src.rows-1;i++) {
        for (int j=1;j<src.cols-1;j++) {
            _Tp center = src.at<_Tp>(i,j);
            unsigned char code = 0;
            code |= (src.at<_Tp>(i-1,j-1) >= center) << 7;
            code |= (src.at<_Tp>(i-1,j) >= center) << 6;
            code |= (src.at<_Tp>(i-1,j+1) >= center) << 5;
            code |= (src.at<_Tp>(i,j+1) >= center) << 4;
            code |= (src.at<_Tp>(i+1,j+1) >= center) << 3;
            code |= (src.at<_Tp>(i+1,j) >= center) << 2;
            code |= (src.at<_Tp>(i+1,j-1) >= center) << 1;
            code |= (src.at<_Tp>(i,j-1) >= center) << 0;
            dst.at<unsigned char>(i-1,j-1) = code;
        }
    }
}

template <typename _Tp> inline void elbp_(cv::InputArray _src, cv::OutputArray _dst, int radius, int neighbors) {
    //get matrices
    cv::Mat src = _src.getMat();
    // allocate memory for result
    _dst.create(src.rows-2*radius, src.cols-2*radius, CV_32SC1);
    cv::Mat dst = _dst.getMat();
    // zero
    dst.setTo(0);
    for(int n=0; n<neighbors; n++) {
        // sample points
        float x = static_cast<float>(radius * cos(2.0*CV_PI*n/static_cast<float>(neighbors)));
        float y = static_cast<float>(-radius * sin(2.0*CV_PI*n/static_cast<float>(neighbors)));
        // relative indices
        int fx = static_cast<int>(floor(x));
        int fy = static_cast<int>(floor(y));
        int cx = static_cast<int>(ceil(x));
        int cy = static_cast<int>(ceil(y));
        // fractional part
        float ty = y - fy;
        float tx = x - fx;
        // set interpolation weights
        float w1 = (1 - tx) * (1 - ty);
        float w2 =      tx  * (1 - ty);
        float w3 = (1 - tx) *      ty;
        float w4 =      tx  *      ty;
        // iterate through your data
        for(int i=radius; i < src.rows-radius;i++) {
            for(int j=radius;j < src.cols-radius;j++) {
                // calculate interpolated value
                float t = static_cast<float>(w1*src.at<_Tp>(i+fy,j+fx) + w2*src.at<_Tp>(i+fy,j+cx) + w3*src.at<_Tp>(i+cy,j+fx) + w4*src.at<_Tp>(i+cy,j+cx));
                // floating point precision, so check some machine-dependent epsilon
                dst.at<int>(i-radius,j-radius) += ((t > src.at<_Tp>(i,j)) || (std::abs(t-src.at<_Tp>(i,j)) < std::numeric_limits<float>::epsilon())) << n;
            }
        }
    }
}

static void elbp(cv::InputArray src, cv::OutputArray dst, int radius, int neighbors)
{
    int type = src.type();
    switch (type) {
    case CV_8SC1:   elbp_<char>(src,dst, radius, neighbors); break;
    case CV_8UC1:   elbp_<unsigned char>(src, dst, radius, neighbors); break;
    case CV_16SC1:  elbp_<short>(src,dst, radius, neighbors); break;
    case CV_16UC1:  elbp_<unsigned short>(src,dst, radius, neighbors); break;
    case CV_32SC1:  elbp_<int>(src,dst, radius, neighbors); break;
    case CV_32FC1:  elbp_<float>(src,dst, radius, neighbors); break;
    case CV_64FC1:  elbp_<double>(src,dst, radius, neighbors); break;
    default:
        cv::String error_msg = cv::format("Using Original Local Binary Patterns for feature extraction only works on single-channel images (given %d). Please pass the image data as a grayscale image!", type);
        CV_Error(cv::Error::StsNotImplemented, error_msg);
        break;
    }
}

cv::Mat histc_(const cv::Mat& src, int minVal=0, int maxVal=255, bool normed=false)
{
    cv::Mat result;
    // Establish the number of bins.
    int histSize = maxVal-minVal+1;
    // Set the ranges.
    float range[] = { static_cast<float>(minVal), static_cast<float>(maxVal+1) };
    const float* histRange = { range };
    // calc histogram
    calcHist(&src, 1, 0, cv::Mat(), result, 1, &histSize, &histRange, true, false);
    // normalize
    if(normed) {
        result /= (int)src.total();
    }
    return result.reshape(1,1);
}

cv::Mat histc(cv::InputArray _src, int minVal, int maxVal, bool normed)
{
    cv::Mat src = _src.getMat();
    switch (src.type()) {
        case CV_8SC1:
            return histc_(cv::Mat_<float>(src), minVal, maxVal, normed);
            break;
        case CV_8UC1:
            return histc_(src, minVal, maxVal, normed);
            break;
        case CV_16SC1:
            return histc_(cv::Mat_<float>(src), minVal, maxVal, normed);
            break;
        case CV_16UC1:
            return histc_(src, minVal, maxVal, normed);
            break;
        case CV_32SC1:
            return histc_(cv::Mat_<float>(src), minVal, maxVal, normed);
            break;
        case CV_32FC1:
            return histc_(src, minVal, maxVal, normed);
            break;
        default:
            CV_Error(cv::Error::StsUnmatchedFormats, "This type is not implemented yet."); break;
    }
    return cv::Mat();
}


cv::Mat spatial_histogram(cv::InputArray _src, int numPatterns, int grid_x, int grid_y, bool /*normed*/)
{
    cv::Mat src = _src.getMat();
    // calculate LBP patch size
    int width = src.cols/grid_x;
    int height = src.rows/grid_y;
    // allocate memory for the spatial histogram
    cv::Mat result = cv::Mat::zeros(grid_x * grid_y, numPatterns, CV_32FC1);
    // return matrix with zeros if no data was given
    if(src.empty())
        return result.reshape(1,1);
    // initial result_row
    int resultRowIdx = 0;
    // iterate through grid
    for(int i = 0; i < grid_y; i++) {
        for(int j = 0; j < grid_x; j++) {
            cv::Mat src_cell = cv::Mat(src, cv::Range(i*height,(i+1)*height), cv::Range(j*width,(j+1)*width));
            cv::Mat cell_hist = histc(src_cell, 0, (numPatterns-1), true);
            // copy to the result matrix
            cv::Mat result_row = result.row(resultRowIdx);
            cell_hist.reshape(1,1).convertTo(result_row, CV_32FC1);
            // increase row count in result matrix
            resultRowIdx++;
        }
    }
    // return result as reshaped feature vector
    return result.reshape(1,1);
}

static cv::Mat elbp(cv::InputArray src, int radius, int neighbors) {
    cv::Mat dst;
    elbp(src, dst, radius, neighbors);
    return dst;
}

void Lbph::update(const std::vector<cv::Mat> & imgs) {
    for (const cv::Mat & img : imgs) {
        cv::Mat lbp_image = elbp(img, _radius, _neighbors);
        cv::Mat p = spatial_histogram(lbp_image, static_cast<int>(std::pow(2.0, static_cast<double>(_neighbors))), _grid_x, _grid_y, true);
        _histograms.push_back(p);
    }
}

double Lbph::compute_similarity(const cv::Mat & img) const {
    if(_histograms.empty()) {
        cv::String error_message = "This Lbph model is not computed yet. Did you call the train method?";
        CV_Error(cv::Error::StsBadArg, error_message);
    }
    cv::Mat lbp_image = elbp(img, _radius, _neighbors);
    cv::Mat query = spatial_histogram(lbp_image, static_cast<int>(std::pow(2.0, static_cast<double>(_neighbors))), _grid_x, _grid_y, true);
    double min_dist = DBL_MAX;
    for (const cv::Mat & hist : _histograms) {
        double dist = compareHist(hist, query, cv::HISTCMP_CHISQR_ALT);
        min_dist = std::min(min_dist, dist);
    }
    return min_dist;
}

cv::Mat Lbph::describe(const cv::Mat & img) {
    cv::Mat lbp_image = elbp(img, _radius, _neighbors);
    cv::Mat p = spatial_histogram(lbp_image, static_cast<int>(std::pow(2.0, static_cast<double>(_neighbors))), _grid_x, _grid_y, true);
    return p;
}

const cv::Mat & Lbph::operator[](int i) const {
    return _histograms[i];
}

void Lbph::getImage(const cv::Mat & orig, cv::Mat & dest) {
    dest = elbp(orig, _radius, _neighbors);
}
