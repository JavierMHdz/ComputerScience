#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

int main(){
   //string line to read from text file
   string line;
   //Declare an input file stream(ifstream) variable.
   ifstream inFile;
   //Open the file
   inFile.open("data2Rx.txt");
   
   //if file is open
   if(inFile.is_open()){
      //get the line while it is reading input
      while(getline(inFile,line)){
         //to check if I am done reading the data
         bool data = false;
         //vector where I would be storing the data
         vector<int> dataVec;
         //vector where I would be storing the generator
         vector<int> generatorVec;
         
         //reading the data
         for(unsigned int i = 0; i < line.size(); i++){
            //if I am not done reading the data, keep going into this
            //condition bracket
            if(!data){
               if(line[i] == ' '){
                  data = true;
               }
               else{
                  //add to the vector
                  int x;
                  stringstream str;
                  str << line[i];
                  str >> x;
                  dataVec.push_back(x);
               }
            }
            //done reading the data, added to the generator vector
            else{
               //add it to the generator
               int x;
               stringstream str;
               str << line[i];
               str >> x;
               generatorVec.push_back(x);
            }
         }
         
         //getting the size of the data vector
         int nData = dataVec.size();
         //getting the size of the generator vector
         int nGenerator = generatorVec.size();
         //creating the data array
         int dataArray[nData];
         //creating the generator array
         int generatorArray[nGenerator];

         //storing the data into the dataArray
         for(int i = 0; i < nData; i++){
            dataArray[i] = dataVec[i];
         }
         
         //storing the data into the generatorArray
         for(int i = 0; i < nGenerator; i++){
            generatorArray[i] = generatorVec[i];
         }
         
         //starting and final position for each round in CRC 
         int startPos = 0;
         int lastPos = nGenerator;
         
         //flag to know when the CRC is done
         bool done = false;
         while(!done){
            for(int i = startPos; i < lastPos; i++){
               //XORing generator with data
               dataArray[i] = dataArray[i]^generatorArray[i-startPos];
            }

            bool firstOne = false;
            //Updating the starting position and last position for next iteration
            for(int i = startPos; i < lastPos; i++){
               //if the first one is found in sequence, update the start and final
               //position and break out of the for loop
               if(!firstOne && dataArray[i] == 1){
                  firstOne = true;
                  startPos = i;
                  lastPos = i + nGenerator;
                  break;
               }
            }

            //If all numbers are zero, update the start and final position
            if(!firstOne){
               startPos = lastPos;
               lastPos = lastPos + nGenerator; 
            }

            //check if we are done doing the CRC method
            if(lastPos > nData){
               done = true;
            } 

         }
         //check if there the CRC has an error and if it passes or not
         bool pass = true;
         for(int i = 0; i < nData; i++){
            if(dataArray[i] == 1){
               pass = false;
            }
         }

         if(pass){
            cout<<"pass"<<endl;
         }
         else{
            cout<<"Not pass"<<endl;
         }
      }
      //Close the input stream
      inFile.close();
   }
   else{
      cout<<"Unable to open file data2Rx.txt"<<endl;
   }   
   return 0;
}
