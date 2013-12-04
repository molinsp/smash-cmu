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

// The size in degrees of the view zone for the drone.
#define APERTURE_WIDTH  (1.0 * DEGREES_PER_METER)
#define APERTURE_HEIGHT (1.0 * DEGREES_PER_METER)

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
  bool useFile = true;
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
 * @return  Returns true (1) always.
 */
///////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record SMASH::AreaCoverage::madaraUpdateCoverageTracking(
  Madara::Knowledge_Engine::Function_Arguments &args,
  Madara::Knowledge_Engine::Variables &variables)
{
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

  // Mark everything the drone is currently seeing as covered.
  // Loop over the area we view with the aperture, advancing one grid cell
  // at a time, and marking it as covered.
  double viewInitLat = myLat + APERTURE_HEIGHT;
  double viewEndLat = myLat - APERTURE_HEIGHT;
  double viewInitLon = myLon - APERTURE_WIDTH;
  double viewEndLon = myLon + APERTURE_WIDTH;
  //printf("Init lat: %0.10f, end lat: %0.10f\n", viewInitLat, viewEndLat);
  //printf("Init lon: %0.10f, end lon: %0.10f\n", viewInitLon, viewEndLon);
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
  bool useFile = true;
  if(useFile)
  {
    double timePassedInSeconds = variables.get(".coverage_tracking.time_passed_s").to_double();
    outputFile << timePassedInSeconds << "," << percentageCovered << std::endl;
  }
}