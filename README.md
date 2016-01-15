# log_jni
For android jni: write log in sdcard

in logJNI.cpp
#define FILE_NUM_MAX 50
#define FILE_SIZE_MAX 1024*200 // 200KB

FILE_NUM_MAX： the max number of file, if outnumber, delete the oldest one
FILE_SIZE_MAX: the max size before write


functions:
stringFromJNI  // for test

writeFile(String filePath, String content);
