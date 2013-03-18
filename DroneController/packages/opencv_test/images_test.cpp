/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include <iostream>
#include <iomanip>
#include <sstream>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "ace/High_Res_Timer.h"
#include "ace/Sched_Params.h"

#define ITERATIONS 10

#define scale_size(...) (sizeof(__VA_ARGS__) / sizeof(Scale))

class Scale
{
public:
    double x_scale;
    double y_scale;
    Scale (double x, double y)
    {
        x_scale = x;
        y_scale = y;
    }
};

ACE_hrtime_t execute_opencv_resize (cv::Mat opencv_src_img, int iterations, Scale resize_scale, int interpolation)
{
    
    int opencv_src_width = opencv_src_img.size().width;
    int opencv_src_height = opencv_src_img.size().height;

	ACE_hrtime_t overall_time;
    ACE_High_Res_Timer overall_timer;

    int opencv_dest_width = opencv_src_width * resize_scale.x_scale;
    int opencv_dest_height = opencv_src_height * resize_scale.y_scale;
    cv::Mat opencv_dest_img = cv::Mat(opencv_dest_height, opencv_dest_width, opencv_src_img.type());
    overall_timer.start();
    for (int x = 0; x < iterations; x++)
    {
        
        cv::resize(opencv_src_img, opencv_dest_img, opencv_dest_img.size(), 0.0, 0.0, interpolation);
    }
    overall_timer.stop();
    overall_timer.elapsed_time (overall_time);

    return overall_time;
}

ACE_hrtime_t execute_opencv_crop (cv::Mat opencv_src_img, int iterations)
{
    
    int opencv_src_width = opencv_src_img.size().width;
    int opencv_src_height = opencv_src_img.size().height;

    int x1 = opencv_src_width >> 2;
	int y1 = opencv_src_height >> 2;
	int x2 = opencv_src_width >> 1;
	int y2 = opencv_src_height >> 1;

	ACE_hrtime_t overall_time;
    ACE_High_Res_Timer overall_timer;

    cv::Rect rect(x1, y1, x2, y2);
    //cv::Mat opencv_dest_img = cv::Mat(opencv_dest_height, opencv_dest_width, opencv_src_img.type());
    overall_timer.start();
    for (int x = 0; x < iterations; x++)
    {
        cv::Mat dest = opencv_src_img(rect);
    }
    overall_timer.stop();
    overall_timer.elapsed_time (overall_time);

    return overall_time;
}

std::string to_mili_seconds(uint64_t t)
{
    std::stringstream buffer;
    double ms = (double) t / 1000000;
    
    buffer << std::setprecision (4) << std::fixed << ms << "ms";
    
    return buffer.str();
}

std::string to_percent(double d)
{
    std::stringstream buffer;
    buffer << std::setprecision (1) << std::fixed << (d * 100);
    return buffer.str();
}

int main (int argc, char ** argv)
{

    if (argc < 2)
    {
        std::cout << "Must specify a file to load" << std::endl;
        return 1;
    }
    
    Scale resize_scales[] = {Scale(0.6, 0.6), Scale(0.3, 0.3), Scale(0.3, 0.6), Scale(0.6, 0.3)};
    int resize_scales_len = scale_size(resize_scales);
    
    //SET PRIORITY HERE
    

    std::string fname = argv[1];
    
    cv::Mat opencv_src_mat;
    opencv_src_mat = cv::imread(fname);
    
    ACE_hrtime_t cur_time;


    std::cout << "\n" << std::left << std::setw(21) << "Name" << "|" << std::right << std::setw(10) << "Average" << "\n";
    
    std::cout << std::left << std::setw(21) << "OpenCV Crop" << " " << std::flush;
    cur_time = execute_opencv_crop(opencv_src_mat, ITERATIONS);
    std::cout << std::right << std::setw(10) << to_mili_seconds(cur_time / ITERATIONS) << "\n";



    for (int x = 0; x < resize_scales_len; x++)
    {
        Scale cur_scale = resize_scales[x];
        std::cout << "\n\nTesting Resize: (" << to_percent(cur_scale.x_scale) << "%, " << to_percent(cur_scale.y_scale) << "%)\n";
        std::cout << std::left << std::setw(21) << "Name" << "|" << std::right << std::setw(10) << "Average" << "\n";
        
        std::cout << std::left << std::setw(21) << "OpenCV Nearest" << " " << std::flush;
        cur_time = execute_opencv_resize(opencv_src_mat, ITERATIONS, cur_scale, cv::INTER_NEAREST);
        std::cout << std::right << std::setw(10) << to_mili_seconds(cur_time / ITERATIONS) << "\n";
        
        std::cout << std::left << std::setw(21) << "OpenCV Linear" << " " << std::flush;
        cur_time = execute_opencv_resize(opencv_src_mat, ITERATIONS, cur_scale, cv::INTER_LINEAR);
        std::cout << std::right << std::setw(10) << to_mili_seconds(cur_time / ITERATIONS) << "\n";
        
        std::cout << std::left << std::setw(21) << "OpenCV Area" << " " << std::flush;
        cur_time = execute_opencv_resize(opencv_src_mat, ITERATIONS, cur_scale, cv::INTER_AREA);
        std::cout << std::right << std::setw(10) << to_mili_seconds(cur_time / ITERATIONS) << "\n";
        
        std::cout << std::left << std::setw(21) << "OpenCV Cubic" << " " << std::flush;
        cur_time = execute_opencv_resize(opencv_src_mat, ITERATIONS, cur_scale, cv::INTER_CUBIC);
        std::cout << std::right << std::setw(10) << to_mili_seconds(cur_time / ITERATIONS) << "\n";
        
        std::cout << std::left << std::setw(21) << "OpenCV Lanczos4" << " " << std::flush;
        cur_time = execute_opencv_resize(opencv_src_mat, ITERATIONS, cur_scale, cv::INTER_LANCZOS4);
        std::cout << std::right << std::setw(10) << to_mili_seconds(cur_time / ITERATIONS) << "\n";
    }

    std::cout << "\n";

    return 0;
}
