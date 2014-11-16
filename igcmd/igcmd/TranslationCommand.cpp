//
//  TranslationCommand.cpp
//  igcmd
//
//  Created by Yang Zhang on 11/10/14.
//  Copyright (c) 2014 Yang Zhang. All rights reserved.
//

#include "TranslationCommand.h"

using namespace cv;
using namespace std;

#define TRANSLATION_COMMAND_DECISION_FRAME_NR   (4)
#define TRANSLATION_COMMAND_DIS_EPSILON         (15.f)

TranslationCommand::TranslationCommand()
{
    m_poiInit = Point(0,0);
    m_iCommand = E_TC_NR;
}

Point center(Rect r){
    return Point(r.x + r.width / 2, r.y + r.height / 2);
}

void TranslationCommand::start(HandGesture* hg)
{
    GestureCommandBase::start(hg);
    
    m_poiInit = center(hg->bRect);
    insertPos(m_poiInit);
}

Point TranslationCommand::average(){
    
    Point ave = Point(0,0);
    
    for (PosList::const_iterator citer = m_lPos.begin(); citer != m_lPos.end(); citer ++)
    {
        ave += *citer;
    }
    
    int size = m_lPos.size();
    
    return Point(ave.x / size, ave.y / size);
}

void TranslationCommand::update(HandGesture* hg, float fTimeTick)
{
    insertPos(center(hg->bRect));
    
    Point pos = average();
    
    m_iCommand = E_TC_NR;
    
    Point dis = pos - m_poiInit;
    
    if (dis.x > TRANSLATION_COMMAND_DIS_EPSILON)
        m_iCommand |= E_TC_RIGHT;
    else if (pos.x < -TRANSLATION_COMMAND_DIS_EPSILON)
        m_iCommand |= E_TC_LEFT;
    
    if (dis.y > TRANSLATION_COMMAND_DIS_EPSILON)
        m_iCommand |= E_TC_UP;
    else if (dis.y < -TRANSLATION_COMMAND_DIS_EPSILON)
        m_iCommand |= E_TC_DOWN;
    
}

void TranslationCommand::insertPos(cv::Point pos)
{
    if (m_lPos.size() >= TRANSLATION_COMMAND_DECISION_FRAME_NR)
    {
        m_lPos.pop_front();
        m_lPos.push_back(pos);
    }
    else
        m_lPos.push_back(pos);
}

void TranslationCommand::execute()
{
    
}

void TranslationCommand::debugExecute(Mat src)
{
    Point ave = average();
    char output[64];
    sprintf(output, "Command %d; pos : (%d, %d); init pos : (%d, %d)", m_iCommand, ave.x, ave.y, m_poiInit.x, m_poiInit.y);
    putText(src, output, Point(src.cols / 4, src.rows / 10), FONT_HERSHEY_PLAIN, 1.5, Scalar(0,0,200),2);
}




