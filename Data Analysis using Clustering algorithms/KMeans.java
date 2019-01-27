package Project;

import java.util.*;
import java.util.Map.Entry;

public class KMeans {

    int MAX_ITERATIONS;

    private List<List<Float>> initializeCentroids(List<List<Float>> data, List<Integer> indexes, int k){
        List<List<Float>> centroids = new ArrayList<>();
        //Map<Integer, List<Float>> map = new HashMap<>();
        System.out.println("Enter initial centroids: ");
        Scanner scanner = new Scanner(System.in);
        for(int i=0; i<k; i++){
            centroids.add(data.get(scanner.nextInt()));
        }

        /*int count = 0;
        for(int i=0; i<k; i++){
            int index = indexes.indexOf(i);
            map.put(i, data.get(index));
        }

        for(int index: map.keySet()){
            centroids.add(map.get(index));
        }*/
        return centroids;
    }

    private float distance(List<Float> data, List<Float> centroid){
        float distance = 0;
        for(int i=0; i<data.size(); i++){
            distance += Math.sqrt(Math.pow((data.get(i)-centroid.get(i)),2));
        }
        return distance;
    }

    private int getClusterFor(List<Float> data,List<List<Float>> centroids){
        int minIndex = -1;
        float min = Float.MAX_VALUE;
        for(int i=0; i<centroids.size(); i++){
            float distance = distance(data,centroids.get(i));
            if(distance < min){
                min = distance;
                minIndex = i;
            }
        }
        return minIndex;
    }

    private List<Integer> assignToClusters(List<List<Float>> data, List<List<Float>> centroids){
        List<Integer> clusterIndexes = new ArrayList<>();

        for(List<Float> individual:data){
            clusterIndexes.add(getClusterFor(individual, centroids));
        }

        return clusterIndexes;
    }

    private List<Float> getNewCentroid(List<List<Float>> data){
        List<Float> centroid = data.get(0);
        int size = data.size();
        //System.out.println("size: "+size);
        for(int i=1; i<size; i++){
            for(int j=0; j<data.get(i).size(); j++){
                //System.out.println("in size: "+i+" "+j);
                //System.out.println(data.get(i).get(j)+" "+j);
                centroid.set(j, centroid.get(j)+data.get(i).get(j));
            }
        }

        for(int i=0; i<centroid.size(); i++){
            centroid.set(i, (centroid.get(i)/size));
        }
        return centroid;
    }

    private List<List<Float>> getNewCentroids(List<List<Float>> data, List<Integer> indexes, int k){
        Map<Integer, List<List<Float>>> clusterMap = new HashMap<>();
        for(int i=0; i<data.size(); i++){
            List<List<Float>> list = clusterMap.getOrDefault(indexes.get(i), new ArrayList<>());
            list.add(data.get(i));
            clusterMap.put(indexes.get(i), list);
        }
        for(int i=0; i<k; i++){
            //System.out.println(i+" : "+clusterMap.get(i).size());
        }
        List<List<Float>> centroids = new ArrayList<>();
        for(Entry<Integer, List<List<Float>>> entry:clusterMap.entrySet()){
            centroids.add(getNewCentroid(entry.getValue()));
        }
        return centroids;
    }

    void cluster(List<List<Float>> data, List<Integer> indexes, int k, String filename){
        List<List<Float>> centroids = initializeCentroids(data, indexes, k);
        List<List<Float>> prevCentroids = null;
        List<Integer> clusterIndexes = new ArrayList<>();
        int iteration = 0;

        while(canStop(prevCentroids, centroids, iteration)){
            prevCentroids = centroids;
            clusterIndexes = assignToClusters(data, centroids);
            centroids = getNewCentroids(data, clusterIndexes, k);
            iteration++;
        }
        System.out.println("Rand Index: "+new Index().randIndex(clusterIndexes, indexes));
        new FileWriter(filename+"1").write(data, clusterIndexes);
    }

    private boolean canStop(List<List<Float>> prevCentroid, List<List<Float>> centroid, int iteration){
        if(iteration>MAX_ITERATIONS)
            return false;
        if(prevCentroid==null)
            return true;
        for(int i=0; i<prevCentroid.size(); i++){
            if(!prevCentroid.get(i).equals(centroid.get(i)))
                return true;
        }
        return false;
    }

}
