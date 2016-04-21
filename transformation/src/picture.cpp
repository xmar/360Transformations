#include "picture.hpp"


using namespace IMT;

double Picture::m_mssimWeight[m_nlevs] = {0.0448, 0.2856, 0.3001, 0.2363, 0.1333};

Pixel Picture::GetInterPixel(CoordF pt) const
{
    const cv::Mat& img = m_pictMat;
    assert(!img.empty());
    assert(img.channels() == 3);

    int x = (int)pt.x;
    int y = (int)pt.y;

    int x0 = cv::borderInterpolate(x,   img.cols, cv::BORDER_REFLECT_101);
    int x1 = cv::borderInterpolate(x+1, img.cols, cv::BORDER_REFLECT_101);
    int y0 = cv::borderInterpolate(y,   img.rows, cv::BORDER_REFLECT_101);
    int y1 = cv::borderInterpolate(y+1, img.rows, cv::BORDER_REFLECT_101);

    float a = pt.x - (float)x;
    float c = pt.y - (float)y;

    uchar b = (uchar)cvRound((img.at<Pixel>(y0, x0)[0] * (1.f - a) + img.at<Pixel>(y0, x1)[0] * a) * (1.f - c)
                           + (img.at<Pixel>(y1, x0)[0] * (1.f - a) + img.at<Pixel>(y1, x1)[0] * a) * c);
    uchar g = (uchar)cvRound((img.at<Pixel>(y0, x0)[1] * (1.f - a) + img.at<Pixel>(y0, x1)[1] * a) * (1.f - c)
                           + (img.at<Pixel>(y1, x0)[1] * (1.f - a) + img.at<Pixel>(y1, x1)[1] * a) * c);
    uchar r = (uchar)cvRound((img.at<Pixel>(y0, x0)[2] * (1.f - a) + img.at<Pixel>(y0, x1)[2] * a) * (1.f - c)
                           + (img.at<Pixel>(y1, x0)[2] * (1.f - a) + img.at<Pixel>(y1, x1)[2] * a) * c);

    return Pixel(b, g, r);
}

void Picture::ImgShowWithLimit(std::string txt, cv::Size s) const
{
    unsigned int width = s.width;
    unsigned int height = s.height;
    const unsigned int pictWidth = m_pictMat.cols;
    const unsigned int pictheight = m_pictMat.rows;
    if (height < pictheight && width < pictWidth)
    {
        if (pictheight > pictWidth)
        {
            width = float(height) * pictWidth / pictheight;
        }
        else
        {
            height = float(width) * pictheight / pictWidth;
        }
    }
    else if (width < pictWidth)
    {
        height = float(width) * pictheight / pictWidth;
    }
    else if (height < pictheight)
    {
        width = float(height) * pictWidth / pictheight;
    }
    else
    {
        height = pictheight;
        width = pictWidth;
        ImgShow(txt);
        return;
    }
    ImgShowResize(txt, cv::Size(width,height));
}

double Picture::GetMSE(const Picture& pic) const
{
    if (pic.GetHeight()!= GetHeight() && pic.GetWidth() != GetWidth())
    {
        throw std::invalid_argument("MSE computation require pictures to have the same width and height");
    }
    cv::Mat tmp(cv::Mat::zeros(GetHeight(), GetWidth(), m_pictMat.type()));
    cv::subtract(m_pictMat, pic.m_pictMat, tmp);
    cv::multiply(tmp, tmp, tmp);
    return cv::mean(tmp).val[0];
}

double Picture::GetPSNR(const Picture& pic) const
{
    auto mse = GetMSE(pic);
    return mse != 0 ? 10.0*std::log10(255*255/mse) : 100.0;
}

// Smoothing using a Gaussian kernel of size ksize with standard deviation sigma
// Returns only those parts of the correlation that are computed without zero-padded edges
// (similarly to 'filter2' in Matlab with option 'valid')
void Picture::ApplyGaussianBlur(const cv::Mat& src, cv::Mat& dst, int ksize, double sigma) const
{
    int invalid = (ksize-1)/2;
    cv::Mat tmp(src.rows, src.cols, m_pictMat.type());
    cv::GaussianBlur(src, tmp, cv::Size(ksize,ksize), sigma);
    tmp(cv::Range(invalid, tmp.rows-invalid), cv::Range(invalid, tmp.cols-invalid)).copyTo(dst);
}


std::tuple<double, double> Picture::ComputeSSIM(const cv::Mat& img1, const cv::Mat& img2) const
{

    if (img1.rows!= img2.rows && img1.cols !=  img2.cols)
    {
        throw std::invalid_argument("SSIM computation require pictures to have the same width and height");
    }

    int ht = img1.rows;
    int wt = img1.cols;
    int w = wt - 10;
    int h = ht - 10;

    cv::Mat mu1(h,w,m_pictMat.type()), mu2(h,w,m_pictMat.type());
    cv::Mat mu1_sq(h,w,m_pictMat.type()), mu2_sq(h,w,m_pictMat.type()), mu1_mu2(h,w,m_pictMat.type());
    cv::Mat img1_sq(ht,wt,m_pictMat.type()), img2_sq(ht,wt,m_pictMat.type()), img1_img2(ht,wt,m_pictMat.type());
    cv::Mat sigma1_sq(h,w,m_pictMat.type()), sigma2_sq(h,w,m_pictMat.type()), sigma12(h,w,m_pictMat.type());
    cv::Mat tmp1(h,w,m_pictMat.type()), tmp2(h,w,m_pictMat.type()), tmp3(h,w,m_pictMat.type());
    cv::Mat ssim_map(h,w,m_pictMat.type()), cs_map(h,w,m_pictMat.type());

    // mu1 = filter2(window, img1, 'valid');
    ApplyGaussianBlur(img1, mu1, 11, 1.5);

    // mu2 = filter2(window, img2, 'valid');
    ApplyGaussianBlur(img2, mu2, 11, 1.5);

    // mu1_sq = mu1.*mu1;
    cv::multiply(mu1, mu1, mu1_sq);
    // mu2_sq = mu2.*mu2;
    cv::multiply(mu2, mu2, mu2_sq);
    // mu1_mu2 = mu1.*mu2;
    cv::multiply(mu1, mu2, mu1_mu2);

    cv::multiply(img1, img1, img1_sq);
    cv::multiply(img2, img2, img2_sq);
    cv::multiply(img1, img2, img1_img2);

    // sigma1_sq = filter2(window, img1.*img1, 'valid') - mu1_sq;
    ApplyGaussianBlur(img1_sq, sigma1_sq, 11, 1.5);
    sigma1_sq -= mu1_sq;

    // sigma2_sq = filter2(window, img2.*img2, 'valid') - mu2_sq;
    ApplyGaussianBlur(img2_sq, sigma2_sq, 11, 1.5);
    sigma2_sq -= mu2_sq;

    // sigma12 = filter2(window, img1.*img2, 'valid') - mu1_mu2;
    ApplyGaussianBlur(img1_img2, sigma12, 11, 1.5);
    sigma12 -= mu1_mu2;

    // cs_map = (2*sigma12 + C2)./(sigma1_sq + sigma2_sq + C2);
    tmp1 = 2*sigma12 + m_ssim_c2;
    tmp2 = sigma1_sq + sigma2_sq + m_ssim_c2;
    cv::divide(tmp1, tmp2, cs_map);
    // ssim_map = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))./((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2));
    tmp3 = 2*mu1_mu2 + m_ssim_c1;
    cv::multiply(tmp1, tmp3, tmp1);
    tmp3 = mu1_sq + mu2_sq + m_ssim_c1;
    cv::multiply(tmp2, tmp3, tmp2);
    cv::divide(tmp1, tmp2, ssim_map);

    // mssim = mean2(ssim_map);
    double mssim = cv::mean(ssim_map).val[0];
    // mcs = mean2(cs_map);
    double mcs = cv::mean(cs_map).val[0];

    return std::make_tuple(mssim, mcs);
}

double Picture::GetSSIM(const Picture& pic) const
{
    return std::get<0>(ComputeSSIM(m_pictMat, pic.m_pictMat));
}

double Picture::GetMSSSIM(const Picture& pic) const
{
    if (pic.GetHeight()!= GetHeight() && pic.GetWidth() != GetWidth())
    {
        throw std::invalid_argument("MS-SSIM computation require pictures to have the same width and height");
    }
    int h = GetHeight() + (GetHeight() % 16 != 0 ? 16-(GetHeight() % 16):0);
    int w = GetHeight() + (GetWidth() % 16 != 0 ? 16-(GetWidth() % 16):0);


    double mssim[m_nlevs];
    double mcs[m_nlevs];

    cv::Mat im1[m_nlevs];
    cv::Mat im2[m_nlevs];

    cv::resize(m_pictMat, im2[0], cv::Size(w, h), 0, 0, cv::INTER_LINEAR);
    cv::resize(pic.m_pictMat, im1[0], cv::Size(w, h), 0, 0, cv::INTER_LINEAR);

    for (int l=0; l<m_nlevs; l++) {
        // [mssim_array(l) ssim_map_array{l} mcs_array(l) cs_map_array{l}] = ssim_index_new(im1, im2, K, window);
        auto res = ComputeSSIM(im1[l], im2[l]);
        mssim[l] = std::get<0>(res);
        mcs[l] = std::get<1>(res);

        if (l < m_nlevs-1) {
            w /= 2;
            h /= 2;
            im1[l+1] = cv::Mat(h,w,m_pictMat.type());
            im2[l+1] = cv::Mat(h,w,m_pictMat.type());

            // filtered_im1 = filter2(downsample_filter, im1, 'valid');
            // im1 = filtered_im1(1:2:M-1, 1:2:N-1);
            cv::resize(im1[l], im1[l+1], cv::Size(w,h), 0, 0, cv::INTER_LINEAR);
            // filtered_im2 = filter2(downsample_filter, im2, 'valid');
            // im2 = filtered_im2(1:2:M-1, 1:2:N-1);
            cv::resize(im2[l], im2[l+1], cv::Size(w,h), 0, 0, cv::INTER_LINEAR);
        }
    }

    // overall_mssim = prod(mcs_array(1:level-1).^weight(1:level-1))*mssim_array(level);
    double msssim = mssim[m_nlevs-1];
    for (int l=0; l<m_nlevs-1; l++)
    {
        msssim *= pow(mcs[l], m_mssimWeight[l]);
    }
    return msssim;
}
