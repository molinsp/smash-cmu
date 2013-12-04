/********************************************************************* 
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/*********************************************************************
 * coverage_tracker.cpp - Defines functions for tracking coverage.
*********************************************************************/

#include "coverage_tracker.h"
#include "utilities/CommonMadaraVariables.h"
#include "utilities/gps_utils.h"
#include "utilities/string_utils.h"

#include <math.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace SMASH::AreaCoverage;
using namespace SMASH::Utilities;
using std::string;

// The angle the sensor has when covering the area.
#define DEFAULT_THERMAL_SENSOR_ANGLE 60.0

// The size of a cell in the grid which we will use to discretize coverage, in
// degrees.
#define GRID_CELL_WIDTH   (0.5 * DEGREES_PER_METER)
#define GRID_CELL_HEIGHT  (0.5 * DEGREES_PER_METER)

// Madara variables for the grid size.
#define MV_GRID_WIDTH       ".coverage_tracking.grid.width"
#define MV_GRID_HEIGHT      ".coverage_tracking.grid.height"
#define MV_GRID_SIZE        ".coverage_tracking.grid.size"

// Stores whether each cell in the grid has been covered. These have to be
// global variables so that they are shared between drones and an overall
// coverage percentage can be calculated.
#define MV_GRID_CELL(i)     "coverage_tracking.grid.cell." + std::string(i) + ".covered"

// Stores locally the percentage covered by all drones.
#define MV_PERCENT_COVERED  ".coverage_tracking.grid.covered"

// File to store the data in an easy-to-process form.
ofstream outputFile;

// Function declarations, locally used below.
static void setupCoverageTracking(
  Madara::Knowledge_Engine::Variables &variables);
static void updateCoveragePercentage(
  Madara::Knowledge_Engine::Variables &variables);

///////////////////////////////////////////////////////////////////////////////
/**
 * Sets up the area coverage tracker.
 */
///////////////////////////////////////////////////////////////////////////////
void setupCoverageTracking(
  Madara::Knowledge_Engine::Variables &variables)
{
  printf("Resetting coverage.\n");

  // Get the coordinates of the search area.
  std::string myAssignedSearchArea = variables.get(
    variables.expand_statement(MV_ASSIGNED_SEARCH_AREA("{" MV_MY_ID "}")))
    .to_string();
  std::string myAssignedSearchRegion = variables.get(
    MV_SEARCH_AREA_REGION(myAssignedSearchArea)).to_string();
  double topLeftLat = variables.get(
    MV_REGION_TOPLEFT_LAT(myAssignedSearchRegion)).to_double();
  double topLeftLon = variables.get(
    MV_REGION_TOPLEFT_LON(myAssignedSearchRegion)).to_double();
  double bottomRightLat = variables.get(
    MV_REGION_BOTRIGHT_LAT(myAssignedSearchRegion)).to_double();
  double bottomRightLon = variables.get(
    MV_REGION_BOTRIGHT_LON(myAssignedSearchRegion)).to_double();

  // Calculate the size of the search area.
  // Note that this may not be accurate for big areas, as we are simplifying
  // the diff between locations by just substracting latitudes and longitudes.
  double areaWidth = fabs(bottomRightLon - topLeftLon);
  double areaHeight = fabs(topLeftLat - bottomRightLat);

  // Calculate the size of the grid we are using to track coverage.
  double gridWidth = ceil(areaWidth / GRID_CELL_WIDTH);
  double gridHeight = ceil(areaHeight / GRID_CELL_HEIGHT);
  double gridSize = gridWidth * gridHeight;

  // Store this in the knowledge base.
  variables.set(MV_GRID_WIDTH, gridWidth);
  variables.set(MV_GRID_HEIGHT, gridHeight);
  variables.set(MV_GRID_SIZE, gridSize);

  // Create a file to store data to parse it later easily.
  int fileEnabled = variables.get(MV_COVERAGE_TRACKING_FILE_ENABLED).to_integer();
  bool useFile = (fileEnabled == 1);
  if(useFile)
  {
    int id = variables.get(MV_MY_ID).to_integer();
    std::string outputFileName = "coveragedata_id" + NUM_TO_STR(id) + ".csv";

    // Close the file if it was already open due to a previous tracking.
    if(outputFile.is_open())
    {
      outputFile.close();
    }

    // Open the file, overwriting the previous one if there was one.
    outputFile.open(outputFileName);
  }
}

///////////////////////////////////////////////////////////////////////////////
/**
 * Updates the tracking variables.
 * @return  Returns true (1) if it tracks, or false (0) if it was disabled.
 */
///////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record SMASH::AreaCoverage::madaraUpdateCoverageTracking(
  Madara::Knowledge_Engine::Function_Arguments &args,
  Madara::Knowledge_Engine::Variables &variables)
{
  // Only do something if tracking is enabled.
  int trackingEnabled = variables.get(MV_COVERAGE_TRACKING_ENABLED).to_integer();
  if(trackingEnabled == 0)
  {
    // If we are not enabled, just return saying so.
    return Madara::Knowledge_Record(0.0);
  }

  // Setup the overal tracking variables, if they have not been setup yet.
  int resetCoverage = variables.get(MV_START_COVERAGE_TRACKING).to_integer();
  if(resetCoverage == 1)
  {
    // Setup and mark
    setupCoverageTracking(variables);
    variables.set(MV_START_COVERAGE_TRACKING, 0.0);
  }

  // Set the starting time, if this is the first time we are called.
  variables.evaluate("(.coverage_tracking.init_time == 0) => (.coverage_tracking.init_time = #get_time());");

  // Set the time that has passed so far.
  variables.evaluate(".coverage_tracking.curr_time = #get_time();");
  variables.evaluate(".coverage_tracking.time_passed = (.coverage_tracking.curr_time - .coverage_tracking.init_time);");
  variables.evaluate(".coverage_tracking.time_passed_s = .coverage_tracking.time_passed/1000000000.0;");

  // Set up the default value for the aperture, locally, if there is none yet.
  double currApertureAngle = variables.get(MV_THERMAL_SENSOR_ANGLE).to_double();
  if(currApertureAngle == 0)
  {
    variables.set(MV_THERMAL_SENSOR_ANGLE, DEFAULT_THERMAL_SENSOR_ANGLE, 
      Madara::Knowledge_Engine::Eval_Settings(true, true));
  }

  // Get the coordinates of the search area.
  std::string myAssignedSearchArea = variables.get(
    variables.expand_statement(MV_ASSIGNED_SEARCH_AREA("{" MV_MY_ID "}")))
    .to_string();
  std::string myAssignedSearchRegion = variables.get(
    MV_SEARCH_AREA_REGION(myAssignedSearchArea)).to_string();
  double topLeftLat = variables.get(
    MV_REGION_TOPLEFT_LAT(myAssignedSearchRegion)).to_double();
  double topLeftLon = variables.get(
    MV_REGION_TOPLEFT_LON(myAssignedSearchRegion)).to_double();
  printf("Top left lon: %0.10f, top left lat: %0.10f\n", topLeftLon, topLeftLat);

  // Get grid information.
  double gridWidth = variables.get(MV_GRID_WIDTH).to_double();
  double gridHeight = variables.get(MV_GRID_HEIGHT).to_double();
  double gridSize = variables.get(MV_GRID_SIZE).to_double();

  // Get our current location.
  double myLat = variables.get(variables.expand_statement(
    MV_DEVICE_LAT("{" MV_MY_ID "}"))).to_double();
  double myLon = variables.get(variables.expand_statement(
    MV_DEVICE_LON("{" MV_MY_ID "}"))).to_double();

  // Calculate the aperture, how far we can see.
  // This is done by using the aperture angle from the sensor that is covering
  // the area. Since we have the height, with half that angle we can form an
  // renctangular triangle and calculate the radius of the circle that the
  // sensor can see through trigonometry. We then inscribe a square in that
  // circle (to simplify we will represent our view area as a square).
  double apertureDegrees = variables.get(MV_THERMAL_SENSOR_ANGLE).to_double();
  double currentHeight = variables.get(MV_LOCAL_ALTITUDE).to_double();
  double apertureDiameter = tan(DEG_TO_RAD(apertureDegrees/2.0))*currentHeight*2;
  double squareAreaSide = apertureDiameter / sqrt(2.0);
  printf("View area side size: %f m\n", squareAreaSide);

  // Calculate the borders of the square centered on our current location, with
  // a side size calculated above.
  double degreesFromCenter = squareAreaSide/2.0 * DEGREES_PER_METER;
  double viewInitLat = myLat + degreesFromCenter;
  double viewEndLat = myLat - degreesFromCenter;
  double viewInitLon = myLon - degreesFromCenter;
  double viewEndLon = myLon + degreesFromCenter;
  //printf("Init lat: %0.10f, end lat: %0.10f\n", viewInitLat, viewEndLat);
  //printf("Init lon: %0.10f, end lon: %0.10f\n", viewInitLon, viewEndLon);

  // Mark everything the drone is currently seeing as covered.
  // Loop over the area we view with the aperture, advancing one grid cell
  // at a time, and marking it as covered.
  for(double lat = viewInitLat; lat >= viewEndLat; lat -= GRID_CELL_HEIGHT)
  {
    for(double lon = viewInitLon; lon <= viewEndLon; lon += GRID_CELL_WIDTH)
    {
      // Convert this coordinate into a grid cell.
      int cellX = (ceil((lon - topLeftLon) / GRID_CELL_WIDTH));
      int cellY = (ceil((topLeftLat - lat) / GRID_CELL_HEIGHT));

      // Ignore cells outside of our area (since the view area can see things
      // outside of our search area if we are in borders).
      if(cellX < 0 || cellY < 0)
      {
        //printf("Ignoring current cell as it is outside our search area (%d, %d)\n", cellX, cellY);
        continue;
      }

      int cellNumber = cellY * gridWidth + cellX;
      //printf("Marking cell: %d, coords %0.10f,%0.10f, parts x=%d, y=%d\n", cellNumber, lat, lon, cellX, cellY);

      // Mark this cell as covered in the knowledge base.
      std::string cellNumberString = NUM_TO_STR(cellNumber);
      variables.set(MV_GRID_CELL(cellNumberString), 1.0);
    }
  }

  // Update the percentage.
  updateCoveragePercentage(variables);

  return Madara::Knowledge_Record(1.0);
}

///////////////////////////////////////////////////////////////////////////////
/**
 * Returns the percentage of area covered.
 */
///////////////////////////////////////////////////////////////////////////////
void updateCoveragePercentage(
  Madara::Knowledge_Engine::Variables &variables)
{
  // Get grid information.
  double gridWidth = variables.get(MV_GRID_WIDTH).to_double();
  double gridHeight = variables.get(MV_GRID_HEIGHT).to_double();
  double gridSize = variables.get(MV_GRID_SIZE).to_double();

  // Loop over the array to summ all values.
  int totalCellsCovered = 0;
  for(double i = 0; i < gridSize; i++)
  {
    std::string cellNumberString = NUM_TO_STR(i);
    int currCellCovered = variables.get(MV_GRID_CELL(cellNumberString))
      .to_integer();
    totalCellsCovered += currCellCovered;
  }

  // Store the percentage in the knowledge base.
  //printf("Grid size: %f, total covered: %d\n", gridSize, totalCellsCovered);
  double percentageCovered = totalCellsCovered / gridSize * 100;
  variables.set(MV_PERCENT_COVERED, percentageCovered);

  // Store in file.
  int fileEnabled = variables.get(MV_COVERAGE_TRACKING_FILE_ENABLED).to_integer();
  bool useFile = (fileEnabled == 1);
  if(useFile)
  {
    double timePassedInSeconds = variables.get(".coverage_tracking.time_passed_s").to_double();
    outputFile << timePassedInSeconds << "," << percentageCovered << std::endl;
  }
}