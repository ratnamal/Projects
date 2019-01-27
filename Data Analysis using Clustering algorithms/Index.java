package Project;

import java.util.List;

public class Index {

    private int[][] incidenceMatrix(List<Integer> indexes){
        int[][] incidenceMatrix = new int[indexes.size()][indexes.size()];
        for(int i=0; i<incidenceMatrix.length; i++){
            for(int j=0; j<incidenceMatrix.length; j++){
                if(indexes.get(i).equals(indexes.get(j)))
                    incidenceMatrix[i][j]=1;
                else
                    incidenceMatrix[i][j]=0;
            }
        }
        return incidenceMatrix;
    }

    float randIndex(List<Integer> clusterIndexes, List<Integer> groundTruth){
        int[][] C = incidenceMatrix(clusterIndexes);
        int[][] P = incidenceMatrix(groundTruth);
        int agree=0, disagree=0;
        for(int i=0; i<C.length; i++){
            for(int j=0; j<C.length; j++){
                if(C[i][j]==P[i][j])
                    agree+=1;
                else
                    disagree+=1;
            }
        }
        float total = (agree+disagree);
        float randIndex = agree/total;
        return randIndex;
    }
}
