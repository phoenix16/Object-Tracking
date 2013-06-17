#include "FeatureDetector.h"

#define PI 3.1415926535;
#define NUM_FRAMES_TO_PROCESS  650

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout << "Usage: ./ObjectTracker <input video file>" << endl;
        return -1;
    }

    VideoCapture inputVideo(argv[1]);
    if (!inputVideo.isOpened())
    {
        cout << "Error opening input video " << argv[1] << endl;
        return -1;
    }
    double fps = inputVideo.get(CV_CAP_PROP_FPS);
    int delay = 1000/fps;    // time in ms between successive frames = 1000/fps
    cout << "FPS = " << fps << endl;
    cout << "Number of frames = " << static_cast<int>(inputVideo.get(CV_CAP_PROP_FRAME_COUNT)) << endl;

    // Create an instance of Feature Detector class
    FeatureDetector feat;

    Mat frame;
    bool stop(false);
    int frameNum = 1;

    namedWindow("input video");
    moveWindow("input video", 600, 300);
    // Loop to process every frame of video
    while(!stop)
    {
        inputVideo >> frame;              // read
        if( frame.empty()) break;       // check if at end

        imshow("input video", frame);
        cout << "\nFrame Number : " << frameNum << endl;
        frameNum++;

        feat.performFeatureDetection(frame);

        if (inputVideo.get(CV_CAP_PROP_POS_FRAMES) == NUM_FRAMES_TO_PROCESS)
        {
            cout << "\nDone" << endl;
            break;
        }

        // introduce delay or press key to stop
        if (waitKey(delay) >= 0)
            stop = true;

    }

    return 0;
}


