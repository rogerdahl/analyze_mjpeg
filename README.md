analyze_mjpeg
=============

A simple app that connects to a MJPEG web camera, performs some filtering on the
video and displays the filtered video.

The app mainly shows how to:

- Read and process a MJPEG stream using C++ and Boost.
- Use ASIO to get the bytes of an MJPEG stream.
- Parse the MJPEG stream (MIME multi-part).
- Uncompress JPEGs in memory with IJG libjpeg.
- Perform some filtering on the stream.
- Display the filtered stream with SDL.

Filters
-------

There are two filters implemented:

FilterAverage: Displays the average of each pixel.

FilterNightVision: Calculates the inter-frame and intra-frame average of each
pixel. Uses the intra-frame average to determine a "black level" for each frame.
The black level is then used as a basis for amplifying each color chanel to
cover the entire color range. The inter-frame average is then updated and
displayed.

Technologies
------------

- C++
- Boost::ASIO
- SDL (Simple DirectMedia Layer)
- Lib
