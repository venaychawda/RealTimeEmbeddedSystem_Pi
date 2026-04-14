/* cam_capture.c — Day 17: OpenCV Timed Frame Capture
 *
 * WHY: Measures per-frame capture latency to determine WCET for camera service.
 * First frame is typically slower (camera warmup). Subsequent frames give
 * realistic WCET estimate.
 *
 * BUILD:  gcc cam_capture.c -o cam_capture $(pkg-config --cflags --libs opencv4) -lrt
 * RUN:    ./cam_capture
 * EXPECT: 10 frames at 320x240, first ~85ms, rest ~33ms each.
 */

#include <stdio.h>
#include <time.h>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/highgui/highgui_c.h>

int main() {
    CvCapture *cap = cvCaptureFromCAM(0);
    if (!cap) { printf("Cannot open camera\n"); return 1; }

    cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_WIDTH, 320);
    cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_HEIGHT, 240);

    for (int i = 0; i < 10; i++) {
        struct timespec t1, t2;
        clock_gettime(CLOCK_MONOTONIC, &t1);
        IplImage *frame = cvQueryFrame(cap);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        long ms = (t2.tv_sec-t1.tv_sec)*1000 + (t2.tv_nsec-t1.tv_nsec)/1000000;
        printf("Frame %d: %dx%d captured in %ld ms\n",
               i, frame->width, frame->height, ms);
        char fname[64];
        sprintf(fname, "capture_%02d.jpg", i);
        cvSaveImage(fname, frame, 0);
    }
    cvReleaseCapture(&cap);
    return 0;
}
