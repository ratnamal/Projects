import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.*;
import org.apache.hadoop.mapred.lib.MultipleOutputs;

public class KMeansmr extends Configured {
    public static class Map extends MapReduceBase implements org.apache.hadoop.mapred.Mapper<Text, Text, IntWritable, Text> {
        private List<List<Float>> centroids;

        private List<Float> initCentroid(String[] attributes){
            List<Float> centroid = new ArrayList<>();
            for(int index=1; index < attributes.length ; index++){
                centroid.add(Float.parseFloat(attributes[index]));
            }
            return centroid;
        }

        private void initializeCentroids(JobConf conf){
            int count = Integer.parseInt(conf.get("centroid.count"));
            for (int i=0; i < count; i++) {
                List<Float> centroid = initCentroid(conf.get("centroid."+i).split("\t"));
                centroids.add(centroid);
            }
        }

        public void configure(JobConf conf) {
            super.configure(conf);
            if (this.centroids == null) {
                this.centroids = new ArrayList<>();
                initializeCentroids(conf);
            }
        }

        private float distance(List<Float> object1, List<Float> object2){
            float distance = 0;
            for (int i=0; i<object1.size(); i++){
                distance += Math.sqrt(Math.pow((object1.get(i) - object2.get(i)), 2));
            }
            return distance;
        }

        private int assignToCluster(List<Float> object){
            double nearestClusterDistance = Float.MAX_VALUE;
            int index = 0;
            for (int i=0; i<centroids.size(); i++) {
                float distance = distance(centroids.get(i), object);
                if (distance < nearestClusterDistance) {
                    nearestClusterDistance = distance;
                    index = i;
                }
            }
            return index;
        }

        public void map(Text key, Text input,
                        OutputCollector<IntWritable, Text> output, Reporter reporter) throws IOException {
            String inputPath = input.toString();
            String[] in = inputPath.split("\t");

            List<Float> object = new ArrayList<>();
            for(int index=1; index < in.length ; index++)
                object.add(Float.parseFloat(in[index]));

            int nearsetCentroid = assignToCluster(object);
            output.collect(new IntWritable(nearsetCentroid), new Text(object.toString()));
        }
    }

    public static class Reduce extends MapReduceBase implements org.apache.hadoop.mapred.Reducer<IntWritable, Text, IntWritable, Text> {
        private List<List<Float>> centroids;
        private MultipleOutputs Outputs;

        private List<Float> initCentroid(String[] attributes){
            List<Float> centroid = new ArrayList<>();
            for(int index=1; index < attributes.length ; index++){
                centroid.add(Float.parseFloat(attributes[index]));
            }
            return centroid;
        }

        private void initializeCentroids(JobConf conf){
            int count = Integer.parseInt(conf.get("centroid.count"));
            for (int i=0; i < count; i++) {
                List<Float> centroid = initCentroid(conf.get("centroid."+i).split("\t"));
                centroids.add(centroid);
            }
        }

        public void configure(JobConf conf) {
            Outputs = new MultipleOutputs(conf);
            super.configure(conf);
            if (this.centroids == null) {
                this.centroids = new ArrayList<>();
                initializeCentroids(conf);
            }
        }

        private List<Float> convert(String str){
            List<Float> object = new ArrayList<>();
            String[] words = str.split(",");
            int size = words.length;
            for(int i=0; i<size; i++){
                if(((i+1)==size)||i==0){
                    StringBuilder buffer = new StringBuilder(words[i]);
                    int length =  i==0 ? buffer.length():buffer.length()-1;
                    words[i]="";
                    for(int j=0; j<length; j++){
                        words[i] = words[i] + buffer.charAt(j);
                    }
                }
                object.add(Float.parseFloat(words[i]));
            }
            return object;
        }

        private List<List<Float>> getClusterObjects(Iterator<Text> input){
            List<List<Float>> data = new ArrayList<>();
            while(input.hasNext()){
                data.add(convert(input.next().toString()));
            }
            return data;
        }

        private List<Float> getNewClusterCentroids(List<List<Float>> data){
            List<Float> centroids = data.get(0);
            int size = data.size();
            for(int i=1; i<size; i++){
                for(int j=0; j<data.get(i).size(); j++){
                    float sum = centroids.get(j)+data.get(i).get(j);
                    centroids.set(j, sum);
                }
            }
            for(int i=0; i<centroids.size(); i++){
                float average = centroids.get(i)/size;
                centroids.set(i, average);
            }
            return centroids;
        }

        private String getCentroids(List<List<Float>> data){
            StringBuilder buffer = new StringBuilder();
            for(List<Float> object:data){
                for(float num:object){
                    buffer.append(Float.toString(num));
                    buffer.append("\t");
                }
            }
            return buffer.toString();
        }

        private String getNewCentroids(List<Float> centroids){
            StringBuilder buffer = new StringBuilder();
            for(float num:centroids){
                buffer.append(Float.toString(num));
            }
            return buffer.toString();
        }

        public void reduce(IntWritable key, Iterator<Text> values, OutputCollector<IntWritable, Text> output,
                           Reporter reporter) throws IOException {
            List<List<Float>> data = getClusterObjects(values);
            List<Float> newCentroids = getNewClusterCentroids(data);
            String centroid = getCentroids(data);

            Outputs.getCollector("newcentroid", reporter).collect(key, new Text(getNewCentroids(newCentroids)));
            Outputs.getCollector("output", reporter).collect(key, new Text(centroid));

            output.collect(new IntWritable(1+Integer.parseInt(key.toString())), new Text(centroid));
        }
    }


    private static void setPaths(String inp, String out, JobConf job){
        MultipleOutputs.addNamedOutput(job, "newcentroid", TextOutputFormat.class , IntWritable.class, Text.class);
        MultipleOutputs.addNamedOutput(job, "output", TextOutputFormat.class , IntWritable.class, Text.class);
        org.apache.hadoop.mapred.FileInputFormat.setInputPaths(job, new Path(inp));
        org.apache.hadoop.mapred.FileOutputFormat.setOutputPath(job, new Path(out));
    }

    private static int getIndex(String str, JobConf job) throws IOException{
        int count=0;
        FileSystem fs = FileSystem.get(job);
        DataInputStream stream = new DataInputStream(fs.open(new Path(str)));
        BufferedReader reader = new BufferedReader(new InputStreamReader(stream));
        String line = reader.readLine();
        while (line!= null) {
            job.set("centroid."+count, line);
            count++;
            line=reader.readLine();
        }
        stream.close();
        return count;
    }

    private static void startHadoopJob(String cluster, String out, int k, String file, String inp) throws IOException {
        JobConf job = new JobConf(new Configuration(), KMeans.class);

        job.setJobName(file);
        job.setInputFormat(KeyValueTextInputFormat.class);
        job.setOutputKeyClass(IntWritable.class);
        job.setOutputValueClass(Text.class);
        job.setMapperClass(Map.class);
        job.setReducerClass(Reduce.class);
        job.setNumMapTasks(k);

        setPaths(inp, out, job);
        int index = getIndex(cluster, job);
        job.set("centroid.count", Integer.toString(index));
        JobClient.runJob(job);
    }

    private static void runMapReduce(String file) throws Exception{
        String path = "./input/"+file+".txt";
        String fileName = "kmeans-"+file+"-output";
        String cluster = "./output/"+fileName+"/"+0+"/newcentroid-r-00000";
        startHadoopJob("./input/"+file+"-center.txt", "./output/"+fileName+"/0", 5, fileName, path);
        for(int i=10 ; i>0; i--){
            startHadoopJob(cluster, "./output/"+file+"/"+i, 5, fileName, path);
        }
    }


    public static void main(String[] args) throws Exception {
        runMapReduce("cho");
        runMapReduce("/iyer");
    }
}