package finalProject.version1;

import javax.swing.*;

public class CameraStream {
	public JFrame window;
	public DrawFrame content;
	//public Listener listener;
	int width;
	int height;
	
	public CameraStream(int width,int height,String windowTitle,Listener listener){
		this.width = width;
		this.height = height;
		this.content = new DrawFrame(this.width, this.height);
		this.window = new JFrame(windowTitle);
		//this.listener = new Listener();
		this.window.addWindowListener(listener);
		this.window.setSize(this.width, this.height); 
		this.window.add(this.content);
		this.window.setVisible(true);
	}

}
