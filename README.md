# P4A - Social Network Interaction     

P4A Social Network Interaction is a module for clustering large number of social networks’ data based on their common features. By encoding the 
activities of the entities within the network as multi-modal and/or multi-parametric objects (the object-user may 
have many attributes), and by modelling the social network data into k-partite graphs we manage to efficiently
 categorize the entities into certain behavioral groups – provided that the positioning of the objects over the 
 graph is based on the relevance of their attributes.

The repository contains the below directories:  

1. [GGraph] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Code/Visualizations/GGraph): This includes the source code of the Social Network Interaction Module.
2. [Supplementary] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Supplementary): 
	1. [App] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Supplementary/App) : This includes the source [code] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/blob/master/Supplementary/App/main.cpp) of  sample application that uses the Social Network Interaction Module and the [pro] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/blob/master/Supplementary/App/KPGraph.pro) file.   
	2. [Configuration Files] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Supplementary/Configuration%20Files) : This includes two configuration files. The [configuration.ini] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/blob/master/Supplementary/Configuration%20Files/configuration.ini) is used for the configuration of the Twitter's parsers, while the [parameters.ini] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/blob/master/Supplementary/Configuration%20Files/parameters.ini) is responsible for configuring the sample application. 
	3. [Twitter Parsers] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Supplementary/Twitter%20Parsers) : This includes the source code of the twitter parsers, written in Python.  
		i. [getTwitterTrendingTopicsPerCity.py] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/blob/master/Supplementary/Twitter%20Parsers/getTwitterTrendingTopicsPerCity.py) : Twitter parser for collecting the trending topics for specified cities.  
			Inputs: The following parameters are passed as inputs to the script via the [configuration.ini] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/blob/master/Supplementary/Configuration%20Files/configuration.ini)  
					- User's authentication params  
					- Cities : A list with the cities (separated with commas) that we are interested in  collecting their trending topics.  
					- Cities WOEID: A list with the corresponding cities' WOEID (separated with commas). You can find a city's WOEID [here] (http://woeid.rosselliot.co.nz/).  
					- Continent : A list with the corresponding cities' continent (separated with commas).  
			Sample Output:   
					
		ii. [getTweetsCity.py] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/blob/master/Supplementary/Twitter%20Parsers/getTweetsCity.py) : Twitter parser for collecting tweets containing certain keywords around specified areas.  
			Inputs: The following parameters are passed as inputs to the script via the [configuration.ini] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/blob/master/Supplementary/Configuration%20Files/configuration.ini)  
					- User's authentication params  
					- KeywordsList: A list with key-words (separated with comma), for searching tweets containing these key-words.  
					- Cities : A list with the cities (separated with commas) that we are interested in  collecting the relative tweets.  
					- Cities_lat_lng : List with the latitude, longitude and the searching area for each of the corresponding cities, that we are interested in  collecting the relative tweets.   
					- lang : language  
					- recordsPerCity : Max number of records per city  
			Sample Output:  
			
		iii. [getUsersIdsSpecificTweets_v4.py] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/blob/master/Supplementary/Twitter%20Parsers/getUsersIdsSpecificTweets_v4.py) : Twitter parser for collecting the Following and the Followings for a number of users.  
			Inputs: The following parameters are passed as inputs to the script via the [configuration.ini] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/blob/master/Supplementary/Configuration%20Files/configuration.ini)  
					- User's authentication params  
					- keyword: Searching users who have posted tweets containing the specified keyword.  
					- users_limit : Max number of users  
			Sample Output:  
	4. [Sample Data] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Supplementary/SampleData) contains:   
		i.  sample output data for each Twitter Parser.  
		
		ii. [Results] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/tree/master/Supplementary/SampleData/Results) : contains the final output of the 1st example. The directory contains two csv files describing topology of the graph ([graph_structure.csv] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/blob/master/Supplementary/SampleData/Results/graph_structure.csv) and the  [vertex_positions.csv] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/blob/master/Supplementary/SampleData/Results/vertex_positions.csv) ) and an [image] (https://github.com/P4ALLcerthiti/P4ALL_Social_Network_Interaction/blob/master/Supplementary/SampleData/Results/trending_topics.png) view of the results.  
	

# Dependencies  

The following libraries are necessary for the execution of Social Network Interaction module. Older subversions may also be compatible:  

1. [Qt 5.5.1] (http://www.qt.io/download-open-source/) : Used to build the sample App.        

2. [Tweepy] (http://www.tweepy.org/) : Used to access the Twitter API.      

The Twitter parsers were tested in Python 3.4. The app was built and ran using [Qt Creator] (https://www.qt.io/download/) 3.5.1  

# App usage   

## Step 1: Set the [configuration.ini] () file    

## Step 2: Set the [parameters.ini] () file and pass it as input to the app

1. Set the number of the example in order to to run the corresponding twitter parser (e.g. runExample=1: Runs the [TwitterParser1] ())


# Funding Acknowledgement

The research leading to these results has received funding from the European Union's Seventh Framework Programme (FP7) under grant agreement No.610510