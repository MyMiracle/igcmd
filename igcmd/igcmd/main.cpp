//
//  main.cpp
//  igcmd
//
//  Created by Yang Zhang on 11/10/14.
//  Copyright (c) 2014 Yang Zhang. All rights reserved.
//

#include "Igcmd.h"
#include <opencv2/highgui/highgui.hpp>
#include <chrono>

using namespace std::chrono;
using namespace cv;

Igcmd* gIgcmd = NULL;
milliseconds lastTick;

void createObject()
{
    gIgcmd = new Igcmd();
}

void destroyObject()
{
    delete gIgcmd;
    gIgcmd = NULL;
}

void start()
{
    lastTick = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
    
    gIgcmd->start();
}

int main(){
    VideoCapture cap(0);
    namedWindow("img1",CV_WINDOW_KEEPRATIO);
    Mat image;
    
    createObject();
    
    start();
    int key = 0;
    
    while ((key = cv::waitKey(30)) != char('q'))
    {
        if (key == char('r'))
        {
            gIgcmd->stop();
            gIgcmd->start();
            continue;
        }
        
        milliseconds cur = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
        
        float timeTick = (float)(cur.count() - lastTick.count()) / 1000;
        lastTick = cur;
        
        cap >> image;
        
        gIgcmd->update(image, timeTick);
        
        imshow("img1", image);
    }
    
    destroyObject();
    
    destroyAllWindows();
    cap.release();
    return 0;
}

