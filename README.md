# AGS-Controller

Created by Dualnames

[AGS Forums Topic](https://www.adventuregamestudio.co.uk/forums/index.php?topic=57129.0)

## 1. Prologue:

I know that Wyz is an incredibly busy man, and has barely managed to maintain the source of the AGS Joystick Plugin, given that, and given that I'm also implementing a version of it with SDL, it seems fitting to disengage the two things between them. A majority of the code has been written/based off code by eri0o and Wyz, so kudos go to them! The inclusion of the SDL library allows for two important things:
1) XInput recognition
2) Cross-platform

## 2. Instructions/Going through the functions

The only thing u have to do, is make sure u put SDL2.dll inside ur debug and compiled folders, otherwise you'll get error messages. The dll will be provided below.


**int ControllerCount ()**

*This returns the number of gamecontrollers found, from 0 to N controllers.*


## Controller Properties & Functions

The plugin uses a custom typedef which is 'Controller'. Controller is self-explanatory, and has a couple of functions and attributes.

**int ID**

*Returns the ID of the controller.*


**int ButtonCount**

*Returns the number of buttons on the controller.*


**int AxesCount**

*Returns the number of Axis on the controller.*


**Open (int ID);**

*Opens specified controller. (0-15)*

A good practice to do is put this on room_load, or game_start

```AGS Script
Controller*gamepad;
function game_start()
{  
  gamepad = Controller.Open(0);
}
``` 
 

**void Close ();**

*Closes the controller, probably not really needed to be honest.*


**bool Plugged ();**

*Returns if the controller is currently plugged or not (true / false)*


**String GetName ();**

*This returns a string containing the name of the gamecontroller. Will return "" on error.*


**int GetAxis (int axis);**

*Returns axis by number -32768 to 32768*

```AGS Script
if (gamepad.GetAxis(0) < -200)
{
//LEFT
}
else if (gamepad.GetAxis(0) > 200)
{
//RIGHT
}
``` 

**int GetPOV();**

*Returns POV value. (0-8)*

```AGS Script
if (gamepad.GetPOV() == ePOVUp)
{
    cEgo.y--;
}
```

**bool IsButtonDown (int button);**

*Returns true when the specified button is currently down. (0-31)*

```AGS Script
int i=0;
String app="";
while (i < gamepad.ButtonCount+2)
{
  app = app.Append(String.Format("button %d is %d[",i, gamepad.IsButtonDown(i)));
  i++;
}
cEgo.SayBackground(app);
//this will parse all the buttons' states and print them on the screen.
``` 


**bool IsButtonDownOnce(int button);**

*Returns true when the specified button is currently down (single press). (0-31)*

```AGS Script
if (gamepad.IsButtonDownOnce(11))
    {
       //click on UI
    }
```

**void Rumble(int left,int right,int duration);**


*Rumbles the Controller for Duration (in loops). Left and right are motors. Values go from 0 to 65535*

```AGS Script
if (gamepad.IsButtonDownOnce(11))
    {
      gamepad.Rumble(65535, 65535, 40);     
    }
``` 

### v1.1

**-Added two new functions**

**void BatteryStatus();**

*Returns the status of the controller battery. (-1 - 5) UNKNOWN = -1, LESS THAN 5% = 0, LESS THAN 20% = 1, LESS THAN 70% = 2, 100% = 3, WIRED = 4, MAX = 5*

```AGS Script
lblstatus.Text=String.Format("BATTERY STATUS: %d",gamepad.BatteryStatus());
```


**int PressAnyKey();**

*Returns the first button the player hits on the controller, otherwise returns -1. (0-31)*

```AGS Script
while (gamepad.PressAnyKey()==-1)
    {
      Wait(1);
    }
    player.Say("Button pressed is button %d",gamepad.PressAnyKey());
```
