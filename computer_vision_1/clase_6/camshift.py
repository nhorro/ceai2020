# -*- coding: utf-8 -*-
import numpy as np
import cv2 as cv
import matplotlib.pyplot as plt
import argparse

MAIN_WINDOW_NAME = "Video"

class Application:
    def __init__(self,args):
        self.mode = 'IDLE'
        self.has_roi = False
        self.args = args
        self.frame = None
        pass

    def get_mode(self):
        return self.mode()

    def setup(self):
        if self.args.file == '':
            self.cap = cv.VideoCapture(self.args.cam)
        else:
            self.cap = cv.VideoCapture(args.file )
        ret, self.frame = self.cap.read()
        if ret:        
            cv.namedWindow(MAIN_WINDOW_NAME,cv.WINDOW_NORMAL)
            cv.imshow(MAIN_WINDOW_NAME, self.frame)
            cv.setMouseCallback(MAIN_WINDOW_NAME, self.mouse_callback)
            #if args.w != None and args.h != None:
            #   cv.resizeWindow(MAIN_WINDOW_NAME, self.args.w,self.args.h )
        else:
            raise RuntimeError("Falla al abrir dispositivo de captura de video")
        pass

    def teardown(self):
        cv.destroyAllWindows()
        self.cap.release()

    def run(self):
        self.setup()

        # Condición de terminación: 10 iteraciones o moverse al menos 1pt
        self.term_crit = ( cv.TERM_CRITERIA_EPS | cv.TERM_CRITERIA_COUNT, 1, 5 )

         # Main loop
        while (1):                        
            k = cv.waitKey(30) & 0xff
            if k == 27:
                break
            elif k == ord('x'):
                self.mode = "SELECT_ROI"

            if self.has_roi:
                hsv = cv.cvtColor(self.frame, cv.COLOR_BGR2HSV)
                dst = cv.calcBackProject([hsv],[0],self.roi_hist,[0,180],1)
                cv.imshow(MAIN_WINDOW_NAME, self.frame)
                ret, self.track_window = cv.CamShift(dst, self.track_window, self.term_crit)
                if ret:
                    x,y,w,h = self.track_window 
                    self.frame = cv.rectangle(self.frame, (x,y), (x+w,y+h), 255,2)
                hsv = cv.rectangle(hsv , (x,y), (x+w,y+h), (0,0,255),4)                    
                cv.imshow("BackProjection", hsv)                    

            self.frame = cv.putText( self.frame, 
                     'MODE: '+ self.mode, 
                     (10,self.frame.shape[0]-20), 
                     cv.FONT_HERSHEY_SIMPLEX, 1, (0,0,255), 2, cv.LINE_AA) 
            cv.imshow(MAIN_WINDOW_NAME, self.frame)
            
            # Read next frame
            ret, self.frame = self.cap.read()

        self.teardown()

    def mouse_callback(self,event,x,y,flags,param):
        if event == cv.EVENT_LBUTTONDOWN:
            x,y,w,h = cv.selectROI(MAIN_WINDOW_NAME, self.frame,True,True)   
            self.has_roi = True         
            roi = self.frame[y:(y+h),x:(x+w)]
            cv.imshow("ROI", roi)
            self.mode = "TRACKING"
            # Pasamos la ROI a HSV (los valores de H son más estables a cambios de intensidad)
            hsv_roi =  cv.cvtColor(roi, cv.COLOR_BGR2HSV)
            # Definimos una máscara para el histograma (Hue: 0..180, Saturation:60..255, Value:32..255)
            mask = cv.inRange(hsv_roi, np.array([0., 60.,32.]), np.array([180.,255.,255.]))
            # Y calculamos el histograma sobre esa máscara (toma solo el Hue: tonalidad)
            self.roi_hist = cv.calcHist([hsv_roi],[0],mask,[180],[0.,180.])
            # Normalizamos el histograma (entre 0 y 255 según indica el flag cv.NORM_MINMAX)
            cv.normalize(self.roi_hist,self.roi_hist,0,255,cv.NORM_MINMAX);
            self.track_window = (x,y,w,h)

            plt.imshow(mask)
            plt.title("Mask")
            plt.show(block = False)

            plt.hist(self.roi_hist)
            plt.title("ROI Histogram")
            plt.show(block = False)

        return True
            
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Seguimiento con CAMSHIFT')
    parser.add_argument('--file', default = '', help='Archivo de entrada. Si es nulo, usa webcam')
    parser.add_argument('--cam', type=int, default = 0, help='Webcam')
    #parser.add_argument('--w', default = None, help='Ancho en pixels(opcional)')
    #parser.add_argument('--h', default = None, help='Alto en pixels(opcional)')
    args = parser.parse_args()
    app = Application(args)
    app.run()


