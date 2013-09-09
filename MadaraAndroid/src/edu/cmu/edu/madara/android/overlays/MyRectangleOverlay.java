/**
 *	Copyright (c) 2013 Carnegie Mellon University.
 *	All Rights Reserved.
 *	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following acknowledgments and disclaimers.
 *	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *	3. The names “Carnegie Mellon University,” "SEI” and/or “Software Engineering Institute" shall not be used to endorse or promote products derived from this software without prior written permission. For written permission, please contact permission@sei.cmu.edu.
 *	4. Products derived from this software may not be called "SEI" nor may "SEI" appear in their names without prior written permission of permission@sei.cmu.edu.
 *	5. Redistributions of any form whatsoever must retain the following acknowledgment:
 *	This material is based upon work funded and supported by the Department of Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon University for the operation of the Software Engineering Institute, a federally funded research and development center.
 *	Any opinions, findings and conclusions or recommendations expressed in this material are those of the author(s) and do not necessarily reflect the views of the United States Department of Defense.
 *	NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING INSTITUTE MATERIAL IS FURNISHED ON AN “AS-IS” BASIS. CARNEGIE MELLON UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 *	This material has been approved for public release and unlimited distribution.
 *	DM-0000245
 *
 */

package edu.cmu.edu.madara.android.overlays;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import com.google.android.maps.GeoPoint;
import com.google.android.maps.MapView;
import com.google.android.maps.Overlay;
import com.google.android.maps.Projection;

public class MyRectangleOverlay extends Overlay
{
	
	public static final String CLASS_TAG = MyRectangleOverlay.class.getSimpleName();
	
	private GeoPoint topLeft;
	private GeoPoint bottomRight;
	private Paint outLinePaint;
	private Paint fillPaint;
	
	public MyRectangleOverlay( GeoPoint topleft, GeoPoint bottomright )
	{
		this.topLeft = topleft;
		this.bottomRight = bottomright;
		fillPaint = new Paint();
		fillPaint.setDither( true );
		fillPaint.setColor( Color.rgb( 0, 0, 255 ) );
		fillPaint.setAlpha( 50 );
		fillPaint.setStyle( Paint.Style.FILL );
		
		outLinePaint = new Paint();
		outLinePaint.setDither( true );
		outLinePaint.setColor( Color.rgb( 0, 0, 255 ) );
		outLinePaint.setAlpha( 100 );
		outLinePaint.setStyle( Paint.Style.STROKE );
		outLinePaint.setStrokeWidth( 5 );
	}
	
	public GeoPoint getTopLeft() 
	{
		return topLeft;
	}

	public void setTopLeft( GeoPoint topLeft ) 
	{
		this.topLeft = topLeft;
	}

	public GeoPoint getBottomRight() 
	{
		return bottomRight;
	}

	public void setBottomRight( GeoPoint bottomRight ) 
	{
		this.bottomRight = bottomRight;
	}
	
	public int[] latMinlatMaxlonMinlonMax()
	{
		int latMin = ( topLeft.getLatitudeE6() < bottomRight.getLatitudeE6() ) ? topLeft.getLatitudeE6() : bottomRight.getLatitudeE6();
		int latMax = ( topLeft.getLatitudeE6() > bottomRight.getLatitudeE6() ) ? topLeft.getLatitudeE6() : bottomRight.getLatitudeE6();
		int lonMin = ( topLeft.getLongitudeE6() < bottomRight.getLongitudeE6() ) ? topLeft.getLongitudeE6() : bottomRight.getLongitudeE6();
		int lonMax = ( topLeft.getLongitudeE6() > bottomRight.getLongitudeE6() ) ? topLeft.getLongitudeE6() : bottomRight.getLongitudeE6();
		return new int[]{ latMin, latMax, lonMin, lonMax };
	}

	@Override
	public void draw( Canvas canvas, MapView mapView, boolean shadow ) 
	{
		super.draw( canvas, mapView, shadow );
		Projection projection = mapView.getProjection();
		Point point1 = new Point();
		Point point2 = new Point();
		projection.toPixels( topLeft, point1 );
		projection.toPixels( bottomRight, point2 );
		int left = ( point1.x < point2.x ) ?  point1.x : point2.x;
		int right = ( point1.x > point2.x ) ?  point1.x : point2.x;
		int top = ( point1.y < point2.y ) ? point1.y : point2.y;
		int bottom = ( point1.y > point2.y ) ? point1.y : point2.y;
		canvas.drawRect( left, top, right, bottom, fillPaint );
		canvas.drawRect( left, top, right, bottom, outLinePaint );
	}
	
/*	@Override
	public boolean onTouchEvent( MotionEvent motionEvent, MapView mapView ) 
	{
		int touchX = (int) motionEvent.getX();
		int touchY = (int) motionEvent.getY();
		Projection projection = mapView.getProjection();
		Point point1 = new Point();
		Point point2 = new Point();
		projection.toPixels( topLeft, point1 );
		projection.toPixels( bottomRight, point2 );
		int left = ( point1.x < point2.x ) ?  point1.x : point2.x;
		int right = ( point1.x > point2.x ) ?  point1.x : point2.x;
		int top = ( point1.y < point2.y ) ? point1.y : point2.y;
		int bottom = ( point1.y > point2.y ) ? point1.y : point2.y;
		Rect rect = new Rect( left, top, right, bottom );
		return rect.contains( touchX, touchY );
	}*/
	
}
