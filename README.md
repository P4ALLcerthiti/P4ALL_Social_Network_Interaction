# P4A - Social Network Interaction  

A module that performs anomaly detection and root cause analysis.  

The repository contains the below directories:  

1. [Code] (): This includes the source code of the Social Network Interaction Module.
2. [Supplementary] (): 
	1. [App] : This includes the source code of  sample application that uses the Social Network Interaction Module.
	2. [Supplementary Files] : Contains the examples files test1.csv, test2.csv, testFileFollowers.csv and testFileFollowing.csv that can be 
	   used in two groups and are necessary for a user to test k-partite graph. The first group (testFile1.csv and testFile2.csv) is a small and 
	   very simple example and the second group (testFileFollowers.csv and testFileFollowing.csv) contains real data from twitter that presents 
	   the followers IDs and the following people IDs of some random twitter users.
3. [API Documentation] () : This includes a documentation of module's basic functions.  

# Dependencies  

The following console is necessary for the execution of Social Network Interaction module:  

[Qt Creator 5.5] (http://www.qt.io/download-open-source/) : The installed version of Qt Creator, must be compatible the existing version of Visual Studio.  
[Eigen] (http://eigen.tuxfamily.org/index.php?title=Main_Page) : Library for linear algebra: matrices, vectors, numerical solvers, and related algorithms.  


# App usage  

## Step 1: Add file(s) for testing   

In main.cpp file, the user have to add the paths where the csv files are placed (obj->addFile("C:\\...\\*.csv");).   

## Step 2: Set output path 

1. In main.cpp file, the user must set the output path (obj->savePositionsToFile("C:\\..\\testFileCommon.pos");), in order to get the positions  
   of the main K-partite Graph and connections of each node.
2. In main.cpp file, the user must set the output path (obj->savePositionsToFile("C:\\..\\testFileAbstract.pos");), in order to get the positions  
   of Abstract K-partite Graph and connections of each node. 

## Step 3: Input parameters of the K-partite Graph

1. In main.cpp, the user has the ability either to use the default values of basic parameters of the K-partite Graph or to enter values according to the needs of the test.
