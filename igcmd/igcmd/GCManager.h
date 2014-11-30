//
//  GCManager.h
//  igcmd
//
//  Created by Yang Zhang on 11/16/14.
//  Copyright (c) 2014 Yang Zhang. All rights reserved.
//

#ifndef _GESTURE_COMMAND_MANAGER_H_
#define _GESTURE_COMMAND_MANAGER_H_

#include "GestureCommandList.h"
#include <opencv2/core/core.hpp>

class GestureCommandBase;
class HandGesture;

class GCManager
{
public:
    GCManager();
    ~GCManager();
    
    void start(HandGesture *hg);
    void stop();
    
    void startControllingCommands(HandGesture* hg);
    void stopControllingCommands();
    
    void update(HandGesture* hg, float fTimeTick);
    void execute(cv::Mat src, HandGesture* hg);
    
    static GCManager* getInstance();
    static void releaseInstance();
    
private:
    GestureCommandBase* createGestures(int type);
    
private:
    GestureCommandBase* m_gestures[E_GC_NR];
    
    static GCManager* m_instance;
};

#endif /* defined(_GESTURE_COMMAND_MANAGER_H_) */
