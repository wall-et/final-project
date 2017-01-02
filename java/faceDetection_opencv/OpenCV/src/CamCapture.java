  import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
//import org.opencv.highgui.Highgui;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.videoio.VideoCapture;
import org.opencv.imgproc.Imgproc;
import org.opencv.objdetect.CascadeClassifier;

public class CamCapture {

    /**
     * @param args
     */ 

    public static void main(String[] args) {

        //load opencv native library
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);

        String face_cascade_name = "haarcascade_frontalface_alt.xml";
        String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
        CascadeClassifier face_cascade = new CascadeClassifier();
        CascadeClassifier eyes_cascade = new CascadeClassifier();
        String window_name = "Capture - Face detection.jpg";

        System.out.println("capture through camera "+Core.VERSION);


        //load the face xml cascade
        if(!face_cascade.load(face_cascade_name))
        {
            System.out.println("Error loading face cascade");
        }
        else
        {
            System.out.println("Success loading face cascade");
        }

        //load the eyes xml cascade
        if(!eyes_cascade.load(eyes_cascade_name))
        {
            System.out.println("Error loading eyes cascade");
        }
        else
        {
            System.out.println("Success loading eyes cascade");
        }

        //detect default camera
        VideoCapture capture = new VideoCapture(0);

        if(!capture.isOpened())
        {
            System.out.println("Did not connected to camera.");
        }
        else
        {
            System.out.println("Conected to camera: "+capture.toString());
        }

        //create new Mat image
        Mat frame = new Mat();
        capture.retrieve(frame);

        Mat frame_gray = new Mat();
        Imgproc.cvtColor(frame, frame_gray, Imgproc.COLOR_BGRA2GRAY);
        Imgproc.equalizeHist(frame_gray, frame_gray);


        MatOfRect faces = new MatOfRect();

        face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0, new Size(30,30), new Size() );


        Rect[] facesArray = faces.toArray();

        for(int i=0; i<facesArray.length; i++)
        {
        	System.out.println("found face no: "+(i+1));
        	
            Point center = new Point(facesArray[i].x + facesArray[i].width * 0.5, facesArray[i].y + facesArray[i].height * 0.5);
            Imgproc.ellipse(frame, center, new Size(facesArray[i].width * 0.5, facesArray[i].height * 0.5), 0, 0, 360, new Scalar(255, 0, 255), 4, 8, 0);

             Mat faceROI = frame_gray.submat(facesArray[i]);
             MatOfRect eyes = new MatOfRect();

             //-- In each face, detect eyes
             eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0,new Size(30,30), new Size());            

             Rect[] eyesArray = eyes.toArray();
             System.out.println("eyes length " + eyesArray.length);

             
             for (int j = 0; j < eyesArray.length; j++)
             {
            	System.out.println("found eye no: "+(j+1));
                Point center1 = new Point(facesArray[i].x + eyesArray[i].x + eyesArray[i].width * 0.5, facesArray[i].y + eyesArray[i].y + eyesArray[i].height * 0.5);
                int radius = (int) Math.round((eyesArray[i].width + eyesArray[i].height) * 0.25);
                Imgproc.circle(frame, center1, radius, new Scalar(255, 0, 0), 4, 8, 0);
             }
           /*/for (int j = 0; j < eyesArray.length; j++)
             //{
            	//System.out.println("found eye no: "+(j+1));
                Point center1 = new Point(facesArray[i].x + eyesArray[0].x + eyesArray[0].width * 0.5, facesArray[0].y + eyesArray[0].y + eyesArray[0].height * 0.5);
                int radius1 = (int) Math.round((eyesArray[0].width + eyesArray[0].height) * 0.25);
                Imgproc.circle(frame, center1, radius1, new Scalar(255, 0, 0), 4, 8, 0);
                
                //System.out.println("found eye no: "+(j+1));
                Point center2 = new Point(facesArray[i].x + eyesArray[1].x + eyesArray[1].width * 0.5, facesArray[0].y + eyesArray[1].y + eyesArray[1].height * 0.5);
                int radius2 = (int) Math.round((eyesArray[1].width + eyesArray[1].height) * 0.25);
                Imgproc.circle(frame, center2, radius2, new Scalar(255, 255, 0), 4, 8, 0);
             //}*/
        }

        Imgcodecs.imwrite(window_name, frame);
        capture.release();




    }

}