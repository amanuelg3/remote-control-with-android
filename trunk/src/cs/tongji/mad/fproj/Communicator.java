package cs.tongji.mad.fproj;

import java.net.DatagramSocket;
import java.util.Set;



public interface Communicator {
	public static final int MAX_FIND_TIME = 3000;
	public static final int ERROR_SUCCESS = 0;
	public static final int ERROR_HOSTNAME_UNKNOWN = 1;
	public static final int ERROR_NO_RESPONSE = 2;
	public static final int ERROR_UNKNOWN = -1;
	
	public DatagramSocket searchHost();
	public Set<String> getSearchHostResult();
	public int connect(String hostname);
	public void disconnect();  
	public void send(ControlDatas datas);
}
