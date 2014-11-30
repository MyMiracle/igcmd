//
//  Igcmd.cpp
//  igcmd
//
//  Created by Yang Zhang on 11/17/14.
//  Copyright (c) 2014 Yang Zhang. All rights reserved.
//

#include "Igcmd.h"
#include "handGesture.hpp"
#include "myImage.hpp"
#include "roi.hpp"
#include "GCManager.h"

#define ORIGCOL2COL                     (CV_BGR2Lab)
#define COL2ORIGCOL                     (CV_Lab2BGR)

#define WAIT_FOR_PALM_COVRING_FRAMES    (50)
#define AVERAGING_FRAMES                (30)

#define SQUARE_LEN                      (20)

#define DEBUG_PRINT                     (printf)

using namespace cv;
using namespace std;

Igcmd::Igcmd()
: eState(E_STATE_NR)
, frameCounting(0)
, bGcStarted(false)
{
    gcManager = GCManager::getInstance();
    m = new MyImage();
    hg = new HandGesture();
}

Igcmd::~Igcmd()
{
    GCManager::releaseInstance();
    
    if (m)
    {
        delete m;
        m = NULL;
    }
    
    if (hg)
    {
        delete hg;
        hg = NULL;
    }
}


void Igcmd::start()
{
    reset();
    
    eState = E_STATE_PREPARING_TRAINING;
}

void Igcmd::update(Mat image, float fTimeTick)
{
    m->src = image;
    flip(m->src,m->src,1);
    m->src.copyTo(m->srcLR);
    
    switch (eState)
    {
        case E_STATE_PREPARING_TRAINING:
        {
            prepareTraining(m);
            eState = E_STATE_WAITING_PALM;
            frameCounting = 0;
        }
            break;
        case E_STATE_WAITING_PALM:
        {
            waitForPalmCover(m);
            frameCounting ++;
            
            if (frameCounting >= WAIT_FOR_PALM_COVRING_FRAMES)
            {
                eState = E_STATE_AVERAGING;
                frameCounting = 0;
            }
        }
            break;
        case E_STATE_AVERAGING:
        {
            cvtColor(m->srcLR,m->srcLR,ORIGCOL2COL);
            average(m);
            frameCounting ++;
            
            if (frameCounting >= AVERAGING_FRAMES)
            {
                eState = E_STATE_TRACKING;
                initTrackbars();
                frameCounting = 0;
                bGcStarted = false;
                
                for (int i = 0; i < 7; i ++)
                {
                    DEBUG_PRINT("sample %d : \n", i + 1);
                    DEBUG_PRINT("[%d, %d, %d]\n", avgColor[i][0], avgColor[i][1], avgColor[i][2]);
                }
            }
        }
            break;
        case E_STATE_TRACKING:
        {
            hg->frameNumber++;
            
            pyrDown(m->srcLR,m->srcLR);
            GaussianBlur(m->srcLR,m->srcLR,Size(3,3), 5, 5);
            cvtColor(m->srcLR,m->srcLR,ORIGCOL2COL);
            produceBinaries(m);
            makeContours(m, hg);
            hg->getFingerNumber(m);
            
            if (!bGcStarted)
            {
                gcManager->start(hg);
                bGcStarted = true;
            }
            else
            {
                gcManager->update(hg, fTimeTick);
                gcManager->execute(m->src, hg);
            }
            showWindows(m);
        }
            break;
        case E_STATE_NR:
        default:
            break;
    }
}

void Igcmd::stop()
{
    gcManager->stop();
}



void Igcmd::printText(Mat src, std::string text)
{
    putText(src,text,Point(src.cols/2, src.rows/10),FONT_HERSHEY_PLAIN, 1.2f,Scalar(200,0,0),2);
}

int Igcmd::getMedian(vector<int> val)
{
    int median;
    size_t size = val.size();
    sort(val.begin(), val.end());
    if (size  % 2 == 0)  {
        median = val[size / 2 - 1] ;
    } else{
        median = val[size / 2];
    }
    return median;
}

void Igcmd::getAvgColor(MyImage *m,My_ROI roi,int avg[3])
{
    Mat r;
    roi.roi_ptr.copyTo(r);
    vector<int>hm;
    vector<int>sm;
    vector<int>lm;
    // generate vectors
    for(int i=2; i<r.rows-2; i++){
        for(int j=2; j<r.cols-2; j++){
            hm.push_back(r.data[r.channels()*(r.cols*i + j) + 0]) ;
            sm.push_back(r.data[r.channels()*(r.cols*i + j) + 1]) ;
            lm.push_back(r.data[r.channels()*(r.cols*i + j) + 2]) ;
        }
    }
    avg[0]=getMedian(hm);
    avg[1]=getMedian(sm);
    avg[2]=getMedian(lm);
}

void Igcmd::waitForPalmCover(MyImage* m)
{
    for(int i=0;i<NSAMPLES;i++){
        roi[i].draw_rectangle(m->src);
    }
    string imgText=string("Cover rectangles with palm");
    printText(m->src,imgText);
}

void Igcmd::average(MyImage *m)
{
    for(int i=0;i<NSAMPLES;i++){
        getAvgColor(m,roi[i],avgColor[i]);
        roi[i].draw_rectangle(m->src);
    }
    
    string imgText=string("Finding average color of hand");
    printText(m->src,imgText);
}

void Igcmd::initTrackbars()
{
    for(int i=0;i<NSAMPLES;i++){
        c_lower[i][0]=12;
        c_upper[i][0]=7;
        c_lower[i][1]=30;
        c_upper[i][1]=40;
        c_lower[i][2]=80;
        c_upper[i][2]=80;
    }
}

void Igcmd::normalizeColors(MyImage * myImage)
{
    // copy all boundries read from trackbar
    // to all of the different boundries
    for(int i=1;i<NSAMPLES;i++){
        for(int j=0;j<3;j++){
            c_lower[i][j]=c_lower[0][j];
            c_upper[i][j]=c_upper[0][j];
        }
    }
    // normalize all boundries so that
    // threshold is whithin 0-255
    for(int i=0;i<NSAMPLES;i++){
        if((avgColor[i][0]-c_lower[i][0]) <0){
            c_lower[i][0] = avgColor[i][0] ;
        }if((avgColor[i][1]-c_lower[i][1]) <0){
            c_lower[i][1] = avgColor[i][1] ;
        }if((avgColor[i][2]-c_lower[i][2]) <0){
            c_lower[i][2] = avgColor[i][2] ;
        }if((avgColor[i][0]+c_upper[i][0]) >255){
            c_upper[i][0] = 255-avgColor[i][0] ;
        }if((avgColor[i][1]+c_upper[i][1]) >255){
            c_upper[i][1] = 255-avgColor[i][1] ;
        }if((avgColor[i][2]+c_upper[i][2]) >255){
            c_upper[i][2] = 255-avgColor[i][2] ;
        }
    }
}

void Igcmd::produceBinaries(MyImage *m)
{
    Scalar lowerBound;
    Scalar upperBound;
    Mat foo;
    for(int i=0;i<NSAMPLES;i++){
        normalizeColors(m);
        lowerBound=Scalar( avgColor[i][0] - c_lower[i][0] , avgColor[i][1] - c_lower[i][1], avgColor[i][2] - c_lower[i][2] );
        upperBound=Scalar( avgColor[i][0] + c_upper[i][0] , avgColor[i][1] + c_upper[i][1], avgColor[i][2] + c_upper[i][2] );
        m->bwList.push_back(Mat(m->srcLR.rows,m->srcLR.cols,CV_8U));
        inRange(m->srcLR,lowerBound,upperBound,m->bwList[i]);
    }
    m->bwList[0].copyTo(m->bw);
    for(int i=1;i<NSAMPLES;i++){
        m->bw+=m->bwList[i];
    }
    medianBlur(m->bw, m->bw,7);
}

void Igcmd::showWindows(MyImage* m)
{
    pyrDown(m->bw,m->bw);
    pyrDown(m->bw,m->bw);
    Rect roi( Point( 3*m->src.cols/4,0 ), m->bw.size());
    vector<Mat> channels;
    Mat result;
    for(int i=0;i<3;i++)
        channels.push_back(m->bw);
    merge(channels,result);
    result.copyTo( m->src(roi));
}

int Igcmd::findBiggestContour(vector<vector<Point> > contours)
{
    int indexOfBiggestContour = -1;
    int sizeOfBiggestContour = 0;
    for (int i = 0; i < contours.size(); i++){
        if(contours[i].size() > sizeOfBiggestContour){
            sizeOfBiggestContour = contours[i].size();
            indexOfBiggestContour = i;
        }
    }
    return indexOfBiggestContour;
}

void Igcmd::myDrawContours(MyImage *m,HandGesture *hg)
{
    drawContours(m->src,hg->hullP,hg->cIdx,cv::Scalar(200,0,0),2, 8, vector<Vec4i>(), 0, Point());
    
    rectangle(m->src,hg->bRect.tl(),hg->bRect.br(),Scalar(0,0,200));
    vector<Vec4i>::iterator d=hg->defects[hg->cIdx].begin();
    
    
    vector<Mat> channels;
    Mat result;
    for(int i=0;i<3;i++)
        channels.push_back(m->bw);
    merge(channels,result);
    //	drawContours(result,hg->contours,hg->cIdx,cv::Scalar(0,200,0),6, 8, vector<Vec4i>(), 0, Point());
    drawContours(result,hg->hullP,hg->cIdx,cv::Scalar(0,0,250),10, 8, vector<Vec4i>(), 0, Point());
    
    
    while( d!=hg->defects[hg->cIdx].end() ) {
   	    Vec4i& v=(*d);
        int startidx=v[0]; Point ptStart(hg->contours[hg->cIdx][startidx] );
        int endidx=v[1]; Point ptEnd(hg->contours[hg->cIdx][endidx] );
        int faridx=v[2]; Point ptFar(hg->contours[hg->cIdx][faridx] );
        float depth = v[3] / 256;
        /*
         line( m->src, ptStart, ptFar, Scalar(0,255,0), 1 );
         line( m->src, ptEnd, ptFar, Scalar(0,255,0), 1 );
         circle( m->src, ptFar,   4, Scalar(0,255,0), 2 );
         circle( m->src, ptEnd,   4, Scalar(0,0,255), 2 );
         circle( m->src, ptStart,   4, Scalar(255,0,0), 2 );
         */
        circle( result, ptFar,   9, Scalar(0,205,0), 5 );
        
        
        d++;
        
    }
}

void Igcmd::makeContours(MyImage *m, HandGesture* hg)
{
    Mat aBw;
    pyrUp(m->bw,m->bw);
    m->bw.copyTo(aBw);
    findContours(aBw,hg->contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    hg->initVectors();
    hg->cIdx=findBiggestContour(hg->contours);
    if(hg->cIdx!=-1)
    {
        hg->bRect=boundingRect(Mat(hg->contours[hg->cIdx]));
        convexHull(Mat(hg->contours[hg->cIdx]),hg->hullP[hg->cIdx],false,true);
        convexHull(Mat(hg->contours[hg->cIdx]),hg->hullI[hg->cIdx],false,false);
        approxPolyDP( Mat(hg->hullP[hg->cIdx]), hg->hullP[hg->cIdx], 18, true );
        if(hg->contours[hg->cIdx].size()>3 )
        {
            convexityDefects(hg->contours[hg->cIdx],hg->hullI[hg->cIdx],hg->defects[hg->cIdx]);
            hg->eleminateDefects(m);
        }
        hg->getFingerTips(m);
        bool isHand=hg->detectIfHand();
        hg->printGestureInfo(m->src);
        if(isHand)
        {
            hg->drawFingerTips(m);
            myDrawContours(m,hg);
        }
    }
}

void Igcmd::prepareTraining(MyImage* m)
{
    int square_len = m->src.rows / 20;
    roi.push_back(My_ROI(Point(m->src.cols/2, m->src.rows/4),
                         Point(m->src.cols/2+square_len,m->src.rows/4+square_len),
                         m->srcLR));
    roi.push_back(My_ROI(Point(m->src.cols*5/12, m->src.rows*5/12),
                         Point(m->src.cols*5/12+square_len,m->src.rows*5/12+square_len),
                         m->srcLR));
    roi.push_back(My_ROI(Point(m->src.cols*7/12, m->src.rows*5/12),
                         Point(m->src.cols*7/12+square_len,m->src.rows*5/12+square_len),
                         m->srcLR));
    roi.push_back(My_ROI(Point(m->src.cols/2, m->src.rows*7/12),
                         Point(m->src.cols/2+square_len,m->src.rows*7/12+square_len),
                         m->srcLR));
    roi.push_back(My_ROI(Point(m->src.cols/1.5, m->src.rows*7/12),
                         Point(m->src.cols/1.5+square_len,m->src.rows*7/12+square_len),
                         m->srcLR));
    roi.push_back(My_ROI(Point(m->src.cols*4/9, m->src.rows*3/4),
                         Point(m->src.cols*4/9+square_len,m->src.rows*3/4+square_len),
                         m->srcLR));
    roi.push_back(My_ROI(Point(m->src.cols*5/9, m->src.rows*3/4),
                         Point(m->src.cols*5/9+square_len,m->src.rows*3/4+square_len),
                         m->srcLR));
}

void Igcmd::reset()
{
    for (int i = 0; i < NSAMPLES; i ++)
    {
        for (int j = 0; j < 3; j ++)
        {
            avgColor[i][j] = 0;
            c_lower[i][j] = 0;
            c_upper[i][j] = 0;
        }
    }
    
    roi.clear();
}
