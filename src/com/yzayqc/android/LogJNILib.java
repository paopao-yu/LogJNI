package com.yzayqc.android;

/**
 * Wrapper for native library
 * 
 * @author ning.chang
 *
 */
public class LogJNILib {

	
	static {
		System.loadLibrary("logJNI");
//		nativeInit();
	}

	public native static String stringFromJNI();

//	public native static void nativeInit();

//	public native static void nativeFree();
	
	public native static void writeFile(String filePath, String content);

//	public static void clearCash() {
//		nativeFree();
//	}
}
