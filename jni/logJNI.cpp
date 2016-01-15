#include <jni.h>
#include <android/log.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <regex.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define LOG_TAG    "LOG_JNI"
#define MI_SEC 1000000
#define FILE_NUM_MAX 50
#define FILE_SIZE_MAX 1024*200 // 200KB
#define MAX_TIME 9999999999999999
#define REGEX_PATTERN "^[0-9]\\{16\\}\\.log$"
#define FILE_NAME_PATTERN "%s/%lld.log"

#define DEBUG

// For info log
#ifdef 	  DEBUG
#define   JNILOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#else
#define   JNILOGI(...)
#endif

#define   JNILOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

/**
 * Get current time for name of log
 *
 */
long long getCurrentTime()
{
	struct timeval tms;
	gettimeofday(&tms, NULL);

	return (long long) tms.tv_sec * MI_SEC + tms.tv_usec;

}

/**
 * New the folder
 */
DIR* newFolder(const char* fileDir)
{
	if (NULL == fileDir || strlen(fileDir) <= 0)
	{
		JNILOGE("File is NULL!");
		return NULL;
	}

	DIR* fd = opendir(fileDir);
	if (NULL == fd)
	{
		JNILOGI("The folder do not exist, create it!");
		if (mkdir(fileDir, 0775) != 0)
		{
			JNILOGE("Create folder fail");
			return NULL;
		}
		fd = opendir(fileDir);
	}
	return fd;
}

/**
 * Delete one file
 */
void deleteFile(const char* file)
{
	if (NULL == file || strlen(file) <= 0)
	{
		JNILOGE("File is NULL!");
		return;
	}

	// delete
	if (remove(file) != 0)
	{
		JNILOGE("delete file [%s] fail", file);
	}
}

/**
 * Write to the full path
 */
void write(const char* fileFullPath, const char* content)
{
	if (NULL == fileFullPath || strlen(fileFullPath) <= 0)
	{
		JNILOGE("file full path is NULL!");
		return;
	}

	// Begin write
	JNILOGI("Begin write fileFullPath: %s", fileFullPath);
	FILE *fp = NULL;
	fp = fopen(fileFullPath, "a");
	if (NULL == fp)
	{
		JNILOGE("File open Fail!");
		return;
	}
	int ret = fprintf(fp, "%s\t", content);
	if (ret < 0)
	{
		JNILOGE("Write Fail!");
	}

	fclose(fp);
}

/**
 * Get folder is first and last file name.
 * Return the number of logs
 */
int getLogFiles(DIR* fd, long long & firstTime, long long & lastTime)
{
	// Compile the regex
	const char *regex = REGEX_PATTERN;
	regex_t reg;
	int compResult = regcomp(&reg, regex, REG_BASIC);
	if (0 != compResult)
	{
		char ebuf[129];
		regerror(compResult, &reg, ebuf, sizeof(ebuf));
		JNILOGE("regcomp error %s", ebuf);
		return -1;
	}

	// Handler the folder
	int count = 0;
	regmatch_t pmatch[1];
	struct dirent* dirp;
	while (NULL != (dirp = readdir(fd)))
	{
		// Check the right file
		char * name = dirp->d_name;
		if (DT_REG != dirp->d_type || regexec(&reg, name, 1, pmatch, 0) != 0)
		{
			JNILOGI("File [%s] is not vaild", name);
			continue;
		}

		JNILOGI("File [%s] ", name);
		// Get time
		char nameTime[17];
		nameTime[16] = '\0';
		strncpy(nameTime, name, strlen(name) - 4);
		long long time = atoll(nameTime);
//		JNILOGI("time is %lld", time);

// Update time
		firstTime = (time < firstTime) ? time : firstTime;
		lastTime = (time > lastTime) ? time : lastTime;

		count++;
	}

	// Clean the reg
	regfree(&reg);

	return count;
}

void writeFile(const char* folderPath, const char* content)
{
	if (NULL == folderPath || strlen(folderPath) <= 0 || NULL == content || strlen(content) <= 0)
	{
		JNILOGE("Folder path or content is NULl !");
		return;
	} JNILOGI("folder path: %s", folderPath); JNILOGI("content: %s", content);

	// New folder
	DIR* fd = newFolder(folderPath);
	if (NULL == fd)
	{
		JNILOGE("New folder fail");
		return;
	}

	// Get log number , first and last log
	long long firstTime = MAX_TIME;
	long long lastTime = 0;
	int count = getLogFiles(fd, firstTime, lastTime);
	JNILOGI("firstTime is %lld ", firstTime); JNILOGI("lastTime is %lld", lastTime); JNILOGI("count is %d", count);

	// Close the folder
	if (NULL != fd)
	{
		closedir(fd);
	}

	// Get the last time file, check the size < 200K
	bool newUpdate = false;
	char lastFileFullName[200];
	if (lastTime != 0)
	{
		snprintf(lastFileFullName, sizeof(lastFileFullName), FILE_NAME_PATTERN, folderPath, lastTime);
		JNILOGI("Last time file is : %s", lastFileFullName);

		// Get file's size
		struct stat buf;
		stat(lastFileFullName, &buf);
		JNILOGI("file size: %lld", buf.st_size);
		if (buf.st_size < FILE_SIZE_MAX)
		{
			JNILOGI("newUpdate!!!");
			newUpdate = true;
		}
	}

	// File number exceed the maximum, delete the oldest file
	if (!newUpdate && count >= FILE_NUM_MAX && firstTime < MAX_TIME)
	{
		char firstFileFullName[200];
		snprintf(firstFileFullName, sizeof(firstFileFullName), FILE_NAME_PATTERN, folderPath, firstTime);
		deleteFile(firstFileFullName);
	}

	// Begin write
	char fileFullName[200];
	long long time = getCurrentTime();
	JNILOGI("current time: %lld", time);
	snprintf(fileFullName, sizeof(fileFullName), FILE_NAME_PATTERN, folderPath, time);
	if (newUpdate)
	{
		// Update the file name
		rename(lastFileFullName, fileFullName);
	}

	write(fileFullName, content);
}

extern "C"
{
JNIEXPORT jstring JNICALL Java_com_yzayqc_android_LogJNILib_stringFromJNI(JNIEnv * env, jobject obj);
JNIEXPORT void JNICALL Java_com_yzayqc_android_LogJNILib_writeFile(JNIEnv * env, jobject obj, jstring filePath, jstring content);
}
;

/**
 * For test
 */
JNIEXPORT jstring JNICALL Java_com_yzayqc_android_LogJNILib_stringFromJNI(JNIEnv* env, jobject thiz)
{
	return env->NewStringUTF("Hello from JNI !! ");
}

JNIEXPORT void JNICALL Java_com_yzayqc_android_LogJNILib_writeFile(JNIEnv * env, jobject obj, jstring filePath, jstring content)
{
	JNILOGI("[JNI] Call the writeFile");

	const char * cFilePath = env->GetStringUTFChars(filePath, JNI_FALSE);
	const char * cContent = env->GetStringUTFChars(content, JNI_FALSE);

	writeFile(cFilePath, cContent);

}

