import java.io.File;
import java.util.Scanner;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.Queue;
import java.util.LinkedList;
import java.util.ListIterator;
import java.awt.Point;
import java.util.HashSet;
import java.util.*;
import java.lang.Math;

class homework{
    
    public static class MyPoint extends Point{
        private MyPoint parent;
        
        public MyPoint(int x, int y, MyPoint input){
            super(x,y);
            parent = input;
        }
        
        public MyPoint getParent(){
            return parent;       
        }   
    }
    
    public static class SecondPoint extends Point{
        private SecondPoint parent;
        private int path;
        
        public SecondPoint(int x, int y, SecondPoint input, int pathInput){
            super(x,y);
            parent = input;
            path = pathInput;
        }
        
        public void updatePath(int pathInput){
            path = pathInput;
        }
        
        public void updateParent(SecondPoint input){
            parent = input;
        }
        
        public int getPath(){
            return path;
        }
        
        public SecondPoint getParent(){
            return parent;
        }
    }
    
    public static class ThirdPoint extends Point{
        private ThirdPoint parent;
        private int path;
        private int score;
        
        public ThirdPoint(int x, int y, ThirdPoint input, int scoreInput, int pathInput){
            super(x,y);
            parent = input;
            path = pathInput;
            score = scoreInput;
        }
                
        public int getScore(){
            return score;
        }
        
        public ThirdPoint getParent(){
            return parent;
        }
        
        public int getPath(){
            return path;
        }        
    }
    
    //Comparator used in UCS
    public static class SecondPointComparator implements Comparator<SecondPoint>{
        public int compare(SecondPoint a, SecondPoint b){
            //Used for sorting in ascending order of the paths
            return a.getPath() - b.getPath();
        }
    }
    
    //Comparator used in A*
    public static class ThirdPointComparator implements Comparator<ThirdPoint>{
        public int compare(ThirdPoint a, ThirdPoint b){
            //Used for sorting in ascending order of the paths
            return a.getScore() - b.getScore();
        }
    }
    
    public static void main(String args[]) throws Exception{
        File file = new File("input.txt");
        Scanner sc = new Scanner(file);
        
        // Reading input.txt
        
        /* 
           First line
           Instruction of which program algorithm to use: BFS, UCS or A*
        */
        String algorithm = "NULL";
        if(sc.hasNextLine()){
            algorithm = sc.nextLine();
        }
        
        /* 
           Second line
           Two strictly positive 32-bit integers separated by one space character,
           for "W H" the number of columns (width) and rows (height), in cells,
           of the map.
        */
        int width = 0;
        int height = 0;
        if(sc.hasNextLine()){
            width = Integer.parseInt(sc.next());
            height = Integer.parseInt(sc.next());
        }
        
        /*
            Third line
            Two positive 32-bit integers separated by one space character, for "X Y"
            the coordinates (in cells) of the landing site. 0 <= X <= W-1 and
            0 <= Y <= H-1
        */
        int xCoordinate = 0;
        int yCoordinate = 0;
        if(sc.hasNextLine()){
            xCoordinate = Integer.parseInt(sc.next());
            yCoordinate = Integer.parseInt(sc.next());
        }
        
        /*
            Fourth line
            Positive 32-bit integer number for the maximum difference in elevation between
            two adjacent cells which the rover can drive over.
        */
        int maxSlope = 0;
        if(sc.hasNextLine()){
            maxSlope = Integer.parseInt(sc.next());
        }
        
        /*
            Fifth line
            Strictly positive 32-bit integer N, the number of target sites.
        */
        int numTargets = 0;
        if(sc.hasNextLine()){
            numTargets = Integer.parseInt(sc.next());
        }
        
        /*
            Next N lines
            Two positive 32-bit integer separated by one space character,
            for "X Y" the coordinates (in cells) of each target site.
            0 <= X <= W-1 and 0 <= Y <= H-1
        */
        // I use the counter to avoid getting Z elevation values
        int counter = 0;
        LinkedHashSet<Point> targetSet = new LinkedHashSet<>();
        while(sc.hasNextLine() && counter != numTargets){
            int pointX = Integer.parseInt(sc.next());
            int pointY = Integer.parseInt(sc.next());
            targetSet.add(new Point(pointX, pointY));
            counter++;
        }
        
        /*
            Next H lines
            W 32-bit integer numbers separated by any numbers of spaces
            for the elevation (Z) values of each of the W cells in each
            row of the map.
        */
        int[][] zElevation = new int[height][width];
        int firstIndex= 0;
        while(sc.hasNextLine() && firstIndex != height){
            for(int subArray = 0; subArray < width; subArray++){
                zElevation[firstIndex][subArray] = Integer.parseInt(sc.next());
            }
            firstIndex++;
        }
        
        //String algoInput = algorithm.remove(0);
        String firstAlgo = "BFS";
        String secondAlgo = "UCS";     
        String thirdAlgo = "A*";
        
        //This is the visited array to know where the rover has been before
        boolean[][] visited = new boolean[height][width];
        
        //Initializing the visited array
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                visited[i][j] = false;
            }
        }
        
        int checkOne = firstAlgo.compareTo(algorithm.toUpperCase());
        if(checkOne == 0){
            MyPoint landingSite = new MyPoint(yCoordinate, xCoordinate, null);
            
            //Call Method BFS
            HashSet<MyPoint> resultSet = bfs(landingSite, targetSet, visited, zElevation, maxSlope, width, height, numTargets);
            
            //This part is to check if BFS found a path from landing site to target
            //for every point in targetSet
            
            //This is a map to check whether the targets were found
            LinkedHashMap<Point, Boolean> hMap = new LinkedHashMap<>();
            
            Iterator<Point> it = targetSet.iterator();
            //Initializing the HashMap with the target values
            while(it.hasNext()){
                Point curr = it.next();
                hMap.put(new Point((int)curr.getX(),(int)curr.getY()), false);
            }
            
            for(Point goal: targetSet){
                Iterator<MyPoint> checker = resultSet.iterator();
                while(checker.hasNext()){
                    MyPoint curr = checker.next();
                    //Check if the target sites are in the resulSet
                    if(goal.getX()==curr.getY() && goal.getY() == curr.getX()){
                        hMap.replace(new Point((int)goal.getX(),(int)goal.getY()), true);
                        
                    }
                }
            }

            try{
                FileOutputStream outputFile = new FileOutputStream("output.txt");
                            
                for(Map.Entry<Point, Boolean> entry: hMap.entrySet()){
                    if(entry.getValue()){
                       //Print out the path
                        Point currentMap = entry.getKey();
                        //Check the Value in the Result Set

                        //Iterate over the resultSet
                        Iterator<MyPoint> setIt = resultSet.iterator();
                        while(setIt.hasNext()){
                            MyPoint currSet = setIt.next();
                            if(currentMap.getX() == currSet.getY() && currentMap.getY() == currSet.getX()){
                                MyPoint traceBackNode = currSet;
                                Stack<String> finalStack = new Stack<>();
                                while(traceBackNode != null){
                                    //Create a Stack of Points - Last One In, First One Out
                                    finalStack.push((int)traceBackNode.getY() + "," + (int)traceBackNode.getX() + " ");
                                    //Update traceBackNode to parent
                                    traceBackNode = traceBackNode.getParent();
                                }
                                String text = "";
                                while(!finalStack.empty()){
                                    text += finalStack.pop();
                                }
                                outputFile.write(text.getBytes());
                                text = "\n";
                                outputFile.write(text.getBytes());
                            }
                        }
                    }
                    else{
                        //Print "FAIL"
                        String text = "FAIL";
                        outputFile.write(text.getBytes());
                        text = "\n";
                        outputFile.write(text.getBytes());
                    }
                }
                outputFile.close();
            }
            catch(Exception e){
                System.out.println(e);
            }

        }
        
        int checkTwo = secondAlgo.compareTo(algorithm.toUpperCase());
        if(checkTwo == 0){
            SecondPoint landingSite = new SecondPoint(yCoordinate, xCoordinate, null, 0);
            
            //Call Method UCS
            HashSet<SecondPoint> resultSet = ucs(landingSite, visited, zElevation, maxSlope, width, height);
                        
            //This is a map to check whether the targets were found
            LinkedHashMap<Point, Boolean> hMap = new LinkedHashMap<>();
                       
            Iterator<Point> it = targetSet.iterator();
            //Initializing the HashMap with the target values
            while(it.hasNext()){
                Point curr = it.next();
                hMap.put(new Point((int)curr.getX(),(int)curr.getY()), false);
            }
            
            for(Point goal: targetSet){
                Iterator<SecondPoint> checker = resultSet.iterator();
                while(checker.hasNext()){
                    SecondPoint curr = checker.next();
                    //Check if the Target Sites are in the resulSet
                    if(goal.getX()==curr.getY() && goal.getY() == curr.getX()){
                        hMap.replace(new Point((int)goal.getX(),(int)goal.getY()), true);
                    }
                }
            }
            try{
                FileOutputStream outputFile = new FileOutputStream("output.txt");
                            
                for(Map.Entry<Point, Boolean> entry: hMap.entrySet()){
                    if(entry.getValue()){
                       //Print Out the Path
                        Point currentMap = entry.getKey();
                        //Check the Value in the Result Set

                        //Iterate over the resultSet
                        Iterator<SecondPoint> setIt = resultSet.iterator();
                        while(setIt.hasNext()){
                            SecondPoint currSet = setIt.next();
                            if(currentMap.getX() == currSet.getY() && currentMap.getY() == currSet.getX()){
                                SecondPoint traceBackNode = currSet;
                                Stack<String> finalStack = new Stack<>();
                                while(traceBackNode != null){
                                    //Create a Stack of Points - Last One In, First One Out
                                    finalStack.push((int)traceBackNode.getY() + "," + (int)traceBackNode.getX() + " ");
                                    //Update traceBackNode to parent
                                    traceBackNode = traceBackNode.getParent();
                                }
                                String text = "";
                                while(!finalStack.empty()){
                                    text += finalStack.pop();
                                }
                                outputFile.write(text.getBytes());
                                text = "\n";
                                outputFile.write(text.getBytes());
                            }
                        }
                    }
                    else{
                        //Print "FAIL"
                        String text = "FAIL";
                        outputFile.write(text.getBytes());
                        text = "\n";
                        outputFile.write(text.getBytes());
                    }
                }
                outputFile.close();
            }
            catch(Exception e){
                System.out.println(e);
            }            
        }
                
        int checkThree = thirdAlgo.compareTo(algorithm.toUpperCase());
        if(checkThree == 0){            
            //Call Method A* Search
            HashSet<ThirdPoint> resultSet = aSearch(yCoordinate, xCoordinate, visited, zElevation, maxSlope, width, height, numTargets, targetSet);
            
            //This is a map to check whether the targets were found
            LinkedHashMap<Point, Boolean> hMap = new LinkedHashMap<>();
            
            Iterator<Point> it = targetSet.iterator();
            //Initalizing the HashMap with the target values
            while(it.hasNext()){
                Point curr = it.next();
                hMap.put(new Point((int)curr.getX(),(int)curr.getY()), false);
            }
            
            for(Point goal: targetSet){
                Iterator<ThirdPoint> checker = resultSet.iterator();
                while(checker.hasNext()){
                    ThirdPoint curr = checker.next();
                    //Check if the target sites are in the resulSet
                    if(goal.getX()==curr.getY() && goal.getY() == curr.getX()){
                        hMap.replace(new Point((int)goal.getX(),(int)goal.getY()), true);  
                    }
                }
            }
            
            try{
                FileOutputStream outputFile = new FileOutputStream("output.txt");
                            
                for(Map.Entry<Point, Boolean> entry: hMap.entrySet()){
                    if(entry.getValue()){
                        Point currentMap = entry.getKey();
                        //Iterate over the resultSet
                        Iterator<ThirdPoint> setIt = resultSet.iterator();
                        while(setIt.hasNext()){
                            ThirdPoint currSet = setIt.next();
                            if(currentMap.getX() == currSet.getY() && currentMap.getY() == currSet.getX()){
                                ThirdPoint traceBackNode = currSet;
                                Stack<String> finalStack = new Stack<>();
                                while(traceBackNode != null){
                                    //Create a Stack of Points - Last One In, First One Out
                                    finalStack.push((int)traceBackNode.getY() + "," + (int)traceBackNode.getX() + " ");
                                    //Update traceBackNode to parent
                                    traceBackNode = traceBackNode.getParent();
                                }
                                String text = "";
                                while(!finalStack.empty()){
                                    text += finalStack.pop();
                                }
                                outputFile.write(text.getBytes());
                                text = "\n";
                                outputFile.write(text.getBytes());
                            }
                        }
                    }
                    else{
                        //Print "FAIL"
                        String text = "FAIL";
                        outputFile.write(text.getBytes());
                        text = "\n";
                        outputFile.write(text.getBytes());
                    }
                }
                outputFile.close();
            }
            catch(Exception e){
                System.out.println(e);
            }

            
        }
    } //End of main function
    
    private static HashSet<MyPoint> bfs(MyPoint landingSite, LinkedHashSet<Point> targetSet, boolean[][] visited, int[][] zElevation, int maxSlope, int width, int height, int numTargets){
        Queue<MyPoint> queue = new LinkedList<>(); 
        queue.add(landingSite);
        int currentElevation = 0;
        int firstIndex = 0;
        int subArray = 0;
        int counter = 0;
        
        //This set contains all the points with its parent node
        HashSet<MyPoint> resultSet = new HashSet<>();
        
        while(!queue.isEmpty() && counter < numTargets){
            /*
                Now, I will add the neighbors to the queue if they meet
                the requirement of the maxSlope difference.
            */
            MyPoint curr = queue.poll();
            firstIndex = (int)curr.getX();
            subArray = (int)curr.getY();
            currentElevation = zElevation[firstIndex][subArray];
            visited[firstIndex][subArray] = true;
            
            if(targetSet.contains(new Point(firstIndex, subArray))){
                counter++;
            }
            
            //North
            if((firstIndex-1) >= 0 && visited[firstIndex-1][subArray] == false){
                int absDifference = Math.abs(zElevation[firstIndex-1][subArray] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new MyPoint(firstIndex-1, subArray, curr));
                    queue.add(new MyPoint(firstIndex-1, subArray, curr));
                }
            }

            //East
            if((subArray+1) < width && visited[firstIndex][subArray+1] == false){
                int absDifference = Math.abs(zElevation[firstIndex][subArray+1] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new MyPoint(firstIndex, subArray+1, curr));
                    queue.add(new MyPoint(firstIndex, subArray+1, curr));
                }
            }

            //West
            if((subArray-1) >= 0 && visited[firstIndex][subArray-1] == false){
                int absDifference = Math.abs(zElevation[firstIndex][subArray-1] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new MyPoint(firstIndex, subArray-1, curr));
                    queue.add(new MyPoint(firstIndex, subArray-1, curr));
                }
            }

            //South
            if((firstIndex+1) < height && visited[firstIndex+1][subArray] == false){
                int absDifference = Math.abs(zElevation[firstIndex+1][subArray] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new MyPoint(firstIndex+1, subArray, curr));
                    queue.add(new MyPoint(firstIndex+1, subArray, curr));
                }
            }

            //North-East
            if((firstIndex-1) >= 0 && (subArray+1) < width && visited[firstIndex-1][subArray+1] == false){
                int absDifference  = Math.abs(zElevation[firstIndex-1][subArray+1] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new MyPoint(firstIndex-1, subArray+1, curr));
                    queue.add(new MyPoint(firstIndex-1, subArray+1, curr));
                }
            }

            //North-West
            if((firstIndex-1) >= 0 && (subArray-1) >= 0 && visited[firstIndex-1][subArray-1] == false){
                int absDifference = Math.abs(zElevation[firstIndex-1][subArray-1] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new MyPoint(firstIndex-1, subArray-1, curr));
                    queue.add(new MyPoint(firstIndex-1, subArray-1, curr));
                }
            }

            //South-East
            if((firstIndex+1) < height && (subArray+1) < width && visited[firstIndex+1][subArray+1] == false){
                int absDifference = Math.abs(zElevation[firstIndex+1][subArray+1] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new MyPoint(firstIndex+1, subArray+1, curr));
                    queue.add(new MyPoint(firstIndex+1, subArray+1, curr));
                }
            }

            //South-West
            if((firstIndex+1) < height && (subArray-1) >= 0 && visited[firstIndex+1][subArray-1] == false){
                int absDifference = Math.abs(zElevation[firstIndex+1][subArray-1] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new MyPoint(firstIndex+1, subArray-1, curr));
                    queue.add(new MyPoint(firstIndex+1, subArray-1, curr));
                }
            }
             
        }
        return resultSet;
    }
    
    private static HashSet<SecondPoint> ucs(SecondPoint landingSite, boolean[][] visited, int[][] zElevation, int maxSlope, int width, int height){
        PriorityQueue<SecondPoint> queue = new PriorityQueue<SecondPoint>(5, new SecondPointComparator()); 
        queue.add(landingSite);
        int currentElevation = 0;
        int currentPath = 0;
        int firstIndex = 0;
        int subArray = 0;
        
        //Path 2D Matrix
        int[][] pathArray = new int[height][width];
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                //Initializing all the values to zero
                pathArray[i][j] = 0;
            }
        }
        
        //This set contains all the points with its parent node
        HashSet<SecondPoint> resultSet = new HashSet<>();
        
        while(!queue.isEmpty()){    
            
            /*
                Now, I will add the neighbors to the queue if the meet
                the requirement of the maxSlope difference.
            */
            SecondPoint curr = queue.poll();
            firstIndex = (int)curr.getX();
            subArray = (int)curr.getY();
            currentElevation = zElevation[firstIndex][subArray];
            currentPath = curr.getPath();
            visited[firstIndex][subArray] = true;
            
            //North
            if((firstIndex-1) >= 0 && visited[firstIndex-1][subArray] == false){
                int absDifference = Math.abs(zElevation[firstIndex-1][subArray] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new SecondPoint(firstIndex-1, subArray, curr, currentPath+10));
                    queue.add(new SecondPoint(firstIndex-1, subArray, curr, currentPath+10));
                    pathArray[firstIndex-1][subArray] = currentPath+10;
                }
            }
            //North, but I will check if there is a shorter path
            else if((firstIndex-1) >= 0 && visited[firstIndex-1][subArray] && currentPath > (pathArray[firstIndex-1][subArray]+10)){
                
                //Iterate over the resultSet
                Iterator<SecondPoint> it = resultSet.iterator();
                while(it.hasNext()){
                    SecondPoint currSet = it.next();
                    //Replacing the parent of the current node
                    if(currSet.getX() == (firstIndex-1) && currSet.getY() == subArray){
                        curr.updateParent(currSet);
                        curr.updatePath(pathArray[firstIndex-1][subArray]+10);
                    }             
                }   
            }

            //East
            if((subArray+1) < width && visited[firstIndex][subArray+1] == false){
                int absDifference = Math.abs(zElevation[firstIndex][subArray+1] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new SecondPoint(firstIndex, subArray+1, curr, currentPath+10));
                    queue.add(new SecondPoint(firstIndex, subArray+1, curr, currentPath+10));
                    pathArray[firstIndex][subArray+1] = currentPath+10;
                }
            }
            //East, but I will check if there is a shorter path
            else if((subArray+1) < width && visited[firstIndex][subArray+1] && currentPath > (pathArray[firstIndex][subArray+1]+10)){
                
                //Iterate over the resultSet
                Iterator<SecondPoint> it = resultSet.iterator();
                while(it.hasNext()){
                    SecondPoint currSet = it.next();
                    //Replacing the parent of the current node
                    if(currSet.getX() == firstIndex && currSet.getY() == (subArray+1)){
                        curr.updateParent(currSet);
                        curr.updatePath(pathArray[firstIndex][subArray+1]+10);
                    }
                }
            }

            //West
            if((subArray-1) >= 0 && visited[firstIndex][subArray-1] == false){
                int absDifference = Math.abs(zElevation[firstIndex][subArray-1] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new SecondPoint(firstIndex, subArray-1, curr, currentPath+10));
                    queue.add(new SecondPoint(firstIndex, subArray-1, curr, currentPath+10));
                    pathArray[firstIndex][subArray-1] = currentPath+10;
                }
            }
            //West, but I will check if there is a shorter path
            else if((subArray-1) >= 0 && visited[firstIndex][subArray-1] && currentPath > (pathArray[firstIndex][subArray-1]+10)){
                
                //Iterate over the resultSet
                Iterator<SecondPoint> it = resultSet.iterator();
                while(it.hasNext()){
                    SecondPoint currSet = it.next();
                    //Replacing the parent of the current node
                    if(currSet.getX() == firstIndex && currSet.getY() == (subArray-1)){
                        curr.updateParent(currSet);
                        curr.updatePath(pathArray[firstIndex][subArray-1]+10);
                    }
                }
            }

            //South
            if((firstIndex+1) < height && visited[firstIndex+1][subArray] == false){
                int absDifference = Math.abs(zElevation[firstIndex+1][subArray] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new SecondPoint(firstIndex+1, subArray, curr, currentPath+10));
                    queue.add(new SecondPoint(firstIndex+1, subArray, curr, currentPath+10));
                    pathArray[firstIndex+1][subArray] = currentPath+10;
                }
            }
            //South, but I will check if there is a shorter path
            else if((firstIndex+1) < height && visited[firstIndex+1][subArray] && currentPath > (pathArray[firstIndex+1][subArray]+10)){
                
                //Iterate over the resultSet
                Iterator<SecondPoint> it = resultSet.iterator();
                while(it.hasNext()){
                    SecondPoint currSet = it.next();
                    //Replacing the parent of the current mode
                    if(currSet.getX() == (firstIndex+1) && currSet.getY() == subArray){
                        curr.updateParent(currSet);
                        curr.updatePath(pathArray[firstIndex+1][subArray]+10);
                    }
                }
            }

            //North-East
            if((firstIndex-1) >= 0 && (subArray+1) < width && visited[firstIndex-1][subArray+1] == false){
                int absDifference  = Math.abs(zElevation[firstIndex-1][subArray+1] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new SecondPoint(firstIndex-1, subArray+1, curr, currentPath+14));
                    queue.add(new SecondPoint(firstIndex-1, subArray+1, curr, currentPath+14));
                    pathArray[firstIndex-1][subArray+1] = currentPath+14;
                }
            }
            //North-East, but I will check if there is a shorter path
            else if((firstIndex-1) >= 0 && (subArray+1) < width && visited[firstIndex-1][subArray+1] && currentPath > (pathArray[firstIndex-1][subArray+1]+14)){
                
                //Iterate over the resultSet
                Iterator<SecondPoint> it = resultSet.iterator();
                while(it.hasNext()){
                    SecondPoint currSet = it.next();
                    //Replacing the parent of the current node
                    if(currSet.getX() == (firstIndex-1) && currSet.getY() == (subArray+1)){
                        curr.updateParent(currSet);
                        curr.updatePath(pathArray[firstIndex-1][subArray+1]+14);
                    }
                }        
            }

            //North-West
            if((firstIndex-1) >= 0 && (subArray-1) >= 0 && visited[firstIndex-1][subArray-1] == false){
                int absDifference = Math.abs(zElevation[firstIndex-1][subArray-1] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new SecondPoint(firstIndex-1, subArray-1, curr, currentPath+14));
                    queue.add(new SecondPoint(firstIndex-1, subArray-1, curr, currentPath+14));
                    pathArray[firstIndex-1][subArray-1] = currentPath+14;
                }
            }
            //North-West, but I will check if there is a shorter path
            else if((firstIndex-1) >= 0 && (subArray-1) >= 0 && visited[firstIndex-1][subArray-1] && currentPath > (pathArray[firstIndex-1][subArray-1]+14)){
                
                //Iterate over the resultSet
                Iterator<SecondPoint> it = resultSet.iterator();
                while(it.hasNext()){
                    SecondPoint currSet = it.next();
                    //Replacing the parent of the current node
                    if(currSet.getX() == (firstIndex-1) && currSet.getY() == (subArray-1)){
                        curr.updateParent(currSet);
                        curr.updatePath(pathArray[firstIndex-1][subArray-1]+14);
                    }
                }
            }

            //South-East
            if((firstIndex+1) < height && (subArray+1) < width && visited[firstIndex+1][subArray+1] == false){
                int absDifference = Math.abs(zElevation[firstIndex+1][subArray+1] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new SecondPoint(firstIndex+1, subArray+1, curr, currentPath+14));
                    queue.add(new SecondPoint(firstIndex+1, subArray+1, curr, currentPath+14));
                    pathArray[firstIndex+1][subArray+1] = currentPath+14;
                }
            }
            //South-East, but I will check if there is a shorter path
            else if((firstIndex+1) < height && (subArray+1) < width && visited[firstIndex+1][subArray+1] && currentPath > (pathArray[firstIndex+1][subArray+1]+14)){
                
                //Iterate over the resultSet
                Iterator<SecondPoint> it = resultSet.iterator();
                while(it.hasNext()){
                    SecondPoint currSet = it.next();
                    //Replacing the parent of the current node
                    if(currSet.getX() == (firstIndex+1) && currSet.getY() == (subArray+1)){
                        curr.updateParent(currSet);
                        curr.updatePath(pathArray[firstIndex+1][subArray+1]+14);
                    }
                }
            }

            //South-West
            if((firstIndex+1) < height && (subArray-1) >= 0 && visited[firstIndex+1][subArray-1] == false){
                int absDifference = Math.abs(zElevation[firstIndex+1][subArray-1] - currentElevation);
                if(absDifference <= maxSlope){
                    resultSet.add(new SecondPoint(firstIndex+1, subArray-1, curr, currentPath+14));
                    queue.add(new SecondPoint(firstIndex+1, subArray-1, curr, currentPath+14));
                    pathArray[firstIndex+1][subArray-1] = currentPath+14;
                }
            }
            //South-West, but I will check if there is a shorter path
            else if((firstIndex+1) < height && (subArray-1) >= 0 && visited[firstIndex+1][subArray-1] && currentPath > (pathArray[firstIndex+1][subArray-1]+14)){
                
                //Iterate over the resultSet
                Iterator<SecondPoint> it = resultSet.iterator();
                while(it.hasNext()){
                    SecondPoint currSet = it.next();
                    //Replacing the parent of the current node
                    if(currSet.getX() == (firstIndex+1) && currSet.getY() == (subArray-1)){
                        curr.updateParent(currSet);
                        curr.updatePath(pathArray[firstIndex+1][subArray-1]+14);
                    }
                }
            }
        }
        return resultSet;
    }
    
    private static HashSet<ThirdPoint> aSearch(int yCoordinate, int xCoordinate, boolean[][] visited, int[][] zElevation, int maxSlope, int width, int height, int numTargets, LinkedHashSet<Point> targetSet){
        PriorityQueue<ThirdPoint> queue = new PriorityQueue<ThirdPoint>(5, new ThirdPointComparator());
        //Creating the heuristic measure
        int[][] heuristicArray = new int[height][width];
        
        int heuristicCount = 0;
        
        for(Point goal: targetSet){
            for(int i = 0; i < height; i++){
                for(int j = 0; j < width; j++){
                    
                    int distanceOne = Math.abs((int)goal.getY()-i);
                    int distanceTwo = Math.abs((int)goal.getX()-j);
                    
                    double squaredOne = Math.pow(distanceOne, 2);
                    double squaredTwo = Math.pow(distanceTwo, 2);
                    
                    int distance = (int)Math.sqrt(squaredOne+squaredTwo);
                    
                    //Initializing the values in the array with the heuristic to find the first target node
                    if(heuristicCount == 0){
                        heuristicArray[i][j] = distance;
                    }
                    //Adding heuristics for the following target nodes
                    else{
                        if(distance < heuristicArray[i][j]){
                            //Replacing the value of the array with the distance to a differente target node
                            heuristicArray[i][j] = distance;
                        }
                    }
                }
            }
            heuristicCount++;
        }
        
        ThirdPoint landingSite = new ThirdPoint(yCoordinate, xCoordinate, null, heuristicArray[yCoordinate][xCoordinate], 0);
        
        queue.add(landingSite);
        int currentElevation = 0;
        int currentPath = 0;
        int firstIndex = 0;
        int subArray = 0;
        int counter = 0;
        
        //This set contains all the points with its parent node
        HashSet<ThirdPoint> resultSet = new HashSet<>();
        
        while(!queue.isEmpty() && counter < numTargets){
            /*
                Now, I will add the neighbors to the queue if they meet
                the requirement of the maxSlope difference.
            */
            
            ThirdPoint curr = queue.poll();
            firstIndex = (int)curr.getX();
            subArray = (int)curr.getY();
            currentElevation = zElevation[firstIndex][subArray];
            currentPath = curr.getPath();
            visited[firstIndex][subArray] = true;
            
            if(targetSet.contains(new Point(firstIndex, subArray))){
                counter++;
            }
            
            //North
            if((firstIndex-1) >= 0 && visited[firstIndex-1][subArray] == false){
                int absDifference = Math.abs(zElevation[firstIndex-1][subArray] - currentElevation);
                if(absDifference <= maxSlope){
                    int gFunc = currentPath+10+absDifference;
                    resultSet.add(new ThirdPoint(firstIndex-1, subArray, curr, heuristicArray[firstIndex-1][subArray]+gFunc, gFunc));
                    queue.add(new ThirdPoint(firstIndex-1, subArray, curr, heuristicArray[firstIndex-1][subArray]+gFunc, gFunc));
                }
            }
            
            //East
            if((subArray+1) < width && visited[firstIndex][subArray+1] == false){
                int absDifference = Math.abs(zElevation[firstIndex][subArray+1] - currentElevation);
                if(absDifference <= maxSlope){
                    int gFunc = currentPath+10+absDifference;
                    resultSet.add(new ThirdPoint(firstIndex, subArray+1, curr, heuristicArray[firstIndex][subArray+1]+gFunc, gFunc));
                    queue.add(new ThirdPoint(firstIndex, subArray+1, curr, heuristicArray[firstIndex][subArray+1]+gFunc, gFunc));
                }
            }
            
           //West
            if((subArray-1) >= 0 && visited[firstIndex][subArray-1] == false){
                int absDifference = Math.abs(zElevation[firstIndex][subArray-1] - currentElevation);
                if(absDifference <= maxSlope){
                    int gFunc = currentPath+10+absDifference;
                    resultSet.add(new ThirdPoint(firstIndex, subArray-1, curr, heuristicArray[firstIndex][subArray-1]+gFunc, gFunc));
                    queue.add(new ThirdPoint(firstIndex, subArray-1, curr, heuristicArray[firstIndex][subArray-1]+gFunc, gFunc));
                }
            } 
            
            //South
            if((firstIndex+1) < height && visited[firstIndex+1][subArray] == false){
                int absDifference = Math.abs(zElevation[firstIndex+1][subArray] - currentElevation);
                if(absDifference <= maxSlope){
                    int gFunc = currentPath+10+absDifference;
                    resultSet.add(new ThirdPoint(firstIndex+1, subArray, curr, heuristicArray[firstIndex+1][subArray]+gFunc, gFunc));
                    queue.add(new ThirdPoint(firstIndex+1, subArray, curr, heuristicArray[firstIndex+1][subArray]+gFunc, gFunc));
                }
            }
            
            //North-East
            if((firstIndex-1) >= 0 && (subArray+1) < width && visited[firstIndex-1][subArray+1] == false){
                int absDifference  = Math.abs(zElevation[firstIndex-1][subArray+1] - currentElevation);
                if(absDifference <= maxSlope){
                    int gFunc = currentPath+14+absDifference;
                    resultSet.add(new ThirdPoint(firstIndex-1, subArray+1, curr, heuristicArray[firstIndex-1][subArray+1]+gFunc, gFunc));
                    queue.add(new ThirdPoint(firstIndex-1, subArray+1, curr, heuristicArray[firstIndex-1][subArray+1]+gFunc, gFunc));
                }
            }
            
            //North-West
            if((firstIndex-1) >= 0 && (subArray-1) >= 0 && visited[firstIndex-1][subArray-1] == false){
                int absDifference = Math.abs(zElevation[firstIndex-1][subArray-1] - currentElevation);
                if(absDifference <= maxSlope){
                    int gFunc = currentPath+14+absDifference;
                    resultSet.add(new ThirdPoint(firstIndex-1, subArray-1, curr, heuristicArray[firstIndex-1][subArray-1]+gFunc, gFunc));
                    queue.add(new ThirdPoint(firstIndex-1, subArray-1, curr, heuristicArray[firstIndex-1][subArray-1]+gFunc, gFunc));
                }
            }
            
            //South-East
            if((firstIndex+1) < height && (subArray+1) < width && visited[firstIndex+1][subArray+1] == false){
                int absDifference = Math.abs(zElevation[firstIndex+1][subArray+1] - currentElevation);
                if(absDifference <= maxSlope){
                    int gFunc = currentPath+14+absDifference;
                    resultSet.add(new ThirdPoint(firstIndex+1, subArray+1, curr, heuristicArray[firstIndex+1][subArray+1]+gFunc, gFunc));
                    queue.add(new ThirdPoint(firstIndex+1, subArray+1, curr, heuristicArray[firstIndex+1][subArray+1]+gFunc, gFunc));
                }
            }
            
            //South-West
            if((firstIndex+1) < height && (subArray-1) >= 0 && visited[firstIndex+1][subArray-1] == false){
                int absDifference = Math.abs(zElevation[firstIndex+1][subArray-1] - currentElevation);
                if(absDifference <= maxSlope){
                    int gFunc = currentPath+14+absDifference;
                    resultSet.add(new ThirdPoint(firstIndex+1, subArray-1, curr, heuristicArray[firstIndex+1][subArray-1]+gFunc, gFunc));
                    queue.add(new ThirdPoint(firstIndex+1, subArray-1, curr, heuristicArray[firstIndex+1][subArray-1]+gFunc, gFunc));
                }
            } 
            
        }//End of while loop
        return resultSet;
    } 
}