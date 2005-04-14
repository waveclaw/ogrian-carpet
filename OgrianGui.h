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
OgrianGui.h
Original Author: Mike Prosser
Additional Authors: 

Description: An interface to the CEGUI system

/*------------------------------------*/


#ifndef __OgrianGui_H__
#define __OgrianGui_H__

#include <CEGUI/CEGUIImageset.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUILogger.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/elements/CEGUICombobox.h>
#include <CEGUI/elements/CEGUIListbox.h>
#include <CEGUI/elements/CEGUIListboxTextItem.h>
#include <CEGUI/elements/CEGUIPushButton.h>
#include <CEGUI/elements/CEGUIScrollbar.h>
#include <CEGUI/elements/CEGUIStaticImage.h>
#include "OgreCEGUIRenderer.h"
#include "OgreCEGUIResourceProvider.h"

#include <Ogre.h>
#include <OgreSingleton.h>

#include "OgrianGuiFrameListener.h"


using namespace Ogre;

namespace Ogrian
{

class Gui : public Singleton< Gui >
{
public:
	virtual ~Gui();
    static Gui& getSingleton(void);

	void createFrameListener(Root* root, RenderWindow* window, Camera* camera);

	void createScene(Root* root, RenderWindow* window, Camera* camera);

	void setupEventHandlers(void);
	void setupEnterExitEvents(CEGUI::Window* win);
	void setupLoadedLayoutHandlers(void);
	CEGUI::Window* createRttGuiObject(void);
	CEGUI::Window* createStaticImageObject(void);
	bool handleQuit(const CEGUI::EventArgs& e);
	bool handleNew(const CEGUI::EventArgs& e);
	bool handleLoad(const CEGUI::EventArgs& e);
	bool handleObjectSelection(const CEGUI::EventArgs& e);
	bool handleColourChanged(const CEGUI::EventArgs& e);
	bool handleAdd(const CEGUI::EventArgs& e);
	bool handleMouseEnters(const CEGUI::EventArgs& e);
	bool handleMouseLeaves(const CEGUI::EventArgs& e);
	CEGUI::MouseButton convertOgreButtonToCegui(int buttonID);

private:
	Gui();

	GuiFrameListener* mFrameListener;
	Root* mRoot;
	SceneManager* mSceneMgr;
	RenderWindow* mWindow;

    CEGUI::OgreCEGUIRenderer* mGUIRenderer;
    CEGUI::System* mGUISystem;
    CEGUI::Window* mEditorGuiSheet;
	CEGUI::Scrollbar* mRed;
	CEGUI::Scrollbar* mGreen;
	CEGUI::Scrollbar* mBlue;
	CEGUI::StaticImage* mPreview;
	CEGUI::Window* mTip;
	CEGUI::Listbox* mList;
	CEGUI::Window* mEditBox;
	typedef std::map<CEGUI::String, CEGUI::String> DescriptionMap;
	DescriptionMap mDescriptionMap;
};

}

#endif