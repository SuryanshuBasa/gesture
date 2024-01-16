setting range of acceleration -> 4g
need to remove 1g from z axis.

max gx gy gz is not crossing 20

need ways to normalize acceleration to [-1 , 1] from [-4g , 4g]
need ways to normalose gyroscope values [-1 , 1] from [-20 , 20]

(todo and explore and check impact on model performance during inference) ways to normalize:
set range to [-1,1]
set range to [0,1]
use standard scalar from sk learn on individual ax , ay , az , gx , gy , gz columns

(todo)need to check duration for each gesture
number of samples = baud rate
                    _______________    x    duration of gesture
                    bits per sample
