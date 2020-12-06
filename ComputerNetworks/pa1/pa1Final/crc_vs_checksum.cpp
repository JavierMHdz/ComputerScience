#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

//prototypes for functions I use to convert from binary
//to decimal and decimal to binary
int decimalNum(int);
int* binaryNum(int);

int main(){
   string line;
   //Declare an input file stream(ifstream) variable.
   ifstream inFile;
   //Open the file
   inFile.open("data2Vs.txt");
   //all the following variables are used for the F1 score of
   //CRC and Checksum
   int falsePositiveCRC = 0;
   int truePositiveCRC = 0;
   int trueNegativeCRC = 0;
   int falseNegativeCRC = 0;
   int falsePositiveChecksum = 0;
   int truePositiveChecksum = 0;
   int trueNegativeChecksum = 0;
   int falseNegativeChecksum = 0;
   int actualError = 0;
   int noError = 0;
   double f1CRC = 0;
   double f1Checksum = 0;
   double precisionCRC = 0;
   double recallCRC = 0;
   double precisionChecksum = 0;
   double recallChecksum = 0;
   
   //Check if the file opened properly
   if(inFile.is_open()){
      
      //while there are lines in the text file, keep reading
      while(getline(inFile,line)){
         //to check if I am done reading the data
         bool data = false;
         //to check if I am done reading the generator
         bool generator = false;
         //vector where I would be storing the data
         vector<int> dataVec;
         //vector where I would be storing the generator
         vector<int> generatorVec;
         //vector where I would be storing the error
         vector<int> errorVec;
         
         //reading the data
         for(unsigned int i = 0; i < line.size(); i++){
            //if I am not done reading the data, keep going into this
            //condition bracket
            if(!data && !generator){
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
            //this bracket is used when I am done reading the data, but
            //I am not done reading the generator
            else if(data && !generator){
               if(line[i] == ' '){
                  generator = true;
               }
               else{
                  //add to the vector
                  int x;
                  stringstream str;
                  str << line[i];
                  str >> x;
                  generatorVec.push_back(x);
               }
            }
            //this bracket is to used the error
            else{
               //add to the vector
               int x;
               stringstream str;
               str << line[i];
               str >> x;
               errorVec.push_back(x);
            }
         }
         
         //getting the size of the data vector
         int nData = dataVec.size();
         //getting the size of the generator vector
         int nGenerator = generatorVec.size();
         //getting the size of the error vector
         int nError = errorVec.size();
         //creating the data array
         int dataArrayCRC[nData+nGenerator-1];
         //creating the generator array
         int generatorArray[nGenerator];
         //creating the error array
         int errorArray[nError];

         //storing the data into the dataArray
         for(int i = 0; i < nData; i++){
            dataArrayCRC[i] = dataVec[i];
         }
         
         //Initializing the extra space to zero for now. It will be changed
         //eventually.
         for(int i = nData; i < (nData+nGenerator-1); i++){
            dataArrayCRC[i] = 0;
         }
         
         //storing the data into the generatorArray
         for(int i = 0; i < nGenerator; i++){
            generatorArray[i] = generatorVec[i];
         }

         //counting how many errors are found
         bool error = false;
         for(int i = 0; i < nError; i++){
            errorArray[i] = errorVec[i];
            if(errorArray[i] == 1){
               //error found in the array
               error = true;
            }
         }

         //adding those errors to the variable in the main program
         if(error){
            actualError++;
         }
         else{
            noError++;
         }
         
         //starting and final postion for each round in CRC    
         int startPos = 0;
         int lastPos = nGenerator;
          
         bool done = false;
         //Updating the starting position and last position for next iteration
         while(!done){
            for(int i = startPos; i < lastPos; i++){
               //XORing generator with data
               dataArrayCRC[i] = dataArrayCRC[i]^generatorArray[i-startPos];
            }

            bool firstOne = false;
            //Updating the starting position and last position for next iteration
            for(int i = startPos; i < lastPos; i++){
               //if the first one is found in sequence, update the start and final
               //position and break out the for loop
               if(!firstOne && dataArrayCRC[i] == 1){
                  firstOne = true;
                  startPos = i;
                  lastPos = i + nGenerator;
                  break;
               }
            }

            //If all numbers are zero
            if(!firstOne){
               startPos = lastPos;
               lastPos = lastPos + nGenerator; 
            }

            //check if we are done doing the CRC method
            if(lastPos > (nData+nGenerator-1)){
               done = true;
            } 

         }

         //update the array value back
         for(int i = 0; i < nData; i++){
            dataArrayCRC[i] = dataVec[i];
         }

         //Creating the array with all the bits
         int dataArrayChecksum[nData+nGenerator-1];
         for(int i = 0; i < nData; i++){
            //storing the values to the checksum array for data
            dataArrayChecksum[i] = dataVec[i];
         }

         //Converting from int array to string
         string dataString;
         for(int i = 0; i < nData; i++){
            //converting the array to a string
            dataString.push_back(dataArrayChecksum[i] + '0');
         }

         int binaryVal = 0;  
         //creating a stringstream object with the string
         stringstream tempDataTx(dataString);
         //adding the value of the stringstream to integer
         tempDataTx >> binaryVal;

         //get the decimal value
         int decimalVal = decimalNum(binaryVal);

         //Checksum starts
         int quotient = decimalVal/256; //2^8
         int remainder = decimalVal%256;
         int sum = quotient + remainder;
         //keep repeating until sum is less than 255
         while(sum > 255){
            quotient = sum/256; //2^8
            remainder = sum%256;
            sum = quotient+remainder;
         }
         //getting the 1's complement
         int onesComplement = 255-sum;

         //getting the binary value of the 1's complement
         int* flippedBinaryArrayTx;   
         flippedBinaryArrayTx = binaryNum(onesComplement);
         //Checksum ends


         //Add it to the array
         int counter = nData;
         for(int i = (nGenerator-2); i >= 0; i--){
            //adding the 1's complement to the data array
            dataArrayChecksum[counter] = flippedBinaryArrayTx[i];
            counter++;
         }

         //Now I will XOR the dataArrayCRC and dataArrayChecsum to introduce the error
         //This is the transmission
         for(int i = 0; i < (nData+nGenerator-1); i++){
            //adding the random errors bits, simulating transmission
            dataArrayCRC[i] = dataArrayCRC[i] ^ errorArray[i];
            dataArrayChecksum[i] = dataArrayChecksum[i] ^ errorArray[i];
         }

         //Check the resulting data with CRC and Checksum
         //First, I will check it with CRC

         //starting and final position of the CRC
         startPos = 0;
         lastPos = nGenerator;

         //flag to know when the CRC is done
         done = false;
         while(!done){
            for(int i = startPos; i < lastPos; i++){
               //XORing generator with data
               dataArrayCRC[i] = dataArrayCRC[i]^generatorArray[i-startPos];
            }

            bool firstOne = false;
            //Updating the starting position and last position for next iteration
            for(int i = startPos; i < lastPos; i++){
               //if the first one is found a sequence, update the start and final
               //position and break out the for loop
               if(!firstOne && dataArrayCRC[i] == 1){
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

            //check if we are done doing CRC method
            if(lastPos > (nData+nGenerator-1)){
               done = true;
            } 

         }

         //check if there are any errors in the CRC
         bool isErrorCRC = false;
         for(int i = 0; i < (nData+nGenerator-1); i++){
            if(dataArrayCRC[i] == 1){
               //if there are any errors then update the flag to true
               isErrorCRC = true;
            }
         }

         //counting the results in the main method varibles for F1 score
         if(isErrorCRC){
            if(error){
               trueNegativeCRC++;
            }
            else{
               falseNegativeCRC++;
            }
         }
         else{
            if(error){
               falsePositiveCRC++;
            }
            else{
               truePositiveCRC++;
            }
         }

         //Now, I will check it with Checksum
         //Converting from int array to string
         string dataStringError;
         //creating an array for the received checksum
         int receivedChecksum[nGenerator-1];
         //creating an array for the checksum calculated here
         int ownChecksum[nGenerator-1];

         //converting from integer array to string
         for(int i = 0; i < nData; i++){
            dataStringError.push_back(dataArrayChecksum[i] + '0');
         }

         //storing the received checksum into its array
         for(int i =  nData; i < (nData+nGenerator-1); i++){
            receivedChecksum[i-nData] = dataArrayChecksum[i];
         }

         binaryVal = 0; 
         //creating a stringstream object from the string
         stringstream tempDataRx(dataStringError);
         //converting the information in stringstream to integer
         tempDataRx >> binaryVal;

         //converting the binary number to decimal value
         decimalVal = decimalNum(binaryVal);

         //Checksum starts
         quotient = decimalVal/256; //2^8
         remainder = decimalVal%256;
         sum = quotient + remainder;
         //keep repeating until sum is less than 255
         while(sum > 255){
            quotient = sum/256; //2^8
            remainder = sum%256;
            sum = quotient+remainder;
         }

         //getting the 1's complement
         onesComplement = 255-sum;

         //getting the binary value of the 1's complement
         int* flippedBinaryArrayRx;
         flippedBinaryArrayRx = binaryNum(onesComplement);
         //Checksum ends

         //Add it to the ownChecksum
         counter = 0;
         for(int i = (nGenerator-2); i >= 0; i--){
            ownChecksum[counter] = flippedBinaryArrayRx[i];
            counter++;
         }

         //check if both checksums match, if not then update the error to true
         bool isErrorChecksum = false;
         for(int i = 0; i < (nGenerator-1); i++){
            if(receivedChecksum[i] != ownChecksum[i]){
               isErrorChecksum = true;
            }
         }

         //count the final result in the main method variables for the F1 score
         if(isErrorChecksum){
            if(error){
               trueNegativeChecksum++;
            }
            else{
               falseNegativeChecksum++;
            }
         }
         else{
            if(error){
               falsePositiveChecksum++;
            }
            else{
               truePositiveChecksum++;
            }
         }
      } //while loop ends here


      //First, I will calculate both precisions
      precisionCRC = (double)truePositiveCRC/(double)(truePositiveCRC+falsePositiveCRC);
      precisionChecksum = (double)truePositiveChecksum/(double)(truePositiveChecksum+falsePositiveChecksum);

      //Now, I will calculate both recalls
      recallCRC = (double)truePositiveCRC/(double)(truePositiveCRC+falseNegativeCRC);
      recallChecksum = (double)truePositiveChecksum/(double)(truePositiveChecksum+falseNegativeChecksum);

      //Finally, I will calculate the F1 score
      f1CRC = 2*((precisionCRC*recallCRC)/(precisionCRC+recallCRC));
      f1Checksum = 2*((precisionChecksum*recallChecksum)/(precisionChecksum+recallChecksum));

      cout<<"F1 score of 1-byte checksum:"<<f1Checksum<<endl;
      cout<<"F1 score of CRC:"<<f1CRC<<endl;

      //Close the input stream
      inFile.close();   
   }
   else{
      cout<<"Unable to open file data2Tx.txt"<<endl;
   }
   return 0;
}

//I used some ideas from www.geeksforgeeks.org to implement
//the following two functions: decimalNum() and binaryNum().
int decimalNum(int n){
   int binaryNum = n;
   int decimalNum = 0;
   int base = 1;

   int tempVal = binaryNum;
   //keep looping until tempVal reaches zero
   while(tempVal != 0){
      //get the mod 10 of the tempVal
      int lastDigit = tempVal%10;
      //divide the tempVal by 10
      tempVal = tempVal/10;

      //get the decimal value, update it every loop
      decimalNum = decimalNum + (lastDigit*base);

      //update the base number
      base = base * 2;
   }
   //return the decimal number to main function
   return decimalNum;
}

int* binaryNum(int number){
   //array to store binary number that I will return
   static int binaryArray[8];

   //index position in array, starting from last position
   int i = 0;
   //keep looping until number reaches zero
   while(number > 0){
      //get the mod 2 of number
      binaryArray[i] = number%2;
      //divide the number by two
      number = number/2;
      //update the counter
      i++;
   }
   //return the array to the main function
   return binaryArray;
}