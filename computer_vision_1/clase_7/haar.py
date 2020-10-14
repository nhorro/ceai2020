# -*- coding: utf-8 -*-
import numpy as np
import cv2 as cv
import matplotlib.pyplot as plt
import argparse

MAIN_WINDOW_NAME = "Video"


class Application:
    def __init__(self,args):
        self.args = args
        self.frame = None
        pass

    def setup(self):
        if self.args.file == '':
            self.cap = cv.VideoCapture(self.args.cam)
        else:
            self.cap = cv.VideoCapture(args.file )
        ret, self.frame = self.cap.read()
        if ret:        
            cv.namedWindow(MAIN_WINDOW_NAME,cv.WINDOW_NORMAL)
            cv.imshow(MAIN_WINDOW_NAME, self.frame)
            #if args.w != None and args.h != None:
            #   cv.resizeWindow(MAIN_WINDOW_NAME, self.args.w,self.args.h )
            self.face_cascade = cv.CascadeClassifier()
            self.eyes_cascade = cv.CascadeClassifier()
            self.smile_cascade = cv.CascadeClassifier()

            haar_cascade_filenames = [
                'data/haarcascade_frontalface_default.xml',
                'data/haarcascade_eye_tree_eyeglasses.xml',#'data/haarcascade_eye.xml', 
                'data/haarcascade_smile.xml'
            ]
            haar_filters = [ self.face_cascade, self.eyes_cascade, self.smile_cascade ]

            for i,f in enumerate(haar_filters):
                if not f.load(haar_cascade_filenames[i]):
                    raise RuntimeError("Error al cargar filtro HAAR. %s" % haar_cascade_filenames[i])
            
        else:
            raise RuntimeError("Falla al abrir dispositivo de captura de video")
        pass

    def teardown(self):
        cv.destroyAllWindows()
        self.cap.release()

    def run(self):
        self.setup()

         # Main loop
        while (1):                        
            k = cv.waitKey(30) & 0xff
            if k == 27:
                break
            
            # Pasamos la imagen a escala de grises
            gray = cv.cvtColor(self.frame, cv.COLOR_BGR2GRAY)
            gray = cv.equalizeHist(gray)

            # Llamamos al clasificador de Haar (AdaBoost)
            faces = self.face_cascade.detectMultiScale(gray, 
                            scaleFactor= 1.1, 
                            minNeighbors=5, 
                            minSize= (100,130),
                            maxSize=(400,400))
            
            for (x,y,w,h) in faces:
                # Le dibujamos un rectángulo amarillo
                cv.rectangle(self.frame,(x,y),(x+w,y+h),(255,255,0),2)
                
                # Definimos las ROIs en la imagen gris y color
                roi_gray = gray[y:y+h, x:x+w] 
                roi_color = self.frame[y:y+h, x:x+w]                 
                # Para cada rostro hallado le buscamos los ojos y sonrisa
                eyes = self.eyes_cascade.detectMultiScale(roi_gray, 
                                scaleFactor= 1.1,
                                minNeighbors=3,
                                minSize = (32,32),
                                maxSize = (64,64))  
                for (ex,ey,ew,eh) in eyes:
                    cv.rectangle(roi_color,(ex,ey),(ex+ew,ey+eh),(0,255,0),2)
                
                smiles = self.smile_cascade.detectMultiScale(roi_gray,
                            scaleFactor= 1.1, 
                            minNeighbors=4, 
                            minSize= (50,50),
                            maxSize=(200,300))                 
                if len(smiles) > 0:
                    (sx,sy,sw,sh) = smiles[0]
                    cv.rectangle(roi_color,(sx,sy),(sx+sw,sy+sh),(0,255,255),2)                    
            cv.imshow(MAIN_WINDOW_NAME, self.frame)
            
            # Read next frame
            ret, self.frame = self.cap.read()
            
        self.teardown()
            
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Detección de Características de Rostros con Filtros HAAR')
    parser.add_argument('--file', default = '', help='Archivo de entrada. Si es nulo, usa webcam')
    parser.add_argument('--cam', type=int, default = 0, help='Webcam')
    #parser.add_argument('--w', default = None, help='Ancho en pixels(opcional)')
    #parser.add_argument('--h', default = None, help='Alto en pixels(opcional)')
    args = parser.parse_args()
    app = Application(args)
    app.run()
