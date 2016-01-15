# log_jni
For android jni: write log in sdcard



In logJNI.cpp

FILE_NUM_MAX： the max number of file, if outnumber, delete the oldest one

FILE_SIZE_MAX: the max size before write



Functions:

stringFromJNI  // for test

writeFile(String filePath, String content);
