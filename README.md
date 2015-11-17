# Blink Detector

This is an experiment. It uses an [Arduino Uno R3 SMD](https://www.sparkfun.com/products/11224) -- though any Arduino with [MouseKeyboard](https://www.arduino.cc/en/Reference/MouseKeyboard) support will work fine -- along with a [VCNL4000 Infrared Proximity Breakout](https://www.sparkfun.com/products/retired/10901) as a primitive detector of eye blinks. Hold the proximity sensor very close to your eye (from the top looking down seems to work best) and the setup will detect the small changes in reflectivity when the eyelid is down.

The average length of a blink is 100-400 milliseconds, and this system is able to handle that with no problem. We use a rudimentary averaging function to try to reduce noise.

The Arduino is programmed to act like a USB keyboard and to deliver a space bar whenever a blink is detected. This in turn can be recieved by anything on a computer; in this case, we've built a couple of web pages with JavaScript listeners for any keypress (just because it's an experiment).

Web page examples
-------
If these work properly, the viewer should never see a transition between states -- that happens while the eye is closed during the blink.
* [index.html](index.html): Change the background color with every blink
* [index-1.html](index-1.html): Show a different animal picture with every blink
* [index-2.html](index-2.html): While the eyelid is closed (for 100-400 milliseconds), show a picture, in this case of Baywatch and Knight Rider star David Hasselhoff. If it's set properly, the viewer should *never* see the Hoff, though onlookers will. 
* [Play Flappy Bird](http://flappybird.io/?game) with your blinks!