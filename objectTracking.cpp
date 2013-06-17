#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <vector>
#include <string>
#include <iostream>

#define PI 3.1415926535;


using namespace cv;
using namespace std;

inline static double square(int a)
{
    return a * a;
}

class FeatureDetector
{
private:
    Mat gray;  // current grayscale frame
    Mat grayPrev; // previous grayscale frame

    vector<Point2f> points[2];  // Tracked features from 0->1
    vector<Point2f> initial;    // Initial position of tracked points
    vector<Point2f> features;   // detected features
    vector <uchar> status;      // status of tracked features
    vector <float> err;         // error in tracking

public:
    // Determine if new points should be added
    bool addNewPoints()
    {
        // if too few points
        cout << "Number of features in current frame = " << points[0].size() << endl;
        return points[0].size() <= 10;
    }

    // Feature point detection
    void detectFeaturePoints()
    {
        cout << "\nDetecting Features..." << endl;
        int maxCorners = 200;
        // Parameters for Shi-Tomasi algorithm
        double qualityLevel = 0.001;
        double minDistance = 5;

        // Detect the features
        cv::goodFeaturesToTrack(grayPrev,       // input image
                                features,   // output detected features
                                maxCorners,  // max number of features
                                qualityLevel,     // quality level
                                minDistance);   // min distance between 2 features
        cout << "Number of features detected = " << features.size() << endl;

//        // Copy the source image
//        Mat src = framePrev.clone();

//        // Draw the detected corners
//        int radius = 3;
//        for(size_t i = 0; i < features.size(); i++ )
//        { circle( src, features[i], radius, cv::Scalar(255,0,255), -1, 8); }

//        imshow("Detected feature points", src);
    }

    // Determine if tracked point should be accepted
    // Criteria: if (Point cannot be detected by calcOpticalFlowPyrLK function)
    // OR (Point does not move), reject it
    bool acceptTrackedPoint(int i)
    {
        cout << "i = " << i << endl;
        cout << "status[i] = " << (int)status[i] << endl;
        return status[i] &&
                (abs(points[0][i].x - points[1][i].x) +
                (abs(points[0][i].y - points[1][i].y)) > 2);
    }

    // Draw tracked points
    void drawTrackedPoints(Mat& output)
    {
        // for all tracked points
        for (size_t i = 0; i < points[1].size(); i++)
        {
            // Draw line and circle
            cv::line(output, initial[1], points[1][i], cv::Scalar(255,0,255));
            cv::circle(output, points[1][i], 3, cv::Scalar(0,0,255), -1, 8);
        }
        imshow("Feature Tracking", output );
    }

    // Perform entire Feature Detection pipeline on give frame
    void performFeatureDetection(Mat& frame)
    {
        cvtColor(frame, gray, CV_BGR2GRAY);

        // If number of feature points is insufficient, detect more
        if (addNewPoints())
        {
            // Detect feature points
            detectFeaturePoints();

            // Add detected features to currently tracked features
            points[0].insert(points[0].end(), features.begin(), features.end());
            // Set initial position of features with detected features
            initial.insert(initial.end(), features.begin(), features.end());

            cout << "points[0] size = " << points[0].size() << endl;
            cout << "initial size = " << initial.size() << endl;
        }

        // For first frame of sequence
        if (grayPrev.empty())
            gray.copyTo(grayPrev);

        // Track features using Lucas-Kanade method
        calcOpticalFlowPyrLK(grayPrev, gray,  // 2 consecutive frames
                             points[0],       // input point positions in prev frame
                             points[1],       // output point positions in current frame
                             status,          // tracking success
                             err);            // tracking error

        // Loop over the tracked points to reject some
        int numValid = 0;
        for (size_t i = 0; i < points[1].size(); i++)
        {
            // Do we keep this point?
            if (acceptTrackedPoint(i))
            {
                // Set the tracked points as initial position of features for next iteration
                initial[numValid] = initial[i];
                points[1][numValid++] = points[1][i];
            }
        }

        // Reduce vectors length to number of valid feature points
        points[1].resize(numValid);
        initial.resize(numValid);

        // Draw the trail of the tracked points
        drawTrackedPoints(frame);

        // Set the current (points,image) = previous (points,image) for next iteration
        std::swap(points[1], points[0]);
        cv::swap(grayPrev, gray);
    }
};

int main(int argc, char* argv[])
{
    VideoCapture video(argv[1]);

    if (!video.isOpened())
    {
        cout << "Error opening video!\n";
        return 1;
    }

    double videoFPS = video.get(CV_CAP_PROP_FPS);

    // print all the properties of the video
    cout << "FPS = " << video.get(CV_CAP_PROP_FPS) << endl;
    cout << "Width = " << video.get(CV_CAP_PROP_FRAME_WIDTH) << endl;
    cout << "Height = " << video.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
    cout << "Num frames = " << static_cast<int>(video.get(CV_CAP_PROP_FRAME_COUNT)) << endl;



    waitKey(0);
    return 0;
}
