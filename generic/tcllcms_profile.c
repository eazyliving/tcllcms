/***********************************************************************
* Little Colormanagement System - tcl-extension
* v0.20 (or just around that)
* 2004 Christian Bednarek
* ----------------------------------------------------------------------
* here you'll find the commands for handling profiles and
* getting information about them...
*
************************************************************************/

 
#include "tcllcms.h"

extern void ObjProfileDelete (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {
	cmsCloseProfile( (cmsHPROFILE) clientData );
}

extern cmsHPROFILE getProfileFromObj (Tcl_Interp *interp, char *cmdname) {
    Tcl_CmdInfo info;
	Tcl_GetCommandInfo(interp, cmdname, &info);
	return info.objClientData;
}



DEFINE_CMD_E(tcl_cmsOpenProfile)
{

    cmsHPROFILE profile;
    char access='r';
	char * profilename=NULL,buffer[20];
	char * options[]={"-whitepoint","-new",(char *)NULL};
	char * builtin_profiles[]={"*Lab","*LabD65","XYZ","sRGB","Null",(char *)NULL};
	
	cmsCIExyY whitepoint;
	int idx_obj,listc;
	BOOL whitepoint_used=FALSE;

	Tcl_Obj **list;

	if (objc<2 || objc>4) {
	  Tcl_WrongNumArgs(interp,objc,objv,"filename or one of *Lab, *LabD65, XYZ or sRGB ?-whitepoint whitepoint? ?-new?");
      return TCL_ERROR;
	}

	if (objc==4 || objc==3) {
		
		if (Tcl_GetIndexFromObj(interp,objv[2],options,"option",(int)NULL,&idx_obj)==TCL_ERROR) return TCL_ERROR;
		switch (idx_obj) {
			case 0:
				// whitepoint
				if (Tcl_ListObjGetElements(interp, objv[3], &listc, &list)==TCL_ERROR) return TCL_ERROR;
				if (listc!=3) {
					Tcl_SetResult(interp,"whitepoint must consist of 3 elements x,y and Y",TCL_STATIC);
					return TCL_ERROR;
				}
				whitepoint.x=atof(Tcl_GetString(list[0]));
				whitepoint.y=atof(Tcl_GetString(list[1]));
				whitepoint.Y=atof(Tcl_GetString(list[2]));
				whitepoint_used=TRUE;
				break;
			case 1:
				// -new
				access='w';
				break;
		}	
	}

	
	Tcl_GetIndexFromObj(interp,objv[1],builtin_profiles,"profile",(int)NULL,&idx_obj);
		
	switch (idx_obj) {

		case 0: if (whitepoint_used) profile=cmsCreateLabProfile(&whitepoint); else
					profile=cmsCreateLabProfile(NULL);
				break;
		case 1:
				cmsWhitePointFromTemp(6500, &whitepoint);           
				profile=cmsCreateLabProfile(&whitepoint);
				break;
		case 2: profile=cmsCreateXYZProfile();
				break;
		case 3: profile=cmsCreate_sRGBProfile();
				break;
			case 4: profile=cmsCreateNULLProfile();
				break;
		default:
				profile=cmsOpenProfileFromFile(Tcl_GetString(objv[1]),&access);

				if (profile==NULL) {
					Tcl_SetResult(interp,"profile not found",TCL_STATIC);
					return TCL_ERROR;
				}	
				
				break;
	}

	sprintf(buffer,"profile%d",lastprofilenum);
	lastprofilenum++;
    
	Tcl_CreateObjCommand(interp,(char *)&buffer,ObjProfile,(ClientData) profile,(Tcl_CmdDeleteProc *)&ObjProfileDelete);
    Tcl_SetResult(interp,(char *)&buffer,TCL_VOLATILE);
	
	return TCL_OK;
}


DEFINE_CMD_E(tcl_cmsSaveProfile)
{

	cmsHPROFILE profile;
	
	if (objc!=3) {
		Tcl_WrongNumArgs(interp,objc,objv,"profilehandle filename");
		return TCL_ERROR;
	}

	profile=getProfileFromObj(interp,Tcl_GetString(objv[1]));
	if (_cmsSaveProfile(profile,Tcl_GetString(objv[2])))
		return TCL_OK;
	else
		return TCL_ERROR;

}

DEFINE_CMD_E(tcl_cmsCreateGrayProfile)
{

	cmsHPROFILE profile;
	LPGAMMATABLE gamma;
	cmsCIExyY whitepoint;
	int listc;
	Tcl_Obj **list;
	char buffer[200];

	if (objc!=3) {
		Tcl_WrongNumArgs(interp,objc,objv,"whitepoint gamma");
		return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements(interp, objv[1], &listc, &list)==TCL_ERROR) return TCL_ERROR;
	if (listc!=3) {
		Tcl_SetResult(interp,"whitepoint must consist of 3 elements x,y and Y",TCL_STATIC);
		return TCL_ERROR;
	}
	whitepoint.x=atof(Tcl_GetString(list[0]));
	whitepoint.y=atof(Tcl_GetString(list[1]));
	whitepoint.Y=atof(Tcl_GetString(list[2]));
		
	gamma=(LPGAMMATABLE) getDataFromObj(interp,Tcl_GetString(objv[2]));
	profile=cmsCreateGrayProfile(&whitepoint,gamma);
	sprintf(buffer,"profile%d",lastprofilenum);
	lastprofilenum++;
    
	Tcl_CreateObjCommand(interp,(char *)&buffer,ObjProfile,(ClientData) profile,(Tcl_CmdDeleteProc *)&ObjProfileDelete);
    Tcl_SetResult(interp,(char *)&buffer,TCL_VOLATILE);
	
	return TCL_OK;
}

DEFINE_CMD_E(tcl_cmsTransform2DeviceLink) {

	cmsHTRANSFORM trans;
	cmsHPROFILE profile;
	int flags=0,idx;
	char buffer[200];
	char * flagnames[]={"-highresprecalc","-lowresprecalc",(char *)NULL};
	
	int command_flags[]={cmsFLAGS_HIGHRESPRECALC,cmsFLAGS_LOWRESPRECALC,(int)NULL};

	if (objc<2 || objc>3) { 
      Tcl_WrongNumArgs(interp,objc,objv,"transformhandle ?-highresprecalc? ?-lowresprecalc?");
      return TCL_ERROR;
	}
	
	if (objc==3) {
		if (Tcl_GetIndexFromObj(interp,objv[2],flagnames,"option",(int)NULL,&idx)==TCL_ERROR) return TCL_ERROR;
		flags=flags|command_flags[idx];
	}

	trans=(cmsHTRANSFORM) getTransFromObj(interp,Tcl_GetString(objv[1]));
	profile=cmsTransform2DeviceLink(trans,flags);
	
	sprintf(buffer,"profile%d",lastprofilenum);
	lastprofilenum++;
    
	Tcl_CreateObjCommand(interp,(char *)&buffer,ObjProfile,(ClientData) profile,(Tcl_CmdDeleteProc *)&ObjProfileDelete);
    Tcl_SetResult(interp,(char *)&buffer,TCL_VOLATILE);

	return TCL_OK;
}


/*
 icSigInputClass               
    icSigDisplayClass                  
    icSigOutputClass                   
    icSigLinkClass                      
    icSigAbstractClass                  
    icSigColorSpaceClass               
    icSigNamedColorClass               


  void cmsSetDeviceClass(cmsHPROFILE hProfile, icProfileClassSignature sig);
    Does set device class signature in profile header

void cmsSetColorSpace(cmsHPROFILE hProfile, icColorSpaceSignature sig);
    Does set colorspace signaure in profile header

void cmsSetPCS(cmsHPROFILE hProfile, icColorSpaceSignature pcs);
    Does set PCS signature in profile header

void cmsSetRenderingIntent(cmsHPROFILE hProfile, int RenderingIntent);
    Sets the rendering intent in profile header

void cmsSetProfileID(cmsHPROFILE hProfile, LPBYTE ProfileID);
    Sets the profile ID in profile header (ver 4 only)

BOOL cmsAddTag(cmsHPROFILE hProfile, icTagSignature sig, void* data);
    Adds a new tag to a profile. Supported tags are:

       icSigCharTargetTag               const char*
       icSigCopyrightTag                const char*
       icSigProfileDescriptionTag       const char*
       icSigDeviceMfgDescTag            const char*
       icSigDeviceModelDescTag          const char*
       icSigRedColorantTag              LPcmsCIEXYZ
       icSigGreenColorantTag            LPcmsCIEXYZ
       icSigBlueColorantTag             LPcmsCIEXYZ
       icSigMediaWhitePointTag          LPcmsCIEXYZ
       icSigMediaBlackPointTag          LPcmsCIEXYZ
       icSigRedTRCTag                   LPGAMMATABLE
       icSigGreenTRCTag                 LPGAMMATABLE
       icSigBlueTRCTag                  LPGAMMATABLE
       icSigGrayTRCTag                  LPGAMMATABLE
       icSigAToB0Tag                    LPLUT
       icSigAToB1Tag                    LPLUT
       icSigAToB2Tag                    LPLUT
       icSigBToA0Tag                    LPLUT
       icSigBToA1Tag                    LPLUT
       icSigBToA2Tag                    LPLUT
       icSigGamutTag                    LPLUT
       icSigPreview0Tag                 LPLUT
       icSigPreview1Tag                 LPLUT
       icSigPreview2Tag                 LPLUT
       icSigChromaticityTag             LPcmsCIExyYTRIPLE
       icSigNamedColor2Tag              LPcmsNAMEDCOLORLIST

*/