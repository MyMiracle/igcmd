//
//  GCManager.cpp
//  igcmd
//
//  Created by Yang Zhang on 11/16/14.
//  Copyright (c) 2014 Yang Zhang. All rights reserved.
//

#include "GCManager.h"
#include "TranslationCommand.h"
#include "StartendCommand.h"
#include "handGesture.hpp"


GCManager* GCManager::m_instance = NULL;


GCManager* GCManager::getInstance()
{
    if (m_instance == NULL)
    {
        m_instance = new GCManager();
    }
    
    return m_instance;
}

void GCManager::releaseInstance()
{
    if (m_instance)
    {
        delete m_instance;
        m_instance = NULL;
    }
}

GCManager::GCManager()
{
    GestureCommandBase* gestureCommand = NULL;
    for (int i = 0; i < E_GC_NR; i ++)
    {
        gestureCommand = createGestures(i);
        assert(gestureCommand);
        m_gestures[i] = gestureCommand;
    }
}

GCManager::~GCManager()
{
    for (int i = 0; i < E_GC_NR; i ++)
    {
        delete m_gestures[i];
    }
}


void GCManager::start(HandGesture *hg)
{
    // TO-DO
    // to be modified when more commands are added
//    m_gestures[E_GC_TRANSITION]->start(hg);
    m_gestures[E_GC_STARTEND]->start(hg);
}

void GCManager::stop()
{
    for (int i = 0; i < E_GC_NR; i ++)
    {
        m_gestures[i]->stop();
    }
}

void GCManager::startControllingCommands(HandGesture* hg)
{
    for (int i = E_GC_STARTEND + 1; i < E_GC_NR; i ++)
    {
        m_gestures[i]->start(hg);
    }
}

void GCManager::stopControllingCommands()
{
    for (int i = E_GC_STARTEND + 1; i < E_GC_NR; i ++)
    {
        m_gestures[i]->stop();
    }
}


void GCManager::update(HandGesture* hg, float fTimeTick)
{
    for (int i = 0; i < E_GC_NR; i ++)
    {
        m_gestures[i]->update(hg, fTimeTick);
    }
}

void GCManager::execute(cv::Mat src, HandGesture* hg)
{
    for (int i = 0; i < E_GC_NR; i ++)
    {
        // for debug using
        m_gestures[i]->debugExecute(src);
        // should be this after debug finished
        m_gestures[i]->execute(hg);
    }
}



GestureCommandBase* GCManager::createGestures(int type)
{
    GestureCommandBase* reVal = NULL;
    switch (type) {
        case E_GC_TRANSITION:
            reVal = new TranslationCommand();
            break;
            
        case E_GC_STARTEND:
            reVal = new StartendCommand();
            break;
            
        default:
            break;
    }
    
    return reVal;
}
