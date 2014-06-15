/***********************************************************************
* Little Colormanagement System - tcl-extension
* v0.20 (or just around that)
* 2004 Christian Bednarek
* ----------------------------------------------------------------------
* Here you'll find functions for conversion (Lab, XYZ, xyY) and
* DeltaE
************************************************************************/

#include "tcllcms.h"

extern void getLabFromList (LPcmsCIELab Lab,Tcl_Obj **list) {
	Lab->L=atof(Tcl_GetString(list[0]));
	Lab->a=atof(Tcl_GetString(list[1]));
	Lab->b=atof(Tcl_GetString(list[2]));
	return;
}

extern void getXYZFromList (LPcmsCIEXYZ XYZ,Tcl_Obj **list) {
	XYZ->X=atof(Tcl_GetString(list[0]));
	XYZ->Y=atof(Tcl_GetString(list[1]));
	XYZ->Z=atof(Tcl_GetString(list[2]));
	return;
}

extern void getLChFromList (LPcmsCIELCh LCh,Tcl_Obj **list) {
	LCh->L=atof(Tcl_GetString(list[0]));
	LCh->C=atof(Tcl_GetString(list[1]));
	LCh->h=atof(Tcl_GetString(list[2]));
	return;
}

extern void getxyYFromList (LPcmsCIExyY xyY,Tcl_Obj **list) {
	xyY->x=atof(Tcl_GetString(list[0]));
	xyY->y=atof(Tcl_GetString(list[1]));
	xyY->Y=atof(Tcl_GetString(list[2]));
	return;
}

extern void getValFromList (WORD input[],Tcl_Obj **list,int count,double norm) {
	
	int i;
    double value;

	for (i=0;i<count;i++) {
		value=atof(Tcl_GetString(list[i]));
		input[i]=(int) floor(value*norm+0.5);
	}
	return;
}

extern void getDoubleFromList (double input[],Tcl_Obj **list,int count) {
	
	int i;
    
	for (i=0;i<count;i++) {
		input[i]=atof(Tcl_GetString(list[i]));
	}
	return;
}



DEFINE_CMD_E(tcl_cmsDeltaE) {


	cmsCIELab Lab1;
    cmsCIELab Lab2;
	
	Tcl_Obj **list;

   	char * methods[]={"Lab","CIE2000","CIE94","BFD","CMC",(char *)NULL};

	double de;
    char buffer[20]; 
	int type,listc;	
	
	if (objc!=4) {	 
      Tcl_WrongNumArgs(interp,1,objv,"type Lab1 Lab2");
      return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements(interp, objv[2], &listc, &list)==TCL_ERROR) return TCL_ERROR;
	if (listc!=3) {
		Tcl_SetResult(interp,"Lab1 must contain 3 elements.",TCL_STATIC);
		return TCL_ERROR;
	}
	
    getLabFromList(&Lab1,list);
     
	
	if (Tcl_ListObjGetElements(interp, objv[3], &listc, &list)==TCL_ERROR) return TCL_ERROR;
	if (listc!=3) {
		Tcl_SetResult(interp,"Lab2 must contain 3 elements.",TCL_STATIC);
		return TCL_ERROR;
	}
    
	getLabFromList(&Lab2,list);
	
	if (Tcl_GetIndexFromObj(interp,objv[1],methods,"method",(int)NULL,&type)==TCL_ERROR) return TCL_ERROR;
	 
     switch (type) {
		case 0:
			de=cmsDeltaE(&Lab1,&Lab2);
			break;
		case 1:
			de=cmsCIE2000DeltaE(&Lab1,&Lab2, 1.,1.,1.);
			break;
		case 2:
			de=cmsCIE94DeltaE(&Lab1,&Lab2);
			break;
		case 3:
			de=cmsBFDdeltaE(&Lab1,&Lab2);
			break;
		case 4:
			de=cmsCMCdeltaE(&Lab1,&Lab2);
			break;
		default:
			Tcl_SetResult(interp,"unknown DeltaE-Method. Choose one of Lab, CIE2000, CIE94,BFD, CMC", TCL_STATIC);
			return TCL_ERROR;	
			break;
	 } 

	 sprintf(buffer,"%.2f",de);
     
     Tcl_SetResult(interp,buffer,TCL_VOLATILE);
	 return TCL_OK;

}

DEFINE_CMD_E(tcl_cmsConversion) {

	cmsCIELab Lab;
	cmsCIELCh LCh;
	cmsCIEXYZ XYZ,whitepoint;
	cmsCIExyY xyY;
	
	char buffer[255];
	Tcl_Obj **list;
	int listc,function_idx;
	BOOL whitepoint_used=FALSE;
	char *conv_functions[]={
		"Lab2XYZ",
		"XYZ2Lab",
		"Lab2LCh",
		"LCh2Lab",
		"XYZ2xyY",
		"xyY2XYZ",
		(char *)NULL
	};
	
	if (objc<2 || objc==4) {	 
      Tcl_WrongNumArgs(interp,1,objv,"function inputvalue ?-whitepoint whitepoint?");
      return TCL_ERROR;
	}	
	
	if (!stricmp("-whitepoint",Tcl_GetString(objv[3]))) {
		//whitepoint!
		if (Tcl_ListObjGetElements(interp, objv[4], &listc, &list)==TCL_ERROR) return TCL_ERROR;
	
		if (listc!=3) {
			Tcl_SetResult(interp,"whitepoint must contain 3 elements.",TCL_STATIC);
			return TCL_ERROR;
		}

		getXYZFromList(&whitepoint,list);
		whitepoint_used=TRUE;
	}
	
	if (Tcl_ListObjGetElements(interp, objv[2], &listc, &list)==TCL_ERROR) return TCL_ERROR;
	
	if (listc!=3) {
		Tcl_SetResult(interp,"list of values must contain 3 elements.",TCL_STATIC);
		return TCL_ERROR;
	}

	if (Tcl_GetIndexFromObj(interp,objv[1],conv_functions,"conversion function",(int)NULL,&function_idx)==TCL_ERROR) return TCL_ERROR;

	switch (function_idx) {

		case 0:
			 // Lab2XYZ
			 	getLabFromList(&Lab,list);
				if (whitepoint_used) cmsLab2XYZ(&whitepoint,&XYZ,&Lab); else cmsLab2XYZ(NULL,&XYZ,&Lab);
				sprintf(buffer,"%.6f %.6f %.6f",XYZ.X,XYZ.Y,XYZ.Z);
				break;
		case 1:
			 // XYZ2Lab 
				getXYZFromList(&XYZ,list);
				if (whitepoint_used) cmsXYZ2Lab(&whitepoint,&Lab,&XYZ); else cmsXYZ2Lab(NULL,&Lab,&XYZ);
						sprintf(buffer,"%.2f %.2f %.2f",Lab.L,Lab.a,Lab.b);
				break;
		case 2:
			 // Lab2LCh
			 	getLabFromList(&Lab,list);
				cmsLab2LCh(&LCh,&Lab);
				sprintf(buffer,"%.2f %.2f %.2f",LCh.L,LCh.C,LCh.h);
				break;
		case 3:
			// LCh2Lab
				getLChFromList(&LCh,list);
				cmsLCh2Lab(&Lab,&LCh);
				sprintf(buffer,"%.2f %.2f %.2f",Lab.L,Lab.a,Lab.b);
				break;
		case 4:
			// XYZ2xyY
				getXYZFromList(&XYZ,list);
				cmsXYZ2xyY(&xyY,&XYZ);
				sprintf(buffer,"%.6f %.6f %.6f",xyY.x,xyY.y,xyY.Y);
				break;
		case 5:
			// xyY2XYZ
				getxyYFromList(&xyY,list);
				cmsxyY2XYZ(&XYZ,&xyY);
				sprintf(buffer,"%.6f %.6f %.6f",XYZ.X,XYZ.Y,XYZ.Z);
				break;
		default:
			// whoops
				sprintf(buffer,"function what?");
				break;
	}
	Tcl_SetResult(interp,buffer,TCL_VOLATILE);
	return TCL_OK;
}


DEFINE_CMD_E(tcl_cmsClampLab) {
	
	double amax,amin,bmax,bmin;
	cmsCIELab Lab;
	Tcl_Obj **list;
	int listc;
	char buffer[200];

	if (objc!=6) {	 
      Tcl_WrongNumArgs(interp,objc,objv,"Lab amax amin bmax bmin");
      return TCL_ERROR;
	}
	
	if (Tcl_ListObjGetElements(interp, objv[1], &listc, &list)==TCL_ERROR) return TCL_ERROR;
	if (listc!=3) {
		Tcl_SetResult(interp,"Lab must contain 3 elements.",TCL_STATIC);
		return TCL_ERROR;
	}
	
	getLabFromList(&Lab,list);
	
	amax=atof(Tcl_GetString(objv[2]));
	amin=atof(Tcl_GetString(objv[3]));
	bmax=atof(Tcl_GetString(objv[4]));
	bmin=atof(Tcl_GetString(objv[5]));
	
	cmsClampLab(&Lab,amax,amin,bmax,bmin);
	
	sprintf(buffer,"%0.2f %0.2f %0.2f",Lab.L,Lab.a,Lab.b);
	Tcl_SetResult(interp,buffer,TCL_VOLATILE);

	return TCL_OK;

	

}
