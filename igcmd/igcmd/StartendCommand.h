//
//  TranslationCommand.h
//  igcmd
//
//  Created by Yang Zhang on 11/10/14.
//  Copyright (c) 2014 Yang Zhang. All rights reserved.
//

#ifndef _STARTEND_COMMAND_H_
#define _STARTEND_COMMAND_H_

#include "GestureCommandBase.h"


enum E_STARTEND_COMMAND{
    E_SC_START,
    E_SC_END,
    
    E_SC_NR
};

class StartendCommand : public GestureCommandBase
{
public:
    typedef std::list<cv::Point> PosList2;

    StartendCommand();
    ~StartendCommand(){}
   
    void update(HandGesture* hg, float fTimeTick);
    void execute(HandGesture* hg);
    Point center(Rect r);
    
    void debugExecute(Mat src);
    
    int getId(){
        return E_GC_STARTEND;
    }

private:
    int m_iCommand;
    int num;
    bool m_bControlling;
};

#endif /* defined(_TRANSLATION_COMMAND_H_) */
