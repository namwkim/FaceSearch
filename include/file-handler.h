/*
 * file-handler.h
 *
 *  Created on: Jul 25, 2014
 *      Author: namwkim85
 */

#ifndef FILE_HANDLER_H_
#define FILE_HANDLER_H_

#define FLEN 1024
#define	IMG_EXT 	".png"
#define DEPTH_EXT	".depth.png"
#define MASK_EXT	".mask.png"
#define FTR_EXT		".feature.xml"

int IsImageFile(char* filename);
const char *GetFileExt(const char *filename) ;
int Exists(const char *fname);
int FileCopy(const char* src, const char* dst);
void ForAllImages(
		char* folder, 	// For all images in the DB
		void (*f)(char*) // Execute the custom function
		);

#endif /* FILE_HANDLER_H_ */
