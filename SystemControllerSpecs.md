# Connection #

A System Controller can communicate with a drone swarm using a Madara Knowledge Base (which is the same method drones use to communicate with each other). Though the Knowledge Base variables are always the same, the transport used depends on whether real or simulated drones are being used.

Transport settings to connect to real drones:
  * **Host (IP)**: 192.168.1.255:15000
  * **Transport Type**: BROADCAST

Transport settings to connect to simulated drones:
  * **Host (IP**): 239.255.0.1:4150
  * **Transport Type**: MULTICAST

# IDs #

The following ids are recommended for the existing System Controllers:

  * Android System Controller: 201
  * VRep System Controller: 202
  * Command Line System Controller: 203

# Commands #

The following commands can be sent by the System Controller, and are defined by the following variables.

**1. Setup**

> The following variables have to be set before any commands are sent, as the drones are expecting the System Controller to set them. This can be done only once, if the System Controller knows all the drones are connected, or periodically, to ensure that new drones that join the network get this setup values.
    * **devices (optional)**: the total number of drones in the network. By default drones will try to calculate how many there are by seeing the data received from other drones.
    * **device.min\_alt (optional)**: set to a number in meters, such as 2.0. No drones will fly lower than this. Default value is 1.5 m.
    * **bridge.max\_communication\_distance (optional)**: the max range of the short-range radio (WiFi), in meters. Has a default value of 4 m.
    * **area\_coverage.tracking.enabled (optional)**: set to 1 to enable tracking of % of area covered at each iteration of the main loop. By default it is disabled.
    * **area\_coverage.tracking\_file.enabled (optional)**: set to 1 to enable the generation of a CSV file with times (s) and % of area covered at each iteration of the main loop. By default it is disabled.
    * **area\_coverage.tracking.sensor\_angle (optional)**: the angle used by the sensor covering the area. Used to calculate the size of the area being seen at each point in time. Default value is 60 degrees.

**2. Request Area Coverage**

> This command is used to tell some drones to start performing a certain area coverage algorithm on the given area.

  * **Define a new region**: The first step for this is to define a region. Note that this could be done independently on the Request Area Coverage command, if regions want to be defined for future use. The variables for this are (where X is the highest region id, starting at 0):
    * **region.{X}.type** = 0
    * **region.{X}.top\_left.location** = {latitude1},{longitude1}
    * **region.{X}.bottom\_right.location** = {latitude2},{longitude2}

  * **Set a region as a search area**: The next step is to set up a given region (X) as a search area. The variables for defining a new region are (where Y is the highest search area id, starting at 0):
    * **search\_area.{Y}** = {X}
    * **search\_areas** = {search\_areas} + 1

  * **Send search request**: The final step is to tell each drone that we want to be part of a search area that it is, and how to search that area. This has to be done for each drone that we want to participate in a search. The variables for this are (Y is the search area id, Z is the id of each drone, and "{algorithm}" can be "random", "snake" or "inside\_out"):
    * **device.{Z}.search\_area\_id** = {Y}
    * **device.{Z}.area\_coverage\_requested** = "{algorithm}"
    * **device.{Z}.human\_detection\_requested** = "basic"
    * **area\_coverage.line\_width (optional)**: the width of a line of search, or how much to move to the side once this line of search has been finished, in degrees. Has a default value, so it is not mandatory to set this.
    * **area\_coverage.wait\_for\_swarm (optional)**: 1 to wait for each other drone to reach its nth search point before moving onwards; 0 to move independently on where the others are. Defaults to 0.

  * **NOTE**: when setting these variables, the values for all drones should be sent at the same time, so dissemination of the variables should be stopped until all of them are set in the System Controller. This could be modified in the future to use "swarm." commands, to ensure this is sent atomically.

**3. Request Bridge Formation**

> This command is used to tell drones to form a bridge between two regions. This has to be sent to all drones, as they will independently decide whether they should and can join the bridge, or not.

  * **Define two new regions**: This is equivalent to the first step shown for Requesting Area Coverage. This has to be done twice, for the source region and the sink region. NOTE however, than the current bridge algorithm is expecting the top left and bottom right corners of each region to be the same, as it will treat each of these regions as a point (source point and sink point).  The variables for this are (where X is the highest region id, starting at 0):
    * region.{X}.type = 0
    * region.{X}.top\_left.location = {latitudeSource},{longitudeSource}
    * region.{X}.bottom\_right.location = {latitudeSource},{longitudeSource}
    * region.{X+1}.type = 0
    * region.{X+1}.top\_left.location = {latitudeSink},{longitudeSink}
    * region.{X+1}.bottom\_right.location = {latitudeSink},{longitudeSink}

  * **Define a bridge**: The next step is to define a bridge by giving it an id and a source and sink regions. The variables for this are (where Y is the highest bridge id, starting at 0):
    * bridge.{Y}.endpoint.1 = {X}
    * bridge.{Y}.endpoint.2 = {X+1}
    * bridges = {bridges} + 1

  * **Send a bridge request**: The final step is to simply ask the network to form a bridge. This will make all drones check if there are new bridges defined, and calculate if they have to be part of them. The variables for this are):
    * bridge.bridge\_requested = 1

# Information From Drones #

The following is information from the drones that the SystemController can use to show an updated status of the network:
  * **device.{X}.location**: for now {latitude},{longitude}
  * **device.{X}.busy**: 1 if busy (currently means on a bridge)
  * **device.{X}.bridge\_id**: if on a bridge, this id the bridge the drone is part of
  * **location`_`{LAT}`_`{LONG}`_`thermals**: the amount of potential humans found on the LAT,LONG location.