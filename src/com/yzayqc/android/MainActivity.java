package com.yzayqc.android;

import java.io.File;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main_layout);

		// Test
		TextView textView = (TextView) findViewById(R.id.text);
		textView.setText(LogJNILib.stringFromJNI());

		// New file folder
		String path = "";
		String state = Environment.getExternalStorageState();
		if (Environment.MEDIA_MOUNTED.equals(state)) {
			path = Environment.getExternalStorageDirectory().getAbsolutePath();
			path += "/.logjni";

			File destDir = new File(path);
			if (!destDir.exists()) {
				destDir.mkdirs();
			}

		} else {
			Log.e("logJNI", "Error, not mounted");
		}

		LogJNILib.writeFile(path, "hello， this is log this is log this is log this is log this is log");
	}

}
