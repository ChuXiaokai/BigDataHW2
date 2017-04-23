rm -f *.class *.jar
echo 'rm'
javac Hw2Part1.java
echo 'compile.fuck'
jar cfm Hw2Part1.jar WordCount-manifest.txt Hw2Part1*.class
echo 'start run'
hdfs dfs -rm -f -r /hw2/output
hadoop jar ./Hw2Part1.jar /hw2/input_0 /hw2/output
echo 'cat'
hdfs dfs -cat '/hw2/output/part-*'

