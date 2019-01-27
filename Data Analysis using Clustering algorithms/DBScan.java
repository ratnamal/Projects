package Project;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class DBScan {

    private float getEpsilon(List<List<Float>> data, int minPts){
        float epsilon = 0;

        for(int i = 0; i < data.size(); i++) {
            float distance[] = new float[data.size() - 1];
            int count = 0;
            for(int j = 0; j < data.size(); j++) {
                if(j != i) {
                    distance[count] = distance(data.get(i), data.get(j));
                    count+=1;
                }
            }
            Arrays.sort(distance);
            epsilon+= distance[minPts - 1];
        }
        return epsilon/data.size();
    }

    private float distance(List<Float> object1, List<Float> object2){
        float distance = 0;
        for(int i=0; i<object1.size(); i++){
            distance += Math.sqrt(Math.pow((object1.get(i) - object2.get(i)), 2));
        }
        return distance;
    }

    private List<Integer> rangeQuery(int index, List<List<Float>> data, float epsilon){
        List<Integer> neighbours = new ArrayList<>();
        for(List<Float> object:data){
            if(distance(object, data.get(index))<=epsilon){
                neighbours.add(data.indexOf(object));
            }
        }
        return neighbours;
    }

    private void expandNeighbours(List<Integer> neighbors, List<List<Float>> data, int[] labels, int C, float epsilon, int minPoints){
        for (int i=0; i<neighbors.size(); i++) {
            int object = neighbors.get(i);
            if (labels[object] == -1) {
                labels[object] = C;
            }
            if (labels[object] != 0) {
                continue;
            }
            labels[object] = C;
            List<Integer> newNeighbors = rangeQuery(object, data, epsilon);
            if (newNeighbors.size() >= minPoints) {
                neighbors.addAll(newNeighbors);
            }
        }
    }

    void cluster(List<List<Float>> data, List<Integer> indexes, String filename){
        float epsilon = getEpsilon(data, 6);
        int minPoints = 20, C=0;
        int[]labels = new int[data.size()];

        for(int i=0; i<data.size(); i++) {
            if (labels[i] != 0)
                continue;
            List<Integer> neighbors = rangeQuery(i, data, epsilon);
            //System.out.println(neighbors.size());
            if (neighbors.size() >= minPoints) {
                C += 1;
                labels[i] = C;
                expandNeighbours(neighbors, data, labels, C, epsilon, minPoints);
            } else{
                labels[i]=-1;
            }
        }
        List<Integer> clusterIndexes = new ArrayList<>();
        for(int i=0; i<labels.length; i++)
            clusterIndexes.add(labels[i]);
        System.out.println("Rand Index: " +new Index().randIndex(clusterIndexes, indexes));
        new FileWriter(filename+"1").write(data, clusterIndexes);
    }
}
