/* cam_capture.cpp — Day 17: OpenCV Timed Frame Capture
 *
 * WHY: Measures per-frame capture latency to determine WCET for camera service.
 * First frame is typically slower (camera warmup). Subsequent frames give
 * realistic WCET estimate.
 *
 * NOTE: Uses OpenCV 4 C++ API (cv::VideoCapture, cv::Mat, cv::imwrite).
 * The old C API (CvCapture, IplImage, cvSaveImage) has been removed in
 * OpenCV 4. The POSIX timing calls (clock_gettime) work identically in C++.
 *
 * BUILD:  g++ cam_capture.cpp -o cam_capture $(pkg-config --cflags --libs opencv4) -lrt
 * RUN:    ./cam_capture
 * EXPECT: 10 frames at 320x240, first ~85ms, rest ~33ms each.
 */

#include <cstdio>
#include <ctime>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>

int main() {
    /* Open the first available camera (index 0 = /dev/video0) */
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        printf("Cannot open camera\n");
        return 1;
    }

    /* Set low resolution for speed — important when targeting 10 Hz.
     * 320x240 reduces both capture and processing time vs 640x480. */
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);

    cv::Mat frame;

    for (int i = 0; i < 10; i++) {
        struct timespec t1, t2;

        /* Measure how long the driver takes to deliver one frame.
         * This becomes the WCET of your camera service. */
        clock_gettime(CLOCK_MONOTONIC, &t1);
        cap >> frame;
        clock_gettime(CLOCK_MONOTONIC, &t2);

        if (frame.empty()) {
            printf("Frame %d: capture failed\n", i);
            continue;
        }

        long ms = (t2.tv_sec - t1.tv_sec) * 1000 +
                  (t2.tv_nsec - t1.tv_nsec) / 1000000;
        printf("Frame %d: %dx%d captured in %ld ms\n",
               i, frame.cols, frame.rows, ms);

        /* Save the frame as JPEG — cv::imwrite replaces the removed cvSaveImage */
        char fname[64];
        sprintf(fname, "capture_%02d.jpg", i);
        cv::imwrite(fname, frame);
    }

    /* VideoCapture is released automatically by its destructor,
     * but explicit release is good practice in RT code. */
    cap.release();
    return 0;
}
