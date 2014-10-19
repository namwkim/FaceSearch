/*
 * face-detection.h
 *
 *  Created on: Jul 16, 2014
 *      Author: namwkim85
 */

#ifndef FACE_DETECTION_H_
#define FACE_DETECTION_H_

int DetectFaces( IplImage* img, IplImage* depth, CvSeq** faces, int foregrnd_on);

#endif /* FACE_DETECTION_H_ */
