# Prefixes #
| **Prefix** | **Description** |
|:-----------|:----------------|
| device.{X} | Will prefix any device specific information or commands |
| sim        | Used only for simulation purposes |
| .area\_coverage | Used for internal local variables used by the Area Coverage module |
| .bridge    | Used for internal local variables used by the Bridge module |
| `area_coverage_` | Used for internal functions used by the Area Coverage module |
| `bridge_`  | Used for internal functions used by the Bridge module |

# Unique Postfixes #
| **Postfix** | **Type** | **Description**|
|:------------|:---------|:|
| {variable}.location | String   | Contains location information in the form "latitude,longitude[,altitude]" <br> Variables with this postfix will be expanded to {variable}.location.<code>[</code>latitude | longitude | altitude<code>]</code> <br>
<tr><td> {X}         </td><td> Integer  </td><td> Positive integers starting from 0.  </td></tr></tbody></table>

<h1>Global #
| **Variable Name** | **Type** | **Comment** | **Modifier** |
|:------------------|:---------|:------------|:-------------|
| devices           | Integer  | The number of devices in the entire system | System Controller |
| device.min\_alt   | Double   | The minimum height or altitude to use for the drones; this means, their altitude should never be set to a value lower than this one, | System Controller |
| device.{X}.location | Double [Latitude, Longitude, Altitude] | The latest known location of device X | Only device X |
| device.{X}.mobile | Integer  | Set to 1 if this drone is capable of movement | Only device X |
| device.{X}.battery | Integer  | Battery level of the device, measured in minutes remaining | Only device X |
| device.{X}.busy   | Integer  | Set to 1 if this device is occupied | Only device X |
| device.{X}.bridge\_id | Integer  | If the device is part of a bridge, this indicates the id of the bridge | Only device X |
| device.{X}.area\_coverage\_requested | String   | Indicates the type of area coverage that has been requested (currently "random", "snake" and "inside\_out") | System Controller |
| device.{X}.next\_area\_coverage\_requested | String   | Simiar to area\_coverage\_requested, only to queue up which algorithm should be applied once the first one has finished| System Controller |
| device.{X}.search\_area\_id | Integer  | If area\_coverage\_requested was set, this indicates the id of the search area assigned | System Controller |
|                   |          |             |              |
| environment.temperature | Double   | Ambient temperature of the environment. Used by some thermal detection algorithms. | System Controller or Device |
|                   |          |             |              |
| search\_areas     | Integer  | Number of search areas in the system | System Controller |
| search\_area.{X}  | Integer  | Contains the id of the region corresponding to this search area | System Controller |
| region.{X}.type   | Integer  | The type of region X (0 = Rectangle) | System Controller |
| region.{X}.top\_left.location | Double [Latitude, Longitude, Altitude] | The location of the top left corner for region X | System Controller |
| region.{X}.bottom\_right.location | Double [Latitude, Longitude, Altitude]| The location of the bottom right corner for region X | System Controller |
| area\_coverage.line\_width | Double   | Contains width of a line of search, in degrees, for the algorithms that use this (snake and inside\_out). Has a default value. | System Controller |
| area\_coverage.height\_diff | Double   | Contains the vertical distance to leave between drones searching one area, in meters. Has a default value.| System Controller |
|                   |          |             |              |
| bridges           | Integer  | The number of bridges being formed by the entire system | System Controller |
| bridge.{X}.endpoint.{Y} | String   | Contains the variable name of the region for this end point. {Y} can be 1 or 2. | System Controller |
| bridge.bridge\_requested | Integer  | Set to 1 if a bridge has been requested | System Controller |
| bridge.max\_communication\_distance | Double   | Contains the comm. range for the high-bandwith radio, in degrees.  Has a default value.| System Controller |
|                   |          |             |              |
| swarm.movement\_command | String   | The name of a movement action for the entire swarm to perform, for example "takeoff" | System Controller |
| swarm.movement\_command.{X} | Unknown  | Argument X of the movement command action for the swarm, will be copied to .movement\_command.{X} | System Controller |
| device.{X}.movement\_command | String   | The name of a movement action for drone X to perform | System Controller |
| device.{X}.movement\_command.{Y} | Unknown  | Argument X of the movement command action for device X, will be copied to .movement\_command.{X}. {Y} starts from 0. | System Controller |
|                   |          |             |              |
| location`_`{LAT}`_`{LONG}`_`thermals | Integer  | The amount of thermal detections found on the specified (LAT, LONG) location. | Devices      |


# Local #
| **Variable Name** | **Type** | **Comment** |
|:------------------|:---------|:------------|
| .id               | Integer  | This devices ID. The following ranges will be used: 0->199 for Drone Controllers, 200-> for System Controllers. |
|                   |          |             |
| .devices.mobile   | Integer  | The number of mobile devices we know about |
| .devices.mobile.{X} | String   | The name of a specific mobile device, for example .device.mobile.1 might be "device.0" |
|                   |          |             |
| .location         | Double [Latitude, Longitude, Altitude] | This devices current location, same value as device.{.id}.location |
| .location.gps.locks | Integer  | The number of satalite locks the GPS has, assuming GPS is present/enabled |
| .location.latitude | Double   | Latitude of this devices position |
| .location.longitude | Double   | Longitude of this devices position |
| .location.altitude | Double   | Altitude of this devices position |
|                   |          |             |
| .device.{X}.location | Double [Latitude, Longitude, Altitude] | Local copy of device X's location, same as device.{X}.location |
| .device.{X}.location.latitude | Double   | The latitude of device X |
| .device.{X}.location.longitude | Double   | The longitude of device X |
| .device.{X}.location.altitude | Double   | The altitude of device X |
| .device.{X}.assigned\_alt| Double   | The altitude (in meters) assigned for this device to fly at |
|                   |          |             |
| .region.{X}.top\_left.location | Double [Latitude, Longitude, Altitude]| Local copy of region X's top\_left location, same as region.{X}.top\_left.location |
| .region.{X}.top\_left.location.latitude | Double   | The latitude of region X's top\_left location |
| .region.{X}.top\_left.location.longitude | Double   | The longitude of region X's top\_left location |
| .region.{X}.bottom\_right.location | Double [Latitude, Longitude, Altitude] | Local copy of region X's bottom\_right location, same as region.{X}.bottom\_right.location |
| .region.{X}.bottom\_right.location.latitude | Double   | The latitude of region X's bottom\_right location |
| .region.{X}.bottom\_right.location.longitude | Double   | The longitude of region X's bottom\_right location |
|                   |          |             |
| .needs\_bridge    | Integer  | Set to 1 if this device needs to assist in forming a bridge |
|                   |          |             |
| .movement\_command | String   | The name of the movement action to perform, for example, "takeoff" |
| .movement\_command.{X} | Unknown  | Argument X of the movement command action. |