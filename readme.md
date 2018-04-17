# Real-time Video stitching Using Camera Path Estimation and Homography Refinement
> This is a framework that combines multiple frames acquired from moving cameras

**Paper** [link](http://www.mdpi.com/2073-8994/10/1/4) **address**



![head]("\image\abstract.png")



## Abstract

  We propose a novel real-time video stitching method using camera path estimation and homography refinement. The method can stably stitch multiple frames acquired from moving cameras in real time. In the proposed method, one initial between-camera (BC) homography and each camera path (CP) homography are used to estimate the BC homography at every frame. The BC homography is refined by using block matching to adjust the errors of estimated CPs (homography refinement). For fast processing, we extract features using the difference of intensities and use the optical flow to estimate camera motion (CM) homographies, which are multiplied with the previous CMs to calculate CPs (camera path estimations). In experiments, we demonstrated the performance of the CP estimation and homography refinement approach by comparing it with other methods. The experimental results show that the proposed method can stably stitch two image sequences at a rate exceeding 13 fps (frames per second).



## Method

![head]("\image\method.png")



## Camera Path Estimation



## Homography Refinement



## Result

- ​
- ​



## Installation

OS X & Linux:

```sh
npm install my-crazy-module --save
```

Windows:

```sh
edit autoexec.bat
```

## Usage example

A few motivating and useful examples of how your product can be used. Spice this up with code blocks and potentially more screenshots.

_For more examples and usage, please refer to the [Wiki][wiki]._

## Development setup

Describe how to install all development dependencies and how to run an automated test-suite of some kind. Potentially do this for multiple platforms.

```sh
make install
npm test
```



## Meta

Your Name – [@YourTwitter](https://twitter.com/dbader_org) – YourEmail@example.com

Distributed under the XYZ license. See ``LICENSE`` for more information.

[https://github.com/yourname/github-link](https://github.com/dbader/)



## Thanks to



