# P4A - Social Network Interaction  

A module that performs anomaly detection and root cause analysis.  

The repository contains the below directories:  

1. [Code] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Code): This includes the source code of the Social Network Interaction Module.
2. [Supplementary] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Supplementary): 
	1. [App] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Supplementary/App) : This includes the source code of  sample application that uses the Social Network Interaction Module.
	2. [Supplementary Files] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Supplementary/SupplementaryFiles/TwitterData): Contains the examples files test1.csv, test2.csv, testFileFollowers.csv and testFileFollowing.csv that can be 
	   used in two groups and are necessary for a user to test k-partite graph. The first group (testFile1.csv and testFile2.csv) is a small and 
	   very simple example and the second group (testFileFollowers.csv and testFileFollowing.csv) contains real data from twitter that presents 
	   the followers IDs and the following people IDs of some random twitter users.
3. [API Documentation] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/API%20Documentation) : This includes a documentation of module's basic functions.  

# Dependencies  

The following libraries is necessary for the execution of Social Network Interaction module:  

[Qt 5.5] (http://www.qt.io/download-open-source/) : Used to build the sample App.    
[Eigen] (http://eigen.tuxfamily.org/index.php?title=Main_Page) : Library for linear algebra: matrices, vectors, numerical solvers, and related algorithms.    


# App usage   

## Step 1: Add file(s) for testing:     

In main.cpp file, the user have to add the paths where the csv files are placed (obj->addFile("C:\\...\\*.csv");).   

## Step 2: Set output path 

1. In main.cpp file, the user must set the output path (obj->savePositionsToFile("C:\\..\\testFileCommon.pos");), in order to save the vertices' positions and connections  
   of the main K-partite Graph.
2. In main.cpp file, the user must set the output path (obj->savePositionsToFile("C:\\..\\testFileAbstract.pos");), in order to save the vertices' positions and connections 
   of Abstract K-partite Graph. 

## Step 3: Input parameters of the K-partite Graph

1. In main.cpp, the user has the ability either to use the default values of basic parameters of the K-partite Graph or to enter values according to the needs of the test.

# Funding Acknowledgement

The research leading to these results has received funding from the European Union's Seventh Framework Programme (FP7) under grant agreement No.610510