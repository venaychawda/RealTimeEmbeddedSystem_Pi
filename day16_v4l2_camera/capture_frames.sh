#!/bin/bash
# Day 16: V4L2 Camera Enumeration & Frame Capture
echo "=== Camera Capabilities ==="
v4l2-ctl -d /dev/video0 --all 2>/dev/null || echo "Cannot open /dev/video0"
echo ""

echo "=== Supported Formats & Frame Rates ==="
v4l2-ctl --list-formats-ext 2>/dev/null || echo "Cannot list formats"
echo ""

echo "=== Capturing 10 frames with ffmpeg ==="
ffmpeg -f v4l2 -framerate 10 -video_size 640x480 \
       -i /dev/video0 -frames:v 10 frame_%03d.jpg -y 2>/dev/null

echo ""
echo "=== Captured files ==="
ls -la frame_*.jpg 2>/dev/null || echo "No frames captured — check camera connection."
