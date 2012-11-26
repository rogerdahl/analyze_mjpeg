analyze_mjpeg
=============

A simple app that connects to a MJPEG web camera, performs some filtering on the
video and displays the filtered video.

The app mainly shows how to:

- Read and process a MJPEG stream using C++ and Boost.
- Use Boost::ASIO to read an MJPEG stream.
- Parse the MJPEG stream (multipart MIME).
- Uncompress JPEGs in memory with IJG libjpeg.
- Perform some filtering on the stream.
- Display the filtered stream with SDL.

There is minimal error checking and no attempt to recover from glitches in the
stream.


Filters
-------

There are two filters implemented:

FilterAverage: Displays the average of each pixel.

FilterNightVision: Calculates the inter-frame and intra-frame average of each
pixel. Uses the intra-frame average to determine the black level for each frame.
The black level is then used as a basis for amplifying each color channel to
cover the entire color range. The inter-frame average is then updated and
displayed.

Technologies
------------

- C++
- Boost::ASIO
- SDL (Simple DirectMedia Layer)
- IJG libjpeg


Platform
--------

Project files for Visual Studio 2010 and binary for 64-bit Windows are included.
The project is untested on other platforms, but is intended to be cross-platform.
