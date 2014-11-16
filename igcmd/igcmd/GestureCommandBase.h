//
//  GestureCommandBase.h
//  igcmd
//
//  Created by Yang Zhang on 11/16/14.
//  Copyright (c) 2014 Yang Zhang. All rights reserved.
//

#ifndef _GESTURE_COMMAND_BASE_H_
#define _GESTURE_COMMAND_BASE_H_

#include "GestureCommandList.h"
#include "handGesture.hpp"



class GestureCommandBase{
public:
    GestureCommandBase(){}
    virtual ~GestureCommandBase(){}
    
    virtual void start(HandGesture* hg){
        m_bStarted = true;
    }
    
    virtual void stop(){
        m_bStarted = false;
    }
    
    virtual void update(HandGesture* hg, float fTimeTick) = 0;
    virtual void execute() = 0;
    
    
    // used only for debuging
    // print out result
    virtual void debugExecute(Mat src){}
    
    virtual int getId(){
        return E_GC_NR;
    }
    
protected:
    bool m_bStarted;
};

#endif /* defined(_GESTURE_COMMAND_BASE_H_) */
