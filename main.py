import numpy as np
import cv2 as cv


cap = cv.VideoCapture(0)
while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()
    flipped = cv.flip(frame, 1)

    # Our operations on the frame come here
    gray = cv.cvtColor(flipped, cv.COLOR_BGR2GRAY)

    edges = cv.Canny(gray,50,75)

    # Display the resulting frame
    cv.imshow('Camera', edges)
    if cv.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap.release()
cv.destroyAllWindows()
