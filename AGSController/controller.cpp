////*

#ifdef WIN32
#define WINDOWS_VERSION
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma warning(disable : 4244)
#endif

#if !defined(BUILTIN_PLUGINS)
#define THIS_IS_THE_PLUGIN
#endif

#include <stdio.h>
#include <SDL.h>
#include <SDL_joystick.h>


#if defined(PSP_VERSION)
#include <pspsdk.h>
#include <pspmath.h>
#include <pspdisplay.h>
#define sin(x) vfpu_sinf(x)
#endif

#include "plugin/agsplugin.h"

#if defined(BUILTIN_PLUGINS)
namespace agscontroller {
#endif

#if defined(__GNUC__)
inline unsigned long _blender_alpha16_bgr(unsigned long y) __attribute__((always_inline));
inline void calc_x_n(unsigned long bla) __attribute__((always_inline));
#endif

const unsigned int Magic = 0xACAB0000;
const unsigned int Version = 1;
const unsigned int SaveMagic = Magic + Version;
const float PI = 3.14159265f;

IAGSEngine* engine;

struct Controller
{
	int32 id;
	int32 button_count;
	int32 axes_count;
	int32 pov;
	int buttstate[32];
	int axes[16];
	bool isHeld[32];
};


char const* constructname = "controller";
int32 dummydata = 0;

Controller ControllerInAGS;
int MaxControllers = 0;
int ControllerIndex = 0;
Controller dummyController;
SDL_Joystick* sdlController;
SDL_GameController* sdlGameController;
bool isGamepad=false;

int clamp(int value,int min, int max)
{
	if (value <min) value=min;
	if (value <max) value=max;
	return value;
}

class coninterface : public IAGSScriptManagedObject {
public:
	// when a ref count reaches 0, this is called with the address
	// of the object. Return 1 to remove the object from memory, 0 to
	// leave it
	virtual int Dispose(const char *address, bool force);
	// return the type name of the object
	virtual const char *GetType();
	// serialize the object into BUFFER (which is BUFSIZE bytes)
	// return number of bytes used
	virtual int Serialize(const char *address, char *buffer, int bufsize);
};

class conreader : public IAGSManagedObjectReader {
public:
	virtual void Unserialize(int key, const char *serializedData, int dataSize);
};

int coninterface::Dispose(const char *address, bool force)
{
	// delete ((Joystick*) address);
	return 0; //1;
}

const char* coninterface::GetType()
{
	return constructname;
}
coninterface conintf;
conreader conread;

int coninterface::Serialize(const char *address, char *buffer, int bufsize)
{
	// put 1 byte there
	memcpy(buffer, &dummydata, sizeof(dummydata));
	return sizeof(dummydata);
}



void conreader::Unserialize(int key, const char *serializedData, int dataSize)
{
	engine->RegisterUnserializedObject(key, &ControllerInAGS, &conintf);
}



int ControllerCount()
{
	return SDL_NumJoysticks();
}

Controller* Controller_Open(int num)
{
	Controller* con;
	int ax;
	int b;

	if (num == -1)
	{
		con = &dummyController;
	}
	else
	{
		sdlController = SDL_JoystickOpen(num);
		
		if (SDL_IsGameController(num))
		{
			sdlGameController=SDL_GameControllerOpen(num);
			isGamepad=true;
		}
		

		ControllerInAGS.button_count = SDL_JoystickNumButtons(sdlController);
		ControllerInAGS.axes_count = SDL_JoystickNumAxes(sdlController);		

		for (ax = 0; ax<16; ax = ax + 1)
		{
			ControllerInAGS.axes[ax] = 0;
		}

		for (b = 0; b<32; b = b + 1)
		{
			ControllerInAGS.buttstate[b] = SDL_RELEASED;
		}

		int AMAXINT = 0;//131072;

		ControllerInAGS.id = num;
		ControllerInAGS.pov = AMAXINT;

		con = &ControllerInAGS;
	}
	engine->RegisterManagedObject(con, &conintf);


	return con;
}

void Controller_Close(Controller* con)
{
	SDL_JoystickClose(sdlController);
}

int Controller_Plugged(Controller* con)
{
	SDL_JoystickUpdate();
	if (!sdlController)
	{
		return 0;
	}
	else 
	{
		return SDL_JoystickGetAttached(sdlController);
	}
}

int Controller_GetAxis(Controller* con, int axis)
{
	if (axis>con->axes_count || axis<0 || !sdlController) { return 0; }
	SDL_JoystickUpdate();
	ControllerInAGS.axes[axis]=SDL_JoystickGetAxis(sdlController,axis);
	return ControllerInAGS.axes[axis];
}


int Controller_GetPOV(Controller* con)
{
	if (!sdlController)
	{
		return -1;
	}
	SDL_JoystickUpdate();
	int setHat=SDL_JoystickGetHat(sdlController,0);

	if (setHat == SDL_HAT_CENTERED)	ControllerInAGS.pov = 0;
	else if (setHat == SDL_HAT_DOWN) ControllerInAGS.pov = 2 ^ 6;
	else if (setHat == SDL_HAT_LEFT) ControllerInAGS.pov = 2 ^ 10;
	else if (setHat == SDL_HAT_RIGHT) ControllerInAGS.pov = 2 ^ 0;
	else if (setHat == SDL_HAT_UP) ControllerInAGS.pov = 2 ^ 3;
	else if (setHat == SDL_HAT_LEFTDOWN)ControllerInAGS.pov = 2 ^ 14;
	else if (setHat == SDL_HAT_RIGHTDOWN)ControllerInAGS.pov = 2 ^ 4;
	else if (setHat == SDL_HAT_LEFTUP)ControllerInAGS.pov = 2 ^ 11;
	else if (setHat == SDL_HAT_RIGHTUP)ControllerInAGS.pov = 2 ^ 1;
	
	return ControllerInAGS.pov;
}

int Controller_IsButtonDown(Controller* con, int butt)
{
	if (butt>32 || butt<0 || !sdlController) 
	{
		return 0;
	}
	SDL_JoystickUpdate();
	if (butt < ControllerInAGS.button_count)
	{
		ControllerInAGS.buttstate[butt]=SDL_JoystickGetButton(sdlController,butt);
		
	}

	return ControllerInAGS.buttstate[butt];
}


int Controller_PressAnyKey(Controller*con)
{
	//SDL_JoystickUpdate();
	int butt=0;
	while (butt < 32)//ControllerInAGS.button_count)
	{
		int getButtonState=Controller_IsButtonDown(con,butt);//SDL_JoystickGetButton(sdlController,butt);
		if (getButtonState==1)
		{
			ControllerInAGS.buttstate[butt]=getButtonState;
			return butt;
		}
		butt++;
	}
	return -1;
}


int Controller_BatteryStatus(Controller* con)
{
	if (!sdlController) 
	{
		return 0;
	}
	SDL_JoystickUpdate();
	return SDL_JoystickCurrentPowerLevel(sdlController);
}



int Controller_IsButtonDownOnce(Controller* con, int butt)
{
	if (butt>32 || butt<0 || !sdlController) 
	{
		return 0;
	}
	SDL_JoystickUpdate();
	if (ControllerInAGS.isHeld[butt])
	{
	}
	else 
	{
		if (butt < ControllerInAGS.button_count)
		{
			ControllerInAGS.buttstate[butt]=SDL_JoystickGetButton(sdlController,butt);
		}
		if (ControllerInAGS.buttstate[butt]) ControllerInAGS.isHeld[butt]=true;
		//else ControllerInAGS.isHeld[butt]=false;
		
		return ControllerInAGS.buttstate[butt];
	}
	
}



void Controller_Rumble(Controller* con,int left,int right,int duration)//duration is in LOOPS
{
	if (sdlController)
	{
		duration = (duration/40)*1000;
		int maxFq=65535;
		SDL_JoystickRumble(sdlController,clamp(left,0,maxFq), clamp(right,0,maxFq), duration);
	}
}




const char* Controller_GetName(Controller* con)
{
	if (!Controller_Plugged(con)) {
		return engine->CreateScriptString("disconnected");
	}
	else if (SDL_JoystickName(sdlController) != NULL) {
		return engine->CreateScriptString(SDL_JoystickName(sdlController));
	}
	else {
		return engine->CreateScriptString("");
	}
}

void ClickMouse(int button)
{
	engine->SimulateMouseClick(button);
}

///

void Controller_Update()
{
	if (sdlController)
	{
		int n=0;
		while (n < 32)
		{
			if (!Controller_IsButtonDown(&ControllerInAGS,n)
				&& ControllerInAGS.isHeld[n])
			{
				ControllerInAGS.isHeld[n]=false;
			}
			n++;
		}
	}
	if (sdlGameController!=NULL)
	{
	int up  = SDL_GameControllerGetButton(sdlGameController, SDL_CONTROLLER_BUTTON_DPAD_UP);
	int down= SDL_GameControllerGetButton(sdlGameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    int left= SDL_GameControllerGetButton(sdlGameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    int right = SDL_GameControllerGetButton(sdlGameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

	if (up==1)
	{
		if (left==1) ControllerInAGS.pov = 2 ^ 11;
		else if (right==1) ControllerInAGS.pov = 2 ^ 1;
		else ControllerInAGS.pov = 2 ^ 3;
	}
	else if (down==1)
	{
		if (left==1) ControllerInAGS.pov = 2 ^ 14;
		else if (right==1) ControllerInAGS.pov = 2 ^ 4;
		else ControllerInAGS.pov = 2 ^ 6;
	}
	else 
	{
		if (left==1) ControllerInAGS.pov = 2 ^ 10;
		else if (right==1) ControllerInAGS.pov = 2 ^ 0;
		else ControllerInAGS.pov = 0;
	}

    bool LeftTrigger= !(SDL_GameControllerGetAxis(sdlGameController, SDL_CONTROLLER_AXIS_TRIGGERLEFT)< (abs(32768)-1000));
	bool RightTrigger= !(SDL_GameControllerGetAxis(sdlGameController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)< (abs(32768)-1000));

	int getButtons=ControllerInAGS.button_count-1;

	ControllerInAGS.buttstate[getButtons+1]=LeftTrigger;
	ControllerInAGS.buttstate[getButtons+2]=RightTrigger;

	}
}

//------------------------------------------------------------
// Engine stuff
//------------------------------------------------------------

void AGS_EngineStartup(IAGSEngine *lpEngine)
{
  

	engine = lpEngine;

	engine->RegisterScriptFunction("ControllerCount", (void*)&ControllerCount);
	engine->RegisterScriptFunction("Controller::Open", (void*)&Controller_Open);
	engine->RegisterScriptFunction("Controller::Plugged", (void*)&Controller_Plugged);
	engine->RegisterScriptFunction("Controller::GetAxis", (void*)&Controller_GetAxis);
	engine->RegisterScriptFunction("Controller::GetPOV",(void*)&Controller_GetPOV);
	engine->RegisterScriptFunction("Controller::IsButtonDown", (void*)&Controller_IsButtonDown);
	engine->RegisterScriptFunction("Controller::Close", (void*)&Controller_Close);
	engine->RegisterScriptFunction("Controller::GetName^0", reinterpret_cast<void *>(Controller_GetName));
	engine->RegisterScriptFunction("Controller::Rumble", (void*)&Controller_Rumble);
	engine->RegisterScriptFunction("Controller::IsButtonDownOnce",(void*)&Controller_IsButtonDownOnce);
	engine->RegisterScriptFunction("Controller::PressAnyKey",(void*)&Controller_PressAnyKey);
    engine->RegisterScriptFunction("Controller::BatteryStatus",(void*)&Controller_BatteryStatus);
	engine->RegisterScriptFunction("ClickMouse",(void*)&ClickMouse);

	
	
 	
	engine->AddManagedObjectReader(constructname, &conread);

	//SDL_Init(SDL_INIT_JOYSTICK);
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	engine->RequestEventHook(AGSE_PREGUIDRAW);
}


void AGS_EngineInitGfx(const char *driverID, void *data)
{
}


void AGS_EngineShutdown()
{
	SDL_Quit();
}

int AGS_EngineOnEvent(int event, int data)
{
  if (event == AGSE_PREGUIDRAW)
  {
	 Controller_Update();
  }  
  else if (event == AGSE_RESTOREGAME)
  {
	//engine->FRead(&SFX[j].repeat,sizeof(int),data);
	
  }
  else if (event == AGSE_SAVEGAME)
  {
	  //engine->FWrite(&SFX[j].repeat,sizeof(int),data);
  }
  else if (event == AGSE_PRESCREENDRAW)
  {
    // Get screen size once here.   
	//engine->UnrequestEventHook(AGSE_SAVEGAME);
	//engine->UnrequestEventHook(AGSE_RESTOREGAME);
	
  }
  else if (event ==AGSE_POSTSCREENDRAW)
  {
	  
  }
  return 0;
}

int AGS_EngineDebugHook(const char *scriptName, int lineNum, int reserved)
{
  return 0;
}



#if defined(WINDOWS_VERSION) && !defined(BUILTIN_PLUGINS)

// ********************************************
// ***********  Editor Interface  *************
// ********************************************


IAGSEditor* editor;


LPCSTR AGS_GetPluginName(void)
{
  // Return the plugin description
  return "AGSController";
}

const char* scriptHeader =
"/// Does a single mouse click. \r\n"
"import void ClickMouse(int button);\r\n"
"\r\n"
"/// Returns the number of gamecontrollers found\r\n"
"import int ControllerCount (); \r\n"
"\r\n"
"enum ControllerPOV {\r\n"
"  ePOVCenter = 0,\r\n"
"  ePOVUp = 1,\r\n"
"  ePOVRight = 2,\r\n"
"  ePOVUpRight = 3,\r\n"
"  ePOVDown = 4,\r\n"
"  ePOVDownRight = 6,\r\n"
"  ePOVLeft = 8,\r\n"
"  ePOVUpLeft = 9,\r\n"
"  ePOVDownLeft = 12\r\n"
"};\r\n"
"#define AXIS_RANGE 32768\r\n"
"\r\n"
"managed struct Controller {\r\n"
"readonly int ID;\r\n"
"readonly int ButtonCount;\r\n"
"readonly int AxesCount;\r\n"
"readonly ControllerPOV POV;\r\n"
"\r\n"
"/// Opens specified controller. (0-15)\r\n"
"import static Controller* Open (int ID); // $AUTOCOMPLETESTATICONLY$\r\n"
"\r\n"
"/// Closes controller\r\n"
"import void Close ();\r\n"
"\r\n"
"/// Returns if the controller is currently plugged or not (true / false) \r\n"
"import bool Plugged ();\r\n"
"\r\n"
"/// Returns the controller name\r\n"
"import String GetName ();\r\n"
"\r\n"
"/// Returns axis value bynumber. (0-5)\r\n"
"import int GetAxis (int axis);\r\n"
"\r\n"
"/// Returns POV value. (0-8)\r\n"
"import int GetPOV();\r\n"
"\r\n"
"/// Rumbles the Controller for Duration (in loops). Left and right are motors. Values go from 0 to 65535\r\n"
"import void Rumble(int left,int right,int duration);\r\n"
"\r\n"
"/// Returns true when the specified button is currently down. (0-31)\r\n"
"import bool IsButtonDown (int button);\r\n"
"\r\n"
"/// Returns the first button the player hits on the controller, otherwise returns -1. (0-31)\r\n"
"import int PressAnyKey();\r\n"
"\r\n"
"/// Returns the status of the controller battery. (-1 - 5) UNKNOWN = -1, LESS THAN 5% = 0, LESS THAN 20% = 1, LESS THAN 70% = 2, 100% = 3, WIRED = 4, MAX = 5 \r\n"
"import int BatteryStatus();\r\n"
"\r\n"
"/// Returns true when the specified button is currently down (single press). (0-31)\r\n"
"import bool IsButtonDownOnce(int button);\r\n"
"\r\n"
"};\r\n"
  ;

int  AGS_EditorStartup(IAGSEditor* lpEditor)
{
  // User has checked the plugin to use it in their game

  // If it's an earlier version than what we need, abort.
  if (lpEditor->version < 1)
    return -1;

  editor = lpEditor;
  editor->RegisterScriptHeader(scriptHeader);

  // Return 0 to indicate success
  return 0;
}

void AGS_EditorShutdown()
{
  // User has un-checked the plugin from their game
  editor->UnregisterScriptHeader(scriptHeader);
}

void AGS_EditorProperties(HWND parent)
{
  // User has chosen to view the Properties of the plugin
  // We could load up an options dialog or something here instead
  MessageBoxA(parent, "AGSController", "About", MB_OK | MB_ICONINFORMATION);
}

int AGS_EditorSaveGame(char* buffer, int bufsize)
{
  // We don't want to save any persistent data
  return 0;
}

void AGS_EditorLoadGame(char* buffer, int bufsize)
{
  // Nothing to load for this plugin
}

#endif


#if defined(BUILTIN_PLUGINS)
} // namespace agscontroller
#endif
