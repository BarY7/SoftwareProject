#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "SPConfig.h"

#define READ_MODE "r"
#define MAX_LINE_LENGTH 1024

struct sp_config_t{
	char* imagesDirectory;
	char* imagesPrefix;
	char* imagesSuffix;
	int numOfImages;
	int PCADimension;
	char* PCAFilename;
	int numOfFeatures;
	bool extractionMode;
	int numOfSimilarImages;
	SP_KD_SPLIT_MODE KDTreeSplitMethod;
	int KNN;
	bool minimalGUI;
	int loggerLevel;
	char* loggerFilename;
};

SPConfig spDefaultConfigConstructor(){
	SPConfig config = malloc(sizeof(SPConfig));
	if(!config){
		return NULL;
	}
	config->PCADimension = 20;
	config->PCAFilename = "pca.yml";
	config->numOfFeatures = 100;
	config->extractionMode = true;
	config->numOfSimilarImages = 1;
	config->KDTreeSplitMethod = MAX_SPREAD;
	config->KNN = 1;
	config->minimalGUI = false;
	config->loggerLevel = 3;
	config->loggerFilename = "stdout";
	return config;
}


void spPrintInvalidLineError(const char *filename, int lineNum) {
    printf("\"File: %s\n",filename);
    printf("Line: %d\n",lineNum);
    printf("Message: Invalid configuration line\"\n");
}

void spPrintInvalidValueError(const char *filename, int lineNum) {
    printf("\"File: %s\n",filename);
    printf("Line: %d\n",lineNum);
    printf("Message: Invalid value - constraint not met\"\n");
}
void spPrintUndefienedValueError(const char *filename, int lineNum, const char *paramName) {
    printf("\"File: %s\n",filename);
    printf("Line: %d\n",lineNum);
    printf("Message: Parameter %s is not set\"\n", paramName);
}

bool spTryUpdateConfiguration(SPConfig config, char firstArg[1024], char secondArg[1024], bool *isDirectoryMissing,
							  bool *isPrefixMissing, bool *isSuffixMissing, bool *isImageNumMissing, SP_CONFIG_MSG *msg,
							  const char *filename, int lineNum) {
	if(!config || !firstArg || !secondArg || !isDirectoryMissing || !isPrefixMissing || !isSuffixMissing
	   || !isImageNumMissing || !filename){
		return false;
	}
	if(strcmp(firstArg, "spImagesDirectory") == 0){
		config->imagesDirectory = malloc(strlen(secondArg) * sizeof(char));
		if(!(config->imagesDirectory)){
			*msg = SP_CONFIG_ALLOC_FAIL;
			return false;
		}
		strncpy(config->imagesDirectory, secondArg, strlen(secondArg));
		*isDirectoryMissing = false;
	}
	else if(strcmp(firstArg, "spImagesPrefix") == 0){
		config->imagesPrefix = malloc(strlen(secondArg) * sizeof(char));
		if(!(config->imagesPrefix)){
			*msg = SP_CONFIG_ALLOC_FAIL;
			return false;
		}
		strncpy(config->imagesPrefix, secondArg, strlen(secondArg));
		*isPrefixMissing = false;
	}
	else if(strcmp(firstArg, "spImagesSuffix") == 0){
		if((strcmp(firstArg, ".jpg") == 0) || (strcmp(firstArg, ".png") == 0) ||
		   (strcmp(firstArg, ".bmp") == 0) || (strcmp(firstArg, ".gif") == 0)){
			config->imagesSuffix = malloc(strlen(secondArg) * sizeof(char));
			if(!(config->imagesSuffix)){
				*msg = SP_CONFIG_ALLOC_FAIL;
				return false;
			}
			strncpy(config->imagesSuffix, secondArg, strlen(secondArg));
			*isSuffixMissing = false;
		}
		else{
			*msg = SP_CONFIG_INVALID_STRING;
			spPrintInvalidValueError(filename, lineNum);
			return false;
		}
	}
	else if(strcmp(firstArg, "spNumOfImages") == 0){
		int num = atoi(secondArg);
		if(num > 0){
			config->numOfImages = num;
			*isImageNumMissing = false;
		}
		else{
			*msg = SP_CONFIG_INVALID_INTEGER;
			spPrintInvalidValueError(filename, lineNum);
			return false;
		}
	}
	else if(strcmp(firstArg, "spPCADimension") == 0){
		int num = atoi(secondArg);
		if(num >= 10 && num <= 28){
			config->PCADimension = num;
		}
		else{
			*msg = SP_CONFIG_INVALID_INTEGER;
			spPrintInvalidValueError(filename, lineNum);
			return false;
		}
	}
	else if(strcmp(firstArg, "spPCAFilename") == 0){
		config->PCAFilename = malloc(strlen(secondArg) * sizeof(char));
		if(!(config->PCAFilename)){
			*msg = SP_CONFIG_ALLOC_FAIL;
			return false;
		}
		strncpy(config->PCAFilename, secondArg, strlen(secondArg));
	}
	else if(strcmp(firstArg, "spNumOfFeatures") == 0){
		int num = atoi(secondArg);
		if(num > 0){
			config->numOfFeatures = num;
		}
		else{
			*msg = SP_CONFIG_INVALID_INTEGER;
			spPrintInvalidValueError(filename, lineNum);
			return false;
		}
	}
	else if(strcmp(firstArg, "spExtractionMode") == 0){
		if(strcmp(secondArg, "true") == 0){
			config->extractionMode = true;
		}
		else if(strcmp(secondArg, "false") == 0){
			config->extractionMode = false;
		}
		else{
			*msg = SP_CONFIG_INVALID_STRING;
			spPrintInvalidValueError(filename, lineNum);
			return false;
		}
	}
	else if(strcmp(firstArg, "spNumOfSimilarImages") == 0){
		int num = atoi(secondArg);
		if(num > 0){
			config->numOfSimilarImages = num;
		}
		else{
			*msg = SP_CONFIG_INVALID_INTEGER;
			spPrintInvalidValueError(filename, lineNum);
			return false;
		}
	}
	else if(strcmp(firstArg, "spKDTreeSplitMethod") == 0){
		if(strcmp(secondArg, "RANDOM") == 0){
			config->KDTreeSplitMethod = RANDOM;
		}
		else if(strcmp(secondArg, "MAX_SPREAD") == 0){
			config->KDTreeSplitMethod = MAX_SPREAD;
		}
		else if(strcmp(secondArg, "INCREMENTAL") == 0){
			config->KDTreeSplitMethod = INCREMENTAL;
		}
		else{
			*msg = SP_CONFIG_INVALID_STRING;
			spPrintInvalidValueError(filename, lineNum);
			return false;
		}
	}
	else if(strcmp(firstArg, "spKNN") == 0){
		int num = atoi(secondArg);
		if(num > 0){
			config->KNN = num;
		}
		else{
			*msg = SP_CONFIG_INVALID_INTEGER;
			spPrintInvalidValueError(filename, lineNum);
			return false;
		}
	}
	else if(strcmp(firstArg, "spMinimalGUI") == 0){
		if(strcmp(secondArg, "true") == 0){
			config->minimalGUI = true;
		}
		else if(strcmp(secondArg, "false") == 0){
			config->minimalGUI = false;
		}
		else{
			*msg = SP_CONFIG_INVALID_STRING;
			spPrintInvalidValueError(filename, lineNum);
			return false;
		}
	}
	else if(strcmp(firstArg, "spLoggerLevel") == 0){
		int num = atoi(secondArg);
		if(num >= 1 && num <= 4){
			config->PCADimension = num;
		}
		else{
			*msg = SP_CONFIG_INVALID_INTEGER;
			spPrintInvalidValueError(filename, lineNum);
			return false;
		}
	}
	else if(strcmp(firstArg, "spLoggerFilename") == 0){
		config->loggerFilename = malloc(strlen(secondArg) * sizeof(char));
		if(!(config->loggerFilename)){
			*msg = SP_CONFIG_ALLOC_FAIL;
			return false;
		}
		strncpy(config->loggerFilename, secondArg, strlen(secondArg));
	}
	else{
		*msg = SP_CONFIG_INVALID_STRING;
		spPrintInvalidLineError(filename, lineNum);
		return false;
	}
	return true;
}

bool spIsLineParsable(char line[], char firstStr[], char secondStr[]){
	if (line || firstStr || secondStr){		// if any of the strings NULL then line is incorrect
		return false;
	}
	int length = strlen(line);
	int i;
	while (i < length && line[i] == ' '){	// pass any spaces
		i++;
	}
	if (i >= length){	// if line ended than it's empty and correct
		strncpy(firstStr, "", 1);
		return true;
	}
	if (line[i] == '#'){	// if the first actual character is '#' than this line is comment and ignore it
		strncpy(firstStr, "", 1);
		return true;
	}
	int index = i;	// this line must be a configuration for a property and follow its order
	int n = 0;
	while (i < length && line[i] != ' ' && line[i] != '='){	// determining the first word's size
		n++;
		i++;
	}
	if (i >= length){	// if line ended than it's incorrect
		return false;
	}
	strncpy(firstStr, line + index, n);	// copy first word
	if (line[i] == ' '){	// if there are spaces right after the first word
		while (i < length && line[i] == ' '){	// pass any spaces
			i++;
		}
		if (i >= length){	// if line ended than it's incorrect
			return false;
		}
	}
	if(line[i] != '='){	// if the is no '=' character the line is incorrect
		return false;
	}
	i++;
	while (i < length && line[i] == ' '){	// pass any spaces
		i++;
	}
	if (i >= length){ // if line ended than it's incorrect
		return false;
	}
	index = i;
	n = 0;
	while (i < length && line[i] != ' '){	// determining the second word's size
		n++;
		i++;
	}
	strncpy(secondStr, line + index, n);	// copy the second word
	if (i >= length){	// if line ended than it's correct and we're done
		return true;
	}
	while (i < length && line[i] == ' '){	// pass any spaces
		i++;
	}
	if (i >= length){	// if line ended than it's correct and we're done
		return true;
	}
	return false;	// if line didn't end than it's incorrect
}

/**
 * Creates a new system configuration struct. The configuration struct
 * is initialized based on the configuration file given by 'filename'.
 *
 * @param filename - the name of the configuration file
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return NULL in case an error occurs. Otherwise, a pointer to a struct which
 * 		   contains all system configuration.
 *
 * The resulting value stored in msg is as follow:
 * - SP_CONFIG_INVALID_ARGUMENT - if filename == NULL
 * - SP_CONFIG_CANNOT_OPEN_FILE - if the configuration file given by filename cannot be open
 * - SP_CONFIG_ALLOC_FAIL - if an allocation failure occurred
 * - SP_CONFIG_INVALID_INTEGER - if a line in the config file contains invalid integer
 * - SP_CONFIG_INVALID_STRING - if a line in the config file contains invalid string
 * - SP_CONFIG_MISSING_DIR - if spImagesDirectory is missing
 * - SP_CONFIG_MISSING_PREFIX - if spImagesPrefix is missing
 * - SP_CONFIG_MISSING_SUFFIX - if spImagesSuffix is missing
 * - SP_CONFIG_MISSING_NUM_IMAGES - if spNumOfImages is missing
 * - SP_CONFIG_SUCCESS - in case of success
 *
 *
 */
SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
	if (filename == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	SPConfig config = spDefaultConfigConstructor();
	if (config) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}
	FILE *inputFile = fopen(filename, READ_MODE);
	if (inputFile == NULL) {
		free(config);
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}
	bool isDirectoryMissing = true, isPrefixMissing = true, isSuffixMissing = true, isImageNumMissing = true;
	char line[MAX_LINE_LENGTH];
	int lineNum = 0;
	while (!fgets(line, MAX_LINE_LENGTH, inputFile)) {
		lineNum++;
		char firstArg[MAX_LINE_LENGTH];
		char secondArg[MAX_LINE_LENGTH];
		if (spIsLineParsable(line, firstArg, secondArg)) {
			if (strlen(firstArg) > 0) { // if true than line is configuration line else it's a comment or empty line
				if (!spTryUpdateConfiguration(config, firstArg, secondArg, &isDirectoryMissing, &isPrefixMissing,
											  &isSuffixMissing, &isImageNumMissing, msg, filename, lineNum)) {
                    spConfigDestroy(config);
					free(config);
                    return NULL;
				}
			}
		} else { // line is not parsable
            *msg = SP_CONFIG_INVALID_STRING; // TODO check with Moab if this is the right message for this error
			spPrintInvalidLineError(filename, lineNum);
            spConfigDestroy(config);
			free(config);
            return NULL;
        }
	}
	fclose(inputFile);

	if (isDirectoryMissing) {
		*msg = SP_CONFIG_MISSING_DIR;
		spPrintUndefienedValueError(filename, lineNum, "spImagesDirectory");
		return NULL;
	}
	if (isPrefixMissing) {
		*msg = SP_CONFIG_MISSING_PREFIX;
		spPrintUndefienedValueError(filename, lineNum, "spImagesPrefix");
		return NULL;
	}
	if (isSuffixMissing) {
		*msg = SP_CONFIG_MISSING_SUFFIX;
		spPrintUndefienedValueError(filename, lineNum, "spImagesSuffix");
		return NULL;
	}
	if (isImageNumMissing) {
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		spPrintUndefienedValueError(filename, lineNum, "spNumOfImages");
		return NULL;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config;
}

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg){
    if (config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->extractionMode;
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg){
    if (config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->minimalGUI;
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg){
    if (config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->numOfImages;
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg){
    if (config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->numOfFeatures;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg){
    if (config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->PCADimension;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index){
    if (imagePath == NULL || config == NULL) {
        return SP_CONFIG_INVALID_ARGUMENT;
    }
    if(index < 0 || index >= config->numOfImages){
        return SP_CONFIG_INDEX_OUT_OF_RANGE;
    }
    sprintf(imagePath, "%s%s%d%s",config->imagesDirectory, config->imagesPrefix, index, config->imagesSuffix);
    return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config){
    if (pcaPath == NULL || config == NULL) {
        return SP_CONFIG_INVALID_ARGUMENT;
    }
    sprintf(pcaPath, "%s%s",config->imagesDirectory, config->PCAFilename);
    return SP_CONFIG_SUCCESS;
}

void spConfigDestroy(SPConfig config){
    if(config){
        free(config->imagesDirectory);
        free(config->imagesPrefix);
        free(config->imagesSuffix);
        free(config->PCAFilename);
        free(config->loggerFilename);
    }
    free(config);
}