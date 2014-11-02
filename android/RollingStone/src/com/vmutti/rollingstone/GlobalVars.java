package com.vmutti.rollingstone;

import java.io.OutputStream;

import android.app.Application;



public class GlobalVars extends Application{
	  public OutputStream OS;
	  public boolean Robot = false;
	  
	  public void onCreate() {
	    super.onCreate();
	  }
}