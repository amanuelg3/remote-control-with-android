package cs.tongji.mad.fproj;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import android.util.Log;

public class WifiComm implements Communicator {
	
	public int searchHostPort = 55555;
	public int connectPort = 44444;
	public static final String FIND_MESSAGE = "BEACONxBEACON\0";
	
	private static WifiComm instance = new WifiComm();
	private Map<String,InetAddress> availDevice;
	private Socket sendSocket;
	private Thread sendThread;
	private BlockingQueue<ControlDatas> sendQueue;
	
	
	private class SearchHostThread extends Thread {
		private DatagramSocket findSocket;
		private static final int RECV_BUFFER_SIZE = 1024;
		private static final int RECV_TIMEOUT = 500;
		public SearchHostThread() throws SocketException {
			findSocket = new DatagramSocket();
		}
		public void run() {
			try {
				findSocket.setBroadcast(true);		
				InetAddress bcastAddress;
				byte [] bcastIp = new byte [] {(byte) 0xFF,(byte) 0xFF,(byte) 0xFF,(byte) 0xFF};
				bcastAddress = InetAddress.getByAddress(bcastIp);
				//bcastAddress = InetAddress.getByName("10.0.2.2");
				byte [] bcastBuffer = FIND_MESSAGE.getBytes("US-ASCII");
				DatagramPacket bcastPacket = new DatagramPacket 
				(bcastBuffer, bcastBuffer.length, bcastAddress, searchHostPort);
				byte [] recvBuffer = new byte [RECV_BUFFER_SIZE];
				DatagramPacket recvPacket = new DatagramPacket(recvBuffer,recvBuffer.length);
				findSocket.setSoTimeout(RECV_TIMEOUT);
				Log.v("BeaconInit","Y");
				while (true) {
					findSocket.send(bcastPacket);
					try {
						findSocket.receive(recvPacket);
					} catch (IOException e) {
						if (!e.getClass().equals(java.net.SocketTimeoutException.class)) {
							findSocket.close();
							return;
						} else {
							continue;
						}
					}
					byte [] recvedData = recvPacket.getData();
					String recvedString = new String(recvedData,0,FIND_MESSAGE.length());
					if (recvedString.equals(FIND_MESSAGE)) {
						Log.v("Beacon", "S");
						recvedString = new String(recvedData,FIND_MESSAGE.length(),
								recvPacket.getLength() - FIND_MESSAGE.length() - 1);
						availDevice.put(recvedString, recvPacket.getAddress());
					}
				}
			} catch (SocketException e1) {
				e1.printStackTrace();
			}
			catch (UnknownHostException e) {
				e.printStackTrace();
			} catch (UnsupportedEncodingException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
	
	private class SendThread extends Thread {
		public void run() {
			while (true) {
				ControlDatas controlDatas;
				try {
					controlDatas = sendQueue.take();
				} catch (InterruptedException e) {
					e.printStackTrace();
					return;
				}
				
				try {
					PrintWriter out = new PrintWriter(sendSocket.getOutputStream());
					controlDatas.generateXML(out);
					out.append((char) 0);
					out.flush();
				} catch (IllegalArgumentException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IllegalStateException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}
	
	public static WifiComm getInstance() {
		return instance;
	}
	
	private WifiComm() {
		availDevice = new HashMap<String,InetAddress>();
		sendSocket = null;
	}
	
	public Set<String> getSearchHostResult() {
		return availDevice.keySet();
	}
	
	public DatagramSocket searchHost() {
		SearchHostThread ft;
		availDevice.clear();
		try {
			ft = new SearchHostThread();
		} catch (SocketException e) {
			e.printStackTrace();
			return null;
		}
		ft.start();
		return ft.findSocket;
	}
	public int getFindPort() {
		return searchHostPort;
	}

	public void setSearchHostPort(int searchHostPort) {
		this.searchHostPort = searchHostPort;
	}

	public int getConnectPort() {
		return connectPort;
	}

	public void setConnectPort(int connectPort) {
		this.connectPort = connectPort;
	}
	
	public int connect(String hostname) {
		InetAddress inetAddress = availDevice.get(hostname);
		if (inetAddress == null) {
			try {
				inetAddress = InetAddress.getByName(hostname);
			} catch (UnknownHostException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return ERROR_HOSTNAME_UNKNOWN;
			}
		}
		InetSocketAddress cnctAddress = new InetSocketAddress(inetAddress, connectPort);
		try {
			sendSocket = new Socket();
			sendSocket.connect(cnctAddress,10000);
			sendSocket.setTcpNoDelay(true);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			if (e.getClass().equals(java.net.SocketTimeoutException.class)) {
				return ERROR_NO_RESPONSE;				
			}
			return ERROR_UNKNOWN;
		}
		InitSend();
		return ERROR_SUCCESS;
	}
	
	public void disconnect() {
		sendThread.interrupt();
		try {
			sendSocket.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		sendSocket = null;
		sendQueue = null;
		Log.v("SOCKET", "disconnect");
	}
	
	public void send(ControlDatas datas) {
		try {
			sendQueue.put(datas);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
	
	private void InitSend() {
		sendQueue = new LinkedBlockingQueue<ControlDatas>();
		sendThread = new SendThread();
		sendThread.start();
	}
}
