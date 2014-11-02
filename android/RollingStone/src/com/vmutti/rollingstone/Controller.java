package com.vmutti.rollingstone;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.UUID;

import org.json.JSONException;
import org.json.JSONObject;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.PebbleKit.PebbleDataReceiver;
import com.getpebble.android.kit.util.PebbleDictionary;

import android.app.Activity;
import android.app.Fragment;
import android.content.Context;
import android.graphics.Point;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;

public class Controller extends Activity {
	private PebbleDataReceiver receiver;
	private UUID uuid = UUID.fromString("a9df8db9-74ee-4f77-b2fa-929b678deab2");
	private int width;
	private int height;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_controller);
		if (savedInstanceState == null) {
			getFragmentManager().beginTransaction()
					.add(R.id.container, new PlaceholderFragment()).commit();
		}
		Display display = getWindowManager().getDefaultDisplay();
		Point size = new Point();
		display.getSize(size);
		width = size.x;
		height = size.y;
	}

	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		receiver = new PebbleDataReceiver(uuid) {
			@Override
			public void receiveData(Context context, int transactionId,
					PebbleDictionary data) {
				PebbleKit.sendAckToPebble(getApplicationContext(),
						transactionId);
				if (!((GlobalVars) getApplication()).Robot) {
					JSONObject obj = new JSONObject();
					if (data.getInteger(0) != null) {
						try {
							obj.put("x", data.getInteger(0));
						} catch (JSONException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
					if (data.getInteger(1) != null) {
						try {
							obj.put("y", data.getInteger(1));
						} catch (JSONException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
					if (data.getInteger(2) != null) {
						try {
							obj.put("z", data.getInteger(2));
						} catch (JSONException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
					if (data.getInteger(3) != null) {
						try {
							obj.put("select", data.getInteger(3));
						} catch (JSONException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
					if (data.getInteger(4) != null) {
						try {
							obj.put("mod", data.getInteger(4));
						} catch (JSONException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
					if (data.getInteger(5) != null) {
						try {
							obj.put("action", data.getInteger(5));
						} catch (JSONException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
					if (data.getInteger(6) != null) {
						try {
							obj.put("extra", data.getInteger(6));
						} catch (JSONException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
					Log.i(getLocalClassName(), obj.toString());
					byte[] bytes = null;
					try {
						bytes = (obj.toString() + "\n").getBytes("utf-8");
					} catch (UnsupportedEncodingException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
					try {
						((GlobalVars) getApplication()).OS.write(bytes);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				} else {
					byte[] bytes = null;
					try {
						bytes = ("x" + String.valueOf(data.getInteger(0)) + "y" + String
								.valueOf(data.getInteger(1))).getBytes("utf-8");
					} catch (UnsupportedEncodingException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
					try {
						((GlobalVars) getApplication()).OS.write(bytes);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			}
		};
		PebbleKit
				.registerReceivedDataHandler(getApplicationContext(), receiver);

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {

		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.controller, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	/**
	 * A placeholder fragment containing a simple view.
	 */
	public static class PlaceholderFragment extends Fragment {

		public PlaceholderFragment() {
		}

		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container,
				Bundle savedInstanceState) {
			View rootView = inflater.inflate(R.layout.fragment_controller,
					container, false);
			return rootView;
		}
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		int x = -((int) event.getX()) * 2048 / width - 1024;
		int y = -((int) event.getY()) * 2048 / height - 1024;
		if (!((GlobalVars) getApplication()).Robot) {
			JSONObject obj = new JSONObject();
			try {
				obj.put("x", x);
			} catch (JSONException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			try {
				obj.put("y", y);
			} catch (JSONException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			try {
				obj.put("action", 0);
			} catch (JSONException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			byte[] bytes = null;
			try {
				bytes = (obj.toString() + "\n").getBytes("utf-8");
			} catch (UnsupportedEncodingException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			try {
				((GlobalVars) getApplication()).OS.write(bytes);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		} else {
			byte[] bytes = null;
			try {
				bytes = ("x" + String.valueOf(x) + "y" + String.valueOf(y))
						.getBytes("utf-8");
			} catch (UnsupportedEncodingException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			try {
				((GlobalVars) getApplication()).OS.write(bytes);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return false;
	}
}
