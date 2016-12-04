package finalProject.version1;

import javax.swing.*;

import intel.rssdk.PXCMCapture;
import intel.rssdk.PXCMImage;
import intel.rssdk.pxcmStatus;

public class CameraStream {
	public JFrame window;
	public DrawFrame content;
	//public Listener listener;
	int width;
	int height;
	int streamType;//[color,depth,ir]=[0,1,2]
	
	public CameraStream(int width,int height,String windowTitle,Listener listener,int type){
		this.width = width;
		this.height = height;
		this.streamType = type;
		this.content = new DrawFrame(this.width, this.height);
		this.window = new JFrame(windowTitle);
		//this.listener = new Listener();
		this.window.addWindowListener(listener);
		this.window.setSize(this.width, this.height); 
		this.window.add(this.content);
		this.window.setVisible(true);
	}
	
	public void updateStreamImage(PXCMCapture.Sample sample){
		PXCMImage.ImageData Data = new PXCMImage.ImageData();
		pxcmStatus sts = null;
		if(this.streamType == 0){
			sts = sample.color.AcquireAccess(PXCMImage.Access.ACCESS_READ,PXCMImage.PixelFormat.PIXEL_FORMAT_RGB32, Data);
		}else{
			if(this.streamType == 1){
				sts = sample.depth.AcquireAccess(PXCMImage.Access.ACCESS_READ,PXCMImage.PixelFormat.PIXEL_FORMAT_RGB32, Data);
			}else{
				System.out.println ("IR stream is currently disabled.");
	     	    System.exit(3);
			}
		}
	    if (sts.compareTo(pxcmStatus.PXCM_STATUS_NO_ERROR) < 0)
		{
 	        System.out.println ("Failed to AcquireAccess of image data");
     	    System.exit(3);
         }
      
         int Buff[] = new int[Data.pitches[0]/4 * this.height];
         
         Data.ToIntArray(0, Buff);
         this.content.image.setRGB (0, 0, this.width, this.height, Buff, 0, Data.pitches[0]/4);
         this.content.repaint();
         if(this.streamType == 0){
        	 sts = sample.color.ReleaseAccess(Data);
         }else{
        	 if(this.streamType == 1){
        		 sts = sample.depth.ReleaseAccess(Data);
        	 }else{
        		 System.out.println ("IR stream is currently disabled.");
        		 System.exit(3);
        	 }
         }
	     //sts = sample.color.ReleaseAccess(Data);
			
	     if (sts.compareTo(pxcmStatus.PXCM_STATUS_NO_ERROR)<0)
	     {
	    	 System.out.println ("Failed to ReleaseAccess of image data");
	    	 System.exit(3);
	     }
	}
	
	/*public void updateDepthStream(PXCMCapture.Sample sample){
		PXCMImage.ImageData dData = new PXCMImage.ImageData();
		pxcmStatus sts = sample.depth.AcquireAccess(PXCMImage.Access.ACCESS_READ,PXCMImage.PixelFormat.PIXEL_FORMAT_RGB32, dData);
         if (sts.compareTo(pxcmStatus.PXCM_STATUS_NO_ERROR)<0)
			{
             System.out.println ("Failed to AcquireAccess of depth image data");
             System.exit(3);
         }

         int dBuff[] = new int[dData.pitches[0]/4 * depthHeight];
         dData.ToIntArray(0, dBuff);
         depthStream.content.image.setRGB (0, 0, depthWidth, depthHeight, dBuff, 0, dData.pitches[0]/4);
         depthStream.content.repaint();
         sts = sample.depth.ReleaseAccess(dData);
         if (sts.compareTo(pxcmStatus.PXCM_STATUS_NO_ERROR)<0)
         {
             System.out.println ("Failed to ReleaseAccess of depth image data");
             System.exit(3);
         }
	}*/

}
