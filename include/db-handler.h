/*
 * db-handler.h
 *
 *  Created on: Jul 7, 2014
 *      Author: namwkim85
 */

#ifndef DB_HANDLER_H_
#define DB_HANDLER_H_

int Insert(
		char* filename,
		char* DBFolder,
		char* categoryFolder
		);

int Delete(
		char* filename,
		char* DBFolder,
		char* categoryFolder
		);

int Update(
		char* DBFolder
		);

void ForAllImages(
		char* DBFolder, 	// For all images in the DB
		void (*f)(char*) // Execute the custom function
		);


#endif /* DB_HANDLER_H_ */

