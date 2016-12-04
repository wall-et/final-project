package finalProject.version1;

import java.awt.*;
import javax.swing.*;
import intel.rssdk.*;
import org.opencv.core.*;

import java.awt.Component;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.image.BufferedImage;

class DrawFrame extends Component { 
    public BufferedImage image;
    int rectX = 0;
    int rectY = 0;
    int rectH = 0;
    int rectW = 0;

    public DrawFrame(int width, int height) { 
       image=new BufferedImage(width,height,BufferedImage.TYPE_INT_RGB);
    }
    
    public DrawFrame(int width, int height, int x,int y, int w, int h) { 
		this.rectW = w;
        this.rectH = h;
        this.rectX = x;
        this.rectY = y;
        image=new BufferedImage(width,height,BufferedImage.TYPE_INT_RGB);
	}

    public void paint(Graphics g) {
        ((Graphics2D)g).drawImage(image,0,0,null);
        g.setColor(Color.RED);
        ((Graphics2D)g).drawRect(rectX, rectY, rectW, rectH);
    }
    
    public void setPoints(int x,int y, int w, int h){
    	this.rectW = w;
        this.rectH = h;
        this.rectX = x;
        this.rectY = y;
	}
}

class Listener extends WindowAdapter {
    public boolean exit = false;
	@Override public void windowClosing(WindowEvent e) {
		exit=true;
	}
}

