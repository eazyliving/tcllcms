/***********************************************************************
* Little Colormanagement System - tcl-extension
* v0.20 (or just around that)
* 2004 Christian Bednarek
* ----------------------------------------------------------------------
* This is gamma-table stuff
* 
************************************************************************/

#include "tcllcms.h"

extern void ObjGammaDelete (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {
   cmsFreeGamma((LPGAMMATABLE) clientData);
}



DEFINE_CMD_E(ObjGamma) {

	char *command_list[]={"delete",(char *)NULL};
	int idx_command;
	
	if (Tcl_GetIndexFromObj(interp,objv[1],command_list,"command",(int)NULL,&idx_command)!=TCL_OK) return TCL_ERROR;

	switch (idx_command) {
	case 0:
		
		Tcl_DeleteCommand(interp,Tcl_GetString(objv[0]));
		break;
    default:
		break;
	}

	return TCL_OK;
}

DEFINE_CMD_E(tcl_cmsBuildGamma) {

	LPGAMMATABLE table;
	char buffer[255];

	if (objc!=3) {
		Tcl_WrongNumArgs(interp,objc,objv,"size gamma");
		return TCL_ERROR;
	}
	
	table=cmsBuildGamma(atoi(Tcl_GetString(objv[1])),atof(Tcl_GetString(objv[2])));
	if (table==NULL) {
		Tcl_SetResult(interp,"failed to create gammatable!",TCL_STATIC);
		return TCL_ERROR;
	}
	
	sprintf(buffer,"gamma%d",lastgammanum);
	lastgammanum++;
	
	Tcl_CreateObjCommand(interp,(char *)&buffer,ObjGamma,table,(Tcl_CmdDeleteProc *)&ObjGammaDelete);
    Tcl_SetResult(interp,(char *)&buffer,TCL_VOLATILE); 
 
	return TCL_OK;
}

DEFINE_CMD_E(tcl_cmsBuildParametricGamma) {

	Tcl_Obj **list;
	int listc,type,size;
	LPGAMMATABLE table;
	char buffer[255];
	double params[7];

	if (objc!=4) {
		Tcl_WrongNumArgs(interp,objc,objv,"size type param-list");
		return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements(interp, objv[3], &listc, &list)==TCL_ERROR) return TCL_ERROR;
	
	getDoubleFromList(params,list,listc);
	type=atoi(Tcl_GetString(objv[2]));
	size=atoi(Tcl_GetString(objv[1]));

	switch (type) {

		case 1:
			if (listc!=1) {
				Tcl_SetResult(interp,"type 1 gamma needs gamma as value!",TCL_STATIC);
				return TCL_ERROR;
			}
			break;
		case 2:
			if (listc!=3) {
				Tcl_SetResult(interp,"type 2 gamma needs gamma, a and b as values!",TCL_STATIC);
				return TCL_ERROR;
			}
			break;
		case 3:
			if (listc!=4) {
				Tcl_SetResult(interp,"type 3 gamma needs gamma, a, b and c as values!",TCL_STATIC);
				return TCL_ERROR;
			}
			break;
		case 4:
			if (listc!=5) {
				Tcl_SetResult(interp,"type 4 gamma needs gamma, a, b, c and d as values!",TCL_STATIC);
				return TCL_ERROR;
			}
			break;
		case 5:
			if (listc!=7) {
				Tcl_SetResult(interp,"type 5 gamma needs gamma, a, b, c, d, e and f as values!",TCL_STATIC);
				return TCL_ERROR;
			}
			break;
		default:
			break;
	}
	
	table=cmsBuildParametricGamma(size,type,params);

	if (table==NULL) {
		Tcl_SetResult(interp,"failed to create gammatable!",TCL_STATIC);
		return TCL_ERROR;
	}
	
	sprintf(buffer,"gamma%d",lastgammanum);
	lastgammanum++;
	
	Tcl_CreateObjCommand(interp,(char *)&buffer,ObjGamma,table,(Tcl_CmdDeleteProc *)&ObjGammaDelete);
    Tcl_SetResult(interp,(char *)&buffer,TCL_VOLATILE); 
 
	return TCL_OK;	
}

DEFINE_CMD_E(tcl_cmsAllocGamma) {
	
	LPGAMMATABLE table;
	char buffer[255];

	if (objc!=2) {
		Tcl_WrongNumArgs(interp,objc,objv,"size");
		return TCL_ERROR;
	}
	
	table=cmsAllocGamma(atoi(Tcl_GetString(objv[1])));
	
	if (table==NULL) {
		Tcl_SetResult(interp,"failed to create gammatable!",TCL_STATIC);
		return TCL_ERROR;
	}
	
	sprintf(buffer,"gamma%d",lastgammanum);
	lastgammanum++;
	
	Tcl_CreateObjCommand(interp,(char *)&buffer,ObjGamma,table,(Tcl_CmdDeleteProc *)&ObjGammaDelete);
    Tcl_SetResult(interp,(char *)&buffer,TCL_VOLATILE); 
	
	return TCL_OK;	
}

DEFINE_CMD_E(tcl_cmsReverseGamma) {

	LPGAMMATABLE table,tablein;
	char buffer[255];

	if (objc!=3) {
		Tcl_WrongNumArgs(interp,objc,objv,"size gammatable");
		return TCL_ERROR;
	}
	tablein=(LPGAMMATABLE) getDataFromObj(interp,Tcl_GetString(objv[2]));
	table=cmsReverseGamma(atoi(Tcl_GetString(objv[1])),tablein);
	
	if (table==NULL) {
		Tcl_SetResult(interp,"failed to reverse gammatable!",TCL_STATIC);
		return TCL_ERROR;
	}
	
	sprintf(buffer,"gamma%d",lastgammanum);
	lastgammanum++;
	
	Tcl_CreateObjCommand(interp,(char *)&buffer,ObjGamma,table,(Tcl_CmdDeleteProc *)&ObjGammaDelete);
    Tcl_SetResult(interp,(char *)&buffer,TCL_VOLATILE); 
	
	return TCL_OK;		
}

DEFINE_CMD_E(tcl_cmsJoinGamma) {

	LPGAMMATABLE table,tablein,tableout;
	char buffer[255];

	if (objc!=3) {
		Tcl_WrongNumArgs(interp,objc,objv,"gammatable-in gammatable-out");
		return TCL_ERROR;
	}

	tablein=(LPGAMMATABLE) getDataFromObj(interp,Tcl_GetString(objv[1]));
	tableout=(LPGAMMATABLE) getDataFromObj(interp,Tcl_GetString(objv[2]));

	table=cmsJoinGamma(tablein,tableout);
	
	if (table==NULL) {
		Tcl_SetResult(interp,"failed to join gammatables!",TCL_STATIC);
		return TCL_ERROR;
	}
	
	sprintf(buffer,"gamma%d",lastgammanum);
	lastgammanum++;
	
	Tcl_CreateObjCommand(interp,(char *)&buffer,ObjGamma,table,(Tcl_CmdDeleteProc *)&ObjGammaDelete);
    Tcl_SetResult(interp,(char *)&buffer,TCL_VOLATILE); 
	
	return TCL_OK;	
}

DEFINE_CMD_E(tcl_cmsSmoothGamma) {

	LPGAMMATABLE table;
	double lambda;

	if (objc!=3) {
		Tcl_WrongNumArgs(interp,objc,objv,"gammatable lambda");
		return TCL_ERROR;
	}

	table=(LPGAMMATABLE) getDataFromObj(interp,Tcl_GetString(objv[1]));
	lambda=atof(Tcl_GetString(objv[2]));
	
	
	if (cmsSmoothGamma(table,lambda)==FALSE) {
		Tcl_SetResult(interp,"failed to smooth gammatable!",TCL_STATIC);
		return TCL_ERROR;
	}
	
	return TCL_OK;	
}


