import time  
import picamera  
  
cam0 = picamera.PiCamera()  
cam0.resolution = (640,480)  
cam0.start_preview(fullscreen=False,window=(0,50,440,280)) 
 
cam1 = picamera.PiCamera(camera_num=1)  
cam1.resolution = (640,480)  
cam1.start_preview(fullscreen=False,window=(380,50,440,280))  

time.sleep(100)