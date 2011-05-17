//|||||||||||||||||||||||||||||||||||||||||||||||

#include "App.hpp"

#include <OGRE/OgreLight.h>
#include <OGRE/OgreWindowEventUtilities.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

App::App()
{
    m_pCubeNode     = 0;
    m_pCubeEntity       = 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

App::~App()
{
    delete OgreFramework::getSingletonPtr();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void App::startDemo()
{
    new OgreFramework();
    if(!OgreFramework::getSingletonPtr()->initOgre("Open Uber Shader", this, 0))
        return;
    
    m_bShutdown = false;

    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Application initialized!");

    setupDemoScene();
    runDemo();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void App::setupDemoScene()
{
    OgreFramework::getSingletonPtr()->m_pSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");

    OgreFramework::getSingletonPtr()->m_pSceneMgr->createLight("Light1")->setPosition(75,75,75);
	OgreFramework::getSingletonPtr()->m_pSceneMgr->createLight("Light2")->setPosition(75, 75, -75);

    m_pCubeEntity = OgreFramework::getSingletonPtr()->m_pSceneMgr->createEntity("Cube", "athene.mesh");
    m_pCubeNode = OgreFramework::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode("CubeNode");
    m_pCubeNode->attachObject(m_pCubeEntity);
   
  
    Ogre::MaterialPtr simpleMat = Ogre::MaterialManager::getSingletonPtr()->getByName("Simple");
    m_pCubeEntity->setMaterial(simpleMat);
    OgreFramework::getSingletonPtr()->setUpGUI();
    OgreFramework::getSingletonPtr()->setUpShaderParams();
  
}


//|||||||||||||||||||||||||||||||||||||||||||||||

void App::runDemo()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Start main loop...");
    
    double timeSinceLastFrame = 0;
    double startTime = 0;

    OgreFramework::getSingletonPtr()->m_pRenderWnd->resetStatistics();
    
    while(!m_bShutdown && !OgreFramework::getSingletonPtr()->isOgreToBeShutDown()) 
    {
        if(OgreFramework::getSingletonPtr()->m_pRenderWnd->isClosed())m_bShutdown = true;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_LINUX
        Ogre::WindowEventUtilities::messagePump();
#endif  
        if(OgreFramework::getSingletonPtr()->m_pRenderWnd->isActive())
        {
            startTime = OgreFramework::getSingletonPtr()->m_pTimer->getMillisecondsCPU();
                    
            OgreFramework::getSingletonPtr()->m_pKeyboard->capture();
            OgreFramework::getSingletonPtr()->m_pMouse->capture();

            OgreFramework::getSingletonPtr()->updateOgre(timeSinceLastFrame);
            OgreFramework::getSingletonPtr()->m_pRoot->renderOneFrame();
        
            timeSinceLastFrame = OgreFramework::getSingletonPtr()->m_pTimer->getMillisecondsCPU() - startTime;
        }
        else
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
        }
    }

    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Main loop quit");
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Shutdown OGRE...");
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool App::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
    
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_F))
    {
         //do something
    }

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool App::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);
    
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||
