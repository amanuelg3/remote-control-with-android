package cs.tongji.mad.fproj;

import java.net.DatagramSocket;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.concurrent.TimeUnit;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

public class SettingView extends Activity {

	public static final int SEARCH_END = 10;
	public static final int CONNECT_END = 11;
	public Handler handler;
	private Communicator communicator;
	
	private class SettingViewHandler extends Handler{
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case SEARCH_END:
				@SuppressWarnings("unchecked")
				Set<String> findResult = (Set<String>)msg.obj;
				List<String> deviceNames = new ArrayList<String>();
				deviceNames.addAll(findResult);
				TextView textViewHostCount =
					(TextView)SettingView.this.findViewById(R.id.textViewHostCount);
				if (deviceNames.size() == 0) {
					textViewHostCount.setText(R.string.no_host_found);
				} else {
					String hostCount = Integer.toString(deviceNames.size());
					hostCount += " " + SettingView.this.getText(R.string.host_found);
					textViewHostCount.setText(hostCount);
				}
				Button btnSearch = 
					(Button)SettingView.this.findViewById(R.id.buttonSearch);
				btnSearch.setText(R.string.search_button);
				btnSearch.setEnabled(true);
				ProgressBar progressBarSearch = 
					(ProgressBar)SettingView.this.findViewById(R.id.progressBarSearch);
				progressBarSearch.setVisibility(View.INVISIBLE);
				ListView listViewSearchResult = 
					(ListView)findViewById(R.id.listViewSearchResult);
				listViewSearchResult.setAdapter(new ArrayAdapter<String>(
						SettingView.this,android.R.layout.simple_expandable_list_item_1,
						deviceNames));
				//progressBar1.setIndeterminate(false);
				break;
			
			case CONNECT_END:
				String ErrorMessage;
				switch ((Integer)msg.obj) {
				case Communicator.ERROR_SUCCESS:
					Intent intent = new Intent();
					intent.setClass(SettingView.this, GontrolMain.class);
					startActivity(intent);
					SettingView.this.finish();
					return;
				case Communicator.ERROR_NO_RESPONSE:
					Log.v("Connect","No response");
					ErrorMessage = 
						SettingView.this.getText(R.string.no_response).toString();
					break;
				case Communicator.ERROR_HOSTNAME_UNKNOWN:
					ErrorMessage = 
						SettingView.this.getText(R.string.host_unknown).toString();
					break;
				default:
					ErrorMessage = 
						SettingView.this.getText(R.string.error_unknown).toString();
					break;
				}
				Toast.makeText(SettingView.this, ErrorMessage, Toast.LENGTH_LONG).show();
				break;
			}
			super.handleMessage(msg);
		}
	}
	
	private class SearchThread extends Thread {
		public void run() {
			DatagramSocket findSocket = SettingView.this.communicator.searchHost();
			try {
				TimeUnit.MILLISECONDS.sleep(5000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			//findThread.
			findSocket.close();
			
			Message msg = new Message();
			msg.what = SEARCH_END;
			msg.obj = SettingView.this.communicator.getSearchHostResult();
			handler.sendMessage(msg);
		}
	}
	
	private class ConnectThread extends Thread {
		private String hostname;
		private ProgressDialog prgrssDialog;
		public ConnectThread(String hostname, ProgressDialog prgrssDialog) {
			this.hostname = hostname;
			this.prgrssDialog = prgrssDialog;
		}
		public void run() {
			int result = communicator.connect(hostname);	
			prgrssDialog.dismiss();
			Message msg = new Message();
			msg.what = CONNECT_END;
			msg.obj = new Integer(result);
			Log.v("Connect","End Post");
			handler.sendMessage(msg);
		}
	}

	private class SearchClickListener implements OnClickListener {
		@Override
		public void onClick(View arg0) {
			SettingView.this.search();
		}
	}

	private class SearchResultItemClickListener implements OnItemClickListener {

		@Override
		public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
				long arg3) {
			TextView textViewItem = (TextView)arg1;
			SettingView.this.connect(textViewItem.getText().toString());
			Log.v("Item", textViewItem.getText().toString());
			
			// TODO Auto-generated method stub
			
		}
	}
	
	
	private class CustomizeHostConnectClickListener implements OnClickListener {
		@Override
		public void onClick(View arg0) {
			EditText editTextHostName = (EditText)
			SettingView.this.findViewById(R.id.editTextCustomizeHostName);
			String hostName = editTextHostName.getText().toString();
			if (hostName == null || hostName.equals("")) {
				Toast.makeText(SettingView.this, R.string.hostname_empty,
						Toast.LENGTH_SHORT).show();
			}
			else
			{
				Log.v("Host",hostName);
				InputMethodManager imm = (InputMethodManager)
				getSystemService(Context.INPUT_METHOD_SERVICE);
				imm.hideSoftInputFromWindow(editTextHostName.getWindowToken(), 0);
				SettingView.this.connect(hostName);
			}
		}
	}
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		this.setContentView(R.layout.setting);
		communicator = WifiComm.getInstance();
		handler = new SettingViewHandler();
		Button btnSearch = (Button)findViewById(R.id.buttonSearch);
		btnSearch.setOnClickListener(new SearchClickListener());
		Button btnCustomizeHostConnect = 
			(Button)findViewById(R.id.buttonCustomizeHostConnect);
		btnCustomizeHostConnect.setOnClickListener(
				new CustomizeHostConnectClickListener());
		ListView listViewSearchResult = 
			(ListView)findViewById(R.id.listViewSearchResult);
		listViewSearchResult.setOnItemClickListener(
				new SearchResultItemClickListener());
		search();
	}

	private void search() {
		Button btnSearch = (Button)findViewById(R.id.buttonSearch);
		btnSearch.setText(R.string.searching_button);
		btnSearch.setEnabled(false);
		ProgressBar progressBarSearch = 
			(ProgressBar)SettingView.this.findViewById(R.id.progressBarSearch);
		progressBarSearch.setVisibility(View.VISIBLE);
		new SearchThread().start();		
	}
	
	private void connect(String hostname) {
		ProgressDialog dialog=new ProgressDialog(this);
		dialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
		//dialog.setIcon(android.R.drawable.ic_dialog_info);
		dialog.setTitle(R.string.please_wait);
		dialog.setMessage(getText(R.string.connecting_host).toString());
		dialog.setCancelable(false);
		dialog.show();
		new ConnectThread(hostname,dialog).start();
	}
}
