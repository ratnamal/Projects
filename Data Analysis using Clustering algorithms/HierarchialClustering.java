package Project;

import java.util.ArrayList;
import java.util.List;

public class HierarchialClustering {

    int clustersFormed = 0;
    List<List<Integer>> clusters = new ArrayList<>();

    private int[] getMergingClusterIndexes(float[][] distanceMatrix){
        float min = Float.MAX_VALUE;
        int index1=0, index2=0;
        for(int i=0; i<distanceMatrix.length; i++){
            for(int j=0; j<distanceMatrix[i].length; j++){
                if(i==j)
                    continue;
                if(distanceMatrix[i][j]<min){
                    min = distanceMatrix[i][j];
                    index1 = i;
                    index2 = j;
                }
            }
        }
        if(index1>index2){
            int temp = index1;
            index1 = index2;
            index2 = temp;
        }
        return new int[]{index1, index2};
    }

    private void mergeClusters(int index1, int index2){
        List<Integer> cluster1 = clusters.get(index1);
        List<Integer> cluster2 = clusters.get(index2);

        clusters.remove(index1);
        clusters.remove(index2-1);
        int size = cluster2.size();
        for(int i=0; i<size; i++){
            cluster1.add(cluster2.get(i));
        }
        clusters.add(index1, cluster1);
    }

    private float[][] getDistanceMatrix(float[][] distanceMatrix){
        int[] mergingIndexes = getMergingClusterIndexes(distanceMatrix);
        if(mergingIndexes[0]==mergingIndexes[1]){
            System.out.println(mergingIndexes[0]+" "+clusters.size());
            return distanceMatrix;
        }
        mergeClusters(mergingIndexes[0], mergingIndexes[1]);
       // System.out.println(mergingIndexes[0]+" "+mergingIndexes[1]+"----------------------------------------------------------------------------");
        //System.out.println(clusters);
        //int[][] newMatrix = new int[distanceMatrix.length-1][distanceMatrix.length-1];
        //float[] newDistances = getUpdatedDistances(distanceMatrix.length, mergingIndexes);
        float[][] newDistanceMatrix = new float[distanceMatrix.length-1][distanceMatrix.length-1];

        for(int i=0; i<distanceMatrix.length-1; i++){
            for(int j=0; j<distanceMatrix.length-1; j++){
                if(i==j){
                    newDistanceMatrix[i][j] = 0;
                } else if(clusters.get(i).contains(mergingIndexes[0])){
                    if(j>=mergingIndexes[1]){
                        newDistanceMatrix[i][j] = Math.min(distanceMatrix[mergingIndexes[0]][j+1], distanceMatrix[mergingIndexes[1]][j+1]);
                    }else {
                        newDistanceMatrix[i][j] = Math.min(distanceMatrix[mergingIndexes[0]][j], distanceMatrix[mergingIndexes[1]][j]);
                    }
                } else if(clusters.get(j).contains(mergingIndexes[0])){
                    if(i>=mergingIndexes[1]){
                        newDistanceMatrix[i][j] = Math.min(distanceMatrix[i+1][mergingIndexes[0]], distanceMatrix[i+1][mergingIndexes[1]]);
                    } else {
                        newDistanceMatrix[i][j] = Math.min(distanceMatrix[i][mergingIndexes[0]], distanceMatrix[i][mergingIndexes[1]]);
                    }
                } else{
                    newDistanceMatrix[i][j] = j>=mergingIndexes[1]?distanceMatrix[i][j+1]:(i>=mergingIndexes[1]?distanceMatrix[i+1][j]:distanceMatrix[i][j]);
                }
                //System.out.print(newDistanceMatrix[i][j]+" ");
            }
            //System.out.println();
        }
        return newDistanceMatrix;
    }

    private float distance(List<Float> cluster1, List<Float> cluster2){
        float distance = 0;
        for(int i=0; i<cluster1.size(); i++){
            distance += Math.sqrt(Math.pow((cluster1.get(i)-cluster2.get(i)),2));
        }
        return distance;
    }

    private float[][] initialize(List<List<Float>> data){
        int size = data.size();
        for(int i=0; i<size; i++){
            List<Integer> cluster = new ArrayList<>();
            cluster.add(i);
            clusters.add(cluster);
        }

        float[][] distanceMatrix = new float[size][size];
        for(int i=0; i<size; i++){
            for (int j=0; j<size; j++){
                if(i==j){
                    distanceMatrix[i][j] = 0;
                } else if(distanceMatrix[j][i]!=0){
                    distanceMatrix[i][j]=distanceMatrix[j][i];
                } else {
                    distanceMatrix[i][j] = distance(data.get(i), data.get(j));
                }
                //System.out.print(distanceMatrix[i][j]+" ");
            }
            //System.out.println();
        }
        return distanceMatrix;
    }

    void cluster(List<List<Float>> data, List<Integer> indexes, String filename){
        int k=20;
        float[][] distanceMatrix = initialize(data);
        float[][] prevdistanceMatrix;
        while(clusters.size()!=k && clusters.size()>1){
            prevdistanceMatrix = getDistanceMatrix(distanceMatrix);
            if(prevdistanceMatrix==distanceMatrix){
                break;
            }
            distanceMatrix=prevdistanceMatrix;
        }
        List<Integer> clusterIndexes = new ArrayList<>();
        for(int i=0; i<data.size(); i++){
            clusterIndexes.add(0);
        }
        int index=0;
        for(int i=0; i<clusters.size(); i++){
            for(int num:clusters.get(i)){
                clusterIndexes.set(num, index);
            }
            index+=1;
        }
        System.out.println("Rand Index: "+new Index().randIndex(clusterIndexes, indexes));
        new FileWriter(filename+"1").write(data, clusterIndexes);
    }

}
