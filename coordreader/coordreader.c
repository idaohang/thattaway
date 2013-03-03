#include <stdio.h>
#include <inttypes.h>
#include <endian.h>

//used to directly convert ints to floats and floats to ints
typedef union{
  uint32_t i;
  float f;
}intfloat_t;

//read the next value in from the EEPROM dump
//  FILE* file   - the file to read
//  float* val   - where to write the value that was read
//  returns char - 0 if the read failed, 1 if it succeeded
char read_next_val(FILE* file, float* val){
  char result = 0;
  intfloat_t temp;

  //read in 1 value from the file
  if( fread(&temp, sizeof(temp), 1, file) == 1 ){
    //we succeeded
    result = 1;
    //convert the AVR's little-endian encoding into whatever the host uses
    temp.i = le32toh(temp.i);
    //copy over the new value as a float
    *val = temp.f;
  }

  return result;
}

int main(int argc, char** argv){
  FILE* eepromfile = NULL;
  FILE* csvfile = NULL;
  int current_slot = 0;
  float current_lat = 0;
  float current_long = 0;
  char noerror = 1;

  //too few args?
  if( argc < 2 ){
    printf("syntax: %s <name of eeprom dump file> [name of output CSV file]\n",
           argv[0]);
  }
  else{
    //open input file
    eepromfile = fopen(argv[1], "r");
    if( eepromfile == NULL ){
      perror("Error opening input file");
    }
    else{
      //open the output file
      if( argc > 2 ){
        csvfile = fopen(argv[2], "w");
      }else{
        csvfile = fopen("coords.csv", "w");
      }
      if( csvfile == NULL ){
        perror("Error opening output file");
      }
      else{
        //write the column headings
        fprintf(csvfile, "slot,latitude,longitude\n");

        //write the data
        while(noerror){
          noerror &= read_next_val(eepromfile, &current_lat);
          noerror &= read_next_val(eepromfile, &current_long);
          fprintf(csvfile, "%d,%f,%f\n", current_slot,
                                         current_lat,
                                         current_long);
          current_slot++;
        }

        //close the files
        fclose(eepromfile);
        fclose(csvfile);
      }
    }
  }

  return 0;
}
