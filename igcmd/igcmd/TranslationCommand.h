//
//  TranslationCommand.h
//  igcmd
//
//  Created by Yang Zhang on 11/10/14.
//  Copyright (c) 2014 Yang Zhang. All rights reserved.
//

#ifndef _TRANSLATION_COMMAND_H_
#define _TRANSLATION_COMMAND_H_

#include "GestureCommandBase.h"


enum E_TRANSLATION_COMMAND{
    E_TC_UP                 = 0x1,
    E_TC_DOWN               = 0x2,
    E_TC_LEFT               = 0x4,
    E_TC_RIGHT              = 0x8,
    
    E_TC_NR                 = 0x0
};

class TranslationCommand : public GestureCommandBase
{
public:
    typedef std::list<cv::Point> PosList;
    
    TranslationCommand();
    ~TranslationCommand(){}
    
    void start(HandGesture* hg);
    
    void update(HandGesture* hg, float fTimeTick);
    void execute();
    
    void debugExecute(Mat src);
    
    int getId(){
        return E_GC_TRANSITION;
    }
    
private:
    void insertPos(cv::Point pos);
    cv::Point average();
    
private:
    PosList m_lPos;
    
    cv::Point m_poiInit;
    
    int m_iCommand;
};

#endif /* defined(_TRANSLATION_COMMAND_H_) */
