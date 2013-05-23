//####################################################################
//# Usage of this software requires acceptance of the SMASH-CMU License,
//# which can be found at the following URL:
//#
//# https://code.google.com/p/smash-cmu/wiki/License
//######################################################################

package cmu.sei.smash.bridgeforming;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;
import java.util.Comparator;
import java.util.HashSet;

/**
 * Class that calculates the optimal solution for bridge forming, as well as a
 * simplified version of the BridgeForming algorithm in order to compare the time
 * it takes to complete.
 */
public class BridgeForming 
{
	private static String csvFile = "optimalInput.csv";	
	
	// Information about the distances between all drones to all locations in the bridge.
	private static int numDrones = 0;	
	private static int numLocations = 0;	
	private static double[][] distances;
	
	// The best configuration for a specific scenario will be stored here.
	private static String bestTotalDistancePermutation = "";
	private static double bestTotalDistance = -1;
	private static double largestIndividualDistanceOnBestTotalDistance = -1; 
	
	private static String bestTotalTimePermutation = "";
	private static double bestIndividualDistance = -1;
	private static double totalDistanceOnBestTotalTime = -1;
	
	/**
	 * @param args
	 */
	public static void main(String[] args) 
	{
		searchFromFile();
	}
	
	/**
	 * Loads drone-location distances info from file, for several scenarios. After loading each
	 * scenario, it searches for the optimal solution for that scenario and it displays it.
	 */
	private static void searchFromFile()
	{
		try 
		{
			BufferedReader br = new BufferedReader(new FileReader(csvFile));
		    String line = "";
		    int currDrone = -1;
			while ((line = br.readLine()) != null) 
			{
				// First we have to take the initial setup lines, before we can actually get to the matrix.
				if(currDrone == -1 || currDrone == numDrones-1)
				{
					char separator = ',';
					String scenario = line.substring(0, line.indexOf(separator));
					
					line = br.readLine();
					numDrones = Integer.parseInt(line.substring(0, line.indexOf(separator)));
					numDrones = numDrones - 1; // One less as one got stopped looking at the person.
					
					line = br.readLine();
					numLocations = Integer.parseInt(line.substring(0, line.indexOf(separator)));
					
					line = br.readLine();
					int range = Integer.parseInt(line.substring(0, line.indexOf(separator)));
					distances = new double[numDrones][numLocations];
					currDrone = -1;
					System.out.println("*******************************************************************************");
					System.out.println(scenario + ": " + numDrones + " drones, " + numLocations + " locations, range " + range);
					
					// This is needed to jump to the first line of the matrix after we grabbed the headers.
					line = br.readLine();					
				}

				// Load a line in the matrix, which are the distances, for one drone, to all locations.
				currDrone++;				
		        String[] splitStr = line.split(",");
		        for(int currLocation=0; currLocation<numLocations; currLocation++)
		        {
		        	distances[currDrone][currLocation] = Double.parseDouble(splitStr[currLocation]);
		        	//System.out.println(currDrone+","+currLocation+"="+distances[currDrone][currLocation]);
		        }
		        
		        // When we have loaded all lines from the matrix, find the best solution for that scenario.
		        if(currDrone == numDrones-1)
		        {
		        	findBestPermutation();
		        	
		        	// Also calculate the approximation.
		        	calculateNonOptimalSolution();		        	
		        }
			}
		} 
		catch (FileNotFoundException e) 
		{
			e.printStackTrace();
		} 
		catch (IOException e) 
		{
			e.printStackTrace();
		}		
	}
	
	/**
	 * Sets up a search for the optimal solution for a given set of distances between drones and bridge
	 * locations, already loaded in the program, and actually finds the best solution.
	 */
	private static void findBestPermutation()
	{
		bestTotalDistancePermutation = "";
		bestTotalDistance = -1;
		largestIndividualDistanceOnBestTotalDistance = -1;
		
		bestTotalTimePermutation = "";
		bestIndividualDistance = -1;
		totalDistanceOnBestTotalTime = -1;
				
        long initTime = System.nanoTime();
        String baseString = generateBaseString(numDrones);
        combinations(baseString);

        long endTime = System.nanoTime();
        long timeElapsed = endTime - initTime;
        System.out.println("Elapsed time: " + timeElapsed/1000000.0 + " ms");
        System.out.println("Best permutation in total distance: " + bestTotalDistancePermutation + " with total distance " + bestTotalDistance + " (and worst individual distance " + largestIndividualDistanceOnBestTotalDistance + ")");
        System.out.println("Best permutation in minimum total time: " + bestTotalTimePermutation + " with worst individual distance " + bestIndividualDistance + " (and total distance " + totalDistanceOnBestTotalTime + ")");
	}	
	
	/**
	 * Creates a string with the number of drones indicated. Letters are used
	 * for the drone's IDs. In theory this supports more than 25 drones, though
	 * the characters used after Z may break down things. 
	 * @param numDrones
	 * @return
	 */
	private static String generateBaseString(int numDrones)
	{
		String baseString = "";
		for(int i=0; i <numDrones; i++)
		{
			baseString += getLetterFromNumberId(i);
		}
		System.out.println("Base string: " + baseString);
		return baseString;
	}

	/**
	 * Goes over all permutations, at the same time trying to find the optimal one.
	 */
	public static void combinations(String str) 
	{ 
	    combine("", str); 
	}

	/**
	 * Recursive function used for permutations, which at the same time is checking
	 * to keep the best solution. Uses a string instead of an array for this. Each 
	 * element is one character.
	 * @param permutation
	 * @param availableOptions
	 */
	private static void combine(String permutation, String availableOptions) 
	{
		// Only check permutations which have exactly numLocation elements.
		if(permutation.length() == numLocations)
		{			
			double currDistance = calculateCurrTotalDistance(permutation);
			double largestDistanceForThisPermutation = calculateLargestDistance(permutation);

			// Keep the best total distance found so far.
			if(bestTotalDistance == -1 || currDistance < bestTotalDistance)
			{
				bestTotalDistance = currDistance;
				bestTotalDistancePermutation = permutation;
				largestIndividualDistanceOnBestTotalDistance = largestDistanceForThisPermutation;
			}
			
			// Keep the time required for this permutation (storing the largest distance that has to be travelled
			// by one drone in this permutation)
			if(bestIndividualDistance == -1 || largestDistanceForThisPermutation < bestIndividualDistance)
			{
				bestIndividualDistance = largestDistanceForThisPermutation;
				bestTotalTimePermutation = permutation;
				totalDistanceOnBestTotalTime = currDistance;
			}			
		
			//System.out.println(permutation + " with total dist " + currDistance);
			
			// We don't want to continue checking with larger permutations.
			return;
		}
		
		int n = availableOptions.length();		
	    for (int i = 0; i < n; i++)
	    {
	    	char optionToAdd = availableOptions.charAt(i);
	    	String optionsStillAvailable = availableOptions.substring(0, i) + availableOptions.substring(i+1, n);
	    	combine(permutation + optionToAdd, optionsStillAvailable);
	    }
	} 
	
	/**
	 * Calculates the total distance to be travelled by a current drone configuration
	 * given in "permutation" and the initial locations already stored in the program.
	 * @param permutation
	 * @return
	 */
	private static double calculateCurrTotalDistance(String permutation)
	{
		double totalDistance = 0;
		for(int location=0; location<permutation.length(); location++)
		{
			char droneId = permutation.charAt(location);
			double currDistance = getDistance(droneId, location);
			totalDistance += currDistance;
		}
		
		return totalDistance;
	}
	
	/**
	 * Finds the largest distance between the drones in the current permutation. This is useful
	 * to define how long the drones will take to create a bridge, as this depends on the one that
	 * has to travel the most.
	 * @param permutation
	 * @return
	 */
	private static double calculateLargestDistance(String permutation)
	{
		double largestDistance = 0;
		for(int location=0; location<permutation.length(); location++)
		{
			char droneId = permutation.charAt(location);
			double currDistance = getDistance(droneId, location);
			if(currDistance > largestDistance)
			{
				largestDistance = currDistance;
			}
		}
		
		return largestDistance;
	}
	
	/**
	 * Returns the distance to travel for a certain drone to a certain location given that 
	 * a table with these values has already been loaded.
	 * @param droneId The character id of the drone.
	 * @param location The 0-based location inside the bridge.
	 * @return
	 */
	private static double getDistance(char droneId, int location)
	{
		int dronePos = droneId - ((int)'A');
		//System.out.println(location + " , " + dronePos + " = " +  distances[dronePos][location]);
		return distances[dronePos][location];
	}
	
	private static void calculateNonOptimalSolution()
	{
		// Move everything from the matrix into one big array.
		Tuple[] allDistances = new Tuple[numDrones*numLocations];
		int tupleNum = 0;
		for(int currDrone=0; currDrone<numDrones; currDrone++)
		{
			for(int currLocation=0; currLocation<numLocations;currLocation++)
			{
				Tuple currTuple = new Tuple();
				currTuple.dronePos = currDrone;
				currTuple.locationPos = currLocation;
				currTuple.distance = distances[currDrone][currLocation];
				allDistances[tupleNum++] = currTuple;
			}
		}
		
        long initTime = System.nanoTime();		
		
		// Sort the big array.
		Arrays.sort(allDistances, new Comparator<Tuple>() {
            public int compare(Tuple tuple, Tuple otherTuple) {
                return Double.compare(tuple.distance, otherTuple.distance);
            }});
		
		// Loop over the sorted array to find results.
		HashSet<Integer> usedDrones = new HashSet<Integer>();
		HashSet<Integer> usedLocations = new HashSet<Integer>();
		int[] assignedDrones = new int[numLocations];
		double totalDistance = 0;
		for(int i=0; i<allDistances.length; i++)
		{
			Tuple currTuple = allDistances[i];
			if(!usedDrones.contains(currTuple.dronePos) && !usedLocations.contains(currTuple.locationPos))
			{
				usedDrones.add(currTuple.dronePos);
				usedLocations.add(currTuple.locationPos);
				assignedDrones[currTuple.locationPos] = currTuple.dronePos;
				totalDistance += currTuple.distance;
			}
		}
		
        long endTime = System.nanoTime();
        long timeElapsed = endTime - initTime;
		String permutation = generateStringId(assignedDrones);        
        System.out.println("Heuristic- Elapsed time: " + timeElapsed/1000000.0 + " ms");
        System.out.println("Heuristic- Best permutation: " + permutation + " with total distance " + totalDistance);
	}
	
	private static String getLetterFromNumberId(int i)
	{
		return Character.toString ((char) (i + (int) 'A'));
	}
	
	private static String generateStringId(int[] drones)
	{
		String idString = "";
		
		for(int i=0; i<drones.length; i++)
		{
			idString += getLetterFromNumberId(drones[i]);
		}
		
		return idString;
	}
	
	public static class Tuple
	{
		public int dronePos;
		public int locationPos;
		public double distance;
	}
}