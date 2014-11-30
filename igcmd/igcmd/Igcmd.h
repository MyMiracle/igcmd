//
//  Igcmd.h
//  igcmd
//
//  Created by Yang Zhang on 11/17/14.
//  Copyright (c) 2014 Yang Zhang. All rights reserved.
//

#ifndef _IGCMD_H_
#define _IGCMD_H_

#include <vector>
#include <opencv2/core/core.hpp>


#define NSAMPLES                        (7)


class My_ROI;
class GCManager;
class MyImage;
class HandGesture;

class Igcmd
{
public:
    enum E_STATE
    {
        E_STATE_PREPARING_TRAINING,
        E_STATE_WAITING_PALM,
        E_STATE_AVERAGING,
        E_STATE_TRACKING,
        
        E_STATE_NR
    };
    
public:
    Igcmd();
    ~Igcmd();
    
    void start();
    void update(cv::Mat image, float fTimeTick);
    void stop();
    
    bool isTracking()
    {
        return eState == E_STATE_TRACKING;
    }
    
private:
    void printText(cv::Mat src, std::string text);
    void waitForPalmCover(MyImage* m);
    int getAverage(std::vector<int> val);
    void getAvgColor(MyImage *m,My_ROI roi,int avg[3]);
    void average(MyImage *m);
    void initTrackbars();
    void normalizeColors(MyImage * myImage);
    void produceBinaries(MyImage *m);
    void showWindows(MyImage* m);
    int findBiggestContour(std::vector<std::vector<cv::Point> > contours);
    void myDrawContours(MyImage *m,HandGesture *hg);
    void makeContours(MyImage *m, HandGesture* hg);
    void prepareTraining(MyImage* m);
    void reset();
    
private:
    int avgColor[NSAMPLES][3] ;
    int c_lower[NSAMPLES][3];
    int c_upper[NSAMPLES][3];
    std::vector<My_ROI> roi;
    
    E_STATE eState;
    int frameCounting;
    bool bGcStarted;
    GCManager* gcManager;
    MyImage* m;
    HandGesture* hg;
};

#endif /* defined(_IGCMD_H_) */
