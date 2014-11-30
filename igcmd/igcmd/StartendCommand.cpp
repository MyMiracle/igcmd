//
//  TranslationCommand.cpp
//  igcmd
//
//  Created by Yang Zhang on 11/10/14.
//  Copyright (c) 2014 Yang Zhang. All rights reserved.
//

#include "StartendCommand.h"
#include "GCManager.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace cv;
using namespace std;


#define STARTEND_COMMAND_DECISION_FRAME_NR   (4)
#define STARTEND_COMMAND_DIS_EPSILON         (15.f)

StartendCommand::StartendCommand()
{
    m_iCommand = E_SC_NR;
    m_bControlling = false;
}


void StartendCommand::update(HandGesture* hg, float fTimeTick)
{
    if (!m_bStarted)
        return;
    
    num = hg->numoffinger;
   
    m_iCommand = E_SC_NR;
    if (num == 2)
        m_iCommand = E_SC_START;
    if (num == 3)
        m_iCommand = E_SC_END;
}

void StartendCommand::execute(HandGesture* hg)
{
    if (!m_bStarted)
    return;
    
    if (m_iCommand == E_SC_START)
    {
        m_bControlling = true;
        GCManager::getInstance()->startControllingCommands(hg);
    }
    else if (m_iCommand == E_SC_END)
    {
        m_bControlling = false;
        GCManager::getInstance()->stopControllingCommands();
    }
}

void StartendCommand::debugExecute(Mat src)
{
    if (!m_bStarted)
    return;
    
    char output[64];
    sprintf(output, "%d Command %d",num, m_iCommand);
    putText(src, output, Point(src.cols / 4, src.rows / 2), FONT_HERSHEY_PLAIN, 1.5, Scalar(0,0,200),2);
}



