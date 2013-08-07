************************************************************
* Author: Subhav Pradhan <subhav.m.pradhan@vanderbilt.edu> *
*                                                          *
* This is a README file that contains information          *
* about the human detection application.                   *
*                                                          *
************************************************************

This application can use different user defined strategies
to detect human using thermal sensors (Grideye).

This application *REQUIRES* four input parameters:
-s: Human detection strategy. As of right now we have two 
    strategies implemented - (a) Basic strategy, and (b) 
    Sliding window strategy. These are described in detail
    below.

-h: Desired altitude (in meters) for the drone to attain
    before running the detection algorithm.

-e: Expected error. This value will be used to account for
    expected noise when calculating ambient temperature
    range.

-l: Lower bound of expected human temperature.

-u: Upper bound of expected human temperature.

In order to see help -h argument can be used.

******************
* Basic Strategy *
******************
This strategy uses a comparitively simple algorithm to 
detect human. The algorithm reads certain number of frame 
samples (set statically in helpers.h file) and determines 
the ambient temperature range, across the entire 8x8 matrix,
using the minimum and maximum temperature read. Once the 
ambient temperature range is set, the algorithm uses that 
range to determine anomalies. After an anomaly has been 
detected, the algorithm checks whether the detected anomaly 
is a human. In order to detect human, this algorithm uses 
lower and upper bounds of human temperature (determined by 
user input parameters -l and -u).

***************************
* Sliding Window Strategy *
***************************
This strategy uses sliding window approach where
environment models are constructed at certain interval
to account for changing environment which is not
considered by the Basic Strategy. The models consists of
maximum and minimum temperature observed in a given
window. This model is then used as a basis to detect
anomalies.

**NOTE: This Sliding Window Strategy cannot detect
an object (including human) more than once if both
the drone and the object doesn't move. Therefore,
this algorithm assumes that the drone incurs movement
even while it is in a hovering state.
