package com.vmutti.rollingstone;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import org.json.JSONArray;
import org.json.JSONException;

import android.os.Bundle;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ListView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

import com.getpebble.android.kit.*;

public class MainActivity extends Activity {

   private Button On,Off,Visible,list;
   private BluetoothAdapter BA;
   private Set<BluetoothDevice>pairedDevices;
   private ListView lv;
   private ArrayList<BluetoothDevice> asdfList;
   private BluetoothSocket Socket;
   private InputStream IS;

   @Override
   protected void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
      setContentView(R.layout.activity_main);
      On = (Button)findViewById(R.id.button1);
      Off = (Button)findViewById(R.id.button2);
      Visible = (Button)findViewById(R.id.button3);
      list = (Button)findViewById(R.id.button4);

      lv = (ListView)findViewById(R.id.listView1);

      BA = BluetoothAdapter.getDefaultAdapter();
   }

   public void on(View view){
      if (!BA.isEnabled()) {
         Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
         startActivityForResult(turnOn, 0);
         Toast.makeText(getApplicationContext(),"Turned on" 
         ,Toast.LENGTH_LONG).show();
      }
      else{
         Toast.makeText(getApplicationContext(),"Already on",
         Toast.LENGTH_LONG).show();
         }
   }
   public void list(View view){
      pairedDevices = BA.getBondedDevices();
      final ArrayList asdfList = new ArrayList(pairedDevices);

      ArrayList<String> list = new ArrayList();
      for(BluetoothDevice bt : pairedDevices){
         list.add(bt.getName());
      }
      Toast.makeText(getApplicationContext(),"Showing Paired Devices",
      Toast.LENGTH_SHORT).show();
      final ArrayAdapter adapter = new ArrayAdapter
      (this,android.R.layout.simple_list_item_1, list);
      lv.setAdapter(adapter);
      lv.setOnItemClickListener(new OnItemClickListener() {

		@Override
          public void onItemClick(AdapterView<?> parent, View view,
             int position, long id) {
				Toast.makeText(getApplicationContext(), ( (BluetoothDevice) asdfList.get(position)).getName(), Toast.LENGTH_LONG).show();
		        UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"); //Standard SerialPortService ID
		        try {
					Socket = ((BluetoothDevice) asdfList.get(position)).createRfcommSocketToServiceRecord(uuid);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
		        try {
					Socket.connect();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
		        try {
		        	((GlobalVars) getApplication()).OS = Socket.getOutputStream();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
		        try {
					IS = Socket.getInputStream();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
		        final CheckBox checkBox = (CheckBox) findViewById(R.id.check_id);
		        if (checkBox.isChecked()) {
		        	((GlobalVars) getApplication()).Robot = true;
		        }
			 	Intent intent = new Intent(getBaseContext(), Controller.class);
			 	startActivity(intent);
			 
    	  }
      }); 
   }
   public void off(View view){
      BA.disable();
      Toast.makeText(getApplicationContext(),"Turned off" ,
      Toast.LENGTH_LONG).show();
   }
   public void visible(View view){
      Intent getVisible = new Intent(BluetoothAdapter.
      ACTION_REQUEST_DISCOVERABLE);
      startActivityForResult(getVisible, 0);

   }
   @Override
   public boolean onCreateOptionsMenu(Menu menu) {
      // Inflate the menu; this adds items to the action bar if it is present.
      getMenuInflater().inflate(R.menu.main, menu);
      return true;
   }
   
}

