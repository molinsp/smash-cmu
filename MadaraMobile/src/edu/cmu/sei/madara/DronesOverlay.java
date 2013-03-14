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
 */

package edu.cmu.sei.madara;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.util.Log;

import com.google.android.maps.GeoPoint;
import com.google.android.maps.MapView;
import com.google.android.maps.Overlay;
import edu.cmu.sei.madara.MadaraMobile.MadaraHandler;

public class DronesOverlay extends Overlay{
	
	private MadaraHandler handler;
	private Paint mPaint;
	
	public DronesOverlay(MadaraHandler handler){
		this.handler = handler;
		mPaint = new Paint();
		mPaint.setColor(Color.WHITE);
		mPaint.setStrokeWidth(5);
		mPaint.setAntiAlias(true);
		//mPaint.setAlpha(10);
		mPaint.setDither(true);
	}
	
	@Override
	public void draw(Canvas canvas, MapView mapView, boolean arg2) {
		
		for(String var: handler.getMap().keySet()){
			//Log.e("madara", "ondraw");
			if(var.contains("gps.loc")){
				//Log.e("madara", "suppose to draw");
				String[] split = handler.getMap().get(var).split(",");
				double dlat = Double.parseDouble(split[0])*10E5;
				double dlon = Double.parseDouble(split[1])*10E5;
				int lat = (int)dlat;
				int lon = (int)dlon;
				//Log.e("madara", "lat lon "+ lat +", " + lon);
				GeoPoint geoPoint = new GeoPoint(lat, lon);
				Point point = new Point();
				mapView.getProjection().toPixels(geoPoint, point);
				canvas.drawCircle(point.x, point.y, 10, mPaint);
			}
		}
		mapView.postInvalidate();
	}
}
