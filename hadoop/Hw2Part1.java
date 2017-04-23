/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Modified by Shimin Chen to demonstrate functionality for Homework 2
// April-May 2015

import java.io.IOException;
import java.util.StringTokenizer;
import java.util.ArrayList;
import java.text.DecimalFormat; 

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.FloatWritable;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapred.TextInputFormat;
import org.apache.hadoop.mapred.TextOutputFormat;
import org.apache.hadoop.util.GenericOptionsParser;

public class Hw2Part1 {

  // This is the Mapper class
  // reference: http://hadoop.apache.org/docs/r2.6.0/api/org/apache/hadoop/mapreduce/Mapper.html
  //
  public static class TokenizerMapper 
       extends Mapper<Object, Text, Text, DoubleWritable>{  // input type

    private Text  user2user = new Text;
    private DoubleWritable time = new DoubleWritable(0);
      
    public void map(Object key, Text value, Context context
                    ) throws IOException, InterruptedException { 
      System.out.println("fuck");
      String line = value.toString();
      String[] lines = line.split("\n");
      for (String each_line: lines){
        String[] words = each_line.split(' ');
        for(int i = 0; i <= 3; i++){
          System.out.println(words[i]);
        }
        // judge if legal
        if(words.length == 3){
          user2user = words[0] + "?" + word[1];
          time = DoubleWritable(words[2]);
          context.write(user2user, time);
        }
      }
    }
  }
  
  public static class IntSumCombiner
       extends Reducer<Text[],DoubleWritable,Text[],ArrayList> {  // first and second param is the input from map
    private ArrayList result = new ArrayList();                   // 3rd and 4th is the output of reducer

    public void reduce(Text key, Iterable<DoubleWritable> values,  // input key: value
                       Context context
                       ) throws IOException, InterruptedException {
      double sum = 0.0;
      int num = 0;
      for (DoubleWritable val: values) {
        sum += val.get();
        num += 1;
      }
      result.add(num);
      result.add(result);
      // for (IntWritable val : values) {
      //   sum += val.get();
      // }
      // result.set(sum);
      context.write(key, result);
    }
  }

  // This is the Reducer class
  // reference http://hadoop.apache.org/docs/r2.6.0/api/org/apache/hadoop/mapreduce/Reducer.html
  //
  // We want to control the output format to look at the following:
  //
  // count of word = count
  //
  public static class IntSumReducer
       extends Reducer<Text,DoubleWritable,Text,DoubleWritable> {  // first and second param is the input from map
                                                                  // 3rd and 4th is the output of reducer
    private Text result_key= new Text();
    private Text result_value= new Text();
    private byte[] prefix;
    private byte[] mid;

    protected void setup(Context context) {  // what it for
      try {
        prefix= Text.encode("count of ").array();
        suffix= Text.encode(" =").array();
      } catch (Exception e) {
        prefix = suffix = new byte[0];
      }
    }

    public void reduce(Text key, Iterable<DoubleWritable> values, 
                       Context context
                       ) throws IOException, InterruptedException {
      // int sum = 0;
      // for (IntWritable val : values) {
      //   sum += val.get();
      // }
      double sum = 0.0;
      int num = 0;
      for (DoubleWritable val: values) {
        sum += val.get();
        num += 1;
      }
      double average = sum*1.0/(num*1.0);
      
      DecimalFormat df = new DecimalFormat("#.000");
      sum = df.format(sum);
      average = df.format(average);

      // generate result key
      // result_key: from-to-
      Text new_key = new Text(key + " " + Integer.toString(num) + " ");
      result_key.set(prefix);
      result_key.append(new_key.getBytes(), 0, new_key.getLength());
      result_key.append(suffix, 0, suffix.length);
      // gen result value
      context.write(new_key, DoubleWritable(average));
    }
  }

  public static void main(String[] args) throws Exception {
    Configuration conf = new Configuration();
    String[] otherArgs = new GenericOptionsParser(conf, args).getRemainingArgs();
    if (otherArgs.length < 2) {
      System.err.println("Usage: wordcount <in> [<in>...] <out>");
      System.exit(2);
    }

    Job job = Job.getInstance(conf, "word count");

    job.setJarByClass(Hw2Part1.class);

    System.out.println("jinrufauck");
    job.setMapperClass(TokenizerMapper.class);
  //  job.setCombinerClass(IntSumCombiner.class);
    job.setReducerClass(IntSumReducer.class);

    job.setMapOutputKeyClass(Text[].class);
    job.setMapOutputValueClass(DoubleWritable.class);

    job.setOutputKeyClass(Text.class);
    job.setOutputValueClass(Text.class);

    // add the input paths as given by command line
    for (int i = 0; i < otherArgs.length - 1; ++i) {
      FileInputFormat.addInputPath(job, new Path(otherArgs[i]));
    }

    // add the output path as given by the command line
    FileOutputFormat.setOutputPath(job,
      new Path(otherArgs[otherArgs.length - 1]));

    System.exit(job.waitForCompletion(true) ? 0 : 1); //
  }
}
