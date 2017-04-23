rm -f *.class *.jar
javac Hw2Part1.java
jar cfm Hw2Part1.jar WordCount-manifest.txt Hw2Part1*.class
hdfs dfs -rm -f -r /hw2/output
hadoop jar ./Hw2Part1.jar /hw2/input_0 /hw2/output
hdfs dfs -cat '/hw2/output/part-*'

