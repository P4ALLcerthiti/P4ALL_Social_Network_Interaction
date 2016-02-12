# P4A - Social Network Interaction  

A module for clustering large numbers of social networks’ users based on their common features.   

P4A Social Network Interaction is a module for clustering large numbers of social networks’ users based on their common features. The algorithm performs anomaly detection and root cause analysis over a K-partite graph, for being able to cluster the data.  

The repository contains the below directories:  

1. [Code] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Code): This includes the source code of the Social Network Interaction Module.
2. [Supplementary] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Supplementary): 
	1. [App] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Supplementary/App) : This includes the source code of  sample application that uses the Social Network Interaction Module.
	2. [Supplementary Files] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Supplementary/SupplementaryFiles/TwitterData): Contains the example files test1.csv, test2.csv, testFileFollowers.csv and testFileFollowing.csv that can be 
	   used in two groups and are necessary for a user to test k-partite graph. The first group (test1.csv and test2.csv) is a small and 
	   very simple example and the second group (testFileFollowers.csv and testFileFollowing.csv) contains real data from twitter that presents 
	   the followers IDs and the following people IDs of some random twitter users.
3. [API Documentation] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/API%20Documentation) : This includes a documentation of module's most basic functions.  

# Dependencies  

The following libraries is necessary for the execution of Social Network Interaction module. Older subversions may also be compatible:  

[Qt 5.5.1] (http://www.qt.io/download-open-source/) : Used to build the sample App.        


# App usage   

## Step 1: Add file(s) for testing:     

In main.cpp file, the user have to add the paths where the csv files are placed (obj->addFile("C:\\...\\*.csv");).   

## Step 2: Set output path 

1. In main.cpp file, the user must set the output path (obj->savePositionsToFile("C:\\..\\testFileCommon.pos");), in order to save the vertices' positions and connections of the main K-partite Graph.
2. In main.cpp file, the user must set the output path (obj->savePositionsToFile("C:\\..\\testFileAbstract.pos");), in order to save the vertices' positions and connections of Abstract K-partite Graph. 

## Step 3: Set K-partite Graph's parameters.

1. User has also the opportunity to set the values of the basic parameters of the K-partite Graph.  

# Funding Acknowledgement

The research leading to these results has received funding from the European Union's Seventh Framework Programme (FP7) under grant agreement No.610510