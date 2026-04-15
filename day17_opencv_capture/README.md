# Day 17: OpenCV Frame Capture in C++

## Goal
Capture camera frames programmatically and measure per-frame latency (WCET for camera service).

## Why C++ instead of C?
OpenCV 4 has removed the legacy C API (`CvCapture`, `IplImage`, `cvSaveImage`). The modern C++ API (`cv::VideoCapture`, `cv::Mat`, `cv::imwrite`) is the supported interface. All POSIX RT calls (`clock_gettime`, `pthread_create`, `sched_setscheduler`, etc.) work identically in C++, so this change does **not** affect any other file in the project.

## Build & Run
```bash
make
./cam_capture
```

## Expected Output
```
Frame 0: 320x240 captured in 85 ms
Frame 1: 320x240 captured in 33 ms
Frame 2: 320x240 captured in 33 ms
...
Frame 9: 320x240 captured in 34 ms
```

**Verify:**
- Frame 0 is slower (50–200 ms) — camera initialization and auto-exposure settling
- Subsequent frames should be ~33 ms (30 fps) or ~100 ms (10 fps) depending on your camera
- Resolution shows 320x240
- 10 JPEG files created (`capture_00.jpg` to `capture_09.jpg`), each 10–50 KB
- If you see `Cannot open camera`, check that no other process has `/dev/video0` open