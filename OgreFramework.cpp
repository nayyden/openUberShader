//|||||||||||||||||||||||||||||||||||||||||||||||

#include "OgreFramework.hpp"

//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;

//|||||||||||||||||||||||||||||||||||||||||||||||

template<> OgreFramework* Ogre::Singleton<OgreFramework>::ms_Singleton = 0;

//|||||||||||||||||||||||||||||||||||||||||||||||

OgreFramework::OgreFramework()
{
    m_MoveSpeed			= 0.1f;
    m_RotateSpeed		= 0.3f;

    m_bShutDownOgre		= false;
    m_iNumScreenShots		= 0;

    m_pRoot			= 0;
    m_pSceneMgr			= 0;
    m_pRenderWnd		= 0;
    m_pCamera			= 0;
    m_pViewport			= 0;
    m_pLog			= 0;
    m_pTimer			= 0;

    m_pInputMgr			= 0;
    m_pKeyboard			= 0;
    m_pMouse			= 0;

    m_pTrayMgr          	= 0;
    m_FrameEvent        	= Ogre::FrameEvent();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFramework::initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener)
{
    Ogre::LogManager* logMgr = new Ogre::LogManager();

    m_pLog = Ogre::LogManager::getSingleton().createLog("OgreLogfile.log", true, true, false);
    m_pLog->setDebugOutputEnabled(true);

    m_pRoot = new Ogre::Root("");
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    m_pRoot->loadPlugin("./RenderSystem_GL.dll");
#else
    m_pRoot->loadPlugin("/usr/lib/OGRE/RenderSystem_GL.so");
#endif

    if(!m_pRoot->showConfigDialog())
        return false;
    m_pRenderWnd = m_pRoot->initialise(true, wndTitle);

    m_pSceneMgr = m_pRoot->createSceneManager(ST_GENERIC, "SceneManager");
    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    m_pCamera = m_pSceneMgr->createCamera("Camera");
    m_pCamera->setPosition(Vector3(0, 60, 60));
    m_pCamera->lookAt(Vector3(0, 0, 0));
    m_pCamera->setNearClipDistance(1);

    m_pViewport = m_pRenderWnd->addViewport(m_pCamera);
    m_pViewport->setBackgroundColour(ColourValue(0.8f, 0.7f, 0.6f, 1.0f));

    m_pCamera->setAspectRatio(Real(m_pViewport->getActualWidth()) / Real(m_pViewport->getActualHeight()));

    m_pViewport->setCamera(m_pCamera);

    unsigned long hWnd = 0;
    OIS::ParamList paramList;
    m_pRenderWnd->getCustomAttribute("WINDOW", &hWnd);

    paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

    m_pInputMgr = OIS::InputManager::createInputSystem(paramList);

    m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputMgr->createInputObject(OIS::OISKeyboard, true));
    m_pMouse = static_cast<OIS::Mouse*>(m_pInputMgr->createInputObject(OIS::OISMouse, true));

    m_pMouse->getMouseState().height = m_pRenderWnd->getHeight();
    m_pMouse->getMouseState().width	 = m_pRenderWnd->getWidth();

    if(pKeyListener == 0)
        m_pKeyboard->setEventCallback(this);
    else
        m_pKeyboard->setEventCallback(pKeyListener);

    if(pMouseListener == 0)
        m_pMouse->setEventCallback(this);
    else
        m_pMouse->setEventCallback(pMouseListener);

    Ogre::String secName, typeName, archName;
    Ogre::ConfigFile cf;
    cf.load("resources.cfg");

    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    m_pTimer = new Ogre::Timer();
    m_pTimer->reset();

    m_pTrayMgr = new OgreBites::SdkTrayManager("TrayMgr", m_pRenderWnd, m_pMouse, this);
    m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    
    m_pTrayMgr->hideTrays();
    this->guiVisible = false;
    //m_pTrayMgr->hideCursor();

    m_pRenderWnd->setActive(true);
    
    cameraManager	= new OgreBites::SdkCameraMan(m_pCamera);  
    cameraCenter 	= m_pSceneMgr->getRootSceneNode()->createChildSceneNode("cameraCenter");
    cameraCenter ->setPosition(Ogre::Vector3(0,0,0));
    cameraManager->setTarget(cameraCenter);
    cameraManager->setStyle(OgreBites::CS_ORBIT);
    
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void OgreFramework::setUpGUI()
{
    this->currentLight = "Light1";
    Ogre::StringVector strVec;
    strVec.push_back("Light1");
    strVec.push_back("Light2");
    OgreBites::SelectMenu *menu = m_pTrayMgr->createLongSelectMenu(OgreBites::TL_TOPRIGHT, "Lights", "Lights", 200, 100, 2, strVec);
    menu->selectItem(0, false);
    enableLight =  m_pTrayMgr->createCheckBox(OgreBites::TL_TOPRIGHT, "EnableLight", "Enable");
    enableLight->setChecked(true, false);
    enableLight->hide();

    lightPosx = m_pTrayMgr->createLongSlider(OgreBites::TL_TOPRIGHT, "LightX", "LightX", 100, 60, -400.0f, 400.0f, 256);
    lightPosy = m_pTrayMgr->createLongSlider(OgreBites::TL_TOPRIGHT, "LightY", "LightY", 100, 60, -400.0f, 400.0f, 256);
    lightPosz =  m_pTrayMgr->createLongSlider(OgreBites::TL_TOPRIGHT, "LightZ", "LightZ", 100, 60, -400.0f, 400.0f, 256);
    m_pTrayMgr->createCheckBox(OgreBites::TL_BOTTOMRIGHT, "MatSwitch", "TurnBumpMap");
    m_pTrayMgr->createCheckBox(OgreBites::TL_BOTTOMRIGHT, "MultiLights", "Multiple Lights")->setChecked(true, false);
    
    
    
    
   
}

//|||||||||||||||||||||||||||||||||||||||||||||||

OgreFramework::~OgreFramework()
{
    if(m_pInputMgr) OIS::InputManager::destroyInputSystem(m_pInputMgr);
    if(m_pTrayMgr)  delete m_pTrayMgr;
    if(m_pRoot)     delete m_pRoot;
    if(cameraManager) delete cameraManager;
   
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFramework::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if(m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        m_bShutDownOgre = true;
        return true;
    }

    if(m_pKeyboard->isKeyDown(OIS::KC_SYSRQ))
    {
        m_pRenderWnd->writeContentsToTimestampedFile("BOF_Screenshot_", ".jpg");
        return true;
    }

    if(m_pKeyboard->isKeyDown(OIS::KC_M))
    {
        static int mode = 0;

        if(mode == 2)
        {
            m_pCamera->setPolygonMode(PM_SOLID);
            mode = 0;
        }
        else if(mode == 0)
        {
            m_pCamera->setPolygonMode(PM_WIREFRAME);
            mode = 1;
        }
        else if(mode == 1)
        {
            m_pCamera->setPolygonMode(PM_POINTS);
            mode = 2;
        }
    }

    if(m_pKeyboard->isKeyDown(OIS::KC_O))
    {
        if(m_pTrayMgr->isLogoVisible())
        {
            m_pTrayMgr->hideLogo();
            m_pTrayMgr->hideFrameStats();
        }
        else
        {
            m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
            m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
        }
    }
    
    if(m_pKeyboard->isKeyDown(OIS::KC_P)) {
        if(this->guiVisible) {
            m_pTrayMgr->hideTrays();
            this->guiVisible = false;
        } else {
            m_pTrayMgr->showTrays();
            this->guiVisible = true;
        }
        
    }

    return true;
}


//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFramework::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFramework::mouseMoved(const OIS::MouseEvent &evt)
{
    /* For the GUI ? */
    if(m_pTrayMgr->injectMouseMove(evt)) 
        return true;
   
    /* If not for the GUI */
    cameraManager->injectMouseMove(evt);
    return true;   
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFramework::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    /* For the GUI ? */
    if(m_pTrayMgr->injectMouseDown(evt, id))
        return true;
    
    /* If not for the GUI */
    cameraManager->injectMouseDown(evt, id);
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFramework::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    /* For the GUI ? */
    if(m_pTrayMgr->injectMouseUp(evt, id))
        return true;
    
    /* If not for the GUI */
    cameraManager->injectMouseUp(evt, id);
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void OgreFramework::updateOgre(double timeSinceLastFrame)
{
    m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    m_pTrayMgr->frameRenderingQueued(m_FrameEvent);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void OgreFramework::setUpShaderParams()
{
    
   
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName("Bump_Lambert");
    vertexShaderParams = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();
    fragmentShaderParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void OgreFramework::sliderMoved(OgreBites::Slider* slider)
{
    if(slider->getName() == "LightX") {  
       float xPos = slider->getValue();
       Ogre::Vector3 lightPos = this->m_pSceneMgr->getLight(currentLight)->getPosition();
       this->m_pSceneMgr->getLight(currentLight)->setPosition(xPos, lightPos.y, lightPos.z);
    }
    if(slider->getName() == "LightY") {  
       float yPos = slider->getValue();
       Ogre::Vector3 lightPos = this->m_pSceneMgr->getLight(currentLight)->getPosition();
       this->m_pSceneMgr->getLight(currentLight)->setPosition(lightPos.x, yPos, lightPos.z);
    }
    if(slider->getName() == "LightZ") {  
       float zPos = slider->getValue();
       Ogre::Vector3 lightPos = this->m_pSceneMgr->getLight(currentLight)->getPosition();
       this->m_pSceneMgr->getLight(currentLight)->setPosition(lightPos.x, lightPos.y, zPos);
    }
   
}


void OgreFramework::checkBoxToggled(OgreBites::CheckBox *checkBox)
{
    if(checkBox->getName() == "MatSwitch") {
        if(checkBox->isChecked()) {
          Ogre::MaterialPtr bumpMat = Ogre::MaterialManager::getSingletonPtr()->getByName("Bump_Lambert");
          Ogre::Entity *ent =  this->m_pSceneMgr->getEntity("Cube");
          ent->setMaterial(bumpMat);
        } else if(!checkBox->isChecked()) {
            Ogre::MaterialPtr simpleMat = Ogre::MaterialManager::getSingletonPtr()->getByName("Simple");
            Ogre::Entity *ent =  this->m_pSceneMgr->getEntity("Cube");           
            ent->setMaterial(simpleMat);
        }
       
    }
    
    if(checkBox->getName() == "EnableLight") {
        Ogre::Light *l = this->m_pSceneMgr->getLight(currentLight); 
           if(checkBox->isChecked()) {
               l->setVisible(true);
               return;
           } else {
               l->setVisible(false);
               return;
           }
    }
    
    if(checkBox->getName() == "MultiLights") {
        Ogre::Light *light =  this->m_pSceneMgr->getLight("Light1");
        if(checkBox->isChecked()) {
            if(!light->isVisible())
                light->setVisible(true);
            updateLightVis();
            this->vertexShaderParams->setNamedConstant("multiLight", 1);
            this->fragmentShaderParams->setNamedConstant("multiLight", 1);
        } else {
           if(light->isVisible())
                light->setVisible(false);
            updateLightVis();
            this->vertexShaderParams->setNamedConstant("multiLight", 0);
            this->fragmentShaderParams->setNamedConstant("multiLight", 0);
        }
    }
}

void OgreFramework::updateLightVis()
{
    Ogre::Light *light =  this->m_pSceneMgr->getLight(currentLight);
    if(light->isVisible()) {
        enableLight->setChecked(true, false);
        return;
    } else {
        enableLight->setChecked(false, false);
        return;
    }
    
}


void OgreFramework::updateLightPos()
{
    Ogre::Light *light =  this->m_pSceneMgr->getLight(currentLight);
  
    
    Ogre::Vector3 lightPos = light->getPosition();
    lightPosx->setValue(lightPos.x);
    lightPosy->setValue(lightPos.y);
    lightPosz->setValue(lightPos.z);   
}

void OgreFramework::itemSelected(OgreBites::SelectMenu *menu)
{
   int choice =  menu->getSelectionIndex();
   if(choice == 0) {
       this->currentLight = "Light1";
       updateLightPos();
       updateLightVis();
       return;
   } else if(choice == 1) {
       this->currentLight = "Light2";
       updateLightPos();
       updateLightVis();
   }
   
   return ;
}

