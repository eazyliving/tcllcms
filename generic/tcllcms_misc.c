/***********************************************************************
* Little Colormanagement System - tcl-extension
* v0.20 (or just around that)
* 2004 Christian Bednarek
* ----------------------------------------------------------------------
* misc functions
* 
************************************************************************/

#include "tcllcms.h"



DEFINE_CMD_E(tcl_cmsErrorAction) {

	int action,idx_obj;
	char * actions[]={"abort","show","ignore",(char *)NULL};
	int action_flags[]={LCMS_ERROR_ABORT,LCMS_ERROR_SHOW,LCMS_ERROR_IGNORE,(int) NULL};

	if (objc!=2) {
	  Tcl_WrongNumArgs(interp,objc,objv,"abort|show|ignore");
      return TCL_ERROR;
	}	

	
	if (Tcl_GetIndexFromObj(interp,objv[1],actions,"option",(int)NULL,&idx_obj)==TCL_ERROR) return TCL_ERROR;
	
	action=action_flags[idx_obj];
	
	cmsErrorAction(action);
   	return TCL_OK;
}

DEFINE_CMD_E(tcl_cmsGetErrorHandler) {

	if (tcl_EHandler.script!=NULL) {
		Tcl_SetResult(interp,tcl_EHandler.script,TCL_VOLATILE);
		return TCL_OK;
	} else {
		Tcl_SetResult(interp,"no handler specified yet",TCL_STATIC);
		return TCL_ERROR;
	}
}

DEFINE_CMD_E(tcl_cmsSetErrorHandler) {

	char *script;

	if (objc!=2) {
	  Tcl_SetResult(interp,"wrong # args: should be tcl_cmsSetErrorHandler script", TCL_STATIC);
      return TCL_ERROR;
	}	
	
	if (tcl_EHandler.script!=NULL) {
		free(tcl_EHandler.script);
	}

	script=malloc(strlen(Tcl_GetString(objv[1]))+2);
	strncpy(script,Tcl_GetString(objv[1]),strlen(Tcl_GetString(objv[1])));
	script[strlen(Tcl_GetString(objv[1]))]=0;

	tcl_EHandler.script=script;
	tcl_EHandler.interp=interp;
	
	cmsSetErrorHandler(tcl_ErrorHandler);
   	
	return TCL_OK;
	
}


static int tcl_ErrorHandler (int ErrorCode, const char *ErrorText) {

	char *script;
	int length;
	
	if (tcl_EHandler.script==NULL) {
		// Oops. You shouldn't be here!
		return 0;
	}
	
	length=strlen(tcl_EHandler.script)+strlen(ErrorText)+32;
	script=malloc(length);
	sprintf(script,"%s {%s} {%d}",tcl_EHandler.script,ErrorText,ErrorCode);

	if (tcl_EHandler.interp!=NULL) {
		Tcl_Eval(tcl_EHandler.interp,script);
	} else {
		// Well... this is practically impossible... I hope ;)
		return 0;
	}

	free(script);
	return 1;

}

DEFINE_CMD_E(tcl_cmsWhitePointFromTemp) {

	BOOL result;
	cmsCIExyY xyY;
	char buffer[100];

	if (objc!=2) {
	  Tcl_SetResult(interp,"wrong # args: should be cmsWhitePointFromTemp temp", TCL_STATIC);
      return TCL_ERROR;
	}	
	
	result=cmsWhitePointFromTemp(atoi(Tcl_GetString(objv[1])),&xyY);
	
	if (result==FALSE) {
		Tcl_SetResult(interp,"error calculcating whitepoint", TCL_STATIC);
	    return TCL_ERROR;
	}	
	
	sprintf(buffer,"%.6f %.6f %.6f",xyY.x,xyY.y,xyY.Y);
	Tcl_SetResult(interp,buffer,TCL_VOLATILE);
	
	return TCL_OK;



}
