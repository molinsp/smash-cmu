/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/*********************************************************************
* PluginUtils.cpp - Util functions for creating V-Rep plugins.
*********************************************************************/

#include "PluginUtils.h"
#include <cstdlib>

using namespace VREP;

///////////////////////////////////////////////////////////////////////////////
// Gets a param of int type. Returns 0 if not found.
///////////////////////////////////////////////////////////////////////////////
int PluginUtils::getIntParam(std::string paramName)
{
  return (int) PluginUtils::getDoubleParam(paramName);
}

///////////////////////////////////////////////////////////////////////////////
// Gets a param of double type. Returns 0 if not found.
///////////////////////////////////////////////////////////////////////////////
double PluginUtils::getDoubleParam(std::string paramName)
{
  // Buffer to get the parameter as a string.
  int paramBufferSize = 100;
  char* paramBuffer;

  // Get the param as a string.
  double paramValue = 0;
  paramBuffer = simGetScriptSimulationParameter(sim_handle_main_script, 
    paramName.c_str(), &paramBufferSize);

  // If we got it, try to convert it to double and clear the buffer.
  if(paramBuffer != NULL)
  {
    paramValue = atof(paramBuffer);
    simReleaseBuffer(paramBuffer);
  }
  else
  {
    simAddStatusbarMessage(("Double parameter " + paramName + 
      " not found, returning default value.").c_str());
  }

  return paramValue;
}

///////////////////////////////////////////////////////////////////////////////
// Gets a param of string type. Returns an empty string if not found.
///////////////////////////////////////////////////////////////////////////////
std::string PluginUtils::getStringParam(std::string paramName)
{
  // Buffer to get the parameter as a string.
  int paramBufferSize = 100;
  char* paramBuffer;

  // Get the param as a string.
  std::string paramValue = "";
  paramBuffer = simGetScriptSimulationParameter(sim_handle_main_script, 
    paramName.c_str(), &paramBufferSize);

  // If we got it, try to convert it to string and clear the buffer.
  if(paramBuffer != NULL)
  {
    paramValue = std::string(paramBuffer);
    simReleaseBuffer(paramBuffer);
  }
  else
  {
    simAddStatusbarMessage(("String parameter " + paramName + 
      " not found, returning default value.").c_str());
  }

  return paramValue;
}

///////////////////////////////////////////////////////////////////////////////
// Check for button presses, and return its text. Returns an empty string if no 
// button was pressed.
///////////////////////////////////////////////////////////////////////////////
std::string PluginUtils::getButtonPressedText(std::string uiName)
{
  // Get the details of the last button press, if any.
  int commandsUIHandle = simGetUIHandle(uiName.c_str());
  int eventDetails[2];
  int buttonHandle = simGetUIEventButton(commandsUIHandle, eventDetails);

  // If the button handle is valid and the second event detail is 0, it means a 
  // button was released.
  bool buttonEventHappened = (buttonHandle != -1) ;
  bool buttonWasReleased = (eventDetails[1] == 0);
  if(buttonEventHappened && buttonWasReleased)
  {
    // Get the text of the button that was pressed.
    char* buttonText = simGetUIButtonLabel(commandsUIHandle, buttonHandle);
    std::string buttonTextString(buttonText);
    simReleaseBuffer(buttonText);

    simAddStatusbarMessage(("Button pressed and released: " + 
      buttonTextString).c_str());
    return buttonTextString;
  }

  return "";
}

///////////////////////////////////////////////////////////////////////////////
// Returns the position of an object given its name, in meters from the origin.
///////////////////////////////////////////////////////////////////////////////
void PluginUtils::getObjectPosition(std::string objectName, float position[3])
{
  // Get the handle of the object.
  int objectHandle = simGetObjectHandle(objectName.c_str());

  // Get the cartesian position.
  simGetObjectPosition(objectHandle, -1, position);
}

///////////////////////////////////////////////////////////////////////////////
// Moves an object to the given position.
///////////////////////////////////////////////////////////////////////////////
void PluginUtils::setObjectPosition(std::string objectName, double x, double y, 
  double z)
{
  // Get the handle of the object.
  int objectHandle = simGetObjectHandle(objectName.c_str());

  // Set the cartesian position.
  float position[3];
  position[0] = x;
  position[1] = y;
  position[2] = z;
  simSetObjectPosition(objectHandle, -1, position);
}
