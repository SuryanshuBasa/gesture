setting range of acceleration -> 4g
need to remove 1g from z axis.

max gx gy gz is not crossing 20

need ways to normalize acceleration to [-1 , 1] from [-4g , 4g]
need ways to normalose gyroscope values [-1 , 1] from [-20 , 20]

(todo and explore and check impact on model performance during inference) ways to normalize:
  1.set range to [-1,1]
  2.set range to [0,1]
  3.use standard scalar from sk learn on individual ax , ay , az , gx , gy , gz columns
  for now we are using linear scaling x = (x - minVal) / (maxVal-minVal) range[0,1]

(todo)need to check duration for each gesture:
  number of samples = (baud rate / bits per sample) * duration of gesture

find better ways fir activation of gesture 




GESTURES:
forward fall
left fall
up clockwise
up anti clockwise




TO TRY AND IMPLEMENT:
add a denoising factor to smooth out acceleration
call a calibrate function from setup:
  this take 10 samples of ax ay az values finds the avg of 10 acc samples
      totx = totx + ax
      samplex = totx/10
      similar to y and z
use this value to remove the noise element from the actual reading values
      
