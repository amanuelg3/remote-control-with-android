package cs.tongji.mad.fproj;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.Iterator;
import java.util.LinkedList;

import org.xmlpull.v1.XmlSerializer;

import android.util.Xml;


public class ControlDatas extends LinkedList<Object>{

	private static final long serialVersionUID = 3747062807359212820L;
	
	public static final String NAMESPACE = "http://android.sse.tongji.edu.cn/ControlData";
	void generateXML(PrintWriter output)
	throws IllegalArgumentException, IllegalStateException, IOException {
		
		XmlSerializer serializer = Xml.newSerializer();
		Iterator<Object> iterData = this.iterator();
		
		serializer.setOutput(output);
		serializer.startDocument("UTF-8", true);
		serializer.setPrefix("tns", NAMESPACE);
		serializer.startTag(NAMESPACE, "Controls");
		while (iterData.hasNext()) {
			serializer.startTag(NAMESPACE, "Data");
			ControlData data = (ControlData)iterData.next();
			data.generateXML(serializer, output);
			serializer.endTag(NAMESPACE, "Data");
		}
		serializer.endTag(NAMESPACE, "Controls");
		serializer.endDocument();
	}
}
