package Project;

import java.io.IOException;
import java.util.List;
import java.util.Scanner;

public class Main {

    public static void main(String[] args) throws IOException {
        FileReader reader = new FileReader();
        Scanner scanner = new Scanner(System.in);
        String fileName = scanner.nextLine();
        List<List<Float>> data = reader.readFile(fileName);
        System.out.println("Enter k:");
        int k = scanner.nextInt();

       KMeans kmeans = new KMeans();
       System.out.println("Max iterations: ");
       kmeans.MAX_ITERATIONS = scanner.nextInt();
       kmeans.cluster(data, reader.getGroundTruth(),k, fileName);


        HierarchialClustering hierarchialClustering = new HierarchialClustering();
        hierarchialClustering.cluster(data, reader.getGroundTruth(),  fileName);

        DBScan dbScan = new DBScan();
        dbScan.cluster(data, reader.groundTruth, fileName);
    }

}