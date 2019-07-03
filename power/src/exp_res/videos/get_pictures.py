import cv2
import numpy as np
import matplotlib.pyplot as plt

picture1 = cv2.imread('./day_pic1.png')
picture2 = cv2.imread('./day_pic2.png')
picture3 = cv2.imread('./day_pic3.png')
picture4 = cv2.imread('./day_pic4.png')
picture5 = cv2.imread('./day_pic5.png')
picture6 = cv2.imread('./day_pic6.png')

gray_pic1 = cv2.cvtColor(picture1, cv2.COLOR_BGR2GRAY)
gray_pic2 = cv2.cvtColor(picture2, cv2.COLOR_BGR2GRAY)
gray_pic3 = cv2.cvtColor(picture3, cv2.COLOR_BGR2GRAY)
gray_pic4 = cv2.cvtColor(picture4, cv2.COLOR_BGR2GRAY)
gray_pic5 = cv2.cvtColor(picture5, cv2.COLOR_BGR2GRAY)
gray_pic6 = cv2.cvtColor(picture6, cv2.COLOR_BGR2GRAY)


np.savez('traffic_pictures_day.npz',
         pic1=gray_pic1, pic2=gray_pic2, pic3=gray_pic3,
         pic4=gray_pic4, pic5=gray_pic5, pic6=gray_pic6)


picture1 = cv2.imread('./night_pic1.png')
picture2 = cv2.imread('./night_pic2.png')
picture3 = cv2.imread('./night_pic3.png')
picture4 = cv2.imread('./night_pic4.png')
picture5 = cv2.imread('./night_pic5.png')
picture6 = cv2.imread('./night_pic6.png')

gray_pic1 = cv2.cvtColor(picture1, cv2.COLOR_BGR2GRAY)
gray_pic2 = cv2.cvtColor(picture2, cv2.COLOR_BGR2GRAY)
gray_pic3 = cv2.cvtColor(picture3, cv2.COLOR_BGR2GRAY)
gray_pic4 = cv2.cvtColor(picture4, cv2.COLOR_BGR2GRAY)
gray_pic5 = cv2.cvtColor(picture5, cv2.COLOR_BGR2GRAY)
gray_pic6 = cv2.cvtColor(picture6, cv2.COLOR_BGR2GRAY)


np.savez('traffic_pictures_night.npz',
         pic1=gray_pic1, pic2=gray_pic2, pic3=gray_pic3,
         pic4=gray_pic4, pic5=gray_pic5, pic6=gray_pic6)


picture1 = cv2.imread('./feature1_car.png')
picture2 = cv2.imread('./feature2_car.png')
picture3 = cv2.imread('./feature3_car.png')
picture4 = cv2.imread('./feature1_sign.png')
picture5 = cv2.imread('./feature2_sign.png')
picture6 = cv2.imread('./feature3_sign.png')

gray_pic1 = cv2.cvtColor(picture1, cv2.COLOR_BGR2GRAY)
gray_pic2 = cv2.cvtColor(picture2, cv2.COLOR_BGR2GRAY)
gray_pic3 = cv2.cvtColor(picture3, cv2.COLOR_BGR2GRAY)
gray_pic4 = cv2.cvtColor(picture4, cv2.COLOR_BGR2GRAY)
gray_pic5 = cv2.cvtColor(picture5, cv2.COLOR_BGR2GRAY)
gray_pic6 = cv2.cvtColor(picture6, cv2.COLOR_BGR2GRAY)


np.savez('traffic_features.npz',
         pic1=gray_pic1, pic2=gray_pic2, pic3=gray_pic3,
         pic4=gray_pic4, pic5=gray_pic5, pic6=gray_pic6)
