package Project;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

public class FileReader {

    List<Integer> groundTruth;

    List<Integer> getGroundTruth(){
        return groundTruth;
    }

    private void addGenesToList(String line, List<List<Float>> list){
        String[] words = line.split("\\t");
        List<Float> temp = new ArrayList<>();
        groundTruth.add(Integer.parseInt(words[1])-1);
        for(int i=2; i<words.length; i++){
            temp.add(Float.parseFloat(words[i]));
        }
        list.add(temp);
    }

    List<List<Float>> readFile(String fileName) throws IOException {
        List<List<Float>> data = new ArrayList<>();
        groundTruth = new ArrayList<>();
        BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(fileName),"UTF-8"));
        try {
            String line = "";
            while (line != null) {
                line = br.readLine();
                if(line != null)
                    addGenesToList(line, data);
            }
        }finally{
            br.close();
        }
        return data;
    }
}
