// imageTest - A program that performs some image processing.
//
// This program is an example use of the image8bit module,
// a programming project for the course AED, DETI / UA.PT
//
// You may freely use and modify this code, NO WARRANTY, blah blah,
// as long as you give proper credit to the original and subsequent authors.
//
// João Manuel Rodrigues <jmr@ua.pt>
// 2023

#include <assert.h>
#include <errno.h>
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image8bit.h"
#include "instrumentation.h"

int main(int argc, char* argv[]) {
  
  /*
    !Testes Default:

    program_name = argv[0];
    if (argc != 3) {
      error(1, 0, "Usage: imageTest input.pgm input2.pgm");
    }

    ImageInit();
    
    printf("# LOAD image1");
    InstrReset(); // to reset instrumentation
    Image img1 = ImageLoad(argv[1]);
    if (img1 == NULL) {
      error(2, errno, "Loading %s: %s", argv[1], ImageErrMsg());
    }
    InstrPrint(); // to print instrumentation


    Try changing the behaviour of the program by commenting/uncommenting
    the appropriate lines.

    img2 = ImageCrop(img1, ImageWidth(img1)/4, ImageHeight(img1)/4, ImageWidth(img1)/2, ImageHeight(img1)/2);
    Image img2 = ImageRotate(img1);
    if (img2 == NULL) {
      error(2, errno, "Rotating img2: %s", ImageErrMsg());
    }

    ImageNegative(img2);
    ImageThreshold(img2, 100);
    ImageBrighten(img2, 1.3);

    if (ImageSave(img2, argv[2]) == 0) {
      error(2, errno, "%s: %s", argv[2], ImageErrMsg());
    }

    ImageDestroy(&img1);
    ImageDestroy(&img2);
    return 0;
  */

 // cl imageTest.c image8bit.c instrumentation.c error.c
 
  //Image img1 = ImageLoad("ComplexityTests/ImageLocateTests/Small/PurpleWindows.pgm");
  //Image img2 = ImageLoad("test/small.pgm");
  //InstrReset();
  //int pxTemp = 0;
	//int pyTemp = 0;
	//int* px = &pxTemp;
	//int* py = &pyTemp;
  //int success = ImageLocateSubImage(img1,px,py,img2);
  //if (success == 0) { error(2, errno, "LOCATE img: %s", ImageErrMsg()); }
  //InstrPrint();




/*
    program_name = argv[0];
    if (argc != 2) {
      error(1, 0, "Usage: imageTest input.pgm");
    }

    ImageInit();
    printf("# LOAD image1");

    InstrReset(); // to reset instrumentation

    Image img1 = ImageLoad(argv[1]); //oad a raw PGM file, On success, a new image is returned.
    if (img1 == NULL) {
      error(2, errno, "Loading %s: %s", argv[1], ImageErrMsg());
    }
    InstrPrint(); // to print instrumentation

    InstrReset(); // to reset instrumentation

    Image img2 = ImageLoad("test/small.pgm");
    InstrPrint(); // to print instrumentation

    InstrReset(); // to reset instrumentation

    ImagePaste(img1, 50, 50, img2);  //Paste img2 into position (x, y) of img1.
    InstrPrint(); // to print instrumentation

    InstrReset(); // to reset instrumentation

    int pxTemp = 0;
    int pyTemp = 0;
    int* px = &pxTemp;
    int* py = &pyTemp;
    ImageLocateSubImage(img1,px,py,img2); // Locate a subimage inside another image.
    InstrPrint(); // to print instrumentation

    ImageDestroy(&img1); // Destroy the image pointed to by (*imgp).
    ImageDestroy(&img2); // Destroy the image pointed to by (*imgp).
*/
  

  /*
    program_name = argv[0];
  if (argc != 2) {
    error(1, 0, "Usage: imageTest input.pgm");
  }

  ImageInit();
  printf("# LOAD image1");
  Image img1 = ImageLoad(argv[1]); 
  if (img1 == NULL) {
    error(2, errno, "Loading %s: %s", argv[1], ImageErrMsg());
  }

  InstrReset(); // to reset instrumentation
  ImageBlur(img1, 400, 400);
  InstrPrint();

  ImageDestroy(&img1); // Destroy the image pointed to by (*imgp).
  */
}

