gqrxVI
An interface to gqrx for visually impaired users

This version expects the users own screen reader software to 'do the talking'.
Control-X to exit
Control-Y to synchronize the frequency between gqrx and this program (mainly used during debugging)

gqrx must be started with the 'Remote Control via TCP' button clicked and Port used is the default for gqrx

There are basically 4 different areas on the screen:
- boxes that shows the current frequency and mode gqrx is tuned to, that a screen reader should be able to speak.
- a spin box that allows quick selection of all ham bands from Top Band to 70cm. By default this tunes to the bottom of the band so the user has a known reference point that most hams know.
- a spin box that contains all the modes that gqrx supports (which is loaded by querying gqrx at start up so is future proof)
- a spin box to control bandwidth, this defaults to the bandwidth of the chosen mode but the spin box can be used to alter the bandwidth (the version of gqrx that supports this feature must be installed) 
- spin boxes for frequency. Each frequency digit has its own spin box, from gigahertz to tens of hertz. Frequency rollover is implemented so moving from 9 to 0 will cause the higher frequency digit to increase by 1 and the opposite for going down in frequency.
- moving between all the selections is achieved by the Tab key or Shift-Tab to move backwards.
- changing the options is achieved by pressing the up or down cursor keys.

Copyright 2017 Simon Kennedy g0fcu at g0fcu.com
