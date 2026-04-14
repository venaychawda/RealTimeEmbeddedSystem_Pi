"""motion.py — Day 19: Frame Differencing Motion Detector

WHY: Frame differencing is the simplest motion detection — subtract
consecutive frames and threshold. This is the foundation of the synchronome.

RUN:    python3 motion.py
EXPECT: motion count ~100-500 (still), 10000+ when you wave your hand.
"""
import cv2, time

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)

ret, prev = cap.read()
prev_gray = cv2.cvtColor(prev, cv2.COLOR_BGR2GRAY)

for i in range(100):
    t1 = time.monotonic()
    ret, frame = cap.read()
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    diff = cv2.absdiff(prev_gray, gray)
    _, thresh = cv2.threshold(diff, 30, 255, cv2.THRESH_BINARY)
    motion = cv2.countNonZero(thresh)
    t2 = time.monotonic()
    print(f"Frame {i}: motion={motion:6d} pixels, time={1000*(t2-t1):.1f}ms")
    prev_gray = gray

cap.release()
