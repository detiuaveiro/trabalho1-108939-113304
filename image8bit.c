/// image8bit - A simple image processing module.
///
/// This module is part of a programming project
/// for the course AED, DETI / UA.PT
///
/// You may freely use and modify this code, at your own risk,
/// as long as you give proper credit to the original and subsequent authors.
///
/// João Manuel Rodrigues <jmr@ua.pt>
/// 2013, 2023

// Student authors (fill in below):
// NMec: 113304  Name: Duarte Rainho dos Santos
// NMec: 108689  Name: Gabriel Janicas da Silva
// NMec: 108939  Name: Rodrigo Miguel Barros Moço 
// 
// Date:
//

#include "image8bit.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "instrumentation.h"

// The data structure
//
// An image is stored in a structure containing 3 fields:
// Two integers store the image width and height.
// The other field is a pointer to an array that stores the 8-bit gray
// level of each pixel in the image.  The pixel array is one-dimensional
// and corresponds to a "raster scan" of the image from left to right,
// top to bottom.
// For example, in a 100-pixel wide image (img->width == 100),
//   pixel position (x,y) = (33,0) is stored in img->pixel[33];
//   pixel position (x,y) = (22,1) is stored in img->pixel[122].
// 
// Clients should use images only through variables of type Image,
// which are pointers to the image structure, and should not access the
// structure fields directly.

// Maximum value you can store in a pixel (maximum maxval accepted)
const uint8 PixMax = 255;

// Internal structure for storing 8-bit graymap images
struct image {
  int width;
  int height;
  int maxval;   // maximum gray value (pixels with maxval are pure WHITE)
  uint8* pixel; // pixel data (a raster scan)
};


// This module follows "design-by-contract" principles.
// Read `Design-by-Contract.md` for more details.

/// Error handling functions

// In this module, only functions dealing with memory allocation or file
// (I/O) operations use defensive techniques.
// 
// When one of these functions fails, it signals this by returning an error
// value such as NULL or 0 (see function documentation), and sets an internal
// variable (errCause) to a string indicating the failure cause.
// The errno global variable thoroughly used in the standard library is
// carefully preserved and propagated, and clients can use it together with
// the ImageErrMsg() function to produce informative error messages.
// The use of the GNU standard library error() function is recommended for
// this purpose.
//
// Additional information:  man 3 errno;  man 3 error;

// Variable to preserve errno temporarily
static int errsave = 0;

// Error cause
static char* errCause;

/// Error cause.
/// After some other module function fails (and returns an error code),
/// calling this function retrieves an appropriate message describing the
/// failure cause.  This may be used together with global variable errno
/// to produce informative error messages (using error(), for instance).
///
/// After a successful operation, the result is not garanteed (it might be
/// the previous error cause).  It is not meant to be used in that situation!
char* ImageErrMsg() { ///
  return errCause;
}


// Defensive programming aids
//
// Proper defensive programming in C, which lacks an exception mechanism,
// generally leads to possibly long chains of function calls, error checking,
// cleanup code, and return statements:
//   if ( funA(x) == errorA ) { return errorX; }
//   if ( funB(x) == errorB ) { cleanupForA(); return errorY; }
//   if ( funC(x) == errorC ) { cleanupForB(); cleanupForA(); return errorZ; }
//
// Understanding such chains is difficult, and writing them is boring, messy
// and error-prone.  Programmers tend to overlook the intricate details,
// and end up producing unsafe and sometimes incorrect programs.
//
// In this module, we try to deal with these chains using a somewhat
// unorthodox technique.  It resorts to a very simple internal function
// (check) that is used to wrap the function calls and error tests, and chain
// them into a long Boolean expression that reflects the success of the entire
// operation:
//   success = 
//   check( funA(x) != error , "MsgFailA" ) &&
//   check( funB(x) != error , "MsgFailB" ) &&
//   check( funC(x) != error , "MsgFailC" ) ;
//   if (!success) {
//     conditionalCleanupCode();
//   }
//   return success;
// 
// When a function fails, the chain is interrupted, thanks to the
// short-circuit && operator, and execution jumps to the cleanup code.
// Meanwhile, check() set errCause to an appropriate message.
// 
// This technique has some legibility issues and is not always applicable,
// but it is quite concise, and concentrates cleanup code in a single place.
// 
// See example utilization in ImageLoad and ImageSave.
//
// (You are not required to use this in your code!)


// Check a condition and set errCause to failmsg in case of failure.
// This may be used to chain a sequence of operations and verify its success.
// Propagates the condition.
// Preserves global errno!
static int check(int condition, const char* failmsg) {
  errCause = (char*)(condition ? "" : failmsg);
  return condition;
}


/// Init Image library.  (Call once!)
/// Currently, simply calibrate instrumentation and set names of counters.
void ImageInit(void) { ///
  InstrCalibrate();
  InstrName[0] = "pixmem";  // InstrCount[0] will count pixel array acesses
  // Name other counters here...
  InstrName[1] = "NumComparacoes";
  InstrName[2] = "NumOperacoes";
  
}

// Macros to simplify accessing instrumentation counters:
#define PIXMEM InstrCount[0]
// Add more macros here...

#define NUMCOMP InstrCount[1]
#define NUMOPERACOES InstrCount[2]

// TIP: Search for PIXMEM or InstrCount to see where it is incremented!


/// Image management functions

/// Create a new black image.
///   width, height : the dimensions of the new image.
///   maxval: the maximum gray level (corresponding to white).
/// Requires: width and height must be non-negative, maxval > 0.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageCreate(int width, int height, uint8 maxval) { ///
  assert (width >= 0);
  assert (height >= 0);
  assert (0 < maxval && maxval <= PixMax);
  // Insert your code here! 
  Image img = (Image)malloc(sizeof(struct image)); // Alocação dinâmica na memória.
  
  if (img == NULL) {  // Em caso de erro:
    check((img != NULL), "Alocação de Memória falhou"); //    Mensagem de erro
    free(img);  //    Liberta de espaço da memória;
    return NULL;   //    Como falhou retorna NULL
  }

  img -> width = width;
  img -> height = height;
  img->maxval = maxval;

  // Alocação de memória para o array de pixels
  img->pixel = (uint8*)malloc(width * height * sizeof(uint8));
  if (img->pixel == NULL) {   // Em caso de erro:
    check((img->pixel != NULL), "AAlocação de memória para o data pixel falhou");  //      Mensagem de erro
    free(img);  //     liberamos a memória alocada para a estrutura Image
    return NULL;  //      Retorno NULL;
  }

  return img;  //Retorna uma nova imagem
}

/// Destroy the image pointed to by (*imgp).
///   imgp : address of an Image variable.
/// If (*imgp)==NULL, no operation is performed.
/// Ensures: (*imgp)==NULL.
/// Should never fail, and should preserve global errno/errCause.
void ImageDestroy(Image* imgp) { ///
  assert (imgp != NULL);
  // Insert your code here!
  free((*imgp)->pixel);  // Libera a memória alocada para o campo pixel
  free(*imgp);   // Desaloca bloco de memória, liberta o número de bits que foram solicitados quando foi alocado.
  *imgp = NULL; // Garantimos que (*imgp) é NULL
}


/// PGM file operations

// See also:
// PGM format specification: http://netpbm.sourceforge.net/doc/pgm.html

// Match and skip 0 or more comment lines in file f.
// Comments start with a # and continue until the end-of-line, inclusive.
// Returns the number of comments skipped.
static int skipComments(FILE* f) {
  char c;
  int i = 0;
  while (fscanf(f, "#%*[^\n]%c", &c) == 1 && c == '\n') {
    i++;
  }
  return i;
}

/// Load a raw PGM file.
/// Only 8 bit PGM files are accepted.
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageLoad(const char* filename) { ///
  int w, h;
  int maxval;
  char c;
  FILE* f = NULL;
  Image img = NULL;

  int success = 
  check( (f = fopen(filename, "rb")) != NULL, "Open failed" ) &&
  // Parse PGM header
  check( fscanf(f, "P%c ", &c) == 1 && c == '5' , "Invalid file format" ) &&
  skipComments(f) >= 0 &&
  check( fscanf(f, "%d ", &w) == 1 && w >= 0 , "Invalid width" ) &&
  skipComments(f) >= 0 &&
  check( fscanf(f, "%d ", &h) == 1 && h >= 0 , "Invalid height" ) &&
  skipComments(f) >= 0 &&
  check( fscanf(f, "%d", &maxval) == 1 && 0 < maxval && maxval <= (int)PixMax , "Invalid maxval" ) &&
  check( fscanf(f, "%c", &c) == 1 && isspace(c) , "Whitespace expected" ) &&
  // Allocate image
  (img = ImageCreate(w, h, (uint8)maxval)) != NULL &&
  // Read pixels
  check( fread(img->pixel, sizeof(uint8), w*h, f) == w*h , "Reading pixels" );
  PIXMEM += (unsigned long)(w*h);  // count pixel memory accesses

  // Cleanup
  if (!success) {
    errsave = errno;
    ImageDestroy(&img);
    errno = errsave;
  }
  if (f != NULL) fclose(f);
  return img;
}

/// Save image to PGM file.
/// On success, returns nonzero.
/// On failure, returns 0, errno/errCause are set appropriately, and
/// a partial and invalid file may be left in the system.
int ImageSave(Image img, const char* filename) { ///
  assert (img != NULL);
  int w = img->width;
  int h = img->height;
  uint8 maxval = img->maxval;
  FILE* f = NULL;

  int success =
  check( (f = fopen(filename, "wb")) != NULL, "Open failed" ) &&
  check( fprintf(f, "P5\n%d %d\n%u\n", w, h, maxval) > 0, "Writing header failed" ) &&
  check( fwrite(img->pixel, sizeof(uint8), w*h, f) == w*h, "Writing pixels failed" ); 
  PIXMEM += (unsigned long)(w*h);  // count pixel memory accesses

  // Cleanup
  if (f != NULL) fclose(f);
  return success;
}


/// Information queries

/// These functions do not modify the image and never fail.

/// Get image width
int ImageWidth(Image img) { ///
  assert (img != NULL);
  return img->width;
}

/// Get image height
int ImageHeight(Image img) { ///
  assert (img != NULL);
  return img->height;
}

/// Get image maximum gray level
int ImageMaxval(Image img) { ///
  assert (img != NULL);
  return img->maxval;
}

/// Pixel stats
/// Find the minimum and maximum gray levels in image.
/// On return,
/// *min is set to the minimum gray level in the image,
/// *max is set to the maximum.
void ImageStats(Image img, uint8* min, uint8* max) { ///
  assert (img != NULL);
  // Insert your code here!

  (*max) = 0;  // Definir um máximo temporário;
  (*min) = PixMax;  // Definir um minimo temporário;

  // Vamos percorrer todos os pixeis da image
  for (int i=0; i < (img->width * img->height); i++) {
    if ((*max) > img->pixel[i]) {
      (*max) = img->pixel[i];  // Procurar pelo nivel de gray max
    }

    if ((*min) < img->pixel[i]) {
      (*min) = img->pixel[i]; // Procurar pelo nivel de gray min
    }
  }
}

/// Check if pixel position (x,y) is inside img.
int ImageValidPos(Image img, int x, int y) { ///
  assert (img != NULL);
  return (0 <= x && x < img->width) && (0 <= y && y < img->height);
}

/// Check if rectangular area (x,y,w,h) is completely inside img.
int ImageValidRect(Image img, int x, int y, int w, int h) { ///
  assert (img != NULL);
  // Insert your code here!

  // Verificar se os limites não são menores de zero nem passam os limites da imagem
  return (x >= 0 && y >= 0 && w > 0 && h > 0 && (x+w) < img->width && (y+h) < img->height);
}

/// Pixel get & set operations

/// These are the primitive operations to access and modify a single pixel
/// in the image.
/// These are very simple, but fundamental operations, which may be used to 
/// implement more complex operations.

// Transform (x, y) coords into linear pixel index.
// This internal function is used in ImageGetPixel / ImageSetPixel. 
// The returned index must satisfy (0 <= index < img->width*img->height)
static inline int G(Image img, int x, int y) {
  int index;
  // Insert your code here!

  index = x + (y * img->width);  // Calculo do indice para as coordenadas (x, y);

  assert (0 <= index && index < img->width*img->height);  // Verificação se esse indice está dentro dos valores corretos
  return index;  //retorno do indice.
}

/// Get the pixel (level) at position (x,y).
uint8 ImageGetPixel(Image img, int x, int y) { ///
  assert (img != NULL);
  assert (ImageValidPos(img, x, y));
  PIXMEM += 1;  // count one pixel access (read)
  return img->pixel[G(img, x, y)];
} 

/// Set the pixel at position (x,y) to new level.
void ImageSetPixel(Image img, int x, int y, uint8 level) { ///
  assert (img != NULL);
  assert (ImageValidPos(img, x, y));
  PIXMEM += 1;  // count one pixel access (store)
  img->pixel[G(img, x, y)] = level;
} 


/// Pixel transformations

/// These functions modify the pixel levels in an image, but do not change
/// pixel positions or image geometry in any way.
/// All of these functions modify the image in-place: no allocation involved.
/// They never fail.


/// Transform image to negative image.
/// This transforms dark pixels to light pixels and vice-versa,
/// resulting in a "photographic negative" effect.
void ImageNegative(Image img) { ///
  assert (img != NULL);
  // Insert your code here!
  
  /*
    Para fazer o negativo da imagem, temos de percorrer todos os pixeis
    da imagem e depois ao valor máximo de intensidade (PixMax -> 255)
    subtraimos o valor atual da intensidade do pixel.
  */
  for(int i=0; i<img->width * img->height; i++) {
    img->pixel[i] = PixMax - img->pixel[i];
  }
}

/// Apply threshold to image.
/// Transform all pixels with level<thr to black (0) and
/// all pixels with level>=thr to white (maxval).
void ImageThreshold(Image img, uint8 thr) { ///
  assert (img != NULL);
  // Insert your code here!

  /* 
    Para aplicar o threshold, temos de percorrer todos os pixeis da imagem
    e verificar se o valor do pixel é menor que o threshold, caso seja, 
    o pixel fica preto, caso contrário fica branco.
  */
  for(int i=0; i<(img->width*img->height); i++) {

    uint8 level = img->pixel[i];

    if (level < thr) {
      img->pixel[i] = (uint8)0;
    } else {
      img->pixel[i] = PixMax;
    }

  }

}

/// Brighten image by a factor.
/// Multiply each pixel level by a factor, but saturate at maxval.
/// This will brighten the image if factor>1.0 and
/// darken the image if factor<1.0.
void ImageBrighten(Image img, double factor) { ///
  assert (img != NULL);
  assert (factor >= 0.0);
  // Insert your code here!

  /*  
    Para fazer o brighten da imagem, temos de percorrer todos os pixeis
    da imagem e depois multiplicar o valor do pixel pelo factor, caso o
    valor do pixel seja maior que o valor máximo de intensidade (PixMax -> 255)
    o pixel fica com o valor máximo de intensidade.
  */
  for (int i=0; i < (img->width * img->height); i++) {

    double newPixelValue = img->pixel[i] * factor;
    img->pixel[i] = (newPixelValue > PixMax) ? PixMax : (uint8)(newPixelValue+0.5);
  }

}


/// Geometric transformations

/// These functions apply geometric transformations to an image,
/// returning a new image as a result.
/// 
/// Success and failure are treated as in ImageCreate:
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.

// Implementation hint: 
// Call ImageCreate whenever you need a new image!

/// Rotate an image.
/// Returns a rotated version of the image.
/// The rotation is 90 degrees anti-clockwise.
/// Ensures: The original img is not modified.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly
Image ImageRotate(Image img) { ///
  assert (img != NULL);
  // Insert your code here!

  Image ImgR = ImageCreate(img->width, img->height, img->maxval); // Criar uma nova imagem com as dimensões trocadas

  /* 
    Para fazer a rotação da imagem, temos de percorrer todos os pixeis
    da imagem e depois trocar as coordenadas x e y, ou seja, a coordenada
    x passa a ser a coordenada y e a coordenada y passa a ser a coordenada x.
  */
  for(int y=0; y<img->height; y++) {
    for(int x=0; x<img->width; x++) {
      ImageSetPixel(ImgR, x, y, ImageGetPixel(img, (img->height - y - 1), x));
    }
  } return ImgR;
}

/// Mirror an image = flip left-right.
/// Returns a mirrored version of the image.
/// Ensures: The original img is not modified.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageMirror(Image img) { ///
  assert (img != NULL);
  // Insert your code here!

  Image ImgM = ImageCreate(img->height, img->width, img->maxval);   // Criar uma nova imagem com as dimensões trocadas

  /* 
    Para fazer o mirror da imagem, temos de percorrer todos os pixeis
    da imagem e depois trocar as coordenadas x e y, ou seja, a coordenada
    x passa a ser a coordenada y e a coordenada y passa a ser a coordenada x.
  */
  for(int y=0; y<img->height; y++) {
    for(int x=0; x<img->width; x++) {
      ImageSetPixel(ImgM, x, y, ImageGetPixel(img, img->width - x - 1, y));
    }
  } return ImgM;
}

/// Crop a rectangular subimage from img.
/// The rectangle is specified by the top left corner coords (x, y) and
/// width w and height h.
/// Requires:
///   The rectangle must be inside the original image.
/// Ensures:
///   The original img is not modified.
///   The returned image has width w and height h.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageCrop(Image img, int x, int y, int w, int h) { ///
  assert (img != NULL);
  assert (ImageValidRect(img, x, y, w, h));
  // Insert your code here!

  Image ImgC = ImageCreate(w, h, img->maxval);  // Criar uma nova imagem com as dimensões do retangulo

   /* 
    Para fazer o crop da imagem, temos de percorrer todos os pixeis
    da imagem e depois copiar os pixeis da imagem original para a nova
    imagem.
  */
  for(int i=0; i<w; i++) {
    for(int j=0; j<h; j++) {
      ImageSetPixel(ImgC, i, j, ImageGetPixel(img, x+i, y+j));
    }
  } return ImgC;
}


/// Operations on two images

/// Paste an image into a larger image.
/// Paste img2 into position (x, y) of img1.
/// This modifies img1 in-place: no allocation involved.
/// Requires: img2 must fit inside img1 at position (x, y).
void ImagePaste(Image img1, int x, int y, Image img2) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  assert (ImageValidRect(img1, x, y, img2->width, img2->height));
  // Insert your code here!

  /* 
    Para fazer o paste da imagem, temos de percorrer todos os pixeis
    da imagem e depois copiar os pixeis da imagem original para a nova
    imagem.
  */
  for(int i=0; i<img2->width; i++) {
    for(int j=0; j<img2->height; j++) {

      if ((x+i < img1->width) && (y + j < img1->height)) {
        ImageSetPixel(img1, i+x, j+y, ImageGetPixel(img2, i, j));
      }
    }
  }
}

/// Blend an image into a larger image.
/// Blend img2 into position (x, y) of img1.
/// This modifies img1 in-place: no allocation involved.
/// Requires: img2 must fit inside img1 at position (x, y).
/// alpha usually is in [0.0, 1.0], but values outside that interval
/// may provide interesting effects.  Over/underflows should saturate.
void ImageBlend(Image img1, int x, int y, Image img2, double alpha) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  assert (ImageValidRect(img1, x, y, img2->width, img2->height));
  // Insert your code here!

  double newPixel;  // Variável para guardar o novo valor do pixel

  /* 
    Para fazer o blend da imagem, temos de percorrer todos os pixeis
    da imagem e depois calcular o novo valor do pixel.
  */
  for(int i=0; i<img2->width; i++) {
    for(int j=0; j<img2->height; j++) {

      newPixel = (1 - alpha) * ImageGetPixel(img1, x + i, j + y) + alpha * ImageGetPixel(img2, i, j);
      newPixel = (newPixel < 0) ? 0 : ((newPixel > PixMax) ? PixMax : newPixel); 
      ImageSetPixel(img1, i + x, j + y, (uint8)(newPixel+0.5));
    }
  }

}

/// Compare an image to a subimage of a larger image.
/// Returns 1 (true) if img2 matches subimage of img1 at pos (x, y).
/// Returns 0, otherwise.
int ImageMatchSubImage(Image img1, int x, int y, Image img2) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  assert (ImageValidPos(img1, x, y));
  // Insert your code here!

  /* 
    Para fazer o match da imagem, temos de percorrer todos os pixeis
    da imagem e depois comparar os pixeis da imagem original com os pixeis
    da nova imagem.
  */
  for (int j = 0; j < img2->height; ++j) {
    for (int i = 0; i < img2->width; ++i) {
      uint8 pixel1 = ImageGetPixel(img1, x + i, y + j);
      uint8 pixel2 = ImageGetPixel(img2, i, j);
      NUMCOMP += 1;
      if (pixel1 != pixel2) {
        return 0;  // Mismatch found
      }
    }
  }

  return 1;  // Subimage matches
}

/// Locate a subimage inside another image.
/// Searches for img2 inside img1.
/// If a match is found, returns 1 and matching position is set in vars (*px, *py).
/// If no match is found, returns 0 and (*px, *py) are left untouched.
int ImageLocateSubImage(Image img1, int* px, int* py, Image img2) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  // Insert your code here!

  /* 
    Para fazer o locate da imagem, temos de percorrer todos os pixeis
    da imagem e depois comparar os pixeis da imagem original com os pixeis
    da nova imagem, esta comparação faz-se usando a função ImageMatchSubImage().
  */
  for(int i=0; i < (img1->width - img2->width); i++) {
    for(int j=0; j < (img1->height - img2->height); j++) {

      if (ImageMatchSubImage(img1, i, j, img2)) {
        *px = i;
        *py = j;
        return 1;
      }

    }
  } return 0;
}


/// Filtering

/// Blur an image by a applying a (2dx+1)x(2dy+1) mean filter.
/// Each pixel is substituted by the mean of the pixels in the rectangle
/// [x-dx, x+dx]x[y-dy, y+dy].
/// The image is changed in-place.
void ImageBlur(Image img, int dx, int dy) {
  /*
  
    ! Função Básica:

    assert(img != NULL);

    Image tempImg = ImageCreate(img->width, img->height, img->maxval);

    for(int x=0; x < img->width; x++) {
      for(int y=0; y < img->height; y++) {

        double sum = 0.0;
        int count = 0;

        for(int i=-dx; i < (dx+1); i++) {
          for(int j=-dy; j < (dy+1); j++) {

            if (ImageValidPos(img, i+x, j+y)) {
              sum += ImageGetPixel(img, i+x, j+y);
              count++;
            }
          }
        }

        double media = sum / count;

        ImageSetPixel(tempImg, x, y, (uint8)(media+0.5));

      }
    }

    for(int x=0; x < img->width; x++) {
      for(int y=0; y < img->height; y++) {
        ImageSetPixel(img, x, y, ImageGetPixel(tempImg, x, y));
      }
    } 

    ImageDestroy(&tempImg);
  
  */

  //! FUnção melhorada

  // Insert your code here!
  assert(img != NULL);  //Verificar se a imagem está NULL!

  int sum, minX, minY, maxX, maxY;

  /*
    Aqui etsamos a criar uma summed Area Table: Serve para acelerar o cálculo da soma dos
    valores de uma imagem para uma matriz bidimensional. Com a summed area table, é possível
    acelerar o processo tornando muito útil para algoritmos de blur.
  */
  int **summedAreaTable = (int **)malloc((img->width + 1) * sizeof(int *));
  for (int i = 0; i < img->width + 1; i++) {
    summedAreaTable[i] = (int *)malloc((img->height + 1) * sizeof(int));
  }

  // Preenchimento da summed area table (Funcionamento explicado no relatório)
  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {

      NUMCOMP += 3;

      sum = ImageGetPixel(img, x, y);

      if (x != 0) {
        sum += summedAreaTable[x-1][y]; NUMOPERACOES+=1; // Deve slatar a linha de indice 0
      }
      if (y != 0) {
        sum += summedAreaTable[x][y-1]; NUMOPERACOES+=1;   // Deve slatar a coluna de indice 0
      }
      if ((x != 0 && y != 0)) {
        sum -= summedAreaTable[x-1][y-1]; NUMOPERACOES+=1;  // Deve slatar a linha e a coluna de indice 0
      }

      summedAreaTable[x][y] = sum;  // Guarda o valor final na matriz.
    }
  }

  /*
    Depois de criada a summed area table, temos que iterar sobre cada pixel da imagem inicial, aplciando
    uma janela centrada no pixel de ((2 * dx + 1) x (2 * dy + 1)). Esta janela define a area a qual 
    será aplicado o blur. Para isso, neste loop apenas temos de calcular a média pois a soma desses valores
    já está guardada na summed area table. Por fim guardamos na imagem original no pixel o valor calculado.
  */
  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {

      NUMCOMP += 4;

      /*
        Este calculo do maxX, maxY, ... serve para definir a janela, temos der ter em atenção que a janela não pode sair 
        dos limites de frame, ou seja, o maxX não pode ser menor de 0, caso contrário estariamos fora do frame, o mesmo se
        aplica para os seguintes valores.
      */
      maxX = (x - dx - 1 > 0) ? x - dx - 1 : 0; 
      maxY = (y - dy - 1 > 0) ? y - dy - 1 : 0;

      minX = (x + dx + 1 > img->width) ? img->width - 1 : x + dx;
      minY = (y + dy + 1 > img->height) ? img->height - 1 : y + dy;
     
     //Em baixo vamos calcular a soma dos limites da janela.
      int sum = summedAreaTable[minX][minY] - summedAreaTable[maxX][minY] - summedAreaTable[minX][maxY] + summedAreaTable[maxX][maxY];
      NUMOPERACOES += 3;
      double mean = (double)(sum) / (((minX - maxX) * (minY - maxY)));  //Calculo da media

      ImageSetPixel(img, x, y, (int)(mean+0.5));  // Temos que acrescentar 0.5 à media para podermos ter arredondamentos corretos
    }
  }

  free(summedAreaTable[0]);  //Liberta a memória alocada para a tabela de soma
  free(summedAreaTable);

}