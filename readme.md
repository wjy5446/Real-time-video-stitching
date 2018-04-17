# Real-time Video stitching Using Camera Path Estimation and Homography Refinement
> This is a framework that combines multiple frames acquired from moving cameras
>
> For reduce stitching time, we estimate homography using the optical-flow algorithm (Camera Path Estimation), then remove errors of homography using the block-matching (Homograpy Refinement).



**Paper address** [link](http://www.mdpi.com/2073-8994/10/1/4) "Yoon, J., & Lee, D. (2017). Real-Time Video Stitching Using Camera Path Estimation and Homography Refinement. *Symmetry*, *10*(1), 4."

**Video address** [link](https://sites.google.com/site/khuaris/home/video-stitching)



![alt text](https://github.com/wjy5446/Real-time-video-stitching/blob/master/image/abstract.png)



## 1. Abstract

  We propose a novel real-time video stitching method using camera path estimation and homography refinement. The method can stably stitch multiple frames acquired from moving cameras in real time. In the proposed method, one initial between-camera (BC) homography and each camera path (CP) homography are used to estimate the BC homography at every frame. The BC homography is refined by using block matching to adjust the errors of estimated CPs (homography refinement). For fast processing, we extract features using the difference of intensities and use the optical flow to estimate camera motion (CM) homographies, which are multiplied with the previous CMs to calculate CPs (camera path estimations). In experiments, we demonstrated the performance of the CP estimation and homography refinement approach by comparing it with other methods. The experimental results show that the proposed method can stably stitch two image sequences at a rate exceeding 13 fps (frames per second).



## 2. Overall Method

![head](https://github.com/wjy5446/Real-time-video-stitching/blob/master/image/method.png)







## 3. Camera Path Estimation

### 3-1. Feature Extraction (diffrence of intensity on grid)

![head](https://github.com/wjy5446/Real-time-video-stitching/blob/master/image/CPestimation1.png)



### 3-2. Feature Tracking (optical flow)

![head](https://github.com/wjy5446/Real-time-video-stitching/blob/master/image/CPestimation2.png)



### 3-3. Result

![head](https://github.com/wjy5446/Real-time-video-stitching/blob/master/image/CPestimation_result.png)



## 4. Homography Refinement

### 4-1. Homography Refinement (block-matching)

![head](https://github.com/wjy5446/Real-time-video-stitching/blob/master/image/HomographyRefinement.png)



### 4-2. Result

![head](https://github.com/wjy5446/Real-time-video-stitching/blob/master/image/HomographyRefinement_result.png)



## 5. Result

- Sample Result

![head](https://github.com/wjy5446/Real-time-video-stitching/blob/master/image/result2.png)

![head](https://github.com/wjy5446/Real-time-video-stitching/blob/master/image/result.png)

**Link** : [result video](https://sites.google.com/site/khuaris/home/video-stitching)



- Time


![head](https://github.com/wjy5446/Real-time-video-stitching/blob/master/image/result3.png)



## Code

> It is based on OpenCV 2.4.1.1,  MFC

```sh
npm install my-crazy-module --save
```

Windows:

```sh
edit autoexec.bat
```



## Thanks to



