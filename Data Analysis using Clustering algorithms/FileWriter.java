package Project;

import java.io.PrintWriter;
import java.util.List;

public class FileWriter {
    PrintWriter writer;

    FileWriter(String fileName){
        try {
            writer = new PrintWriter(fileName);
        }catch(Exception e){}
    }

    void write(List<List<Float>> data, List<Integer> indexes){
        int i=0;
        for(List<Float> object:data){
            StringBuilder buffer = new StringBuilder();
            for(float num:object){
                buffer.append(Float.toString(num));
                buffer.append("\t");
            }
            buffer.append(indexes.get(i));
            writer.println(buffer.toString());
            i+=1;
        }
        writer.close();
    }
}
