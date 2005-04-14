/*****************************************************************************
	Copyright 2004 Mike Prosser

    This file is part of Ogrian Carpet.

    Ogrian Carpet is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Ogrian Carpet is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ogrian Carpet; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*****************************************************************************/

/*------------------------------------*
OgrianGui.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: an interface to the CEGUI system

/*------------------------------------*/

#include "OgrianGui.h"
#include "OgrianConst.h"
#include "OgreMouseEvent.h"
#include "OgrianGuiFrameListener.h"
#include "OgrianRenderer.h"

using namespace Ogre;

template<> Ogrian::Gui * Singleton< Ogrian::Gui >::ms_Singleton = 0;
namespace Ogrian
{

//----------------------------------------------------------------------------

Gui::Gui()
    : mGUIRenderer(0),
    mGUISystem(0),
    mEditorGuiSheet(0),
	mRoot(0),
	mFrameListener(0),
	mSceneMgr(0),
	mWindow(0)
{
	mDescriptionMap[(CEGUI::utf8*)"Demo8"] = 
		(CEGUI::utf8*)"The main containing panel";
	mDescriptionMap[(CEGUI::utf8*)"Demo8/Window1"] = 
		(CEGUI::utf8*)"A test window";
	mDescriptionMap[(CEGUI::utf8*)"Demo8/Window1/Listbox"] = 
		(CEGUI::utf8*)"A list box";
	mDescriptionMap[(CEGUI::utf8*)"Demo8/Window1/Controls/Red"] = 
		(CEGUI::utf8*)"A colour slider";
	mDescriptionMap[(CEGUI::utf8*)"Demo8/Window1/Controls/Green"] = 
		(CEGUI::utf8*)"A colour slider";
	mDescriptionMap[(CEGUI::utf8*)"Demo8/Window1/Controls/Blue"] = 
		(CEGUI::utf8*)"A colour slider";
	mDescriptionMap[(CEGUI::utf8*)"Demo8/Window1/Controls/ColourSample"] = 
		(CEGUI::utf8*)"The colour that will be used for the selection when added to the list";
	mDescriptionMap[(CEGUI::utf8*)"Demo8/Window1/Controls/Editbox"] = 
		(CEGUI::utf8*)"An edit box; this text will be added to the list";
	mDescriptionMap[(CEGUI::utf8*)"Demo8/Window1/Controls/Add"] = 
		(CEGUI::utf8*)"Adds the text to the list";
	mDescriptionMap[(CEGUI::utf8*)"Demo8/Window1/Controls/ins1"] = 
		(CEGUI::utf8*)"Some static text";
}

//----------------------------------------------------------------------------

Gui::~Gui()
{
    if(mEditorGuiSheet)
    {
        CEGUI::WindowManager::getSingleton().destroyWindow(mEditorGuiSheet);
    }
    if(mGUISystem)
    {
        delete mGUISystem;
        mGUISystem = 0;
    }
    if(mGUIRenderer)
    {
        delete mGUIRenderer;
        mGUIRenderer = 0;
    }
}

//----------------------------------------------------------------------------

// Create new frame listener
void Gui::createFrameListener(Root* root, RenderWindow* window, Camera* camera)
{
    mFrameListener= new GuiFrameListener(window, camera, mGUIRenderer);
    root->addFrameListener(mFrameListener);
}

//----------------------------------------------------------------------------

void Gui::createScene(Root* root, RenderWindow* window, Camera* camera)
{
	mRoot = root;
	mWindow = window;
	mSceneMgr = Renderer::getSingleton().getSceneManager();


        // Create a skydome
        mSceneMgr->setSkyDome(true, "Ogrian/CloudySky", 5, 8);


    // setup GUI system
    mGUIRenderer = new CEGUI::OgreCEGUIRenderer(mWindow, 
        Ogre::RENDER_QUEUE_OVERLAY, false, 3000, mSceneMgr);

    mGUISystem = new CEGUI::System(mGUIRenderer);

    CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);

    // Setup Render To Texture for preview window
    RenderTexture* rttTex = mRoot->getRenderSystem()->createRenderTexture( "RttTex", 512, 512, TEX_TYPE_2D, PF_R8G8B8 );
    {
        Camera* rttCam = mSceneMgr->createCamera("RttCam");
        SceneNode* camNode = 
            mSceneMgr->getRootSceneNode()->createChildSceneNode("rttCamNode");
        camNode->attachObject(rttCam);
        rttCam->setPosition(0,0,200);
        rttCam->setVisible(true);

        Viewport *v = rttTex->addViewport( rttCam );
        v->setOverlaysEnabled(false);
        v->setClearEveryFrame( true );
        v->setBackgroundColour( ColourValue::Black );
    }

    // Retrieve CEGUI texture for the RTT
    CEGUI::Texture* rttTexture = mGUIRenderer->createTexture((CEGUI::utf8*)"RttTex");

    CEGUI::Imageset* rttImageSet = 
        CEGUI::ImagesetManager::getSingleton().createImageset(
                (CEGUI::utf8*)"RttImageset", rttTexture);

    rttImageSet->defineImage((CEGUI::utf8*)"RttImage", 
            CEGUI::Point(0.0f, 0.0f),
            CEGUI::Size(rttTexture->getWidth(), rttTexture->getHeight()),
            CEGUI::Point(0.0f,0.0f));

    // load scheme and set up defaults
    CEGUI::SchemeManager::getSingleton().loadScheme(
            (CEGUI::utf8*)"TaharezLook.scheme");
    mGUISystem->setDefaultMouseCursor(
            (CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
    mGUISystem->setDefaultFont((CEGUI::utf8*)"Tahoma-12");

    CEGUI::Window* sheet = 
        CEGUI::WindowManager::getSingleton().loadWindowLayout(
            (CEGUI::utf8*)"ogregui.layout"); 
    mGUISystem->setGUISheet(sheet);

    CEGUI::Combobox* objectComboBox = (CEGUI::Combobox*)CEGUI::WindowManager::getSingleton().getWindow("OgreGuiDemo/TabCtrl/Page2/ObjectTypeList");

    CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem((CEGUI::utf8*)"FrameWindow", 0);
    objectComboBox->addItem(item);
    item = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Horizontal Scrollbar", 1);
    objectComboBox->addItem(item);
    item = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Vertical Scrollbar", 2);
    objectComboBox->addItem(item);
    item = new CEGUI::ListboxTextItem((CEGUI::utf8*)"StaticText", 3);
    objectComboBox->addItem(item);
    item = new CEGUI::ListboxTextItem((CEGUI::utf8*)"StaticImage", 4);
    objectComboBox->addItem(item);
    item = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Render to Texture", 5);
    objectComboBox->addItem(item);

    setupEventHandlers();
}

//----------------------------------------------------------------------------
 
void Gui::setupEventHandlers(void)
{
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
    wmgr.getWindow((CEGUI::utf8*)"OgreGuiDemo/TabCtrl/Page1/QuitButton")
		->subscribeEvent(
			CEGUI::PushButton::EventClicked, 
			CEGUI::Event::Subscriber(&Gui::handleQuit, this));
    wmgr.getWindow((CEGUI::utf8*)"OgreGuiDemo/TabCtrl/Page1/NewButton")
		->subscribeEvent(
			CEGUI::PushButton::EventClicked, 
			CEGUI::Event::Subscriber(&Gui::handleNew, this));
    wmgr.getWindow((CEGUI::utf8*)"OgreGuiDemo/TabCtrl/Page1/LoadButton")
		->subscribeEvent(
			CEGUI::PushButton::EventClicked, 
			CEGUI::Event::Subscriber(&Gui::handleLoad, this));
    wmgr.getWindow((CEGUI::utf8*)"OgreGuiDemo/TabCtrl/Page2/ObjectTypeList")
		->subscribeEvent(
			CEGUI::Combobox::EventListSelectionAccepted, 
			CEGUI::Event::Subscriber(&Gui::handleObjectSelection, this));

}

//----------------------------------------------------------------------------

void Gui::setupEnterExitEvents(CEGUI::Window* win)
{
	win->subscribeEvent(
		CEGUI::Window::EventMouseEnters, 
		CEGUI::Event::Subscriber(&Gui::handleMouseEnters, this));
	win->subscribeEvent(
		CEGUI::Window::EventMouseLeaves, 
		CEGUI::Event::Subscriber(&Gui::handleMouseLeaves, this));
	for (unsigned int i = 0; i < win->getChildCount(); ++i)
	{
		CEGUI::Window* child = win->getChildAtIdx(i);
		setupEnterExitEvents(child);
	}

}

//----------------------------------------------------------------------------

void Gui::setupLoadedLayoutHandlers(void)
{
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	mRed = static_cast<CEGUI::Scrollbar*>(
		wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls/Red"));
	mGreen = static_cast<CEGUI::Scrollbar*>(
		wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls/Green"));
	mBlue = static_cast<CEGUI::Scrollbar*>(
		wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls/Blue"));
	mPreview = static_cast<CEGUI::StaticImage*>(
		wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls/ColourSample"));
	mList = static_cast<CEGUI::Listbox*>(
		wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Listbox"));
	mEditBox = 
		wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls/Editbox");
	mTip = 
		wmgr.getWindow((CEGUI::utf8*)"Demo8/Window2/Tips");

	mRed->subscribeEvent(
			CEGUI::Scrollbar::EventScrollPositionChanged, 
			CEGUI::Event::Subscriber(&Gui::handleColourChanged, this));
	mGreen->subscribeEvent(
		CEGUI::Scrollbar::EventScrollPositionChanged, 
		CEGUI::Event::Subscriber(&Gui::handleColourChanged, this));
	mBlue->subscribeEvent(
		CEGUI::Scrollbar::EventScrollPositionChanged, 
		CEGUI::Event::Subscriber(&Gui::handleColourChanged, this));

	wmgr.getWindow((CEGUI::utf8*)"Demo8/Window1/Controls/Add")
		->subscribeEvent(
		CEGUI::PushButton::EventClicked, 
		CEGUI::Event::Subscriber(&Gui::handleAdd, this));

	CEGUI::Window* root = wmgr.getWindow("Demo8");
	setupEnterExitEvents(root);
}

//----------------------------------------------------------------------------

CEGUI::Window* Gui::createRttGuiObject(void)
{
    static unsigned int rttCounter = 0;
    String guiObjectName = "NewRttImage" + rttCounter;

    CEGUI::Imageset* rttImageSet = 
        CEGUI::ImagesetManager::getSingleton().getImageset(
            (CEGUI::utf8*)"RttImageset");
    CEGUI::StaticImage* si = (CEGUI::StaticImage*)CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/StaticImage", (CEGUI::utf8*)guiObjectName.c_str());
    si->setSize(CEGUI::Size(0.5f, 0.4f));
    si->setImage(&rttImageSet->getImage((CEGUI::utf8*)"RttImage"));

    rttCounter++;

    return si;
}

//----------------------------------------------------------------------------

CEGUI::Window* Gui::createStaticImageObject(void)
{
    static unsigned int siCounter = 0;
    String guiObjectName = "NewStaticImage" + siCounter;

    CEGUI::Imageset* imageSet = 
        CEGUI::ImagesetManager::getSingleton().getImageset(
            (CEGUI::utf8*)"TaharezLook");

    CEGUI::StaticImage* si = (CEGUI::StaticImage*)CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/StaticImage", (CEGUI::utf8*)guiObjectName.c_str());
    si->setSize(CEGUI::Size(0.2f, 0.2f));
    si->setImage(&imageSet->getImage((CEGUI::utf8*)"ClientBrush"));

    siCounter++;

    return si;
}

//----------------------------------------------------------------------------

bool Gui::handleQuit(const CEGUI::EventArgs& e)
{
    static_cast<GuiFrameListener*>(mFrameListener)->requestShutdown();
    return true;
}

//----------------------------------------------------------------------------

bool Gui::handleNew(const CEGUI::EventArgs& e)
{
    if(mEditorGuiSheet)
        CEGUI::WindowManager::getSingleton().destroyWindow(mEditorGuiSheet);

    mEditorGuiSheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultGUISheet", "NewLayout");

    CEGUI::Window* editorWindow = CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"OgreGuiDemo2/MainWindow");
    editorWindow->addChildWindow(mEditorGuiSheet);

    return true;
}

//----------------------------------------------------------------------------

bool Gui::handleLoad(const CEGUI::EventArgs& e)
{
    if(mEditorGuiSheet)
        CEGUI::WindowManager::getSingleton().destroyWindow(mEditorGuiSheet);

    mEditorGuiSheet = 
        CEGUI::WindowManager::getSingleton().loadWindowLayout(
            (CEGUI::utf8*)"cegui8.layout"); 
	setupLoadedLayoutHandlers();

    CEGUI::Window* editorWindow = CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"OgreGuiDemo2/MainWindow");
    editorWindow->addChildWindow(mEditorGuiSheet);

    return true;
}

//----------------------------------------------------------------------------

bool Gui::handleObjectSelection(const CEGUI::EventArgs& e)
{
    static unsigned int windowNumber = 0;
    static unsigned int vertScrollNumber = 0;
    static unsigned int horizScrollNumber = 0;
    static unsigned int textScrollNumber = 0;
    String guiObjectName;
    CEGUI::Window* window = 0;

    // Set a random position to place this object.
    Real posX = Math::RangeRandom(0.0, 0.7); 
    Real posY = Math::RangeRandom(0.1, 0.7); 

    const CEGUI::WindowEventArgs& windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>(e);
    CEGUI::ListboxItem* item = static_cast<CEGUI::Combobox*>(windowEventArgs.window)->getSelectedItem();

    CEGUI::Window* editorWindow = CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"OgreGuiDemo2/MainWindow");

    switch(item->getID())
    {
    case 0:
        guiObjectName = "NewWindow" + windowNumber;
        window = CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/FrameWindow", (CEGUI::utf8*)guiObjectName.c_str());
        window->setSize(CEGUI::Size(0.3f, 0.3f));
        window->setText((CEGUI::utf8*)"New Window");
        windowNumber++;
        break;
    case 1:
        guiObjectName = "NewHorizScroll" + horizScrollNumber;
        window = CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/HorizontalScrollbar", (CEGUI::utf8*)guiObjectName.c_str());
        window->setSize(CEGUI::Size(0.75f, 0.03f));
        horizScrollNumber++;
        break;
    case 2:
        guiObjectName = "NewVertScroll" + vertScrollNumber;
        window = CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/VerticalScrollbar", (CEGUI::utf8*)guiObjectName.c_str());
        window->setSize(CEGUI::Size(0.03f, 0.75f));
        vertScrollNumber++;
        break;
    case 3:
        guiObjectName = "NewStaticText" + textScrollNumber;
        window = CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/StaticText", (CEGUI::utf8*)guiObjectName.c_str());
        window->setSize(CEGUI::Size(0.25f, 0.1f));
        window->setText((CEGUI::utf8*)"Example static text");
        textScrollNumber++;
        break;
    case 4:
        window = createStaticImageObject();
        break;
    case 5:
        window = createRttGuiObject();
        break;
    };

    editorWindow->addChildWindow(window);
    window->setPosition(CEGUI::Point(posX, posY));

    return true;
}

//----------------------------------------------------------------------------

bool Gui::handleColourChanged(const CEGUI::EventArgs& e)
{
	mPreview->setImageColours(CEGUI::colour(
		mRed->getScrollPosition() / 255.0f,
		mGreen->getScrollPosition() / 255.0f,
		mBlue->getScrollPosition() / 255.0f));

	return true;

}

//----------------------------------------------------------------------------

bool Gui::handleAdd(const CEGUI::EventArgs& e)
{
	CEGUI::ListboxTextItem *listboxitem = 
		new CEGUI::ListboxTextItem (mEditBox->getText());
	listboxitem->setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush");
	listboxitem->setSelected(mList->getItemCount() == 0);
	listboxitem->setSelectionColours(mPreview->getImageColours());
	mList->addItem(listboxitem);
	return true;
}

//----------------------------------------------------------------------------

bool Gui::handleMouseEnters(const CEGUI::EventArgs& e)
{
	CEGUI::WindowEventArgs& we = ((CEGUI::WindowEventArgs&)e);
	DescriptionMap::iterator i = 
		mDescriptionMap.find(we.window->getName());
	if (i != mDescriptionMap.end())
	{
		mTip->setText(i->second);
	}
	else
	{
		mTip->setText((CEGUI::utf8*)"");
	}
	
	return true;
}

//----------------------------------------------------------------------------

bool Gui::handleMouseLeaves(const CEGUI::EventArgs& e)
{
	mTip->setText((CEGUI::utf8*)"");
	return true;
}

//----------------------------------------------------------------------------
 
CEGUI::MouseButton Gui::convertOgreButtonToCegui(int buttonID)
{
    switch (buttonID)
	{
	case MouseEvent::BUTTON0_MASK:
        return CEGUI::LeftButton;
    case MouseEvent::BUTTON1_MASK:
        return CEGUI::RightButton;
    case MouseEvent::BUTTON2_MASK:
        return CEGUI::MiddleButton;
    case MouseEvent::BUTTON3_MASK:
        return CEGUI::X1Button;
    default:
        return CEGUI::LeftButton;
    }
}
//----------------------------------------------------------------------------

Gui& Gui::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Gui();
	}
    return Singleton<Gui>::getSingleton();
}

//----------------------------------------------------------------------------

}