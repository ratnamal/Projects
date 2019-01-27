1. For part 1, compile the remaining files and run the main.java file and accept the input from the console

2. For part 2, run the KMeans jar file as
	hadoop jar KMeansmr.jar KMeansmr /input /output 
in the hadoop instance with the required input directory with cho.txt and iyer.txt
	hadoop fs -mkdir /input
	hadoop fs -put cho.txt /input
	hadoop fs -put iyer.txt /output

Hadoop instance used for this project is 2.6.4 and the java version used is 1.8
